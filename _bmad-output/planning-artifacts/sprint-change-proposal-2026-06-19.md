# Sprint Change Proposal — Bank Utility UNLOCK Revert

**Date:** 2026-06-19  
**Author:** BMad Correct Course workflow  
**Prepared for:** Guillaume  
**Status:** Approved  
**Approved by:** Guillaume — 2026-06-19  
**Trigger document:** `Documentation/Development/Plans/2026/06/2026-06-18-Correct-Course-Bank-Utility-UNLOCK-Handoff.md`  
**Supersedes:** D-022 (BANK LOCK toggle); partial U-IDs harmonization (`bankUtilityLockBank` / `kLockBank`)

---

## Section 1: Issue Summary

### Problem statement

Story 7.3 implemented Bank Utility as a **BANK LOCK toggle** (flip `patchManagerBankLock`, send `0AH` on lock-on and `0CH` on lock-off). Post-implementation review and Oberheim Matrix-1000 MIDI spec analysis show this model is **incorrect**:

- Opcode `0AH` (Set Bank) **always enables** hardware bank lock as a side-effect.
- Opcode `0CH` (Unlock Bank) **only** disables lock; it does not select a bank.
- There is no symmetric "lock off via `0AH`" path — the toggle pairing creates unnecessary complexity and diverges from legacy Matrix-1000 Editor (Max for Live) UX.

### Discovery context

- **When:** 2026-06-18, during Story 7.3 code review and MIDI spec cross-check.
- **Evidence:**
  - Oberheim spec: `_local/Documents/References/MD/oberheim-matrix-1000-midi-sysex-implementation.md` — §0AH, §0CH, master byte 165.
  - Implemented toggle in `PatchManagerActionHandler.cpp` lines 112–127 (`kLockBank` flip + conditional `0AH`/`0CH`).
  - U-IDs story renamed `unlockBank` → `bankUtilityLockBank` with display **BANK LOCK** — now reverted.
  - Spike `2026-06-18-Matrix-1000-Bank-Lock-MIDI-Spike.md` archived as **not required** for v1.

### Target mental model

| User action | MIDI | Hardware |
|-------------|------|----------|
| Click bank **0–9** | `SetBank(n)` → `0AH` + PC | Bank changes; lock **enabled** |
| Click **UNLOCK** | `UnlockBank()` → `0CH` | Lock **disabled**; bank unchanged |
| Navigate `<` `>` while unlocked | `0AH` only when bank changes | Per D-023 wrap rules |

---

## Section 2: Impact Analysis

### Epic impact

| Epic | Impact |
|------|--------|
| **Epic 7** (Patch Manager wiring) | **Direct** — Stories 7.3, 7.5, 7.6 affected. New story **7-3b** required. Story 7.3 remains in `review` with errata; do not mark `done` until 7-3b lands. |
| **Epic 8** (device limits) | **Minor** — Story 8.5 docs reference `patchManagerBankLock`; update property name only. Runtime logic (`DeviceMemoryLimits::advancePatch(..., banksLocked)`) unchanged. |
| **Epic U** (UI scale / U-IDs) | **Doc-only** — U-IDs inventory and harmonization stories document `bankUtilityLockBank`; annotate as superseded by this change. No re-open of completed U stories. |

Epic 7 can still be completed as planned; scope adjustment is **corrective**, not a pivot.

### Story impact

| Story | Change |
|-------|--------|
| **7.3** | AC #3 (BANK LOCK toggle) **invalid**. Add errata note; status stays `review` until 7-3b merges. INIT/PASTE/STORE/navigation ACs remain valid. |
| **7-3b** (new) | Bank Utility UNLOCK semantics + full ID/state rename + tests. **Blocks** marking 7.3 done. |
| **7.5** | Amend AC: UNLOCK label (not BANK LOCK); red lock indicator on Internal Patches bank NumberBox when `patchManagerBanksLocked`; active bank red text on Bank Utility buttons. **Remove** padlock-on-bank-button requirement (supersedes D-023a). |
| **7.6** | No logic change; navigation wrap already uses lock state bool. Verify after rename. |
| **7.4, 7.8** | No impact. |

### Artifact conflicts

| Artifact | Conflict | Action |
|----------|----------|--------|
| **PRD** §4.6, FR-20 | BANK LOCK toggle + padlock on button | Replace per Section 4 |
| **`.decision-log.md`** D-022, D-023a | BANK LOCK toggle; padlock on bank button | Supersede D-022; amend D-023a |
| **`epics.md`** FR-20 one-liner, Story 7.5 AC | Toggle + padlock | Update per Section 4 |
| **`7-3-*.md`** story file | AC #3 describes toggle | Add errata block |
| **U-IDs inventory / harmonization** | `bankUtilityLockBank` naming | Annotate superseded; grep cleanup in 7-3b |
| **Architecture** | No bank-lock-specific sections | No change |
| **UX / Figma** | May show BANK LOCK or padlock on button | Reconcile in Story 7.5; red dot on bank NumberBox |
| **Spike** `2026-06-18-Matrix-1000-Bank-Lock-MIDI-Spike.md` | Universal Bank Select SysEx | Mark **not required** for v1 |

### Technical impact

- **Core:** Replace toggle handler with `handleUnlockBank()`; centralize `patchManagerBanksLocked = true` on every `0AH` path.
- **Shared:** APVTS action `bankUtilityUnlockBank`, state `patchManagerBanksLocked`, display **UNLOCK**.
- **GUI:** `unlockBankButton_`, dimension keys renamed.
- **Tests:** Toggle expectations removed; assert lock state after bank select / unlock.
- **Breaking change:** APVTS property `patchManagerBankLock` → `patchManagerBanksLocked`. Pre-release; no migration shim unless requested.
- **MVP:** Unchanged — feature set identical, semantics corrected.

---

## Section 3: Recommended Approach

### Selected path: **Option 1 — Direct Adjustment** (with corrective story)

| Criterion | Assessment |
|-----------|------------|
| Effort | **Medium** — ~15–20 files; rename grep sweep; handler logic is small |
| Risk | **Low** — aligns with hardware spec; simpler state machine |
| Timeline | +1 story (7-3b) before 7.5; ~0.5–1 dev day |
| Rollback | **Not viable** — partial 7.3 work (INIT/PASTE/STORE) is correct; only bank-lock block reverts |

### Rationale

- No MVP scope reduction needed.
- No architectural redesign — same `PatchSelectionMidiSync`, same APVTS state tree.
- Reverting only the toggle block is cheaper than maintaining incorrect toggle semantics through 7.5 UI work.

### Deferred (out of scope)

- Universal Bank Select SysEx spike
- Bidirectional sync `miscBankLockEnable` ↔ `patchManagerBanksLocked`
- Master request at startup for byte 165

---

## Section 4: Detailed Change Proposals

### 4.1 Decision log — add D-022-R1/R2/R3; supersede D-022

**File:** `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md`

**ADD after D-022:**

```markdown
#### D-022-R1 — Bank Utility control is UNLOCK only

- **Decision:** Bank Utility shows **UNLOCK** (not BANK LOCK). Single action: send `0CH`, clear editor lock mirror. No toggle, no `0AH` on UNLOCK.
- **Rationale:** Oberheim asymmetry (`0AH` locks as side-effect; only `0CH` unlocks).
- **Supersedes:** D-022 follow-up « BANK LOCK label »; Story 7.3 AC #3 toggle semantics.

#### D-022-R2 — Lock state is derived, not toggled by button

- **Decision:** `patchManagerBanksLocked` reflects whether banks are currently locked. Set `true` when `0AH` sent; `false` only when `0CH` sent (UNLOCK) or explicit hardware resync (future).
- **Rationale:** State mirrors hardware; avoids toggle confusion.

#### D-022-R3 — `miscBankLockEnable` remains distinct

- **Decision:** Master Edit → Misc → BANK LOCK (byte 165) is **not** the Bank Utility button. No bidirectional sync in this change set.
- **Rationale:** Different domain (master parameter vs patch-manager utility).
```

**ANNOTATE D-022:**

```markdown
- **Status:** SUPERSEDED by D-022-R1/R2/R3 (2026-06-19 Correct Course)
```

**AMEND D-023a:**

**OLD:**
> Locked bank: **red padlock icon** bottom-right of the button — replaces the old bank NumberBox dot.

**NEW:**
> When banks are locked (`patchManagerBanksLocked`): **red dot** on Internal Patches bank NumberBox (not on Bank Utility UNLOCK button). Selected bank: red button text on Bank Utility 0–9.

**Rationale:** Lock is a global hardware state after `0AH`, not per-button toggle; indicator moves to Internal Patches per Oberheim UX.

---

### 4.2 PRD — §4.6 and FR-20

**File:** `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md`

**§4.6 Description — OLD:**
> Banks 0–9 selection and BANK LOCK aligned with Matrix-1000 front-panel semantics (replaces legacy UNLOCK).

**NEW:**
> Banks 0–9 selection and **UNLOCK** aligned with Oberheim MIDI spec and legacy Matrix-1000 Editor UX.

**FR-20 — OLD:**
> **BANK LOCK** toggles lock on the selected bank; locked bank shows red padlock icon on the button. Lock semantics match native synth behavior (not legacy M4L UNLOCK inversion).

**NEW:**
> Bank Utility provides bank buttons 0–9 (Set Bank / `0AH`, enables hardware bank lock) and an **UNLOCK** button (Unlock Bank / `0CH`). Editor state `patchManagerBanksLocked` mirrors hardware lock for navigation wrap and UI indicators. No lock toggle on Bank Utility. Internal Patches bank NumberBox shows a red lock indicator when banks are locked. Master Edit **BANK LOCK** (`miscBankLockEnable`) is a separate master parameter (byte 165).

---

### 4.3 Epics — FR-20 one-liner and Story 7.5

**File:** `_bmad-output/planning-artifacts/epics.md`

**FR summary (line ~44) — OLD:**
> FR-20: Bank lock — BANK LOCK toggles lock on selected bank; red padlock on locked bank button.

**NEW:**
> FR-20: Bank lock — UNLOCK sends `0CH`; bank select sends `0AH` (locks); `patchManagerBanksLocked` mirrors hardware; red dot on Internal Patches bank NumberBox when locked.

**Story 7.5 — OLD:**
> I want bank buttons 0–9 and BANK LOCK wired to Core state,
> **When** user selects bank or toggles lock
> **Then** selected bank shows red text; locked bank shows red padlock; no duplicate bank NumberBox in Internal Patches

**NEW:**
> I want bank buttons 0–9 and UNLOCK wired to Core state,
> **When** user selects bank or clicks UNLOCK
> **Then** selected bank shows red text; Internal Patches bank NumberBox shows red dot when `patchManagerBanksLocked`; UNLOCK button has no padlock icon; no duplicate bank-select control in Internal Patches

**ADD Story 7-3b** (after Story 7.3):

```markdown
### Story 7-3b: Bank Utility UNLOCK Semantics and ID Rename

As a sound designer,
I want the UNLOCK button to send Unlock Bank SysEx only,
So that bank lock behaviour matches Oberheim spec (FR-20 corrected).

**Acceptance Criteria:**

**Given** Story 7.3 Core handler scaffold
**When** user clicks UNLOCK
**Then** handler sends only `0CH`, sets `patchManagerBanksLocked` false, never sends `0AH`
**When** user selects bank 0–9 or navigation sends Set Bank
**Then** `patchManagerBanksLocked` set true
**And** all IDs renamed: `bankUtilityUnlockBank`, `patchManagerBanksLocked`, display UNLOCK
**And** unit tests updated; grep anchors clean
```

---

### 4.4 Story 7.3 — errata

**File:** `_bmad-output/implementation-artifacts/7-3-patchmanageractionhandler-bank-and-internal.md`

**ADD at top of Acceptance Criteria:**

```markdown
> **ERRATA (2026-06-19 — Correct Course D-022-R1):** AC #3 (BANK LOCK toggle) is **superseded**. Implementation in `handleUnlockBank` + derived lock state is tracked in Story **7-3b**. Do not mark 7.3 `done` until 7-3b merges.
```

**REPLACE AC #3** with strikethrough reference or:

```markdown
3. **And** **UNLOCK** action (FR-20 corrected — see Story 7-3b):
   - Action `bankUtilityUnlockBank` sends Unlock Bank **0x0C** only; sets `patchManagerBanksLocked` false.
   - Bank select / navigation Set Bank sets `patchManagerBanksLocked` true.
   - No toggle semantics.
```

---

### 4.5 Code — core handler (reference)

**Remove** toggle block in `PatchManagerActionHandler.cpp` (lines 112–127).

**Replace with:**

```cpp
void handleUnlockBank(const DeviceMemoryLimits& limits)
{
    if (!limits.hasBankConcept()) return;
    if (patchSelectionMidiSync_ != nullptr)
        patchSelectionMidiSync_->sendUnlockBank(limits);
    apvts_.state.setProperty(BankUtilityModule::StateProperties::kBanksLocked, false, nullptr);
}
```

**Add** `markBanksLockedInApvts()` call from `PatchSelectionMidiSync::syncSelection` / `sendSetBank` paths.

---

### 4.6 ID rename inventory (mandatory)

| Current | Target |
|---------|--------|
| `bankUtilityLockBank` | `bankUtilityUnlockBank` |
| `kLockBank` (action) | `kUnlockBank` |
| `"BANK LOCK"` (Bank Utility display) | `"UNLOCK"` |
| `lockBankButton_` | `unlockBankButton_` |
| `patchManagerLockBank` / `kPatchManagerLockBank` | `patchManagerUnlockBank` / `kPatchManagerUnlockBank` |
| `patchManagerBankLock` | `patchManagerBanksLocked` |
| `kBankLock` (StateProperties) | `kBanksLocked` |

**Do NOT rename:** `miscBankLockEnable`, `kBankLockEnable`, `SysExConstants::Opcode::kUnlockBank`, `sendUnlockBank()`.

---

### 4.7 Spike annotation

**File:** `Documentation/Development/Plans/2026/06/2026-06-18-Matrix-1000-Bank-Lock-MIDI-Spike.md`

**ADD header note:**
> **Status:** Not required for v1 — superseded by D-022-R1 (UNLOCK-only Bank Utility, 2026-06-19).

---

## Section 5: Implementation Handoff

### Scope classification: **Moderate**

- Direct implementation by Developer agent (7-3b).
- Backlog update: sprint-status.yaml + epics.md (PO coordination).
- No PM/Architect escalation required.

### Sequencing

1. **Approve** this Sprint Change Proposal.
2. **Update** planning artifacts (PRD, epics, decision-log) — can be same PR as code or preceding commit.
3. **`/bmad-create-story 7-3b`** — story file from Section 4.3.
4. **Implement 7-3b** — handler fix + renames + tests + builds.
5. **Mark 7.3 `done`** only after 7-3b review passes.
6. **Proceed 7.5** with amended AC (UNLOCK label + red dot).

### Handoff recipients

| Role | Responsibility |
|------|----------------|
| **Developer (Amelia)** | Story 7-3b implementation; grep verification; unit + integration builds |
| **Guillaume** | Approve proposal; manual MIDI test plan (Section 9 of handoff) |
| **Tech writer (optional)** | U-IDs inventory annotation |

### Success criteria

- [ ] Click bank N → `0AH` + PC; `patchManagerBanksLocked == true`
- [ ] Click UNLOCK → `0CH` only; `patchManagerBanksLocked == false`; bank unchanged
- [ ] Navigation wrap uses `banksLocked` per D-023
- [ ] Matrix-6: bank buttons + UNLOCK grayed; no `0AH`/`0CH`
- [ ] Grep anchors: zero stale `bankUtilityLockBank|kLockBank|patchManagerBankLock|lockBankButton_` in `Source/`
- [ ] `Matrix-Control_Tests` green

### sprint-status.yaml changes (on approval)

```yaml
7-3-patchmanageractionhandler-bank-and-internal: review  # unchanged until 7-3b done
7-3b-bank-utility-unlock-semantics-and-id-rename: ready-for-dev  # ADD after story file created
```

---

## Checklist completion summary

| Section | Status |
|---------|--------|
| 1. Trigger and context | [x] Done — Story 7.3 review + Oberheim spec |
| 2. Epic impact | [x] Done — Epic 7 primary; 8/U doc-only |
| 3. Artifact conflicts | [x] Done — PRD, epics, decision-log, 7.3, U-IDs |
| 4. Path forward | [x] Done — Direct adjustment + story 7-3b |
| 5. Proposal components | [x] Done — this document |
| 6. Final review | [!] Action-needed — user approval pending |

---

## Approval

- [x] **Approved by Guillaume** — date: 2026-06-19
- [ ] **Revisions requested** — notes: ___________
