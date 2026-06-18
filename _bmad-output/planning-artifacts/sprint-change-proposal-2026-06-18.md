# Sprint Change Proposal — Matrix Modulation Section I/C/P

**Date:** 2026-06-18  
**Author:** Correct Course workflow (BMad)  
**Stakeholder:** Guillaume  
**Status:** Approved — 2026-06-18

---

## 1. Issue Summary

### Problem statement

The Matrix Modulation section currently exposes only a single **Init (I)** button in the section header (`MatrixModulationPanel` / `ModulationBusHeader` row). Users can reset all 10 modulation buses but cannot copy a complete modulation matrix from one patch and paste it into another — a workflow gap for creative exploration.

### Discovery context

Identified during Figma layout validation while planning alignment of section-level actions with per-bus Init buttons. The project's type-aware clipboard (FR-35, Epic 5) is a natural fit; Matrix Mod Init (Epic 3.3, done) already proves the section-header action pattern via `matrixModulationInit` → `MatrixModInitService`.

### Evidence

- Figma + screenshot validation: DESTINATION label 68 px + zero gap + I/C/P trio (60 px) preserves 128 px column width and aligns P with per-bus I buttons.
- Confirmed button size: **20×20** (not 16×16).
- User-confirmed clipboard semantics: Matrix Mod Copy grays all PATCH/MASTER module Paste buttons; only Matrix Modulation Paste remains active.
- Snapshot scope: all 10 buses (source + amount + destination × 10) = PatchModel bytes 104–133; excludes patch name and other PATCH parameters.

---

## 2. Impact Analysis

### Epic impact

| Epic | Impact |
|------|--------|
| **E3** Init | No change — section Init (I) retained |
| **E5** ClipboardService | **Extended** — new matrix-modulation clipboard mode; stories 5.1, 5.2 AC updated; **new story 5.3** (GUI) |
| **E7** ActionDispatcher | Story 7.2 AC extended — Matrix Mod section C/P routing + SysEx paste |
| E4, E6, E8–E10 | No direct impact |

### Story impact

| Story | Change |
|-------|--------|
| 5.1 | Add matrix-mod snapshot + compatibility tests |
| 5.2 | Add matrix-mod gray rules |
| **5.3** (new) | Matrix Modulation section I/C/P GUI + descriptors |
| 7.2 | Matrix Mod section C/P via `ModuleActionHandler` + `ClipboardService` |

### Artifact conflicts (resolved via this proposal)

| Artifact | Update required |
|----------|-----------------|
| `prd.md` FR-35 | Matrix-mod clipboard mode + consequences |
| `addendum.md` § Clipboard | New mode + matrix-mod table |
| `epics.md` | Epic 5 header, stories 5.1–5.3, story 7.2 |
| `architecture.md` | ClipboardService, AD-5, FR mapping |
| `.decision-log.md` | **D-095** |
| `sprint-status.yaml` | Entry `5-3-matrix-modulation-section-i-c-p-gui: backlog` |

### Technical impact

**GUI**

- `DesignAtoms.h`: `kBusDestinationTextWidth` 104 → **68** (header label only; combo `kMatrixModulationDestination` stays 104).
- Exceptional **zero gap** between DESTINATION label and I button (standard 4 px gap elsewhere).
- `MatrixModulationPanel`: replace single Init button with I/C/P trio (3 × 20 px = 60 px); layout from right edge.
- `PluginIDs`, `PluginDisplayNames`, `PluginDescriptorsMatrixModulation`, `ApvtsLayoutBuilder`: Copy/Paste entries **adjacent to** existing Init (not appended at file end).

**Core**

- `ClipboardService`: new mode `matrixModulation`; snapshot bytes 104–133.
- Paste via `ApvtsPatchMapper` + `MatrixModBusParameterSysExDispatcher` (suppress/coalesce pattern from Epic 3.3 / 2.10).
- APVTS properties: `matrixModulationCopy`, `matrixModulationPaste` (timestamp trigger pattern).

**Tests**

- ClipboardService: matrix-mod snapshot round-trip, gray rules.
- Paste: PatchModel bytes 104–133 + SysEx enqueue.

### MVP impact

**No MVP scope reduction.** Extension aligns with brief differentiator « intelligent clipboard » and existing FR-35 framework.

---

## 3. Recommended Approach

**Selected path:** Option 1 — **Direct Adjustment**

| Criterion | Assessment |
|-----------|------------|
| Effort | **Medium** — GUI + descriptors + ClipboardService mode + handler wiring |
| Risk | **Low** — reuses `MatrixModInitService` / SysEx patterns; no new MIDI opcode |
| Timeline | Fits within E5 → E7 critical path (D-058); no epic resequencing |
| Rollback | Not needed |
| MVP review | Not needed |

**Implementation sequence**

1. **5.1** — `ClipboardService` matrix-mod mode + unit tests
2. **5.2** — Paste enable/gray rules (APVTS properties)
3. **5.3** — GUI + descriptors + APVTS properties (can stub handlers initially)
4. **7.1** — `ActionDispatcher` scaffolding
5. **7.2** — `ModuleActionHandler` wires Matrix Mod C/P to `ClipboardService`

---

## 4. Detailed Change Proposals

### 4.1 PRD — FR-35 (`prd.md` §4.10)

**OLD:**
> Module Copy always enabled; Paste enabled only on modules compatible with the last Copy source. Last Copy (module or full patch) defines clipboard mode.
>
> **Consequences:** Full-patch Copy grays PATCH EDIT Paste; compatibility matrix ENV/DCO/LFO.

**NEW:**
> Module Copy always enabled; Paste enabled only on targets compatible with the last Copy source. Last Copy (module, full patch, or matrix modulation) defines clipboard mode.
>
> **Consequences:**
> - Full-patch Copy grays PATCH EDIT Paste; Internal Patches Paste remains active.
> - **Matrix Modulation Copy** grays all PATCH EDIT and MASTER module Paste; only Matrix Modulation Paste active.
> - Matrix-mod snapshot: 10 buses (source + amount + destination); excludes patch name and other PATCH params.

### 4.2 Addendum — § Clipboard (`addendum.md`)

Add mode:
> **Matrix Modulation clipboard:** Copy grays PATCH/MASTER module Paste; only Matrix Modulation Paste active.

Add table:
> Matrix Modulation section — full 10-bus copy/paste; bytes 104–133; section header target only; no cross-compatibility with module/patch modes.

### 4.3 Epics (`epics.md`)

- Epic 5 title scope: « module, patch, and matrix-modulation clipboard »
- Stories 5.1, 5.2: AC extended (approved text)
- **Story 5.3** (new): GUI I/C/P, 68 px label, gap 0, descriptors adjacent to Init
- Story 7.2: Matrix Mod section C/P via handler + SysEx paste

### 4.4 Architecture (`architecture.md`)

- `ClipboardService`: module/patch/**matrix-modulation**
- `ModuleActionHandler`: Matrix Modulation section I/C/P
- §4.4 GUI touchpoint: `ModulationBusHeader` I/C/P
- §4.10 GUI touchpoint: PATCH/MASTER ModuleHeader + Matrix Mod section header

### 4.5 Decision log — D-095

See approved entry in `.decision-log.md` (2026-06-18).

### 4.6 Sprint status (`sprint-status.yaml`)

```yaml
  5-3-matrix-modulation-section-i-c-p-gui: backlog
```

### 4.7 Implementation notes (code handoff)

| File | Change |
|------|--------|
| `DesignAtoms.h` | `kBusDestinationTextWidth`: 104 → 68 |
| `ModulationBusHeaderDimensions` | `destinationToActionsGap = 0` |
| `PanelDimensions.h` / `DimensionFactory.cpp` | Section action buttons 3 × 20 px |
| `MatrixModulationPanel.cpp` | `createSectionActionButtons`; layout X = panelWidth − 60 |
| `PluginIDs.h` | `kMatrixModulationCopy`, `kMatrixModulationPaste` next to Init |
| `PluginDisplayNames.h` | `kCopy`, `kPaste` in `Header::StandaloneWidgets` |
| `PluginDescriptorsMatrixModulation.cpp` | Copy/Paste in `kStandaloneWidgets` vector |
| `ApvtsLayoutBuilder` | Standalone properties for Copy/Paste |
| `ClipboardService` (new, E5) | `matrixModulation` mode |
| `ModuleActionHandler` (E7) | Route Matrix Mod C/P |

---

## 5. Implementation Handoff

### Scope classification

**Moderate** — backlog updates (new story 5.3, AC extensions) + coordinated E5/E7 implementation. No fundamental replan.

### Handoff recipients

| Role | Responsibility |
|------|----------------|
| **PO / Guillaume** | Apply approved artifact edits (PRD, epics, architecture, decision-log, sprint-status) |
| **Developer agent** | Implement stories 5.1 → 5.2 → 5.3 → 7.2 per sequence above |
| **Architect** | No escalation — architecture deltas captured in §4.4 |

### Success criteria

- [ ] Matrix Modulation header shows I/C/P (20×20); P right edge aligns with per-bus I buttons at all UI scales.
- [ ] Copy captures all 10 buses; Paste restores bytes 104–133 and sends SysEx to device.
- [ ] After Matrix Mod Copy, only Matrix Mod Paste is enabled; all PATCH/MASTER module Paste grayed.
- [ ] `ClipboardService` unit tests green for matrix-mod mode.
- [ ] Descriptor entries for Copy/Paste adjacent to Init in source files (not file tail).

---

## Approval

| Proposal | Status |
|----------|--------|
| 1 — PRD FR-35 | ✅ Approved |
| 2 — Addendum § Clipboard | ✅ Approved |
| 3 — Epic 5 (5.1, 5.2, 5.3) | ✅ Approved |
| 4 — Epic 7 Story 7.2 | ✅ Approved |
| 5 — Architecture | ✅ Approved |
| 6 — sprint-status.yaml | ✅ Approved |
| 7 — D-095 + GUI notes | ✅ Approved |

**Complete Sprint Change Proposal — approved by Guillaume 2026-06-18. Planning artifacts updated.**
