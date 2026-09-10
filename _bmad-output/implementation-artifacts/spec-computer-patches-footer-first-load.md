---
title: 'Computer Patches footer first-load scan + Loaded'
type: 'feature'
created: '2026-09-10'
status: 'done'
route: 'oneshot'
baseline_commit: '93d194d07ab1cd10b36e6e2deb797f7ab33c33f1'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-computer-patches-multi-drop-virtual-list.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After OPEN or a successful Computer Patches drop, the footer briefly shows the scan tally alone, then replaces it with `Loaded <path>`. The first successful load should keep both: what was scanned and which file is current. Later Prev/Next (and other in-list loads) should stay `Loaded <path>` only.

**Approach:** Add a combined English footer for the first successful load after a scan/merge that leads to a load. Arm that combine at scan time without publishing the scan-only line when a load is expected, so the UI never flashes the old tally alone. Consume the arm once in the load-success footer path; leave scan-only and cancel-restore behaviour as today when nothing is loaded.

</frozen-after-approval>

## Implementation Notes

- Agent decisions: (1) When name reconciliation applies on that first load, keep the reconciliation Loaded clause and still prefix with `Patch files:` (`formatFirstLoadAfterScanMessage`). (2) Formulas: with invalids `Patch files: N valid / M invalid — Loaded <path>`; with zero invalids omit `/ 0 invalid`. (3) Same wording for OPEN and all drop shapes. (4) Suppress `propagateScanResult` footer write when arming a pending combine; still update scan cache / revision. (5) Clear pending arm on cancel restore, reject/empty paths, abort, session reset, load failure, and abandon of pending Computer select settle so a later nav load cannot accidentally combine.
- Helpers: `FooterMessages::formatScanLoadPrefix`, `formatFirstLoadAfterScan`, `formatFirstLoadAfterScanMessage` in `PluginDisplayNames.h`.
- Arming: OPEN / drop folder / drop single / drop virtual set `pendingCombinedScanLoadFooter_` when a load will follow; empty/unusable scans still publish scan-only footers.
- `publishLoadFooters` consumes the arm once; Prev/Next and later selects use plain Loaded / reconciliation.
- FooterPanel path-style middle truncation also applies when the message contains ` — Loaded ` (combined first-load lines).
- Tests updated: DropLoad first-load+nav, Browser OPEN first-load + empty-folder scan footer.

## Review Triage Log

- FooterPanel did not path-truncate combined `Patch files:… — Loaded …` lines — **high/patch**: extended truncate detection to `contains(" — Loaded ")`.
- Pending arm not cleared on session reset / abort revert-only / empty OPEN-drop — **medium/patch**: clear on those paths.
- Dual grammar scan-only vs combined prefix — **false**: intentional; scan-only stays `formatScanSummary`, first-load uses the approved `Patch files:` formula.
- `formatFirstLoadAfterScan` unused beside Message helper — **low/rejected**: thin convenience wrapper kept for exact Loaded-only combine.
- Soft test assertions / missing OPEN+nav parity / shared scan-without-footer helper / cancel-arm follow-up assert — **defer**: coverage gaps not blocking.
- Spec still `in-progress` at review time — **patch**: set `done` on finalize.

### Review Findings

- [x] [Review][Decision] Update multi-drop frozen footer wording to match combined first-load — Resolved: option 1; renegotiated `spec-computer-patches-multi-drop-virtual-list.md` frozen Always/matrix/AC + Implementation Notes pointer.
- [x] [Review][Patch] Clear pending combined footer when abandoning debounced Computer select settle [Source/Core/Actions/PatchManagerActionHandlerInternalPatches.cpp:207]
- [x] [Review][Patch] Assert exact `formatFirstLoadAfterScanMessage` on folder-drop first-load (invalids path) [Tests/Unit/PatchManagerActionHandlerDropLoadTests.cpp:307]
- [x] [Review][Patch] Assert combined first-load footer on virtual multi-drop happy path [Tests/Unit/PatchManagerActionHandlerDropLoadExtrasTests.cpp:29]
- [x] [Review][Patch] Assert no scan-only footer between OPEN prepare and load dispatch [Tests/Unit/PatchManagerActionHandlerBrowserTests.cpp:71]
- [x] [Review][Defer] Cancel-then-nav no accidental combine assert — deferred: already logged in deferred-work; product clears exist on restore/abort; coverage follow-up only
- [x] [Review][Defer] OPEN + Prev/Next plain Loaded parity assert — deferred: already logged in deferred-work; DropLoad covers nav; OPEN coverage gap only
- [x] [Review][Defer] Shared scan-without-footer helper for OPEN/drop prepare — deferred: already logged in deferred-work; three near-duplicate sites, not a user bug
- [x] [Review][Defer] Automated FooterPanel truncate for combined ` — Loaded ` lines — deferred: paint/GUI unit-test habit; smoke/manual walkthrough covers

#### Rejected

- `formatFirstLoadAfterScan` unused — low/rejected: intentional thin wrapper (prior build triage); not worth deleting in this review.
- Dual grammar unexplained in Implementation Notes — rejected: fix would edit the spec under review; intentional dual wording already recorded.
- Missing lifetime comment on `pendingCombinedScanLoadFooter_` — low/rejected: unlikely everyday harm; clear sites already listed in Implementation Notes.
- Three-way recon `||` in OPEN/single-drop exact asserts — false: intentional tolerance for reconciliation variant, not a soft-count gap.
- Hard-coded ` — Loaded ` separator in FooterMessages vs FooterPanel — low/rejected: only bites on a future wording edit; shared constant can wait with other polish.
