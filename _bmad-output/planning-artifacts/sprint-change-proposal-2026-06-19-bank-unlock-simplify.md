---
organization: Ten Square Software
project: Matrix-Control
title: Sprint Change Proposal — Bank Utility UNLOCK Simplify (MIDI-only)
author: BMad Agent
status: draft
sources:
  - Documentation/Development/Plans/2026/06/2026-06-19-Correct-Course-Bank-Utility-Unlock-Simplify-Handoff.md
  - implementation-artifacts/investigations/next-patch-after-unlock-missing-set-bank-investigation.md
  - planning-artifacts/sprint-change-proposal-2026-06-19.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/epics.md
created: 2026-06-19
updated: 2026-06-19
---

# Sprint Change Proposal — Bank Utility UNLOCK Simplify (MIDI-only)

**Date:** 2026-06-19  
**Author:** BMad Agent (Correct Course workflow)  
**Prepared for:** Guillaume  
**Status:** Approved (2026-06-19 — Guillaume)  
**Trigger document:** `Documentation/Development/Plans/2026/06/2026-06-19-Correct-Course-Bank-Utility-Unlock-Simplify-Handoff.md`  
**Investigation:** `_bmad-output/implementation-artifacts/investigations/next-patch-after-unlock-missing-set-bank-investigation.md`  
**Supersedes (partial):** Sprint Change Proposal 2026-06-19 §D-022-R2 navigation coupling, D-023 cross-bank wrap  
**Does NOT supersede:** Story 7-3b ID renames; UNLOCK label; Set Bank 0–9 paths; D-022-R3; **lock dot on Current Bank NumberBox** (revised semantics per D-023a-R3)

---

## 1. Issue Summary

### Problem statement

Story **7-3b** implemented Bank Utility UNLOCK as a **stateful** control: send `0CH`, mirror hardware lock in `patchManagerBanksLocked`, and enable **cross-bank** Prev/Next navigation when unlocked. Investigation and hardware spike (2026-06-19) show this model is **unsustainable**:

1. **Hardware asymmetry:** Oberheim `Set Bank` (`0AH`) always re-locks; front-panel Bank Lock unlock allows 3-digit entry without re-lock on bank digit — **not reproducible** via remote SysEx alone.
2. **Investigation conclusion:** The reported “missing Set Bank after UNLOCK” trace (`C0 03` only) is **by design** (locked within-bank wrap), not a defect — but follow-up #4 revealed a **real desync** when pairing UNLOCK with same-bank navigation (`clearSyncedBankState` hack).
3. **UX false expectations:** Cross-bank Prev/Next after UNLOCK caused sync bugs and user confusion; Figma layout constraints (4 px grid) forbid widening bank buttons or removing UNLOCK.

### Discovery context

| When | What |
|------|------|
| 2026-06-19 | UAT after Story 7-3b — user MIDI trace: Next after UNLOCK missing Set Bank |
| 2026-06-19 | Investigation — variant 4 confirms locked wrap explains original symptom; targeted UNLOCK→Next from 0/99 works |
| 2026-06-19 | Follow-up #4 — UNLOCK on bank 1 then Next → synth 0/07 while plugin 1/07 (hardware resets bank context on `0CH`) |
| 2026-06-19 | Product decision — simplify UNLOCK to MIDI-only remote helper; navigation always intra-bank |

### Evidence

- Investigation case file (Concluded 2026-06-19, confidence High)
- Hardware spike: `Documentation/Development/Plans/2026/06/2026-06-18-Matrix-1000-Bank-Lock-MIDI-Spike.md`
- Handoff: `2026-06-19-Correct-Course-Bank-Utility-Unlock-Simplify-Handoff.md`

---

## 2. Impact Analysis

### Epic impact

| Epic | Impact |
|------|--------|
| **Epic 7** (Patch Manager wiring) | **Direct** — Course correction on 7-3b semantics. New story **7-3c** required. Stories **7.5**, **7.6** ACs amended. 7-3b remains `done` (ID renames preserved). |
| **Epic 8** (device limits) | **Minor** — `DeviceMemoryLimits::advancePatch` signature simplified (remove `bankLocked` param). Story 8.5 docs may reference removed `patchManagerBanksLocked`. |
| **Epic 6+** | **None** |

### Story impact

| Story | Change |
|-------|--------|
| **7-3b** | **Done** — add errata note; semantics partially superseded by 7-3c |
| **7-3c** (new) | Core: dumb UNLOCK (no bank/patch writes), remove navigation reads of lock indicator, simplify `advancePatch`, revert `clearSyncedBankState`, indicator on/off on bank select/UNLOCK |
| **7.5** | Amended: active bank red text + `selectedBank` sync + **lock dot** (D-023a-R3); optional UNLOCK footer |
| **7.6** | Amended: intra-bank wrap only (remove cross-bank clause) |

### Artifact conflicts

| Artifact | Conflict | Resolution |
|----------|----------|------------|
| **PRD** FR-20, FR-22 | Lock mirror + cross-bank navigation | Update per Section 4 |
| **`.decision-log.md`** D-022-R2, D-023, D-023a | Derived lock state + cross-bank wrap | Add D-022-R4/R5, D-023-R2, D-023a-R3; amend superseded entries |
| **`epics.md`** FR-20/22, Stories 7-3b/7.5/7.6 | Same | Update per Section 4 |
| **Investigation file** | Cross-bank tests + lock dot UX | Mark resolved by D-023-R2; lock dot retained per D-023a-R3 |
| **Branch WIP** | `clearSyncedBankState` after UNLOCK | Revert in 7-3c; keep `refreshBankLockIndicator` with display-only semantics |

### Technical impact

- **Repurpose** `patchManagerBanksLocked` as **display-only** lock indicator (D-023a-R3); navigation must not read it
- **Simplify** `DeviceMemoryLimits::advancePatch` — always wrap within bank
- **UNLOCK handler** — `0CH` + indicator `false`; no bank/patch writes; no `clearSyncedBankState()`
- **Bank select** — unchanged MIDI; indicator `true`
- **Tests** — remove cross-bank/unlock-sync tests; add intra-bank wrap + indicator assertions
- **GUI** — no Bank Utility layout changes (D-022-R5); lock dot on Current Bank NumberBox per D-023a-R3

### MVP impact

**No MVP scope reduction.** Behaviour becomes **simpler and more honest** about hardware limits. UNLOCK retained as remote affordance; bank change remains explicit via buttons 0–9.

---

## 3. Recommended Approach

**Selected:** Option 1 — **Direct Adjustment** (new story 7-3c within Epic 7)

| Criterion | Assessment |
|-----------|------------|
| Effort | **Medium** (~0.5–1 dev day for 7-3c; 7.5 GUI slice separate) |
| Risk | **Low** — removes state machine; fewer sync edge cases |
| Timeline | +1 story before 7.5 GUI work |
| Rollback | **Not viable** — 7-3b ID renames stay; only semantics simplified |

**Rationale:** Hardware cannot support reliable plugin-side lock mirroring or cross-bank navigation. Simplifying to intra-bank navigation + MIDI-only UNLOCK eliminates the desync class (follow-up #4) and matches realistic user workflow (select bank explicitly, browse patches within bank).

**Explicit non-goals:** Renaming module; removing UNLOCK; Set Bank + silent Unlock pairing; cross-bank circular navigation; bidirectional `miscBankLockEnable` sync; reading hardware lock state; **sync after front-panel hardware use** (see D-022-R6 — user manual).

---

## 4. Detailed Change Proposals

### 4.1 Decision log — add D-022-R4, D-022-R5, D-023-R2, D-023a-R2; amend D-022-R2, D-023, D-023a

See applied edits in `.decision-log.md`.

### 4.2 PRD — FR-20, FR-22

**FR-20 — OLD:**
> Bank Utility provides bank buttons 0–9 (Set Bank / `0AH`, enables hardware bank lock) and an **UNLOCK** button (Unlock Bank / `0CH`). Editor state `patchManagerBanksLocked` mirrors hardware lock for navigation wrap and UI indicators. No lock toggle on Bank Utility. Internal Patches bank NumberBox shows a red lock indicator when banks are locked. Master Edit **BANK LOCK** (`miscBankLockEnable`) is a separate master parameter (byte 165).

**FR-20 — NEW (revised D-023a-R3):**
> Bank Utility provides bank buttons 0–9 (Set Bank / `0AH`) and an **UNLOCK** button (Unlock Bank / `0CH`). **UNLOCK** sends `0CH` only — no bank/patch coordinate change. **Action-derived** red dot on Current Bank NumberBox: **on** after bank select (0–9 or Set Bank path), **off** after UNLOCK; display-only, does not drive navigation. Layout frozen per Figma 4 px grid. Master Edit **BANK LOCK** (`miscBankLockEnable`) remains separate (byte 165).

**FR-22 — OLD:**
> User navigates patch number via `<` / `>` and editable patch NumberBox; navigation wraps across banks when no bank is locked (patch 99 → patch 00 next bank).

**FR-22 — NEW:**
> User navigates patch number via `<` / `>` and editable patch NumberBox; navigation **always** wraps within the current bank (patch 99 → patch 00, patch 00 → patch 99). Bank change is **only** via Bank Utility buttons 0–9. After UNLOCK, the user may use the synth front panel for 3-digit entry; plugin Prev/Next without re-selecting bank may diverge from hardware — accepted limitation, optionally documented in footer on UNLOCK click.

### 4.3 Epics — FR one-liners and stories

**FR-20 — NEW:**
> FR-20: Bank lock — UNLOCK sends `0CH` only (MIDI remote helper, no plugin lock mirror); bank select sends `0AH`.

**FR-22 — NEW:**
> FR-22: Patch navigation — `<` / `>` and NumberBox wrap within current bank only; bank change via Bank Utility 0–9.

**Story 7-3c (NEW)** — see Section 5.

**Story 7.5 — OLD AC:**
> **Then** selected bank shows red text; Internal Patches bank NumberBox shows red dot when `patchManagerBanksLocked`; UNLOCK button has no padlock icon

**Story 7.5 — NEW AC:**
> **Then** selected bank shows red text on Bank Utility 0–9; Current Bank NumberBox red dot follows lock indicator (D-023a-R3); UNLOCK button has no padlock icon; optional footer on UNLOCK

**Story 7.6 — OLD:**
> **Then** wrap across banks when unlocked

**Story 7.6 — NEW:**
> **Then** patch navigation wraps within current bank only (99 → 0, 0 → 99)

**Story 7-3b — errata:**
> **ERRATA (2026-06-19 — Correct Course D-023-R2):** AC #1–3 lock mirror and cross-bank navigation **superseded** by Story **7-3c**. ID renames (AC #4) remain valid.

### 4.4 Investigation file

Add resolution note: product pivot D-023-R2; follow-up #4 fix (`clearSyncedBankState`) **reverted**; case closed as UX/expectation + hardware limitation, not plugin defect.

---

## 5. Story Recommendation

### Recommended split: **7-3c** (Core) + **7.5** (GUI)

| Story | ID | Scope | Status after proposal |
|-------|-----|-------|----------------------|
| **7-3c** | `7-3c-bank-utility-unlock-simplify` | Intra-bank navigation; dumb UNLOCK; display-only lock indicator writes; revert `clearSyncedBankState`; tests | **backlog** |
| **7.5** | `7-5-bank-utility-ui-wiring` | Active bank red text; lock dot rendering; `selectedBank` sync; optional UNLOCK footer | **backlog** (blocked by 7-3c) |

**Why not merge:** 7-3c is Core-only (no GUI dependency except reverting dot). Keeps review focused; 7.5 remains the Figma wiring story.

### Story 7-3c — proposed AC (for create-story)

**As a** sound designer,  
**I want** UNLOCK to send remote SysEx only and patch navigation to stay within the current bank,  
**So that** plugin behaviour matches hardware limits without false lock/cross-bank state (FR-20, FR-22 corrected).

**Acceptance Criteria:**

1. **Given** Matrix-1000/1000R **When** user clicks UNLOCK **Then** `0CH` only; lock indicator `false`; bank/patch unchanged; no `clearSyncedBankState()`.
2. **When** user selects bank 0–9 **Then** Set Bank + PC unchanged; lock indicator `true`.
3. **And** Prev/Next wraps within bank only; navigation **never reads** lock indicator.
4. **And** remove cross-bank tests; add indicator + intra-bank wrap tests.
5. **And** 7.5 renders dot from indicator on Current Bank NumberBox.

---

## 6. Implementation Handoff

### Scope classification: **Moderate**

- Core simplification (7-3c) — Developer agent
- Backlog reorder — demote 7.5 until 7-3c lands
- PRD/decision-log already updated by this proposal

### Sequencing

1. **Approve** this Sprint Change Proposal
2. **`/bmad-create-story 7-3c`**
3. **`/bmad-dev-story 7-3c`** — implement + verify grep anchors (Section 10 of handoff)
4. **`/bmad-create-story 7-5`** (if story file missing) then dev when 7-3c done
5. Manual UAT per handoff §8

### Handoff recipients

| Role | Responsibility |
|------|----------------|
| **Developer (Amelia)** | Story 7-3c implementation; grep verification; unit + integration builds |
| **Guillaume** | Approve proposal; manual UAT Matrix-1000 |

### Success criteria

- [ ] UNLOCK → `0CH` + dot off; bank/patch unchanged
- [ ] Bank 0–9 → `0AH` + PC + dot on
- [ ] Prev/Next intra-bank only; navigation does not read indicator
- [ ] No `clearSyncedBankState` on UNLOCK
- [ ] Matrix-6: Bank Utility grayed
- [ ] Tests green; cross-bank tests removed

---

## 7. Checklist Status

| Section | Status |
|---------|--------|
| 1. Trigger and context | [x] Done — Investigation + handoff |
| 2. Epic impact | [x] Done — Epic 7 primary |
| 3. Artifact conflicts | [x] Done — PRD, epics, decision-log, investigation |
| 4. Path forward | [x] Done — Direct adjustment + 7-3c |
| 5. Proposal components | [x] Done |
| 6. Final review | [x] Done — approved 2026-06-19 (Guillaume); cold-start dot validated |

---

## 8. Approval

**Approved 2026-06-19** — Guillaume. Cold-start Set Bank turns lock indicator on (D-023a-R3).

**Next:** `/bmad-dev-story 7-3c`
