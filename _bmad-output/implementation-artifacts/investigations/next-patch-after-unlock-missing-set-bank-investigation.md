# Investigation: Next Patch after UNLOCK — missing Set Bank (0x0A)

## Hand-off Brief

1. **What happened.** After UNLOCK, Next Patch emits `C0 03` (Program Change only) without `F0 10 06 0A 01 F7` (Set Bank 1); synth stays on bank 0 / patch 03 instead of bank 1 / patch 03. **Confirmed** from user MIDI trace; root cause **not yet confirmed in runtime** — code path analysis points to either `patchManagerBanksLocked` still `true` or a `lastSyncedBank_` / hardware desync gap.
2. **Where the case stands.** Handler-level logic and existing unit tests pass; **no test covers UNLOCK → Next at bank boundary**. Most plausible repro: locked wrap at patch 99→0 on bank 0 (4× Next → patch 03) while user expected cross-bank wrap after UNLOCK.
3. **What's needed next.** Confirm repro coordinates + `patchManagerBanksLocked` after UNLOCK in APVTS; add failing integration test `unlock_thenNextAtPatch99_sendsSetBank1`; fix if `banksLocked` not cleared or if cross-bank nav must `forceSetBank` after UNLOCK.

## Case Info

| Field            | Value                                                                 |
| ---------------- | --------------------------------------------------------------------- |
| Ticket           | N/A (ad-hoc UAT / regression)                                        |
| Date opened      | 2026-06-19                                                            |
| Status           | Concluded (product pivot D-023-R2, 2026-06-19)                       |
| System           | Matrix-1000, macOS, Matrix-Control plugin (Story 7-3b bank UNLOCK)   |
| Evidence sources | User MIDI trace; static code trace; unit test run (all green)        |

## Problem Statement

User report (verbatim intent): **Next Patch after UNLOCK** — MIDI trace shows **`C0 03` only**, no **`0A 01`** (Set Bank 1). Observed result: **bank 0 / patch 03** on synth; expected **bank 1 / patch 03**.

## Evidence Inventory

| Source                                      | Status    | Notes                                                                 |
| ------------------------------------------- | --------- | --------------------------------------------------------------------- |
| User MIDI trace (`C0 03`, no `0A 01`)       | Available | Stronghold — symptom confirmed                                        |
| `PatchManagerActionHandler.cpp`             | Available | Navigation + unlock handlers traced                                   |
| `PatchSelectionMidiSync.cpp`                | Available | Set Bank gating via `lastSyncedBank_`                                 |
| `DeviceMemoryLimits.cpp`                    | Available | `advancePatch(..., banksLocked)` wrap rules                           |
| `PatchManagerActionHandlerTests.cpp`        | Available | `navigation_wrapsAcrossBanks` passes; **no unlock+next test**         |
| Runtime APVTS dump after UNLOCK               | Missing   | Would confirm/refute `patchManagerBanksLocked` hypothesis             |
| Exact bank/patch before UNLOCK                | Missing   | Distinguishes boundary wrap vs within-bank step                       |

## Investigation Backlog

| # | Path to Explore                                              | Priority | Status | Notes                                      |
| - | ------------------------------------------------------------ | -------- | ------ | ------------------------------------------ |
| 1 | Add test: unlock @ 0/99 locked → next → Set Bank 1 + PC 0   | High     | Open   | Closes gap vs manual smoke step 3          |
| 2 | APVTS property dump after UNLOCK in standalone               | High     | Open   | Verify `patchManagerBanksLocked == false`  |
| 3 | PluginProcessor integration (suppress hook + action order) | Medium   | Done   | Suppress wired; order looks correct        |
| 4 | `sendUnlockBank` should clear `lastSyncedBank_`?             | Low      | Open   | Defensive resync after UNLOCK              |

## Timeline of Events

| Time       | Event                                              | Source        | Confidence |
| ---------- | -------------------------------------------------- | ------------- | ---------- |
| 2026-06-19 | Story 7-3b manual smoke A/B/C pass; D4/D5 noted    | 7-3b story    | Confirmed  |
| 2026-06-19 | Startup policy: reset 0/00 + `clearSyncedBankState`| spec-internal | Confirmed  |
| 2026-06-19 | User reports Next-after-UNLOCK missing Set Bank     | User report   | Confirmed  |

## Confirmed Findings

### Finding 1: Set Bank is skipped when bank unchanged vs `lastSyncedBank_`

**Evidence:** `Source/Core/MIDI/PatchSelectionMidiSync.cpp:41-48`

**Detail:** `syncSelection` sends Set Bank (0x0A) only when `forceSetBank || !hasLastSyncedBank_ || bank != lastSyncedBank_`. Same-bank navigation sends Program Change only.

### Finding 2: Cross-bank navigation requires `banksLocked == false`

**Evidence:** `Source/Core/Services/DeviceMemoryLimits.cpp:107-124`, `Source/Core/Actions/PatchManagerActionHandler.cpp:130-134`

**Detail:** When `bankLocked == true`, patch 99 + Next wraps to patch 0 **within the same bank**. When `false`, patch 99 + Next advances to `bank+1`, patch `minPatchNumber`.

### Finding 3: UNLOCK does not clear `lastSyncedBank_`

**Evidence:** `Source/Core/MIDI/PatchSelectionMidiSync.cpp:69-75`, `PatchManagerActionHandler.cpp:222-234`

**Detail:** `sendUnlockBank` emits 0x0C only; `lastSyncedBank_` unchanged. Cross-bank next still sends Set Bank if target bank ≠ `lastSyncedBank_`.

### Finding 4: Existing unit tests pass; unlock+next boundary untested

**Evidence:** Test run 2026-06-19 — `navigation_wrapsAcrossBanks`, `unlockBank_sends0CHOnly` green; no combined scenario.

## Deduced Conclusions

### Deduction 1: `C0 03` without Set Bank matches locked within-bank navigation

**Based on:** Findings 1, 2

**Reasoning:** From bank 0 patch 99 with `banksLocked == true`, Next → patch 0 (PC `C0 00`), then +3 Next → patch 3 (`C0 03`), all on bank 0, no Set Bank. User expecting bank 1 patch 3 implies they believed UNLOCK enabled cross-bank wrap, but navigation still used locked math.

**Conclusion:** **Most likely** `patchManagerBanksLocked` remained `true` when Next ran, despite UNLOCK click (or UNLOCK not in repro path).

### Deduction 2: UI bank 1 / synth bank 0 desync is a secondary pattern

**Based on:** Finding 1

**Reasoning:** If APVTS shows bank 1 but `lastSyncedBank_ == 1` while hardware never received Set Bank 1, same-bank Next sends PC only → synth stays on bank 0. Requires prior desync (MIDI not delivered, or bank APVTS changed without sync).

**Conclusion:** Possible if user reports plugin UI at 1/03; **needs UI vs synth confirmation**.

## Hypothesized Paths

### Hypothesis 1: `patchManagerBanksLocked` still true after UNLOCK

**Status:** Open

**Theory:** UNLOCK handler not executed, grayed button, or property not read before Next.

**Supporting indicators:** Symptom matches locked wrap 99→0→…→3 on bank 0; only PC in trace.

**Would confirm:** APVTS `patchManagerBanksLocked == true` after UNLOCK click; or breakpoint in `handleUnlockBank` not hit.

**Would refute:** Property `false` after UNLOCK with same symptom from patch 99 unlocked.

**Resolution:** —

### Hypothesis 2: Cross-bank at 0/99 unlocked but Set Bank skipped (`lastSyncedBank_` stale)

**Status:** Open (lower priority)

**Theory:** `lastSyncedBank_ == 1` while APVTS at 0/99; next to 1/0 skips Set Bank.

**Supporting indicators:** Would show APVTS bank 1 / UI 1/03 with synth 0/03.

**Would confirm:** Debug log of `lastSyncedBank_` vs coordinates at sync time.

**Would refute:** `lastSyncedBank_ == 0` at boundary crossing.

**Resolution:** —

### Hypothesis 3: Handler-only tests miss PluginProcessor regression

**Status:** Refuted (for duplicate-MIDI path)

**Theory:** `applyPatchCoordinates` double-fires sync without suppress.

**Supporting indicators:** Deferred review note 7-3b line 359.

**Would confirm:** Duplicate PC in trace.

**Would refute:** Suppress hook wired at `PluginProcessor.cpp:240`; property updates occur under suppress in `applyPatchCoordinates`.

**Resolution:** Refuted — suppress hook present; nested listener returns early.

## Missing Evidence

| Gap                         | Impact                                      | How to Obtain                          |
| --------------------------- | ------------------------------------------- | -------------------------------------- |
| Bank/patch before UNLOCK    | Distinguishes H1 vs H2                      | User repro notes or APVTS log         |
| `patchManagerBanksLocked` after UNLOCK | Confirms H1                          | APVTS logger / breakpoint              |
| Plugin UI bank at symptom   | Confirms UI vs synth desync (H2)            | Screenshot or NumberBox value          |

## Source Code Trace

| Element       | Detail                                                                 |
| ------------- | ---------------------------------------------------------------------- |
| Error origin  | `PatchSelectionMidiSync::syncSelection` — Set Bank gate               |
| Trigger       | `internalPatchesLoadNext` → `PatchManagerActionHandler::handleAction` |
| Condition     | Same bank in APVTS + `lastSyncedBank_` match → PC only                |
| Related files | `PatchManagerActionHandler.cpp`, `DeviceMemoryLimits.cpp`, `PluginProcessor.cpp` |

## Conclusion

**Confidence:** Medium on mechanism (locked wrap or desync); Low on which hypothesis until runtime APVTS confirmed.

The code **does** send Set Bank on cross-bank navigation when `banksLocked == false` and `bank != lastSyncedBank_` (covered by `navigation_wrapsAcrossBanks`). The observed **`C0 03` only** trace is **fully explained** by **within-bank** navigation on bank 0 — most plausibly because **`advancePatch` still saw `banksLocked == true`** after an UNLOCK the user believed had succeeded. A **secondary** path is APVTS/`lastSyncedBank_` ahead of hardware without Set Bank on same-bank steps.

## Recommended Next Steps

### Fix direction

1. **If H1 confirmed:** Trace why `handleUnlockBank` does not persist `patchManagerBanksLocked = false` (action dispatch, grayed button, session property rename).
2. **If H2 confirmed:** After UNLOCK, call `clearSyncedBankState()` or `forceSetBank=true` on first navigation that changes bank while unlocked.
3. **Regardless:** Add unit test `unlock_thenNextAtPatch99_sendsSetBank1` mirroring manual smoke step 3.

### Diagnostic

1. Repro: bank 0 patch 99 → UNLOCK (verify 0x0C in trace) → single Next (expect 0x0A 01 + C0 00).
2. Log/read `patchManagerBanksLocked` immediately after UNLOCK.
3. Compare Internal Patches NumberBox bank vs synth three-digit display.

## Reproduction Plan

**Setup:** Matrix-1000, MIDI monitor, fresh plugin instance (starts 0/00 per startup policy).

1. Navigate to bank **0**, patch **99** (Set Bank 0 on first nav from cold start).
2. Click bank button **3** (optional — locks, sends 0x0A 03) then **UNLOCK** — expect **0x0C only**.
3. Click **Next Patch** once.
   - **Expected (unlocked):** `F0 10 06 0A 01 F7` + `C0 00`; APVTS **1 / 00**.
   - **Bug (locked):** `C0 00` only; APVTS **0 / 00**.
4. Repeat from step 2; click Next **four times** from 99 — if locked, ends at **0 / 03** with **`C0 03` only** (matches user trace).

## Side Findings

- `HandlerHarness` does not wire `setSuppressPatchSelectionMidiSync` (`PatchManagerActionHandlerTests.cpp:255-259`) — acceptable for handler-only tests but limits Processor integration coverage.
- Manual smoke step 3 (`7-3b` story line 262) has **no automated regression test** for UNLOCK + cross-bank navigation.

## Follow-up: 2026-06-19

### New Evidence

User ran targeted repro steps 1–3 (not variant 4). MIDI trace (Matrix-1000, MT4 Port 1):

| Time | Message | Notes |
| ---- | ------- | ----- |
| 20:59:25.589 | `F0 10 06 0A 00 F7` | Cold sync Set Bank 0 |
| 20:59:25.593 | `C0 01` | Patch 1 |
| 20:59:28.499 | `C0 63` | Patch 99 (0x63) — at boundary |
| 20:59:38.148 | `F0 10 06 0C F7` | UNLOCK only ✓ |
| 20:59:47.870 | `C0 00` + `F0 10 06 0A 01 F7` | Single Next after UNLOCK — **both PC 0 and Set Bank 1 sent** |

Monitor lists `C0 00` before `0A 01` at identical timestamp; code enqueues Set Bank then PC (`PatchSelectionMidiSync.cpp:47-55`) — likely display ordering, not proven send-order inversion.

### Additional Findings

- **Targeted repro passes:** cross-bank navigation after UNLOCK from 0/99 emits Set Bank 1 + Program Change 0 as specified.
- **Original symptom not reproduced** in this path (`C0 03` only / bank 0 vs 1 patch 03).

### Updated Hypotheses

| Hypothesis | Status | Resolution |
| ---------- | ------ | ---------- |
| H1 — `banksLocked` still true after UNLOCK | **Refuted** (this repro) | UNLOCK + single Next from 0/99 crossed to bank 1 with 0x0A 01 |
| H2 — APVTS/`lastSyncedBank_` desync | **Open** (low) | Not observed here |
| H4 — Original report = locked multi-step wrap (variant 4) | **Open → likely** | 99→0→1→2→3 on bank 0 yields `C0 03` only; matches initial trace |

### Updated Conclusion

**Confidence:** High that plugin cross-bank-after-UNLOCK works when repro is 0/99 → UNLOCK → **single** Next. **Medium** that the original incident was a **different navigation path** (multiple Next while still locked, or not at patch 99 before UNLOCK).

**Status:** Concluded for primary repro; original symptom requires variant 4 confirmation or exact prior coordinates.

### Backlog Changes

| # | Item | Status |
| - | ---- | ------ |
| 1 | Add test `unlock_thenNextAtPatch99_sendsSetBank1` | Still recommended (now documents passing behaviour) |
| 2 | APVTS dump after UNLOCK | Cancelled — repro sufficient |
| 4 | Clear `lastSyncedBank_` on UNLOCK | Deprioritized — not needed for 0/99 case |

## Follow-up: 2026-06-19 #2

### New Evidence

User ran **variant 4** (fresh app restart, **no UNLOCK**): navigate to 0/99, then **4× Next**.

| Time | Message | Notes |
| ---- | ------- | ----- |
| 21:02:11.432 | `F0 10 06 0A 00 F7` | Cold start first sync |
| 21:02:11.437 | `C0 63` | Patch 99 |
| 21:02:20.968 | `C0 00` | Next #1 — wrap 99→0 within bank (locked) |
| 21:02:21.723 | `C0 01` | Next #2 |
| 21:02:22.439 | `C0 02` | Next #3 |
| 21:02:23.107 | `C0 03` | Next #4 — **matches original symptom** |

No `0A 01` at any step. Synth/plugin end state: **bank 0 / patch 03**.

### Additional Findings

- Variant 4 trace is **identical in shape** to the original report (`C0 03` only, no Set Bank 1, bank 0 not bank 1).
- Behaviour is **by design** per D-023: `banksLocked == true` (default after Set Bank / bank select) → `advancePatch` wraps within bank at patch 99.

### Updated Hypotheses

| Hypothesis | Status | Resolution |
| ---------- | ------ | ---------- |
| H4 — Original report = locked multi-step wrap (variant 4) | **Confirmed** | User trace 21:02:xx reproduces original symptom exactly |

### Final Conclusion

**Confidence:** **High**

There is **no plugin defect** on the UNLOCK → Next path (confirmed in follow-up #1). The original incident is **explained** as **locked within-bank navigation** (variant 4): four Next clicks from 0/99 without UNLOCK produce bank 0/patch 03 with Program Change only — the user expected bank 1/patch 03 which requires either **UNLOCK first** (repro #1) or explicit bank-1 select.

**Status:** **Concluded** — not a bug; UX/expectation gap unless UNLOCK state was believed active when it was not.

### Recommended Next Steps (product, not code fix)

- Optional UX (Story 7.5 scope): red dot on bank NumberBox when `patchManagerBanksLocked` to signal locked wrap.
- Optional regression tests: `unlock_thenNextAtPatch99_sendsSetBank1` + `locked_at99_fourNext_staysBank0`.
- No Core fix required for reported behaviour.

### Follow-up actions (2026-06-19 #3)

- Added unit tests `locked_at99_fourNext_staysBank0` and `unlock_thenNextAtPatch99_sendsSetBank1` in `PatchManagerActionHandlerTests.cpp`.
- Wired Internal Patches bank NumberBox dot to `patchManagerBanksLocked` (`InternalPatchesPanel::refreshBankLockIndicator`) — first slice of Story 7.5.
- Sprint: `7-5-bank-utility-ui-wiring` promoted to `ready-for-dev`.

## Follow-up: 2026-06-19 #4 — UNLOCK same-bank desync (Confirmed bug)

### New Evidence

User hardware repro: bank 1 patch 6, UNLOCK (`0C`), Next → plugin **1/07**, Matrix-1000 **0/07** unlocked. MIDI trace: **`C0 07` only** after `0C` (no `0A 01`).

### Root Cause (Confirmed)

`sendUnlockBank` left `lastSyncedBank_ == 1`. Next navigation (same bank) skipped Set Bank. Matrix-1000 **resets or invalidates bank context on 0x0C** while plugin APVTS still shows bank 1.

### Fix

`PatchSelectionMidiSync::sendUnlockBank` now calls `clearSyncedBankState()` after `0x0C`. Next `syncSelection` re-sends Set Bank for APVTS bank.

Test added: `unlock_onBank1_nextSameBank_resyncsSetBank`.

## Follow-up: 2026-06-19 #5 — Correct Course product pivot (D-023-R2)

### Resolution

Sprint Change Proposal `sprint-change-proposal-2026-06-19-bank-unlock-simplify.md` — **product pivot**, not a regression fix:

| Topic | Prior (7-3b / investigation #4) | New (D-023-R2, D-022-R4, D-023a-R3) |
|-------|-----------------------------------|----------------------------------------|
| UNLOCK | `0CH` + lock mirror + `clearSyncedBankState` hack | **`0CH` only** — indicator off; no bank/patch change; no sync cache clear |
| Prev/Next | Cross-bank when unlocked | **Always intra-bank** wrap |
| Lock dot UX (#3) | Dot tied to navigation lock state | **Retained** — display-only: on after bank select, off after UNLOCK (D-023a-R3) |

### Case status

**Closed — resolved by product decision.** Original symptom (variant 4) was by-design locked wrap. Follow-up #4 desync: addressed by not resyncing after UNLOCK; dot off signals unlocked mode even if bank digit may diverge from hardware.

### Implementation story

**7-3c** — Core (navigation + indicator writes). **7.5** — dot rendering. Reverts `clearSyncedBankState` on UNLOCK only; keeps `refreshBankLockIndicator`.

