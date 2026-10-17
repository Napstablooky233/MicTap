# Changelog

All notable changes to this project are documented here. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and this project adheres to
[Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.2.1] — 2026-10-17

Audio path work, driven by a "the sound is muffled" report. The cable path turned out to be flat
(it was measured, see `docs/TROUBLESHOOTING.md`), so the changes below remove the conversions that
were still in the chain and stop the mixer from touching the signal before it is audible.

### Changed

- Streams are opened **without** `AUTOCONVERTPCM` when the endpoint already runs at
  48 kHz / stereo / float32 — the format the mixer works in. Nothing is resampled, nothing is
  quantised a second time. Endpoints with another format still get the converter, because they need
  it.
- Output headroom raised from 0.85 to 0.95 (−1.4 dB → −0.45 dB), so the mix is no longer quietly
  attenuated before it reaches the cable.
- The clipper is now a soft knee: the last 3 % of full scale is compressed with `tanh`, everything
  below passes untouched. Hard clipping at full scale was audible as grit on loud mixes.
- Default application and microphone volume is 100 % instead of 60 %; the old default was quiet
  enough that downstream voice-chat AGC would lift the noise floor.

### Verified

Probe tones at 100 Hz / 1 kHz / 10 kHz, 48 kHz source, measured by capturing the cable output:
`-0.6 dB / -0.9 dB / +0.0 dB` — flat. A 44.1 kHz source played through the legacy WinMM player
loses 3 dB at 10 kHz before MicTap sees it at all, which is why the same test used to look bad.

## [0.2.0] — 2026-10-17

Multiple applications can be captured at once. This is the first release where the app-audio lane is
a list rather than a single choice.

### Added

- **Multi-app mixing.** Up to eight applications can be ticked in the source list and are summed into
  the same output, so a music player, a browser and a game can share one virtual microphone.
- The tick boxes are remembered across restarts in `settings.ini` (`[ui] picked=`). Each entry is keyed
  by executable path plus process creation time, so a recycled PID never re-binds to a different
  program.
- `--route` accepts a comma-separated PID list (`--route 37932,37964 6`) and reports one
  `laneN_frames / laneN_peak / laneN_open` triple per captured app.
- `--dump` appends the stable selection key to every row, which makes tick persistence diagnosable.

### Changed

- The source control is now a multi-select list with native check boxes instead of a drop-down.
- A lane that loses its target only reports `Stale target` for that lane; the other lanes keep running.
  The whole lane stops only when every target is gone.
- The window grew from 880×640 to 880×752 to fit the list, and the whole layout moved 116 px down.

### Fixed

- Rebuilding the list fired one `LVN_ITEMCHANGED` per inserted row, which overwrote the stored
  selection mid-rebuild and dropped every tick. Notifications are now ignored while the list is
  repopulated.

## [0.1.1] — 2026-10-17

Process list cleanup; no behavioural change to the audio path.

### Changed

- Processes that share the same executable image are folded into a single row, annotated `×N`
  (`Microsoft Edge ×7`, `腾讯QQ ×9`, `网易云音乐 ×3`). Browsers, Electron apps and chat clients all
  run a parent plus helper processes, and listing each of them was noise. The row captures the root
  process, and process-tree loopback already follows the children.
- The row's representative process is chosen in this order: a tree root over a child, a process that
  is actually rendering audio over one that is idle, then the oldest process — so a freshly spawned
  helper never replaces the real application.
- Long window titles are clipped to 34 characters with an ellipsis instead of being cut mid-word.

### Fixed

- A process whose only window belonged to a helper (for example a browser renderer) no longer shows
  up as a separate entry from the browser itself.

## [0.1.0] — 2026-10-17

First public release.

### Added

- Per-process audio capture through the Windows Application Loopback API
  (`ActivateAudioInterfaceAsync` + `VIRTUAL_AUDIO_DEVICE_PROCESS_LOOPBACK`), scoped to a PID and its
  process tree, so multi-process applications (browsers, Electron players, music clients) are
  captured completely.
- Physical microphone capture alongside the application lane, mixed on a single engine clock and
  written to a virtual audio cable. Each lane starts and stops independently with its own 0–200 %
  gain.
- Single-file Win32 application: custom-drawn UI, no .NET runtime, no PowerShell, no background
  service.
- Bilingual interface (Chinese / English) with per-monitor V2 DPI handling and a language preference
  persisted to `settings.ini`.
- Process discovery that merges three signals — audio sessions, visible windows, and a built-in
  preset table of common media/browser executables — so an application is selectable even when it is
  not currently rendering.
- Target verification by executable path and process creation time before capture starts, to avoid
  attaching to a process that reused the PID.
- Synchronous target liveness check plus a process handle watch in the engine: a dead or exited
  target reports an error instead of streaming silence.
- Soft limiter with gain headroom, plus a 50 ms prefetch high-water mark and a 150 ms overload drop
  threshold to bound latency and prevent drift.
- Command-line self-tests: `--dump` (discovery report) and `--route` (end-to-end routing report).
- `build.cmd` release/debug build script and a Windows CI workflow.

### Known limitations

- Windows 10 build 20348 or newer is required.
- Exclusive-mode and DRM-protected streams may capture as silence.
- Captured audio is the application's post-mix output, not a raw track.

[Unreleased]: https://github.com/Napstablooky233/MicTap/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/Napstablooky233/MicTap/releases/tag/v0.1.0
