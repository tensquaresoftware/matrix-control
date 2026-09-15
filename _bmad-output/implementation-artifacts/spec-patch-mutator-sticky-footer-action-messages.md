---
title: 'Patch Mutator sticky footer action messages (Delete / Flush / Export)'
type: 'feature'
created: '2026-09-15'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'c5f65f1180ce75fd4f0c33de8611b934fd86b719'
context:
  - '{project-root}/_bmad-output/brainstorming/brainstorm-patch-mutator-playable-calibration-2026-08-26/parking-aide-contextuelle-footer.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After Patch Mutator Delete (D) and Flush (F), the left footer sticky band often stays silent (retry delete and Flush success). Export (E) success only reports a file count, without the destination folder the user just chose. Existing Patch Mutator sticky strings also lack a shared module prefix.

**Approach:** Keep the existing sticky / `uiMessageText` action-feedback path. After confirmed successful Delete and Flush, set clear English INFO footer messages. Enrich Export success with count + destination folder. Prefix **all** Patch Mutator sticky footer texts with `Patch Mutator: ` (new and existing) for module-scoped homogeneity ahead of the contextual-help chantier. Truncate long Export paths in the footer band without losing the folder essence. No hover contextual help.

## Boundaries & Constraints

**Always:**
- Reuse `MutatorActionResult.footerMessage` / `footerSeverity` → `MutatorActionHandler::propagateFooterMessage` → APVTS `uiMessageText` / `uiMessageSeverity`.
- Show Delete / Flush success sticky only after confirmation gate returns true and the engine action succeeds.
- Sticky copy pattern for **all** Patch Mutator sticky messages: `Patch Mutator: <detail>` (badge INFO/WARNING stays separate; do not put `INFO`/`WARNING` inside the stored text).
- Migrate every existing Patch Mutator sticky constant in the same pass (engine internals + `PluginDisplayNames` Mutator Messages used as footers), including Compare locked, empty/no-selection, mutate/retry warnings, history limit, defrag, export cancel / not writable / failed, device-dump Mutator footers if they are Mutator sticky — update tests that assert exact strings.
- Delete retry success: `Patch Mutator: Deleted Mxx-Ryy.` (`MutationNaming::formatPatchName`).
- Delete root cascade success: `Patch Mutator: Deleted Mxx and all retries.`
- Flush success: `Patch Mutator: Mutation history flushed.`
- Export success: `Patch Mutator: Exported N mutation file(s) to <fullPath>.` (folder that actually received the written files).
- Preserve Compare clear-only-if-exact (`clearCompareLockedFooterIfPresent`): update `kCompareLockedFooter` to the prefixed string and keep exact-match clear in sync.
- Keep Core free of GUI includes; English strings via existing engine / `PluginDisplayNames` patterns.
- Long Export paths: middle-truncate in the footer paint path (same spirit as Loaded/Saved), so head + useful end remain.
- Dialog titles/bodies (Flush/Delete confirm, history gate, export collision) are **not** sticky footers — leave them unchanged unless they are already the same string as a sticky constant.

**Never:**
- Hover / focus furtive contextual help (separate chantier).
- Footer band layout redesign, floating tooltips, or GUI-wide help.
- Rework Delete / Flush confirmation dialogs beyond what is needed to show post-success sticky.
- Touch unrelated draft `spec-master-pull-on-connect.md`.
- Prefix Bank Utility / Patch Manager footers that are not Patch Mutator sticky.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Delete root cascade | Confirmed delete of root with retries | Sticky INFO: `Patch Mutator: Deleted Mxx and all retries.` | Cancel confirm → no sticky change from this action |
| Delete retry only | Confirmed delete of a retry | Sticky INFO: `Patch Mutator: Deleted Mxx-Ryy.` | Same cancel rule |
| Flush success | Confirmed Flush with non-empty history | Sticky INFO: `Patch Mutator: Mutation history flushed.` | Cancel confirm → no flush, no success sticky |
| Export success flat / session | Successful write of N files to folder F | Sticky INFO: `Patch Mutator: Exported N mutation file(s) to <F.getFullPathName()>.` | Cancel → `Patch Mutator: Export cancelled.` (prefixed) |
| Export long path | Success message wider than left band | Middle-truncated display; stored text remains full path | N/A |
| Compare enter / exit | Toggle C | Prefixed locked sticky; clear only when text equals that constant | N/A |
| Other Mutator stickies | Empty history, no selection, mutate warnings, defrag, export errors, etc. | Same detail as today, with `Patch Mutator: ` prefix | Unchanged severity |

## Decisions (from planning)

- Module sticky prefix: `Patch Mutator: ` for all Patch Mutator sticky messages.
- PREFIX_SCOPE: full migration of existing Mutator sticky strings in this chantier.
- Delete / Flush / Export success copy fixed as in Always above.

</frozen-after-approval>

## Code Map

- `Source/Core/Services/PatchMutator/PatchMutatorEngineActions.cpp` -- `deleteSelected`: root cascade footer; retry branch needs footer; `clearHistory` needs Flush success footer; Compare sets locked sticky.
- `Source/Core/Services/PatchMutator/PatchMutatorEngineInternal.h` -- all engine sticky `k*Footer*` constants + `formatExportCompleteFooterMessage` (add path + prefix) + `makeExportHistoryResult` + Compare clear helper.
- `Source/Core/Services/PatchMutator/PatchMutatorEngineHistory.cpp` -- pass actual write folder into export formatter; cancelled / defrag footers.
- `Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp` -- mutate / history-limit footers that use shared constants.
- `Source/Core/Actions/MutatorActionHandler.cpp` -- confirm gates then propagate footer.
- `Source/Shared/Definitions/PluginDisplayNames.h` -- `kCompareLockedFooter`, `kExportCancelledFooter`, other Mutator sticky Messages — prefix; keep dialog-only strings unprefixed.
- `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp` -- path-style middle truncate for Export-with-` to ` (including `Patch Mutator: Exported `).
- `Source/Core/Services/PatchMutator/MutationNaming.cpp` -- `formatPatchName` for retry Delete sticky.
- `Tests/Unit/PatchMutatorEngineDeleteClearTests.cpp`, `Tests/Unit/PatchMutatorEngineExportTests.cpp`, and other unit tests asserting Mutator footer exact strings -- update + add retry/flush/export-path coverage.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/Services/PatchMutator/PatchMutatorEngineInternal.h` -- prefix all engine sticky constants; add Flush + retry Delete helpers; export formatter takes destination path -- SSOT for sticky copy.
- [x] `Source/Shared/Definitions/PluginDisplayNames.h` -- prefix Mutator sticky Messages (`kCompareLockedFooter`, `kExportCancelledFooter`, …) -- Compare exact clear stays correct.
- [x] `Source/Core/Services/PatchMutator/PatchMutatorEngineActions.cpp` -- retry Delete footer; Flush success footer; root cascade uses new prefixed construction -- close silent-success gaps.
- [x] `Source/Core/Services/PatchMutator/PatchMutatorEngineHistory.cpp` -- pass write folder into export success formatter -- Export sticky shows destination.
- [x] `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp` -- middle-truncate prefixed Export path messages -- long paths stay readable.
- [x] `Tests/Unit/PatchMutatorEngine*.cpp` (and any exact Mutator footer asserts) -- update prefixes; assert retry/flush/export path -- lock AC.

**Acceptance Criteria:**
- Given a selected retry and confirmed Delete, when delete succeeds, then sticky is `Patch Mutator: Deleted Mxx-Ryy.`
- Given a selected root with retries and confirmed Delete, when delete succeeds, then sticky is `Patch Mutator: Deleted Mxx and all retries.`
- Given non-empty history and confirmed Flush, when flush succeeds, then sticky is `Patch Mutator: Mutation history flushed.`; Cancel leaves history and does not show success sticky.
- Given successful Export of N files to folder F, when export completes, then sticky is `Patch Mutator: Exported N mutation file(s) to <F full path>.`
- Given existing Mutator sticky cases (Compare, empty history, warnings, export cancel, …), when they fire, then detail text is unchanged except for the leading `Patch Mutator: `.
- Given that Export message exceeds left band width, when painted, then detail uses middle truncation without changing stored `uiMessageText`.
- Given Compare exit, when the locked sticky is present, then clear-if-exact still clears the prefixed Compare string only.

## Implementation Notes

- Prefixed all engine `k*Footer*` stickies + `PluginDisplayNames` Mutator sticky Messages (`Compare`, Export cancelled, device-dump Mutator footers); dialog-only strings unchanged.
- Added `formatRetryDeleteFooterMessage`, `formatRootDeleteCascadeFooterMessage`, `kFlushSuccessFooterMessage`; `formatExportCompleteFooterMessage` / `makeExportHistoryResult` now take the actual write folder.
- `deleteSelected` sets retry + root cascade INFO footers; `clearHistory` sets Flush success INFO footer.
- Flat export uses picker dir; session export uses session subfolder path in sticky.
- FooterPanel path-style truncate also matches `Patch Mutator: Exported `.
- Unit tests updated/added for retry delete, flush, flat/session export paths, export cancel prefix.
- Verified: `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests`; `Matrix-Control_Tests PatchMutatorEngine`; `python3 Scripts/quality/lint_touched.py` (pass). Note: CMake target is `Matrix-Control_Tests` (underscore); no ctest preset in this repo.
- Review fixes: prefix raw export failure messages; Flush success only after clearing non-empty history; Compare locked sticky + exact-clear tests; device-dump / history-limit / retry-too-similar asserts pinned; shared `kExportCompleteFooterStem` for FooterPanel + export formatter.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence / route |
|---------|---------|------------------|
| Export failure forwards raw `PatchFileExportResult::errorMessage` without `Patch Mutator: ` prefix | medium | Confirmed in `makeExportHistoryResult` failure branch — PREFIX_SCOPE broken for passthrough errors. Route: **patch**. |
| `clearHistory` always sets Flush success sticky even when history already empty | medium | Confirmed: `handleClear` can call `clearHistory` when `kClearEnabled` is false; engine always assigns flush success. Route: **patch**. |
| Compare locked sticky + exact-clear untested after prefix | medium | CompareSnapshotTests assert flags/buffers only, not `footerMessage` / `uiMessageText`. Route: **patch**. |
| Device-dump Mutator footers not prefix-pinned in tests | medium | DeferredGateTests use `.contains("Could not read")` / `"cancelled"` only. Route: **patch**. |
| History-limit sticky unobserved in `mutate_limitBlocks` | medium | Test checks `defragModalRequested` only. Route: **patch**. |
| Retry-too-similar assert equals production constant (not prefixed literal) | low | Constant-equality cannot catch PREFIX_SCOPE regression. Route: **patch**. |
| Other stickies (no-change / no-initial-snapshot / export-failed) lack observers | low | No Tests hits for those strings. Route: **patch** (add where paths are cheap; otherwise covered by constant inventory). |
| FooterPanel hardcodes `Patch Mutator: Exported ` instead of shared stem | low | Desync risk if copy changes. Route: **patch** (Shared display constant). |
| Export long-path middle-truncation has no automated paint test | medium (unverified severity for CI) | GUI paint; repo convention is manual Standalone. Route: **defer**. |
| Stale selected retry missing → falls through to root cascade delete | maybe-false / pre-existing | Pre-change delete control flow; not introduced by sticky copy. Route: **defer**. |
| Spec Verification lists wrong CMake/ctest targets | false | Finding’s only fix is editing this build’s spec — rejected per review rules. Notes already document `Matrix-Control_Tests`. |
| Empty Spec Change Log / Triage Log at review start | false | Process sections filled during this review pass. |
| MutatorActionHandlerTests seeds old Export footer shape | false | Mock fixture for handler wiring; does not claim product Export copy. |
| Blind: fragmented `kStickyModulePrefix` SSOT across PluginDisplayNames | low | Same root as FooterPanel hardcode — grouped into Shared-stem **patch**. |
| Blind: PREFIX_SCOPE only partially regression-locked | medium | Grouped with Compare / device-dump / history-limit / retry test **patches**. |
| Edge claim: full-prefix fails on export errors | medium | Same as export passthrough — **patch**. |
| Edge claim: Flush success only after confirm on non-empty | medium | Same as empty `clearHistory` — **patch**. |

## Design Notes

Export folder in the sticky is the folder that received files:
- Frozen basename session: e.g. `/Users/Guillaume/Desktop/NICEPAD @ B0-P00` (not only the parent picker dir).
- No frozen basename (flat export): the picker destination directory itself.

Canonical Export example (badge separate): `Patch Mutator: Exported 12 mutation file(s) to /Users/Guillaume/Desktop/NICEPAD @ B0-P00.`

Shared sticky pattern: `Patch Mutator: <detail>` — detail stays English, module name untranslated.

Engine sticky inventory to prefix (non-exhaustive guide for implementer — verify against code):
- no module scope / no mutation change / retry too similar / history limit / empty history / no selection / no initial snapshot
- root delete cascade + new retry delete + flush success
- defrag complete / export folder not writable / export failed / export complete with path
- PluginDisplayNames: Compare locked, Export cancelled (+ any other Mutator sticky Messages)

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-ControlTests` -- expected: build success
- `ctest --preset macos-debug-arm64 -R 'PatchMutatorEngine' --output-on-failure` -- expected: Mutator engine unit tests pass
- `python3 Scripts/quality/lint_touched.py` -- expected: pass on touched C++ under `Source/` / `Tests/`

**Manual checks (if no CLI):**
- Standalone: C / D root / D retry / F / E show prefixed stickies; E includes folder; long Export path middle-truncates; Cancel on confirm does not show success sticky.
