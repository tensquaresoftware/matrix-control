---
title: 'Local pre-push mirror of CI release-script-tests pytest'
type: 'chore'
created: '2026-11-11'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The opt-in pre-push quality gate only runs the C++ `lint_touched` analyser, while CI’s early `release-script-tests` job also runs pytest on `Scripts/release/tests/` and `Scripts/quality/tests/`. Local green can still mean an immediate red CI (~14 s), as with the alert Look gated-colour dig after ownership moved to Button.

**Approach:** Add a shared local runner that executes the same pytest command as CI, wire it into the existing pre-push / `run_quality_gate` path after lint (keep bypasses), and document the ~1 s cost plus missing-dependency guidance. Do not fold full C++ unit tests or CMake builds into the hook.

</frozen-after-approval>

## Implementation Notes

- Added `Scripts/quality/run_script_tests.sh` as SSOT for `python3 -m pytest Scripts/release/tests/ Scripts/quality/tests/ -q`, with clear missing `pytest` / PyYAML errors (no silent skip).
- `run_quality_gate.sh` runs lint then script tests; `--lint-only` keeps CI `quality-gate` job lint-only so pytest stays in the sibling `release-script-tests` job.
- Pre-push runs pytest once after all pushable refs lint OK (not per-ref); delete-only zero SHA lines skip lint and pytest.
- CI `release-script-tests` now calls `run_script_tests.sh`; docs/installer mention both requirement files + ~1 s cost; bypasses unchanged.
- Verified: 56 pytest passed; intentional script failure → gate exit 1 (manual + `--pre-push`); `--lint-only` and `SKIP_QUALITY_GATE=1` OK.
- Commit deferred: Guillaume asked for commit/push only after explicit validation.

## Review Triage Log

- Blind Hunter: unfinished spec / empty Implementation Notes — **false** at ship time (finalized `status: done` + notes).
- Blind Hunter: CONTRIBUTING Releasing section still taught release-only pytest — **medium**, patched (SSOT `run_script_tests.sh`).
- Blind Hunter: no migration note for existing hook users missing pytest deps — **medium**, patched (CONTRIBUTING).
- Blind Hunter: wiring tests substring-only / no subprocess proof — **low**, deferred (manual gate proof done; subprocess harness out of chore footprint).
- Blind Hunter: no assert CI dropped inlined pytest — **medium**, patched in `test_quality_gate_wiring.py`.
- Blind Hunter: `test_build_and_test_workflow.py` unaware of SSOT — **medium**, patched.
- Blind Hunter: delete-only refs still ran pytest — **medium**, patched (`had_pushable_ref`).
- Blind Hunter: no executable-mode wiring assert — **low**, rejected (`chmod +x` in installer/CI; gate invokes via `bash`).
- Blind Hunter: workflow header comment drift — **low**, patched.
- Blind Hunter: block message missing pip hint on pytest fail — **medium**, patched.
