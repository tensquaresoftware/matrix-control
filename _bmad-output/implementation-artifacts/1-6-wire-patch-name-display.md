---
organization: Ten Square Software
project: Matrix-Control
title: Story 1.6 — Wire Patch Name Display
author: BMad Agent
status: done
story_key: 1-6-wire-patch-name-display
baseline_commit: c12c85752dc3c483019d8d7badf8bd7de61cc82a
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - planning-artifacts/briefs/brief-matrix-control-2026-05-22/brownfield-inventory-for-prd.md
  - implementation-artifacts/1-5-patch-name-bytes-0-7-sync.md
  - implementation-artifacts/4-4-save-with-filename-injection.md
  - implementation-artifacts/4-5-import-name-reconciliation.md
  - implementation-artifacts/6-3-mutationnaming-and-display-names.md
  - project-context.md
created: 2026-07-16
updated: 2026-07-16
---

# Story 1.6: Wire Patch Name Display

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want the PATCH NAME display in Patch Edit to show the current patch name from APVTS,
so that what I see matches PatchModel bytes 0–7 after dumps, `.syx` loads, and Mutator exports (FR-13 display half).

## Acceptance Criteria

1. **Given** Story 1.5 is done (`PatchNameSyncer`, `apvts.state` property `PluginIDs::PatchEditSection::PatchNameModule::kPatchName` = `"patchEditPatchName"`) **When** the Patch Edit middle-row PATCH NAME widget is shown **Then** it displays the current `apvts.state` string for `kPatchName` (not a hardcoded local-only default that never updates).

2. **And** when Core updates the property via `PatchNameSyncer::bufferToApvts()` (Computer Patches load, Internal paste/init, Mutator audition/mutate naming, SAVE AS name injection, synth dump apply, or any existing caller) **Then** the on-screen `PatchNameDisplay` updates on the message thread without requiring editor reopen.

3. **And** on editor/panel construction **Then** the display performs an initial sync from `apvts.state` so a name already present in session state is visible immediately (same pattern as `FooterPanel` ctor → `syncFromApvtsState`).

4. **And** on `valueTreeRedirected` (DAW/session state replace) **Then** the display re-syncs from the new tree root (do not miss restore — `FooterPanel` today only listens to property changes; prefer the fuller `ModuleHeader` / `CompareLockBinder` pattern that also handles redirect).

5. **And** wiring uses a GUI `juce::ValueTree::Listener` on `apvts.state` filtered to `kPatchName` only — **not** `AudioProcessorValueTreeState::Listener`, **not** `InteractiveDisplayApvtsSync`, and **not** a new Core syncer. Call existing `TSS::PatchNameDisplay::setPatchName(...)`.

6. **And** Core remains unchanged for this story: do not modify `PatchNameSyncer`, do not re-encode Matrix 6-bit ASCII in GUI, do not add a `PluginDescriptors` entry for the name (it is metadata on the ValueTree, not an automatable AudioParameter — Story 1.5 design constraint).

7. **And** empty / missing property falls back to `PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kDefaultPatchName` (`"--------"`) — same SSOT as widget ctor and `PluginProcessor::initializePatchNameProperty`.

8. **Out of scope (explicit):** in-place text editing of the name on the display (FR-13 “editable” half), Matrix charset validator UI, keyboard focus, or a `TextEditor`. `PatchNameDisplay` stays paint-only. Editable UX is a follow-up story once display sync is proven.

9. **And** Epic 10 / `InteractiveDisplayApvtsSync` removal is untouched — do not add patch-name logic into that bridge (it only binds envelope/track **int** AudioParameters and will be deleted by Story 10.1).

10. **And** manual verification checklist is documented in Completion Notes: load a known `.syx` (e.g. fixture with non-default name), Internal paste, Mutate once — display matches `patchEditPatchName` / export stem conventions. Existing `PatchNameSyncerTests` remain green; no new unit test required unless a tiny pure helper is extracted (optional).

## Tasks / Subtasks

- [x] **Pass APVTS into `PatchNameDisplayPanel` and bind** (AC: #1–#7, #9)
  - [x] Extend `PatchNameDisplayPanel` ctor to take `juce::AudioProcessorValueTreeState&` (caller already has `apvts` in `PatchEditDisplaysPanel`)
  - [x] Add `ValueTree::Listener` (panel inherits **or** nested private listener class — match `ModuleHeader::PasteEnabledPropertyListener` / `FooterPanel` style)
  - [x] Ctor: `apvts.state.addListener(...)` then immediate sync → `getPatchNameDisplay().setPatchName(...)`
  - [x] Dtor: `removeListener` before APVTS outlives the panel (editor teardown order)
  - [x] `valueTreePropertyChanged`: filter `property.toString() == kPatchName` (or `juce::Identifier` equal to that string)
  - [x] `valueTreeRedirected`: re-sync from current `apvts.state`
  - [x] Sync helper reads `state.getProperty(kPatchName, kDefaultPatchName).toString()` — single place, no duplicated fallbacks

- [x] **Wire call site only** (AC: #5, #9)
  - [x] Update `PatchEditDisplaysPanel` construction of `PatchNameDisplayPanel` to pass `apvts`
  - [x] Do **not** touch `InteractiveDisplayApvtsSync.{h,cpp}`
  - [x] Do **not** change `PatchNameDisplay` public API unless a tiny `getPatchName()` is useful for tests — prefer keeping widget dumb

- [x] **Self-review / guardrails** (AC: #6, #8, #10)
  - [x] No French in source; English comments only if needed
  - [x] No Core ↔ GUI dependency inversion (Core stays unaware of the widget)
  - [x] Confirm Compare lock / grayed UI: display may stay visible while Compare is active (read-only paint is fine); do not block name updates during Compare
  - [x] Completion Notes: manual checklist results + File List

## Dev Notes

### Problem (brownfield gap)

Core FR-13 foundation is **done** (Story 1.5). Loads, saves, and Mutator paths already write `patchEditPatchName` via `PatchNameSyncer::bufferToApvts()`. The GUI widget never observes that property:

| Layer | Status |
|-------|--------|
| `PatchModel` bytes 0–7 | Done (`getName` / `setName`) |
| `PatchNameSyncer` ↔ `apvts.state["patchEditPatchName"]` | Done |
| `PatchNameDisplay::setPatchName` API | Exists, unused in production |
| Production panel binding | **Missing** — widget ctor sticks on `"--------"` |

Brownfield inventory (PO keep widgets): *“Wire `PatchNameDisplay`”*. [Source: `brownfield-inventory-for-prd.md`]

### Data flow after this story

```
PatchModel bytes 0–7
    ↕ PatchNameSyncer (existing — do not reinvent)
apvts.state["patchEditPatchName"]
    → ValueTree::Listener (NEW — GUI only)
PatchNameDisplay::setPatchName → paint
```

### Preferred implementation pattern

**Best references (copy structure, not domain):**

1. `FooterPanel` — panel is `ValueTree::Listener`; ctor `addListener` + sync; dtor `removeListener`; filter properties then update cached strings / repaint. [Source: `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.{h,cpp}`]
2. `ModuleHeader::PasteEnabledPropertyListener` — nested listener + `valueTreeRedirected` → `syncFromState()`. Prefer including **redirect** handling (FooterPanel currently omits it). [Source: `Source/GUI/Widgets/ModuleHeader.cpp`]

**Anti-patterns (do not):**

- Extend `InteractiveDisplayApvtsSync` — wrong type (int AudioParameters), deleted by Epic 10. [Source: `epics.md` Epic 10; architecture E10]
- Invent GUI-side name codec or second syncer
- Store name in a parallel member that can drift from APVTS
- Use `apvts.addParameterListener` — `patchEditPatchName` is **not** an `AudioProcessorParameter`

### Current code — what exists today

**Widget** (`Source/GUI/Widgets/PatchNameDisplay.{h,cpp}`):

- Ctor seeds `patchName_` from `kDefaultPatchName` (`"--------"`).
- `setPatchName(const juce::String&)` assigns + `repaint()`.
- Paint-only — no mouse/keyboard editing.

**Panel** (`.../Modules/PatchNameDisplayPanel.{h,cpp}`):

- Owns `ModuleHeader` (“PATCH NAME”) + `PatchNameDisplay`.
- No APVTS reference today.
- `getPatchNameDisplay()` already public — use it from sync helper.

**Parent** (`PatchEditDisplaysPanel.cpp`):

- Already stores `apvts_` and constructs `PatchNameDisplayPanel` **without** passing APVTS.
- Builds `InteractiveDisplayApvtsSync` for envelopes/track only — leave alone.

**IDs / names (SSOT):**

```cpp
// PluginIDs.h — PatchEditSection::PatchNameModule
constexpr const char* kPatchName = "patchEditPatchName";

// PluginDisplayNames.h — StandaloneWidgets
constexpr const char* kDefaultPatchName = "--------";
```

Story 1.5 docs may still say `"patchName"` — **code and U-IDs rename won**: always use `kPatchName` / `"patchEditPatchName"`.

### Who already writes the property (verify display updates)

These paths call `PatchNameSyncer::bufferToApvts()` (or push helpers that do). After wiring, each must refresh the display automatically via the listener:

- `PatchManagerActionHandler` — load / paste / init / SAVE AS injection
- `PatchMutatorEngine` — mutate / audition naming (`Mxx` / `Mxx-Ryy` from Story 6.3)
- Any other `pushPatchModelToApvtsWithSuppress(..., patchNameSyncer)` call sites

Do **not** add one-off `setPatchName` calls from handlers — that would bypass APVTS SSOT and break session restore.

### Architecture compliance

| Rule | Implication |
|------|-------------|
| Core ↛ GUI | Listener lives under `Source/GUI/.../PatchEditDisplaysPanel/` |
| APVTS SSOT for plugin state | UI observes property; never reads `PatchModel` from GUI |
| JUCE 8.0.12 | `ValueTree::Listener` API is stable; no new dependencies |
| Epic 10 (FR-11/12) | Orthogonal — envelopes/track only |
| FR-13 full | Display half = this story; editable half = follow-up |

### Deferred / do not fix here

From Story 1.5 code review (`deferred-work.md`):

- Model name not seeded from `"--------"` at startup (zero-init buffer → `"@@@@@@@@"` until first load/edit) — Core deferral; UI showing APVTS default is correct.
- `apvtsToBuffer` threading / charset lossiness — out of scope.

### Project structure notes

| Path | Action |
|------|--------|
| `Source/GUI/.../Modules/PatchNameDisplayPanel.h` | UPDATE — APVTS ref + listener |
| `Source/GUI/.../Modules/PatchNameDisplayPanel.cpp` | UPDATE — bind / sync / unbind |
| `Source/GUI/.../PatchEditDisplaysPanel.cpp` | UPDATE — pass `apvts` into name panel ctor |
| `Source/GUI/Widgets/PatchNameDisplay.*` | KEEP (optional no-op) |
| `Source/Core/Models/PatchNameSyncer.*` | KEEP |
| `InteractiveDisplayApvtsSync.*` | KEEP (untouched) |
| `CMakeLists.txt` | No new `.cpp` files expected |

### Testing requirements

- **Automated:** existing `Tests/Unit/PatchNameSyncerTests.cpp` must still pass. GUI unit tests for this panel are not required by project norms unless you extract a pure sync function.
- **Manual (required in Completion Notes):**
  1. Cold start → display shows `"--------"` (or restored session name).
  2. Load Computer Patches `.syx` with a known internal name → display matches reconciled name.
  3. Mutate → display shows `Mxx` / `Mxx-Ryy` style name from Story 6.3.
  4. SAVE AS with injected stem → display matches injected name after `bufferToApvts`.

### Previous story intelligence (1.5)

- Property is on `apvts.state`, not an AudioParameter — GUI must use ValueTree listener.
- Truncation / uppercase / 6-bit charset live in `PatchModel::setName` — GUI only displays the string Core already normalized when coming from buffer; if something writes a long raw property, Core truncates on `apvtsToBuffer` — display may briefly show pre-truncation only if a future editor writes the property without going through syncer (N/A this story).
- ID rename `patchName` → `patchEditPatchName` already applied in code.

### Git intelligence

Recent work is Mutator/Compare/UI scale — not patch-name GUI. Patterns to reuse for listeners: `CompareLockBinder`, `FooterPanel`, Patch Manager panels’ `ValueTree::Listener` implementations.

### Latest tech notes (JUCE 8)

- No `AudioParameterString` in JUCE 8 — ValueTree string properties remain the correct metadata store (unchanged from 1.5).
- `ValueTree::Listener` callbacks run on the thread that mutated the tree; Core already documents `bufferToApvts` as message-thread. Do not add `MessageManager::callAsync` unless you prove a non-message-thread writer for this property.

### Project context reference

Follow `_bmad-output/project-context.md` and `CONVENTIONS.md`: English in source, Core/GUI separation, no global AffineTransform scaling, reuse existing widgets.

## Dev Agent Record

### Agent Model Used

Composer (Cursor Agent)

### Debug Log References

- `cmake --build --preset macos-debug-arm64` — success
- `Matrix-Control_Tests` full suite — exit 0; `PatchNameSyncer Tests` all completed

### Implementation Plan

- Bind GUI-only `ValueTree::Listener` on `PatchNameDisplayPanel` (FooterPanel-style inheritance + `valueTreeRedirected` like CompareLockBinder / ModuleHeader).
- Single `syncFromApvtsState()` helper reads `kPatchName` with `kDefaultPatchName` fallback and calls `PatchNameDisplay::setPatchName`.
- Pass `apvts` from `PatchEditDisplaysPanel` only; leave Core syncer and envelope bridge untouched.

### Completion Notes List

- Wired `PatchNameDisplayPanel` to `apvts.state["patchEditPatchName"]` via `ValueTree::Listener` with initial sync, property filter, redirect re-sync, and dtor unbind.
- No Core changes; `PatchNameSyncer` and `InteractiveDisplayApvtsSync` diffs empty; widget API unchanged.
- Compare lock: panel does not suppress name updates; paint-only display remains fine under Compare alpha/intercept handling on ancestors.
- **Manual checklist (for human UAT):**
  1. Cold start → display shows `"--------"` (or restored session name).
  2. Load Computer Patches `.syx` with a known internal name → display matches reconciled name.
  3. Mutate → display shows `Mxx` / `Mxx-Ryy` style name from Story 6.3.
  4. SAVE AS with injected stem → display matches injected name after `bufferToApvts`.
- Automated: full `Matrix-Control_Tests` green including all `PatchNameSyncer Tests`.

### File List

- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/PatchNameDisplayPanel.h`
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/PatchNameDisplayPanel.cpp`
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/PatchEditDisplaysPanel.cpp`
- `_bmad-output/implementation-artifacts/1-6-wire-patch-name-display.md`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`

### Change Log

- 2026-07-16: Implemented GUI APVTS binding for PATCH NAME display; story → review.
- 2026-07-16: Code review — empty-name fallback to `--------`; story → done.

## Suggested Review Order

1. `PatchNameDisplayPanel.{h,cpp}` — listener lifecycle + sync helper
2. `PatchEditDisplaysPanel.cpp` — ctor wiring only
3. Manual checklist vs load / mutate / SAVE AS paths (confirm no handler-side `setPatchName`)
4. Confirm `InteractiveDisplayApvtsSync` and `PatchNameSyncer` diffs are empty

### Review Findings

- [x] [Review][Decision] Keep 6-17 sprint-status entry in the 1-6 File List diff? — Resolved: keep together (user choice 1). Same `sprint-status.yaml` hunk that moves 1-6 → `review` and reopens `epic-1` also adds `6-17-history-prev-next-and-compact-action-labels: ready-for-dev`.
- [x] [Review][Patch] Empty `patchEditPatchName` should fall back to `--------` [PatchNameDisplayPanel.cpp:118] — Fixed: `syncFromApvtsState` coerces empty string to `kDefaultPatchName`. AC #7 requires empty **or** missing → `kDefaultPatchName`.
- [x] [Review][Defer] Public `getPatchNameDisplay()` remains an APVTS bypass [PatchNameDisplayPanel.h:33] — deferred, pre-existing; story kept widget API unchanged; any external `setPatchName` can drift until next listener fire.
