---
organization: Ten Square Software
project: Matrix-Control
title: Sprint Change Proposal — INIT SysEx Device Rules & Hierarchical History ComboBox
author: BMad Agent
status: approved
sources:
  - implementation-artifacts/7-6-internal-patches-panel-wiring.md
  - _local/Documents/References/MD/oberheim-matrix-1000-midi-sysex-implementation.md
  - _local/Documents/References/MD/oberheim-matrix-6-6r-midi-sysex-implementation.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/epics.md
created: 2026-07-14
updated: 2026-07-14
---

# Sprint Change Proposal — INIT SysEx Device Rules & Hierarchical History ComboBox

**Date:** 2026-07-14  
**Author:** BMad Agent (Correct Course workflow)  
**Prepared for:** Guillaume  
**Status:** Approved (2026-07-14 — Guillaume, inline during 7-6 UAT)  
**Trigger:** Story **7-6** Standalone smoke — INIT editor-only behaviour diverges from Matrix-1000 Editor prototype; Patch Mutator History UX diverges from PRD intent (two narrow comboboxes vs hierarchical popup).

---

## 1. Issue Summary

### Problem A — Internal Patches INIT (smoke test 8)

**Current behaviour (Story 7.3 AC #4):** INIT loads init template into APVTS only — **no outbound SysEx**. User must STORE to hear init on hardware.

**Expected behaviour (Guillaume prototype + Oberheim M-1000):**

| Action | Matrix-1000 | Matrix-6 / 6R |
|--------|-------------|---------------|
| **INIT** | SysEx **0x0D** — Single Patch Data to **edit buffer** | SysEx **0x01** — Single Patch Data to **current patch number** (no edit-buffer opcodes) |
| **STORE** | Unchanged v1 — **0x01** to current patch slot (smoke 10 OK) | Unchanged — **0x01** |

**Evidence:** Oberheim M-1000 MD §0DH/0EH; updated M-6/6R MD comparison table (2026-07-14) — extra opcodes `07H`–`0EH` are **M-1000 only**; M-6 accepts **01H** patch dumps to numbered slots.

**Discovery:** 7-6 UAT item 8 — plugin resets editor but synth edit buffer unchanged until manual STORE.

### Problem B — Patch Mutator History UI (layout + UX)

**Current behaviour (Story 6.6, D-082):** Two side-by-side `TSS::ComboBox` widgets (`historyMComboBox_`, `historyRComboBox_`), each **24 px** wide — labels truncated; COMPARE and action buttons shifted right.

**Expected behaviour:** One **hierarchical** combobox (primary list = roots M00–M99; hover/arrow opens secondary submenu = `—` + R00–R99), same interaction model as IDE nested menus (Cursor View → Appearance). Width = **RANDOM slider** row (**48 px** @ 100 % UI scale).

**Root cause:** D-082 (2026-05-29) explicitly chose two flat comboboxes; implementation matched spec but not Guillaume's current UX intent.

---

## 2. Impact Analysis

### Epic impact

| Epic | Impact |
|------|--------|
| **Epic 7** | **Direct** — new story **7-11** amends frozen 7.3 INIT MIDI semantics. Story 7-6 smoke checklist item 8 updated (no code change in 7-6). |
| **Epic 6** | **Direct** — new story **6-14** replaces dual-combobox panel wiring; APVTS property model from 6.6 **unchanged**. |
| **Epic U** | **Direct** — new story **U-12** introduces reusable `TSS::HierarchicalComboBox` widget. |
| **Epic 8** | **Minor** — device-type branch in INIT path uses existing `DeviceMemoryLimits` / `DeviceTypeRegistry`. |

### Story impact

| Story | Change |
|-------|--------|
| **7.3** | **Done** — errata: AC #4 INIT editor-only superseded for outbound SysEx by **7-11** |
| **7-11** (new) | Core: 0x0D encode/send on M-1000 INIT; 0x01 on M-6/6R INIT; tests + UAT grid |
| **6.6** | **Done** — errata: dual-combobox panel wiring superseded by **6-14**; property/sync Core unchanged |
| **U-12** (new) | Widget: `TSS::HierarchicalComboBox` |
| **6-14** (new) | Panel: single History control, layout width fix |

### Artifact conflicts

| Artifact | Resolution |
|----------|------------|
| **PRD FR-24** | INIT sends patch to synth per device rules (not editor-only) |
| **addendum D-044** | Add row: Internal INIT → 0x0D (M-1000 edit buffer) / 0x01 (M-6 current patch) |
| **D-082** | **Superseded** by **D-082-R2** — hierarchical single control replaces dual comboboxes |
| **epics.md** | Add stories 7-11, U-12, 6-14; errata on 7.3 AC #4 and FR-54 UX |
| **UAT grid M6-4.1** | INIT expects SysEx on hardware per device type |
| **7-6 smoke checklist** | Item 8: INIT → edit buffer audible on M-1000 |

### Technical impact

- **SysExEncoder** + **SysExConstants**: `kSinglePatchToEditBuffer = 0x0D`
- **MidiManager**: `sendPatchToEditBuffer(packedData)` (no patch number in envelope)
- **PatchManagerActionHandler::handleInternalPatchInit**: after APVTS push, device branch → enqueue appropriate SysEx; suppress flags during APVTS push unchanged
- **GUI**: new `Source/GUI/Widgets/HierarchicalComboBox.{h,cpp}`; `DesignAtoms` — replace `kPatchMutatorHistoryM/R` with `kPatchMutatorHistory = 48` (matches `kPatchMutator` slider width)

### MVP impact

**No scope reduction.** Restores hardware-faithful INIT and fixes Mutator layout regression. STORE remains 0x01 (validated UAT).

---

## 3. Recommended Approach

**Selected:** Option 1 — **Direct Adjustment** (three new stories, no rollback)

| Criterion | Assessment |
|-----------|------------|
| Effort | **Medium** — 7-11 ~0.5 day; U-12 ~1 day; 6-14 ~0.5 day |
| Risk | **Low** — additive SysEx opcode; widget isolated before panel swap |
| Rollback | **Not needed** — 7.3/6.6 Core logic preserved; panel/widget are replaceable layers |

**Implementation sequence:**

1. **7-11** — INIT SysEx (unblocks corrected UAT; no GUI dependency)
2. **U-12** — `HierarchicalComboBox` widget + TestComponent hook (optional)
3. **6-14** — Patch Mutator panel migration (depends on U-12)

---

## 4. Detailed Change Proposals

### 4.1 Decision log — add D-044-R2, D-082-R2

**D-044-R2 — Internal Patches INIT outbound SysEx (2026-07-14)**

- **Decision:** BUILD NEW — device-aware INIT MIDI after template load
- **Matrix-1000:** opcode **0x0D** (edit buffer) — matches prototype Matrix-1000 Editor
- **Matrix-6/6R:** opcode **0x01** to **current patch number** — no 0x0D/0x0E on hardware
- **STORE:** unchanged v1 — **0x01** (UAT validated)
- **Supersedes:** Story 7.3 AC #4 « editor-only INIT »

**D-082-R2 — Hierarchical History combobox (2026-07-14)**

- **Decision:** BUILD NEW — single **`TSS::HierarchicalComboBox`** replaces dual History M/R comboboxes
- **Rationale:** D-082 flat dual control causes layout overflow; PRD intent is two-level selection in one control (IDE submenu pattern)
- **Width:** `Atoms::Widths::Slider::kPatchMutator` (48 px) — aligns History row with RANDOM slider column
- **Supersedes:** D-082 panel layout clause (« two JUCE comboboxes side by side »)

### 4.2 PRD FR-24 — amend

**OLD:** INIT invokes Core via ActionDispatcher; STORE sends patch to synth memory per RAM rules.

**NEW:** INIT loads init template into APVTS **and** sends full patch SysEx to the synth: **Matrix-1000** → edit buffer (**0x0D**); **Matrix-6/6R** → current patch slot (**0x01**). STORE unchanged (**0x01** to current patch, RAM banks 0–1 on M-1000).

### 4.3 Story 7.3 AC #4 — errata

Add note: « INIT outbound SysEx deferred to 7.3 implementation as editor-only; **corrected in Story 7-11** per D-044-R2. »

### 4.4 Story 6.6 AC #5 — errata

Add note: « Dual combobox panel wiring **superseded by Story 6-14** per D-082-R2. APVTS properties and engine sync **unchanged**. »

---

## 5. Implementation Handoff

| Scope | Classification | Owner | Deliverables |
|-------|----------------|-------|--------------|
| INIT SysEx | **Moderate** | Dev story **7-11** | Encoder, MidiManager, handler, tests |
| HierarchicalComboBox | **Moderate** | Dev story **U-12** | Widget class, looks, atoms |
| Mutator panel | **Moderate** | Dev story **6-14** | Panel rewire, layout fix |

**Success criteria:**

- M-1000 Standalone: INIT → immediate audition of init template (edit buffer) without STORE
- M-6 device type (simulated): INIT → 0x01 to current patch number
- Mutator History: one 48 px control; COMPARE column aligns with RANDOM row; submenu shows R entries on M hover
- Full `Matrix-Control_Tests` green

**Story files created:**

- `_bmad-output/implementation-artifacts/7-11-internal-patches-init-sysex-device-rules.md`
- `_bmad-output/implementation-artifacts/u-12-hierarchical-combobox-widget.md`
- `_bmad-output/implementation-artifacts/6-14-patch-mutator-hierarchical-history-combobox.md`

---

## 6. Checklist Summary

| Section | Status |
|---------|--------|
| 1 Trigger & context | [x] Done |
| 2 Epic impact | [x] Done |
| 3 Artifact conflicts | [x] Done |
| 4 Path forward | [x] Direct Adjustment |
| 5 Proposal components | [x] Done |
| 6 User approval | [x] Approved 2026-07-14 |
