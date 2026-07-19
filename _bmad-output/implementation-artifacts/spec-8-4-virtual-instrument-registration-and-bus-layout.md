---
title: '8.4 Virtual Instrument Registration and Bus Layout'
type: 'feature'
created: '2026-07-19'
status: 'done'
baseline_commit: '18348edfb9f0310bd62c36fe926abe8c7167285e'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad-output/implementation-artifacts/epic-8-context.md'
  - '{project-root}/_bmad-output/implementation-artifacts/r-1-remove-plugin-audio-input-bus.md'
  - '{project-root}/_bmad-output/implementation-artifacts/8-3-ui-lock-without-synth.md'
  - '{project-root}/_bmad-output/implementation-artifacts/8-5-matrix-6-6r-patch-memory-limits.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Story 8.4 must close FR-4 / FR-46 / AD-11 for users: hosted instrument role with correct buses, standalone Audio From guidance that follows detected Matrix type, and MASTER EDIT unavailable on Matrix-6/6R. Registration and hosted output-only buses already landed via R-1 and CMake; the remaining gaps are device-type-aware standalone input guidance and MASTER EDIT graying (with Core fail-closed so M-1000 master SysEx cannot reach M-6 hardware).

**Approach:** Verify instrument registration and AD-11 hosted/standalone buses stay correct; wire standalone Audio From defaults/guidance from `deviceType` after Device Inquiry without changing the stereo input bus declaration; gray MASTER EDIT for Matrix-6/6R like Bank Utility, layered under the existing device/compare lock, and skip master outbound/init when M-6 family is detected.

## Boundaries & Constraints

**Always:**
- Keep hosted plugin: stereo output only, no input bus; silent clear via existing passthrough path (AD-11 / R-1).
- Keep standalone: stereo input bus named "Audio From" + passthrough/gain/peak; do not reintroduce hosted input.
- Keep CMake `IS_SYNTH TRUE`, `AU_MAIN_TYPE kAudioUnitType_MusicDevice`, `VST3_CATEGORIES Instrument`, MIDI in/out — verify, do not churn.
- After inquiry, standalone Audio From guidance/default follows device type: mono selection for Matrix-1000, stereo for Matrix-6/6R (FR-4). Prefer a small pure helper (e.g. preferred channel mode from `MatrixDeviceTypes`) over scattering switch logic.
- When `deviceDetected && isMatrix6Family(deviceType)`, gray entire MASTER EDIT (section + modules) using `GrayedControlHelper` on **children** of `MasterEditPanel` — do not gray the panel root (owned by `CompareLockBinder`).
- Compose with 8.3: undetected or Compare still locks the whole MASTER column; when unlocked and M-6, child gray applies.
- Fail-closed Core: skip master parameter SysEx dispatch and master module INIT when M-6 family detected (mirror 8.5 bank no-ops). PATCH / Matrix Mod / Patch Manager stay active.
- English-only source; Core ↛ GUI; unit tests for pure helpers and Core gates.

**Ask First:**
- Changing JUCE standalone input **bus channel count** at runtime (mono bus vs stereo bus) — AD-11 keeps stereo bus; only ask if investigation proves a host requires otherwise.
- Any CMake category / plugin-code change beyond verification.
- Expanding Matrix-6 MASTER model / SPLIT (explicit v2 non-goal).

**Never:**
- Revert R-1 hosted input removal or show Audio From / Input Gain / peak in plugin mode.
- Gate Device Inquiry or instrument-path notes/CC behind MASTER or device-type gray.
- Extend `CompareLockBinder` with `deviceType` for MASTER gray (fights root alpha / click ownership).
- Rework Device Inquiry, footer identity, Bank Utility 8.5 limits, or FR-4b latency.
- French in source or user-facing strings outside `PluginDisplayNames`.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Hosted load | VST3/AU wrapper | Output stereo only; 0 input buses; process clears output | N/A |
| Standalone + M-1000 detected | `deviceDetected=true`, type M-1000 | Audio From guidance/default prefers mono source | If no mono entry, keep prior valid selection |
| Standalone + M-6/6R detected | type Matrix-6 or 6R | Audio From guidance/default prefers stereo pair | Same fallback |
| Device type clears / undetected | `deviceDetected=false` | Do not force Audio From; MASTER column under 8.3 lock | N/A |
| M-6 MASTER gray | detected M-6/6R, not Compare | MASTER children grayed; footer on blocked use; no master SysEx/INIT | PATCH path unchanged |
| M-1000 MASTER | detected M-1000 | MASTER fully interactive; master SysEx allowed when editor outbound allowed | N/A |
| Compare + M-6 | Compare active | Whole MASTER stays device/compare locked at root; no binder fight | N/A |

</frozen-after-approval>

## Code Map

- `CMakeLists.txt` -- verify instrument registration flags (no change expected)
- `Source/Core/PluginProcessor.{h,cpp}` -- buses already AD-11; gate master APVTS→SysEx for M-6; optional standalone default sync on `deviceType`
- `Source/Core/Audio/AudioInputSourceCatalog.{h,cpp}` -- catalog entries; optional filter/guidance hooks
- `Source/Core/Audio/AudioPassthroughProcessor.{h,cpp}` -- channel modes already support mono/stereo
- `Source/Core/Audio/StandaloneAudioInputRouter*.{h,cpp}` -- standalone catalog source
- `Source/Core/Services/DeviceTypeRegistry.{h,cpp}` -- `fromApvtsProperty`; optional preferred-layout helper nearby or in `MatrixDeviceTypes`
- `Source/Shared/Definitions/MatrixDeviceTypes.h` -- `isMatrix6Family` (unused in GUI today — wire it)
- `Source/GUI/Panels/.../MasterEditPanel/MasterEditPanel.{h,cpp}` -- primary FR-46 gray (Bank Utility pattern)
- `Source/GUI/Helpers/GrayedControlHelper.{h,cpp}` -- appearance + footer helpers
- `Source/GUI/Helpers/CompareLockBinder.*` -- **do not modify** for M-6
- `Source/GUI/Panels/.../BodyPanel.cpp` -- keep existing binder wiring
- `Source/GUI/PluginEditor.cpp` / `HeaderPanel.*` -- standalone Audio From refresh on device-type change
- `Source/Core/Actions/ModuleActionHandler.cpp` -- skip master module INIT for M-6
- `Source/Shared/Definitions/PluginDisplayNames.h` -- MASTER M-6 footer copy
- `Tests/Unit/AudioPassthroughProcessorTests.cpp` -- existing hosted zero-input cases
- `Tests/Unit/` -- new pure-helper + Core master-gate tests
- `_bmad-output/implementation-artifacts/sprint-status.yaml` -- advance `8-4-...` when implementing

## Tasks & Acceptance

**Execution:**
- [x] `CMakeLists.txt` + `PluginProcessor` buses -- verify instrument + AD-11 hosted/standalone; document-only if already correct -- close FR-4 registration / bus AC without churn
- [x] `MatrixDeviceTypes` / small helper + `PluginProcessor`/`PluginEditor`/`AudioInputSourceCatalog` -- on standalone `deviceType` change after inquiry, prefer mono (M-1000) or stereo (M-6/6R) Audio From default/guidance without changing stereo bus declaration -- FR-4 standalone consequence
- [x] Unit test preferred-layout helper -- cover M-1000 / M-6/6R / unknown -- lock I/O matrix
- [x] `MasterEditPanel` + `PluginDisplayNames` -- ValueTree listener on `deviceDetected`/`deviceType`; gray children when M-6 family; footer on blocked use -- FR-46 GUI
- [x] `ModuleActionHandler` + `PluginProcessor` master dispatch -- no-op master INIT and master parameter SysEx when M-6 family -- fail-closed like 8.5 banks
- [x] Unit tests for master Core gates -- M-6 no-op / M-1000 still sends when outbound allowed -- regression
- [x] `sprint-status.yaml` -- set `8-4-virtual-instrument-registration-and-bus-layout` in-progress→in-review with implementation -- tracking
- [x] Build + `Matrix-Control_Tests` -- green macOS Debug; smoke Standalone Audio From + hosted silent out -- verification

**Acceptance Criteria:**
- Given hosted VST3/AU, when the plugin loads, then it is an Instrument / Music Device with stereo output only and no input bus, and output is cleared (existing R-1 behaviour preserved).
- Given standalone after Device Inquiry sets `deviceType`, when type is Matrix-1000, then Audio From guidance/default prefers mono; when Matrix-6/6R, prefers stereo; header audio controls remain standalone-only.
- Given Matrix-6 or Matrix-6R detected and Compare inactive, when the user views MASTER EDIT, then the whole section is grayed/non-interactive with footer guidance; PATCH / Matrix Mod / Patch Manager remain usable (subject to other gates).
- Given M-6 family detected, when master parameter or master INIT would fire, then no master SysEx is enqueued; Matrix-1000 detected behaviour unchanged.
- Given device lock or Compare active, when M-6 gray is also applicable, then root lock and child gray compose without last-writer-wins click/alpha bugs.
- Given full unit suite, when tests run, then they pass; no French in source; Core ↛ GUI preserved.

## Spec Change Log

## Design Notes

**Standalone guidance ≠ bus resize.** AD-11 keeps a stereo standalone input bus. FR-4 “mono vs stereo” means Audio From **selection/guidance** (catalog mono:`N` vs stereo:`N` + passthrough channel mode), not `BusesProperties` mono input.

**MASTER gray layering (copy Bank Utility):**
1. `CompareLockBinder` owns `masterEditPanel_` root for `!deviceDetected || compare`.
2. `MasterEditPanel` grays `sectionHeader_` + module panels when `deviceDetected && isMatrix6Family(type)`.
3. Core mirrors GUI for master INIT / master SysEx only.

**Registration:** Treat CMake + `makeBusesProperties` as done unless verification finds drift.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests && ctest --test-dir Builds/macOS/Debug --output-on-failure` -- expected: all tests pass
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_VST3 Matrix-Control_Standalone` -- expected: clean build

**Manual checks (if no CLI):**
- Standalone: connect M-1000 → Audio From prefers mono; connect/simulate M-6 → prefers stereo; MASTER grayed on M-6 only.
- Hosted VST3: no Audio From in header; instrument track instantiates; silent stereo out with no input bus.
- Compare on M-6: MASTER stays locked at root; unlock Compare with M-6 still detected → child gray remains.

## Suggested Review Order

**Master edit gate (FR-46)**

- Shared predicate: MASTER outbound only for detected Matrix-1000
  [`MasterEditGate.h:8`](../../Source/Core/MIDI/MasterEditGate.h#L8)

- Parameter SysEx path gated before mapper/dispatch
  [`PluginProcessor.cpp:1485`](../../Source/Core/PluginProcessor.cpp#L1485)

- Master INIT no-op + footer when blocked
  [`ModuleActionHandler.cpp:106`](../../Source/Core/Actions/ModuleActionHandler.cpp#L106)

- GUI children grayed aligned with Core gate; focus cleared
  [`MasterEditPanel.cpp:78`](../../Source/GUI/Panels/MainComponent/BodyPanel/MasterEditPanel/MasterEditPanel.cpp#L78)

**Standalone Audio From preference (FR-4)**

- Pure mono/stereo preference helper (no bus resize)
  [`DeviceAudioInputPreference.h:18`](../../Source/Core/Audio/DeviceAudioInputPreference.h#L18)

- Apply on device change and session redirect
  [`PluginProcessor.cpp:1661`](../../Source/Core/PluginProcessor.cpp#L1661)

- Editor refreshes combo after preference settles
  [`PluginEditor.cpp:897`](../../Source/GUI/PluginEditor.cpp#L897)

**Registration / AD-11 (verify-only)**

- Existing instrument registration unchanged
  [`CMakeLists.txt:568`](../../CMakeLists.txt#L568)

**Tests**

- Preference + master-gate unit coverage
  [`DeviceAudioInputPreferenceTests.cpp:1`](../../Tests/Unit/DeviceAudioInputPreferenceTests.cpp#L1)

### Review Findings

- [x] [Review][Decision] MASTER gate: Matrix-1000 allowlist vs `isMatrix6Family` — resolved: keep allowlist (MASTER only for detected Matrix-1000; Unknown stays blocked on MASTER). Whole-GUI gray + explicit footer when detected+Unknown deferred (out of 8-4 scope). Footer copy for non–Matrix-6 blocks → patch below.
- [x] [Review][Decision] Master buffer sync while outbound gated — resolved: always `apvtsToBuffer`; gate SysEx `dispatch` only (no auto flush on re-allow).
- [x] [Review][Patch] Always map master APVTS→buffer; gate only SysEx dispatch when outbound blocked [`PluginProcessor.cpp:1487`]
- [x] [Review][Patch] Skip or neutralize child MASTER gray alpha when Compare/device root lock already dims the panel [`MasterEditPanel.cpp:91`]
- [x] [Review][Patch] Footer: Matrix-6 copy only for Matrix-6 family; distinct/explicit copy when MASTER blocked for Unknown (not Matrix-6 wording) [`ModuleActionHandler.cpp:106`] [`MasterEditPanel.cpp:106`]
- [x] [Review][Patch] Treat preferred Audio From as current only if id is still in the catalog; avoid combo fallback rewriting a wrong mono/stereo source [`PluginProcessor.cpp:1676`] [`PluginEditor.cpp:831`]
- [x] [Review][Patch] Prevent keyboard interaction on grayed MASTER children after `giveAwayKeyboardFocus` [`MasterEditPanel.cpp:91`]
- [x] [Review][Defer] Whole-GUI gray + explicit footer when `deviceDetected` and type is Unknown — deferred (product rule confirmed 2026-07-19; out of 8-4); follow-up story / correct course
- [x] [Review][Defer] `updateDeviceStatus` sets `deviceDetected` before `deviceType` — deferred, pre-existing [`MidiManager.cpp:1134`]
- [x] [Review][Defer] Defense-in-depth `sendMaster` MasterEditGate — deferred, pre-existing; known call sites already gated [`MidiManager.cpp:295`]
- [x] [Review][Defer] No PluginProcessor/ModuleActionHandler integration tests beyond pure helpers — deferred, pre-existing coverage gap vs helper unit tests [`DeviceAudioInputPreferenceTests.cpp:1`]
