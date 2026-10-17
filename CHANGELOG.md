# Changelog

All notable changes to this project are documented here. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and this project adheres to
[Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

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
