---
organization: Ten Square Software
project: Matrix-Control
title: Story 6.12 — Recipe Persistence and Action Enabled States
author: BMad Agent
status: done
baseline_commit: b725b26
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - implementation-artifacts/6-4-mutate-action.md
  - implementation-artifacts/6-5-retry-from-parent-snapshot.md
  - implementation-artifacts/6-6-history-m-and-r-ui-properties.md
  - implementation-artifacts/6-8-compare-mode.md
  - implementation-artifacts/6-9-delete-and-clear-history.md
  - implementation-artifacts/6-10-historydefrag-on-limit.md
  - implementation-artifacts/6-11-mutator-export-layout.md
  - implementation-artifacts/7-4-mutatoractionhandler.md
  - project-context.md
created: 2026-06-20
updated: 2026-06-20
completed: 2026-06-20
---

# Story 6.12: Recipe Persistence and Action Enabled States

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want Amount/Random/toggles persisted and buttons enabled correctly,
so that my recipe survives sessions and UI reflects limits (FR-34, FR-60).

## Acceptance Criteria

1. **Given** Stories 6.4–6.5 and AD-7 persistence boundaries **When** the user saves and reloads a DAW session **Then** Amount, Random, and all ten module enable toggles restore from APVTS **state** properties (`patchMutatorAmount`, `patchMutatorRandom`, `patchMutatorEnableDco1`…`patchMutatorEnableLfo2`) **And** Mutator history does **not** restore (FR-31): `patchMutatorHistoryMList`, `patchMutatorHistoryRList`, `patchMutatorSelectedM`, `patchMutatorSelectedR`, and `patchMutatorCompareActive` are reset to empty-history defaults on session load **And** in-memory `MutationHistoryStore` remains empty after session reload (engine not yet in `PluginProcessor` — test via ValueTree round-trip + engine harness).

2. **And** `PluginProcessor::initializeMutatorRecipeState()` (or equivalent) sets recipe property defaults on first run when properties are missing — mirror `initializeClipboardPasteEnabledProperties()` pattern:
   - Amount / Random → `0` (matches `PluginDescriptorsPatchManager.cpp` `defaultValue`)
   - Module toggles → `false`
   - Called from processor constructor after APVTS init

3. **And** `PatchMutatorPanel` hydrates recipe widgets from APVTS on construction and on `valueTreeRedirected` / property change for recipe IDs:
   - Amount / Random sliders reflect persisted int properties
   - Module toggles reflect persisted bool properties
   - **Do not** break existing write path (sliders/toggles still `setProperty` on user change)

4. **And** `PatchMutatorEngine::refreshActionEnabledMirrors(apvts)` replaces the no-op stub and writes **uiMirror** bool properties (Core → GUI, not action events):
   - `patchMutatorMutateEnabled` — `true` when `peekNextRootIndex()` returns a value (root slot available; includes empty history → first M00 case)
   - `patchMutatorRetryEnabled` — `true` when history non-empty **and** `selectedM >= 0` **and** `peekNextRetryIndex(selectedM)` returns a value
   - `patchMutatorExportEnabled` — `true` when `!historyStore_.isEmpty()`
   - `patchMutatorDeleteEnabled` — `true` when history non-empty **and** `selectedM >= 0`
   - Optional v1: `patchMutatorClearEnabled` — `true` when `!historyStore_.isEmpty()` (not in FR-60 table but matches UX; include unless scope pushback)

5. **And** `refreshActionEnabledMirrors` is invoked at the end of `syncHistoryUiProperties()` (covers mutate, retry, delete, clear, defrag paths that already call sync) **And** from `PluginProcessor` startup after recipe init + history reset so panel shows correct graying before first mutation.

6. **And** `PatchMutatorPanel` listens to uiMirror enabled properties (pattern: `ModuleHeader::PasteEnabledPropertyListener` / `InternalPatchesPanel` clipboard flag) and calls `setEnabled` on MUTATE, RETRY, EXPORT, DELETE (and CLEAR if implemented) **without** duplicating FR-60 predicate logic in GUI.

7. **And** blocked action **footer messages** remain engine-owned (already implemented in 6.4–6.11) — this story does **not** re-implement footers; handler propagation stays **7.4**. Enabled mirrors prevent clicks; engine guards remain defense-in-depth.

8. **And** COMPARE enabled/graying stays as implemented in Story 6.8 (`refreshCompareUiState`) — **do not** move compare rules into `refreshActionEnabledMirrors`.

9. **And** unit tests in `PatchMutatorEngineTests.cpp` cover `refreshActionEnabledMirrors` predicates without GUI:
   - Empty history → mutate enabled, retry/export/delete disabled
   - After first mutate → mutate/retry/export/delete enabled (assuming limits not hit)
   - 100 roots → mutate disabled; export still enabled if history non-empty
   - Full retry slots on selected root → retry disabled for that root
   - After delete last entry → export/delete disabled

10. **And** `PluginProcessorTests` or focused processor test (if harness exists) verifies recipe property round-trip through `getStateInformation` / `setStateInformation` while history properties are stripped/reset.

11. **And** this story delivers **recipe persistence + enabled mirrors + panel hydration/listeners + tests only**:
    - **No** `MutatorActionHandler` wiring (**7.4**)
    - **No** clear-on-patch-load hook (**6.13**)
    - **No** footer propagation in handler (**7.4**)
    - **No** EXPORT folder picker (**7.4**)

## Tasks / Subtasks

- [x] **Add uiMirror property IDs** (AC: #4, #6)
  - [x] Extend `PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties` with `kMutateEnabled`, `kRetryEnabled`, `kExportEnabled`, `kDeleteEnabled`, (`kClearEnabled`)
  - [x] Ensure IDs are **not** registered in `ActionPropertyRegistry`

- [x] **Recipe persistence — processor** (AC: #1, #2, #10)
  - [x] `initializeMutatorRecipeState()` in `PluginProcessor`
  - [x] `stripEphemeralMutatorStateForPersistence(juce::ValueTree& state)` — remove or reset history + compare properties before `getStateInformation` save
  - [x] `resetEphemeralMutatorStateAfterSessionLoad()` — called from `setStateInformation` after `replaceState`
  - [x] Wire `getStateInformation` / `setStateInformation` hooks

- [x] **Implement `refreshActionEnabledMirrors`** (AC: #4, #5, #7)
  - [x] Replace stub in `PatchMutatorEngine.cpp`
  - [x] Call at end of `syncHistoryUiProperties`
  - [x] Extract predicate helpers ≤ 15 lines each

- [x] **Panel recipe hydration + enabled listeners** (AC: #3, #6, #8)
  - [x] `refreshRecipeFromApvts()` — sliders + toggles
  - [x] `ActionEnabledPropertyListener` (or inline listener) for uiMirror props
  - [x] Extend `valueTreePropertyChanged` / `valueTreeRedirected`

- [x] **Unit tests** (AC: #9, #10)
  - [x] Extend `PatchMutatorEngineTests.cpp` — enabled mirror matrix
  - [x] Recipe session round-trip test (processor or ValueTree XML)

- [x] **Self-review**
  - [x] Grep — uiMirror IDs not in `ActionPropertyRegistry`
  - [x] `cmake --build Builds/macOS --target Matrix-Control_Tests`

## Dev Notes

### What Story 6.12 IS — and what it is NOT

| In scope (6.12) | Out of scope |
|---|---|
| FR-34 recipe persistence across DAW sessions | Patch-load history clear (**6.13**) |
| FR-60 uiMirror enabled flags (Core → GUI) | Handler footer propagation (**7.4**) |
| Strip/reset ephemeral history on session save/load | `MutatorActionHandler` implementation (**7.4**) |
| Panel hydration for recipe widgets | COMPARE blink/graying refactor (**6.8** done) |
| `refreshActionEnabledMirrors` implementation | Settings Defrag button enabled state (**7.7** / FR-59) |
| Engine + processor unit tests | `PluginProcessor` `PatchMutatorEngine` construction (**7.4**) |

**Critical boundary:** Engine owns **predicates**; panel owns **widget `setEnabled`** only. Footer text on blocked clicks stays in engine results — handler copies to footer in 7.4.

### FR-60 enabled predicates (implement exactly)

Per `prd.md` FR-60 and engine guard alignment:

| Action | Enabled when | Engine blocked footer (already exists) |
|---|---|---|
| **MUTATE** | `peekNextRootIndex().has_value()` | `kHistoryLimitFooterMessage` + defrag modal |
| **RETRY** | `!isEmpty()` ∧ `selectedM >= 0` ∧ `peekNextRetryIndex(selectedM).has_value()` | empty → `kEmptyHistoryFooterMessage`; limit → `kHistoryLimitFooterMessage` |
| **EXPORT** | `!isEmpty()` | `kEmptyHistoryFooterMessage` (6.11) |
| **DELETE** | `!isEmpty()` ∧ `selectedM >= 0` | `kEmptyHistoryFooterMessage` / `kNoSelectionFooterMessage` |
| **CLEAR** (optional) | `!isEmpty()` | `kEmptyHistoryFooterMessage` |

**MUTATE empty-history nuance:** FR-60 wording says “when root **Mi** is selected”, but FR-55 requires first MUTATE from current patch with empty history. Predicate **`peekNextRootIndex()`** matches engine `mutate()` guard — do **not** require `selectedM >= 0` when store empty.

**Selection source for mirrors:** Use engine `selectedRootIndex_` after `applySelectionFromApvts()` inside `refreshActionEnabledMirrors` (same as mutate/retry).

### `refreshActionEnabledMirrors` algorithm

```
refreshActionEnabledMirrors(apvts):
  applySelectionFromApvts()

  const mutateEnabled = historyStore_.peekNextRootIndex().has_value()
  const exportEnabled = !historyStore_.isEmpty()
  const deleteEnabled = exportEnabled && selectedRootIndex_ >= 0

  bool retryEnabled = false
  if (!historyStore_.isEmpty() && selectedRootIndex_ >= 0)
    retryEnabled = historyStore_.peekNextRetryIndex(selectedRootIndex_).has_value()

  state.setProperty(kMutateEnabled, mutateEnabled, nullptr)
  state.setProperty(kRetryEnabled, retryEnabled, nullptr)
  state.setProperty(kExportEnabled, exportEnabled, nullptr)
  state.setProperty(kDeleteEnabled, deleteEnabled, nullptr)
  state.setProperty(kClearEnabled, exportEnabled, nullptr)  // optional
```

Call site: **last line** of `syncHistoryUiProperties()` before return.

### Recipe persistence model (FR-34, AD-7)

| Data | APVTS class | Persisted in session XML? |
|---|---|---|
| Amount, Random | **state** int properties | **Yes** |
| Module toggles | **state** bool properties | **Yes** |
| History lists/selection | **state** (uiMirror-ish) | **No** (FR-31) |
| Compare active | **state** | **No** on session reload (reset with history) |
| Action enabled mirrors | **uiMirror** | **No** (recomputed by engine/processor) |

**Brownfield:** Patch Manager group uses ValueTree properties, not `AudioProcessorParameter` entries (`ApvtsLayoutBuilder.cpp:68`). Amount/Random descriptors exist in `PluginDescriptorsPatchManager.cpp` but are **not** in `ApvtsLayoutBuilder` parameter layout. Engine already reads recipe via state fallback:

```94:102:Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp
    int readPatchMutatorPercent(juce::AudioProcessorValueTreeState& apvts,
                                const juce::Identifier& propertyId,
                                int defaultValue)
    {
        auto* rawValue = apvts.getRawParameterValue(propertyId);
        if (rawValue != nullptr)
            return juce::jlimit(0, 100, juce::roundToInt(rawValue->load()));

        return juce::jlimit(0, 100, static_cast<int>(apvts.state.getProperty(propertyId, defaultValue)));
```

**Do not** add Amount/Random to APVTS parameter layout in this story — state properties are the authoritative persistence path for Patch Manager (matches 7-4 dev notes).

### Session save/load — ephemeral mutator state

**Problem:** `getStateInformation` copies full `apvts.state` XML. History list properties written by `syncHistoryUiProperties` would persist across DAW sessions, violating FR-31.

**Fix (implement in `PluginProcessor`):**

```cpp
void stripEphemeralMutatorStateForPersistence(juce::ValueTree state)
{
    using S = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;
    state.removeProperty(S::kHistoryMList, nullptr);
    state.removeProperty(S::kHistoryRList, nullptr);
    state.removeProperty(S::kSelectedM, nullptr);
    state.removeProperty(S::kSelectedR, nullptr);
    state.removeProperty(S::kCompareActive, nullptr);
    // uiMirror enabled flags — recompute on load, do not persist
    state.removeProperty(S::kMutateEnabled, nullptr);
    // ... other *Enabled mirrors
}

void resetEphemeralMutatorStateAfterSessionLoad()
{
    using S = ...;
    apvts.state.setProperty(S::kHistoryMList, {}, nullptr);
    apvts.state.setProperty(S::kHistoryRList, {}, nullptr);
    apvts.state.setProperty(S::kSelectedM, -1, nullptr);
    apvts.state.setProperty(S::kSelectedR, S::kSelectedRRootOnly, nullptr);
    apvts.state.setProperty(S::kCompareActive, false, nullptr);
    // If patchMutatorEngine_ exists (7.4+): engine->clearHistory() or ensure store empty
    // For 6.12: store only exists in engine instance — processor reset of APVTS is sufficient for UI
}
```

`getStateInformation`: copy state → strip ephemeral → serialize.  
`setStateInformation`: replace state → `initializeMutatorRecipeState()` → reset ephemeral → (future) engine sync.

### Recipe initialization

```cpp
void PluginProcessor::initializeMutatorRecipeState()
{
    using W = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;
    if (!apvts.state.hasProperty(W::kAmount))
        apvts.state.setProperty(W::kAmount, 0, nullptr);
    if (!apvts.state.hasProperty(W::kRandom))
        apvts.state.setProperty(W::kRandom, 0, nullptr);
    const char* toggles[] = { W::kEnableDco1, W::kEnableDco2, ... W::kEnableLfo2 };
    for (auto* id : toggles)
        if (!apvts.state.hasProperty(id))
            apvts.state.setProperty(id, false, nullptr);
}
```

### Panel changes (`PatchMutatorPanel`)

**Recipe hydration gap (brownfield):** Sliders initialized from descriptor defaults only; toggles write-on-change but never read persisted state. Fix:

1. `refreshRecipeFromApvts()` — read Amount/Random ints + bool toggles; update slider/toggle without triggering feedback loops (use guard flag or `dontSendNotification` pattern if widget API supports it).
2. Call from constructor after widget creation, `valueTreeRedirected`, and `valueTreePropertyChanged` when property is a recipe ID.
3. `ActionEnabledPropertyListener` — listen for `kMutateEnabled`…`kDeleteEnabled`; update button `setEnabled`. Combine with compare active: e.g. `deleteButton_->setEnabled(mirrorEnabled && !compareActive)` if compare should block delete (verify — FR-32 grays History, not action buttons; **leave action buttons enabled under compare unless spec says otherwise**).

**Existing compare logic (preserve):**

```350:365:Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp
void PatchMutatorPanel::refreshCompareUiState()
{
    const bool compareActive = static_cast<bool>(apvts_.state.getProperty(MutatorState::kCompareActive,
                                                                         false));
    // ... compareButton_, history combos ...
}
```

Do **not** fold compare rules into enabled mirrors.

### Existing stub to replace

```664:666:Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp
void PatchMutatorEngine::refreshActionEnabledMirrors(juce::AudioProcessorValueTreeState&)
{
}
```

### File structure (this story)

```
Source/Shared/Definitions/
└── PluginIDs.h                              (UPDATE — uiMirror enabled IDs)

Source/Core/
├── PluginProcessor.h                        (UPDATE — init/strip helpers decl)
├── PluginProcessor.cpp                      (UPDATE — recipe init, session strip/reset)
└── Services/PatchMutator/
    └── PatchMutatorEngine.cpp               (UPDATE — refreshActionEnabledMirrors + hook)

Source/GUI/Panels/.../PatchMutatorPanel.h    (UPDATE — refreshRecipeFromApvts, listener)
Source/GUI/Panels/.../PatchMutatorPanel.cpp  (UPDATE — hydration + enabled listener)

Tests/Unit/
├── PatchMutatorEngineTests.cpp              (UPDATE — enabled mirror tests)
└── PluginProcessorTests.cpp or new test      (UPDATE — recipe persistence round-trip)
```

No new source files unless listener class exceeds ~40 lines (then extract `MutatorActionEnabledListener` in GUI folder).

### Testing requirements

**PatchMutatorEngineTests** — extend `EngineHarness`:

| Test | Setup | Assert APVTS mirrors |
|---|---|---|
| `enabled_emptyHistory` | Default store | mutate=T, retry=F, export=F, delete=F |
| `enabled_afterFirstMutate` | mutate success | mutate=T, retry=T, export=T, delete=T |
| `enabled_rootLimit` | Fill 100 roots | mutate=F, export=T |
| `enabled_retryLimit` | 100 retries on M00 | retry=F for that root |
| `enabled_afterDeleteLast` | delete only root | export=F, delete=F |
| `enabled_afterDefrag` | defrag after limit | mutate=T again |

**Session persistence test:**

1. Set recipe: Amount=42, Random=17, enable DCO1=true
2. Set ephemeral history props (simulate post-mutate)
3. `getStateInformation` → `setStateInformation` on fresh processor
4. Assert recipe restored; history props empty defaults; toggles/sliders match

Run: `cmake --build Builds/macOS --target Matrix-Control_Tests`

### Architecture compliance

- **AD-7** — recipe in session XML; history RAM-only
- **D-031** — recipe = **state**; enabled flags = **uiMirror**; action buttons = **event**
- **Core ↛ GUI** — engine sets properties only; panel listens
- **FR-34** — Amount/Random/toggles persist
- **FR-31** — history excluded from session restore
- **FR-60** — enabled mirrors match predicates; footers already in engine
- English source/comments only
- Functions ≤ 15 lines — extract `computeMutateEnabled`, etc.

### Previous story intelligence (6.11 — done)

- `exportHistory` blocked when empty — mirror predicate `!isEmpty()` for `kExportEnabled`
- Engine ctor injection pattern established; processor wiring still **7.4**
- Footer constants in anonymous namespace — do not duplicate in panel

### Previous story intelligence (6.10 — done)

- After defrag, mutate/retry should re-enable — covered by calling `refreshActionEnabledMirrors` from `syncHistoryUiProperties` post-defrag
- `peekNextRootIndex()` / `peekNextRetryIndex()` are authoritative limit probes

### Previous story intelligence (6.8 — done)

- COMPARE enabled/History graying implemented in panel — **leave unchanged**
- Initial snapshot persists in store across CLEAR — unrelated to session XML history props

### Previous story intelligence (6.6 — done)

- `syncHistoryUiProperties` owns history APVTS mirrors — extend with enabled mirrors at end
- `kSelectedM = -1` when empty — drives delete/export disabled state

### Git intelligence

| Commit | Relevance |
|---|---|
| `b725b26` | Story 6.11 — export guard `isEmpty()`; engine injection |
| `781a091` | Story 6.10 — defrag re-enables mutate/retry |
| `8b2d916` | Story 6.9 — delete/clear empty guards |
| `87a2a97` | Story 6.8 — compare panel UX (orthogonal) |

**Brownfield gaps this story closes:**
- `refreshActionEnabledMirrors` is no-op
- No processor recipe init
- Panel does not hydrate recipe from APVTS
- Session XML would persist history properties if written before save

### Story 7.4 handoff

When `PatchMutatorEngine` is constructed in processor:

```cpp
patchMutatorEngine_->syncHistoryUiProperties(apvts); // includes enabled mirrors
// After session load + resetEphemeralMutatorState:
// engine store is empty; APVTS history props already cleared
```

Handler calls engine methods; enabled mirrors update via existing `syncHistoryUiProperties` — handler does **not** call `refreshActionEnabledMirrors` directly.

### Story 6.13 handoff (awareness)

Patch **load** clears in-memory store — must also call `syncHistoryUiProperties` + enabled mirrors. Session reload (this story) vs patch load (6.13) are separate hooks.

### Latest tech / framework notes

- **JUCE 8.0.12**, **C++17** — `ValueTree::removeProperty`, `copyState` / `replaceState`
- No new dependencies; no web research required

### Project context reference

- `project-context.md` — Core/GUI separation; APVTS taxonomy
- `CONVENTIONS.md` — no French in source
- FR-34, FR-31, FR-60; AD-7; D-031

### References

- [epics.md — Story 6.12]
- [prd.md — FR-31, FR-34, FR-60]
- [addendum.md — Recipe vs History §]
- [architecture.md — AD-7, D-031 uiMirror taxonomy]
- [6-4-mutate-action.md — refreshActionEnabledMirrors stub ownership]
- [6-6-history-m-and-r-ui-properties.md — syncHistoryUiProperties]
- [6-8-compare-mode.md — compare panel UX boundary]
- [6-11-mutator-export-layout.md — export empty guard]
- [7-4-mutatoractionhandler.md — recipe state vs events]
- [PatchMutatorEngine.cpp — buildRecipeFromApvts, syncHistoryUiProperties stub]
- [PatchMutatorPanel.cpp — connectToggleToApvts, refreshCompareUiState]
- [ApvtsLayoutBuilder.cpp — Patch Manager uses properties not parameters]
- [PluginProcessor.cpp — getStateInformation/setStateInformation]

## Dev Agent Record

### Agent Model Used

claude-4.6-sonnet-medium-thinking

### Debug Log References

- Fixed accidental removal of `GUI/PluginEditor.h` include during persistence helper extraction
- JUCE `expectEquals` does not support `bool` — use `expect(a == b)` for mirror assertions

### Completion Notes List

- Added uiMirror enabled property IDs (`kMutateEnabled`…`kClearEnabled`) to `StateProperties`; verified absent from `ActionPropertyRegistry`
- Implemented `MutatorSessionPersistence.h` (shared strip/reset/init helpers) used by `PluginProcessor` and tests
- `getStateInformation` strips ephemeral mutator state; `setStateInformation` restores recipe defaults and clears history/compare
- `refreshActionEnabledMirrors` implements FR-60 predicates; hooked at end of `syncHistoryUiProperties` (including empty-history path)
- `PatchMutatorPanel` hydrates recipe widgets from APVTS and listens to enabled mirrors via `ActionEnabledPropertyListener`
- Processor startup initializes recipe + empty-history enabled mirrors (engine wiring deferred to 7.4)
- 6 enabled-mirror engine tests + 1 session persistence round-trip test; `Matrix-Control_Tests` and `Matrix-Control_Standalone` green

### File List

- `Source/Shared/Definitions/PluginIDs.h`
- `Source/Core/Services/PatchMutator/MutatorSessionPersistence.h` (added)
- `Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp`
- `Source/Core/PluginProcessor.h`
- `Source/Core/PluginProcessor.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.h`
- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp`
- `Tests/Unit/PatchMutatorEngineTests.cpp`
- `Tests/Unit/MutatorRecipePersistenceTests.cpp` (added)
- `CMakeLists.txt`

### Change Log

- 2026-06-20: Story 6.12 — recipe persistence, action enabled uiMirror flags, panel hydration/listeners, unit tests

### Review Findings

- [x] [Review][Decision] Startup mirror logic duplicated outside engine — **Resolved: keep as-is until 7.4** (Guillaume 2026-06-20). `setActionEnabledMirrorsForEmptyHistory()` acceptable; consolidate when engine wired in processor.

- [x] [Review][Patch] Session persistence test bypasses real save/load path [MutatorRecipePersistenceTests.cpp:52] — **Fixed 2026-06-20**: test harness mirrors `PluginProcessor` save/load hooks; asserts ephemeral keys absent from serialized XML.

- [x] [Review][Patch] `refreshRecipeFromApvts` exceeds 15-line limit [PatchMutatorPanel.cpp:340] — **Fixed 2026-06-20**: extracted `hydrateIntSlider`, `hydrateToggleBinding`, `hydrateRecipeTogglesFromApvts`.

- [x] [Review][Patch] Persistence test covers subset of recipe toggles [MutatorRecipePersistenceTests.cpp:59] — **Fixed 2026-06-20**: all ten toggles seeded with alternating values and asserted after round-trip.

- [x] [Review][Defer] Recipe property IDs maintained in three separate lists [MutatorSessionPersistence.h:19, PatchMutatorPanel.cpp:326] — deferred, maintainability debt; add/remove toggle requires three edits.

- [x] [Review][Defer] Engine `MutationHistoryStore` not cleared on session load [PluginProcessor.cpp:578] — deferred, story explicitly scopes engine wiring to 7.4; APVTS ephemeral reset sufficient for UI until then.

- [x] [Review][Defer] Four processor methods are one-line passthroughs [PluginProcessor.cpp:1472] — deferred, mirrors clipboard init pattern; thin wrappers acceptable.

- [x] [Review][Defer] `recipeHydrating_` not RAII-guarded [PatchMutatorPanel.cpp:344] — deferred, no exceptions in JUCE UI callbacks; pre-existing risk class.

- [x] [Review][Defer] No amount/random clamp on session load [MutatorSessionPersistence.h:638] — deferred, engine `readPatchMutatorPercent` clamps at mutation time; matches other APVTS property init patterns.

- [x] [Review][Defer] Combo selection change does not refresh enabled mirrors [PatchMutatorPanel.cpp:339] — deferred, requires engine/handler wiring (7.4) to call `syncHistoryUiProperties` on `kSelectedM` change.

- [x] [Review][Defer] Limit tests inject store directly [PatchMutatorEngineTests.cpp:517] — deferred, spec test table allows harness store manipulation for mirror predicate matrix.
