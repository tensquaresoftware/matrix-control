---
title: 'CI Mutator undo RETRY determinism'
type: 'bugfix'
created: '2026-09-16'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
baseline_commit: '4b9871ea4b55cb7b6547cf8991485a47f46cbfe9'
context:
  - '{project-root}/CONTRIBUTING.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The UndoManager checkpoint RETRY unit test still flakes on CI (Windows after the 12 Sep WILD+FREE harden; Linux before it). Quality-gate is already green; the remaining red is this non-deterministic Mutator undo scenario.

**Approach:** Make the RETRY checkpoint scenario deterministic in the test harness (seed MUTATE + guaranteed undo probe + reliable RETRY), without widening lizard/quality gates or softening assertions to hide a real checkpoint bug. Touch production only if a genuine checkpoint/RETRY defect is proven.

</frozen-after-approval>

## Implementation Notes

- Windows CI run 35028483328 failed only expect #2 in `mutate_retry_clearsUndoStack` (`canUndo` after `writeProbeParam(14)`). Seed MUTATE and RETRY both succeeded — so the residual flake after WILD+FREE was probe collision (post-MUTATE DCO1 Frequency already 14 → no undo transaction), not RETRY diversity.
- Linux run 34665505190 (pre-harden) failed expects 1/3/4 — classic Drift MUTATE no-op cascade; addressed by b2fd720b.
- Fix in `Tests/Unit/UndoManagerCheckpointPolicyTests.cpp` only: `writeProbeParam` bumps when preferred matches current; RETRY test forces Frequency=14 then probes 14 to exercise the collision path; clearer `canUndo` failure message; post-write `jassert`. No production change.
- Verified: `macos-debug-arm64` build of `Matrix-Control_Tests`, category UndoManagerCheckpointPolicy green (30× stress), `lint_touched.py --base HEAD` OK.
- Commit deferred: standing project rule — commit only on Guillaume's explicit ask.

## Review Triage Log

- Blind Hunter: comment about RETRY collision lived under `setMutateRecipe` — **low / patch** — moved into `writeProbeParam`.
- Blind Hunter: no proof write changed the param / out-of-range clamp risk — **low / patch** for in-range write check (`jassert`); **maybe-false** for APVTS clamp-back (call sites use in-range 12/14) — rejected as unverified.
- Blind Hunter: collision branch unexercised — **medium / patch** — RETRY test now forces Frequency=14 then `writeProbeParam(14)`.
- Blind Hunter: opaque `canUndo` expect — **low / patch** — added failure message.
- Blind Hunter: 12/14 look meaningful — **low / false** — preferred values remain soft preferences; collision force documents that.
- Blind Hunter: frozen Intent overstates RETRY reliability vs probe-only fix — **false** for editing frozen block (oneshot forbids); Implementation Notes already precise.
- Blind Hunter: spec still in-progress / no verification close-out — **low / patch** — status `done` + verification notes above.
- Blind Hunter: context missing harden commit checklist — **low / defer** — already cited in Implementation Notes (`b2fd720b`); no CLAUDE/AGENTS edit.

## Spec Change Log

## Design Notes

WILD+FREE fixed MUTATE no-ops. Remaining Windows flake was editorial-undo probe colliding with post-MUTATE DCO1 Frequency — not a checkpoint/RETRY product bug.
