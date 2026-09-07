---
title: 'Bank Utility COPY progress message — drop clipboard redundancy'
type: 'chore'
created: '2026-09-07'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** During BANK UTILITY COPY, the progress overlay first line says the bank is being read "into clipboard", while the second line already says "Destination : Clipboard" — redundant and slightly clumsy.

**Approach:** Change the first-line COPY progress string to "Reading source bank X :" (drop "into clipboard"). Leave the destination line and all other bank-transfer messages unchanged.

</frozen-after-approval>

## Implementation Notes

- Changed `PluginDisplayNames` `formatCopyProgressMessage` only: drop `" into clipboard"` so the line reads `"Reading source bank " + bank + " :"`.
- Caller `PatchManagerActionHandlerBankCopyPaste.cpp` already uses that helper; no other call sites. Destination line (`kDestinationLabel` + `kClipboardLabel`) untouched.
- User asked to skip code review for this trivial change; oneshot still ran Blind Hunter per workflow — findings were process/spec-shape only, no code patch.

## Review Triage Log

- Empty Implementation Notes / missing Code Map / AC / Verification in oneshot slim spec — `false` (oneshot route deletes those sections by design; notes filled at finalize).
- Frozen Intent does not name `formatCopyProgressMessage` — `false` (oneshot Intent stays product-level; Code Map deleted on oneshot path).
- No exact before→after AC string — `false` (oneshot; Intent already states the target wording).
- No explicit freeze checklist of other formatters — `false` (Approach already says leave other bank-transfer messages unchanged; verified only `formatCopyProgressMessage` differs in the diff).
- Spec does not document destination-line composition — `false` (existing behavior; out of change scope).
- Spec still `in-progress` at review time — `false` (finalize sets `done`).
