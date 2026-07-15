---
organization: Ten Square Software
project: Matrix-Control
title: Story 6.16 — Matrix Mod Recipe Toggle
author: BMad Agent
status: done
baseline_commit: df0c96ba69985c35622c4ce77183c194fdb13f4e
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/mutation-algorithm-spec.md
  - implementation-artifacts/6-1-mutationalgorithm-specification.md
  - implementation-artifacts/6-4-mutate-action.md
  - implementation-artifacts/6-12-recipe-persistence-and-action-enabled-states.md
  - implementation-artifacts/spec-patch-mutator-history-control-widths.md
  - Source/Core/Services/PatchMutator/MutationAlgorithm.cpp
  - Source/Core/Services/PatchMutator/MutationAlgorithm.h
  - Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp
  - Source/Core/Services/PatchMutator/MutatorSessionPersistence.h
  - Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp
  - Source/GUI/Layout/Design/DesignAtoms.h
  - Source/Shared/Definitions/PluginIDs.h
  - Source/Shared/Definitions/PluginDisplayNames.h
  - project-context.md
created: 2026-07-15
updated: 2026-07-15
---

# Story 6.16: Matrix Mod Recipe Toggle

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

## Story

As a sound designer,
I want a Patch Mutator recipe toggle **MM** that opts Matrix Modulation in or out of MUTATE/RETRY,
so that I can keep modulation routing stable while exploring Patch Edit variations (revises Story 6.1 Option A).

## Acceptance Criteria

1. **Given** the current Option A behavior (Matrix Mod bytes **104–133** always mutate when `A > 0` and `R > 0`, independent of module toggles) **When** this story lands **Then** Matrix Mod mutation is gated solely by a new recipe flag `enableMatrixMod` (`MutationRecipe::enableMatrixMod`) **And** the offset eligibility path remains separate from Patch Edit `isModuleEnabled` (do **not** fold Matrix Mod into D1…L2 `parentGroupId` mapping).

2. **And** `MutationAlgorithm::apply` sets `matrixModScopeActive = recipe.enableMatrixMod` (replace the hardcoded `true` in `MutationAlgorithm.cpp`) **And** when `enableMatrixMod == false`, bytes **104–133** are unchanged by `apply` even if `A > 0`, `R > 0`, and every Patch Edit toggle is on **And** when `enableMatrixMod == true` with `A > 0`, `R > 0` and all Patch Edit toggles off, Matrix Mod bytes **may** still change (MM-only mutation still valid).

3. **And** a new recipe toggle is added to the Patch Mutator panel:
   - Widget ID: `patchMutatorEnableMatrixMod` (`PluginIDs::…::StandaloneWidgets::kEnableMatrixMod`)
   - Display label: `"MM"` (`PluginDisplayNames`)
   - UI control: `TSS::Toggle` (same pattern as D1…L2 — **not** `WidgetFactory` button creation for the toggle itself)
   - Default when property missing: **`false`** (same as other enable toggles)
   - Wired via `connectToggleToApvts` / `hydrateRecipeTogglesFromApvts` / `isRecipeProperty`

4. **And** layout free space for **MM** on **content row 3** (History line), column-aligned under **L2**, without changing SharedColumn width **268**:
   - Change CLEAR display text from `"CLEAR"` → `"CL"`
   - Change `DesignAtoms::…::kPatchMutatorClear` from **44** → **20** (same atom width as DELETE / recipe toggles)
   - Keep `kPatchMutatorExport` at **44** and keep `interControlGap` **4**
   - On History row order (left → right): History combo → COMPARE → DELETE → **CL** → **EXPORT** → **MM**
   - **Geometry check @ scale 1.0:** after CLEAR→20, EXPORT’s right edge is at x=244; L2 and MM share left edge x=248 (width 20 → right edge 268). Preserve 4 px gap between EXPORT and MM.

5. **And** recipe persistence (FR-34) includes the 11th toggle:
   - `MutatorSessionPersistence::initializeRecipeState` lists `kEnableMatrixMod` with default `false`
   - `PatchMutatorEngine::buildRecipeFromApvts` reads it into `MutationRecipe::enableMatrixMod`
   - Session round-trip tests cover the new property alongside the ten existing enables
   - Ephemeral history properties remain stripped/reset (FR-31 unchanged)

6. **And** unit tests are updated for the new gate:
   - Rewrite / replace `gv03_matrixModOnly` so MM mutates **only when** `enableMatrixMod == true` (all Patch Edit toggles still false)
   - Replace `allTogglesOff_matrixModStillMutates` with an explicit assertion that **all Patch Edit off + MM off** → bytes 104–133 unchanged (and ideally Patch Edit range also unchanged)
   - Add (or keep) a case: MM on + Patch Edit off + A,R > 0 → ≥1 Matrix Mod byte changes
   - Add a case: MM off + at least one Patch Edit module on → Matrix Mod range unchanged while that module’s range may change
   - Extend `MutatorRecipePersistenceTests` for the 11th toggle

7. **And** the normative mutation-algorithm spec is amended (same deliverable as code — do not leave Option A as authoritative):
   - File: `_bmad-output/planning-artifacts/architecture/architecture-matrix-control-2026-05-25/mutation-algorithm-spec.md`
   - §2 recipe table + `MutationRecipe`: add `enableMatrixMod` / ID / label `MM`
   - §5.1 / §7.1: Matrix Mod eligibility ≡ `recipe.enableMatrixMod` (still **independent** of D1…L2); document **Option A superseded** by this toggle (call it Option D or “Option A′ — recipe-gated”)
   - Changelog / status note dated **2026-07-15** with owner intent: always-on Matrix Mod was a product mistake; user must opt in via **MM**
   - Golden-vector wording in the spec matches AC #6

8. **And** this story does **not**:
   - Change MUTATE/RETRY/EXPORT/DELETE/CLEAR/COMPARE action semantics beyond the MM gate
   - Change Matrix Modulation panel I/C/P or bus reorder
   - Add an AudioProcessorParameter for MM (state bool property only — same as other enables)
   - Bump Patch Mutator module height or SharedColumn width
   - Fold Matrix Mod into Patch Edit module toggle mapping

## Tasks / Subtasks

- [x] **Amend mutation-algorithm-spec.md** (AC: #1, #7)
  - [x] Supersede Option A with recipe-gated Matrix Mod
  - [x] Add 11th recipe field + update eligibility / golden-vector notes

- [x] **Core recipe + algorithm** (AC: #1, #2, #5)
  - [x] `MutationRecipe::enableMatrixMod = false`
  - [x] `MutationAlgorithm.cpp`: `matrixModScopeActive = recipe.enableMatrixMod`
  - [x] `PatchMutatorEngine::buildRecipeFromApvts` read bool property
  - [x] `MutatorSessionPersistence.h` — append ID to `toggleIds[]`

- [x] **IDs / display names / descriptors** (AC: #3)
  - [x] `PluginIDs.h` — `kEnableMatrixMod = "patchMutatorEnableMatrixMod"`
  - [x] `PluginDisplayNames.h` — `kEnableMatrixMod = "MM"`; `kClear = "CL"`
  - [x] `PluginDescriptorsPatchManager.cpp` — standalone widget entry (same `kButton` taxonomy as peers)

- [x] **Layout atoms + panel** (AC: #3, #4)
  - [x] `DesignAtoms.h` — `kPatchMutatorClear = 20`
  - [x] Confirm `DimensionFactory` / registry still pick up CLEAR width from atoms (no hard-coded 44 left)
  - [x] `PatchMutatorPanel`: create `enableMatrixModToggle_`, wire APVTS, hydrate, skin, `isRecipeProperty`
  - [x] `layoutHistoryLine`: place MM after EXPORT at L2-aligned X; verify 268 net width
  - [x] Keep Random row at five toggles (E1…L2) — MM is **not** a 6th Random-row control

- [x] **Tests** (AC: #6)
  - [x] `MutationAlgorithmTests.cpp` — rewrite Option A assertions
  - [x] `MutatorRecipePersistenceTests.cpp` — round-trip MM
  - [x] Optional: `PatchMutatorEngineTests` recipe helper if it lists toggles explicitly
  - [x] Build + run `Matrix-Control_Tests` for Mutator suites

- [x] **Tracking / product docs** (AC: #7)
  - [x] Append Story 6-16 block to `epics.md` (done by create-story; verify)
  - [x] Optional one-line note in deferred-work / decision log if team usually mirrors FR-30 “ten toggles” → eleven (non-blocking)

- [x] **Self-review**
  - [x] Grep: no remaining `matrixModScopeActive = true`
  - [x] Grep: no leftover `"CLEAR"` display for Mutator (should be `"CL"`)
  - [x] At scale 1.0: History row right edge still 268; MM left == L2 left
  - [x] Default false: opening a session without the property does **not** mutate Matrix Mod until user enables MM

### Review Findings

- [x] [Review][Decision] Co-mingled out-of-scope changes in 6-16 MUST files — **resolved (option 1):** accept co-mingled tree (MM + history-control width oneshot + History `MutationNaming` already in panel) as the reviewed surface; document in Completion Notes / File List.
- [x] [Review][Patch] Document accepted co-mingling in story Completion Notes / File List — note that `DesignAtoms` width reconcile + History `MutationNaming` wiring ship in the same working tree as 6-16 by owner choice (2026-07-15 review).
- [x] [Review][Decision] Sprint status marks `6-15-history-combobox-popup-ux: done` inside the 6-16 artifact diff — **resolved (option 1):** keep `6-15: done`; owner confirms History story is closed in practice with this working tree.
- [x] [Review][Patch] `PatchMutatorEngineTests::setRecipe` still omits `kEnableMatrixMod` — set it explicitly to `false` alongside D1…L2 so engine harnesses cannot inherit a stale true property. [`Tests/Unit/PatchMutatorEngineTests.cpp:201`]
- [x] [Review][Patch] GV-02 / GV-04 rely on `MutationRecipe` default `enableMatrixMod == false` — set the flag explicitly in those tests (and helpers) so deterministic pins stay intention-documented under Option A′. [`Tests/Unit/MutationAlgorithmTests.cpp:138`]
- [x] [Review][Defer] No automated assert for History-row geometry (CLEAR=20, MM×L2 x-alignment @ scale) — deferred, pre-existing manual-UAT pattern; same class as prior Mutator layout ACs
- [x] [Review][Defer] Closed HISTORY `M99-R99` / COMPARE@48 readability — deferred, already tracked under `spec-patch-mutator-history-control-widths` in deferred-work.md
- [x] [Review][Defer] FR-30 / planning “ten module toggles” prose — deferred, already tracked from story 6-16 in deferred-work.md

## Dev Notes

### Product decision — Option A superseded

Story **6.1** / mutation-algorithm-spec §7.1 approved **Option A** (2026-06-19): Matrix Mod always mutates when A,R > 0; **no UI toggle**. Owner decision **2026-07-15**: that was wrong — users must choose. This story **replaces** Option A with an explicit **MM** recipe toggle (default **off**).

**Behavior break (intentional):** after this story, MUTATE/RETRY with A,R > 0 and all former toggles left as-is will **no longer** change bytes 104–133 until **MM** is enabled. Document in spec changelog; no migration that forces MM on.

### Width budget (do not re-derive ad hoc)

SharedColumn / Patch Mutator content width = **268** (`DesignChecks` / ModulationBus cell).

History row @ scale 1.0 **before** this story:

| Control | Width | Trailing gap in step |
|---|---|---|
| Label | 40 | flush (no gap in `labelStep`) |
| History | 56 | +4 |
| COMPARE | 48 | +4 |
| DELETE | 20 | +4 |
| CLEAR | **44** | +4 |
| EXPORT | 44 | (terminal) |
| **Sum** | **268** | |

**After** CLEAR→20: reclaim **24** px → place **MM** (20) with **4** px gap after EXPORT, left-aligned with L2 (`toggleOrigin + 4 * toggleStep` = `152 + 96 = 248`).

Random row stays five toggles ending at 268 (L2). Do **not** add a sixth toggle to `layoutSliderLine` for Random.

Owner layout intent (verbatim intent): CLEAR→CL @ 20 px; EXPORT shifts left; free slot becomes **MM** on row 3 under L2.

### Algorithm path (preserve structure)

```cpp
// MutationAlgorithm.cpp — TODAY (wrong for this story)
const bool matrixModScopeActive = true;

// REQUIRED
const bool matrixModScopeActive = recipe.enableMatrixMod;
```

`isIntDescriptorEligible` / `isChoiceDescriptorEligible` already short-circuit Matrix Mod offsets via `isMatrixModOffset` — keep that; only change the bool source.

Do **not** add Matrix Mod to `isModuleEnabled`’s `parentGroupId` table.

### Persistence pattern (copy 6.12)

| Piece | Storage | Default |
|---|---|---|
| Amount / Random | state int (+ optional param) | 0 |
| D1…L2 enables | state bool only | false |
| **MM enable** | state bool only | **false** |

SSOT init list: `MutatorSessionPersistence.h` `toggleIds[]`. Panel must treat the new ID as a recipe property for hydration listeners.

### Widget / descriptor conventions

- ID prefix `patchMutatorEnable*` keeps `WidgetDimensionRegistry` dimension mapping working without special cases.
- Descriptors use `StandaloneWidgetType::kButton` for enables (existing taxonomy debt with `TSS::Toggle` UI — do not invent a new type in this story).
- Action buttons (CLEAR/EXPORT) stay `widgetFactory.createStandaloneButton`; only the **label** and **atom width** of CLEAR change.

### Files to update (MUST)

| File | Change |
|---|---|
| `Source/Core/Services/PatchMutator/MutationAlgorithm.h` | `enableMatrixMod` |
| `Source/Core/Services/PatchMutator/MutationAlgorithm.cpp` | wire scope flag |
| `Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp` | `buildRecipeFromApvts` |
| `Source/Core/Services/PatchMutator/MutatorSessionPersistence.h` | init list |
| `Source/Shared/Definitions/PluginIDs.h` | ID |
| `Source/Shared/Definitions/PluginDisplayNames.h` | `MM`, `CL` |
| `Source/Shared/Definitions/PluginDescriptorsPatchManager.cpp` | widget entry |
| `Source/GUI/Layout/Design/DesignAtoms.h` | CLEAR width 20 |
| `Source/GUI/Panels/.../PatchMutatorPanel.h` | toggle member |
| `Source/GUI/Panels/.../PatchMutatorPanel.cpp` | create / layout / hydrate |
| `Tests/Unit/MutationAlgorithmTests.cpp` | gate tests |
| `Tests/Unit/MutatorRecipePersistenceTests.cpp` | 11th toggle |
| `.../mutation-algorithm-spec.md` | Option A′ / D |

### Files that MAY need a touch

| File | Why |
|---|---|
| `Source/GUI/Factories/DimensionFactory.cpp` | only if CLEAR width is duplicated (prefer atoms) |
| `Tests/Unit/PatchMutatorEngineTests.cpp` | recipe helper lists |
| `Tests/Unit/WidgetDimensionRegistryTests.cpp` | if CLEAR width asserts exist |
| `Source/GUI/Layout/Design/DesignChecks.h` | only if static asserts encode CLEAR=44 |

### Files to leave alone

- `MutatorActionHandler` (recipe already comes from APVTS/engine)
- Matrix Modulation panel / bus reorder
- History hierarchical ComboBox (6-14 / 6-15)
- EXPORT folder layout (6-11)

### Previous story intelligence

- **6.1 / 6.4:** Option A + golden vectors `gv03_*` and `allTogglesOff_matrixModStillMutates` encode always-on MM — those tests **must** be rewritten, not left green by coincidence.
- **6.12:** Persistence/hydration/`isRecipeProperty` pattern — extend one-for-one.
- **Width reconcile (2026-07-15):** `spec-patch-mutator-history-control-widths.md` — net row width frozen; reclaim only by shrinking CLEAR as specified (do not shrink History/sliders again).

### Git intelligence

Recent commits: hierarchical History ComboBox / popup UX / BMad 6.10. Baseline HEAD at story creation: `df0c96ba…`. Uncommitted Mutator/combo work may exist in the working tree — implement against current tree but keep this story’s diff scoped to MM toggle + CLEAR width/text + spec/tests.

### Project context reference

- Chat French / artifacts English (`project-context.md`)
- No GUI deps in `Source/Core/`
- No French in source
- Prefer existing patterns over new abstractions
- JUCE **8.0.12** — no new libraries

### Testing requirements

```bash
cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests
# run MutationAlgorithmTests + MutatorRecipePersistenceTests (project’s usual test invocation)
```

Manual UAT (scale 100% and at least one non-1.0 scale):

1. MM off, A/R > 0, enable D1 → Patch Edit mutates; Matrix Mod panel values stable on MUTATE
2. MM on, all Patch Edit off, A/R > 0 → Matrix Mod changes; Patch Edit stable
3. Session reload restores MM on/off
4. Row 3: CL (20) → EXPORT → MM under L2; no clip past column edge

### References

- [Source: `_bmad-output/planning-artifacts/architecture/.../mutation-algorithm-spec.md` §5.1, §7.1]
- [Source: `_bmad-output/implementation-artifacts/6-1-mutationalgorithm-specification.md` — Option A sign-off]
- [Source: `_bmad-output/implementation-artifacts/6-12-recipe-persistence-and-action-enabled-states.md`]
- [Source: `_bmad-output/implementation-artifacts/spec-patch-mutator-history-control-widths.md`]
- [Source: `Source/Core/Services/PatchMutator/MutationAlgorithm.cpp` — `matrixModScopeActive`]
- [Source: `Source/GUI/Panels/.../PatchMutatorPanel.cpp` — `layoutHistoryLine` / `layoutSliderLine`]
- [Source: `Source/GUI/Layout/Design/DesignAtoms.h` — `kPatchMutatorClear`]

## Dev Agent Record

### Agent Model Used

Composer (Cursor agent router)

### Debug Log References

- GV-04 byte 13 re-pin: under Option A′ (`enableMatrixMod=false`), Matrix Mod descriptors no longer consume RNG before DCO1 Wave Select → expected value `1` → `2` with seed `0x43484F31`.

### Completion Notes List

- Replaced hardcoded `matrixModScopeActive = true` with `recipe.enableMatrixMod` (default false).
- Added 11th recipe toggle MM on History row after EXPORT (L2-aligned x=248 @ scale 1.0); CLEAR→CL width 20 reclaim.
- Persistence / APVTS / panel hydration extended one-for-one from 6.12 pattern.
- Spec v1.1 documents Option A′ superseding Option A; golden vectors + unit tests updated.
- `Matrix-Control_Tests`: 442 started/completed; MutationAlgorithm + MutatorRecipePersistence suites green.
- Code review (2026-07-15): owner accepted co-mingled working tree — `DesignAtoms` history-control width reconcile (`spec-patch-mutator-history-control-widths`) and History `MutationNaming` submenu wiring remain in the same panel/atoms touch as 6-16; not split out.
- Review patches: `setRecipe` pins `kEnableMatrixMod=false`; `recipeWithOnlyDco1Enabled` sets `enableMatrixMod=false` explicitly for GV-02/GV-04.

### File List

- `_bmad-output/planning-artifacts/architecture/architecture-matrix-control-2026-05-25/mutation-algorithm-spec.md`
- `_bmad-output/implementation-artifacts/deferred-work.md`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`
- `_bmad-output/implementation-artifacts/6-16-matrix-mod-recipe-toggle.md`
- `Source/Core/Services/PatchMutator/MutationAlgorithm.h`
- `Source/Core/Services/PatchMutator/MutationAlgorithm.cpp`
- `Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp`
- `Source/Core/Services/PatchMutator/MutatorSessionPersistence.h`
- `Source/Shared/Definitions/PluginIDs.h`
- `Source/Shared/Definitions/PluginDisplayNames.h`
- `Source/Shared/Definitions/PluginDescriptorsPatchManager.cpp`
- `Source/GUI/Layout/Design/DesignAtoms.h` (also carries history-control width reconcile + popup `kMaxScrollHeight` 200 — accepted co-mingled)
- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.h`
- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp` (also History `MutationNaming` submenu wiring + flush `labelStep` — accepted co-mingled)
- `Tests/Unit/MutationAlgorithmTests.cpp`
- `Tests/Unit/MutatorRecipePersistenceTests.cpp`
- `Tests/Unit/PatchMutatorEngineTests.cpp` (review patch: `kEnableMatrixMod` in `setRecipe`)

### Change Log

- 2026-07-15: Implemented Matrix Mod recipe toggle (Option A′), CLEAR→CL layout reclaim, persistence + tests, spec v1.1.
- 2026-07-15: Code review — co-mingling accepted; engine/algorithm test patches applied; story → done.
