---
title: 'Windows Standalone: enable ASIO audio device type (GH-42)'
type: 'bugfix'
created: '2026-10-30'
status: 'done'
route: 'dispatch'
baseline_commit: '2c077dca7ad191d2a95224996acdf71807177330'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-settings-audio-tab.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** On Windows Standalone, Settings → AUDIO never lists ASIO (only WASAPI / DirectSound), so interfaces like a Focusrite Scarlett cannot use their low-latency ASIO driver outside a DAW. Plugin-in-host is fine because the host owns ASIO.

**Approach:** Turn on JUCE’s Windows ASIO backend for Matrix-Control builds so Standalone’s AudioDeviceManager registers the ASIO device type. Reuse the existing Settings AUDIO UI (already links ASIO input/output). No new Settings rows or driver-list curation.

## Boundaries & Constraints

**Always:**
- Enable `JUCE_ASIO=1` on the shared `Matrix-Control` target for Windows only (module is shared; Standalone-only define is insufficient).
- Use JUCE’s bundled Steinberg ASIO headers (`JUCE_ASIO_USE_EXTERNAL_SDK` stays off).
- Keep existing ASIO UI behavior: both INPUT and OUTPUT device rows visible and linked.
- Leave WASAPI and DirectSound enabled (JUCE defaults).
- Treat exclusive-device conflict (Live already holding ASIO) as expected: ASIO type must still appear when free.

**Never:**
- Change macOS/Linux audio backends or hosted-plugin bus layout.
- Curate or hard-code DRIVER TYPE names (keep enumerate-from-JUCE).
- Rewrite Settings AUDIO apply/profile/preferred-setup paths beyond what enabling the type surfaces.
- Ship a separate ASIO SDK tree or require an `ASIOSDK` env var for the default path.
- Expand scope to user-manual prose in this change (defer if needed).

**Decisions:**
- Steinberg proprietary ASIO license path: keep PolyForm Noncommercial; enable `JUCE_ASIO` in source now for local/CI builds. Guillaume submitted the signed ASIO SDK Licensing Agreement (2026-10-30). Do not publish public GitHub/release binaries that include ASIO until Steinberg countersigns. alpha.4 (no ASIO) may remain online.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Windows Standalone Settings AUDIO | Build with ASIO enabled; open DRIVER TYPE | Combo includes an ASIO entry among JUCE types | N/A |
| Select ASIO + free Scarlett | Choose ASIO, pick Scarlett | Device opens; INPUT/OUTPUT lists stay linked | JUCE reports open failure if driver busy |
| ASIO exclusive conflict | Live already holds Scarlett ASIO | Type still listed; open may fail until Live releases | Surface existing JUCE/device error path |
| Non-Windows build | macOS/Linux configure | No `JUCE_ASIO=1` | N/A |
| Hosted VST3 | Open in Live | Unchanged host audio path; AUDIO tab absent | N/A |

</frozen-after-approval>

## Code Map

- `CMakeLists.txt` (~949–959) — add Windows-only `JUCE_ASIO=1` to `Matrix-Control` `target_compile_definitions` (PUBLIC with other `JUCE_*` flags). Do not put it only on `Matrix-Control_Standalone`.
- `C:\Users\Guillaume\Dev\SDKs\JUCE-9\modules\juce_audio_devices\juce_audio_devices.h` (JUCE_ASIO / JUCE_ASIO_USE_EXTERNAL_SDK) — defaults and license note; bundled headers under `native/asio/`.
- `Source/GUI/Settings/SettingsAudioPageDevices.cpp` — `refreshDriverTypeCombo()` enumerates `getAvailableDeviceTypes()`; no filter to change.
- `Source/GUI/Settings/AudioDeviceSetupSync.h` — reuse `isAsioDeviceType` / `linkAsioDeviceNames` / `resolveEndpointNamesForApply` (already tested).
- `Tests/Unit/SettingsTabsShellTests.cpp` — existing ASIO link helper tests; no live device-manager type assert required for this fix.
- `.github/workflows/build-and-test.yml` — builds `Matrix-Control` + Tests (not Standalone); flag still applies to shared code on Windows CI compile.
- `.github/workflows/release.yml` — builds Standalone; release binaries pick up ASIO once flag is on.
- Do not change: Settings AUDIO layout, Core preferred-setup / profiles / scene-safety, plugin AUDIO absence.

## Tasks & Acceptance

**Execution:**
- [x] `CMakeLists.txt` -- add Windows-gated `JUCE_ASIO=1` on `Matrix-Control` PUBLIC definitions -- register ASIO device type
- [x] Rebuild Windows Standalone (local preset) -- confirm configure/build succeeds with ASIO compiled in
- [x] Manual smoke on Windows Standalone -- DRIVER TYPE lists ASIO; optional open Scarlett when free; INPUT/OUTPUT stay linked
- [x] `Scripts/quality/lint_touched.py` on touched files -- project quality gate

**Acceptance Criteria:**
- Given a Windows Standalone build of this branch, when Settings → AUDIO → DRIVER TYPE is opened, then ASIO appears in the list.
- Given ASIO selected and the interface is free, when the user picks that ASIO device, then INPUT and OUTPUT stay linked as today.
- Given macOS or Linux configure, when building, then `JUCE_ASIO` is not forced on.
- Given hosted VST3 in a DAW, when using the plugin, then host audio wiring is unchanged.

## Implementation Notes

- `CMakeLists.txt`: `if(WIN32)` sets `JUCE_ASIO=1` PUBLIC on `Matrix-Control` and PRIVATE on `Matrix-Control_Tests` (tests link `juce_audio_devices` separately).
- Added `Tests/Unit/WindowsAsioDeviceTypeTests.cpp` — asserts `JUCE_ASIO` and ASIO type registration on Windows; off / no ASIO type on other platforms.
- Verified: `cmake --preset windows-debug` + build Standalone/Tests; `--category SettingsTabsShell` and `--category WindowsAsioDeviceType` pass; `lint_touched.py` OK.
- Optional human smoke: open Standalone → Settings → AUDIO → DRIVER TYPE (ASIO) and Scarlett when free; do not publish public ASIO binaries until Steinberg countersigns.
- Review patch: configure-time FATAL_ERROR if `Matrix-Control` lacks `JUCE_ASIO=1` on Windows (product define cannot drift from Tests-only).

## Spec Change Log

## Review Triage Log

- false — Blind: missing CMake/release hold until Steinberg countersigns — Frozen decision enables ASIO in source for local/CI now; public publish is a human gate, not a configure switch.
- defer — Blind: no LICENSE/third-party ASIO notice in this diff — Real obligation for first public ASIO ship; deferred (see deferred-work.md).
- medium / patch — Blind + Verification Gap: Tests-only `JUCE_ASIO` can stay green if product define is removed — Real; fixed with configure-time assert on `Matrix-Control` definitions.
- low / rejected — Blind: bare `expect(foundAsio)` without listing type names — Cosmetic CI diagnostics; not everyday user harm.
- false — Blind: WASAPI/DirectSound not re-asserted — Change does not disable JUCE defaults; no bad outcome.
- false — Blind: `containsIgnoreCase("ASIO")` false-positive risk — Matches product `isAsioDeviceType` policy; JUCE type name is ASIO.
- low / rejected — Blind: duplicated Windows/non-Windows scan loops — Cosmetic; no demonstrated harm.
- low / rejected — Blind: ASIO define in separate `if(WIN32)` vs inside main PUBLIC block — Style only; Windows gate is correct.
- low / rejected — Blind: no `message(STATUS)` when ASIO enabled — Optional noise; FATAL_ERROR covers product miss.
- false — Blind: no test of `refreshDriverTypeCombo` — Combo only enumerates `getAvailableDeviceTypes()`; registration test covers the same JUCE list.
- (none) — Edge Case Hunter returned no findings.
