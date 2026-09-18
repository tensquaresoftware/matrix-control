---
title: 'DEVICE SETUP first-connection assistant'
type: 'feature'
created: '2026-09-18'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
context: []
baseline_commit: '851445a5'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The one-time EPROM TYPE modal only opens after a successful Device Inquiry. On first launch MIDI FROM/TO are often unset, so detection and any firmware suggestion are missing or misleading.

**Approach:** Replace that modal with a four-row DEVICE SETUP assistant that picks MIDI ports (same APVTS as the header), shows a read-only DEVICE row in footer chrome (with SEARCHING animation while Inquiry runs), lets the user choose EPROM TYPE, then CONFIRM or SPECIFY LATER — still a one-shot via `settingsEpromTypePromptDone`.

## Boundaries & Constraints

**Always:**
- Four rows: MIDI FROM, MIDI TO, DEVICE (read-only, footer-aligned chrome), EPROM TYPE (label + combo).
- Modal title: `DEVICE SETUP`. EPROM row label stays `EPROM TYPE`. Buttons: **CONFIRM** / **SPECIFY LATER** (ASCII in `PluginDisplayNames`).
- Open when the editor is ready and `settingsEpromTypePromptDone` is false (first useful occasion), even if MIDI ports are empty — do not wait for Inquiry success to show the assistant.
- MIDI ports write the same APVTS keys as the header (`midiInputPortId` / `midiOutputPortId`) via the same processor setters so Device Inquiry refreshes after port sync.
- DEVICE row states (live while dialog open):
  - Either MIDI FROM or MIDI TO empty (or either invalid): badge `DEVICE` on red fail chrome + detail `NOT CONNECTED` in footer grey (same colours as footer not-connected).
  - Both ports selected and detection not yet successful: same red badge + detail `SEARCHING` with animated ASCII dots cycling `.` → `..` → `...` on a short timer loop (shows the app is alive).
  - Inquiry success (`deviceDetected`, not unresponsive): badge OK chrome + type + `V` version via `formatDeviceVersionForDisplay` / `MatrixDeviceTypes::toDisplayString` (footer format).
  - Both ports selected but Inquiry fails or times out without detection: return to `NOT CONNECTED` (red badge + grey detail). Align the waiting window with existing Device Inquiry timeout (`SysExConstants::kDefaultTimeoutMs`, currently 2000 ms) rather than inventing a separate long delay.
- EPROM TYPE items stay device-family aware via `EpromTypePolicy` (M-1000 five items; 6/6R no GLIGLI). UNKNOWN = FACTORY capability (stock SysEx delay). Inquiry may only suggest a combo default when a clear version maps; never write EPROM until CONFIRM.
- CONFIRM allowed without a detected device (user may declare EPROM TYPE before cables work). Persists combo selection, sets `promptDone`, clears pending, refreshes SysEx delay.
- SPECIFY LATER (Esc / click-outside same): set `promptDone`, clear pending; keep any ports already chosen in the dialog; leave stored EPROM TYPE unchanged.
- Keep `settingsEpromTypePromptPending` ephemeral (strip on save). Do not treat pending as long-term persistence. Inquiry may still set pending as a secondary wake-up if the editor was not ready; primary open is first editor-ready with `promptDone` false.
- UI strings ASCII-only. Extend `EpromTypePromptDialog` (layout/title/strings) rather than inventing a parallel dialog class tree.
- Recompute dialog design height for four control rows + body + buttons; no Settings scroll / INTERFACE / Unison work.

**Never:**
- INTERFACE Settings / CONTEXTUAL HELP SHOW|HIDE.
- Unison Detune / CC 94 / Master Unison UX.
- Vertical Settings tabs or global Settings scroll.
- Changing SysEx delay semantics beyond the existing EPROM TYPE SSOT.
- Dual port SSOT (dialog must not keep a private port id separate from header properties).
- Silent overwrite of a user-chosen EPROM TYPE without CONFIRM.
- Blocking CONFIRM until detection succeeds.
- Re-prompt after SPECIFY LATER or CONFIRM while `promptDone` remains true.
- French UI strings; Nordcore Settings item / exotic firmware migration.
- Label the DEVICE row `DETECTED DEVICE` (use `DEVICE` like the footer).

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| First editor ready | `promptDone` false | Open DEVICE SETUP (4 rows); ports sync from header; DEVICE = NOT CONNECTED if ports empty | Skip if `promptDone` or dialog already visible |
| Only one port set | FROM or TO empty | DEVICE stays NOT CONNECTED (no SEARCHING) | N/A |
| Both ports set, Inquiry in flight | Ports non-empty, not detected | DEVICE = SEARCHING with animated dots | On fail/timeout → NOT CONNECTED |
| Inquiry success | `deviceDetected` | DEVICE = type + Vversion; EPROM combo may prefer mapped suggestion | Ambiguous version → no false suggestion |
| CONFIRM, no device | User picks EPROM | Persist EPROM; `promptDone`; clear pending; refresh delay | Allowed |
| CONFIRM, device ok | Selection | Same + suggestion may already be selected | N/A |
| SPECIFY LATER | Dismiss | `promptDone`; keep ports; EPROM unchanged | Esc / outside = SPECIFY LATER |
| Reconnect later | `promptDone` true | No assistant | N/A |
| Device family change while open | M-1000 ↔ 6/6R | Rebuild EPROM items; coerce invalid id to UNKNOWN | No crash |
| Plugin vs standalone | Fresh editor | Same contract; host-restored `promptDone` respected | Open after UI ready |

</frozen-after-approval>

## Code Map

- `Source/GUI/Dialogs/EpromTypePromptDialog.{h,cpp}` — four rows (MIDI FROM/TO, DEVICE chrome + SEARCHING timer animation, EPROM); grow `kDesignHeight`; title `DEVICE SETUP`; CONFIRM / SPECIFY LATER callbacks.
- `Source/GUI/PluginEditorWindows.cpp` — open/guard; port wiring to `setMidiInputPort` / `setMidiOutputPort`; live DEVICE state from APVTS + port emptiness; EPROM suggestion refresh; apply / mark-finished.
- `Source/GUI/PluginEditorAudio.cpp` / `PluginEditorUiConstruction.cpp` — primary open on editor ready when `!promptDone`; keep pending listener as secondary wake-up only.
- `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.*` — reuse port list population helpers; do not fork port SSOT.
- `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.*` — reference for badge/detail paint, fail chrome, connected detail format (dialog may paint a compact twin, not a second identity SSOT).
- `Source/Shared/Definitions/PluginDisplayNames.h` — `DEVICE SETUP` title/body; row labels `MIDI FROM` / `MIDI TO` / `DEVICE` / `EPROM TYPE`; `SEARCHING` base string (dots animated in code); reuse footer `NOT CONNECTED` / `DEVICE` where practical.
- `Source/Shared/Definitions/PluginIDs.h` + processor state — keep EPROM / promptDone / promptPending; ports `midiInputPortId` / `midiOutputPortId`.
- `Source/Core/Services/EpromTypePolicy.*` + tests — reuse; new pure helpers only if DEVICE-row state machine is extracted testably.
- `Source/Core/MIDI/MidiManagerDeviceInquiry.cpp` + `SysExConstants::kDefaultTimeoutMs` — Inquiry still drives detection; assistant SEARCHING window follows that timeout/outcome; do not require Inquiry success to open the dialog.
- `Source/Core/PluginProcessorMidiPorts.cpp` — port setters → `refreshDeviceInquiryAfterPortSync`.
- Do not change: Settings DEVICE section layout; Unison; INTERFACE deferred; SysEx delay SSOT rules already shipped.

## Tasks & Acceptance

**Execution:**
- [x] `PluginDisplayNames.h` — DEVICE SETUP strings (title, body, row labels, SEARCHING) ASCII.
- [x] `EpromTypePromptDialog.*` — four-row layout; MIDI combos; DEVICE chrome; animated SEARCHING dots; height metrics.
- [x] `PluginEditorWindows.cpp` (+ Audio / UiConstruction) — first-ready open when `!promptDone`; port sync; live DEVICE + EPROM suggestion; CONFIRM / SPECIFY LATER persistence per frozen rules.
- [x] `MidiManagerDeviceInquiry.cpp` — keep Inquiry detection; pending only as secondary wake-up; pending remains ephemeral on save.
- [x] Pure-logic unit tests if a DEVICE-row state helper is extracted; otherwise manual checks cover GUI animation.
- [x] `lint_touched.py` on touched C++.

**Acceptance Criteria:**
- Given `promptDone` false, when the editor becomes ready, then DEVICE SETUP opens once (guarded if already visible), even with empty ports.
- Given empty or partial ports, when viewing DEVICE, then red `DEVICE` badge + grey `NOT CONNECTED` (footer-aligned).
- Given both ports selected and no detection yet, when Inquiry is in flight, then DEVICE shows animated `SEARCHING` dots; on fail/timeout, then `NOT CONNECTED` again.
- Given Inquiry success, when APVTS updates, then DEVICE shows type + version and EPROM may prefer a mapped suggestion without writing until CONFIRM.
- Given CONFIRM with or without detection, when the dialog closes, then selected EPROM persists, `promptDone` is true, pending cleared, SysEx delay refreshed.
- Given SPECIFY LATER, when the dialog closes, then `promptDone` is true, ports kept, EPROM unchanged, and the assistant does not return.
- Given `promptDone` true, when Inquiry succeeds later, then no assistant.
- Given Matrix-6/6R vs Matrix-1000, when EPROM items rebuild, then GLIGLI rules match Settings.

### Review Findings

- [x] [Review][Decision] DEVICE row look — resolved (2026-09-18): keep Label + ReadOnlyValueField polish; footer badge twin not required for this assistant. AC chrome wording superseded by polish intent.
- [x] [Review][Decision] Family coerce while DEVICE SETUP open — resolved (2026-09-18): do not write stored EPROM while assistant is visible; combo rebuild/coerce on screen only until CONFIRM.
- [x] [Review][Patch] Skip APVTS EPROM coerce write while DEVICE SETUP dialog is open [Source/GUI/PluginEditorAudio.cpp:181]
- [x] [Review][Patch] Repaint body after live DEVICE updates so firmware suggestion suffix appears [Source/GUI/Dialogs/EpromTypePromptDialog.cpp:126]
- [x] [Review][Patch] Refresh DEVICE SETUP MIDI lists on OS hot-plug [Source/GUI/PluginEditorHeader.cpp:106]
- [x] [Review][Patch] Remove unused EditorOutboundGate includes in PluginEditorAudio/Windows [Source/GUI/PluginEditorAudio.cpp:9]
- [x] [Review][Patch] Reuse footer DEVICE label constant instead of dialog duplicate [Source/Shared/Definitions/PluginDisplayNames.h:315]
- [x] [Review][Patch] Finish Header adoption of shared MIDI port helpers (keyboard populate + local find/get twin) [Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp:20]
- [x] [Review][Patch] Unit-test SEARCHING window unresponsive path sets exhausted [Tests/Unit/DeviceSetupDeviceRowTests.cpp:159]
- [x] [Review][Patch] Unit-test exhaust then clear a port then same-pair restart [Tests/Unit/DeviceSetupDeviceRowTests.cpp:95]
- [x] [Review][Defer] Editor-ready open wiring only covered by pure bool helper [Source/GUI/PluginEditorUiConstruction.cpp] — deferred: repo unit tests stay on Core/pure helpers; PluginEditor open path needs an editor/integration harness beyond this chantier's test style.
- [x] [Review][Defer] Packed Inquiry version normalize duplicated in Core and GUI helper [Source/Core/Services/EpromTypePolicy.cpp:10] — deferred: extracting a Shared helper is more than a direct patch; display and suggestion currently match.

#### Rejected

- Blind: no SEARCHING retry without port change — false: exhaust-until-port-pair-change is intentional after prior SEARCHING window patch; timeout → NOT CONNECTED matches frozen AC.
- Blind: suggestion refresh before coerce leaves invalid combo selection — false: `populateComboItems` already coerces the preferred id for the new family before selecting.
- Edge: Inquiry version newlines / non-space whitespace mangling — low: unlikely everyday Inquiry tokens; fix would add guards beyond observed traffic.
- Edge: prior `trim()` contract removed — low: space/tab strip covers the packed-version cases this polish targets; other whitespace is rare.
- VG Other: `testConfirmPersistsEpromAndFinishes` name overclaims APVTS writes — low: helper-only assert is clear enough; rename alone is not worth a patch pass.

## Implementation Notes

Human decisions (2026-09-18): keep full spec; trigger = first editor ready when `!promptDone`; SPECIFY LATER keeps ports / leaves EPROM; CONFIRM allowed without device; DEVICE row (not DETECTED DEVICE) with SEARCHING animated dots; title `DEVICE SETUP`.

Agent default: SEARCHING wait aligns with `SysExConstants::kDefaultTimeoutMs` (2 s) / Inquiry failure path — not a separate longer UI-only timer.

Post-implement hardening: `deviceSetupConfirmResult` / `deviceSetupSpecifyLaterFlags` drive finish persistence; EPROM combo suggestion refresh preserves a user-touched selection (still rebuilds items on family change via coerce); DeviceSetupDeviceRow unit tests cover matrix rows for open/DEVICE states/CONFIRM/LATER/family coerce.

## Spec Change Log

## Review Triage Log

- Blind: SEARCHING is a private 2 s UI clock not Inquiry-in-flight — **medium** — verified: frozen allows SEARCHING when both ports set and not detected; agent note aligns timeout to `kDefaultTimeoutMs`. Real gap is open/start without ensuring Inquiry runs (grouped with Edge open/kick). Route: patch (grouped).
- Blind: combo-selected ≠ port open/usable — **medium** — verified: readiness uses non-empty combo id; Inquiry still refuses closed ports. Everyday case is selecting a real OS port which then opens via setters. Route: defer (pre-existing open semantics; dialog mirrors header selection model).
- Blind: HeaderPanel still forks port population — **medium** — verified `HeaderPanel.cpp` still has private populate* for MIDI FROM/TO. Route: patch.
- Blind: body suggestion hint not refreshed live — **medium** — verified `includeFirmwareSuggestionHint_` set only in `prepareForShow`. Route: patch.
- Blind: MIDI lists not refreshed on hot-plug while open — **medium** — verified `populateMidiPortLists` only in `prepareForShow`. Route: patch.
- Blind: unused `EditorOutboundGate.h` includes in Audio/Windows — **low** — verified includes present with no symbols used. Route: patch (delete).
- Blind: duplicate `kDeviceLabel` vs footer — **low** — verified second constant; everyday harm low but direct reuse is trivial. Route: patch.
- Blind: Connected + `identityOk` false for unsupported type — **false** — footer-aligned UNKNOWN detail with fail chrome is intentional via `buildDeviceSetupConnectedDetail`.
- Blind: unresponsive still shows SEARCHING while window active — **medium** — verified resolver prefers SEARCHING over NOT CONNECTED when `deviceMidiUnresponsive` and window active; footer maps unresponsive to not-connected. Route: patch.
- Blind: thin integration tests / SEARCHING without Inquiry — **medium** — overlaps VG SEARCHING lifecycle + Edge kick Inquiry. Route: patch (grouped).
- Blind: `syncPortSelections` on every device-status tick — **medium** — verified `refreshEpromTypePromptDialogLiveState` always syncs ports. Route: patch.
- Blind: CONFIRM vs LATER finish call-style split — **false** — both use `deviceSetupConfirmResult` / `deviceSetupSpecifyLaterFlags` helpers.
- Blind: MidiManagerInquiry not touched despite task checkbox — **false** — task is keep detection + secondary pending; no Core change required.
- Edge: port change after SEARCHING exhausted while both ports stay ready — **high** — verified `searchingWindowExhausted_` only clears when a port becomes empty or identity OK; changing From A→B keeps exhausted true so SEARCHING never restarts. Route: patch.
- Edge: port change while SEARCHING active does not reset timer — **high** — same root as exhausted restart (port-pair change must restart window). Route: patch (grouped).
- Edge: MIDI lists stale after prepareForShow — **medium** — duplicate of Blind hot-plug. Route: patch (grouped).
- Edge: open with both ports set / undetected without kicking Inquiry — **high** — verified SEARCHING can start without `refreshDeviceInquiryAfterPortSync` from dialog open. Route: patch.
- Edge: SEARCHING claim vs Inquiry-in-flight — **medium** — claim kind; frozen text is ports+undetected; still need Inquiry kick on window start. Route: patch (grouped with Edge open/kick).
- VG: editor-ready open call-site — **medium** — pre-verified gap; UiConstruction uses `!promptDone` but tests only hit pure guard. Route: patch (wire UiConstruction through `shouldOpenDeviceSetupAssistant`).
- VG: SEARCHING start/timeout phantom test — **medium** — pre-verified; extract pure window helper + tests. Route: patch.
- VG: MidiPortComboPopulation untested — **medium** — pre-verified. Route: patch.
- VG: HeaderPanel not adopting shared helper — **medium** — duplicate Blind Header fork. Route: patch (grouped).
- VG: user-touched EPROM refresh untested — **medium** — pre-verified; extract pure next-id helper + test. Route: patch.
- VG: overclaiming test names — **low** — real but everyday suite harm limited; rename/narrow in same test pass. Route: patch.
- VG Other: unused EditorOutboundGate includes — **low** — duplicate Blind. Route: patch (grouped).

## Design Notes

Button order stays LTR: SPECIFY LATER → CONFIRM.

Animated dots: cycle three frames (`.`, `..`, `...`) on a ~400–500 ms timer while in SEARCHING; stop the timer in other DEVICE states and on dialog hide.

Partial ports (only FROM or only TO) never enter SEARCHING — stay NOT CONNECTED until both are set.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` — expected: existing EpromTypePolicy / delay tests green; new pure-logic tests green if added
- `python3 Scripts/quality/lint_touched.py` — expected: no findings on touched C++

**Manual checks:**
- Fresh `promptDone` false: assistant opens at editor ready; empty ports → NOT CONNECTED.
- Set both ports: SEARCHING animation; synth present → identity + optional suggestion; CONFIRM persists; no re-open.
- Both ports, no synth / timeout → NOT CONNECTED; CONFIRM still works; SPECIFY LATER keeps ports.
- Plugin and standalone first-launch paths.
