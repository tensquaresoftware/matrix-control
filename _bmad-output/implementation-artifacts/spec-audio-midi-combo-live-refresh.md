---
title: 'Audio / MIDI combo live refresh while open'
type: 'feature'
created: '2026-09-24'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'ea3c2fb0fdb496d3c89ec69464e44f63173971ae'
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** In Standalone, header MIDI and Audio From combo lists already refresh when devices appear or disappear while the menus are closed, but an open dropdown keeps a stale layout and can feel broken until the user closes and reopens it. That loses the fluid hot-plug habit people get from macOS system menus.

**Approach:** Keep the existing OS device-list callbacks. When a refresh runs and an in-scope combo popup is open, rebuild the item list, dismiss the stale popup, and immediately reopen it with the new list so hot-plug feels live on macOS, Windows, and Linux.

**Decisions:**
- Open-menu UX: dismiss then immediately reopen the same combo with the refreshed list (not in-place relayout; not dismiss-only).
- Surface scope: header Standalone combos MIDI FROM, MIDI TO, KEYBOARD FROM, AUDIO FROM, plus DEVICE SETUP / EPROM MIDI port combos that already share the MIDI refresh path. Stock JUCE Audio/MIDI Settings dialog is out of scope.

## Boundaries & Constraints

**Always:**
- Refresh from the existing Standalone paths: `MidiDeviceListConnection` for MIDI ports and `AudioDeviceManager` `ChangeListener` for Audio From.
- Preserve selection by stable MIDI device `identifier` and by Audio From source id when the device/channel still exists.
- Soft MIDI port sync and inquiry behaviour stay unchanged.
- Works in Standalone on macOS, Windows, and Linux via JUCE device notifications.
- When an in-scope combo’s popup is open during refresh: rebuild items, dismiss, immediately reopen.

**Never:**
- Rewrite JUCE `StandalonePluginHolder` Audio/MIDI Settings dialog internals.
- Invent a second device-list subscription beside the existing MIDI connection / audio ChangeListener.
- Change hosted-plugin Audio From channel-mode semantics or invent hardware open in plugin mode.
- Switch Audio From to a full-system device scan; it remains the active device’s enabled channel catalog.
- Redesign ComboBox skins, hierarchical menus, or reclick-to-close behaviour beyond what live refresh requires.
- Implement in-place popup relayout for this story.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Hot-plug MIDI while closed | New MIDI device appears; header MIDI combo closed | Next open shows new device; closed label/selection stay valid if previous id still present | Missing previous id → sentinel / empty selection per existing soft sync |
| Hot-plug MIDI while open | MIDI FROM/TO/KEYBOARD FROM (or shared DEVICE SETUP / EPROM) popup open; device appears or disappears | Stale popup dismisses; same combo reopens immediately with updated list; selection preserved when id still present | Disappeared selected device → existing soft-clear / sentinel path after refresh |
| Audio device change while open | Standalone Audio From popup open; `AudioDeviceManager` change | Catalog rebuilt; popup dismisses and reopens with updated list | Empty catalog → `NO INPUT` sentinel only |
| Rapid churn | Several connect/disconnect events in a short window | Message-thread refreshes coalesce to a coherent final open list; no crash or stuck modal | Ignore duplicate no-op rebuilds when item set unchanged |

</frozen-after-approval>

## Code Map

- `Source/Core/PluginProcessorMidiPorts.cpp` (`installMidiDeviceListConnection`, `handleMidiDeviceListChanged`) — existing MIDI hot-plug → message-thread hop → editor refresh.
- `Source/GUI/PluginEditorHeader.cpp` (`refreshMidiPortListsFromOsChange`) — header MIDI repopulate + APVTS reselect; also refreshes EPROM/DEVICE SETUP ports when open.
- `Source/GUI/Helpers/MidiPortComboPopulation.h` — shared `clear` + rebuild from `getAvailableDevices()`; keep selection by `identifier`.
- `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp` (`populateMidiPortLists`, `populateAudioFromCombo`) — header item rebuild; currently no dismiss/rebuild of an open popup.
- `Source/GUI/PluginEditorAudio.cpp` (`refreshAudioFromCombo`, `changeListenerCallback`) — Audio From refresh on `AudioDeviceManager` change.
- `Source/Core/Audio/AudioInputSourceCatalog.cpp` + `StandaloneAudioInputRouterStandalone.cpp` — active-device channel catalog only (not full system scan).
- `Source/GUI/Widgets/ComboBox.cpp` / `ScrollablePopupMenu.cpp` / `PopupMenuBase.*` — ButtonLike popup; ctor snapshots height from `getNumItems()`; paint/hit-test re-read live → unsafe `clear`/`addItem` while open without dismiss or relayout API.
- `Source/GUI/Widgets/MultiColumnPopupMenu.cpp` — same family if any in-scope combo uses it; touch only if needed.
- `_bmad-output/implementation-artifacts/deferred-work.md` — stale “audio-from combo not refreshed on device change” note (listener already exists); do not treat as this story’s primary gap.

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Widgets/ComboBox.*` / `ScrollablePopupMenu.*` / `PopupMenuBase.*` -- Add dismiss-then-immediately-reopen for a ButtonLike popup after items were rebuilt -- Avoid stale geometry after `clear`/`addItem` while a menu was showing.
- [x] `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp` (+ helpers if needed) -- After MIDI/Audio populate, if that combo’s popup was open, dismiss and reopen -- Header (and shared MIDI dialog) hot-plug while open is the user-visible goal.
- [x] `Source/GUI/PluginEditorHeader.cpp` / `Source/GUI/PluginEditorAudio.cpp` -- Ensure OS-driven refresh calls still reselect from APVTS and remain message-thread only -- Preserve soft sync and selection.
- [x] `Tests/` (unit or GUI harness as feasible) -- Cover dismiss-then-reopen contract without requiring real hardware -- Lock the regression.
- [x] Manual Standalone UAT note in Implementation Notes -- Plug/unplug MIDI and audio interfaces with each in-scope combo open on macOS (and Windows/Linux when available) -- Cross-platform confirmation.

**Acceptance Criteria:**
- Given Standalone with a header MIDI combo popup open, when a MIDI interface or keyboard is connected or disconnected, then the popup dismisses and reopens immediately with the updated device list.
- Given Standalone with Audio From open, when the active audio device/channel map changes, then the popup dismisses and reopens immediately with the updated catalog.
- Given the same hot-plug with menus closed, when the user later opens a combo, then the list is already current (existing behaviour preserved).
- Given a previously selected device id that still exists after refresh, when the list rebuilds, then that selection remains selected.
- Given hosted plugin mode, when MIDI ports change, then header MIDI refresh behaviour stays coherent; Audio From remains host channel-mode only (no hardware device manager path).
- Given the stock JUCE Audio/MIDI Settings dialog, when devices hot-plug, then this story does not require custom live-refresh behaviour there.

## Implementation Notes

### What shipped
- `ComboBox` tracks the active modal popup (`attachOpenPopup` / `dismissPopup`), coalesces rapid async reopens via `popupShowGeneration_`, and exposes `isPopupOpen()` / `showPopupAfterItemRebuild()` (skips `onAboutToShowPopup` on live reopen).
- Shared helper `Source/GUI/Helpers/ComboBoxLiveRefresh.h`: pure `planRefresh` / `identifiersEqual` plus `rebuildPreservingOpenPopup` (dismiss → rebuild → `showPopupAfterItemRebuild`).
- MIDI populate (`MidiPortComboPopulation`) and Audio From populate (`HeaderPanel`) use that path; unchanged item ids+labels+count skip rebuild so rapid churn does not flicker; DEVICE SETUP / EPROM MIDI combos inherit via shared populate.
- OS MIDI list change: `refreshMidiPortListsFromOsChange(true)` soft-revalidates ports (force-reopen only when presence says not live; clear only on reopen failure). Open-menu poll uses `refreshMidiPortListsFromOsChange(false)` — list rebuild only.
- Apple: `MidiDevicePresence` filters offline CoreMIDI endpoints; non-Apple fail-open pass-through (no exclusive probes).

### Verification
- `cmake --build --preset macos-debug-arm64` — success
- `Matrix-Control_Tests --test ComboBoxLiveRefresh --test MidiPortComboPopulation --test MidiPortStateCoherence --test MidiDevicePresenceMatch` — 0 failures
- `python3 Scripts/quality/lint_touched.py` — clean

### Manual UAT (Standalone)
- Open MIDI FROM / TO / KEYBOARD FROM; plug/unplug a MIDI interface or master keyboard — open menu should dismiss and reopen with the updated list; closed menus still update before next open.
- Select an audio interface in Audio/MIDI Settings; open AUDIO FROM; enable/disable inputs or switch device — same dismiss-and-reopen.
- Repeat with DEVICE SETUP / EPROM MIDI port combos open when those dialogs are showing.
- Confirm selection preserved when the previous device id still exists; disappeared selection follows existing soft-clear / sentinel behaviour.
- Windows / Linux: same checks when hardware is available (JUCE device notifications).

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence / route |
|---------|---------|------------------|
| Skip compares ids only → stale display names (Blind, Edge×2) | low | Confirmed: identifier-only skip left labels stale. Patched: skip also requires matching item count and uppercase display texts. |
| Combo item count can disagree with id vector on skip (Blind, Edge×2) | low | Confirmed desync path. Patched via same item-set unchanged helper. |
| No test of dismiss→rebuild→show (Blind, Edge claim, VG#1) | medium | Confirmed: only `planRefresh` was asserted; Unit target cannot link `ComboBox`. Patched: pure callable sequence + unit tests for order. |
| Duplicate input/output populate (Blind) | false | Parallel populate helpers pre-existed; both correctly share the same live-refresh decision. |
| Async `showPopup` not “immediate” (Blind) | false | Intent means without user re-open; one message-loop hop matches existing ComboBox open path. |
| Rapid churn lacks dismiss debounce (Blind) | false | Matrix asks coherent final list + no-op skip; generation cancel + unchanged skip satisfy that. |
| Stale Code Map wording (Blind) | false | Rejected: fix would only edit this build’s spec. |
| PopupMenuBase task marked done but untouched (Blind) | false | Rejected: checklist wording / spec edit. |
| Scroll/highlight not preserved on reopen (Blind) | false | Expected outcome of approved dismiss-then-reopen UX. |
| Manual UAT only checklist, no pass evidence (Blind) | false | Spec Verification already treats Standalone hot-plug as manual; checklist is the required artifact. |
| `showPopup` may no-op after dismiss (Edge) | maybe-false | Unverified in Standalone with on-screen editor; deferred. |
| Populate MIDI/Audio branches never invoked under test (VG#2) | medium | Pre-verified gap; deferred — locking pure sequence is the feasible CI lock without OS device mocking. |

### Review Findings

#### 2026-09-24 (code review — commit ae1350b4 + polish WT)

- [x] [Review][Decision] Poll 0.5 s while a MIDI popup is open — resolved: **option 1 list-only** (keep poll; do not call port revalidate / force-reopen from poll)
- [x] [Review][Decision] When should UI refresh tear down / clear open MIDI ports? — resolved: **option 1 soft** (force-reopen only if presence says dead; clear only on reopen failure)
- [x] [Review][Patch] Poll open MIDI popups: refresh combo lists only — no `revalidateOpenMidiPortsForUiRefresh` [`Source/GUI/PluginEditorTimers.cpp`]
- [x] [Review][Patch] Soft dead-port policy: force-reopen only when !live; clear only when reopen fails [`Source/Core/PluginProcessorMidiPorts.cpp` / `MidiPortStateCoherence.h`]
- [x] [Review][Patch] Do not treat Keyboard From conflict `false` as a dead port — clear only when not holding that id [`Source/Core/PluginProcessorMidiPorts.cpp`]
- [x] [Review][Patch] Nested `onAboutToShowPopup` during dismiss→reopen — `showPopupAfterItemRebuild` skips callback [`Source/GUI/Widgets/ComboBox.*` / `ComboBoxLiveRefresh.h`]
- [x] [Review][Patch] Non-Apple presence: fail-open pass-through (no exclusive openDevice probe) [`Source/Core/MIDI/MidiDevicePresence.cpp`]
- [x] [Review][Patch] Multi-token CoreMIDI match includes live device UniqueID parts [`Source/Core/MIDI/MidiDevicePresenceMatch.h`]
- [x] [Review][Patch] `audioFromItemSetUnchanged` loops only over nextIds size [`Source/GUI/Helpers/AudioFromComboItemSet.h`]
- [x] [Review][Patch] Unit-test soft dead-port policy helpers [`Tests/Unit/MidiPortStateCoherenceTests.cpp`]
- [x] [Review][Patch] Unit-test MidiDevicePresence identifier↔live-id matching [`Tests/Unit/MidiDevicePresenceMatchTests.cpp`]
- [x] [Review][Patch] Unit-test midiPortItemSetUnchanged / Audio From twin for label+count skip [`Tests/Unit/MidiPortComboPopulationTests.cpp`]
- [x] [Review][Defer] Open-popup populate path never under unit tests — deferred: already recorded; pure dismiss→rebuild→show remains the feasible CI lock
- [x] [Review][Defer] `showPopup` may no-op after dismiss — deferred: already recorded maybe-false; settle with UAT / GUI harness
- [x] [Review][Defer] Dead `PopupMenuRenderer::drawSentinelBottomRule` vs local Scrollable paint — deferred: cosmetic drift; wire or delete in a chrome pass
- [x] [Review][Defer] Hardcoded sentinel item id `1` in ScrollablePopupMenu — deferred: avoid MidiPortComboPopulation coupling into widget paint; share only if a neutral constant lands
- [x] [Review][Defer] Spec Implementation Notes still say editor OS refresh paths “unchanged” — resolved: Implementation Notes updated after soft/list-only patches

**Rejected (this pass):**
- Presence / alpha-sort / popup chrome “out of frozen Never” (Acceptance Auditor) — false: polish explicitly in review scope and prior product decisions.
- `getMidiManager` null crash after revalidate early return — false: same editor lifetime assumption as the rest of PluginEditor; midiManager present when editor refreshes ports.
- `canShowPopup` false after normal rebuild — false for everyday path: sentinel keeps `getNumItems() > 0`.
- keepOpenDeviceId “flicker” path overclaimed — false as a user-facing defect: rare race; comment overstates, behaviour not harmful.
- Scrollbar width / text clip — maybe-false low: overlay scrollbar intentional; not everyday without long lists; rejected for this pass.
- Findings whose only fix is editing the frozen Intent / Never text — rejected per triage rules.

| Finding | Verdict | Evidence / route |
|---------|---------|------------------|
| Poll + force-reopen every 0.5 s / about-to-show (BH, Edge, AA) | high | Confirmed. Decision 1→list-only poll → patch. |
| shouldClearDeadPort clears on !live after successful reopen (BH, Edge, AA soft-sync) | high | Confirmed. Decision 2→soft clear policy → patch. |
| Keyboard conflict → clear (BH, Edge) | high | Confirmed: `setKeyboardFromPort` false → clear. patch. |
| Nested onAboutToShow on reopen (Edge) | medium | Confirmed: `showPopup` always invokes callback. patch. |
| Non-Apple openDevice probe (BH, Edge) | medium | Confirmed else branch. patch. |
| Multi-token device UniqueID (Edge) | medium | Confirmed loop checks endpoints only. patch. |
| audioFrom name loop vs id count (BH) | low | Confirmed; patch bound to `nextIds`. |
| VG: dead-port / presence / item-set text tests | medium | Pre-verified. patch. |
| VG: open-popup populate untested | medium | Pre-verified; defer (already). |
| AA: presence/sort/chrome out of scope | false | Human-authorized polish. |
| midiManager null / canShowPopup / keepOpen / scrollbar | false or reject | See Rejected list. |

## Design Notes

Hot-plug detection already ships. The gap is popup ownership: `ScrollablePopupMenu` sizes itself once from `getNumItems()` at construction, while item text/hit-testing read the ComboBox live. Prefer extending that contract over adding new device listeners.

Audio From lists enabled channels of the **current** `AudioDeviceManager` device. Plugging a Scarlett in does not invent Audio From rows until that interface is the active device with inputs enabled (usually via Audio/MIDI Settings). MIDI ports on that interface still appear via `MidiDeviceListConnection`.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `ctest --preset macos-debug-arm64` (or project’s unit-test target with `MATRIX_BUILD_TESTS=ON`) -- expected: new/related tests pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++ under `Source/` / `Tests/`

**Manual checks (if no CLI):**
- Standalone: open MIDI FROM (then TO, KEYBOARD FROM); plug/unplug a MIDI interface or master keyboard — open menu dismisses and reopens with the new list.
- Standalone: select Scarlett in Audio/MIDI Settings; open AUDIO FROM; enable/disable inputs or switch device — same dismiss-and-reopen behaviour.
- Confirm closed-combo hot-plug still updates before the next open.
