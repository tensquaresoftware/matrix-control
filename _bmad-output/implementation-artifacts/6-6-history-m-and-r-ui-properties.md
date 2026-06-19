---
organization: Ten Square Software
project: Matrix-Control
title: Story 6.6 — History M and R UI Properties
author: BMad Agent
status: done
baseline_commit: d933ea3
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - implementation-artifacts/6-2-mutationhistorystore-two-level-mr.md
  - implementation-artifacts/6-4-mutate-action.md
  - implementation-artifacts/6-5-retry-from-parent-snapshot.md
  - implementation-artifacts/7-4-mutatoractionhandler.md
  - implementation-artifacts/4-2-combobox-sentinel-states.md
  - project-context.md
created: 2026-06-20
updated: 2026-06-20
---

# Story 6.6: History M and R UI Properties

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want History M and R comboboxes mirroring store state,
so that I can select mutations in the panel (FR-54, UX-DR6).

## Acceptance Criteria

1. **Given** Story 6.2 (`MutationHistoryStore`) and Stories 6.4–6.5 (`mutate()` / `retry()` insert into store) **When** `PatchMutatorEngine::syncHistoryUiProperties()` is implemented **Then** APVTS **state** properties expose list + selection mirrors with stable IDs under `PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties`:
   - `patchMutatorHistoryMList` (`kHistoryMList`) — sorted root labels **M00–M99** from `getSortedRootIndices()` + `MutationNaming::formatRootLabel`
   - `patchMutatorHistoryRList` (`kHistoryRList`) — sorted retry labels **R00–R99** for the **currently selected root**, from `getSortedRetryIndices(selectedM)`
   - `patchMutatorSelectedM` (`kSelectedM`) — int root index **0–99**, or **−1** when history empty
   - `patchMutatorSelectedR` (`kSelectedR`) — int retry index **0–99**, or **`MutationHistoryStore::kRootOnly` (−1)** for root-only (`—`) selection

2. **And** sentinels match FR-54 / addendum § Patch Mutator UI:
   - Empty store → `historyMList` empty string; `selectedM = -1`; History M combobox shows **`<EMPTY>`** (grayed/disabled); History R combobox disabled
   - Non-empty store + root-only selection → History R combobox enabled; first selectable row is sentinel **`—`** (em dash, root **Mi** only) mapped to `selectedR = kRootOnly`
   - Retry rows **R00–R99** follow **`—`** in numeric ascending order
   - **No** `<` `>` navigation buttons on the Mutator history row (D-026)

3. **And** **store → APVTS** sync runs after every history mutation path that changes lists or selection:
   - End of successful `mutate()` — select new root at root-only (`selectedM = newRootIndex`, `selectedR = kRootOnly`)
   - End of successful `retry()` — select new retry under same Mi (`selectedM` unchanged, `selectedR = newRetryIndex`)
   - Future delete/clear/defrag stories will call the same helper — stub hooks OK today but `syncHistoryUiProperties` must be callable after manual store ops in tests
   - `syncHistoryUiProperties` updates `historyMList`, `historyRList`, `selectedM`, `selectedR` and bumps no SysEx (audition is **6.7**)

4. **And** **APVTS → engine** selection round-trip:
   - Private `applySelectionFromApvts()` reads `selectedM` / `selectedR` into `selectedRootIndex_` / `selectedRetryIndex_`
   - Called at start of `mutate()`, `retry()`, and `resolveAuditionBuffer()` so panel-driven selection drives MUTATE/RETRY input without `setAuditionSelection()` in production UI
   - Invalid selection (missing root/retry index) → safe fallback: highest sorted root, root-only — same as existing `resolveAuditionBuffer()` fallback; clamp `selectedM`/`selectedR` written back via sync when store proves selection stale

5. **And** **`PatchMutatorPanel` refactored** from legacy single `patchMutatorHistory` combobox to **two** comboboxes (`historyMComboBox_`, `historyRComboBox_`) on the history row (UX-DR6, D-082):
   - Panel implements `juce::ValueTree::Listener` — refresh M combo on `kHistoryMList` / `kSelectedM` changes; refresh R combo on `kHistoryRList` / `kSelectedM` / `kSelectedR` changes
   - User M selection → writes `kSelectedM` (root index from item userData or mapped id); resets `kSelectedR` to `kRootOnly` when M changes (FR-54: changing **M** updates **R** for that root)
   - User R selection → writes `kSelectedR` (retry index or `kRootOnly` for `—`)
   - **Do not** send SysEx or call engine from panel (AD-5) — property writes only
   - Remove legacy `kHistory` / `patchMutatorHistory` widget id, descriptor entry, and panel wiring

6. **And** list property encoding (normative — pick one, document in code comment):
   - **Recommended:** pipe-separated labels — `historyMList = "M00|M02|M05"`, `historyRList = "—|R00|R02"` (retry list includes leading `—` token when root selected and non-empty history)
   - Parallel int arrays optional via `juce::var` if already used elsewhere — **must** remain Core-serializable without GUI types in engine
   - Display strings for M/R rows come from `MutationNaming::formatRootLabel` / `formatRetryLabel` — **not** hand-formatted in panel

7. **And** **`compareActive` property stub** added as `patchMutatorCompareActive` (`kCompareActive`, bool, default `false`) for Story 6.8 / 7.4 — no COMPARE behavior in this story; panel may ignore until 6.8 grays combos via property listener

8. **And** unit tests in `Tests/Unit/PatchMutatorEngineTests.cpp` cover sync + selection round-trip without GUI:
   - `sync_emptyHistory_emptySentinel` — lists empty, `selectedM == -1`, `selectedR == kRootOnly`
   - `sync_afterInsertRoot_listsAndSelectsNewRoot` — insert M00 (+ optional M02 out of order) → M list sorted `M00|M02`, selection on newly mutated root
   - `sync_retryListForSelectedRoot` — root M05 + retries R00,R02 → R list `—|R00|R02` when M05 selected
   - `sync_changingSelectedM_rebuildsRList` — select M02 vs M05 → distinct `historyRList`
   - `applySelectionFromApvts_drivesResolveAuditionBuffer` — set properties, call `applySelectionFromApvts` path via `mutate()` precondition or test accessor — audition buffer matches selected entry
   - `mutate_success_callsSync` / `retry_success_callsSync` — properties updated after engine action (may assert via package-visible test hook or inspect APVTS in harness)
   - Full `Matrix-Control_Tests` Debug target remains green

9. **And** this story delivers **APVTS mirrors + engine sync + panel two-combo refactor + tests only** — no debounced history audition SysEx (**6.7**), no COMPARE toggle logic (**6.8**), no DELETE/CLEAR store mutations (**6.9**), no `MutatorActionHandler` / `PluginProcessor` engine wiring (**7.4**), no action enabled-state mirrors (**6.12**), no clear-on-patch-load (**6.13**). Legacy `patchMutatorHistory` removed — do not leave dual single/two-level paths.

## Tasks / Subtasks

- [x] **Add APVTS property IDs** (AC: #1, #7)
  - [x] `PluginIDs.h` — `PatchMutatorModule::StateProperties` namespace with `kHistoryMList`, `kHistoryRList`, `kSelectedM`, `kSelectedR`, `kCompareActive`
  - [x] Remove `StandaloneWidgets::kHistory` (`patchMutatorHistory`)
  - [x] `PluginDisplayNames.h` — `kEmptyHistorySentinel = "<EMPTY>"`, `kHistoryRootSentinel` (em dash `—`, distinct from DELETE button label constant), optional `kHistoryM` / `kHistoryR` widget labels if layout needs them
  - [x] `PluginDescriptorsPatchManager.cpp` — replace single History widget descriptor with History M + History R combobox descriptors; drop legacy History entry

- [x] **Implement engine sync + selection apply** (AC: #1–#4, #6)
  - [x] `PatchMutatorEngine.cpp` — implement `syncHistoryUiProperties()` (replace stub at line ~282)
  - [x] Add private `applySelectionFromApvts()`; call from `mutate()`, `retry()`, `resolveAuditionBuffer()`
  - [x] Call `syncHistoryUiProperties(apvts_)` at end of successful `mutate()` / `retry()`
  - [x] Serialize lists via `MutationNaming`; use `PluginIDs` StateProperties constants

- [x] **Refactor `PatchMutatorPanel`** (AC: #2, #5)
  - [x] Replace `historyComboBox_` with `historyMComboBox_` + `historyRComboBox_`
  - [x] Add `ValueTree::Listener`; implement refresh/sync helpers mirroring `ComputerPatchesPanel` patterns (sentinel via `setTextWhenNothingSelected`, not `addItem` for `<EMPTY>` — see Story 4.2)
  - [x] Update `layoutHistoryLine()` — two combos, no nav buttons; adjust `DesignAtoms` / `DimensionFactory` if two 48px combos overflow row (minimum: split existing `kPatchMutatorHistory` width)
  - [x] `onChange` handlers write `kSelectedM` / `kSelectedR` only

- [x] **Unit tests** (AC: #8)
  - [x] Extend `PatchMutatorEngineTests` with sync/selection cases listed in AC #8
  - [x] Run `cmake --build Builds/macOS --target Matrix-Control_Tests`

- [x] **Self-review** (AC: #9)
  - [x] Grep `patchMutatorHistory` — zero remaining references except changelog/history docs
  - [x] Grep `PatchMutatorPanel` — no `MidiManager`, no engine includes, no SysEx
  - [x] Grep `syncHistoryUiProperties` — called after mutate/retry success
  - [x] `ApvtsValidator` / widget registry updated if new widget IDs require registration

### Review Findings

- [x] [Review][Decision] Changement de M dans le panel sans `syncHistoryUiProperties` — **Résolu (1+2)** : patch `syncHistoryUiProperties` pour lire APVTS via `applySelectionFromApvts()` en 6.6 ; UX panel complète (rebuild `kHistoryRList` au changement de M) reportée à 7.4 (processor appelle sync sur `kSelectedM`).

- [x] [Review][Patch] `syncHistoryUiProperties` ignore la sélection APVTS [`PatchMutatorEngine.cpp:330`] — `applySelectionFromApvts()` si `kSelectedM` APVTS ≠ interne + comment 7.4
- [x] [Review][Patch] Test `sync_changingSelectedM_rebuildsRList` n'utilise pas `kSelectedM` APVTS [`PatchMutatorEngineTests.cpp:622`]
- [x] [Review][Patch] Test `sync_retryListForSelectedRoot` ne vérifie pas le sentinel `—` ni l'ordre complet [`PatchMutatorEngineTests.cpp:599`]
- [x] [Review][Patch] Panel inclut `MutationHistoryStore.h` pour `kRootOnly` [`PatchMutatorPanel.cpp:249`] — `PluginIDs::kSelectedRRootOnly`
- [x] [Review][Patch] `compareActive` absent jusqu'au premier `sync` [`PatchMutatorEngine.cpp:310`] — init panel constructor

- [x] [Review][Defer] `auditionSelectedHistoryEntry()` stub vide — story 6.7
- [x] [Review][Defer] `syncHistoryUiProperties` non appelé après delete/clear/export — stories 6.9+
- [x] [Review][Defer] Migration preset `patchMutatorHistory` → M/R — brownfield, pas de story migration
- [x] [Review][Defer] `resolveAuditionBuffer` non-const + side-effect `applySelectionFromApvts` — conforme AC#4
- [x] [Review][Defer] `applySelectionFromApvts` skip si `kSelectedM` absent — documenté pour tests 6.4/6.5
- [x] [Review][Defer] `resolveSelectedRootIndex()` ne appelle pas `applySelectionFromApvts` — contrat caller, safe via `retry()` today
- [x] [Review][Defer] `kCompareActive` non lu par le panel — story 6.8
- [x] [Review][Defer] Pas de tests GUI panel — AC#8 exige tests engine sans GUI
- [x] [Review][Defer] Reload DAW avec mirrors APVTS vs store session vide — story 6.13
- [x] [Review][Defer] APVTS corrompu / labels malformés / coercion non-int — cas limites host/state import
- [x] [Review][Defer] Combos 24px — clipping labels possible, UX mineur
- [x] [Review][Defer] Descriptors vs wiring panel manuel — pattern projet existant

## Dev Notes

### What Story 6.6 IS — and what it is NOT

| In scope (6.6) | Out of scope |
|---|---|
| `syncHistoryUiProperties()` + `applySelectionFromApvts()` | Debounced audition SysEx on selection change (**6.7**) |
| APVTS `historyMList`, `historyRList`, `selectedM`, `selectedR` | COMPARE toggle + blinking label (**6.8**) |
| `compareActive` property stub (default false) | DELETE / CLEAR store ops + sync triggers (**6.9**) |
| Two-combo `PatchMutatorPanel` refactor | Defrag renumber (**6.10**) |
| Remove legacy `patchMutatorHistory` | EXPORT (**6.11**) |
| Engine tests for sync/selection | Action enabled mirrors (**6.12**) |
| Call sync after `mutate()` / `retry()` success | Patch-load history clear (**6.13**) |
| | `MutatorActionHandler` / `PluginProcessor` engine construction (**7.4**) |
| | `auditionSelectedHistoryEntry()` implementation (**6.7**) |

**Critical boundary:** Panel writes **state properties only**. Engine owns store → APVTS list serialization. Handler/processor debounce wiring waits for **7.4** + **6.7** — but property IDs and panel listener must exist so 7.4 can subscribe to `kSelectedM` / `kSelectedR`.

### Normative property contract

| Property ID | C++ constant | Type | Written by | Read by |
|---|---|---|---|---|
| `patchMutatorHistoryMList` | `StateProperties::kHistoryMList` | string (pipe labels) | Engine `syncHistoryUiProperties` | Panel refresh M combo |
| `patchMutatorHistoryRList` | `StateProperties::kHistoryRList` | string (pipe labels, leading `—`) | Engine sync (scoped to selected M) | Panel refresh R combo |
| `patchMutatorSelectedM` | `StateProperties::kSelectedM` | int (−1 or 0–99) | Engine sync + panel M onChange | Engine `applySelectionFromApvts` |
| `patchMutatorSelectedR` | `StateProperties::kSelectedR` | int (`kRootOnly` or 0–99) | Engine sync + panel R onChange | Engine `applySelectionFromApvts` |
| `patchMutatorCompareActive` | `StateProperties::kCompareActive` | bool | **6.8** (stub false in 6.6) | **6.8** panel graying |

Architecture AD-6 names these conceptually `historyMList`, `historyRList`, `selectedM`, `selectedR`, `compareActive` — C++ IDs use `patchMutator*` prefix per U-IDs harmonization (`computerPatchesScanRevision` precedent).

### Sentinel & combobox UX rules (FR-54, D-026)

| Combo | Empty history | Non-empty history |
|---|---|---|
| **History M** | Display `<EMPTY>`, disabled, no popup items | Items = sorted **Mxx** labels; map item → root index in userData or parallel index into parsed list |
| **History R** | Disabled (M empty) | Item 0 = **`—`** → `selectedR = kRootOnly`; then **Rxx** rows sorted numerically |

**Do not** add `<` `>` buttons on Mutator history row (D-026) — unlike Computer Patches (D-075).

**`<EMPTY>` vs legacy `EMPTY`:** PRD FR-54 specifies **`<EMPTY>`** — replace `PluginDisplayNames::kEmptyHistory = "EMPTY"` usage in Mutator panel. (Computer Patches uses `<EMPTY!>` for 8-char Matrix chevron convention — Mutator follows PRD wording.)

**`—` sentinel:** Unicode em dash U+2014 — same character as DELETE button display name today; use a **dedicated** display constant (e.g. `kHistoryRootSentinel`) so DELETE label and History R sentinel can diverge later.

### Store → sync algorithm (implement exactly)

```
syncHistoryUiProperties(apvts):
  roots = historyStore_.getSortedRootIndices()
  if roots.isEmpty():
    set kHistoryMList = ""
    set kHistoryRList = ""
    set kSelectedM = -1
    set kSelectedR = kRootOnly
    selectedRootIndex_ = -1
    selectedRetryIndex_ = kRootOnly
    return

  build M labels via MutationNaming::formatRootLabel for each root
  set kHistoryMList = join(labels, "|")

  m = selectedRootIndex_  // after validating against store; fallback highest root if invalid
  if m < 0: m = roots.getLast()
  validate selectedRetryIndex_ against getSortedRetryIndices(m); invalid → kRootOnly

  rLabels = ["—"] + formatRetryLabel for each retry index on m
  set kHistoryRList = join(rLabels, "|")
  set kSelectedM = m
  set kSelectedR = selectedRetryIndex_
```

After **MUTATE** success: `selectedRootIndex_ = newRootIndex`, `selectedRetryIndex_ = kRootOnly`, then sync.

After **RETRY** success: keep `selectedRootIndex_`, `selectedRetryIndex_ = newRetryIndex`, then sync.

### APVTS → engine selection apply

```
applySelectionFromApvts():
  m = apvts.state.getProperty(kSelectedM, -1)
  r = apvts.state.getProperty(kSelectedR, kRootOnly)
  if m < 0 || !historyStore_.hasRoot(m):
    selectedRootIndex_ = -1
    selectedRetryIndex_ = kRootOnly
    return
  selectedRootIndex_ = m
  if r == kRootOnly:
    selectedRetryIndex_ = kRootOnly
  else if historyStore_.hasRetry(m, r):
    selectedRetryIndex_ = r
  else
    selectedRetryIndex_ = kRootOnly
```

Call **before** `resolveAuditionBuffer()`, `resolveSelectedRootIndex()`, and at top of `mutate()` / `retry()` so panel selection is authoritative.

Keep package-private `setAuditionSelection()` for unit tests — tests may still use it; production path goes through APVTS properties in harness by setting properties + `applySelectionFromApvts`.

### Panel refresh pattern (mirror Computer Patches)

Reference: `ComputerPatchesPanel::valueTreePropertyChanged` listens to `kScanRevision` → `refreshPatchFileComboBox()`.

For Mutator:

```cpp
void PatchMutatorPanel::valueTreePropertyChanged(..., property) {
  if (property == kHistoryMList || property == kSelectedM)
    refreshHistoryMComboBox();
  if (property == kHistoryRList || property == kSelectedM || property == kSelectedR)
    refreshHistoryRComboBox();
}
```

Parse `historyMList` by splitting on `|`. Map selectedM to combobox selection without feedback loop (`dontSendNotification` when syncing from APVTS).

When user changes M combo: write `kSelectedM`, then write `kSelectedR = kRootOnly` (FR-54: R list resets to root-only for new Mi).

### Legacy removal checklist

| Remove / replace | Replacement |
|---|---|
| `StandaloneWidgets::kHistory` / `patchMutatorHistory` | `kHistoryM` + `kHistoryR` widget IDs (names TBD: `patchMutatorHistoryM`, `patchMutatorHistoryR`) |
| Single `historyComboBox_` | `historyMComboBox_`, `historyRComboBox_` |
| Panel writes `kHistory` as combobox selectedId | Panel writes `kSelectedM` / `kSelectedR` indices |
| `kEmptyHistory = "EMPTY"` in Mutator context | `kEmptyHistorySentinel = "<EMPTY>"` |

Grep entire repo after edit — Story 7.4 explicitly references legacy id; update 7.4 story only if code comments still cite `patchMutatorHistory` (optional doc pass, not blocking).

### File structure (this story)

```
Source/Shared/Definitions/
├── PluginIDs.h                          (UPDATE — StateProperties + widget IDs)
├── PluginDisplayNames.h                 (UPDATE — sentinels, labels)
└── PluginDescriptorsPatchManager.cpp    (UPDATE — two combo descriptors)

Source/Core/Services/PatchMutator/
├── PatchMutatorEngine.h                 (UPDATE — private applySelectionFromApvts)
└── PatchMutatorEngine.cpp               (UPDATE — sync + apply + mutate/retry hooks)

Source/GUI/Panels/.../PatchMutatorPanel.h   (UPDATE — listener, two combos)
Source/GUI/Panels/.../PatchMutatorPanel.cpp (UPDATE — setup/layout/refresh)

Source/GUI/Layout/Design/DesignAtoms.h      (UPDATE if split combo widths)
Source/GUI/Factories/DimensionFactory.cpp   (UPDATE if new width atoms)

Tests/Unit/PatchMutatorEngineTests.cpp     (UPDATE — sync tests)
```

No new CMake targets — existing engine + panel sources already registered.

### Testing requirements

Extend `PatchMutatorEngineTests` harness (reuse `EngineHarness`, fake APVTS from existing tests):

| Test | Key assert |
|---|---|
| `sync_emptyHistory_emptySentinel` | After sync: M list `""`, `selectedM == -1` |
| `sync_afterInsertRoot_listsAndSelectsNewRoot` | `mutate()` or manual insert + sync → `historyMList` contains `M00`, `selectedM == 0` |
| `sync_sortedRoots_numericOrder` | Insert M05, M00, M02 → list order `M00|M02|M05` |
| `sync_retryListForSelectedRoot` | Retries on M05 → R list starts with `—`, includes `R00`, `R02` |
| `sync_changingSelectedM_rebuildsRList` | Change `selectedM` property, sync → R list matches new root's retries |
| `applySelectionFromApvts_drivesAudition` | Set M02+R01 on APVTS, apply, resolve audition buffer bytes match entry |
| `mutate_success_updatesApvtsHistory` | Successful mutate → new root visible in M list |
| `retry_success_updatesApvtsHistory` | Successful retry → new R visible under Mi |

Optional GUI manual check (not CI): open panel, run mutate via test hook or future 7.4 — M/R combos populate.

Run: `cmake --build Builds/macOS --target Matrix-Control_Tests`

### Architecture compliance

- **Core ↛ GUI** — engine uses `PluginIDs` StateProperties only; no panel includes in engine
- **GUI ↛ Core services** — panel does not include `PatchMutatorEngine.h`
- **AD-5** — panel `setProperty` only; no ActionDispatcher in panel
- **AD-6** — sync lives on `PatchMutatorEngine`, not handler
- Path: `Source/Core/Services/PatchMutator/` per AD-6
- English source/comments only
- Message-thread only (JUCE APVTS + panel listener)

### Previous story intelligence

**6.5 (done):** `retry()` pipeline complete; uses `selectedRootIndex_` / `selectedRetryIndex_` internally. `syncHistoryUiProperties` still stub. After retry, selection should land on new **R** entry — add sync call. Do not regress 11 retry tests.

**6.4 (done):** `mutate()` does not call sync today; after insert select new root. `setAuditionSelection()` exists for tests until APVTS path lands.

**6.3 (done):** Use `MutationNaming::formatRootLabel` / `formatRetryLabel` for list strings — do not duplicate `M%02d` formatting in panel.

**6.2 (done):** `getSortedRootIndices()` / `getSortedRetryIndices()` provide numeric sort order for FR-54.

**4.2 (done):** Sentinel display pattern — `setTextWhenNothingSelected` for `<EMPTY>`; disabled look; do not `addItem("<EMPTY>")`.

### Git intelligence

| Commit | Relevance |
|---|---|
| `d933ea3` | Latest — Story 6.5 `retry()` + tests |
| `5284dd9` | Story 6.4 `mutate()` + algorithm |
| `0f2078d` | Story 6.3 `MutationNaming` |

**Brownfield:** `syncHistoryUiProperties` stub at `PatchMutatorEngine.cpp:282-284`. Panel single combo at `PatchMutatorPanel.cpp:213-240`. `PluginProcessor` does **not** construct `PatchMutatorEngine` yet — tests use harness directly; panel refresh can be validated manually or with future processor wiring in 7.4.

### Latest tech / framework notes

- **JUCE 8.0.12**, **C++17** — `juce::ValueTree::Listener` on panel; pipe-separated strings for lists (simple, debuggable in ApvtsLogger)
- No web research required — FR-54 and addendum fully specify two-level UI
- `compareActive` bool property follows existing panel toggle/bool state pattern (`getProperty` / `setProperty`)

### Project context reference

- `project-context.md` — Core/GUI separation; English artifacts; tests under `Tests/Unit/`
- `CONVENTIONS.md` — reuse Computer Patches combobox listener pattern; descriptor-driven IDs
- FR-54 two-level History; D-026 no Mutator nav buttons; D-082 two-level UI replaces flat HISTORY
- UX-DR6 Patch Mutator layout includes History M/R

### References

- [epics.md — Story 6.6]
- [prd.md — FR-54, FR-55, FR-56]
- [addendum.md — Patch Mutator UI two-level History table]
- [.decision-log.md — D-026, D-082]
- [architecture.md — AD-6 History UI binding]
- [6-2-mutationhistorystore-two-level-mr.md — query API, kRootOnly]
- [6-4-mutate-action.md — setAuditionSelection, resolveAuditionBuffer fallback]
- [6-5-retry-from-parent-snapshot.md — selection semantics, out-of-scope APVTS note]
- [7-4-mutatoractionhandler.md — kSelectedM/kSelectedR processor hook, legacy kHistory removal]
- [4-2-combobox-sentinel-states.md — sentinel display pattern]
- [ComputerPatchesPanel.cpp — ValueTree listener + refresh pattern]
- [PatchMutatorPanel.cpp — legacy single combo to replace]
- [PatchMutatorEngine.cpp — sync stub, mutate/retry success paths]

## Dev Agent Record

### Agent Model Used

Composer (Cursor)

### Debug Log References

- `applySelectionFromApvts` skips APVTS read when `kSelectedM` property absent — preserves `setAuditionSelection()` for existing unit tests (6.4/6.5).

### Completion Notes List

- Implemented `syncHistoryUiProperties()` with pipe-separated M/R label lists via `MutationNaming`.
- Added `applySelectionFromApvts()`; wired into `mutate()`, `retry()`, and `resolveAuditionBuffer()`.
- Successful `mutate()`/`retry()` update selection and sync APVTS mirrors.
- Refactored `PatchMutatorPanel` to two comboboxes with `ValueTree::Listener` refresh pattern.
- Removed legacy `patchMutatorHistory` widget id; split history combo width 48→24+24 px.
- Added 8 sync/selection unit tests; all 26 PatchMutatorEngine tests pass.
- Built `Matrix-Control_Tests` and `Matrix-Control_Standalone` successfully.

### File List

- Source/Shared/Definitions/PluginIDs.h
- Source/Shared/Definitions/PluginDisplayNames.h
- Source/Shared/Definitions/PluginDescriptorsPatchManager.cpp
- Source/Core/Services/PatchMutator/PatchMutatorEngine.h
- Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.h
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp
- Source/GUI/Layout/Design/DesignAtoms.h
- Source/GUI/Layout/WidgetDimensions.h
- Source/GUI/Factories/DimensionFactory.cpp
- Tests/Unit/PatchMutatorEngineTests.cpp
- _bmad-output/implementation-artifacts/sprint-status.yaml

### Change Log

- 2026-06-20: Story 6.6 — History M/R APVTS mirrors, engine sync, two-combo panel refactor, unit tests.

## Story Completion Status

- Ultimate context engine analysis completed — comprehensive developer guide created
- Status: **review**
