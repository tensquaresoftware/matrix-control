---
title: 'Settings Mutation History Defrag'
type: 'feature'
created: '2026-09-19'
status: 'done'
route: 'dispatch'
baseline_commit: '2502371b49733e09104fedd3b9a7cbc7abbb786c'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Settings still shows a "Coming soon" placeholder for Patch Mutator history defrag, even though the engine can already compact and renumber session history. Users need a deliberate Settings action (with confirmation) and a clear success footer that states how much mutation/retry capacity remains.

**Approach:** Replace the placeholder with label **MUTATION HISTORY** and a single **DEFRAG** button (disabled when history is empty). On click, show a Matrix-Control styled confirm overlay (same family as Master Init confirm). On confirm, call the existing `defragHistory()` path. Success footer reports used/available counts for mutations and retries in the agreed English copy. Update contextual help. Keep limit-block Defrag on the same engine method (and prefer the same custom confirm UI for visual consistency).

## Boundaries & Constraints

**Always:**
- Reuse `PatchMutatorEngine::defragHistory()` / `HistoryDefragService` for compaction (no second algorithm).
- Settings wiring mirrors other Settings actions: Editor `onClick` → thin `PluginProcessor` wrapper → engine; propagate footer via existing APVTS `uiMessageText` / `uiMessageSeverity`.
- ASCII-only display strings in `PluginDisplayNames` (no em dash / fancy Unicode).
- Defrag button enabled only when mutation history is non-empty; refresh enable when Settings opens and after history-changing outcomes reachable from Settings flow.
- Success footer exact shape (numbers dynamic):
  `PATCH MUTATOR: Mutation history renumbered. Mutations: {u} used / {a} available. Retries: {ru} used / {ra} left under them.`
  where `{a} = 100 - mutationsUsed`, `{ra} = sum over roots of (100 - retryCount(root))`.

**Never:**
- Add a second Settings button (CLEAR/EXPORT/etc.) or a Defrag control on the main Patch Mutator panel.
- Continuous/automatic background defrag on create/delete.
- Change gap allocation policy, Compare force-exit semantics, or audition behavior beyond what `defragHistory()` already does.
- French in source/UI strings.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Empty history | Settings open, no M/R entries | DEFRAG disabled; no confirm | N/A |
| Confirm Defrag | Non-empty history, user confirms | History renumbered; selection remapped; rich success footer; button stays enabled if still non-empty | N/A |
| Cancel Defrag | Non-empty history, user cancels | No store change; no success footer | N/A |
| Limit-path Defrag | MUTATE/RETRY blocked → confirm Defrag | Same engine + same rich footer as Settings | Cancel leaves history unchanged |
| Already contiguous | Gaps already closed | Defrag still succeeds; footer shows used/available (available may be unchanged vs pre-click index headroom) | N/A |

**Decisions:**
- Label `MUTATION HISTORY`; single button `DEFRAG`; confirm overlay required.
- Footer variant: two sentences after the renumbered lead-in, `Mutations:` / `Retries:` with `used / available` and `used / left under them` (no semicolon separator; no "slots").
- Custom Matrix-Control confirm dialog (Master Init overlay pattern), not system Alert for the Settings path; migrate the existing limit Defrag gate to that same dialog so both entry points match.
- Wire via PluginEditor Settings callbacks + processor wrapper (not a new APVTS action property / MutatorActionHandler Settings ID).

</frozen-after-approval>

## Code Map

- `Source/GUI/Settings/SettingsPanelSetup.cpp` / `SettingsPanel.cpp` / `SettingsPanel.h` -- Replace `defragHistoryPlaceholder_` with DEFRAG button; layout via `layoutButtonRow`; bind help; expose getter; enable/disable API.
- `Source/GUI/PluginEditorSettings.cpp` -- Wire DEFRAG `onClick` → open confirm → processor defrag; refresh enabled state.
- `Source/GUI/PluginEditor.h` / `PluginEditorWindows.cpp` -- Host show/hide/layout for new confirm dialog (mirror Master Init).
- `Source/GUI/Dialogs/MasterInitConfirmDialog.*` -- Pattern to copy (overlay, skin, scale, Cancel/Confirm, Esc/click-outside).
- `Source/GUI/PluginEditorPatchBindings.cpp` -- Replace limit-gate `showOrderedConfirmAlert` with the same custom dialog presenter.
- `Source/Core/PluginProcessor.h` (+ gates/construction as needed) -- Public `defragMutationHistory()` (name flexible) that calls engine and writes footer properties.
- `Source/Core/Services/PatchMutator/PatchMutatorEngineHistory.cpp` / `PatchMutatorEngineInternal.h` -- Build rich footer after successful defrag; keep empty-history warning.
- `Source/Core/Services/PatchMutator/MutationHistoryStore.h` -- Read `rootCount` / `retryCount` / sorted roots for retry totals (no compaction change).
- `Source/Shared/Definitions/PluginDisplayNames.h` -- Rename label; button text; help string; dialog title/body/buttons; footer format helper.
- `Source/Core/Actions/MutatorActionHandler.*` -- Limit confirm still calls `defragHistory()`; only modal presentation changes at editor gate.
- `Tests/Unit/PatchMutatorEngineDefragTests.cpp` -- Assert new footer format (and counts) on success path.
- `CMakeLists.txt` -- Register new dialog `.cpp`.
- Do not change: `HistoryDefragService` renumber algorithm; Patch Mutator panel chrome; automatic defrag policies.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Shared/Definitions/PluginDisplayNames.h` -- Update Settings label/button/help; confirm dialog copy; `formatDefragCompleteFooterMessage(mutationsUsed, mutationsAvailable, retriesUsed, retriesAvailable)` -- SSOT for UI strings.
- [x] `Source/Core/Services/PatchMutator/PatchMutatorEngineHistory.cpp` (+ internal helpers) -- After successful defrag, set footer from live store counts using the format helper -- rich footer for all callers.
- [x] `Source/Core/PluginProcessor.*` -- Add thin public defrag entry that invokes engine and propagates footer/severity -- Settings can call Core without reaching into the engine.
- [x] `Source/GUI/Dialogs/MutatorHistoryDefragConfirmDialog.*` (new) -- Master-Init-style overlay with Defrag/Cancel -- Settings + limit gate share one look.
- [x] `Source/GUI/Settings/SettingsPanel.*` -- MUTATION HISTORY + DEFRAG button; remove Coming soon placeholder; contextual help; enable when history non-empty -- finish 7.7 Phase B row.
- [x] `Source/GUI/PluginEditorSettings.cpp` / `PluginEditor*.*` / `PluginEditorPatchBindings.cpp` -- Wire Settings click + host dialog; point limit Defrag gate at same dialog; refresh enable -- end-to-end UX.
- [x] `Tests/Unit/PatchMutatorEngineDefragTests.cpp` (+ CMake if new files need tests only) -- Expect formatted footer with computed used/available -- lock copy and math.
- [x] `CMakeLists.txt` -- Add dialog source to plugin target -- build registers new GUI file.

**Acceptance Criteria:**
- Given empty mutation history, when Settings opens, then DEFRAG is disabled and no confirm can run.
- Given non-empty history, when the user clicks DEFRAG and confirms, then indices compact, selection remaps as today, and the footer matches the agreed Mutations/Retries used/available text.
- Given non-empty history, when the user cancels the confirm, then history and footer stay unchanged (no success renumber message).
- Given MUTATE/RETRY hits the history limit, when the user confirms Defrag in the overlay, then the same engine path runs and the same rich footer appears.
- Given hover/focus on the MUTATION HISTORY row controls, when contextual help resolves, then the footer help no longer says Coming soon and describes renumber/free capacity intent in English ASCII.

## Implementation Notes

- Shared confirm title set to `Defrag mutation history?` (path-neutral for Settings and limit gate; previous `Mutation history full` was wrong for voluntary Settings defrag).
- Settings DEFRAG enable refreshed on Settings open via `hasMutationHistory()`; Settings closes while confirm is shown (Master Init courtesy).
- Limit gate and Settings both call `openMutatorHistoryDefragConfirmDialog` → same engine `defragHistory()` and rich footer.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence |
|---|---|---|
| Settings/About can open above Defrag confirm (no dismiss) | medium | `openSettingsWindow` / `openAboutWindow` do not hide `mutatorHistoryDefragConfirmDialog_`; stacking is reachable from the limit-gate path. |
| Defrag confirm `onConfirm_` not cleared before invoke; Return could re-fire | medium | `confirm()` calls `onConfirm_()` then `dismiss()` without `std::move`/clear; dialog may still be focused until hide completes. |
| `defrag_successFooter` expects via same formatter as production | medium | Verification-gap pre-verified: helper regression would still pass; needs literal expected string. |
| No handler test that limit Defrag confirm publishes APVTS footer | medium | Verification-gap pre-verified: `defragCallCount` unused; limit confirm path untested for `uiMessageText`. |
| Async limit gate overwrites pending callback | false | Both callbacks only call `defragHistory()`; overwrite does not change outcome. |
| Zero editor width → zero dialog scale | low | Same Master Init pattern; everyday editor width is non-zero; rejected (unlikely + not a small unique fix). |
| `kDesignHeight = 100` may clip body | maybe-false | Short one-sentence body; would need visual check at high UI scale to settle; treat as low-unverified and reject. |
| Symbol `kDefragHistoryLabel` vs display `MUTATION HISTORY` | low | Naming hygiene only; everyday users unaffected; rename is broader than a direct patch. Rejected. |
| Sticky-footer inventory still lists old short Defrag string | defer | Pre-existing doc inventory drift; not required for this feature to ship. |
| `spec-system-init-syx-filenames.md` still mentions Defrag Coming soon | defer | Stale sibling artifact note; out of this change's runtime behavior. |
| Spec Change Log / Implementation Notes formatting / empty triage while in-review | false | Process artifact state, not a product defect; rejected (would only edit the build spec). |
| No unit test for `PluginProcessor::hasMutationHistory` / `defragMutationHistory` | low | Thin wrappers; engine footer and (after patch) handler publish cover the meaningful paths. Rejected. |

## Design Notes

Footer example after compacting three roots and eight retries total:

```
PATCH MUTATOR: Mutation history renumbered. Mutations: 3 used / 97 available. Retries: 8 used / 292 left under them.
```

(`292 = 3*100 - 8`.)

Confirm dialog: reuse Master Init overlay structure (dimmed host, bordered card, title bar, primary Defrag + Cancel). Prefer closing Settings while the confirm is visible (same courtesy as Master global init). Body may reuse/adapt `Dialogs::MutatorHistoryDefrag` copy (ASCII only).

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` -- expected: build succeeds
- `ctest --test-dir Builds/macOS/Debug --output-on-failure -R 'PatchMutatorEngineDefrag|HistoryDefrag'` (adjust binary/dir to preset layout if needed) -- expected: defrag unit tests green
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++ under Source/Tests

**Manual checks:**
- Settings → MUTATION HISTORY → DEFRAG → confirm → footer counts look right; Cancel leaves names/indices unchanged.
- Fill history to M99 (or gap-exhaust) → MUTATE → custom Defrag overlay → confirm → can MUTATE again.
