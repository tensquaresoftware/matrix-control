---
title: 'Diagnostic logging SUPPORT tab and MC_ build/log flags'
type: 'feature'
created: '2026-11-18'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '800f5de43a047aec8bc196e1ad2a03dc77781f02'
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Before v1, MIDI/APVTS logging must stay quiet in Release by default, while a user who hits a bug still needs an opt-in diagnostic journal. CMake build toggles still use the half-name `MATRIX_` prefix.

**Approach:** Add Settings tab SUPPORT (after MASTER) with DIAGNOSTIC LOGGING OFF/ON (default OFF, persisted) and an OPEN LOG FOLDER button that reveals the Application Support logs directory. When ON, write MIDI + APVTS files there. Add `MC_DEV_MIDI_LOG` / `MC_DEV_APVTS_LOG` (Debug ON / Release OFF) for lab behaviour without stripping Release file I/O. Rename `MATRIX_BUILD_TESTS` / `MATRIX_BUILD_SKIN_DESIGNER` to `MC_BUILD_*` with aliases.

**Decisions:** HOW_USER_FINDS_LOGS = C — OPEN LOG FOLDER button on SUPPORT (opens/reveals the Application Support logs parent folder).

## Boundaries & Constraints

**Always:**
- SUPPORT after MASTER (plugin + standalone); ASCII UI strings only.
- Diagnostic OFF by default; persist on `apvts.state` like CONTEXTUAL HELP (not an audio parameter).
- ON → both loggers write under Application Support `Logs/MIDI` and `Logs/APVTS`; console off in Release; message thread only.
- OPEN LOG FOLDER always available on SUPPORT; creates the logs parent folder if missing, then reveals it in the OS file manager.
- OFF in Release → no files; no Info console spam from MidiLogger.
- `MC_DEV_*` = lab auto-start/verbose defaults only — must not disable Release `setLogToFile` (diagnostic needs it).
- Canonical `MC_BUILD_TESTS` / `MC_BUILD_SKIN_DESIGNER`; legacy `MATRIX_BUILD_*` aliases; update CI, CONTRIBUTING, presets, packaging scripts/tests.
- No `getProjectRoot()` walk from the processor constructor; diagnostic uses an explicit Application Support path.

**Never:** Separate MIDI/APVTS Settings toggles; rotation UI / privacy modal / crash service; renumber existing tab ids (append `kSupport = 9`); retarget Debug repo `Logs/` for diagnostic; leave CI docs on `MATRIX_BUILD_*` only.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Fresh Release | Diagnostic unset | OFF; no log files | Seed default OFF |
| Set ON | Combo ON | Files under App Support; support-useful level | File open fail → no crash; file stays off |
| Set OFF | Combo OFF | Close files; quiet | N/A |
| Debug + OFF | Dev logging rules | Existing repo/dev Logs unchanged | N/A |
| Debug + ON | Both paths | Dev Logs may run; diagnostic also writes App Support | N/A |
| CMake `MC_BUILD_TESTS` or legacy `MATRIX_BUILD_TESTS` | Configure | Tests target available | N/A |
| OPEN LOG FOLDER | Button click | Creates App Support `Logs` if needed; reveals folder in OS | If reveal fails: no crash; optional footer later |

</frozen-after-approval>

## Code Map

- `PluginIDs.h` / `PluginDisplayNames.h` / `SettingsShellMetrics.h` / `SettingsPanel*` — SUPPORT tab + DIAGNOSTIC LOGGING combo + OPEN LOG FOLDER
- `PluginProcessorConstruction.cpp` / editor Settings wire-restore — seed, persist, apply on/off; button → reveal App Support logs dir
- `PluginProcessorInternal.h` / `PluginProcessorValueTree.cpp` — Debug lab gate kept; Release diagnostic runtime flag; ApvtsLogger sites follow diagnostic OR development
- `MidiLogger.*` / `ApvtsLogger.*` / `ProjectPaths*` — quiet Release OFF; explicit App Support files when ON; `MC_DEV_*` without killing file I/O; shared helper for diagnostic logs root
- `CMakeLists.txt`, presets, CI, CONTRIBUTING, `check_skin_designer_packaging.py`, release workflow tests — `MC_BUILD_*` + aliases + `MC_DEV_*`
- `SettingsTabsShellTests.cpp` — SUPPORT after MASTER

## Tasks & Acceptance

**Execution:**
- [x] CMake + CI/docs/scripts — `MC_BUILD_*` aliases + `MC_DEV_*` without stripping Release file logging
- [x] Settings IDs/names/panel — SUPPORT under MASTER + DIAGNOSTIC LOGGING OFF/ON + OPEN LOG FOLDER
- [x] Processor + editor — persist/apply App Support file logging; reveal logs folder on button
- [x] Loggers + ValueTree gates — Release quiet when OFF; both loggers when ON; Debug lab preserved
- [x] Unit tests + Release Standalone smoke — tab order; OFF quiet; ON files; folder reveal; OFF closes

**Acceptance Criteria:**
- Given Release + Diagnostic OFF, when running, then no log files and no Info MIDI console spam.
- Given Diagnostic ON, when MIDI/APVTS activity happens, then files appear under Application Support `Logs/MIDI` and `Logs/APVTS`.
- Given Settings rail, when open, then SUPPORT is directly under MASTER.
- Given OPEN LOG FOLDER, when clicked, then the Application Support logs folder is revealed in the OS file manager.
- Given `-DMC_BUILD_TESTS=ON` or legacy `-DMATRIX_BUILD_TESTS=ON`, when configuring, then tests build.
- Given Debug + default `MC_DEV_*` ON, when development logging applies, then existing repo Logs behaviour still works.

## Implementation Notes

- Added `MC_BUILD_TESTS` / `MC_BUILD_SKIN_DESIGNER` with legacy `MATRIX_BUILD_*` aliases; `MC_DEV_MIDI_LOG` / `MC_DEV_APVTS_LOG` via Debug/Release generator expressions.
- SUPPORT tab (`kSupport=9`) after MASTER; DIAGNOSTIC LOGGING OFF/ON + OPEN LOG FOLDER; files under Application Support `Logs/MIDI` and `Logs/APVTS`.
- Extracted `PluginProcessorDiagnosticLogging.cpp` and `PluginEditorSettingsDiagnostic.cpp` for the quality gate.
- Verified: `Matrix-Control_Tests` categories SettingsTabsShell + DiagnosticLoggingSettings (0 failures); `lint_touched.py` OK vs baseline.
- Manual Release Standalone smoke (OFF quiet / ON files / folder reveal) still recommended before ship.

## Spec Change Log

## Review Triage Log


- high — Blind/Edge: Debug lab `setLogToFile(true)` retargeted diagnostic App Support files back to repo Logs — **patched**: `enableFileLoggingForSession` / `enableApvtsLogging` no-op while diagnostic is on; `releaseResources` skips lab teardown while diagnostic is on.
- high — Blind: `isUsingFallbackRoot()` walked project root on diagnostic enable with explicit path — **patched**: skip fallback walk when `filePath` is non-empty (MidiLogger + ApvtsLogger).
- medium — Blind/Edge: re-enable while already ON opened/truncated same-second stamp — **patched**: early-return when `diagnosticLoggingEnabled_ == enabled`.
- medium — Blind: file open failure left preference ON with console off — **patched**: sync flag from `isLoggingToFile()` and force property OFF on failure.
- medium — Edge: destructor left singleton logging after destroy — **patched**: disable diagnostic in `~PluginProcessor`.
- high — Verification/Blind: `test_skin_designer_packaging_wiring.py` still required `MATRIX_BUILD_SKIN_DESIGNER` — **patched**: renamed expectations to `MC_BUILD_*` (pytest green).
- medium — Verification: no test exercised diagnostic file create/close — **patched**: `DiagnosticLoggingSettingsTests` opens explicit App Support probe file via MidiLogger; full `PluginProcessor` on/off still deferred (processor not in test binary).
- medium — CMake legacy alias one-way cache — **patched**: alias FORCE into cache when legacy ON.
- medium — Edge/Blind: multi-instance singleton logger contention — **deferred** (pre-existing singleton design; refcount out of scope for v1).
- low — Edge: MIDI-thread file I/O when logging — **deferred** (pre-existing MidiLogger call sites; not introduced by SUPPORT toggle alone).
- maybe-false/medium — Edge: diagnostic flag atomic race — **deferred** (unverified everyday harm; message-thread Settings is primary writer).
- medium — Verification: Debug `MC_DEV_*` stripped on plugin target without suite noticing — **deferred** (suite does not launch plugin process).
- low — Blind: SUPPORT combo not refreshed if Settings stays open across host restore — **deferred** (Settings typically closed; reopen restores from state).
