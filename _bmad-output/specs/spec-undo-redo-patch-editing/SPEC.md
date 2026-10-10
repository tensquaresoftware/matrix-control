---
id: SPEC-undo-redo-patch-editing
companions:
  - undo-policy.md
  - keyboard-shortcuts.md
  - state-machines.md
  - ../../../CONVENTIONS.md
  - ../../project-context.md
sources:
  - ../../implementation-artifacts/5-2-module-copy-paste-enable-and-gray-rules.md
  - ../../implementation-artifacts/5-3-matrix-modulation-section-i-c-p-gui.md
  - ../../../Documentation/User/manuel-utilisateur.md
---

> **Canonical contract.** This SPEC and the files in `companions:` are the complete, preservation-validated contract for what to build, test, and validate. Source documents listed in frontmatter are for traceability — consult them only if you need narrative rationale or prose color this contract intentionally omits.

# Editorial Undo/Redo for Patch Editing

## Why

Matrix-Control users edit Oberheim Matrix patches in real time with every slider and display change sent to the synth via MIDI SysEx. The user manual today states there is no Undo/Redo; users must rely on STORE, SAVE, or Patch Mutator Compare to recover from mistakes. Modern plugin expectations and daily patch-tweaking workflows need standard undo/redo that groups natural editing gestures, resyncs the synth after each step, and stays separate from Patch Mutator history and Compare audition.

## Capabilities

- **CAP-1**
  - **intent:** User can undo the last editorial transaction, restoring prior patch, master, and Matrix Mod parameter state held in APVTS.
  - **success:** After one grouped gesture (e.g. slider drag), one undo (Cmd/Ctrl+Z) restores pre-gesture values and the synth plays the restored state via the existing APVTS→model→SysEx path.

- **CAP-2**
  - **intent:** User can redo an undone editorial transaction.
  - **success:** Cmd/Ctrl+Shift+Z reapplies the undone transaction; UI parameters and synth output match the re-applied state.

- **CAP-3**
  - **intent:** Standard undo/redo keyboard shortcuts work in Standalone and hosted plugin formats when focus rules allow (see `keyboard-shortcuts.md`).
  - **success:** macOS Cmd+Z / Cmd+Shift+Z and Windows/Linux Ctrl+Z / Ctrl+Shift+Z invoke undo/redo per platform conventions without stealing shortcuts from active text fields or modal dialogs.

- **CAP-4**
  - **intent:** Related parameter changes from one user gesture form a single undo transaction, not per-parameter micro-steps or per arrow keypress.
  - **success:** Each of the following produces exactly one undo step: slider mouse drag (slide); envelope interactive drag (all touched ENV params); Track Generator curve drag; Init module (I); Paste module (P); Matrix Mod bus drag-reorder. Slider arrow keys are **not** one undo per keypress — they group per focus session until focus is lost (see `undo-policy.md`). Undo restores the pre-gesture value; redo restores the post-gesture value.

- **CAP-5**
  - **intent:** After undo or redo, the synthesizer reflects restored APVTS state through the existing model and SysEx dispatch pipeline.
  - **success:** Post undo/redo SysEx output matches restored parameters; existing `suppress*` flags and Matrix Mod SysEx coalesce behavior are preserved (no MIDI storm, no broken bulk paths).

- **CAP-6**
  - **intent:** The dirty-patch indicator reflects true divergence from the load snapshot after undo and redo.
  - **success:** `DirtyPatchTracker` (`isDirty` / `syncApvtsAndIsDirty`) matches user expectation in edit→undo→clean and edit→undo→redo scenarios (see `undo-policy.md` edge matrix).

## Constraints

- Undo logic lives in Core / `PluginProcessor`; GUI handles shortcuts only. Core must not depend on GUI (`CONVENTIONS.md`, `project-context.md`).
- Wire JUCE `UndoManager` into APVTS construction (today: `apvts(*this, nullptr, …)`). Non-editorial `setProperty(..., nullptr)` pattern remains for navigation, clipboard feedback, Patch Mutator state, Compare active, ports, skin, scale, latency, device detection.
- Patch Mutator `MutationHistoryStore` (MUTATE/RETRY/COMPARE navigation) stays separate; editorial undo must not replace mutator history or the M/R combo.
- Mutator Compare (`kCompareActive`, button C) is A/B audition — must not share semantics with Cmd/Ctrl+Z (see `undo-policy.md`).
- Patch load (synth, `.syx` file, Internal Patches navigation) and full patch paste (`pasteFullPatch`) clear the editorial undo stack; loaded/pasted state is the implicit baseline (`undo-policy.md`).
- MUTATE and RETRY clear the editorial undo stack; mutate/retry result is a new baseline (not one undo step per mutate).
- Undo/redo disabled while Mutator Compare is active (`kCompareActive`).
- STORE, SAVE, SAVE AS, bank import/export, and bank transfers are persistent and non-undoable in v1.
- Stack depth counts **transactions** (`beginNewTransaction`), not raw parameter writes. Default **100** transactions with oldest eviction; no user-configurable depth UI in v1.
- v1 delivers keyboard shortcuts only — no Undo/Redo menu items or toolbar buttons unless explicitly added in a later spec revision.
- No host DAW automation undo integration in v1; plugin-internal undo only.
- English-only source; English UI strings; JUCE 9.0.3; builds under `Builds/`; quality gate via `Scripts/quality/lint_touched.py`.
- Update `Documentation/User/manuel-utilisateur.md` §7: remove the no-undo statement; document shortcuts and grouping behavior.

## Non-goals

- Undo of patch/bank navigation or patch selection.
- Undo of Settings preferences (MIDI ports, skin, UI scale, latency).
- Undo of STORE, SAVE, SAVE AS, or bank import/export / transfer.
- Replacing Patch Mutator history navigation with editorial undo.
- Undo of Mutator Compare audition mode.
- Host DAW undo/redo for plugin automation in v1.
- Visible Undo/Redo UI (menus, buttons) in v1.
- User-configurable undo stack depth in v1.
- Patch name inline rename in undo scope until inline rename ships (name display is read-only today).

## Success signal

A user edits a Matrix-1000 patch in Standalone: slider tweak, Init module, Matrix Mod bus change — then presses Cmd/Ctrl+Z repeatedly and hears the synth return to each prior state; Cmd/Ctrl+Shift+Z restores forward steps. Footer dirty state stays consistent with the loaded snapshot. MUTATE and Mutator Compare continue to work without conflating mutator history with editorial undo.

## Assumptions

- Patch name is read-only in the current product (`manuel-utilisateur.md`); editorial undo v1 covers parameter edits only until inline rename exists.
