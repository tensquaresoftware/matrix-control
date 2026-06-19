# Correct Course Handoff — Bank Utility UNLOCK (MIDI-only) + drop cross-bank navigation

**Date:** 2026-06-19  
**Author:** Guillaume (with investigation agent)  
**Prepared for:** Fresh-context `bmad-correct-course` run  
**Status:** Ready to execute  
**Triggers:** Matrix-1000 hardware vs SysEx asymmetry; investigation `next-patch-after-unlock-missing-set-bank`; Figma layout constraints (4 px grid)  
**Supersedes (partial):** `2026-06-18-Correct-Course-Bank-Utility-UNLOCK-Handoff.md` §2 D-022-R2 navigation coupling, §3.3 cross-bank wrap, §3.4 lock dot UX; D-023 cross-bank clause in `.decision-log.md`  
**Does NOT supersede:** Story 7-3b ID renames (`bankUtilityUnlockBank`, `kUnlockBank` action); UNLOCK label; bank 0–9 Set Bank paths; `miscBankLockEnable` separation (D-022-R3)

---

## 1. Executive summary

Keep the **current Figma Bank Utility layout** (module name **BANK UTILITY**, label **SELECT BANK**, bank buttons **0–9** unchanged size/position, **UNLOCK** button retained).

**Product pivot:**

| Topic | Old intent (7-3b / D-023) | New decision |
|-------|---------------------------|--------------|
| UNLOCK button | Send `0CH` + set `patchManagerBanksLocked = false` + enable cross-bank Prev/Next | Send **`0CH` only** — remote helper for synth front panel; **no plugin state** |
| Prev/Next navigation | Wrap within bank when locked; wrap across banks when unlocked | **Always wrap within current bank** (patch 99 → 0, same bank) |
| Cross-bank access | Prev/Next while unlocked | **Bank buttons 0–9 only** (+ patch Prev/Next / NumberBox within bank) |
| Lock mirror in plugin | `patchManagerBanksLocked` drives navigation + UI dot | **Remove** from navigation and UX (optional: remove APVTS property entirely) |

**Why:** Oberheim `Set Bank` (`0AH`) always re-locks; front-panel Bank Lock unlock allows 3-digit entry **without** re-lock on bank digit — **not reproducible** via remote SysEx. Cross-bank Prev/Next after UNLOCK caused sync bugs and false expectations. UNLOCK as a **dumb MIDI shortcut** preserves design, documents opcode `0CH`, and avoids layout churn.

**Investigation reference:** `_bmad-output/implementation-artifacts/investigations/next-patch-after-unlock-missing-set-bank-investigation.md` (Concluded 2026-06-19).

---

## 2. Decision record (for Sprint Change Proposal)

### D-023-R2 — Plugin navigation is always intra-bank

- **Decision:** `DeviceMemoryLimits::advancePatch` (and all Prev/Next Internal Patches handlers) **always** wrap patch index within the current bank (99 → 0, 0 → 99). **Never** increment/decrement bank via Prev/Next.
- **Rationale:** Matches realistic plugin workflow (explicit bank select + patch browse); avoids Set Bank / Unlock asymmetry; eliminates `banksLocked` navigation state machine.
- **Reverts:** D-023 bullet « if no bank locked, patch 99 → patch 00 next bank »; Story 7-3b manual smoke step 3 (cross-bank after UNLOCK); tests `navigation_wrapsAcrossBanks`, `unlock_thenNextAtPatch99_sendsSetBank1`.

### D-022-R4 — UNLOCK is MIDI-only (no plugin lock mirror)

- **Decision:** Bank Utility **UNLOCK** enqueues **`Unlock Bank` SysEx (`0CH`) only**. Handler **must not** write `patchManagerBanksLocked`, **must not** call `clearSyncedBankState()`, **must not** change bank/patch APVTS coordinates.
- **Rationale:** Minor remote affordance for users at the computer; synth front panel remains authoritative for 3-digit entry. Plugin does not model hardware lock state (cannot read it back).
- **Amends:** D-022-R2 (derived lock state + UNLOCK clears mirror); investigation fix #4 (`clearSyncedBankState` after UNLOCK).
- **UX note (footer, optional v1):** On UNLOCK click, info footer e.g. *« Synth bank lock released — use front panel for 3-digit entry; plugin bank/patch unchanged. »*

### D-022-R5 — GUI Bank Utility layout frozen

- **Decision:** Keep module title **BANK UTILITY**, **SELECT BANK** label, current button widths/positions (4 px grid). **Do not** widen 0–9 buttons; **do not** rename module to Bank Selector; **do not** remove UNLOCK for layout reasons.
- **Rationale:** Figma experiments showed no satisfactory all-4-divisible width redistribution.

### D-023a-R2 — Drop lock dot; keep active-bank highlight (Story 7.5)

- **Decision:** **Remove** red dot on Internal Patches bank NumberBox tied to `patchManagerBanksLocked` (revert `refreshBankLockIndicator` behaviour). Story **7.5** scope: **selected bank red text** on Bank Utility 0–9 + `patchManagerSelectedBank` sync after navigation (deferred item from 7-3b review).
- **Rationale:** Without plugin lock mirror, dot has no reliable meaning.

### D-022-R3 — unchanged

Master Edit **`miscBankLockEnable`** (byte 165) remains separate; no sync with Bank Utility UNLOCK.

---

## 3. Target behaviour (acceptance-level)

### 3.1 Bank Utility UNLOCK

1. Label **UNLOCK** (unchanged).
2. Click → `PatchSelectionMidiSync::sendUnlockBank` / `MidiManager::sendUnlockBank` → **`F0 10 06 0C F7` only**.
3. **No** APVTS property writes (`patchManagerBanksLocked`, bank, patch).
4. **No** `clearSyncedBankState()`.
5. Grayed when `!DeviceMemoryLimits::hasBankConcept()` (Matrix-6/6R).

### 3.2 Bank select 0–9

Unchanged from 7-3b:

1. Update `patchManagerSelectedBank` + `internalPatchesCurrentBankNumber`.
2. `syncSelection(..., forceSetBank=true)` → `0AH` + Program Change.
3. **Do not** set `patchManagerBanksLocked` (property removed or unused).

### 3.3 Internal Patches Prev/Next + patch NumberBox

1. Prev/Next changes **patch only** within current bank (wrap at 0/99).
2. Bank change **only** via Bank Utility 0–9 (or future explicit API — not NumberBox bank edit; bank NumberBox remains display-only if present).
3. `syncSelection` sends Set Bank only when bank changes vs `lastSyncedBank_` (cold start, bank button, stale sync) — **not** on ordinary same-bank Prev/Next.
4. **No** navigation path crosses banks.

### 3.4 UNLOCK then Prev/Next (documented limitation)

After UNLOCK, user may use **synth front panel** for 3-digit entry. If user continues **plugin** Prev/Next without re-selecting bank, hardware may diverge (Matrix-1000 may reset bank context on `0CH`). **Accept** — not a plugin bug; document in footer/tooltip. **Do not** add Set Bank + silent Unlock hacks.

### 3.5 Matrix-6 / Matrix-6R

Unchanged: bank utility grayed; no `0AH`/`0CH`.

---

## 4. Code changes (implementation checklist)

### 4.1 Remove or simplify lock state

| Item | Action |
|------|--------|
| `patchManagerBanksLocked` / `kBanksLocked` | **Remove** property + all readers/writers (grep whole repo), **or** stop writing and delete in follow-up — prefer **remove** pre-release |
| `markBanksLockedInApvts()` | **Remove** method and all call sites |
| `PluginProcessor::handlePatchNumberChange` lock write | **Remove** |
| `MidiManager` init default `kBanksLocked` | **Remove** |
| `InternalPatchesPanel::refreshBankLockIndicator` | **Remove**; bank NumberBox `setShowDot(false)` always (or remove dot for bank box only) |

### 4.2 UNLOCK handler

**Target** (`PatchManagerActionHandler::handleUnlockBank`):

```cpp
void PatchManagerActionHandler::handleUnlockBank(const DeviceMemoryLimits& limits)
{
    if (!limits.hasBankConcept())
        return;

    if (patchSelectionMidiSync_ != nullptr)
        patchSelectionMidiSync_->sendUnlockBank(limits);
    // No APVTS writes
}
```

**Target** (`PatchSelectionMidiSync::sendUnlockBank`):

```cpp
void PatchSelectionMidiSync::sendUnlockBank(const DeviceMemoryLimits& limits)
{
    if (midiManager_ == nullptr || !limits.hasBankConcept())
        return;

    midiManager_->sendUnlockBank();
    // Do NOT clearSyncedBankState()
}
```

### 4.3 Navigation

| File | Change |
|------|--------|
| `DeviceMemoryLimits.cpp` | `advancePatch`: remove `bankLocked` parameter; always wrap within bank when patch overflows |
| `DeviceMemoryLimits.h` | Update signature |
| `PatchManagerActionHandler.cpp` | Prev/Next: call `advancePatch(current, direction)` without lock flag |
| `applyPatchCoordinates` | Keep `syncSelection`; remove `markBanksLockedInApvts` when `setBankSent` |

### 4.4 Tests (`PatchManagerActionHandlerTests.cpp`)

| Test | Action |
|------|--------|
| `unlockBank_sends0CHOnly` | **Update:** assert **no** `kBanksLocked` flip (remove banksLocked assertions or delete property) |
| `navigation_wrapsAcrossBanks` | **Remove** |
| `unlock_thenNextAtPatch99_sendsSetBank1` | **Remove** |
| `unlock_onBank1_nextSameBank_resyncsSetBank` | **Remove** |
| `locked_at99_fourNext_staysBank0` | **Keep** (rename optional → `at99_fourNext_staysBank0`) |
| `navigation_locked_withinBank_noSetBank` | **Keep** (drop « locked » from name if lock state gone) |
| **Add** | `unlock_doesNotChangeApvtsCoordinates` — bank/patch unchanged after UNLOCK |
| **Add** | `at99_next_wrapsToPatch0_sameBank` — explicit wrap acceptance |

### 4.5 GUI

- **No layout changes** to `BankUtilityPanel` (keep UNLOCK slot).
- Optional: footer message on UNLOCK (Story 7.5 or small AC in implementation story).

### 4.6 Docs / artifacts (Correct Course output)

| Artifact | Update |
|----------|--------|
| `.decision-log.md` | Add D-023-R2, D-022-R4, D-023a-R2; strike/amend D-023 cross-bank bullet |
| `prd.md` FR-20, FR-21 | UNLOCK = remote `0CH`; navigation intra-bank only |
| `epics.md` Story 7.5 AC | Remove lock dot; keep active bank red text |
| `7-3b` story file | Add errata note pointing to this Correct Course |
| `sprint-status.yaml` | New story **7-3c** or **7-5** reprioritized — agent to propose |
| Investigation case file | Mark follow-up « fixed by D-023-R2 » |

---

## 5. Explicit non-goals

- Renaming module to Bank Selector; widening bank buttons; removing SELECT BANK label
- Removing UNLOCK button
- Set Bank + silent Unlock pairing after navigation
- Cross-bank circular navigation through 1000 patches
- Bidirectional sync with `miscBankLockEnable`
- Reading hardware lock state from synth

---

## 6. Current codebase delta (pre–Correct Course)

Agent should verify `git status` — likely touched on branch:

- `PatchSelectionMidiSync.cpp` — `clearSyncedBankState()` after UNLOCK (**revert**)
- `InternalPatchesPanel.cpp` — `refreshBankLockIndicator()` (**revert/remove**)
- `PatchManagerActionHandlerTests.cpp` — cross-bank / unlock sync tests (**remove/update**)
- `sprint-status.yaml` — 7-5 `ready-for-dev` (reconcile after proposal)

Stories **7-3b** marked **done** — this course is a **course correction**, not a revert of ID renames.

---

## 7. Suggested story split (for agent proposal)

| Story | Scope |
|-------|--------|
| **7-3c** (recommended ID) | Core: dumb UNLOCK, remove `banksLocked` navigation, simplify `advancePatch`, tests |
| **7.5** (amended) | GUI: active bank red text, `selectedBank` sync, optional UNLOCK footer; remove lock dot |

Alternatively single **7-3c** with thin GUI footer if scope stays small.

---

## 8. Manual UAT (post-implementation)

1. Fresh instance → Next → `0AH` + PC; stay bank 0; patch wraps 99→0 with repeated Next.
2. Bank **3** → `0AH 03` + PC; patch unchanged unless navigation moves patch.
3. **UNLOCK** → `0CH` only; plugin bank/patch digits **unchanged**; optional footer.
4. Next → PC only (same bank); synth may differ if user relies on front panel — acceptable.
5. Matrix-6: Bank Utility grayed; no SysEx.

---

## 9. Commands for fresh-context agent

```
/bmad-correct-course
```

**Input to paste:**

> Use handoff: `Documentation/Development/Plans/2026/06/2026-06-19-Correct-Course-Bank-Utility-Unlock-Simplify-Handoff.md`  
> Produce Sprint Change Proposal, update PRD / epics / `.decision-log.md`, recommend story **7-3c** (or merged scope).  
> Investigation context: `_bmad-output/implementation-artifacts/investigations/next-patch-after-unlock-missing-set-bank-investigation.md`

After Correct Course approval:

```
/bmad-create-story 7-3c
```

then

```
/bmad-dev-story 7-3c
```

---

## 10. Grep anchors (verification after implementation)

```bash
# Should return ZERO (navigation must not read banksLocked)
rg 'kBanksLocked|patchManagerBanksLocked|markBanksLockedInApvts|banksLocked' Source/ Tests/

# UNLOCK paths remain
rg 'bankUtilityUnlockBank|sendUnlockBank|handleUnlockBank' Source/

# advancePatch should not take bankLocked
rg 'bankLocked|banksLocked' Source/Core/Services/DeviceMemoryLimits

# Removed cross-bank tests
rg 'wrapsAcrossBanks|unlock_thenNext|unlock_onBank1_nextSameBank' Tests/
```

---

## 11. Reference

- Prior handoff: `Documentation/Development/Plans/2026/06/2026-06-18-Correct-Course-Bank-Utility-UNLOCK-Handoff.md`
- Investigation: `_bmad-output/implementation-artifacts/investigations/next-patch-after-unlock-missing-set-bank-investigation.md`
- Spike (hardware asymmetry): `Documentation/Development/Plans/2026/06/2026-06-18-Matrix-1000-Bank-Lock-MIDI-Spike.md`
- Implemented baseline: Story **7-3b** (`_bmad-output/implementation-artifacts/7-3b-bank-utility-unlock-semantics-and-id-rename.md`)
