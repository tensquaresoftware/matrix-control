# Undo Policy — Editorial vs Non-Editorial State

Defines what editorial undo records, what stays on `nullptr` undo manager, and how Patch Mutator, Compare, and patch load interact with the stack.

## Editorial (undoable via APVTS UndoManager)

| Category | Scope | Transaction boundary |
|----------|-------|----------------------|
| Patch parameters | All APVTS patch params → SysEx 0x06 | One per gesture (see below) |
| Master parameters | Master panel APVTS params → SysEx 0x03 | One per gesture (v1 scope) |
| Matrix Mod | Bus contents and reorder | One drag-reorder; bulk bus param writes under suppress during reorder |
| Init module (I) | All params in target module | One transaction per Init action |
| Paste module (P) | All params pasted into module | One transaction per Paste action |

### Gesture grouping rules

| Gesture | One undo transaction when | Undo / redo effect |
|---------|---------------------------|-------------------|
| Slider mouse drag (slide) | `beginChangeGesture` … `endChangeGesture` on that parameter (SliderAttachment / ParameterCell) | Undo → value before drag; redo → value after drag |
| Slider arrow keys | **Not** one undo per keypress. All keypresses while slider has keyboard focus until `focusLost` form **one** transaction | Undo → value before arrow session; redo → value after arrow session |
| Envelope display drag | Full drag session — implementer must not split ENV point drag into five undo steps | Undo → pre-drag ENV params; redo → post-drag |
| Track Generator drag | One curve edit session | Undo → pre-drag TRACK/FM params; redo → post-drag |
| Matrix Mod bus reorder | One completed drag-reorder operation | Undo → pre-reorder bus layout; redo → post-reorder |

**Product rule (slider):** sliding (mouse drag) = one undo step. Arrow keys ≠ one undo per press.

## Non-editorial (never undoable — keep `setProperty(..., nullptr)`)

- Patch/bank selection and Internal Patches navigation indices
- Clipboard action/feedback properties and gray-state enable flags
- Patch Mutator history selection (M/R combo indices), export/delete enable flags
- `kCompareActive` and Compare audition state
- MIDI port selection, device identity, connection state
- Skin, UI scale, latency, Settings persistence
- Modal/dialog transient UI state

## Patch Mutator interactions

| Action | Editorial undo behavior |
|--------|-------------------------|
| MUTATE | Clear editorial undo stack; mutate result is new baseline (not one undo step that reverts mutate) |
| RETRY | Same as MUTATE — clear stack |
| COMPARE (C) | Does not create editorial undo entries; audition only |
| Compare active (`kCompareActive`) | Undo/redo **disabled** while Compare active |
| Slider tweak after mutate | Normal editorial undo — reverts tweak, not mutate history |
| M/R history navigation | Mutator-only; never mixed into editorial stack |

## Checkpoint events (clear editorial undo stack)

Treat as new baseline — stack cleared, no redo into pre-checkpoint state:

- Load patch from synth (current slot)
- Load `.syx` from computer
- Internal Patches navigation selecting a different patch
- Full patch paste (`pasteFullPatch` / Internal Patches PASTE)
- MUTATE / RETRY

`DirtyPatchTracker::captureSnapshot` aligns with checkpoint: snapshot updated at load/paste/mutate baseline.

## Persistent / non-undoable operations

- STORE (synth RAM write)
- SAVE / SAVE AS (file write)
- Bank import / export / transfer
- Master INIT to device (persistent device write)

## Dirty state after undo/redo

- `DirtyPatchTracker` compares live 134-byte patch buffer (incl. name bytes 0–7) to snapshot captured at last load/sync checkpoint.
- After undo restores values that match snapshot → `isDirty` false.
- After undo leaves divergence → `isDirty` true.
- Undo/redo must not corrupt snapshot; only checkpoint events refresh snapshot.
- Use `syncApvtsAndIsDirty` when UI reads dirty from APVTS-driven state.

## Stack depth and memory

- Count **transactions** (`UndoManager::beginNewTransaction`), not individual parameter writes.
- Default limit: **100 transactions**.
- Eviction: drop oldest transactions when limit exceeded.
- Memory scales with number of APVTS parameters changed per transaction (full patch + master + matrix mod worst case). Spike story should measure typical footprint.

## Edge scenario matrix

| Scenario | Expected result |
|----------|-----------------|
| Slider mouse drag (slide) | 1 undo → pre-drag value + MIDI; redo → post-drag |
| Arrow keys on focused slider | 1 undo per focus session (not per key) → pre-session value; redo → post-session |
| Envelope drag | 1 undo → all touched ENV params restored |
| Track Generator drag | 1 undo → FM/TRACK params restored |
| Init module | 1 undo → full module restored |
| Paste module | 1 undo → module restored |
| Matrix Mod reorder | 1 undo → bus contents restored |
| Mutate | Stack cleared; not 200 undo entries |
| Retry | Stack cleared (same as mutate) |
| Compare active + Cmd+Z | Undo disabled |
| Edit after mutate | Undo reverts edit only |
| Load other patch | Stack cleared |
| Patch name rename | Out of scope v1 (read-only UI) |
| Master param change | Undoable (v1 scope) |
| Settings/modal open + Cmd+Z | No interference with text fields |
| Edit → undo to clean | `isDirty` false |

## MIDI resync after undo/redo

- Editorial undo/redo (`performEditorialUndo` / `performEditorialRedo`): begin a ~500 ms granular-MIDI quiet window (`kEditorialUndoRedoGranularMidiQuietMs`), cancel any pending Matrix Mod coalesce timer, keep all `suppress*` flags true through `undo()`/`redo()`, `resyncSynthAfterEditorialUndoRedo()` (full patch via `sendFullPatchForAudition` — 0x0D on Matrix-1000), and `flushDeferredApvtsParameterSync(apvts)` (`copyState()` while suppress still active), cancel coalesce again, then clear suppress. `dispatchPatchOrMatrixModParameterChange` / `dispatchMasterParameterChange` honour the quiet window so deferred APVTS flushes and coalesce cannot emit 0x06 / 0x0B / 0x03 after the resync.
- Live Matrix Mod edits (outside editorial undo/redo): unchanged — coalesced 0x0B per bus (~40 ms).
- Bulk paths (init module, paste module, Matrix Mod reorder): existing suppress + explicit dispatch — no regression (Story 2.10 reorder path).

## User manual

Update `Documentation/Users/manuel-utilisateur.md` §7: document undo/redo shortcuts, grouping (slide vs arrows), checkpoint clears, and Mutator distinction.
