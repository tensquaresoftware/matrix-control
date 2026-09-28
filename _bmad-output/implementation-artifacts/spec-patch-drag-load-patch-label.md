---
title: 'Patch drag LOAD PATCH header label'
type: 'feature'
created: '2026-09-28'
status: 'done'
route: 'oneshot'
review_loop_iteration: 1
context: []
baseline_commit: 'fa529b1220f959b65bc20a33aaea8410e331a0d8'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** While dragging a Patch over the editor, the PATCH NAME module header still reads `PATCH NAME`, unlike Master drag which temporarily shows `LOAD MASTER` — the cue feels inconsistent.

**Approach:** During a valid Patch drag (single file or multi/folder selection), temporarily replace the module header title with `LOAD PATCH` while keeping the Blue decorative line; restore `PATCH NAME` on exit, drop, or invalid payload. Master Orange / `LOAD MASTER` behaviour stays unchanged.

</frozen-after-approval>

## Implementation Notes

- Agent decision: apply `LOAD PATCH` for both `kValidSingle` and `kValidSelection` (any valid patch-load hover); invalid / junk keep `PATCH NAME` + Blue.
- Agent decision: keep Blue explicitly when applying patch chrome (covers Master→patch kind switch mid-hover).
- Added `kLoadPatchName`, `applyPatchDragChrome()`, `patchDragChromeActive_`, and `updateDragChromeForKind()` (extracted to keep `applyDragOverlay` under the GUI complexity gate).
- Files: `PluginDisplayNames.h`, `PatchNameDisplayPanel.{h,cpp}`. No `PluginEditorFileDragDrop` change — kinds already distinguish patch vs master.
- Build macos-debug-arm64 OK; `lint_touched.py` OK after CCN extract.
- Review patches: documented public drag API + why `patchDragChromeActive_` is required (Blue→Blue has no colour signal).

## Review Triage Log

- Sibling Master-drag matrix still says Patch keeps `PATCH NAME` — **defer**: historical done frozen specs not rewritten in oneshot.
- Oneshot missing I/O / AC / UAT sections — **false**: oneshot template deletes those sections by design.
- No automated LOAD PATCH header assert — **defer**: same as Master chrome gap; GUI component tests discouraged.
- Text-only chrome harder to spot in casual UAT — **false** as a code defect: restates the missing-test risk already deferred.
- `patchDragChromeActive_` why undocumented — **medium→patch**: comment added next to the flag.
- Two bools vs single enum state — **low rejected**: refactor churn; mutual exclusion already enforced in apply helpers.
- Merge applyMaster/applyPatch into one parameterized helper — **low rejected**: WET→DRY threshold not met; twin bodies remain clear.
- Spec notes omit full kind-transition list — **low rejected**: Intent + code paths already cover restore/swap; no user-visible gap.
- Public panel comment still overlay-only — **medium→patch**: comment updated to describe temporary header titles.
- Sibling drag UAT guidance outdated — **defer**: same historical-artifact class as Master matrix.

### Review Findings

- [x] [Review][Defer] No automated LOAD PATCH / restore header assert — deferred: same gap as Master chrome; already in `deferred-work.md`; CONVENTIONS discourage GUI-component unit tests; manual smoke remains the check.

#### Rejected

- Spec / notes polish (list deferred-work in Files, soften “keep”, smoke recipe, Intent asymmetry prose, name obsolete sibling UAT rows) — rejected: would edit the oneshot / frozen Intent under review; template and prior triage already settled these.
- Spec `status: done` while gaps remain deferred — rejected: intentional oneshot practice; gaps already recorded in `deferred-work.md`.
- `kInvalid` / `kInvalidPlural` enum comments omit chrome restore — false: class-level drag comment already states Invalid / exit restore `PATCH NAME` + Blue.
- `applyDragOverlay` early-returns on null `patchNameDisplay_` before chrome update — false: both widgets are constructed together as `unique_ptr` and never reset; same gate as pre-existing Master chrome.
- `kLoadPatchName` comment omits “folder” — false: “selection” already covers multi/folder valid patch hover.
- Public panel comment omits “drop” — false: drop clear is the exit path already covered by “Invalid / exit restore”.
- Document two-bool mutual-exclusion invariant — low rejected: prior triage kept twin bools; apply helpers already enforce exclusion; comment churn without everyday harm.
- ContextualHelp still describes rename-only — rejected: out of scope for this oneshot; same gap already existed for `LOAD MASTER`.
- Edge: `clearDragOverlay` leaves LOAD PATCH if `patchNameDisplay_` null — false: null guard is first; chrome flags cannot be set when display is null (`applyDragOverlay` returns first).
- Edge: `restoreNormalPatchNameChrome` leaves flags stuck if `moduleHeader_` null — false: apply helpers never set flags when header is null; header is never reset to null.
- Edge: null-header kind switch leaves opposite chrome flag — false: unreachable; `moduleHeader_` lifetime matches panel construction.
