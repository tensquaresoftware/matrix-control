---
title: 'Window and modal look strategy'
type: 'feature'
created: '2026-09-29'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '201e7f059201df22f2390d48b140a7f4026882af'
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-system-style-confirmation-modals.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-confirmation-modal-button-order.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Standalone title chrome and modal dialogs mix three looks (OS-native, stock JUCE, Matrix-Control overlays), which feels unprofessional; the earlier native title-bar switch was driven by multi-monitor drag failures with JUCE’s custom bar on mixed-DPI Mac setups.

**Approach:** Keep the OS-native standalone title bar on all platforms (no Luthier spike). Restyle product dialogs into Matrix-Control chrome (FileChooser stays OS) with embedded Montserrat for readable lowercase body text. All action buttons in those Matrix windows/modals use `TSS::Button` (not stock `juce::TextButton`), with uppercase labels and PT Sans like the main GUI. Replace the stock JUCE Audio/MIDI Settings dialog with a Matrix-skinned dialog that keeps audio device controls, a Matrix Test button, and a PeakIndicator meter; hide MIDI sections and Feedback Loop / Mute audio input. Investigate sample-rate / buffer persistence during that wrap and fix if low-risk. Confirm Windows/Linux title-bar behaviour on the Lenovo when available.

**Decisions locked:**
- TITLE_BAR = KEEP_NATIVE (macOS verified; Windows/Linux smoke on Lenovo soon)
- MODAL_POLICY = ALL_MATRIX_DIALOGS (product confirms + Audio/MIDI + Mutator Delete → Matrix chrome; FileChooser stays OS)
- SPIKE_PROCESS = DECIDE_NO_SPIKE
- AUDIO_MIDI_SETTINGS = WRAP_MATRIX simplified (no Active MIDI inputs / MIDI Output; no Feedback Loop / Mute audio input / blue mute banner)
- MODAL_BODY_FONT = SECOND_TYPEFACE → **Montserrat** (SIL OFL 1.1; embed Regular/Bold; titles may stay PT Sans caps/bold; may swap later if readability/spirit fails)
- MODAL_BUTTONS = MATRIX_BUTTON_CAPS (every product dialog/window action control is `TSS::Button` + uppercase label + PT Sans via existing button Look; no stock JUCE `TextButton` chrome in Matrix modals)
- AUDIO_DIAGNOSTICS = KEEP_BOTH with Matrix `Button` for Test and reuse existing `TSS::PeakIndicator` (no new meter class unless PeakIndicator cannot meet dialog layout)
- SAMPLE_RATE_PERSISTENCE = INVESTIGATE_THEN_FIX

## Boundaries & Constraints

**Always:**
- Standalone must remain draggable onto an external display — verified on macOS; re-verify on Windows/Linux when Lenovo testing starts.
- Keep existing confirm semantic codes and LTR Cancel → [middle] → primary / Enter=primary / Escape=cancel.
- Keep sync modal-gate contracts in Core; remap UI only in the editor helper layer.
- File pickers stay `juce::FileChooser` (OS-native where the OS allows).
- Existing Matrix overlays stay Matrix-Control chrome; new Matrix dialogs match that family.
- Modal body text uses embedded Montserrat on all three OSes (same glyphs everywhere); keep OFL notice with font assets.
- Product modal/window buttons use `TSS::Button` (Settings-style), uppercase English labels (align `PluginDisplayNames` with GUI caps habit), PT Sans from button Look — including existing dialogs that still use `juce::TextButton` today.
- Audio/MIDI dialog (standalone): audio device UI + Matrix Test + PeakIndicator; no MIDI sections; no Feedback Loop / Mute checkbox; MIDI ports stay in Settings / DEVICE SETUP.
- English UI strings via `PluginDisplayNames`; ASCII-only display literals.

**Never:**
- Revert standalone to a JUCE-drawn title bar in this story.
- Theme the OS-native title bar colour via stock JUCE LookAndFeel.
- Depend on Menlo or other OS-only system fonts for modal body.
- Leave stock `juce::TextButton` / JUCE LookAndFeel button chrome on Matrix product dialogs (Master Init, Defrag, bank progress Cancel, DEVICE SETUP, M1km choice, restyled confirms, Audio/MIDI Test, etc.).
- Blindly force Windows TaskDialog native alerts when that breaks LTR + Enter=primary.
- Restyle plugin host chrome (plugin window title is host-owned).
- Show duplicate MIDI device pickers in Audio/MIDI Settings.
- Expand into unrelated Look/skin polish outside window/modal chrome and the agreed Audio/MIDI simplification.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Standalone drag to external | Mixed-DPI Mac (and later Win/Linux) | Window rests on second display | Snap-back → regression on native title bar path |
| Confirm / Delete gates | Product confirms | Matrix chrome; Montserrat body; `TSS::Button` caps/PT Sans; same semantic codes | Escape / dismiss → cancel |
| Existing Matrix dialogs | Master Init, Defrag, bank progress, DEVICE SETUP, M1km, … | Same overlay family; buttons upgraded to Matrix caps/PT Sans | Escape / dismiss unchanged |
| File open/save/folder | FileChooser helper | OS picker; Matrix UI raised before/after | Cancel → empty File |
| Audio/MIDI Settings | Standalone open | Matrix chrome; audio only; Test = Matrix Button caps; meter = PeakIndicator; no MIDI / Feedback Loop / Mute / blue banner | Close without apply → no unintended device change beyond current semantics |
| Switch audio interface | Change output/input device | Investigate sample rate + buffer persistence; fix if low-risk else document defer | Prefer last user-chosen rate/buffer when device still supports them |
| AUDIO FROM None | Header combo None | Software silence without Mute checkbox | N/A |
| Plugin host | Plugin format | No standalone title-bar change; in-editor Matrix dialogs follow modal policy | N/A |

</frozen-after-approval>

## Code Map

- `Source/Standalone/MatrixControlStandaloneFilterWindow.h` — keep native title bar + null constrainer
- `Source/GUI/PluginEditorAlerts.cpp` / `PluginEditorInternal.h` — confirms / Mutator Delete → Matrix restyle
- `Source/GUI/Settings/SettingsWindow.*`, `About/AboutWindow.*`, `Dialogs/*` — Matrix dialog patterns
- `Source/GUI/Skins/Skin.cpp`, `CMakeLists.txt` `PluginFonts`, `Assets/Fonts` — embed Montserrat Regular/Bold + OFL notice beside PT Sans / Orbitron
- `Source/GUI/Widgets/PeakIndicator.*`, `HeaderPanel.*` — reuse meter; feed from device/passthrough peak path as appropriate
- `Source/GUI/Widgets/Button.*`, `SettingsPanel::makeButton` — canonical Matrix button (caps + PT Sans Look)
- `Source/GUI/Dialogs/*` — today several still use `juce::TextButton`; migrate to `TSS::Button`
- `Source/Core/Audio/StandaloneAudioInputRouterStandalone.cpp` — replace `showAudioSettingsDialog()`; muteInput helpers
- `Source/Core/PluginProcessorAudio.cpp` — AUDIO FROM ↔ muteInput
- JUCE 9 `AudioDeviceSelectorComponent` / `StandaloneFilterWindow` — sections to omit; device setup persistence hooks

## Tasks & Acceptance

**Execution:**
- [x] `Source/Standalone/MatrixControlStandaloneFilterWindow.h` — confirm KEEP_NATIVE remains — multi-monitor invariant
- [x] `Assets/Fonts` + `CMakeLists.txt` `PluginFonts` + `Skin.cpp` — embed Montserrat Regular/Bold + OFL; expose modal body font API — readable lowercase on all OSes
- [x] Matrix dialog chrome — restyle product confirms / Mutator Delete to Matrix overlays; Montserrat body; all actions via `TSS::Button` uppercase/PT Sans — visual coherence
- [x] `Source/GUI/Dialogs/*` (+ DEVICE SETUP if in scope) — replace remaining `juce::TextButton` with `TSS::Button` + caps labels in `PluginDisplayNames` — end mixed JUCE/Matrix button look
- [x] Audio/MIDI Matrix dialog — wrap/rebuild selector UI without MIDI / Feedback Loop / Mute; Matrix Test `TSS::Button`; PeakIndicator; wire show path from `StandaloneAudioInputRouterStandalone.cpp` — remove stock-JUCE sore thumb
- [x] Sample rate / buffer persistence — diagnose on device switch; fix if low-risk else note defer in Design Notes — reduce painful resets
- [ ] Manual smoke — Mac external drag; Matrix confirm readability; FileChooser; simplified Audio/MIDI Test+meter; Lenovo checklist — verify product intent

**Acceptance Criteria:**
- Given standalone on macOS (and later Win/Linux), when dragged to an external display, then it can rest there without snap-back.
- Given product confirm / Mutator Delete gates, when shown, then Matrix-Control chrome uses Montserrat for body text, `TSS::Button` with uppercase PT Sans labels, and unchanged semantic codes.
- Given existing Matrix dialogs that still show stock JUCE buttons today, when this story ships, then those actions use Matrix `TSS::Button` uppercase/PT Sans instead.
- Given FileChooser flows, when used, then OS picker chrome remains.
- Given Audio/MIDI Settings on standalone, when opened, then Matrix chrome shows audio controls, Matrix Test (`TSS::Button`), and PeakIndicator, without MIDI sections and without Feedback Loop / Mute UI or its blue banner.
- Given AUDIO FROM None, when selected, then audio input stays silent without that Mute checkbox.
- Given an audio interface switch, when sample rate / buffer persistence is investigated, then either a low-risk fix ships or a documented defer remains in Design Notes.

## Implementation Notes

## Spec Change Log

- 2026-09-29 review: MODAL_BUTTONS — require `TSS::Button` + uppercase + PT Sans on all Matrix product dialog/window actions; migrate existing `juce::TextButton` dialogs; avoid mixed JUCE/Matrix button chrome.

## Review Triage Log

- verdict: high | evidence: `changeListenerCallback` always calls `restorePreferredSetupIfNeeded` before capture, so a same-device sample-rate/buffer edit is reverted to the previous preferred values. Route: patch (restore only on device identity change).
- verdict: medium | evidence: `BankTransferProgressDialog::resized` lays Cancel at 72 while `makeButton` builds width 88 — label can clip. Route: patch.
- verdict: medium | evidence: `closeAudioMidiSettingsWindow` only `setVisible(false)`; 30 Hz timer and device listener keep running while hidden. Route: patch (destroy or deactivate on close).
- verdict: medium | evidence: `openAudioMidiSettingsWindow` closes Settings/About/Defrag but not Master Init / M1km / DEVICE SETUP / bank progress — overlays can stack. Route: patch.
- verdict: medium | evidence: `usesMacOsNativeAlertButtonOrder` / `configureOrderedAlertButtons` have no callers after Matrix confirm restyle. Route: patch (delete dead helpers).
- verdict: medium | evidence: verification-gap — preferred restore + MIDI flags tested only as free helpers, not as restore-then-capture / device-change orchestration. Route: patch (Core orchestrator + extend unit tests).
- verdict: false | evidence: stock `AudioDeviceSelectorComponent` inside Matrix frame is the frozen WRAP_MATRIX approach; only Test must be `TSS::Button`.
- verdict: false | evidence: empty-handler path that only clears mute (no stock dialog) matches the decision to replace `showAudioSettingsDialog`, not leave a stock fallback.
- verdict: low | evidence: unused `getModalBodyFontBold` — frozen intent allows titles to stay PT Sans; API reserved. Rejected (cosmetic).
- verdict: low | evidence: `iconType` unused on Matrix confirms — no Matrix icon chrome in scope. Rejected.
- verdict: low | evidence: Mutator Delete `juce::ToggleButton` don't-ask-again — not an action `TSS::Button`; Matrix toggle would add surface. Rejected for this story.
- verdict: low | evidence: BankTransfer custom paint vs shared `paintMatrixOverlayChrome` — already Matrix family chrome; forced refactor > direct fix. Rejected.
- verdict: low | evidence: Black skin fallback / fixed dialog heights / estimateButtonWidth / double typeface build — unlikely everyday harm or cosmetic. Rejected.
- verdict: maybe-false | evidence: second-editor handler clear / null Config in release / Montserrat typeface null — not demonstrated on the single-editor standalone path. Rejected.
- verdict: medium (unverified automation gap) | evidence: verification-gap — standalone show path and Matrix confirm semantic codes lack unit coverage by project policy (stubs / no GUI modal tests). Route: defer.

## Design Notes

KEEP_NATIVE locked; no spike. Mute audio input toggles `StandalonePluginHolder` muteInput and can show a sticky blue banner; AUDIO FROM already drives monitoring. Standalone editor startup now calls `enableInputMonitoring()` so muteInput stays false and the banner stays hidden; silence when AUDIO FROM is None remains software passthrough-off. `TSS::PeakIndicator` already exists for the header (D-071) — reused in Audio Settings. Modal body face = Montserrat (OFL); titles stay PT Sans caps/bold. Product dialog actions use `TSS::Button` + uppercase `PluginDisplayNames`.

### Sample rate / buffer persistence (INVESTIGATE_THEN_FIX)

**Shipped low-risk fix:** while the Matrix Audio Settings overlay is open, capture live sample rate / buffer as preferred values; on **audio device identity change** (not same-device rate/buffer edits), restore them when the new device still lists those values (`Core::planPreferredSetupChange` + `AudioMidiSettingsWindow`). Unit coverage: `Tests/Unit/AudioDevicePreferredSetupTests.cpp`.

**Remaining risk:** restore applies while the overlay is open. Cross-session / closed-dialog device switches still follow JUCE `AudioDeviceManager` defaults. Validate on real multi-interface hardware (Mac + Lenovo).

### I/O matrix coverage

| Matrix row | Automated coverage | Manual / note |
|------------|--------------------|---------------|
| Standalone drag to external | — (OS windowing; CONVENTIONS: no GUI E2E) | Mac smoke + Lenovo checklist |
| Confirm / Delete gates | Core modal-gate tests (`MutatorActionHandler*`); semantic codes 0/1/2 unchanged | Chrome / Montserrat / caps visual smoke |
| Existing Matrix dialogs | — (GUI chrome) | Visual smoke Master Init / Defrag / DEVICE SETUP / M1km / bank progress |
| FileChooser | — (path still `juce::FileChooser`) | Smoke open/save/folder |
| Audio/MIDI Settings | `AudioDevicePreferredSetupTests` (MIDI flags off; advanced kept) | Visual: no MIDI/mute/banner; Test + PeakIndicator |
| Switch audio interface | `AudioDevicePreferredSetupTests` restore decisions | Multi-device hardware smoke |
| AUDIO FROM None | `AudioFromSourceSyncTests` (empty source → silence) | Confirm mute checkbox absent |
| Plugin host | — (no standalone title-bar path in plugin) | In-editor Matrix dialogs only |

### Lenovo checklist (Windows / Linux — deferred)

- [ ] Native title bar present (not JUCE-drawn)
- [ ] Drag to second display rests without snap-back
- [ ] Matrix confirm: Montserrat body, caps buttons, Enter=primary / Escape=cancel / LTR
- [ ] FileChooser remains OS chrome
- [ ] Audio Settings: no MIDI / mute / blue banner; Test + PeakIndicator
- [ ] Sample rate / buffer restore on interface switch while dialog open

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds after code change
- `cmake --preset macos-debug-arm64 -DMATRIX_BUILD_TESTS=ON && cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests`
- `./Builds/macOS/ARM/Debug/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests --category AudioDevicePreferredSetup --category AudioFromSourceSync` -- expected: 0 failures
- `python3 Scripts/quality/lint_touched.py` -- expected: pass on touched `Source/` C++

**Manual checks (if no CLI):**
- Mac: drag standalone to external LG via native title bar
- Matrix confirm body lowercase readability; Audio/MIDI sections and Test/meter
- Lenovo later: title-bar + multi-monitor + Audio/MIDI on Windows (Linux if in that pass)
