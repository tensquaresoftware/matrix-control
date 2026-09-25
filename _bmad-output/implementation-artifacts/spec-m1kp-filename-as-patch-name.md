---
organization: Ten Square Software
project: Matrix-Control
title: M1KP Filename as Patch Name
author: BMad Agent
type: feature
created: 2026-09-25
updated: 2026-09-25
status: done
route: dispatch
review_loop_iteration: 0
baseline_commit: 6ed518230c3c8352ae7954efc58db02c009fcf9b
updated: 2026-09-25
context:
  - {project-root}/_bmad-output/project-context.md
  - {project-root}/_bmad-output/implementation-artifacts/spec-m1kp-patch-import.md
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiate">

## Intent

**Problem:** Legacy `.m1kp` files from the Matrix-1000 Editor often store a useless default in packed name bytes 0–7 (commonly spaced `I N I T`, or ROM-style `BNKx: yy`), while the meaningful name lives only in the filename. After import, Computer Patches D-025 Settings (default: SysEx names) therefore show that unhelpful internal string in PATCH NAME.

**Approach:** When loading a `.m1kp` via any Computer Patches path (OPEN folder, combobox, prev/next, drag-drop), always resolve PATCH NAME from the on-disk filename stem (no `.m1kp`), ignoring the Computer Patches name Settings policy. Leave `.syx` loads under existing D-025 behavior. Bank IMPORT is out of scope.

**Decisions:**
- **Surfaces** — All Computer Patches loads of `.m1kp` (not drag-drop only; not Bank IMPORT).
- **Sanitize** — Existing Matrix-Control File Names sanitizer (uppercase; `A–Z` `0–9` space `-` `_`; drop others; truncate 8; empty → `PATCH`).

## Boundaries & Constraints

**Always:**
- `.m1kp` load on any Computer Patches surface → PATCH NAME from filename stem, independent of Settings (SysEx / File / Ask Once).
- Stem = `File::getFileNameWithoutExtension` (never append the combobox `(m1kp)` badge to PATCH NAME).
- Apply the existing Matrix name sanitizer used by File Names policy.
- Inject the resolved name into `PatchModel` bytes 0–7 (same as File Names reconcile), so APVTS, PATCH NAME UI, and later SAVE stay aligned.
- Reapply Settings changes must not restore SysEx bytes for an origin file that is still `.m1kp`.
- `.syx` load / Ask Once / Settings defaults unchanged.

**Never:**
- Do not change `.m1kp` decode, combobox `Stem (m1kp)` labels, dual-stem listing, or SAVE writers (still `.syx` only).
- Do not invent a display-only PATCH NAME path that leaves packed bytes as INIT while the UI shows something else.
- Do not alter Internal INIT (`* INIT *`) or Mutator naming.
- Do not reopen master `.m1km` import.
- Do not change Bank IMPORT name fallback behavior in this story.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Happy path | Drop/load `CleanBss.m1kp` (internal `BNK7: 57` or `I N I T`) | PATCH NAME = `CLEANBSS`; Settings ignored | N/A |
| Spaces / case | `Rich Pad.m1kp` | PATCH NAME = `RICH PAD` | N/A |
| Long stem | `MyBeautifulPad.m1kp` | Truncated to 8 Matrix chars (`MYBEAUTI`) | N/A |
| Illegal chars | `Pad#1!!.m1kp` | Illegal chars dropped → `PAD1` | Empty → existing `PATCH` fallback |
| Settings Sysex | Policy = Display SysEx Names; load `.m1kp` | Still filename stem | N/A |
| Settings Ask Once | Policy = Ask Once; load `.m1kp` | No picker; force filename | N/A |
| Contrast `.syx` | Same folder, `CleanBss.syx`, any policy | Unchanged D-025 (SysEx default keeps bytes) | N/A |
| Reapply Settings | Origin still `.m1kp`; user flips policy | PATCH NAME stays filename-based | N/A |
| After SAVE | Loaded `.m1kp`, SAVE writes sibling `.syx` | `.syx` embeds the filename-derived Matrix name | Existing overwrite confirm |

</frozen-after-approval>

## Code Map

- `Source/Core/Actions/PatchManagerActionHandlerComputerLoadSave.cpp` — `reconcileLoadedPatchName`, `reapplyComputerPatchDisplayedName`, `stemForFilenameReconcile`, `decodeAndReconcilePatchFile`; primary hook for forcing File Names policy when `hasM1kpExtension`.
- `Source/Core/Services/PatchFileNameReconciler.cpp` — D-025 engine; optional thin helper for "force filename" if clearer than caller-side policy override.
- `Source/Core/Services/PatchFileNameSanitizer.{h,cpp}` — `sanitizeFileStem` / `sanitizeToMatrixName`; `stripPathAndSyxExtension` still `.syx`-only (stems already extension-stripped via JUCE).
- `Source/Core/Services/PatchFileService.{h,cpp}` — `hasM1kpExtension`, `formatOpenListDisplayName` (`Stem (m1kp)` combobox only).
- `Source/Core/Actions/PatchManagerActionHandlerComputerDrop.cpp` — Drop merges into same load path as combobox (`selectAndLoadComputerPatchAtId` / `loadSelectedPatchFileImmediately`).
- `Source/Core/Actions/PatchManagerActionHandlerBankImport.cpp` — Out of scope (do not change).
- Prior frozen decision renegotiated: `_bmad-output/implementation-artifacts/spec-m1kp-patch-import.md` (PATCH NAME no longer follows D-025 for `.m1kp`; filename forced).
- M4L reference (read-only): `…/Matrix-1000 Editor/…/code/m1k.patchController.js` (`selectPatchInFolder` display = stem.slice(0,8), never injects into buffer; `savePatchAs` truncate-only).

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/Actions/PatchManagerActionHandlerComputerLoadSave.cpp` — On `.m1kp`, force filename reconcile (skip Ask Once); mirror in Settings reapply — Computer Patches PATCH NAME
- [x] `Source/Core/Services/PatchFileNameReconciler.*` (optional) — Extract force-filename helper if it keeps the handler thin — clarity
- [x] `Tests/Unit/` (reconciler and/or Computer load tests) — Cover I/O matrix: m1kp ignores Settings; syx unchanged; reapply; sanitize examples — regression

**Acceptance Criteria:**
- Given a valid `.m1kp` whose packed name is `I N I T` (or `BNKx: yy`), when the user loads it via Computer Patches (OPEN, combo, nav, or drop), then PATCH NAME shows the sanitized filename stem and Settings policy is ignored.
- Given the same stem as `.syx`, when loaded under Display SysEx Names, then PATCH NAME still follows packed bytes (no m1kp special-case).
- Given an origin `.m1kp` still selected, when the user changes Computer Patches name Settings, then PATCH NAME does not revert to the packed INIT/BNK string.
- Given a successful `.m1kp` load then SAVE, when the sibling `.syx` is written, then bytes 0–7 match the filename-derived Matrix name.

### Review Findings

- [x] [Review][Decision] Keep or suppress footer "(filename used)" when `.m1kp` force-mismatches packed name? — Resolved: use explicit copy `Loaded … (.m1kp always uses filename)` (option B).
- [x] [Review][Patch] Cover Settings reapply for bank-export-like `.m1kp` stems [`Tests/Unit/PatchManagerActionHandlerM1kpNameTests.cpp`] — applied: `reapplyComputerDisplay_m1kpBankExportLikeStemKeepsFullSanitize` + footer assert `load_m1kp_footerSaysAlwaysUsesFilename`.
- [x] [Review][Defer] Older frozen import spec still says `.m1kp` PATCH NAME follows D-025 [`spec-m1kp-patch-import.md`] — deferred: amend/supersede that frozen decision in a follow-up so the two specs stop contradicting each other.

**Rejected (this code-review pass):**
- false — Spec Code Map / Change Log / duplicate `updated` / Boundaries wording — fix would only edit this spec under review.
- false — Header comment on `reconcileForcedFilename` — describes the injected sanitized result, not a pre-sanitized caller contract.
- false — Drop-load name assert weak — `P-Test.m1kp` packs `BNK2: 02`; assert `P-TEST` already proves force under default Settings.
- false — SAVE assert does not uniquely prove load-time force — SAVE injects target stem by design; load/drop suites cover force (same as Build triage).
- false — Edge null `packed` / Ask Once empty picker on `reseedFromPackedAndReconcile` — unreachable from production callers (stack buffer; Ask Once never passed on reapply).
- low (rejected) — Ask Once reapply untested / direct `reseed` unit tests / `INIT.m1kp` example — same force path already covered; no everyday product gap.
- low (rejected) — Manual Verification omits Settings-reapply AC wording / no sprint-status row — process hygiene, not a code defect for this oneshot.

## Implementation Notes

- Added `PatchFileNameReconciler::reconcileForcedFilename` + `reseedFromPackedAndReconcile`.
- Computer load forces filename when `hasM1kpExtension` (now public); Settings reapply lives in `PatchManagerActionHandlerComputerNameReapply.cpp`.
- `.syx` path unchanged (Settings / Ask Once still apply).
- Tests: reconciler sanitize matrix + `PatchManagerActionHandlerM1kpNameTests` (load / Ask Once / reapply / syx contrast); SAVE asserts embedded name.

## Spec Change Log

## Review Triage Log

- false — Blind: Code Map still cites LoadSave for reapply — reject (fix would only edit this build's spec; code correctly lives in ComputerNameReapply.cpp).
- high→patch (applied) — Blind: `.m1kp` stem went through `stemForFilenameReconcile` (Pxx strip) contrary to frozen `getFileNameWithoutExtension` — verified; fixed to use raw stem on load + reapply; regression test `P10. Nylon.m1kp` → `P10 NYLO`.
- false — Blind: SAVE asserts do not uniquely prove load-time force — SAVE always injects target stem; load force covered by M1kpName + drop tests; After SAVE acceptance still holds.
- low (rejected) — Blind: Ask Once reapply untested — production forces File Names for `.m1kp` via `allowsComputerNameReapply` / `effectiveComputerNamePolicy`; no everyday defect.
- low (rejected) — Blind: dual-stem m1kp+syx untested — same load path as single-file cases already covered.
- low (rejected) — Blind: `reseedFromPackedAndReconcile` lacks direct unit tests — exercised via handler reapply; no product gap.
- low (rejected) — Blind: Design Notes `INIT.m1kp` example untested — sanitize path covered by forced-filename stem examples.
- false — Blind: prev/next/OPEN not named in new suite — same `decodeAndReconcilePatchFile` path as combobox select.
- false — Blind: D-025 footer still shows `(filename used)` on forced mismatch — accurate UX for the forced policy, not a defect.
- false — Blind: empty Spec Change Log / Review Triage Log at handoff — process timing, not a code defect (this log fills that).
- false — Edge: null `packed` to `reseedFromPackedAndReconcile` — production only calls after successful load into a stack buffer; unreachable.
- false — Edge: Ask Once + empty picker leaves seeded name in `reseedFromPackedAndReconcile` — reapply never passes Ask Once (`effectiveComputerNamePolicy` / early return); unreachable.
- (verification-gap) — No verification gaps found.
- code-review (2026-09-25): decision → footer `(.m1kp always uses filename)` applied; patch → reapply bank-export-like stem test + footer unit test applied; defer → older import-spec D-025 conflict remains in deferred-work.md.
## Design Notes

**M4L behavior (v1.0.2):** Computer Patches UI showed the filename stem (max 8) and never wrote it into SysEx/`.m1kp` name bytes. Matrix-Control already injects on File Names policy; forcing that inject for `.m1kp` matches product architecture better than a display-only overlay.

**Examples:**
- `CleanBss.m1kp` + internal `BNK7: 57` → PATCH NAME `CLEANBSS`
- `Rich Pad.m1kp` → `RICH PAD`
- `INIT.m1kp` with packed `I N I T` → `INIT` (stem), not spaced INIT

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-ControlTests` (or project unit-test target) -- expected: new/updated tests green
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++

**Manual checks:**
- Drag a known `.m1kp` with INIT/BNK internal name; confirm PATCH NAME matches stem under Settings = Display SysEx Names.
- Load a sibling `.syx` with a different internal name; confirm D-025 still applies.
