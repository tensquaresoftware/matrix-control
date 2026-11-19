---
title: 'Unify all file logs under Application Support'
type: 'refactor'
created: '2026-11-19'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '0222e6b38b1993b649265d58ea0d4bb6a31be57f'
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiate">

## Intent

**Problem:** Debug lab logging still defaults to the repo `Logs/` folder while SUPPORT diagnostic logging uses Application Support, so Guillaume must remember two places and the dual-path design feels pointless after SUPPORT landed.

**Approach:** Make Application Support the only file-log destination. File logs open only when SUPPORT is ON (Debug and Release). Debug may keep console lab verbosity via `MC_DEV_*`; it must not auto-open log files when SUPPORT is OFF. Stop writing under the project root.

**Decisions:** DEBUG_LAB_WHEN_SUPPORT_OFF = SUPPORT_ONLY_FILES — Debug does not auto-write files; file logs only when SUPPORT ON (same as Release). Console lab behaviour may remain.

## Boundaries & Constraints

**Always:**
- Default logger directories (`getLogsDirectory` / empty `setLogToFile(true)`) resolve under Application Support, same parent as `getDiagnosticLogsRoot()`.
- File logging starts only via SUPPORT ON (both Debug and Release); Debug lab must not call `setLogToFile(true)` when SUPPORT is OFF.
- OPEN LOG FOLDER and SUPPORT diagnostic paths stay Application Support.
- Update contributor-facing docs that still point at repo `Logs/`.
- Unit coverage that default and diagnostic log dirs share the Application Support root (or are identical).

**Never:**
- Change SUPPORT UI copy, tab order, or persistence keys.
- Reintroduce repo-root file logging for MIDI/APVTS.
- Auto-open Debug lab log files when SUPPORT is OFF.
- Delete user Application Support logs as part of this change.
- Broad ProjectPaths refactor beyond log path unification (repo-root discovery for other features stays).

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Debug + SUPPORT OFF | Session start | No log files; console lab OK if `MC_DEV_*` | N/A |
| Debug + SUPPORT ON | Diagnostic enable | Files under Application Support `Logs/MIDI` / `Logs/APVTS` | Open fail → preference OFF |
| Release + SUPPORT OFF | Default | No file logs | N/A |
| Release + SUPPORT ON | Diagnostic enable | Same Application Support tree | Open fail → preference OFF |
| SUPPORT ON → OFF | Toggle off | Close files; Debug does not reopen repo/lab files | N/A |
| OPEN LOG FOLDER | Button | Reveals Application Support `Logs` | No crash if reveal fails |
| Old repo `Logs/` | Leftover local folder | No new writes from plugin | Manual delete OK; keep gitignore |

</frozen-after-approval>

## Code Map

- `Source/Shared/ProjectPaths.cpp` / `.h` — `getLogsDirectory` today uses `getProjectRoot()/Logs/…`; retarget to Application Support (reuse `getDiagnosticLogsRoot` / `getDiagnosticLogsDirectory` or make them share one implementation). Do not break non-log ProjectPaths APIs.
- `Source/Core/Loggers/MidiLogger.cpp` / `ApvtsLogger.cpp` — `getDefaultLogDirectory()` calls `getLogsDirectory`; comments in `.h` still say `<project>/Logs/…`.
- `Source/Core/PluginProcessorValueTree.cpp` — `ensureDevelopmentLoggingStarted` / `enableFileLoggingForSession` / `enableApvtsLogging` currently open files in Debug; stop file open when SUPPORT_ONLY; keep console level if desired. `setDiagnosticLoggingEnabled(false)` must not re-call lab `setLogToFile(true)`.
- `Source/Core/PluginProcessorDiagnosticLogging.cpp` — already Application Support; on OFF today re-enables lab files via `enableFileLoggingForSession` / `enableApvtsLogging` — that re-open must stop under SUPPORT_ONLY_FILES.
- `Source/Core/PluginProcessorInternal.h` — `shouldUseDevelopment*Logging` gates; may still drive console, not files.
- `Tests/Unit/DiagnosticLoggingSettingsTests.cpp` — asserts diagnostic dirs; extend that default dirs are under Application Support.
- `Tests/Unit/ProjectPathsTests.cpp` — add focused log-path coverage if lighter than Diagnostic tests.
- `CONTRIBUTING.md` — still says attach logs from project `Logs/`.
- `_bmad-output/project-context.md` — AD-10 still documents `{root}/Logs/…` for loggers; update for consistency.
- Do **not** change Settings SUPPORT panel wiring unless a path comment is wrong.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Shared/ProjectPaths.*` -- Point `getLogsDirectory` at Application Support (shared with diagnostic root) -- single destination SSOT
- [x] `Source/Core/Loggers/MidiLogger.*` / `ApvtsLogger.*` -- Fix header comments; keep default dir via ProjectPaths -- no stale project-root messaging
- [x] `PluginProcessorValueTree.cpp` / `PluginProcessorDiagnosticLogging.cpp` -- Stop Debug lab auto `setLogToFile(true)`; on SUPPORT OFF do not reopen files; console lab may remain -- SUPPORT_ONLY_FILES
- [x] `Tests/Unit/*` -- Assert default and diagnostic log dirs under Application Support; cover that SUPPORT OFF path does not imply lab file reopen if testable -- lock unification
- [x] `CONTRIBUTING.md` + `_bmad-output/project-context.md` log path note -- Point at Application Support / OPEN LOG FOLDER -- stop sending people to repo `Logs/`

**Acceptance Criteria:**
- Given Debug + SUPPORT OFF, when a session runs, then no new MIDI/APVTS log files are created (repo or Application Support).
- Given SUPPORT ON (Debug or Release), when logging runs, then files appear under Application Support `Logs/MIDI` or `Logs/APVTS`, never under the repo root.
- Given SUPPORT ON → OFF in Debug, when diagnostic closes, then lab code does not reopen file logging.
- Given OPEN LOG FOLDER, when clicked, then the Application Support `Logs` folder is revealed.
- Given CONTRIBUTING guidance, when a contributor looks up where to attach logs, then they are directed to Application Support (or OPEN LOG FOLDER), not repo `Logs/`.

## Implementation Notes

- `getLogsDirectory` now delegates to `getDiagnosticLogsDirectory` (Application Support only).
- Debug lab `enableFileLoggingForSession` / `enableApvtsLogging` set console + level only; no `setLogToFile(true)`. SUPPORT OFF restore therefore cannot reopen files.
- Removed repo-root fallback warning from MidiLogger/ApvtsLogger `setLogToFile` (logs no longer depend on project root).
- Verified: DiagnosticLoggingSettings category 0 failures; `lint_touched.py` OK.
- Review patches: console helpers in `PluginProcessorInternal`; removed dead `getFallbackRootWarning`; empty-path tests for MIDI+APVTS with same-second-safe picker; docs wording softened.
- Post-review patches (unify + polish): removed dead `writeLogRaw` / Midi `getMessageColumnIndentWidth`; lab-session enable bodies shared via Internal (tested); empty-path always deletes probe; RECEIVED one-line + APVTS/MIDI cartouche blank-line asserts.

## Spec Change Log

## Review Triage Log

- medium — Blind: comment claims explicit paths always resolve under Application Support — **false**: explicit `setLogToFile(true, path)` still accepts any path; only empty/default uses `getLogsDirectory`. Comment overstated — **patched** to say default empty path uses Application Support.
- medium — Blind: ApvtsLogger empty-path open not covered — **patched**: parallel empty-path assert for ApvtsLogger (or shared helper covering both).
- medium — Blind/Verification: `developmentConsoleRestoreDoesNotOpenFiles` does not call production helpers — **patched**: extract console-only helpers in `PluginProcessorInternal` used by `enableFileLoggingForSession` / `enableApvtsLogging`; test calls those helpers after files were open.
- low — Blind: CONTRIBUTING omits company folder segment — **patched**: clearer Application Support path wording.
- medium — Blind/Edge: `getFallbackRootWarning` now unused — **patched**: remove declaration/definition.
- low — Blind: `enableFileLoggingForSession` name still says “file” — **deferred**: rename is broader API polish; console-only comments + Internal helpers mitigate.
- low — Blind: project-context AD-10 overstates enforcement — **patched**: soften to product path (SUPPORT ON) without claiming all call sites are gated.
- low — Blind: done sibling `spec-diagnostic-logging-support.md` still describes repo Logs — **deferred**: historical done artifact; this unify spec is the new SSOT.
- low — Blind: dual `getLogsDirectory` / `getDiagnosticLogsDirectory` names — **patched**: header note that they share Application Support.
- medium — Edge: empty-path test same-second stamp can make `after.size() > before` fail — **patched**: select newly created file without relying on strict size growth alone (time/new-file helper).

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` then run ProjectPaths / DiagnosticLoggingSettings (or full suite) -- expected: pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++

**Manual checks (if no CLI):**
- Debug Standalone + SUPPORT OFF: no new files under Application Support or repo `Logs/`.
- SUPPORT ON → files only under `~/Library/Application Support/Ten Square Software/Matrix-Control/Logs/`; OPEN LOG FOLDER reveals it; OFF closes without reopening lab files.

### Review Findings

- [x] [Review][Patch] Remove dead MidiLogger helpers left by SysEx one-line logging (`writeLogRaw`, `getMessageColumnIndentWidth`) [Source/Core/Loggers/MidiLogger.h:76]
- [x] [Review][Patch] Empty-path tests must always delete the probe file they opened [Tests/Unit/DiagnosticLoggingSettingsTests.cpp:151]
- [x] [Review][Patch] SUPPORT OFF restore test must call production `enableFileLoggingForSession` / `enableApvtsLogging`, not only Internal console helpers [Tests/Unit/DiagnosticLoggingSettingsTests.cpp:183]
- [x] [Review][Patch] Assert short RECEIVED SysEx stays header+hex on one line (and honors description) [Tests/Unit/DiagnosticLoggingSettingsTests.cpp:316]
- [x] [Review][Patch] Lock session-header order for APVTS and assert the blank line between cartouche and banner on MIDI [Tests/Unit/DiagnosticLoggingSettingsTests.cpp:280]
- [x] [Review][Defer] CONTRIBUTING only documents the macOS Application Support path — deferred: Win/Linux equivalents are nice-to-have; OPEN LOG FOLDER remains the primary guidance
- [x] [Review][Defer] PluginProcessorInternal pulls full Midi/Apvts logger headers for two inline helpers — deferred: acceptable coupling for small helpers; move to .cpp only if include cost becomes a problem
- [x] [Review][Defer] `enableFileLoggingForSession` / `enableApvtsLogging` naming still says “file” — deferred: already tracked in deferred-work.md; rename remains out of this review’s patch set
- [x] [Review][Defer] No long-SysEx regression for wrap-at-120 readability — deferred: smoke asked for short one-line; long-dump layout is follow-up polish

**Rejected**
- false — Explicit `setLogToFile(true, path)` can open outside Application Support: intentional API for tests/diagnostics; default empty path is the App Support SSOT.
- false — SysEx formatting + session-header cartouche “out of frozen unify Intent”: intentional polish commits in this branch review scope; not a defect.
- false — Done-spec Code Map still present-tense about repo `Logs/`: rejected because the fix would be editing the frozen/done spec under review; Implementation Notes already record the new behavior.
- low — Console-restore test does not assert console/level turned on: helpers are two-line inlines; file-off assert is the contract that matters.
- low — `wrapLogMessage` may break at `:` and push hex to the next line on long dumps: changing punctuation preference would add special-casing beyond a direct fix.
