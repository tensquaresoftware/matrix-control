---
title: 'Master Edit UNISON DETUNE (CC 94)'
type: 'feature'
created: '2026-09-18'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'eee8d92fce9e1c2fc618b0f0ed861308216e66e9'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-settings-device-eprom-type.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-unison-matrix-1000.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Optimised Matrix-1000 EPROMs (GLIGLI / TAUNTEK / UNTERGEEK) expose Unison Detune via MIDI CC 94 (0 = none, 127 = max), but Master Edit has no control for it. Stock FACTORY / UNKNOWN firmware ignore that CC, so a naive always-on control would mislead and spam useless MIDI.

**Approach:** Add UNISON DETUNE as a stable APVTS int parameter (0–127) under Master UNISON in MISC, always visible, grayed when EPROM TYPE is not optimised, and send MIDI CC 94 only while the gate is open — never via Master packed SysEx 0x03.

**Decisions (planning):**
- Keep the full spec in one delivery (token overrun accepted).
- UI: always show UNISON DETUNE under UNISON; when stock/UNKNOWN, gray it and show INFO footer on click. Shift VOLUME INVERT, BANK LOCK, MEMORY PROTECT down one row; consume the existing trailing empty spacer. Build the row through the normal descriptor / WidgetFactory / ParameterCell path — no one-off layout hacks.
- Host model: always register an APVTS `AudioParameterInt` with a stable ID; never remove it later; grey UI only.
- Stock outbound: suppress CC 94 while EPROM is FACTORY/UNKNOWN; stored/automated value may still change in-plugin; do not force to 0 on EPROM flip.

## Boundaries & Constraints

**Always:**
- Place UNISON DETUNE immediately under Master UNISON in MISC via normal factories (descriptor → layout id list → ParameterCell). Order becomes: Tune, Transpose, Bend, Unison, **Unison Detune**, Volume Invert, Bank Lock, Memory Protect (8 cells; former trailing spacer consumed).
- Capability gate SSOT = Settings `settingsEpromType` → `EpromTypePolicy::toEpromClass`; usable only for `EpromClass::kOptimised` (GLIGLI, TAUNTEK, UNTERGEEK). FACTORY and UNKNOWN = grayed + no CC outbound.
- Refresh enablement whenever EPROM TYPE changes (Settings, DEVICE SETUP CONFIRM, host restore, device-family coerce) — same spirit as `refreshSysExDelayFromSettings`.
- Outbound = MIDI CC 94, value 0–127, on APVTS `midiChannel` (Panic channel semantics). Suppress all CC 94 sends while gated off. Do not map into `MasterModel` / SysEx 0x03.
- Register a permanent APVTS int parameter (default 0); include it in Master strip/collect ID lists like other Master Edit params. Never delete this parameter ID in a later release.
- Keep Master UNISON (`miscUnisonEnable`, octet 169) and patch Keyboard Mode Unison independent — no cross-write with Detune.
- Matrix-6/6R: Master Edit already hidden; no Detune UI there.
- When grayed, reuse `GrayedControlHelper` + ASCII INFO footer explaining optimised EPROM requirement.
- Label: `UNISON DETUNE`. ASCII-only strings in `PluginDisplayNames`.

**Never:**
- Treat Device Inquiry version as live Detune capability SSOT.
- Add Detune to Master packed 172-byte model or full Master SysEx dump path.
- Rename or retarget `miscUnisonEnable` / SysEx offset 169.
- Conflate with Patch DCO 2 DETUNE (`dco2Detune`).
- Grow Misc / Master Edit design height.
- Hand-built widgets or ad-hoc rows outside the descriptor/factory ParameterCell path.
- French in source or UI strings.
- Remove or renumber existing APVTS parameter IDs (including the new Detune ID once shipped).
- Force Detune to 0 when switching EPROM to FACTORY/UNKNOWN.
- Send CC 94 while the EPROM gate is closed.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Optimised EPROM | EPROM = GLIGLI/TAUNTEK/UNTERGEEK; user edits Detune | Control enabled; CC 94 sent 0–127 on `midiChannel` | N/A |
| Stock EPROM | EPROM = FACTORY or UNKNOWN | Control visible, grayed; click → INFO footer; no CC 94 | N/A |
| EPROM flips optimised→stock | Settings / restore changes EPROM | Control grays; CC suppressed; APVTS value kept (not forced to 0) | No crash; no Master SysEx for Detune |
| EPROM flips stock→optimised | User selects TAUNTEK etc. | Control enables; last APVTS value kept; further edits send CC | N/A |
| Host automation while stock | Host writes Detune AudioParameter | Value updates in APVTS; no CC 94; no host crash | Suppress send |
| Master Edit hidden | Matrix-6/6R | No Detune UI (panel hidden); param still in APVTS | N/A |
| No MIDI output | Ports unset / outbound blocked | Value may update; no hard crash | Existing outbound gates |
| Init / default | Fresh param | Default 0 (no detune) | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Panels/.../MiscPanel.cpp` — insert Detune id under `kUnisonEnable`; remove trailing `""` spacer; shift Volume Invert / Bank Lock / Memory Protect; contextual help.
- `Source/Shared/Definitions/DesignPanels.h` — Misc `kParameterCellCount = 8` unchanged.
- `Source/Shared/Definitions/PluginIDs.h` / `PluginDisplayNames.h` / `PluginDescriptorsMasterEdit.cpp` — new int 0–127 descriptor + label/help; wire through existing Master Edit descriptor tables.
- `Source/Core/Models/ApvtsLayoutBuilder.cpp` / `ApvtsFactory.*` / `WidgetFactory` — register AudioParameterInt via normal Master Edit layout path.
- `Source/Core/Services/EpromTypePolicy.*` + `SysExDelayProfile.h` `EpromClass` — reuse `toEpromClass` / `kOptimised`; optional thin `supportsUnisonDetune` alias only if it clarifies call sites.
- `Source/GUI/Helpers/GrayedControlHelper.*` — grayed appearance + footer on gated Detune cell.
- `Source/Core/MIDI/Transport/MidiSender.*` + `MidiManager.*` / outbound queue — enqueue CC 94 on value change when gate open; never via Master SysEx debounce.
- `Source/GUI/PluginEditorSettings.cpp` + DEVICE SETUP finish paths — refresh Detune graying when EPROM persists.
- `Source/Core/Models/ApvtsMasterMapper.*` / `MasterModel.*` — **do not** add Detune to packed buffer mapping; do add ID to strip/collect lists where Master params are enumerated.
- `Tests/` + `MigratedModulePanelLayouts.cpp` — gate truth table; CC 94 channel/controller/value; layout mirror includes Detune row.
- Deferred note consumed: `_bmad-output/implementation-artifacts/deferred-work.md` (Unison Detune gate entry).

## Tasks & Acceptance

**Execution:**
- [x] `PluginIDs.h` + `PluginDisplayNames.h` + `PluginDescriptorsMasterEdit.cpp` + APVTS layout — add stable UNISON DETUNE int 0–127 (default 0) through normal Master Edit factories.
- [x] `MiscPanel.cpp` (+ layout mirror) — order under UNISON; consume spacer; no height change.
- [x] EPROM gate + grayed UI refresh — `toEpromClass` → enable/gray; refresh on EPROM change paths; INFO footer when grayed.
- [x] MidiManager / sender path — emit CC 94 on change only when gate open and outbound allowed; suppress when gated off.
- [x] State strip/collect managed IDs — include new APVTS id with other Master Edit params.
- [x] Unit tests — gate truth table; CC 94 payload; assert no Master buffer offset for Detune.

**Acceptance Criteria:**
- Given EPROM TYPE = TAUNTEK (or GLIGLI/UNTERGEEK), when the user moves UNISON DETUNE, then the plugin sends MIDI CC 94 with the same 0–127 value on the configured MIDI channel.
- Given EPROM TYPE = FACTORY or UNKNOWN, when the user views Master Edit MISC, then UNISON DETUNE is visible, grayed, shows INFO on click, and no CC 94 is sent.
- Given EPROM TYPE flips between optimised and stock, when Settings/DEVICE SETUP/host restore updates EPROM, then Detune graying refreshes, the APVTS value is kept, and no Master SysEx is written for Detune.
- Given a host project that automated Detune, when the project reopens with EPROM TYPE = FACTORY, then the host must not crash and CC 94 stays suppressed while gated off.
- Given Matrix-6/6R, when Master Edit is hidden, then no Detune control is shown.
- Given Misc layout construction, when cells are built, then Detune uses the same descriptor/factory ParameterCell path as neighbouring Misc params.

## Implementation Notes

- 2026-09-18: Implemented UNISON DETUNE (`miscUnisonDetune`) as APVTS int 0–127 with `kNoSysExOffset`; `ApvtsMasterMapper::buildIntDescriptors` filters packed-only ints so Master SysEx / init / `masterParameterIds_` skip Detune while session strip still includes it via Misc `kIntParameters`.
- Outbound: `MidiManager::sendUnisonDetune` (CC 94) gated by `supportsUnisonDetune` + `isMasterEditOutboundAllowed`; processor `dispatchUnisonDetuneChange` bypasses Master SysEx debounce.
- UI: MiscPanel listens to `settingsEpromType`, grays slider via `setEnabled`, INFO footer via `GrayedControlHelper` (same pattern as LEGATO PORTA).
- Matrix coverage (unit tests run): optimised CC → MidiManager; stock suppress → MidiManager + EpromTypePolicy; outbound blocked → MidiManager; default/range/exclusion → ApvtsMasterMapper; strip ID → SessionPersistencePolicy; factory Slider path → ModulePanelConfigBuilder + MigratedModulePanelLayouts; Matrix-6/6R hide → MasterEditGate. Grayed-click footer interaction remains manual (GUI convention).
- Review patches (2026-09-18): skip non-packed ints in Init/Master test helpers; MiscPanel `valueTreeRedirected` refresh; Basic Channel + dispatch harness tests; PluginEditorAudio EPROM dialog include kept (required to compile).

## Spec Change Log

## Review Triage Log

- high — MasterModuleInitServiceTests / InitTemplateWriterTests walk MiscModule::kIntParameters including miscUnisonDetune (sysExOffset -1) and call MasterModel::setValue/getValue → PackedFieldCodec jassert / OOB; verified by failing MasterModuleInitService and InitTemplateWriter suites. Route: patch.
- high — MiscPanel lacks valueTreeRedirected; host replaceState can leave Detune graying stale while Spec requires restore refresh; confirmed pattern used by MasterEditPanel and peers. Route: patch.
- medium — MidiManagerTests only cover Omni 16-channel spray for CC 94, not configured midiChannel Basic Channel path (verification-gap). Route: patch.
- medium — No processor/harness test that APVTS miscUnisonDetune change reaches sendUnisonDetune (verification-gap). Route: patch.
- false — Unrelated PluginEditorAudio.cpp include of EpromTypePromptDialog.h: removing it fails compile (`member access into incomplete type 'EpromTypePromptDialog'` at isVisible()); include is required, not scope noise.
- false — Claim that Init Misc must reset miscUnisonDetune: Init/Master template path intentionally uses packed buildIntDescriptors only; Detune is APVTS+CC outside MasterInit.syx; matrix “Init/default 0” is fresh-param default, not Init-button semantics.
- low (rejected) — Extract shared Panic/Detune channel helper: real duplication but fix adds shared surface beyond a direct correction; unlikely everyday harm now.
- false — Stale Code Map pointing at MidiSender / Settings hooks: fixing would edit this build’s spec; rejected per review rules.

## Design Notes

Firmware docs (GliGli v1.16+, Tauntek, Untergeek): CC 94 = Unison Detune / Celeste Level, 0–127, not stored in patch memory, active even in poly. Stock Oberheim EPROM has no equivalent — hence EPROM TYPE gate.

Stable APVTS ID + grayed UI avoids the real long-term host risk (removing a parameter after automation exists). Suppressing CC while stock avoids useless MIDI without wiping automation curves.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build success
- `ctest --preset macos-debug-arm64 -R 'Eprom|UnisonDetune|Misc|Midi' --output-on-failure` -- expected: related unit tests pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++

**Manual checks (if no CLI):**
- Master Edit MISC: Detune under UNISON; Volume Invert / Bank Lock / Memory Protect shifted down; toggle EPROM TYPE and confirm gray + footer + MIDI monitor CC 94 only when optimised.
