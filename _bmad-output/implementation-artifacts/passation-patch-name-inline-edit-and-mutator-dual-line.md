---
organization: Ten Square Software
project: Matrix-Control
title: Passation — Patch Name inline edit + Mutator dual-line display
author: BMad Help / Guillaume
status: done
created: 2026-07-26
updated: 2026-09-09
priority: high
workflow: bmad-quick-dev (fresh context)
related:
  - PRD FR-13 (editable half deferred by story 1-6)
  - _bmad-output/implementation-artifacts/1-6-wire-patch-name-display.md (AC #8 out of scope = this work)
  - _bmad-output/implementation-artifacts/1-5-patch-name-bytes-0-7-sync.md
  - _bmad-output/implementation-artifacts/6-3-mutationnaming-and-display-names.md
  - _bmad-output/implementation-artifacts/6-11-mutator-export-layout.md
  - Decision log D-025 (filename ↔ patch name on SAVE)
---

# Passation — Patch Name inline edit + Mutator dual-line

Hand-off for a **fresh-context** `bmad-quick-dev` run. Product decisions below are **locked** (Guillaume + BMad Help, 2026-07-26). Do not re-litigate UX (EDIT button vs double-click) unless a hard technical blocker appears.

**Chat language with Guillaume:** French, plain language (project clarity bar).  
**Spec / code / this file:** English.

---

## 1. Executive summary

Complete the deferred half of **FR-13**: make PATCH NAME editable in place, and stop Mutator from destroying the musical patch name by overwriting bytes 0–7 with `Mxx` / `Mxx-Ryy`.

| Piece | Intent |
|---|---|
| **A — Inline rename** | Double-click the patch name → custom caret editor → commit updates model + dirty + live SysEx (`0x0D`) |
| **B — Dual-line display** | Primary line = user patch name; secondary line = Mutator label (`M00`, `M00-R00`, or `INITIAL` in Compare) |
| **C — Mutator naming semantics** | Keep `Mxx` as **file/folder stems** and History combo labels; write the **real** name into SysEx bytes 0–7 |

Ship **A + B + C in one Quick Dev** (same widget / same Mutator name path). Splitting would force a second pass on `PatchNameDisplay`.

---

## 2. Locked product decisions

### 2.1 Edit affordance

- **Double-click** on the primary name line to enter edit mode.
- **No EDIT button** in the module header.
- Interaction model is closer to a hardware LCD name editor than to JUCE `TextEditor` (custom paint + keyboard), while still aligning with the existing “double-click to edit” gesture on the Internal Patches **NumberBox**.

### 2.2 When editing is allowed

| Source | Editable? |
|---|---|
| Device RAM banks (Matrix-1000: 0–1; Matrix-6/6R: all writable banks per `DeviceMemoryLimits`) | Yes |
| Computer / `.syx` load (`PatchLoadContext::Origin::kComputerFile`) | Yes |
| Device **ROM** banks (`DeviceMemoryLimits::isRomBank`) | **No** — double-click is a no-op |

Reuse the same ROM notion as PASTE/STORE gating (`DeviceMemoryLimits::isRomBank`), not a one-off bank≥2 hardcode.

### 2.3 Dirty tracking

- Successful rename (**Enter**) counts as a patch edit → **dirty** (same class of change as a parameter edit for `DirtyPatchTracker`).
- Escape / abandon / focus-loss cancel → **no** dirty change.

### 2.4 MIDI on commit

- There is **no** dedicated “name-only” SysEx.
- Packed bytes 0–7 are the name, but they have **no** Remote Parameter Edit ID (table column empty). Opcode `0x06` parameter `0` is **DCO1 frequency**, not name char 0 — never use `0x06` for rename.
- On Enter: update buffer + APVTS property `patchEditPatchName`, mark dirty, enqueue **Single Patch Data to Edit Buffer (`0x0D`)** with the full 134-byte patch (same live path Mutator / loads already use). Do **not** STORE to a bank slot on rename.

### 2.5 Editor UX (custom caret)

- Fixed **8** character slots (Matrix name length). Pad with spaces as `PatchModel::setName` already does.
- **Caret:** red rectangle around the focused character; when caret “on”, draw that glyph **black** inside the red rect (inverse contrast); when caret “off”, draw the focused glyph in the normal name colour (current red text). Blink ≈ **500 ms on / 500 ms off** (~1 Hz full cycle). Classic text-editor rate — not 1 s on + 1 s off.
- Typing advances caret to the next slot; **Left/Right** move caret; **Enter** commits; **Escape** abandons with no confirmation.
- **Focus lost** (click away, panel change, Mutate/load/nav interrupting): treat as **Escape** (abandon).
- **Forbidden character:** ignore input; show footer **`uiMessageSeverity = error`** (not warning). Clear the message as soon as the user types a valid character or exits edit (Escape/Enter).
- Allowed charset SSOT: reuse `PatchFileNameSanitizer::isAllowedMatrixChar` — `A–Z`, `0–9`, space, `-`, `_`. Input is uppercased (same as `PatchModel::setName`).
- Empty / all-spaces after trim: reject commit or fall back to display default `--------` / sanitizer empty fallback — pick one rule and unit-test it; prefer **keep previous name** on empty commit rather than wiping to blank.
- Secondary Mutator line is **read-only** during edit; only the primary line is edited.

### 2.6 Dual-line layout

Display height today: `Atoms::Heights::kPatchNameDisplay = 72` — fit both lines **inside** this box; do **not** grow the Patch Name module / break Epic U layout asserts unless unavoidable (prefer typography + vertical packing).

| State | Layout |
|---|---|
| No Mutator history / no Compare | Single primary name, **vertically centred** (current look) |
| History present (at least one mutation root) | Primary name shifts **up**; secondary line appears below |
| Compare active (`kCompareActive`) | Secondary line shows literal **`INITIAL`** (not `Mxx`) |

Secondary text colour: current Patch Name red at **~75% opacity** (or equivalent skin token). Add look fields as needed (`PatchNameDisplayLook` secondary colour / smaller font) via skin builders — no magic hex in the widget.

### 2.7 Mutator naming / export (behaviour change)

**Today (to change):** `MutationNaming::applyPatchName` writes `M00` / `M05-R02` into `PatchModel` bytes 0–7; display shows that string; export stems match.

**Target:**

1. **Do not** overwrite user patch name bytes with `Mxx` on Mutate / Retry / Defrag / audition push.
2. History combobox labels and on-disk stems stay `Mxx` / `Mxx-Ryy` via `MutationNaming::formatPatchName` / `formatExportStem`.
3. Exported `.syx` files keep filenames `M00.syx`, `M00-R00.syx`, etc., but **bytes 0–7 inside** carry the **current user name** (e.g. `COLDPAD`).
4. `Initial.syx` likewise carries the current user name in bytes 0–7 at export time (after a rename, not a stale pre-rename string).

### 2.8 Export session folder basename (behaviour change)

**Today:** basename frozen once at first MUTATE (`freezeExportBasename` → `MutationHistoryStore::frozenExportBasename_`) and never updated.

**Target:** on successful user rename, **recompute and store** the frozen basename from `PatchLoadContext::computeExportBasename(newName)` so a later EXPORT uses the new folder name.

**Canonical scenario (acceptance):**

1. Load bank **1** patch **73**, name `WARMPAD` → Patch Name shows `WARMPAD` only.
2. Mutate + one Retry → secondary line `M00-R00` (or `M00` when root selected); History still `M00` / `M00-R00`.
3. EXPORT → folder `WARMPAD @ B1P73` with `Initial.syx` + `M00/…`.
4. Double-click `WARMPAD`, type `COLDPAD`, Enter → dirty + `0x0D`; secondary line unchanged; History unchanged.
5. EXPORT again to same parent (e.g. Desktop) → **new** folder `COLDPAD @ B1P73`; **do not** delete `WARMPAD @ B1P73` (user cleans up).
6. If `COLDPAD @ B1P73` already exists → existing collision modal (Overwrite / Keep / Cancel).

**Computer-file origin note:** `computeExportBasename` currently ignores patch name for `kComputerFile` (uses file stem + `SyxFile`). Prefer extending so rename still refreshes a sensible basename (e.g. sanitized current patch name + ` @ SyxFile` when non-empty). Document the chosen rule in the Quick Dev spec; do not leave device vs file inconsistent without an explicit sentence.

### 2.9 SAVE / SAVE AS

- Unchanged product rule: SAVE / SAVE AS may **inject** the filename stem into patch name bytes (D-025 / stories 4-4 / 4-5).
- If the user renamed to `COLDPAD` then SAVE AS under another stem, **filename injection wins** — expected.

### 2.10 Interruptions while editing

If Mutate, Retry, History audition, Compare, patch load, bank/patch nav, or similar runs while the caret editor is open → **cancel edit** first (Escape semantics), then apply the action.

---

## 3. Current codebase anchors

| Area | Path / symbol |
|---|---|
| Paint-only widget | `Source/GUI/Widgets/PatchNameDisplay.{h,cpp}` |
| Panel + APVTS listen | `…/Modules/PatchNameDisplayPanel.{h,cpp}` — property `PluginIDs::…::kPatchName` (`patchEditPatchName`) |
| Name sync | `Source/Core/Models/PatchNameSyncer.{h,cpp}` |
| Model encode | `PatchModel::getName` / `setName` / `normalizeNameEncoding` (8 chars, uppercase, space pad) |
| Charset helper | `PatchFileNameSanitizer::isAllowedMatrixChar` / `sanitizeToMatrixName*` |
| Double-click precedent | `NumberBox::mouseDoubleClick` → editor (use as gesture precedent only; name editor is custom-paint) |
| Mutator overwrites name | `MutationNaming::applyPatchName` callers in `PatchMutatorEngine` / `HistoryDefragService` |
| Freeze export basename | `PatchMutatorEngine::freezeExportBasename` + `MutationHistoryStore::setFrozenExportBasename` |
| Basename formula | `PatchLoadContext::computeExportBasename` |
| Live full-patch send | existing Mutator / load `0x0D` / `sendPatch` paths — reuse encoder + queue |
| ROM gate | `DeviceMemoryLimits::isRomBank` |
| Compare flag | `PluginIDs::…::PatchMutatorModule::StateProperties::kCompareActive` |
| Footer messages | `uiMessageText` / `uiMessageSeverity` (`error` for illegal char) |
| Display height | `DesignAtoms.h` → `kPatchNameDisplay = 72` |
| Story that deferred edit | `1-6-wire-patch-name-display.md` AC #8 |

Reference MIDI: `_bmad-output/reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md` — bytes 0–7 name (no parameter ID); `06H` remote edit uses front-panel parameter numbers (DCO1 freq = parameter **0** at packed byte 9).

---

## 4. Suggested implementation slices (for Quick Dev plan)

Order is advisory; keep one coherent reviewable change-set.

1. **Mutator stop clobbering name** — remove / no-op `applyPatchName` on mutate/retry/defrag paths that mutate the live buffer; keep format helpers for labels + export stems. Update unit tests that assert bytes 0–7 == `M00`.
2. **Propagate user name into history buffers / export** — ensure export and audition buffers carry current `getName()`; on rename, update live model + refresh names inside stored history snapshots (or apply name at export/audition time from a single SSOT user-name property — prefer one clear ownership story in the spec).
3. **Refresh frozen export basename on rename.**
4. **`PatchNameDisplay` dual-line layout** + Compare `INITIAL` + skin opacity.
5. **Inline editor** (caret, keys, charset, footer error) + ROM gate + commit path (APVTS + `PatchNameSyncer` + dirty + `0x0D`).
6. **Wire panel** — origin/ROM editability, Mutator secondary label source (selected History entry / Compare), cancel-on-interrupt.
7. **Tests** — naming/export basename refresh; charset reject; ROM no-op if testable without GUI; Mutator export bytes 0–7 == user name while file stem stays `Mxx`.

---

## 5. Acceptance checklist (product)

- [ ] FR-13 editable: double-click renames when allowed; ROM no-op.
- [ ] Enter → dirty + `patchEditPatchName` + model bytes + `0x0D` (no bank STORE).
- [ ] Escape / focus loss → no change.
- [ ] Illegal char → ignored + footer **error**; clears on correction / exit.
- [ ] No history: single centred name.
- [ ] With history: primary user name + secondary `Mxx` / `Mxx-Ryy`.
- [ ] Compare on: secondary `INITIAL`.
- [ ] Mutate/Retry no longer set bytes 0–7 to `Mxx`.
- [ ] Export stems/folders under session still `M00/…`; `.syx` **contents** name = user name.
- [ ] Rename WARMPAD→COLDPAD then re-export creates `COLDPAD @ B1P73` without deleting old folder.
- [ ] Collision modal still works if target folder exists.
- [ ] SAVE AS injection still overrides name when used.
- [ ] No French strings in source; English footer copy.

---

## 6. Out of scope

- EDIT header button.
- Per-character `0x06` remote edits for the name.
- Renaming History combo entries away from `Mxx` / `Mxx-Ryy`.
- Auto-deleting previous export session folders after rename.
- Changing Computer Patches reconciliation policy (D-025) beyond staying compatible.
- Growing Patch Name module height / reopening Epic U full layout audit (avoid unless forced).
- DAW automation of patch name (name remains ValueTree property, not AudioParameter — story 1.5 constraint).

---

## 7. Risks / watchouts

| Risk | Mitigation |
|---|---|
| Tests assert `getName() == "M00"` after mutate | Expect widespread test updates in Mutator / export suites |
| Stale names inside history `PatchModel` snapshots after rename | Explicit update-all-snapshots or name-at-export SSOT |
| `0x0D` vs `0x01` confusion | Commit uses edit buffer only |
| Layout regression at UI scale | Stay within 72 px; use TestComponent PatchNameDisplay if useful |
| Footer error left sticky | Clear on valid input and on exit |
| Computer-file basename ignoring patch name | Explicit rule in Quick Dev spec (§2.8) |

---

## 8. How to launch Quick Dev

In a **new** Agent chat:

```text
/bmad-quick-dev

Implement the locked contract in:
_bmad-output/implementation-artifacts/passation-patch-name-inline-edit-and-mutator-dual-line.md

Scope: Patch Name inline rename (double-click) + Mutator dual-line display + stop writing Mxx into bytes 0–7 + refresh export session basename on rename.
Do not reopen EDIT-button vs double-click; decisions are locked in that passation.
```

Recommend: fresh context, then code review after the Quick Dev review phase.

---

## 9. Decision log (this conversation)

| # | Topic | Decision |
|---|---|---|
| 1 | Dirty | Rename = dirty |
| 2 | MIDI | Full patch `0x0D` on commit |
| 3 | Export folder after rename | Update frozen basename; new folder; leave old folder |
| 4 | SAVE | Filename injection still wins on SAVE AS |
| 5 | Illegal char footer | **error** |
| UX | Affordance | Double-click, custom red caret |
| UX | Dual-line | User name + Mutator / `INITIAL` |
| Mutator | Bytes 0–7 | Real name; stems stay `Mxx` |
