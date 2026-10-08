# Sprint Change Proposal — GETTING STARTED wizard

**Project:** Matrix-Control  
**Date:** 2026-10-08  
**Author:** Correct Course workflow (Incremental — Guillaume)  
**Change signal:** Align sprint / deferred / epics-stories with product chantier **GETTING STARTED** (multi-step first-run assistant), replacing Device Setup one-shot and vague « First-run setup assistant » scope  
**Product SSOT:** `Documentation/Development/Plans/2026/10/2026-10-08-Getting-Started-Wizard-Decisions.md`  
**Scope classification:** **Moderate** — backlog reorganization (new Epic GS + stories), tracking/docs updates; no code in this Correct Course; no rollback of shipped epics

---

## 1. Issue Summary

### 1.1 Problem Statement

Device Setup today is a **one-shot** MIDI + EPROM modal. Settings is now unified (audio + MIDI). Standalone users still need a path that unlocks **edit, play, and hear**; plugin users must not walk the same Audio / Keyboard From path as Standalone.

A deferred smoke item (2026-10-02) and the smoke guide still track a vague **« First-run setup assistant »** (Scale + Skin + Keyboard + audio + MIDI in one overloaded intention). That conflicts with the locked product direction: a dedicated multi-step **GETTING STARTED** wizard (Previous / Next), per-step flags, Settings UI for Scale/Skin + Getting Started preference, plugin ≠ Standalone applicability.

Leaving both intentions in tracking would cause Spec/Build to reopen discarded approaches (mega-modal, pastilles, auto-open Settings alone, Device Setup-only for Standalone).

### 1.2 Trigger Type

- **Strategic / product pivot** during post-sprint hardening — requirements clarified after Settings unification and smoke follow-up.
- **Not** a failed implementation of Epic 7/8 (those remain done).

### 1.3 Evidence

| Source | Finding |
|--------|---------|
| Decisions plan 2026-10-08 | Frozen product SSOT (steps, buttons, flags, copy, Settings block) |
| `deferred-work.md` ~2209 | First-run setup assistant — wrong vehicle vs Device Setup / UltraWide |
| `guide-smoke-window-modal-look.md` | § First-run + execution order #5 |
| Shipped Device Setup specs | Assistant / cross-project friction / welcome — historical one-shot |
| UX chrome (session) | Matrix monochrome like Settings; same width; lower height per step |
| `sprint-status.yaml` | All numbered epics done — no GS tracking yet |

---

## 2. Impact Analysis

### 2.1 Checklist Summary

| Section | Status | Notes |
|---------|--------|-------|
| 1 — Trigger & context | [x] Done | Pivot to GETTING STARTED; SSOT locked |
| 2 — Epic impact | [x] Done | New **Epic GS**; do not reopen Epic 7/8 |
| 3 — Artifact conflicts | [x] Done | Deferred, smoke, modal guide, PRD addendum, architecture, Device Setup specs |
| 4 — Path forward | [x] Done | **Direct Adjustment** |
| 5 — Proposal components | [x] Done | This document + approved edit proposals |
| 6 — Final review | [x] Done | **Approved by Guillaume 2026-10-08**; artifact edits applied |

### 2.2 Epic Impact

| Epic | Status | Impact |
|------|--------|--------|
| Epics 0–12, U, T, V1 | **done** | No reopen; no rollback |
| Smoke #6 Audio Settings Matrix rebuild | Separate | Remains after / outside GETTING STARTED |
| **Epic GS — Getting Started** | **new — backlog** | GS-1…GS-4 |

**Recommended implementation order:** GS-1 (Settings UI) before or opening Spec/Build of wizard → GS-2 shell → GS-3 steps/flags/absorb Device Setup → GS-4 manual (doc, parallel OK).

### 2.3 Story Impact

| Story | Action |
|-------|--------|
| GS-1 Settings User Interface — Scale, Skin, Getting Started block | **Add** backlog |
| GS-2 Wizard shell, navigation, copy, chrome | **Add** backlog |
| GS-3 Steps, flags, plugin vs Standalone | **Add** backlog |
| GS-4 Manual — first launch & host keyboard | **Add** backlog (doc dependency) |
| Historical Device Setup stories/specs | **Supersede for future work** — keep as implementation record |

### 2.4 Artifact Conflicts

| Artifact | Change |
|----------|--------|
| PRD `prd.md` core FRs | **No rewrite** — MVP intact; FR-40 already allows Skin/Scale in Settings |
| PRD addendum | **Add** Getting Started section |
| Decision log | **Add** D-entry pointing at 2026-10-08 plan + Epic GS |
| Architecture | **Light** — Settings/Getting Started mapping + per-step flags note |
| UX DESIGN/EXPERIENCE | **N/A** (none); modal guide is UX SSOT for chrome |
| `guide-matrix-modal-design.md` | GETTING STARTED chrome + width/height |
| `guide-smoke-window-modal-look.md` | Recast First-run → GETTING STARTED; order #5 |
| `deferred-work.md` | Recast ~2209; supersede unused SKIN/UI SCALE string deferral |
| Device Setup specs (3) | Supersede banner for future work |
| `epics.md` + `sprint-status.yaml` | Epic GS + stories backlog |

### 2.5 Technical Impact (for later Spec/Build — not this Correct Course)

- Absorb `EpromTypePromptDialog` / Device Setup into wizard STEP 2; retire CONFIRM / SPECIFY LATER / single `promptDone` as sole completion model.
- Machine prefs: **per-step** flags + auto-open combo; plugin vs Standalone applicability; Configure later policy.
- Reuse Settings/header MIDI/audio population — no duplicated lists.
- Distinct from audio-safety `sceneAudioSafetyDefaultsApplied` first-run gate.

---

## 3. Recommended Approach

**Selected:** Option 1 — **Direct Adjustment**

| Option | Verdict |
|--------|---------|
| 1 Direct Adjustment | **Chosen** — new Epic GS + tracking/docs |
| 2 Rollback | Not viable — shipped Settings/Device Setup remain useful bases |
| 3 MVP Review | Not needed — MVP goals unchanged |

**Effort:** Medium (Spec + Build of wizard + Settings UI; manual doc separate)  
**Risk:** Low (product locked; main risk is dual tracking if edits not applied)  
**Timeline:** Next Spec/Build cycle after approval; smoke #6 stays later

**Rationale:** Product decisions are frozen; conflict is backlog language and missing epic/stories. Direct adjustment prevents dual intention without rewriting history.

---

## 4. Detailed Change Proposals

All proposals below were **Approved** by Guillaume in Incremental mode (2026-10-08).

### 4.1 Deferred — First-run entry (~2209)

**File:** `_bmad-output/implementation-artifacts/deferred-work.md`

Replace First-run setup assistant summary/evidence with GETTING STARTED scope + SSOT plan path + Epic GS pointer (see chat proposal 1 NEW text).

### 4.2 Smoke guide

**File:** `_bmad-output/implementation-artifacts/guide-smoke-window-modal-look.md`

- Rename § First-run → GETTING STARTED checklist (steps, flags, Settings block, UltraWide Scale).
- Execution order item 5 → `GETTING STARTED (wizard + Settings UI Scale/Skin/bloc)`.

### 4.3 Epic GS + sprint-status

**Files:** `planning-artifacts/epics.md`, `implementation-artifacts/sprint-status.yaml`

```yaml
  # Epic GS: Getting Started wizard (Correct Course 2026-10-08)
  epic-gs: backlog
  gs-1-settings-ui-scale-skin-getting-started-block: backlog
  gs-2-getting-started-wizard-shell-and-navigation: backlog
  gs-3-getting-started-steps-flags-plugin-standalone: backlog
  gs-4-manual-first-launch-and-host-keyboard: backlog
```

Epic narrative + four stories as approved in proposal 3.

### 4.4 Modal design guide

**File:** `_bmad-output/implementation-artifacts/guide-matrix-modal-design.md`

Chrome list includes GETTING STARTED; same width as Settings; lower per-step height; titles in title band; shared bricks.

### 4.5 PRD addendum

**File:** `planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md`

Append « Getting Started wizard (Correct Course 2026-10-08) » section (proposal 5 NEW).

### 4.6 Architecture + decision log

**Files:** `architecture.md`, `.decision-log.md`

- FR→structure Settings row includes Getting Started.
- Modal/error pattern note: per-step flags; no single setup-done for both formats; no duplicated device lists.
- New decision-log entry (next free D-id) → plan SSOT + Epic GS.

### 4.7 Deferred — unused SKIN/UI SCALE strings

**File:** `deferred-work.md` (2026-09-03 Settings reorg)

Mark superseded by Epic GS-1 (Correct Course 2026-10-08).

### 4.8 Device Setup specs — supersede banners

**Files:**

- `spec-device-setup-assistant.md`
- `spec-device-setup-cross-project-friction.md`
- `spec-device-setup-welcome-intro.md`

Banner: superseded for future work by GETTING STARTED / Epic GS; keep as historical record.

---

## 5. Implementation Handoff

### 5.1 Scope classification

**Moderate** — backlog reorganization + planning/tracking docs. After approval, apply artifact edits in this Correct Course (or immediate follow-up). Code via Spec → Build.

### 5.2 Handoff

| Role | Responsibility |
|------|----------------|
| **This Correct Course (after yes)** | Apply §4 edits to artifacts; update `sprint-status.yaml` |
| **Spec** (`bmad-spec`) | Spec(s) for GS-1 and/or GS-2+GS-3 from product SSOT — do not reopen discarded UX approaches |
| **Build** (`bmad-build`) | Implement per Spec; absorb Device Setup; Settings UI |
| **Manual / doc** | GS-4 — user manual first launch + host keyboard (DAW examples only in manual) |
| **PM / Architect** | Only if Spec discovers need to reopen product — otherwise not required |

### 5.3 Success criteria

1. Tracking shows **only** GETTING STARTED (no parallel First-run / Device Setup élargi target).
2. Epic GS stories backlog in `sprint-status.yaml` and `epics.md`.
3. Deferred + smoke + modal guide + Device Setup supersede notes applied.
4. Next skill: **Spec** (GS-1 ± wizard) then **Build**; manual GS-4 tracked as doc.
5. Product copy/buttons/steps remain those in the 2026-10-08 decisions plan.

### 5.4 Next skills (after approve + apply)

1. Apply this proposal’s artifact edits.  
2. `/bmad-spec` — Getting Started (+ Settings UI Scale/Skin/block).  
3. `/bmad-build` — implement.  
4. Manual update (GS-4) when ready — not a code Spec detail.

---

## 6. Out of scope (locked)

- Code implementation in Correct Course  
- Reopening: pastilles, mega-modal, Settings-only auto-open, manual-only  
- Second « full setup » button  
- Changing frozen English UI copy without artifact inconsistency  
- Detailed DAW examples inside the wizard  
- Merging Audio Settings Matrix rebuild into GETTING STARTED  

---

## Approval

- Incremental edit proposals 1–8: **Approved** (Guillaume, 2026-10-08)  
- Full Sprint Change Proposal: **Approved** (Guillaume, 2026-10-08)  
- Artifact edits applied: **2026-10-08**  
- Handoff: Spec (`bmad-spec`) → Build (`bmad-build`); GS-4 manual as doc track
