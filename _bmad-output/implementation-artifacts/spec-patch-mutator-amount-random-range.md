---
title: 'Patch Mutator Amount/Random Range and Defaults'
type: 'feature'
created: '2026-07-15'
status: 'done'
baseline_commit: '82b2799eff7e3c142c76fb8b4444269aa9f670e5'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/planning-artifacts/architecture/architecture-matrix-control-2026-05-25/mutation-algorithm-spec.md'
---

# Patch Mutator Amount/Random Range and Defaults

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Amount and Random allow 0%, and both default to 0%, so MUTATE is a no-op by default (and still a no-op if only Amount is raised while Random stays 0). During Matrix-1000 smoke testing this felt like a broken Mutator. Allowing 0% on the sliders while mutation requires both > 0 is awkward UX.

**Approach:** Keep the current Amount × Random algorithm model. Recalibrate both sliders to **1–100%**, with defaults **Amount = 50%** and **Random = 25%**. Clamp persisted recipe values on session init so old states storing 0 cannot leave the UI and the engine disagreeing. Treat the reported “stuck after Mutate at 0%” as this no-op / defaults trap (investigation found no sticky history or button-disable corruption on the early-exit path).

## Boundaries & Constraints

**Always:**
- Keep Amount × Random multiplication semantics and engine early-exit when either percent is 0 (defensive for API/tests/legacy state).
- UI slider range is exactly 1–100 for both Amount and Random; user cannot select 0% from the panel.
- Missing recipe properties initialize to Amount 50 / Random 25; existing properties clamp into [1, 100] on `initializeRecipeState` (write-back into state).
- Panel slider construction reads `PatchMutatorModule::kIntParameters` (not `ApvtsFactory::getAllIntParameters()` fallbacks, which miss these IDs today).
- Unit tests that intentionally pass 0 for no-op golden vectors / engine guards remain valid.

**Ask First:**
- Changing module-enable toggle default-on behavior (today all false — a separate “Mutate does nothing” feel if no modules are enabled).
- Greying MUTATE/RETRY based on recipe, or changing Random to mean “jitter on top of a guaranteed change”.
- Raising UI/engine clamp maximum above 100 or below 1.

**Never:**
- Change int/choice mutation curves (spread / pMutate formulas).
- Remove or invert GV-01 / `mutate_noOpRecipe_blocked` coverage of programmatic 0.
- Touch History, COMPARE, EXPORT, Defrag, or FR-60 enabled-mirror predicates beyond what recipe init already does.
- Translate UI footer strings to French in this change.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Fresh session | Amount/Random properties missing | State gets Amount=50, Random=25; sliders show those values | N/A |
| Legacy session | Amount=0 and/or Random=0 persisted | Init clamps each to 1 and writes back; sliders hydrate to clamped values | Engine never sees 0 from hydrate path |
| Panel drag | User tries to drag below 1% | Thumb stops at 1%; state stores ≥ 1 | N/A |
| Happy mutate | Amount=50, Random=25, ≥1 module on | MUTATE can apply changes (non-no-op recipe) | History/footer only on real outcomes |
| Programmatic zero | Engine/tests pass amount or random 0 | Still no-op + existing footer path | Early-exit unchanged |
| Out-of-range persist | Amount=150 or Random=-3 | Clamp to [1, 100] on init | N/A |

</frozen-after-approval>

## Code Map

- `Source/Shared/Definitions/PluginDescriptorsPatchManager.cpp` -- SSOT `kIntParameters` min/default for Amount/Random (today 0/0/100)
- `Source/Core/Services/PatchMutator/MutatorSessionPersistence.h` -- `initializeRecipeState` missing-prop defaults (today 0); must also clamp existing values
- `Source/GUI/.../PatchMutatorPanel.cpp` -- slider `SliderConfig` lookup (today falls back to 0–100/0); `hydrateIntSlider` default 0
- `Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp` -- `readPatchMutatorPercent` clamp 0–100; `buildRecipeFromApvts` default args 0; no-op footer copy
- `Source/Core/Services/PatchMutator/MutationAlgorithm.cpp` -- `clampPercent` 0–100; early-exit at 0 (keep)
- `Tests/Unit/MutatorRecipePersistenceTests.cpp` -- recipe init / round-trip coverage
- `Tests/Unit/PatchMutatorEngineTests.cpp` -- keep `*_noOpRecipe_blocked`; add/adjust if defaults or clamp affect setup
- `Tests/Unit/MutationAlgorithmTests.cpp` -- keep `gv01_*Zero_noOp`
- `_bmad-output/planning-artifacts/architecture/.../mutation-algorithm-spec.md` -- §2.1 range/defaults; note UI min 1 vs algorithm still accepting 0

## Tasks & Acceptance

**Execution:**
- [x] `Source/Shared/Definitions/PluginDescriptorsPatchManager.cpp` -- Set Amount/Random `minValue=1`, `defaultValue=50` / `25`, keep `maxValue=100` -- descriptor SSOT
- [x] `Source/Core/Services/PatchMutator/MutatorSessionPersistence.h` -- Missing props → 50/25; clamp any existing Amount/Random into [1,100] with write-back -- closes legacy-0 trap
- [x] `Source/GUI/.../PatchMutatorPanel.cpp` -- Build Amount/Random sliders from `PatchMutatorModule::kIntParameters`; hydrate defaults match descriptors; ensure hydrate after clamp sees ≥1 -- UI follows SSOT
- [x] `Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp` -- Align `readPatchMutatorPercent` / recipe defaults with [1,100] for state reads used by the panel path; keep early-exit at 0; adjust footer wording if it still claims “above 0” as the primary UX (prefer wording that matches reachable UI, e.g. ensure both sliders are set / leave defensive message for API zeros) -- engine/UI consistency
- [x] `Tests/Unit/MutatorRecipePersistenceTests.cpp` (+ engine tests if needed) -- Cover missing defaults 50/25 and clamp 0→1 write-back; keep programmatic no-op tests -- verify matrix
- [x] `_bmad-output/.../mutation-algorithm-spec.md` -- Update §2.1 range/defaults; note UI recipe floor 1 while algorithm early-exit at 0 remains -- planning SSOT

**Acceptance Criteria:**
- Given a fresh plugin state, when the Patch Mutator panel opens, then Amount shows 50% and Random shows 25%, and neither slider can go below 1%.
- Given a restored session with Amount or Random stored as 0, when recipe state is initialized, then each property is written back as 1 (or higher if already ≥1) before hydrate.
- Given Amount=50 and Random=25 with at least one module enabled, when the user clicks MUTATE, then the engine does not take the zero-recipe early-exit path.
- Given tests or callers that still pass amountPercent=0 or randomPercent=0, when `mutate`/`apply` runs, then behavior remains a no-op as today.

## Spec Change Log

## Design Notes

Panel previously looked up mutator ints via `ApvtsFactory::getAllIntParameters()`, which does **not** include Patch Mutator recipe ints. This change reads `PatchMutatorModule::kIntParameters` instead. `refreshRecipeFromApvts` also clamps Amount/Random into [1, 100] with write-back before hydrate so a live out-of-range property cannot leave the thumb and engine disagreeing.

Investigation of the “Mutator stuck after Mutate at 0%” report: `mutate()` no-op return only sets a footer warning and does not disable MUTATE, corrupt history, or skip enabling mirrors. The dominant failure mode with shipping defaults was intentional no-op (Amount × Random product is zero). Clamping + non-zero defaults are the product fix.

Secondary confusion (all module toggles off → “no change” footer with A/R > 0) is deferred — do not flip toggle defaults in this pass.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` -- expected: build succeeds
- `ctest --preset macos-debug-arm64 -R 'MutationAlgorithm|PatchMutatorEngine|MutatorRecipePersistence|MutatorActionHandler' --output-on-failure` -- expected: listed suites pass (adjust filter to match project ctest names if needed)

**Manual checks (if no CLI):**
- Open Patch Mutator: Amount 50%, Random 25%, both floors at 1%.
- Enable ≥1 module, Mutate — expect a new history root (not the zero-recipe footer).
- Optional: load a state that had Amount=0; after reopen, Amount shows ≥1% and Mutate is not locked by a silent 0 in state.

## Suggested Review Order

**Descriptor / session floors**

- Amount/Random SSOT: min 1, defaults 50 / 25.
  [`PluginDescriptorsPatchManager.cpp:325`](../../Source/Shared/Definitions/PluginDescriptorsPatchManager.cpp#L325)

- Missing props → defaults; existing values clamped [1,100] with write-back.
  [`MutatorSessionPersistence.h:13`](../../Source/Core/Services/PatchMutator/MutatorSessionPersistence.h#L13)

**Panel binding**

- Sliders read `PatchMutatorModule::kIntParameters` (not APVTS layout ints).
  [`PatchMutatorPanel.cpp:181`](../../Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp#L181)

- Hydrate re-clamps Amount/Random into state so UI thumb and engine stay aligned.
  [`PatchMutatorPanel.cpp:505`](../../Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp#L505)

**Engine defaults**

- Missing property fallbacks match product defaults; early-exit at 0 unchanged.
  [`PatchMutatorEngine.cpp:785`](../../Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp#L785)

**Docs and tests**

- Architecture §2.1 notes UI floor 1 vs algorithm zero early-exit.
  [`mutation-algorithm-spec.md:49`](../planning-artifacts/architecture/architecture-matrix-control-2026-05-25/mutation-algorithm-spec.md#L49)

- Persistence tests cover missing defaults and legacy 0 / OOR clamp.
  [`MutatorRecipePersistenceTests.cpp:164`](../../Tests/Unit/MutatorRecipePersistenceTests.cpp#L164)

