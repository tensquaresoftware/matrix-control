---
title: 'Dirty / unsaved patch UX — chantier 2 (device Save As risk + file Persist)'
type: 'feature'
created: '2026-08-22'
status: 'done'
baseline_commit: '51c726f'
review_loop_iteration: 0
context:
  - '{project-root}/Documentation/Devs/Plans/2026/08/2026-08-03-Dirty-Vs-Unsaved-Patch-UX.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-dirty-unsaved-patch-ux-chantier-1.md'
  - '{project-root}/_bmad-output/project-context.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After chantier 1, two gaps remain. (1) Successful SAVE / SAVE AS from a device-origin (or INIT / not-yet-STORED) context refreshes the clean snapshot and clears `patchNotStoredInRam_`, so a later leave can stay silent even though the current RAM slot never received STORE. (2) Leaving a dirty patch that came from a `.syx` still offers Store / Save As (RAM/ROM), not Cancel / Discard / **Save** (known file) or **Save As** (no clear target).

**Approach:** Keep the chantier-1 at-risk rule after device-context SAVE / SAVE AS (lecture B until STORE). Track a known computer `.syx` path for file-origin Persist so leave uses Save vs Save As. File intent primes the leave modal for chantier 2 — no fourth Store+Save button.

## Boundaries & Constraints

**Always:**
- Product SSOT §0 decisions 5–6, §4.5, §5.2–5.3, §6 (Computer Patches load), §8 points 6–7; build on chantier 1 (`75f397d` / its spec).
- After successful SAVE / SAVE AS while still in a **device / not-yet-STORED-in-RAM** risk context: refresh the clean snapshot **and** keep (or re-set) not-STORED-in-RAM so the next leave uses the chantier-1 family (RAM: Cancel / Discard / Store; ROM: Cancel / Discard / Save As).
- Do **not** flip leave Persist to file Save merely because a device-origin patch wrote a `.syx` backup — Store remains the leave Persist when RAM risk applies (§5.1 / §4.5).
- File-origin dirty leave: Cancel / Discard / **Save** when a known overwrite `.syx` path exists; otherwise Cancel / Discard / **Save As**. Known path = durable absolute path from last successful computer load or last successful Save that established/kept computer origin; empty or unusable → Save As.
- Never-warn Settings still suppresses the whole risk family; Mutator history modal still runs **after** the risk gate; English UI only; Core ↛ AlertWindow / dialog types.
- Preserve chantier-1 INIT not-STORED, PASTE silent entry, Cancel abort / restore, and second-INIT gating.

**Ask First:**
- Offering Store **and** Save in the same leave modal (explicitly out of chantier 2 unless Guillaume reopens it).
- Implementing chantier 3 (plugin / DAW / session close warning).
- Changing never-warn so Mutator history would still prompt when risk is suppressed.

**Never:**
- Chantier 3; French UI strings; merging Mutator / name-recon / IMPORT / Defrag / Master INIT dialogs with the risk family.
- Four-button Store+Save leave modal; regressing PASTE entry silence or INIT not-STORED from chantier 1.
- Large refactors unrelated to origin / Persist path / not-STORED retention after save.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Device Save As keeps RAM risk | Device-origin (or INIT not-STORED); successful Save As | Clean snapshot refreshed; `patchNotStoredInRam_` true; next leave prompts chantier-1 family (Store if RAM allowed) | Picker cancel / save fail → no leave; flags unchanged for abort paths |
| File dirty + known path | `PatchLoadContext` computer + remembered `.syx` path; dirty; leave | Cancel / Discard / **Save**; Persist overwrites that file then proceeds | Save fail → abort leave (same as Persist fail today) |
| File dirty + no clear target | Computer origin without usable known path; dirty; leave | Cancel / Discard / **Save As**; Persist opens picker | Picker cancel / fail → abort leave |
| Never-warn | Any at-risk / file-dirty | No risk-family modal | N/A |
| Cancel abort | Gate Cancel | Context change aborted; selection / NumberBox / OPEN / browser restore unchanged from chantier 1 / 9.2 | N/A |
| Device-origin after Save As then STORE | not-STORED true after Save As; user Stores | not-STORED cleared; leave silent if clean | STORE blocked (ROM) → Persist fail / keep risk |
| INIT / PASTE regression | INIT then leave; PASTE entry | INIT still not-STORED leave modal; PASTE still silent on entry | N/A |

</frozen-after-approval>

## Code Map

- `Source/Core/Actions/PatchManagerActionHandler.cpp` -- `captureCleanSnapshot` clears not-STORED today; `tryPersistCurrentPatchFromUnsavedGate(bool storeAllowed)` only Store vs Save As
- `Source/Core/Actions/PatchManagerActionHandler.h` -- `patchNotStoredInRam_`, Persist API, save completion hooks
- `Source/Core/Actions/PatchManagerActionHandlerComputerLoadSave.cpp` -- `commitLoadedComputerPatchFile`, `handleSavePatchFile` / `handleSavePatchAs`, `completeSuccessfulSave` → `captureCleanSnapshot`
- `Source/Core/Actions/PatchManagerActionHandlerInternalPatches.cpp` -- INIT capture-then-`markPatchNotStoredInRam` pattern to mirror after device Save As
- `Source/Core/Services/PatchMutator/PatchLoadContext.{h,cpp}` -- origin device vs computer (stem only today); extend or sibling for known absolute `.syx` path without breaking Export basename rules
- `Source/Core/PluginProcessorGates.cpp` -- leave gate; today passes `storeAllowed` only for Persist labeling / action
- `Source/GUI/PluginEditorPatchBindings.cpp` -- Store vs Save As labels from `storeAllowed`
- `Source/Shared/Definitions/PluginDisplayNames.h` -- `Dialogs::UnsavedEditConfirm` (add Save + file-origin body; keep Store / Save As)
- `Tests/Unit/PatchManagerActionHandlerGateTests.cpp` -- extend; update `unsavedGate_captureAfterSaveLeavesClean` for not-STORED retention after device Save As
- `Tests/Unit/DirtyPatchTrackerTests.cpp` -- policy never-warn / risk (keep green; extend if Persist mode helpers land here)
- `Tests/Unit/PatchManagerActionHandlerTestSupport.*` -- harness for gate choice + save pick callbacks
- `Tests/Unit/PatchManagerActionHandlerSaveTests.cpp` -- optional continuity for save write paths

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/Services/PatchMutator/PatchLoadContext.*` (+ processor/handler ownership as needed) -- remember durable known `.syx` absolute path on computer load and on successful Save that keeps/establishes computer origin; clear on device-load / INIT / PASTE / paths that reset to deviceMemory; do not treat device Save As backup as flipping leave Persist to file mode
- [x] `Source/Core/Actions/PatchManagerActionHandlerComputerLoadSave.cpp` + `PatchManagerActionHandler.cpp` -- after successful SAVE / SAVE AS in device / not-yet-STORED risk context: capture clean then keep/re-set not-STORED (INIT-style); adjust Persist success so a retained not-STORED after device Save As is not treated as Persist failure when dirty is clear
- [x] `Source/Core/Actions/PatchManagerActionHandler.*` + `PluginProcessorGates.cpp` -- widen leave Persist beyond `storeAllowed` bool: device/RAM-risk → Store or Save As (chantier 1); file-origin dirty → Save (known path) or Save As (else); wire Persist to `handleSavePatchFile` equivalent using known path (not only browser selection) or `handleSavePatchAs`
- [x] `Source/GUI/PluginEditorPatchBindings.cpp` + `PluginDisplayNames.h` -- Cancel / Discard / Save (or Save As) copy for file-origin dirty; English only; Core stays dialog-free
- [x] `Tests/Unit/PatchManagerActionHandlerGateTests.cpp` (+ policy/harness as needed) -- device Save As keeps not-STORED; file-origin Persist Save; never-warn silent; Cancel abort unchanged; INIT/PASTE chantier-1 cases still pass; update obsolete “save washes not-STORED” assertion

**Acceptance Criteria:**
- Given device-origin (or INIT not-STORED) and a successful Save As, when leaving without STORE, then the chantier-1 at-risk modal appears (Store when RAM allowed).
- Given a dirty computer-origin patch with a known `.syx` path, when leaving, then Cancel / Discard / Save and Persist overwrites that file.
- Given a dirty computer-origin patch without a usable known path, when leaving, then Cancel / Discard / Save As.
- Given Settings never-warn, when leaving any at-risk or file-dirty patch, then no risk-family alert.
- Given Cancel on the risk modal, when confirming, then the context change aborts as today.
- Given chantier-1 INIT not-STORED and PASTE silent entry, when re-tested, then behaviour is unchanged.
- Given non-empty Mutator history and a proceeding risk choice, when the gate continues, then the Mutator history modal may still follow as today.

## Spec Change Log

## Design Notes

**Device Save As ≠ file leave mode.** Writing a `.syx` backup from a synth/INIT context must refresh the plugin clean photo but keep RAM risk (lecture B) until STORE. Do not switch leave Persist to Save just because a file was written.

**Decouple snapshot from not-STORED clear.** Today `captureCleanSnapshot()` always clears `patchNotStoredInRam_`. Mirror INIT: capture then re-mark when device/not-STORED risk must survive SAVE / SAVE AS. Prefer a small, explicit retention rule over a silent side effect in every capture caller.

**Persist mode.** Replace the single `storeAllowed` bool with an explicit leave-Persist kind (Store / Save / Save As) chosen in Core from origin + known path + RAM limits; GUI only maps labels. File-origin dirty → file Persist only (no fourth Store button).

**Known path.** Stem + Computer Patches combo selection are not enough for reliable overwrite after navigation. Remember absolute path at computer load / successful computer Save; if missing or file gone → Save As.

**Suggested review order:** known-path / origin retention → completeSuccessfulSave not-STORED rule → Persist kind + gate → display names / bindings → gate unit tests.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` -- expected: build succeeds
- `ctest --preset macos-debug-arm64 -R 'DirtyPatch|PatchManagerActionHandlerGate|PatchManagerActionHandlerSave'` -- expected: pass (adjust regex if names differ)
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++ under `Source/` and `Tests/`

**Manual checks (if no CLI):**
- Device dump → edit → Save As → leave without Store → Store modal; Store then leave silent if clean.
- Load `.syx` → edit → leave → Save overwrites; clear known path / Save As outside library → Save As button.
- never-warn silent; INIT leave still warns; PASTE entry silent; Mutator history still after Discard when history non-empty.

## Suggested Review Order

**Device Save As keeps RAM risk**

- After a device-context `.syx` write, refresh clean then re-mark not-STORED (INIT-style).
  [`PatchManagerActionHandlerComputerLoadSave.cpp:275`](../../Source/Core/Actions/PatchManagerActionHandlerComputerLoadSave.cpp#L275)

- Leave Persist succeeds when dirty is clear even if not-STORED stays true.
  [`PatchManagerActionHandler.cpp:321`](../../Source/Core/Actions/PatchManagerActionHandler.cpp#L321)

**File-origin Persist kind**

- Dirty computer origin → Save (known path) or Save As; else Store / Save As by RAM limits.
  [`PatchManagerActionHandler.cpp:262`](../../Source/Core/Actions/PatchManagerActionHandler.cpp#L262)

- Gate passes Persist kind into Core action (no Store+Save four-button modal).
  [`PluginProcessorGates.cpp:160`](../../Source/Core/PluginProcessorGates.cpp#L160)

- Remember absolute `.syx` path on computer load for overwrite Save.
  [`PatchManagerActionHandler.cpp:283`](../../Source/Core/Actions/PatchManagerActionHandler.cpp#L283)

**UI copy**

- Modal body/button switch for Store / Save / Save As (English only).
  [`PluginEditorPatchBindings.cpp:173`](../../Source/GUI/PluginEditorPatchBindings.cpp#L173)

- Persist kind enum shared by Core and GUI binding.
  [`UnsavedEditWarningPolicy.h:16`](../../Source/Core/Services/UnsavedEditWarningPolicy.h#L16)

**Tests**

- Device Save As (INIT and dirty dump) keeps not-STORED; file Save / Save As kinds.
  [`PatchManagerActionHandlerUnsavedChantier2Tests.cpp:13`](../../Tests/Unit/PatchManagerActionHandlerUnsavedChantier2Tests.cpp#L13)
