---
organization: Ten Square Software
project: Matrix-Control
title: Story 7.8 — Header Footer Shell and Persistence
author: BMad Agent
status: done
baseline_commit: 8047ec979ca7e967ea8262cf20f37922ad5eae1
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md
  - implementation-artifacts/2-9b-header-routing-controls-uat-slice.md
  - implementation-artifacts/7-7-settings-page-consolidation.md
  - implementation-artifacts/7-10-about-modal.md
  - implementation-artifacts/7-6-internal-patches-panel-wiring.md
  - implementation-artifacts/8-5-matrix-6-6r-patch-memory-limits.md
  - implementation-artifacts/deferred-work.md
  - project-context.md
created: 2026-07-14
updated: 2026-07-14
---

# Story 7.8: Header Footer Shell and Persistence

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a Matrix-Control user,
I want header routing, skin, scale, footer messages, and prefs persistence fully wired per PRD,
so that the shell behaves consistently across plugin and standalone sessions (FR-3, FR-39, FR-42, FR-43, FR-44, FR-45, FR-41, UX-DR7–8).

## Acceptance Criteria

### AC 1 — Header routing completion (FR-39, FR-45 partial)

1. **Plugin mode KEYBOARD FROM regression fix:** In hosted plugin mode, the KEYBOARD FROM packet is **visible** (label + combo + instrument LED), combo shows fixed **`HOST`** (`PluginDisplayNames::HeaderPanel::kHostDisplay`), combo is **disabled/grayed**, non-interactive. Instrument path continues to use host MIDI buffer (Story 2.3). **Do not** write `keyboardFromPortId` / `keyboardFromEnabled` in plugin mode.
2. **Standalone KEYBOARD FROM** unchanged: port combo enabled, `KeyboardFromMidiInput` wired, persistence via `keyboardFromPortId`.
3. **MIDI FROM / TO / activity LEDs** unchanged — verify round-trip after other edits only.
4. **Audio FROM / Input Gain / Peak** remain standalone-only (hidden in plugin — AD-11); wiring unchanged from Stories 2.7 / 7.7 intent.
5. **Optional (defer if timeboxed):** `juce::MidiDeviceListConnection` to refresh port lists on OS hot-plug — document in Dev Agent Record if skipped.

### AC 2 — Skin and UI scale (FR-42, FR-43, FR-41)

1. **Logo popup remains SSOT** for Skin + UI Scale (Story 7.10 + FR-41). Do **not** duplicate controls in Settings — 7.7 Phase A migration was superseded by logo popup wiring.
2. Skin (Black/Cream) and scale presets (50–200%) persist via existing APVTS properties (`settingsSkinVariant`, `guiScaleId`); restore on editor open unchanged.
3. `applyUiScale` / `applySkinFromItemId` propagate through `MainComponent` without global `AffineTransform` (UX-DR8).
4. Regression: Settings (`SETTINGS...`) and About (`ABOUT...`) still open from logo popup; Shift+click shortcuts unchanged.

### AC 3 — Footer messaging polish (FR-44)

1. `FooterPanel` continues to listen to `uiMessageText` + `uiMessageSeverity` on APVTS root.
2. **Two-zone layout:** left = message + severity icon; **right = device identity** (AC 4). Message text truncates with ellipsis if needed; identity zone never overlaps message.
3. Severity colours use **skin tokens** where available (`SkinColourId` or new footer severity IDs in `SkinValues`) — remove hardcoded hex literals in `FooterPanel::getSeverityColour`.
4. Remove French comment in `FooterPanel.cpp` (CONVENTIONS — English only in source).
5. Grayed-control clicks across the app use **footer feedback, not modals** (FR-44) — see AC 5.

### AC 4 — Footer device identity (FR-53 partial — display only)

1. **Given** `deviceDetected=true` on APVTS **When** footer paints **Then** right zone shows `{deviceType}` + firmware string from `deviceVersion` (e.g. `Matrix-1000 · v1.20`).
2. **Given** `deviceDetected=false` **Then** right zone shows placeholder from `PluginDisplayNames::FooterPanel` (e.g. `"No device"`) or empty — no crash on missing version.
3. Listen to `deviceDetected`, `deviceType`, `deviceVersion` property changes; repaint on message thread only.
4. **Out of scope:** Device Inquiry logic, timeout recovery copy, UI lock (Epic 8 Stories 8.1–8.3). Inquiry already runs via `MidiManager::performDeviceInquiry()` after port sync — this story **displays** existing APVTS state only.

### AC 5 — Graying rules matrix (FR-45)

Introduce a **shared GUI helper** (recommended: `Source/GUI/Helpers/GrayedControlHelper.h` or extend `TSS::Button`) encapsulating the pattern proven in `BankUtilityPanel` / `InternalPatchesPanel`:

| Rule | Location | Behaviour |
|------|----------|-----------|
| ROM INIT/PASTE/STORE | `InternalPatchesPanel` | **Already done (7.6)** — verify no regression |
| Bank Utility device gating | `BankUtilityPanel` | **Already done (8.5)** — migrate to shared helper |
| Mutator compare / history | `PatchMutatorPanel` | **Already done (6.8)** — verify no regression |
| **Keyboard From plugin HOST** | `HeaderPanel` | AC 1 |
| **LEGATO PORTA vs UNISON** | `RampPortamentoPanel` | When `portamentoKeyboardMode` choice index = **UNISON** (index **2** in descriptor choices: Rotate=0, Reasgn=1, Unison=2, Rearob=3), **LEGATO PORTA** control is grayed (visual inactive, click intercept). Footer message from `PluginDisplayNames::PatchEditSection::RampPortamentoModule::kLegatoPortaUnisonBlockedFooter` (new constant). APVTS value must **not** change on blocked click. |
| **Paste incompatibility** | `ModuleHeader`, `MatrixModulationPanel` | Replace `pasteButton_.setEnabled(false)` with grayed-but-clickable + footer (`PluginDisplayNames` message for incompatible paste). Core `*PasteEnabled` mirrors remain SSOT. |

**Helper contract (minimum):**
- `applyGrayedAppearance(Component&, bool grayed)` — alpha / inactive look
- `setGrayedClickHandler(Component&, std::function<void()> showFooter)` — intercept click when grayed
- Footer writes via `ExceptionPropagator::propagateToApvts` or thin wrapper setting `uiMessageText` + severity

### AC 6 — Startup persistence policy (FR-3, AD-7)

1. **Prefs persist** (already via `getStateInformation` / `setStateInformation` + `apvts.copyState()`): MIDI port IDs, skin, scale, Computer Patches folder, Mutator recipe, reconciliation policy, hardware latency, standalone audio prefs.
2. **Mutator history** remains stripped on save / reset on load (`MutatorSessionPersistence`) — verify unchanged.
3. **Patch parameters NOT restored when device connected at session load:**
   - After `apvts.replaceState` in `setStateInformation`, if restored `midiInputPortId` + `midiOutputPortId` are non-empty (device connection intent), **strip all PATCH (and MASTER) APVTS parameter values** from restored state before they affect UI, **or** replace with InitDefaults and enqueue `requestCurrentPatch()` after deferred port sync + successful Device Inquiry.
   - Preferred approach: add `Core::SessionPersistencePolicy::stripPatchAndMasterParameters(juce::ValueTree& state)` called from `setStateInformation` when both MIDI ports are configured in restored state; after inquiry succeeds and `deviceDetected=true`, call existing `MidiManager::requestCurrentPatch()` (or document hook if RPC not yet user-facing — minimum: do not apply stale XML patch params to APVTS while connected).
4. **Unit test:** temp-file round-trip — save with non-default patch param + ports → reload → assert patch param equals InitDefault or post-sync value, **not** saved XML value, when device mock reports connected.
5. **Out of scope:** unsaved-edit warning policy UI (Settings Phase B), INIT template path Settings UI, automatic patch recall when synth disconnected (PRD explicit non-goal).

### AC 7 — Header/footer shell hygiene

1. Remove unused `ActivityLed` members (`width_`, `height_`, `uiScale_`) if still present — deferred from Story 2.8 (`deferred-work.md`).
2. Align `BankUtilityPanel` alpha hack with shared helper (optional refactor — at minimum document migration path).
3. Builds: macOS preset — VST3 + Standalone + `Matrix-Control_Tests` green.

### AC 8 — Epic boundaries (do not expand scope)

| Item | Owner |
|------|-------|
| Full UI lock without synth (FR-2) | Epic 8 Story 8.3 |
| Device Inquiry timeout footer copy / re-inquiry on port change polish | Epic 8 Story 8.2 |
| Footer/header **pixel layout** audit (gaps, padding @ 50–200%) | Epic U Stories U-3, U-4 |
| Settings Phase B (Master ops, Defrag, logging, INIT paths) | Future 7.7 follow-up |
| MASTER EDIT graying Matrix-6 (FR-46) | Epic 8 / existing |

## Tasks / Subtasks

- [x] **T1 — Header plugin HOST fix** (AC: 1)
  - [x] Add `kHostDisplay = "HOST"` to `PluginDisplayNames::HeaderPanel`
  - [x] Change `updateKeyboardFromVisibility()`: always show packet; plugin mode → populate HOST, disable combo, skip port list
  - [x] `resized()` always lays out KEYBOARD FROM packet (remove `if (!isPluginMode_)` guard around placement)
  - [x] Manual: plugin in Live — KEYBOARD FROM visible, grayed HOST

- [x] **T2 — Footer two-zone + identity** (AC: 3, 4)
  - [x] `FooterPanel`: listen `deviceDetected`, `deviceType`, `deviceVersion`
  - [x] Split paint: left message, right identity; add `PluginDisplayNames::FooterPanel` strings
  - [x] Skin-based severity colours; remove French comment
  - [x] Add design constants for right-zone min width in `DesignPanels.h` if needed (÷4)

- [x] **T3 — GrayedControlHelper + migrations** (AC: 5)
  - [x] New helper under `Source/GUI/Helpers/`
  - [x] Migrate `BankUtilityPanel` grayed footer clicks to helper
  - [x] `RampPortamentoPanel`: APVTS listener on `portamentoKeyboardMode` → gray LEGATO PORTA when UNISON
  - [x] `ModuleHeader` + `MatrixModulationPanel`: paste grayed-click footer

- [x] **T4 — FR-3 patch param persistence boundary** (AC: 6)
  - [x] `SessionPersistencePolicy.{h,cpp}` in Core (no GUI deps)
  - [x] Wire into `PluginProcessor::setStateInformation`
  - [x] Post-inquiry sync hook (minimal — document if `requestCurrentPatch` not invoked yet)
  - [x] `SessionPersistencePolicyTests.cpp`

- [x] **T5 — Verify + hygiene** (AC: 2, 7, 8)
  - [x] Logo popup skin/scale regression
  - [x] ActivityLed cleanup
  - [x] Full test suite + builds

### Review Findings

- [x] [Review][Decision] Matrix Mod parameters included in session strip — **resolved:** keep matrix-mod in strip scope (patch state, not prefs).

- [x] [Review][Decision] Post-inquiry `requestCurrentPatch()` hook deferred — **resolved:** accept FR-3 minimum (strip on load) for story closure.

- [x] [Review][Patch] GrayedControlHelper static map retains dangling `Component*` pointers [Source/GUI/Helpers/GrayedControlHelper.cpp:31-88]

- [x] [Review][Patch] GrayedControlHelper does not update footer callback on re-register [Source/GUI/Helpers/GrayedControlHelper.cpp:68-75]

- [x] [Review][Patch] BankUtilityPanel label/header clicks not intercepted when grayed [Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/BankUtilityPanel.cpp:90-94]

- [x] [Review][Patch] Whitespace-only MIDI port IDs arm patch strip policy [Source/Core/Services/SessionPersistencePolicy.cpp:115-117]

- [x] [Review][Patch] Footer identity blank when `deviceDetected=true` but `deviceType` empty [Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp:184-193]

- [x] [Review][Patch] Plugin HOST combo not visually grayed (AC 1) [Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp:431-437]

- [x] [Review][Patch] LEGATO PORTA uses `setEnabled(false)` instead of FR-45 enabled+intercept pattern [Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/RampPortamentoPanel.cpp:66-69]

- [x] [Review][Patch] LEGATO graying not cleared when keyboard-mode param missing [Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/RampPortamentoPanel.cpp:61-64]

- [x] [Review][Patch] AC 6.4 test incomplete — no `PluginProcessor` temp-file round-trip [Tests/Unit/SessionPersistencePolicyTests.cpp]

- [x] [Review][Patch] No unit test asserts matrix-mod PARAM nodes are stripped [Tests/Unit/SessionPersistencePolicyTests.cpp]

- [x] [Review][Defer] Hand-maintained parameter ID collection in `SessionPersistencePolicy` — deferred, maintenance burden when new APVTS groups added

- [x] [Review][Defer] `RampPortamentoPanel` hard-coded `getParameterCellAt(7)` layout index — deferred, fragile coupling

- [x] [Review][Defer] `GrayedControlHelper` writes footer via direct APVTS `setProperty` not `ExceptionPropagator` — deferred, spec allows thin wrapper

## Dev Notes

### Brownfield state (2026-07-14)

| Component | Path | Today | Gap |
|-----------|------|-------|-----|
| Header routing | `HeaderPanel.cpp` | MIDI/audio wired; plugin hides KEYBOARD FROM entirely | AC 1 regression vs 2-9b |
| Logo popup | `HeaderLogoPopupMenu.cpp` | Skin, scale, Settings, About | Keep as SSOT |
| Settings | `SettingsPanel.cpp` | HW latency, reconciliation policy only | No skin/scale (correct post-7.10) |
| Footer | `FooterPanel.cpp` | Message-only left zone; hardcoded colours | AC 3–4 |
| Persistence | `PluginProcessor.cpp:591-625` | Full APVTS restore including patch params | AC 6 |
| Device props | `MidiManager::updateDeviceStatus` | Writes `deviceDetected`, `deviceType`, `deviceVersion` | Footer doesn't read |
| Graying | Per-panel ad hoc | Bank/ROM/Mutator done | LEGATO, Paste, shared helper |

### Architecture compliance

- **Core ↛ GUI** — `SessionPersistencePolicy` lives in `Source/Core/`; footer reads APVTS properties only.
- **Display strings** — all new user-visible text in `PluginDisplayNames.h`.
- **No descriptor duplication** — LEGATO/UNISON rule uses existing parameter IDs from `PluginIDs::PatchEditSection::RampPortamentoModule`.
- **Threading** — footer/header updates on message thread; no `repaint()` from audio/MIDI callbacks.
- **Exception → footer** — reuse `ExceptionPropagator` pattern (`architecture.md` § patterns).

### LEGATO PORTA / UNISON rule

Matrix-1000 hardware: Legato Portamento (SysEx ID 47, offset 34) is incompatible with Keyboard Mode = **UNISON** (choice index **2**, SysEx ID 48). When UNISON is selected, gray LEGATO PORTA and show footer on click attempt. When user changes away from UNISON, restore normal control state. Implement in `RampPortamentoPanel` (owns both widgets via `BaseModulePanel` + `WidgetFactory` attachments).

### Keyboard From plugin (2-9b contract)

```61:61:_bmad-output/implementation-artifacts/2-9b-header-routing-controls-uat-slice.md
6. **KEYBOARD FROM — plugin (DAW):** combo **visible, disabled (grayed)**, fixed display **HOST**
```

Current code hides the entire packet when `isPluginMode_`:

```364:371:Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp
void HeaderPanel::updateKeyboardFromVisibility()
{
    const bool showKeyboardFrom = !isPluginMode_;

    instrumentActivityLed_.setVisible(showKeyboardFrom);
    keyboardFromLabel_.setVisible(showKeyboardFrom);
    keyboardFromComboBox_.setVisible(showKeyboardFrom);
}
```

### Paste graying today

`ModuleHeader::PasteEnabledPropertyListener` calls `pasteButton_.setEnabled(false)` — disabled buttons do not receive clicks, so **no footer** (FR-44 gap). Switch to helper pattern: stay enabled, `setInactiveAppearance(true)`, intercept click → footer.

### FR-3 implementation hint

Architecture AD-7:

| Data | Persisted |
|------|-----------|
| Plugin prefs | Session XML |
| Patch parameters (device connected) | **Not** restored |

`stripEphemeralMutatorStateForPersistence` already exists — add sibling `SessionPersistencePolicy::stripPatchAndMasterParameters` that iterates APVTS groups/parameters via descriptor lists (reuse `ApvtsLayoutBuilder` group IDs or existing factory metadata). Call when restored state has both MIDI ports set, **before** UI attaches to parameters.

`requestCurrentPatch()` exists in `MidiManager` but has **no production call sites** (Story 6-13 note). Minimum FR-3 fix: prevent stale XML params from appearing in UI; ideal: after successful inquiry, trigger sync RPC (optional enhancement — document in completion notes).

### Reference graying pattern (Bank Utility)

```82:97:Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/BankUtilityPanel.cpp
void BankUtilityPanel::setBankUtilityGrayed(bool grayed)
{
    bankUtilityGrayed_ = grayed;
    const float alpha = grayed ? 0.5f : 1.0f;
    // buttons stay enabled; onClick shows footer when grayed
```

### Files likely touched

| File | Change |
|------|--------|
| `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.{h,cpp}` | Plugin HOST display |
| `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.{h,cpp}` | Two-zone layout, identity |
| `Source/GUI/Helpers/GrayedControlHelper.{h,cpp}` | **New** shared FR-45 helper |
| `Source/GUI/Panels/.../RampPortamentoPanel.{h,cpp}` | LEGATO/UNISON constraint |
| `Source/GUI/Widgets/ModuleHeader.cpp` | Paste footer |
| `Source/GUI/Panels/.../MatrixModulationPanel.cpp` | Matrix-mod paste footer |
| `Source/GUI/Panels/.../BankUtilityPanel.cpp` | Helper migration (optional) |
| `Source/Core/Services/SessionPersistencePolicy.{h,cpp}` | **New** FR-3 strip logic |
| `Source/Core/PluginProcessor.cpp` | Call strip on session load |
| `Source/Shared/Definitions/PluginDisplayNames.h` | `kHostDisplay`, footer identity, graying messages |
| `Source/GUI/Layout/Design/DesignPanels.h` | Footer zone widths if needed |
| `Source/GUI/Skins/SkinValues.h` | Optional footer severity colours |
| `Tests/Unit/SessionPersistencePolicyTests.cpp` | **New** |
| `Tests/Unit/HeaderPanelPluginModeTests.cpp` | **New** (optional) |
| `CMakeLists.txt` | Register new sources/tests |

### Testing

| Test | Covers |
|------|--------|
| `SessionPersistencePolicyTests` | Patch param strip when ports configured |
| `DeviceMemoryLimitsTests` | No regression |
| `PatchManagerActionHandlerTests` | No regression |
| Manual UAT | Plugin HOST; footer identity after inquiry; LEGATO grayed @ UNISON; paste footer |
| Reference | `manual-uat-epic2-midi-audio-hardware.md` — plugin KEYBOARD FROM HOST |

### Previous story intelligence

**7.7 (Settings):** Phase A moved controls off header; logo popup later became skin/scale SSOT (7.10). Do not re-migrate skin/scale to Settings.

**7.10 (About):** Logo popup column width 80px; SETTINGS... / ABOUT... entries — preserve.

**7.6 (Internal Patches):** ROM footer on click + hover pattern — canonical for grayed controls; reuse helper, don't rewrite logic.

**8.5 (Device limits):** Bank Utility graying depends on `deviceDetected` + `deviceType`; footer-on-click was a review fix — migrate to helper.

**2.9b (Header routing):** Port persistence via APVTS; plugin HOST display was AC #6 — **regressed**, fix in T1.

### Git intelligence (recent commits)

- `7-6` closed with footer message fixes and `ActionExecutionHooks` init
- `PluginDisplayNames.h` modified in working tree — coordinate with any new constants
- CI/build gate available (Epic 11 done) — run full test target before review

### Epic / FR traceability

| FR | Story coverage |
|----|----------------|
| FR-3 | AC 6 |
| FR-39 | AC 1 |
| FR-41 | AC 2 (logo popup SSOT) |
| FR-42 | AC 2 |
| FR-43 | AC 2 |
| FR-44 | AC 3, 5 |
| FR-45 | AC 1, 5 |
| FR-53 | AC 4 (display; inquiry in Epic 8) |
| UX-DR7 | AC 2 |
| UX-DR8 | AC 2 |

### References

- [epics.md § Story 7.8]
- [prd.md §4.1 FR-3, §4.11–4.12 FR-39–45, FR-53]
- [architecture.md AD-7, § Exception → footer]
- [2-9b-header-routing-controls-uat-slice.md] — HOST grayed AC
- [7-6-internal-patches-panel-wiring.md] — ROM grayed footer pattern
- [epic-ui-scale-audit-pixel-perfect-layout.md] — U-3/U-4 layout vs 7.8 functional split
- [deferred-work.md] — ActivityLed cleanup, grayed-button API debt

## Dev Agent Record

### Agent Model Used

{{agent_model_name_version}}

### Debug Log References

### Completion Notes List

- T1: Plugin mode shows KEYBOARD FROM packet with fixed disabled HOST combo; standalone behaviour unchanged.
- T2: Footer two-zone layout with device identity from APVTS; severity colours moved to skin tokens.
- T3: Added `GrayedControlHelper`; migrated Bank Utility, paste buttons, and LEGATO PORTA @ UNISON constraint.
- T4: `SessionPersistencePolicy` strips patch/master/matrix-mod PARAM nodes on session load when both MIDI ports are configured; prefs preserved. `requestCurrentPatch()` post-inquiry hook deferred — no production call sites yet (Story 6-13 note).
- T5: Removed redundant `Led` width/height members; logo popup skin/scale wiring untouched. All unit tests green; Standalone + VST3 build green.
- Deferred: `juce::MidiDeviceListConnection` hot-plug refresh (AC1 optional).

### File List

- CMakeLists.txt
- Source/Core/PluginProcessor.cpp
- Source/Core/Services/SessionPersistencePolicy.h
- Source/Core/Services/SessionPersistencePolicy.cpp
- Source/GUI/Helpers/GrayedControlHelper.h
- Source/GUI/Helpers/GrayedControlHelper.cpp
- Source/GUI/Layout/Design/DesignPanels.h
- Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.h
- Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp
- Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h
- Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/RampPortamentoPanel.h
- Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/RampPortamentoPanel.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/MatrixModulationPanel/MatrixModulationPanel.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/BankUtilityPanel.cpp
- Source/GUI/Skins/SkinValues.h
- Source/GUI/Skins/SkinColoursWidgetsLayout.h
- Source/GUI/Skins/Skin.cpp
- Source/GUI/Widgets/Led.h
- Source/GUI/Widgets/Led.cpp
- Source/GUI/Widgets/ModuleHeader.cpp
- Source/GUI/Widgets/ParameterCell.h
- Source/Shared/Definitions/PluginDisplayNames.h
- Tests/Unit/SessionPersistencePolicyTests.cpp
- _bmad-output/implementation-artifacts/sprint-status.yaml

## Change Log

- 2026-07-14: Story created (ready-for-dev) — shell persistence, footer identity, FR-45 graying matrix, 2-9b HOST regression fix.
- 2026-07-14: Implementation complete — header HOST fix, footer identity/severity, GrayedControlHelper, FR-3 session strip policy, builds/tests green.
