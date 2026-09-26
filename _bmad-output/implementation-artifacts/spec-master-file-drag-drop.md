---
organization: Ten Square Software
project: Matrix-Control
title: Master File Drag-Drop Load
author: BMad Agent
type: feature
created: '2026-09-26'
updated: '2026-09-26'
status: done
route: dispatch
review_loop_iteration: 0
baseline_commit: ff80039e8d6c43e10ac58412d7beef7f5e3ae67e
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-syx-drag-drop-load.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-m1km-master-import.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Loading a Master still requires Settings → LOAD even though patch `.syx` / `.m1kp` already drop onto the editor. Users of Master `.syx` and legacy `.m1km` need the same hot-swap path.

**Approach:** Extend the existing whole-window file drag target so a single valid Master file is classified separately from patches, shows Master-specific drag feedback on the PATCH NAME module (temporary chrome + overlay), and on drop reuses the Settings Master LOAD commit path (including the `.m1km` Groups/cascade choice modal).

## Boundaries & Constraints

**Always:**
- Whole editor remains the `FileDragAndDropTarget` (`PluginEditor`); no separate drop zone widget.
- Classify one dropped/dragged file as Master vs Patch by extension + size / decode contract before choosing load path:
  - Patch `.syx` = existing single-patch validate (**275** B); Patch `.m1kp` = **268** B decode.
  - Master `.syx` = master SysEx (**351** B, opcode `0x03`); Master `.m1km` = **344** B / 172× int16 LE in range.
- Drag feedback stays display-only (no APVTS / live Master mutation until drop commits).
- **Master drag chrome (decision):** reuse the PATCH NAME module as the drop stage. While a valid Master file hovers:
  - Module header title switches from `PATCH NAME` to **`LOAD MASTER`** (temporary only).
  - Module header decorative line switches from **Blue** to **Orange** (`ModuleHeader::ColourVariant::Orange` — same Master Edit colour language).
  - Overlay primary = fixed **`MASTER`**; secondary = **`DROP TO LOAD`** blinking at the same 2 Hz cadence as patch drag.
  - On `dragExit`, cancel, or after drop handling starts: restore title `PATCH NAME`, Blue line, and normal PATCH NAME display.
- Invalid / unsupported payloads keep the existing `********` + `BAD FILE` / `BAD FILES` overlay contract (Blue / `PATCH NAME` chrome — do not switch to Master chrome for junk).
- Drop of a valid Master `.syx` calls `PluginProcessor::loadMasterFromUserFile` (same as Settings LOAD).
- Drop of a valid Master `.m1km` calls `tryDecodeMasterM1kmUserFile` → `openMasterM1kmLoadChoiceDialog` → `commitMasterM1kmUserLoad` (same modal and Groups policy as Settings).
- Footer messages only on drop (success / reject); no live footer text during drag.
- V1 Master drop accepts exactly one file; multi-file or folder drops that are not already handled as Computer Patches selection stay on the patch path or reject — never load a Master from a multi-file selection.
- ASCII-only new UI strings in `PluginDisplayNames` (`LOAD MASTER`, overlay `MASTER`; reuse existing `DROP TO LOAD`).

**Never:**
- Do not invent a second Master loader beside Settings / InitTemplateWriter commit.
- Do not write `.m1km` on SAVE AS; do not add Master SAVE via drop.
- Do not auto-pick Groups/cascade policy for `.m1km` (modal required).
- Do not treat a 351 B master `.syx` as a Computer Patches load or a 275 B patch as Master.
- Do not change `SysExParser` length contracts or Master packed 172-byte layout.
- Do not permanently rename the PATCH NAME module or permanently recolour it orange.
- Do not show the OS Master filename in the overlay primary (fixed `MASTER` only).
- Do not build a new permanent module UI for Master name (Master has no 8-char name field).

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Drag valid Master `.syx` | One 351 B master SysEx over editor | Header `LOAD MASTER` + Orange line; primary `MASTER`; secondary `DROP TO LOAD` blinks; footer unchanged | N/A |
| Drag valid Master `.m1km` | One 344 B `.m1km` over editor | Same Master chrome/overlay as `.syx`; no modal yet | N/A |
| Drag valid Patch | Existing `.syx` / `.m1kp` / multi / folder | Unchanged patch overlay; header stays `PATCH NAME` + Blue | N/A |
| Drag ambiguous / junk | Wrong size `.syx`, bad `.m1km`, unknown ext | `********` + `BAD FILE`; header stays `PATCH NAME` + Blue; footer unchanged | N/A |
| Drop Master `.syx` | Valid master | Settings-equivalent load; success footer; chrome restored | Existing load errors |
| Drop Master `.m1km` | Valid; user picks settings-only / full / cancel | Same modal + commit / cancel as Settings LOAD; chrome restored before/at modal | Cancel → no Master change |
| Drop Master + other files | ≥2 files including a Master | No Master load; reject or fall through per classifier (never silent Master commit) | Footer explains |
| Drag exit | Leave without drop | Restore `PATCH NAME` + Blue + real patch name display | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/PluginEditorFileDragDrop.cpp` (+ `PluginEditor.h`) — extend classifier: Master single-file vs existing patch/selection heuristics; branch `filesDropped` to Master load vs `handleSyxFilesDropped`; keep interest open for bad payloads.
- `Source/GUI/Panels/.../PatchNameDisplayPanel.{h,cpp}` — Master drag chrome API: temporary header title + `ColourVariant` swap; Master overlay kind (`MASTER` + `DROP TO LOAD`); restore on clear.
- `Source/GUI/Widgets/PatchNameDisplay.{h,cpp}` — reuse overlay show/clear + 2 Hz secondary blink; fixed primary `MASTER` for Master kind.
- `Source/GUI/Widgets/ModuleHeader.{h,cpp}` — ensure title + colour variant can change at runtime (if not already); Blue↔Orange for temporary Master chrome.
- `Source/Shared/Definitions/PluginDisplayNames.h` — `LOAD MASTER`, overlay primary `MASTER`, Master drop footer reject/success formatters (ASCII); reuse `DROP TO LOAD`.
- Core Master-file assess (new thin helper near Init / `MasterM1kmCodec` / loader — **not** overloaded into `PatchFileService` patch assess) — size + decode peek without mutating live Master; callable from editor for drag preview + drop gate.
- `Source/Core/Init/InitTemplateLoader.*` / `InitTemplateWriter.*` / `MasterM1kmCodec.*` — SSOT validate/decode for Master `.syx` / `.m1km`.
- `Source/Core/PluginProcessorInitTemplates.cpp` — reuse `loadMasterFromUserFile`, `tryDecodeMasterM1kmUserFile`, `commitMasterM1kmUserLoad`.
- `Source/GUI/PluginEditorSettings.cpp` / `MasterM1kmLoadChoiceDialog` — reference only; drop must call same dialog API already on `PluginEditor`.
- `Tests/Unit/` — Core-side Master file classify / reject sizes; do not GUI-test blink.
- Do **not** change: Computer Patches multi-drop virtual list, `SysExParser` lengths, `.m1km` SAVE AS policy.

## Tasks & Acceptance

**Execution:**
- [x] Core Master-file assess helper (`.syx` 351 / `.m1km` 344+range) — public peek without live commit — SSOT for drag preview + drop gate
- [x] `PluginEditorFileDragDrop.cpp` — classify Master vs Patch vs invalid; Master chrome/overlay + drop branch — product entry
- [x] `PatchNameDisplayPanel` (+ `ModuleHeader` / `PatchNameDisplay` as needed) — temporary `LOAD MASTER` + Orange line + `MASTER` / `DROP TO LOAD` blink; full restore on clear — feedback contract
- [x] `PluginDisplayNames.h` — Master drag/drop strings (ASCII) — UI SSOT
- [x] Wire drop to `loadMasterFromUserFile` / `.m1km` modal+commit — Settings parity
- [x] Unit tests for classify / reject wrong sizes — lock Core matrix
- [ ] Manual: drag Master `.syx` / `.m1km` / Patch / junk; drop `.m1km` modal Cancel — UAT

**Acceptance Criteria:**
- Given a valid Master `.syx` or `.m1km` dragged over the editor, when the pointer stays inside, then the PATCH NAME module shows `LOAD MASTER` + Orange line, primary `MASTER`, blinking `DROP TO LOAD`, and the live Master is unchanged until drop+commit.
- Given drag exit or drop handling start, when Master chrome was active, then title, line colour, and display restore to the normal patch state.
- Given a valid Master `.m1km` dropped, when the Groups/cascade dialog is shown, then Cancel leaves Master unchanged and the two load options match Settings LOAD.
- Given a valid Patch file drag/drop, when Master support is present, then Computer Patches behaviour and Blue / `PATCH NAME` chrome are unchanged.
- Given a 275 B patch `.syx` and a 351 B master `.syx`, when each is dragged, then they never share the wrong load path or chrome.

## Implementation Notes

- Added `Core::MasterFileAssess` (peek-only; `.syx` via `SysExDecoder::decodeMasterSysEx`, `.m1km` via `MasterM1kmCodec`).
- `PluginEditorFileDragDrop` classifies single Master before patch/selection; drop reuses Settings `loadMasterFromUserFile` / `.m1km` choice dialog + commit.
- PATCH NAME temporary chrome: `LOAD MASTER` + Orange via `ModuleHeader::setColourVariant`; overlay primary fixed `MASTER`.
- Unit tests: `MasterFileAssess` — 0 failures (includes correct-size invalid Syx/m1km). Lint OK on touched C++.
- Review patches: ignore drag/drop while `MasterM1kmLoadChoiceDialog` is visible; added correct-size reject tests.
- Manual UAT (drag chrome / `.m1km` Cancel) still required.

## Spec Change Log

## Review Triage Log

- Blind: Master path-cache short-circuit after patch fall-through — verdict: `false` — after Master fall-through the patch overlay already owns the cache key; early `return true` only skips redundant reassess for a stable path (same pattern as patch drag caching).
- Blind: `isJunkDragSelection` couples Master `.syx` to patch-extension heuristic — verdict: `false` — intentional: invalid-as-Master `.syx` must fall through to patch assess; interest via `.syx` is required.
- Blind: Master+Patch multi falls through to Computer Patches with no Master reject footer — verdict: `false` — frozen intent allows reject **or** fall through; never silent Master commit is satisfied.
- Blind: Multi with Master ext shows `PATCHES...` not Master-or-reject — verdict: `false` — mixed/acceptable multi stays on patch selection chrome per intent.
- Blind: Unit tests miss correct-size decode / content rejects — verdict: `medium` — verified: size-only impostors would pass current tests; assess requires decode/`isValidContents`.
- Blind: Code Map InitTemplateLoader SSOT vs MasterFileAssess peek — verdict: `false` — fix would be editing this build's spec Code Map; rejected.
- Blind: Code Map lists PatchNameDisplay files untouched by diff — verdict: `false` — fix would be editing this build's spec; rejected.
- Blind: `MidiManager::getSysExDecoder()` widens GUI→MIDI surface — verdict: `low` — rejected: façade would add complexity beyond a direct correction; no everyday user harm.
- Blind: `decodeMasterSysEx` logs Info/Error on drag peek — verdict: `low` — rejected: quiet-decode path would add API surface; hover logging is developer-only noise.
- Blind: Spec matrix / empty triage / Manual unchecked — verdict: `false` — process meta / Manual UAT remaining by design.
- Edge: mid-drag size/modTime TOCTOU vs path cache — verdict: `maybe-false` — same class as existing patch drag cache; would be medium if proven distinct Master/Patch flip mid-hover.
- Edge: drop Master while m1km choice dialog already visible — verdict: `medium` — verified: `openMasterM1kmLoadChoiceDialog` replaces callbacks via `prepareForShow`; second drop can swap packed commit or load `.syx` under an open modal.
- Edge: drag overlay updates while m1km choice dialog visible — verdict: `medium` — verified: editor remains `FileDragAndDropTarget`; no guard on `updatePatchNameDragOverlay`.
- VerGap: MasterFileAssess correct-size invalid payload untested — verdict: `medium` — pre-verified gap; disposition patch.
- VerGap: editor Master drop routing never under tests — verdict: `medium` — pre-verified; disposition defer (Core/GUI pyramid + Manual UAT).
- VerGap: Master drag chrome never under tests — verdict: `medium` — pre-verified; disposition defer (explicit Manual UAT / no GUI harness).

## Design Notes

- Patch vs Master `.syx` distinction is reliable: different message lengths (275 vs 351) and opcodes (`0x01`/`0x0D` vs `0x03`). Extension alone is not enough for `.syx`.
- `.m1kp` (268) vs `.m1km` (344) are already separated by extension and size.
- Master chrome reuses existing `ModuleHeader` Blue/Orange language (Patch Edit vs Master Edit) — temporary only; not a permanent recolour of PATCH NAME.
- Fixed overlay primary `MASTER` avoids filename clipping (Master SAVE AS has no stem-length sanitizer).

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build success
- `ctest --preset macos-debug-arm64 -R 'Master|M1km|PatchFile|Drag' --output-on-failure` (adjust filter to new tests) -- expected: pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++

**Manual checks:**
- Drag Master `.syx` / `.m1km` / Patch `.syx` / junk over Standalone; confirm `LOAD MASTER` + Orange + `MASTER` / `DROP TO LOAD`; drop + `.m1km` modal parity with Settings LOAD; exit restores Blue / `PATCH NAME`.
