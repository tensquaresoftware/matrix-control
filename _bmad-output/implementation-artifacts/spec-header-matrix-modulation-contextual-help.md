---
title: 'Header + Matrix Modulation contextual help Wave 1'
type: 'feature'
created: '2026-10-09'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'dc3cefb3da58e9550c894b9a6e712612db4c4ec4'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-patch-mutator-footer-contextual-help.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-patch-mutator-labels-contextual-help.md'
  - '{project-root}/_bmad-output/implementation-artifacts/inventory-contextual-help-messages.md'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Hovering the header bar and several Matrix Modulation chrome zones does not give a usable footer HELP line: Header strings still use the obsolete `SESSION:` prefix, cartouche titles and monitoring labels are mute, and bus-number HELP is bound but never receives mouse hits. Users miss orientation without opening the manual.

**Approach:** Wave 1 only — retarget Header HELP prefixes by cartouche (`EDIT:` / `MIDI:` / `AUDIO:`, logo included as `EDIT:`), bind missing Header hover targets (cartouche badges via transparent hit areas, monitoring labels + INPUT GAIN label), fix bus-number hit-testing without breaking drag-to-reorder, bind `ModulationBusHeader` with one short orientation string, and align the Header section of the contextual-help inventory. Keep the existing footer HELP overlay; no floating JUCE tooltips.

## Boundaries & Constraints

**Always:**
- Prefix map (Header): EDIT cartouche + logo + logo-menu items → `EDIT:`; MIDI cartouche (labels, LEDs, MIDI badge) → `MIDI:`; AUDIO cartouche (INPUT GAIN label/slider, peak, AUDIO badge) → `AUDIO:`.
- ASCII-only help copy in `PluginDisplayNames` (hyphen `-`, ellipsis `...`).
- Existing `ContextualHelpBinder` + footer overlay; Settings CONTEXTUAL HELP = HIDE must still suppress all help.
- Cartouche badge hover via transparent child hit targets sized to existing `*CartoucheBadgeBounds_` (Footer `deviceHitArea_` pattern) — do not move painted chrome out of `paintCartoucheChrome`.
- Bus-number HELP must work on hover while drag-to-reorder (story 2-10 path) still starts from the bus-number zone.
- Agent-owned mouse strategy for bus numbers (forward events or equivalent) as long as both HELP and reorder work.
- Inventory Header section: retire the frozen `SESSION` decision; document cartouche prefixes.
- Scope keep (decided): ship full Wave 1 (Header + Matrix Modulation hits) in this spec despite ~1900-token length.
- Approved new HELP copy (ASCII); existing Header bodies keep wording with prefix retarget only:

| Target | Help text |
|--------|-----------|
| EDIT badge | EDIT: Undo, redo, panic, and the logo menu for Settings, About, Skin, and UI Scale. |
| MIDI badge | MIDI: Monitors MIDI activity from the keyboard (or host), the synthesizer, and to the synthesizer. |
| AUDIO badge | AUDIO: Monitors the selected audio input level and sets INPUT GAIN (Standalone). |
| FROM KEYBOARD | MIDI: Labels the keyboard (or host) MIDI activity lane. Set the device in Settings > MIDI & DEVICE. |
| FROM SYNTH | MIDI: Labels the synthesizer MIDI input activity lane. Set SYNTH FROM in Settings > MIDI & DEVICE. |
| TO SYNTH | MIDI: Labels the synthesizer MIDI output activity lane. Set SYNTH TO in Settings > MIDI & DEVICE. |
| ModulationBusHeader | MATRIX MODULATION: Column guide for bus number, source, amount, and destination. Drag a bus number to reorder. |

**Never:**
- SectionHeader / ModuleHeader / PATCH NAME (Wave 2).
- Exhaustive unbound-control audit (Wave 3).
- MIDI/audio/port behaviour changes; Settings copy except collision avoidance.
- User manual; new product controls outside listed zones.
- Classic JUCE floating tooltips as primary UX.
- Reopening deferred PITCH-disabled help unless this Wave inevitably touches that path (it should not).

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Header control hover | Mouse enters bound Header control / label / badge hit area | Footer left band shows matching HELP with cartouche prefix | N/A |
| Cartouche badge hover | Mouse over painted EDIT / MIDI / AUDIO badge | HELP for that cartouche; layout/paint unchanged | N/A |
| Plugin mode audio | Plugin build; AUDIO cartouche hidden | No AUDIO badge hit / no INPUT GAIN help targets required | N/A |
| Bus number hover | Mouse over bus `#` digit | HELP `kBusHandle`; sticky restored on leave | N/A |
| Bus number drag | Drag from bus `#` past threshold | Reorder still works as today | N/A |
| ModulationBusHeader hover | Mouse over column header strip | Short orientation HELP | N/A |
| CONTEXTUAL HELP HIDE | Preference HIDE | No HELP overlay for any Wave 1 target | N/A |
| Adjacent traverse | Mouse moves across adjacent bound Header/Matrix controls | No sticky flash (existing binder anti-flicker) | N/A |

</frozen-after-approval>

## Code Map

- `Source/Shared/Definitions/PluginDisplayNames.h` -- `HeaderPanel::ContextualHelp` retarget + new strings; `MatrixModulationSection::ContextualHelp` header orientation constant.
- `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h` / `HeaderPanel.cpp` -- `registerContextualHelp` binds; badge hit-area members; logo-menu strings already use Header help constants.
- `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanelLayout.cpp` -- place badge hit areas on `*CartoucheBadgeBounds_` inside `layoutCartouches()`.
- `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanelSetup.cpp` -- add/makeVisible hit areas if constructed there.
- `Source/GUI/Widgets/HeaderLogoPopupMenu.cpp` -- verify menu items pick up retargeted `kSettings` / `kAbout` / `kSkin` / `kUiScale` (no behaviour change).
- `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp` -- precedent: `deviceHitArea_` transparent bind target.
- `Source/GUI/Settings/SettingsPanel.cpp` -- precedent: label + control share one help string.
- `Source/GUI/Widgets/ModulationBusCell.cpp` / `.h` -- bus-number mouse hit vs reorder (`setInterceptsMouseClicks`, `isBusNumberLabelHit`, drag callbacks).
- `Source/GUI/Widgets/ModulationBusHeader.cpp` / `.h` -- paint-only column titles; add hit path or bind whole component for orientation HELP.
- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/MatrixModulationPanel/MatrixModulationPanelContextualHelp.cpp` -- existing `kBusHandle` bind; bind `modulationBusHeader_`.
- `_bmad-output/implementation-artifacts/inventory-contextual-help-messages.md` -- Header prefix decide + section 1 table.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Shared/Definitions/PluginDisplayNames.h` -- Retarget all `HeaderPanel::ContextualHelp` prefixes; add cartouche-badge, monitoring-label, and `MatrixModulationSection::ContextualHelp` header-orientation constants (approved copy) -- SSOT for footer HELP.
- [x] `Source/GUI/Panels/MainComponent/HeaderPanel/*` -- Bind monitoring labels + INPUT GAIN label; add three transparent badge hit areas synced to badge bounds; bind them; keep LEDs/peak/slider/buttons/logo binds with new strings -- Wave 1 Header hits.
- [x] `Source/GUI/Widgets/ModulationBusCell.*` + `MatrixModulationPanelContextualHelp.cpp` -- Make bus-number hover deliver `kBusHandle` without breaking reorder drag -- fix mute bind.
- [x] `Source/GUI/Widgets/ModulationBusHeader.*` + `MatrixModulationPanelContextualHelp.cpp` -- Bind header to orientation HELP -- column chrome orientation.
- [x] `_bmad-output/implementation-artifacts/inventory-contextual-help-messages.md` -- Replace Header `SESSION` decide + section 1 messages with cartouche prefixes / current UI labels -- inventory alignment.
- [ ] UAT Standalone (no pure unit extract planned) -- smoke Wave 1 hovers + bus reorder + HIDE preference -- human-visible proof.

**Acceptance Criteria:**
- Given Standalone with CONTEXTUAL HELP shown, when the user hovers EDIT / MIDI / AUDIO cartouche badges, then the footer HELP line uses the matching cartouche prefix and approved badge copy.
- Given Standalone, when the user hovers FROM KEYBOARD / FROM SYNTH / TO SYNTH / INPUT GAIN labels (and existing bound Header controls), then HELP appears with the correct cartouche prefix (no remaining `SESSION:` on Header HELP).
- Given Plugin mode, when AUDIO cartouche is hidden, then no AUDIO badge HELP target is required and MIDI/EDIT HELP still work.
- Given Matrix Modulation, when the user hovers a bus number, then `kBusHandle` HELP shows; when they drag that number past the reorder threshold, then bus reorder still works.
- Given Matrix Modulation, when the user hovers `ModulationBusHeader`, then the short orientation HELP shows.
- Given CONTEXTUAL HELP = HIDE, when the user hovers any Wave 1 target, then no HELP overlay appears.
- Given the inventory Header section, when Wave 1 lands, then it no longer freezes `SESSION` as the Header prefix decision.

## Implementation Notes

- Header: three transparent `juce::Component` badge hit areas (`edit/midi/audioCartoucheBadgeHitArea_`), bounds synced in `layoutCartoucheBadgeHitAreas()`; AUDIO hit area hidden with audio controls in Plugin mode.
- Header binds: badges, FROM KEYBOARD / FROM SYNTH / TO SYNTH labels, INPUT GAIN label (+ existing slider/LEDs/peak/logo/buttons) with cartouche-prefixed `PluginDisplayNames` strings.
- Bus number: `setInterceptsMouseClicks(true, false)` + `addMouseListener(this)`; reorder handlers use `getEventRelativeTo(this)`; dtor removes listener.
- `ModulationBusHeader` bound to `Help::kColumnHeader` (whole component receives hover by default).
- Inventory Header section retargeted away from frozen `SESSION`.
- Build `macos-debug-arm64` + `lint_touched.py` OK. Standalone UAT still human-owned (hover matrix + bus reorder + HIDE).
- Context file `spec-patch-mutator-labels-contextual-help.md` was in stash (pre-Wave-1 clean tree), not on disk during implement.

## Spec Change Log

## Review Triage Log

- Blind: PANIC HELP uses EDIT: while panicButton_ is laid out in MIDI cartouche — `false`; human Intent places PANIC in the EDIT HELP family with UNDO/REDO; MIDI chrome placement is pre-existing layout, not a Wave 1 prefix bug.
- Blind: Inventory dropped old Header port-select rows without Settings homes — `false`; Wave 1 intent only realigns Header inventory to current monitoring chrome; Settings owns port selection copy elsewhere.
- Blind: Inventory still marked Header prefix as “(decide)” after lock — `low` → patch; wording set to “(locked)” in inventory summary + section title.
- Blind: Spec Code Map says hit areas inside layoutCartouches vs separate helper — `false`; reject findings whose fix is editing this build’s spec; Implementation Notes already match code.
- Blind: Tasks list ModulationBusHeader.* but diff only binds from panel — `false`; bind-only change satisfies the task; no header source edit required when whole-component hover works.
- Blind: kMidiBadge wording mixes from/to awkwardly — `false`; copy was human-approved in frozen Always table.
- Blind: kAudioBadge says badge “sets INPUT GAIN” — `false`; same approved frozen copy.
- Blind: kUndo body still says “in this session” after SESSION: prefix removal — `false`; Intent keeps existing bodies with prefix retarget only.
- Blind: FROM KEYBOARD label help lacks Plugin/host caveat that LED string has — `false`; approved frozen copy; LED string already covers host-in-Plugin.
- Blind: FROM KEYBOARD help does not name KEYBOARD FROM Settings control — `low` rejected; unlikely everyday harm; approved copy; renaming Settings in the sentence is more than a direct correction.
- Blind: Empty Spec Change Log / Review Triage Log mid-review — `false`; process state; this log fills the triage trail.
- Blind: Frozen Always bullet omits explicit UNDO/REDO/PANIC under EDIT — `false`; reject edits to frozen Intent; user prompt + inventory EDIT row already include them.
- Blind: Sibling namespaces share kFromKeyboardLabel name for caption vs help — `low` rejected; developer-only naming smell without a named wrong-bind site in this diff; rename would add public surface.
- Blind: I/O matrix omits HELP behavior mid bus-number drag — `maybe-false` rejected (would be low); binder + sticky restore during drag not shown to break; UAT can observe if needed.
- Edge Case Hunter: no findings — `false`; empty report `[]`.
- Verification Gap: Bus-number mouse/reorder path untested by Core reorder suites — `medium` → defer; real GUI regression risk; project forbids GUI unit tests; Standalone UAT is the gate.
- Verification Gap: Wave 1 HELP binds/hit areas lack CI observer — `medium` → defer; pre-filed disposition defer; UAT + Core-test convention.

## Design Notes

**Cartouche badges:** Keep `paintCartoucheChrome` as Graphics-only. Add three opaque=false child `juce::Component` hit areas (`setInterceptsMouseClicks(true, false)`), bounds = stored badge rectangles after `layoutCartouches()`, bind via `contextualHelpBinder_`. Mirror Footer `deviceHitArea_`.

**Bus number:** Prefer enabling mouse intercept on the bound label (or a same-bounds sibling) and forwarding `mouseDown` / `mouseDrag` / `mouseUp` into the existing `ModulationBusCell` reorder path so `ContextualHelpBinder` receives `mouseEnter` without inventing a second help channel. Do not bind the whole cell (would steal help from source/amount/destination).

**Prefix retarget for existing constants:** Example — `SESSION: Undoes...` → `EDIT: Undoes...`; LED strings → `MIDI:`; `kInputGain` / peak → `AUDIO:`. Logo menu constants (`kSettings`, `kAbout`, `kSkin`, `kUiScale`) → `EDIT:`. Approved new-copy table lives in the frozen Boundaries block.

## Verification

**Commands:**
- `python3 Scripts/quality/lint_touched.py` -- expected: pass on touched C++ under `Source/`
- Build preset `macos-debug-arm64` (or active platform Debug) -- expected: compile success for changed targets

**Manual checks (if no CLI):**
- Standalone: hover Header badges, monitoring labels, INPUT GAIN label, LEDs, undo/redo/panic, logo; confirm prefixes and leave-restore.
- Matrix Modulation: hover bus `#`, drag reorder, hover column header.
- Settings → CONTEXTUAL HELP = HIDE → no HELP on those targets.
- Grep touched display strings for Unicode dashes / ellipsis.
