# Architecture

MicTap is one Win32 process with two halves: a UI thread and a single audio engine thread. This
document explains the data path, the concurrency model, and why the capture code looks the way it
does.

## 1. Layered view

```
┌──────────────────────────── UI thread ────────────────────────────┐
│ window, owner-drawn controls, language switch, device/process list │
│ writes: wantProc / wantMic / gainProc / gainMic / cfg*  (atomic)   │
│ reads : stateProc / stateMic / captured / peak / queued  (atomic)  │
└───────────────────────────────┬───────────────────────────────────┘
                                │ shared state, no locks on the hot path
┌───────────────────────────────▼───────────────────────────────────┐
│                         engine thread                              │
│  open/close sources on demand → poll capture clients → sum frames  │
│  → soft limit → write to the render client of the virtual cable    │
└───────────────────────────────────────────────────────────────────┘
```

The UI thread never touches an audio interface, and the engine thread never touches a window.

## 2. Sources

`kMaxApps` (8) application lanes plus one microphone lane exist for the whole session:

| Source | Opened with | Flags |
| --- | --- | --- |
| application audio (per ticked app) | `ActivateAudioInterfaceAsync(VIRTUAL_AUDIO_DEVICE_PROCESS_LOOPBACK, …)` + `IAudioClient` | `AUDCLNT_STREAMFLAGS_LOOPBACK \| AUTOCONVERTPCM \| SRC_DEFAULT_QUALITY` |
| physical microphone | `IMMDeviceEnumerator::GetDevice` → `IMMDevice::Activate(IAudioClient)` | `AUTOCONVERTPCM \| SRC_DEFAULT_QUALITY` (no loopback flag) |

All of them produce the same internal format — 48 kHz, stereo, `float32` — so the mixer is a plain
per-frame addition over however many lanes are open.

The engine keeps a `std::vector<DWORD>` of wanted PIDs (copied under `cfgLock` once per iteration) and
compares it with the currently open set. A difference closes every lane and reopens the wanted ones;
this only happens on a start/stop, because the UI refuses to change the selection while the lane runs.
Each lane carries its own `HANDLE watch` from `OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION)`,
so a target that exits is detected with `WaitForSingleObject(watch, 0)` and that one lane reports
`Stale target` instead of the whole lane dying.

### 2.1 Why Application Loopback instead of the default loopback device

Classic WASAPI loopback captures the *render endpoint*, i.e. everything the machine plays. The
Application Loopback API activates an audio client against a **process tree**, so the capture scope
is exactly the app the user picked. The activation parameters are built by hand in
`src/process_loopback_params.h` (the MinGW headers in use do not ship them):

```cpp
AUDIOCLIENT_ACTIVATION_PARAMS params = {};
params.ActivationType = AUDIOCLIENT_ACTIVATION_TYPE_PROCESS_LOOPBACK;
params.ProcessLoopbackParams.TargetProcessId = pid;
params.ProcessLoopbackParams.ProcessLoopbackMode = PROCESS_LOOPBACK_MODE_INCLUDE_TARGET_PROCESS_TREE;
```

`INCLUDE_TARGET_PROCESS_TREE` is what makes browsers and Electron players behave: their audio is
rendered by a child process, not by the process the user selected.

### 2.2 Asynchronous activation, done carefully

`ActivateAudioInterfaceAsync` completes on an arbitrary thread. The completion handler implements
`IActivateAudioInterfaceCompletionHandler` and `IAgileObject`, and follows three rules learned the
hard way:

1. `GetActivateResult` returns two HRESULTs — its own, and the activation result. Merging them hides
   activation failures behind a successful call.
2. After a wait timeout the handler, the event and the operation object stay alive; releasing them
   while a callback is still pending is a use-after-free.
3. `QueryInterface` only answers for interfaces that are actually implemented.

## 3. Mixing and buffering

Each source owns a `std::deque<float>` of interleaved stereo frames.

- **Prefetch**: the write side is opened, and samples start flowing, only after the queue holds
  ~50 ms. This hides scheduler jitter without adding permanent latency.
- **Overload**: if the queue grows past ~150 ms the oldest frames are dropped, preferring latency
  over an ever-growing delay.
- **Sum and limit**: frames from both sources are added, scaled by the lane gain, and passed through
  a soft limiter that stays inside ±1.0 with headroom, so two loud sources do not clip into
  distortion.
- **On demand**: when neither lane has data the render client is not asked for buffers at all.

## 4. Process discovery

The source list is a union of three signals, because no single one is reliable:

1. **Audio sessions** (`IAudioSessionManager2`) — tells which processes are rendering *right now*.
   A session can exist while a player is paused, so this marks an entry as `●` rather than
   filtering on it.
2. **Visible windows** (`EnumWindows`) — catches apps with no audio session yet.
3. **Preset table** — known media players and browsers, matched by executable name, so a
   not-yet-playing app is still listed.

Rows are ranked: known and rendering → known → rendering → everything else. Selecting a row stores
the PID, the executable path and the process creation time; the capture is only started after those
match again.

## 5. Failure behaviour

- A source that fails to open sets the lane state to *error* and reports a message; it does not
  silently retry forever.
- A process handle (`SYNCHRONIZE`) is held for the application lane. When the target exits, the
  engine closes the source, stops the lane and reports that the target is gone.
- `--dump` and `--route` expose the same internals to the command line, which is how the data path
  is verified without a GUI.

## 6. Threads and shutdown

- `engineStop` is set first, then the engine thread is joined from `wWinMain` after the message loop
  returns. The window procedure only flips flags, so closing the window cannot deadlock against a
  live audio callback.
- Fonts and GDI objects created for the current DPI/language are retired through a queue and freed
  once no control references them.
