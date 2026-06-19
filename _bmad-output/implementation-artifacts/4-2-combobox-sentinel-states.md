---
organization: Ten Square Software
project: Matrix-Control
title: Story 4.2 — Combobox Sentinel States
author: BMad Agent
status: done
baseline_commit: 5bed772
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - implementation-artifacts/4-1-patchfileservice-folder-scan.md
  - project-context.md
created: 2026-06-19
updated: 2026-06-19
---

# Story 4.2: Combobox Sentinel States

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want EMPTY and SELECT combobox states,
so that I understand when no folder or no file is chosen (FR-26).

## Acceptance Criteria

1. **Given** Story 4.1 complete (`PatchFileService`, OPEN → scan → footer, `getLastScanResult()` cache) **When** Computer Patches panel is shown before any successful folder scan **Then** the patch-file combobox displays **`<EMPTY>`** (grayed / disabled), popup cannot open, and **no hardcoded factory patch names** appear anywhere in the combobox.
2. **And** after OPEN scan with **unusable folder** (`folderUsable == false` — missing path, not a directory, unreadable) or **zero valid files** (`validCount == 0`, including empty folder or folder with only invalid `.syx`) **Then** combobox shows **`<EMPTY>`** (grayed / disabled); footer already set by Story 4.1 (`"Folder not found"` or `"0 files in folder"` / `"N valid, M invalid"` with `N == 0`).
3. **And** after OPEN scan with **`validCount > 0`** and **no file chosen yet** **Then** combobox shows **`<SELECT>`** (active / enabled, not grayed), popup lists **only** validated filenames from `sortedValidFileNames` (ascending lexicographic order from service — same order Story 4.6 will use), and **`<SELECT>` is not a dropdown list item**.
4. **And** when user picks a filename from the popup **Then** combobox shows that filename (active), `computerPatchesSelectPatch` APVTS property updates with the **1-based item id** (index into sorted list + 1) — **no patch load**, **no SysEx**, **no APVTS parameter mutation** beyond the selection property (load deferred to Story 4.5).
5. **And** re-scan via OPEN **resets** combobox to sentinel state: `<EMPTY>` if scan result unusable or zero valid files; `<SELECT>` if `validCount > 0` (clears prior file selection even if the same folder is re-chosen).
6. **And** sentinel strings **`<EMPTY>`** and **`<SELECT>`** live in `PluginDisplayNames::PatchManagerSection::ComputerPatchesModule` — **not** in popup items; chevron convention per D-024 (8 Matrix-compatible characters). Do **not** reuse `StandaloneWidgets::kSelectPatchFile` (`"SELECT A PATCH"`) — that is the widget descriptor label, not the sentinel text.
7. **And** `<` / `>` navigation buttons are **disabled** when combobox is in `<EMPTY>` or `<SELECT>` state (D-075 / FR-52 prep; load behaviour remains Epic 4 stub).
8. **And** `TSS::ComboBox` correctly renders `setTextWhenNothingSelected()` when `selectedId == 0` (see Dev Notes — **mandatory widget fix**).
9. **And** manual smoke passes; no new Core service logic required beyond a scan-revision APVTS bump; full `Matrix-Control_Tests` suite still passes.

## Tasks / Subtasks

- [x] **Add sentinel display names** (AC: #6)
  - [x] `PluginDisplayNames.h` — `ComputerPatchesModule::kEmptySentinel = "<EMPTY!>"`, `kSelectSentinel = "<SELECT>"`
  - [x] Do not change `StandaloneWidgets::kSelectPatchFile` descriptor string

- [x] **Fix `TSS::ComboBox` nothing-selected text** (AC: #8)
  - [x] `ComboBox.cpp` — `getSelectedItemText()` returns `getTextWhenNothingSelected()` when `getSelectedItemIndex() < 0`
  - [x] Verify HeaderPanel port combos still paint correctly (they use item id 1 sentinels — unaffected)

- [x] **Add scan-revision APVTS property** (AC: #3, #5)
  - [x] `PluginIDs.h` — `ComputerPatchesModule::StateProperties::kScanRevision` (e.g. `"computerPatchesScanRevision"`)
  - [x] `PatchManagerActionHandler::handleOpenPatchFolder()` — after `scanFolder` + footer, `setProperty(kScanRevision, juce::Time::getMillisecondCounterHiRes(), nullptr)`
  - [x] Initial value `0` on fresh session (implicit default)

- [x] **Wire `ComputerPatchesPanel` to service + refresh** (AC: #1–#5, #7)
  - [x] Add `const Core::PatchFileService& patchFileService` ctor param; thread from `PluginEditor` → `MainComponent` → `BodyPanel` → `PatchManagerPanel` → `ComputerPatchesPanel` (GUI may include Core — `Core ↛ GUI` preserved)
  - [x] Implement `ValueTree::Listener` on `apvts_.state` — react to `kScanRevision`
  - [x] `refreshPatchFileComboBox()` reads `patchFileService.getLastScanResult()` and applies state machine:
    - `!folderUsable || validCount == 0` → **EmptySentinel**: `clear()`, `setTextWhenNothingSelected(kEmptySentinel)`, `setSelectedId(0)`, `setEnabled(false)`
    - `validCount > 0` && no file selected → **SelectSentinel**: repopulate items ids `1..N` from `sortedValidFileNames`, `setTextWhenNothingSelected(kSelectSentinel)`, `setSelectedId(0, dontSendNotification)`, `setEnabled(true)`
    - file selected → **FileSelected**: `setSelectedId(savedId)` if still in list after rescan; else fall back to SelectSentinel
  - [x] Remove hardcoded 48-name `StringArray` in `setupSelectPatchFileComboBox` (`ComputerPatchesPanel.cpp:202-218`)
  - [x] Remove / replace unused `kSelectPatchFileEmptyId_` — use `0` for no selection; file ids start at `1`
  - [x] Call `refreshPatchFileComboBox()` from ctor (initial `<EMPTY>`) and on scan revision
  - [x] `onChange`: only write `computerPatchesSelectPatch` when `getSelectedId() >= 1`
  - [x] Disable `loadPreviousPatchFileButton_` / `loadNextPatchFileButton_` in EmptySentinel and SelectSentinel states

- [x] **Constructor plumbing** (AC: #1)
  - [x] Update `PatchManagerPanel`, `BodyPanel`, `MainComponent` constructors to pass `const Core::PatchFileService&`
  - [x] `PluginEditor.cpp` — pass `pluginProcessor.getPatchFileService()` into `MainComponent`

- [x] **Self-review** (AC: #9)
  - [x] No patch load / save / folder persistence / prev-next load logic
  - [x] Methods ≤ 15 lines; extract combo state helpers if needed
  - [x] English only in source

## Dev Notes

### What this story IS — and what it is NOT

Story 4.2 replaces the **dev placeholder hardcoded patch list** with a **service-driven combobox** and implements **FR-26 / D-024 sentinel UX**.

It must **NOT** in this story:
- Load `.syx` into `PatchModel`, push APVTS parameters, or enqueue patch **0x01** SysEx (**Story 4.5**)
- Persist folder path or rescan on relaunch (**Story 4.3** — D-010)
- Save / Save As / filename injection (**Story 4.4**)
- Implement Previous/Next **load** or circular navigation (**Story 4.6** — FR-52); only **disable** buttons in sentinel states here
- Name reconciliation (**Story 4.5** — D-025)
- `DirtyPatchTracker` (**Epic 9**)

[Source: Story 4.1 "what it is NOT"; epics.md Story 4.2; addendum § Computer Patches combobox states]

### Sentinel state machine (authoritative)

| State | Condition (`getLastScanResult()`) | Combobox display | Enabled | Popup items | `<` `>` |
|---|---|---|---|---|---|
| **EmptySentinel** | Default / `!folderUsable` / `validCount == 0` | `<EMPTY>` | No (grayed via disabled look) | 0 items | Disabled |
| **SelectSentinel** | `folderUsable && validCount > 0` && no file selected | `<SELECT>` | Yes | N filenames only | Disabled |
| **FileSelected** | User chose item id ≥ 1 still present in list | Filename | Yes | N filenames | Enabled (load still stub) |

**Critical:** `<EMPTY>` and `<SELECT>` are **display-only** via `setTextWhenNothingSelected()` + `setSelectedId(0)`. They must **never** be passed to `addItem()`. This differs from `HeaderPanel` MIDI port combos (sentinel as item id `1`) — do **not** copy that pattern here. [Source: D-024; addendum.md:168]

`SELECT.syx` as a real filename is fine — it appears only as a normal list entry; no collision with the `<SELECT>` sentinel because the sentinel is not in the list.

### Mandatory `TSS::ComboBox` fix

Current custom paint path ignores JUCE's nothing-selected text:

```133:140:Source/GUI/Widgets/ComboBox.cpp
    juce::String ComboBox::getSelectedItemText() const
    {
        const auto selectedIndex = getSelectedItemIndex();
        if (selectedIndex >= 0)
            return getItemText(selectedIndex);

        return juce::String();
    }
```

When `selectedId == 0`, this returns an empty string — sentinels will not render. **Fix:** return `getTextWhenNothingSelected()` in the fallback branch. `canShowPopup()` already requires `isEnabled() && getNumItems() > 0` — EmptySentinel stays closed; SelectSentinel opens with file list only.

### Brownfield state (READ before editing)

| File | Current behaviour | This story changes |
|---|---|---|
| `ComputerPatchesPanel.cpp:202-218` | 48 hardcoded factory names; `setSelectedId(1)` | Service-driven list; sentinel states |
| `ComputerPatchesPanel.h:32` | `kSelectPatchFileEmptyId_ = 1` (unused intent) | Remove; use id `0` = no selection |
| `PatchManagerActionHandler.cpp:240-241` | Scan + footer only | Also bump `kScanRevision` APVTS property |
| `PluginDisplayNames.h:771` | `kSelectPatchFile = "SELECT A PATCH"` (descriptor label) | Add `kEmptySentinel` / `kSelectSentinel`; leave descriptor unchanged |
| `PatchMutatorPanel.cpp:224` | `kEmptyHistory = "EMPTY"` (no chevrons) | Unrelated — Mutator uses different convention |

### Service API consumed (do not duplicate)

```28:29:Source/Core/Services/PatchFileService.h
        PatchFolderScanResult scanFolder(const juce::File& folder);
        const PatchFolderScanResult& getLastScanResult() const noexcept { return lastScan_; }
```

`PatchFolderScanResult` fields used by UI: `folderUsable`, `validCount`, `sortedValidFileNames`. Footer fields already handled in Story 4.1.

### GUI refresh pattern

`ComputerPatchesPanel` cannot pull scan results from APVTS alone (file list is not serialized to properties in 4.1). Use **both**:
1. **`const PatchFileService&`** — read `getLastScanResult()` for list + state
2. **`kScanRevision` APVTS property** — message-thread notification after OPEN (handler writes; panel listens)

Do **not** poll `PatchFileService` on a timer. Do **not** add GUI headers to Core.

### Suggested `refreshPatchFileComboBox()` sketch

```cpp
void ComputerPatchesPanel::refreshPatchFileComboBox()
{
    const auto& scan = patchFileService_.getLastScanResult();
    const int previousId = selectPatchFileComboBox_->getSelectedId();

    selectPatchFileComboBox_->clear(juce::dontSendNotification);

    if (! scan.folderUsable || scan.validCount == 0)
    {
        selectPatchFileComboBox_->setTextWhenNothingSelected(kEmptySentinel);
        selectPatchFileComboBox_->setSelectedId(0, juce::dontSendNotification);
        selectPatchFileComboBox_->setEnabled(false);
        setNavigationButtonsEnabled(false);
        return;
    }

    for (int i = 0; i < scan.sortedValidFileNames.size(); ++i)
        selectPatchFileComboBox_->addItem(scan.sortedValidFileNames[i], i + 1);

    selectPatchFileComboBox_->setTextWhenNothingSelected(kSelectSentinel);
    selectPatchFileComboBox_->setEnabled(true);

    if (previousId >= 1 && previousId <= scan.sortedValidFileNames.size())
        selectPatchFileComboBox_->setSelectedId(previousId, juce::dontSendNotification);
    else
        selectPatchFileComboBox_->setSelectedId(0, juce::dontSendNotification);

    setNavigationButtonsEnabled(selectPatchFileComboBox_->getSelectedId() >= 1);
}
```

Extract helpers (`applyEmptySentinel`, `applySelectSentinel`, `setNavigationButtonsEnabled`) to satisfy Clean Code line limits.

### Architecture compliance

- **GUI → Core OK:** panel holds `const PatchFileService&` reference from composition root (`PluginProcessor`).
- **Core ↛ GUI:** scan revision bump stays in `PatchManagerActionHandler` (Core); no `FileChooser` or panel headers in Core.
- **No APVTS file list:** sorted names remain in service cache until Story 4.3+ needs persistence.
- **Threading:** refresh only from message thread (ValueTree listener, ctor).
- **Descriptor separation:** `PluginDescriptors` / `PluginIDs` widget ids unchanged; only new state property id.

### File structure (this story)

```
Source/GUI/Widgets/
└── ComboBox.cpp                         (UPDATE — getSelectedItemText fallback)

Source/GUI/Panels/.../ComputerPatchesPanel.{h,cpp}  (UPDATE — sentinel FSM, listener, service ref)

Source/GUI/Panels/.../PatchManagerPanel.{h,cpp}     (UPDATE — pass service ref)
Source/GUI/Panels/.../BodyPanel.{h,cpp}             (UPDATE — pass service ref)
Source/GUI/MainComponent.{h,cpp}                      (UPDATE — pass service ref)
Source/GUI/PluginEditor.cpp                           (UPDATE — getPatchFileService())

Source/Core/Actions/
└── PatchManagerActionHandler.cpp        (UPDATE — bump kScanRevision after scan)

Source/Shared/Definitions/
├── PluginDisplayNames.h                 (UPDATE — sentinel strings)
└── PluginIDs.h                          (UPDATE — kScanRevision property)
```

No new `.cpp` in Core/Services. Optional: small GUI unit test for state helper if extracted to a testable free function — not required if logic stays in panel with manual smoke.

### Testing requirements

**Automated:** run full `Matrix-Control_Tests` — no regressions.

**Manual smoke (required):**

1. Fresh plugin → Computer Patches combobox shows **`<EMPTY>`** grayed; no factory names; `<` `>` disabled.
2. OPEN → folder with valid `.syx` → footer counts; combobox **`<SELECT>`** enabled; popup lists only valid names sorted; `<` `>` still disabled.
3. Select a file → combobox shows filename; `<` `>` enabled; **no** patch parameter changes / no SysEx (verify PATCH NAME unchanged).
4. OPEN → empty folder → **`<EMPTY>`** grayed; footer `"0 files in folder"`.
5. OPEN → nonexistent path cancelled vs invalid — cancel: no change; bad folder from test: **`<EMPTY>`** + `"Folder not found"`.
6. Re-scan populated folder → returns to **`<SELECT>`** (selection cleared).
7. Folder containing a file literally named `SELECT.syx` → appears as normal list entry only.

### Previous story intelligence (Story 4.1)

| Learning | Application in 4.2 |
|---|---|
| `getLastScanResult()` cache is the SSOT for sorted filenames | Bind combobox items directly — no second sort |
| OPEN handler already wired; other Computer Patches actions stubbed | Keep `kSelectPatchFile` handler as `return; // Epic 4` — panel may still write property for future stories |
| Footer messages in `PluginDisplayNames::ComputerPatchesModule::FooterMessages` | Do not duplicate footer logic in panel |
| `PatchFileService` zero GUI deps | Inject service ref into panel from `PluginEditor` only |
| Hardcoded combobox explicitly left for 4.2 | **Delete** the 48-name block now |
| Review: synchronous scan on message thread | Combo refresh happens after scan completes — no extra async needed |

### Git intelligence (recent commits)

`5bed772` — Story 4.1 landed `PatchFileService`, OPEN wiring, footer propagation, `getPatchFileService()` on processor. Follow same **constructor injection** and **PluginEditor registration** patterns. Computer Patches combobox was intentionally untouched — this story is the first GUI consumption of scan cache.

### Latest tech information

- **JUCE 8.0.12** — `juce::ComboBox::setTextWhenNothingSelected`, `setSelectedId(0)`, `clear(dontSendNotification)` are the correct sentinel primitives when custom paint delegates to `getSelectedItemText()`.
- **Item ids** — use `1..N` for files; reserve `0` for no selection. Positive ids only in popup (JUCE convention).
- **No new dependencies.**

### Project context reference

- `Core ↛ GUI` strict; GUI may reference Core services [project-context.md]
- Clean Code: methods ≤ 15 lines, classes ≤ 200 lines [CONVENTIONS.md]
- English only in source/comments [project-context.md]
- Do not duplicate `PluginDescriptors` parameter data

### References

- [Source: `_bmad-output/planning-artifacts/epics.md` — Epic 4 Story 4.2, FR-26]
- [Source: `_bmad-output/planning-artifacts/prds/.../prd.md` — FR-26, §4.8 Computer Patches]
- [Source: `_bmad-output/planning-artifacts/prds/.../addendum.md` — § Computer Patches combobox states (D-024), prev/next (D-075)]
- [Source: `_bmad-output/planning-artifacts/prds/.../.decision-log.md` — D-024]
- [Source: `implementation-artifacts/4-1-patchfileservice-folder-scan.md` — service API, deferrals]
- [Source: `Source/GUI/Panels/.../ComputerPatchesPanel.cpp:202-218` — placeholder to remove]
- [Source: `Source/GUI/Widgets/ComboBox.cpp:133-140` — nothing-selected paint bug]
- [Source: `Source/Core/Services/PatchFileService.h` — last scan cache]

## Dev Agent Record

### Agent Model Used

Composer

### Debug Log References

- Build: `cmake --build Builds/macOS/ARM --target Matrix-Control_Tests Matrix-Control_Standalone` — success
- Tests: `Matrix-Control_Tests` — 230 test cases completed; exit 1 from pre-existing JUCE leak assertions at shutdown (no test failures)

### Completion Notes List

- Added `<EMPTY>` / `<SELECT>` sentinel strings in `PluginDisplayNames::ComputerPatchesModule`; descriptor label `"SELECT A PATCH"` unchanged.
- Fixed `TSS::ComboBox::getSelectedItemText()` to render `setTextWhenNothingSelected()` when no item is selected.
- Added `computerPatchesScanRevision` APVTS property; bumped after OPEN scan in `PatchManagerActionHandler`.
- Replaced 48-name hardcoded combobox with service-driven FSM in `ComputerPatchesPanel` (ValueTree listener + `PatchFileService` ref).
- Re-scan resets to `<EMPTY>` or `<SELECT>` and clears `computerPatchesSelectPatch`; user file pick enables `<` `>` nav only.
- Threaded `const PatchFileService&` from `PluginEditor` through MainComponent → BodyPanel → SharedPanel → PatchManagerPanel.

### File List

- Source/GUI/Widgets/ComboBox.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/ComputerPatchesPanel.h
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/ComputerPatchesPanel.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/PatchManagerPanel.h
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/PatchManagerPanel.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/SharedPanel.h
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/SharedPanel.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/BodyPanel.h
- Source/GUI/Panels/MainComponent/BodyPanel/BodyPanel.cpp
- Source/GUI/MainComponent.h
- Source/GUI/MainComponent.cpp
- Source/GUI/PluginEditor.cpp
- Source/Core/Actions/PatchManagerActionHandler.cpp
- Source/Shared/Definitions/PluginDisplayNames.h
- Source/Shared/Definitions/PluginIDs.h

### Change Log

- 2026-06-19: Story 4.2 — combobox sentinel states (`<EMPTY!>` / `<SELECT>`), ComboBox paint fix, scan-revision refresh wiring, removed hardcoded patch list.
- 2026-06-19: Code review — empty sentinel renamed to `<EMPTY!>` (8 chars); stripped `.syx` display confirmed intentional.

### Review Findings

- [x] [Review][Decision] Popup items strip `.syx` extension — resolved: keep stripped display (UX); intentional deviation from AC#3 literal; ids still map to `sortedValidFileNames` order.
- [x] [Review][Patch] Empty sentinel 8 chars — resolved: `kEmptySentinel = "<EMPTY!>"` in `PluginDisplayNames.h` (D-024 compliant).
- [x] [Review][Defer] `kScanRevision` uses `getMillisecondCounterHiRes()` — theoretical duplicate value if two scans complete in the same tick; project-wide timestamp trigger pattern (see deferred-work 5-3). [`PatchManagerActionHandler.cpp:242-245`] — deferred, pre-existing pattern
- [x] [Review][Defer] Lexicographic case-sensitive file sort — `PatchFileService` `sort(false)` from Story 4.1; combobox inherits order. [`PatchFileService.cpp:80`] — deferred, pre-existing
