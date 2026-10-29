---
title: 'Silence false ERROR on Matrix-6 Master dump'
type: 'bugfix'
created: '2026-10-29'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** When a Matrix-6/6R returns a legitimate Master dump (~478 bytes) during presence / fallback, the generic receive-log path still validates it as a Matrix-1000 Master (351 bytes) and logs a false ERROR even though Matrix-6 detection succeeds.

**Approach:** Before calling the Matrix-1000-length `validateSysEx` from receive-log analysis, recognize a Matrix-6/6R Master dump via the existing filter and emit a normal analysis line instead of an ERROR. Leave parser decode / M-1000 Master length rules unchanged.

</frozen-after-approval>

## Implementation Notes

- Root cause: `MidiReceiver` / fallback `logSysExReceived` → `analyzeSysExMessage` → `validateSysEx` (expects Master 351) → `logError` on legitimate Matrix-6 478/479.
- Fix: short-circuit receive-log analysis via `Core::matrix6MasterDumpReceiveLogAnalysis` before Matrix-1000 `validateSysEx`. Parser length rules unchanged.
- Label wording: structural dump only (no “Valid” claim); checksum echoed, not verified.
- Tests: filter receive-log helper (478/479 labeled, 351 empty); parser type check (351 still valid, 478/479 length-fail).
- Review patches: clearer helper comment; softened label; renamed test; added 479 parser assert.

## Spec Change Log

## Review Triage Log

- Blind Hunter: no MidiLogger path test asserting ERROR absent — verdict: `medium` — defer (needs log-capture infra); helper locks short-circuit label.
- Blind Hunter: “Valid” overclaims vs structural-only filter — verdict: `medium` — patched (dropped “Valid”).
- Blind Hunter: receive-log copy lives in discovery filter header — verdict: `low` — rejected (thin oneshot; helper stays next to filter for unit access).
- Blind Hunter: test name claimed skip of validateSysEx without asserting it — verdict: `low` — patched (renamed to labels-only).
- Blind Hunter: SysExParserTests miss validateSysEx→logError mechanism; omit 479 — verdict: `false` / `low` — false for mechanism (logger short-circuit is the fix); patched 479 length-fail assert.
- Blind Hunter: double logSysExReceived on fallback success — verdict: `medium` — defer (pre-existing; out of ERROR-silence scope).
- Blind Hunter: oneshot spec still in-progress / empty triage — verdict: `low` — patched (status done + this log).
- Blind Hunter: helper comment easy to misread — verdict: `low` — patched (empty vs non-empty caller contract).

### Review Findings

Combined code review Builds 1+2 (2026-10-29) — silence-ERROR-scoped items.

- [x] [Review][Defer] MidiLogger analyze/logSysExReceived path never asserts ERROR absent for 478/479 [MidiLoggerFormatting.cpp:250-254] — deferred: already noted; needs log-capture infra
- [x] [Review][Defer] Double logSysExReceived on Matrix-6 Master fallback success — deferred: pre-existing; out of ERROR-silence scope

#### Rejected

- (none new for Build 1 beyond prior triage; combined rejected items live on the Master-only presence spec Review Findings).
