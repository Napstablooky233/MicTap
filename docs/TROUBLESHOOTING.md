# Troubleshooting

## The audio sounds muffled or flat

This one is worth measuring rather than guessing, because the cable path is usually innocent.
The 48 kHz path was measured end to end with three probe tones (100 Hz / 1 kHz / 10 kHz) played by
a 48 kHz source, captured through process loopback, mixed and written to the cable:

```text
100 Hz  -0.6 dB
1000 Hz -0.9 dB
10000 Hz +0.0 dB   <- flat, no treble loss anywhere in MicTap
```

So if music sounds dull, look at the two ends:

- **The voice app is filtering it.** Discord (Krisp noise suppression, echo cancellation, automatic
  gain), KOOK, TeamSpeak and every game voice chat treat a music signal as noise. Turn all three off
  for the microphone you selected, per app. This is by far the most common cause.
- **The system audio enhancements are on.** Sound Control Panel → CABLE Input → Properties →
  Enhancements: disable *Loudness Equalization* and anything else listed. Loudness Equalization is a
  compressor; it flattens music into mush.
- **The source itself is already dull.** Measured with the same probe tones, a 44.1 kHz file played
  through the legacy WinMM player lost 3 dB at 10 kHz *before* MicTap ever saw it. Music players that
  use WASAPI shared mode with a 48 kHz endpoint do not have that problem.
- **Volume is turned down on the cable endpoint.** CABLE Input below 100 % costs level, not treble,
  but downstream AGC in the voice app then boosts the noise floor. Keep it at 100 % and use MicTap's
  own volume control.

## Nothing happens after clicking Start

- Both lanes need a render target. If the *Detect devices* status line says the cable is missing,
  VB-CABLE is not installed or not visible to this session.
- If the lane state shows an error message, open **Open logs** and read the newest file: the engine
  reports the first HRESULT that failed.

## The status says the target is gone

The process you selected exited (or was restarted with a new PID). Click **Refresh list**, pick it
again. MicTap verifies a target by executable path and creation time, so a restarted app is never
silently re-attached.

## The app list is suspiciously short

Process visibility depends on where the executable lives. Running `MicTap.exe` from a sandboxed,
roaming or otherwise restricted directory can reduce the enumerable process set to a dozen entries.
Copy it to `%LOCALAPPDATA%\Programs\MicTap\` and start it from there. Same binary, full list.

## A lane runs, `peak` stays at 0

`peak` proves audio actually flows. If it stays at 0:

- the target app is paused, or muted in the Windows volume mixer;
- the app renders through an exclusive-mode or DRM-protected path (protected audio is intentionally
  not capturable);
- the selected PID belongs to a launcher that hands playback to a child that is *not* in its tree.

Verify from the command line before touching the UI:

```text
MicTap.exe --route <PID> 5
type logs\route-test.txt
```

## The microphone lane is silent

- Windows microphone privacy setting blocks the app (Settings → Privacy → Microphone).
- The device is disconnected or held in exclusive mode by another program.
- The lane volume is at 0 %.

## Choppy, crackling, or drifting audio

- Set the CABLE control panel to 48 kHz; MicTap's engine runs at 48 kHz stereo float.
- Keep both lanes in shared mode; exclusive mode removes the mixer Windows needs.
- Very small buffer sizes on the receiving side (some voice apps) can stutter with heavy CPU load.

## Echo or doubled audio

- The voice app is monitoring its own input, or another virtual cable tool is injecting audio into
  the same endpoint.
- Do not enable the physical microphone lane *and* select the physical microphone in the voice app
  at the same time.

## Build failures

- `g++ not found` — install w64devkit and either put its `bin` on `PATH` or set `MINGW_BIN`.
- `windres not found` — same cause; `windres` ships with the toolchain.
- Undefined `AUDIOCLIENT_ACTIVATION_PARAMS` — make sure `src/process_loopback_params.h` is included
  and that the include path points at `src`.
- Linker errors about `mmdevapi` / `ole32` — the build command must pass
  `-lole32 -luuid -loleaut32 -lmmdevapi -lshell32 -lgdi32 -luser32`.
