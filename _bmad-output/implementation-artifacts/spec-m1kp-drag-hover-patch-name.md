---
organization: Ten Square Software
project: Matrix-Control
title: M1KP Drag Hover Patch Name
author: BMad Agent
type: bugfix
created: '2026-09-26'
updated: '2026-09-26'
status: done
route: oneshot
review_loop_iteration: 0
baseline_commit: acfd2840327e0a94ccba65bc427969de9c1c02b7
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-m1kp-patch-import.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** While hovering a valid `.m1kp` over the editor, PATCH NAME line 1 still shows the packed SysEx name (often `I N I T`), even though a successful drop already shows the cleaned filename.

**Approach:** For `.m1kp` drag assess only, set the overlay primary to the same sanitized filename stem used after drop; leave `.syx` hover preview and D-025 policy unchanged.

</frozen-after-approval>

## Implementation Notes

- Root cause: `resolveDragPreviewPrimaryName` preferred a usable packed SysEx name for all formats; `.m1kp` drop already forces `sanitizeFileStem(getFileNameWithoutExtension())`.
- Fix: early-return that same sanitized stem when `hasM1kpExtension(file)`; `.syx` hover path unchanged.
- Side effect vs prior BNK-fallback hover: stem is now Matrix-sanitized (`P-Test` → `P-TEST`), matching post-drop (was raw stem before).
- Tests: literal expectations `P-TEST` / `NICE PAD` / `P10 NYLO`; INIT+BNK fixtures plus usable packed name `NYLON 12` under distinct stem (post-review harden).
- Verified: `PatchFileServiceAssess` category green; `lint_touched.py` OK.

## Review Triage Log

- Blind: oneshot missing AC / hors-scope sections — verdict: `false` — oneshot route deletes those sections by template; Intent already pins `.syx` unchanged.
- Blind: `status: in-progress` while notes claim green — verdict: `false` — workflow state during implement; finalized to `done` after review.
- Blind: BNK hover stem now `P-TEST` undocumented — verdict: `low` — patched Implementation Notes; behaviour matches drop.
- Blind: hover/drop m1kp naming duplicated without shared helper — verdict: `false` — both already call `sanitizeFileStem(getFileNameWithoutExtension())`; extract helper is cosmetic YAGNI.
- Blind: `packedData` unused on m1kp early-return — verdict: `low` — rejected; parameter required for `.syx` path.
- Blind: tests assert via `sanitizeFileStem(...)` not literals — verdict: `medium` — patched to `P-TEST` / `NICE PAD` / `P10 NYLO`.
- Blind: hand-rolled m1kp int16 encode in test — verdict: `low` — deferred encode helper beside `PatchM1kpCodec`.
- Blind: missing bank-export-like stem hover case — verdict: `medium` — patched `assessSinglePatch_m1kpBankExportLikeStemMatchesDropSanitize`.
- Blind: new test missing `rejectKind == kNone` / weak `!= INIT` — verdict: `low` — patched `rejectKind` + literal equality.
- Blind: dropped BNK fixture comment — verdict: `low` — restored comment noting drop-parity sanitize.

### Review Findings

- [x] [Review][Patch] Strengthen m1kp hover test: pack a usable non-INIT SysEx name (e.g. `NYLON 12`) under a different stem — current INIT-only case still passes if a regression only special-cases the INIT sentinel [Tests/Unit/PatchFileServiceAssessTests.cpp:assessSinglePatch_m1kpUsableInternalNameUsesSanitizedStem] — applied 2026-09-26

#### Rejected (code review 2026-09-26)

- Blind: bank-export test asserts literal `P10 NYLO` instead of calling `sanitizeFileStem` — `false` — literals were an intentional prior fix; expected sanitize result is stable.
- Blind: hand-rolled m1kp int16 encode in new test — `false` (as new open item) — already deferred in the same diff via `deferred-work.md` / prior triage.
- Blind: hover sanitizes directly while drop passes raw stem into reconciler — `false` — `reconcileForcedFilename` sanitizes via `kDisplayFileNames`; result parity holds; shared helper was explicitly out of scope.
- Blind: no hover case for stem that sanitizes to empty-name fallback (`PATCH`) — `low` rejected — unlikely everyday; not worth new fixture complexity for this oneshot.
- Blind: `resolveDragPreviewPrimaryName` API docs not updated — `low` rejected — cpp comment documents the contract; header churn cosmetic.
- Blind: hand-built writer never asserts codec round-trip before assess — `false` — `isValidSinglePatch` already requires successful decode of the written bytes.
- Edge Case Hunter / Verification Gap / Acceptance Auditor — no surviving findings.
