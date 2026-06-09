---
stepsCompleted: [1, 2, 3]
session_status: complete
ideas_generated: 10
facilitation_notes: 'SCAMPER skipped — convergence via First Principles + Morphological Analysis. User validated all major decisions.'
inputDocuments:
  - _bmad-output/planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md
  - _local/Documents/Notes/figma-mockup.md
  - Source/Shared/Definitions/PluginDesignDimensions.h
  - _bmad-output/project-context.md
session_topic: 'UI dimensions strategy — SSOT (PluginDesignDimensions.h) vs alternative approaches for widgets/panels/gaps before Epic U'
session_goals: 'Homogeneous DRY geometry system; zero magic numbers; generic reusable widgets + project-specific dimensions elsewhere; Figma Lego assembly model; extensibility; single edit point; Factory-ready plan; decision before Epic U U-0'
selected_approach: 'ai-recommended'
techniques_used:
  - First Principles Thinking
  - Morphological Analysis
  - SCAMPER Method
ideas_generated: []
context_file: ''
---

# Brainstorming Session Results

**Facilitator:** Guillaume
**Date:** 2026-06-07

## Session Overview

**Topic:** UI dimensions strategy — keep `PluginDesignDimensions.h` as SSOT or adopt another approach for widgets/panels/gaps, before Epic U (UI Scale Audit & Pixel-Perfect Layout).

**Goals:**

- Single authoritative geometry source — no scattered coordinates or magic numbers
- DRY + Clean Code — readable in 6–12 months; exemplary JUCE reference project
- Generic widget classes (width/height at construction) + project dimensions injected externally (mirror `PluginDescriptors*` / `PluginDisplayNames*` pattern)
- Figma Lego model — adjacency/stacking, composable sums, ÷4 grid, `ScaledLayout` at runtime
- Reusable spacing tokens (Standard/Medium/Large paddings and gaps)
- Hierarchy reflecting Figma assembly (widgets → cells → modules → panels → GUI) without unreadable monolith
- Easy extension (new widget/panel + parent/neighbour relationships)
- Fast iteration when Figma mockup changes
- Factory integration — construction plan consumable by `WidgetFactory`
- **Decision gate:** validate or replace current SSOT approach before Epic U Story U-0

### Context Guidance

- Epic U assumes `PluginDesignDimensions` + U-0 Figma reconciliation (`figma-mockup.md` is current reference; header C++ is stale)
- Brownfield: nested namespaces, `static_assert` audit trail, partial alignment with mockup (PatchManager height budget, SharedPanel gap 24, ModuleHeader structure)
- User rejected per-class private dimension constants (early vibecoding) — caused Matrix-specific widgets and maintenance pain

### Session Setup

User confirmed session analysis 2026-06-07. Approach: AI-Recommended Techniques.

## Technique Selection

**Approach:** AI-Recommended Techniques
**Analysis Context:** UI dimensions SSOT strategy before Epic U, with focus on DRY, zero magic numbers, generic widgets + project injection, Figma Lego model.

**Recommended Techniques:**

- **First Principles Thinking:** Extract immutable design/architecture truths before comparing SSOT implementations.
- **Morphological Analysis:** Systematically explore storage × shape × consumer × verification combinations.
- **SCAMPER Method:** Stress-test leading candidates via Substitute/Combine/Eliminate/Reverse lenses.

**AI Rationale:** Engineering-heavy, architecture decision session — structured divergence first (first principles + matrix), then constrained refinement (SCAMPER). Matches user's Clean Code constraints and Epic U gate timing.

## Technique Execution Results

### First Principles Thinking (Phase 1 — complete)

**Interactive Focus:** Immutable geometry rules, calculation contract, SSOT vs widget boundary, consumer boundary.

**First Principles Ledger:**

| ID | Principle |
|----|-----------|
| P1 | Container dims @ 100% divisible by 4 |
| P2 | Internal strokes non-÷4 via ScaledDrawing |
| P3 | No coordinates in SSOT — adjacency + removeFrom* |
| P4 | Taxonomy: simple / semi-dynamic / composed / panel |
| P5 | Atoms fixed; derived = sums, no arbitrary override |
| P6 | Parent width constrained by children |
| P7 | Single edit → full GUI ripple |
| P8 | figma-mockup.md = interrogatable assembly spec |
| P9 | Panels: no ad-hoc arithmetic; ScaledLayout helpers for multi-column scale |
| P10 | Option A constexpr header — if well structured |
| P11 | Formulas visible in repo |
| P12 | SSOT aligned with PluginIDs / DisplayNames / Descriptors* |
| P13 | New element = recipe updates (accepted cost) |
| P14 | All 100% recipes live in SSOT only |
| P15 | Generic widgets — layout only inside class |
| P16 | Factory injects dimensions at construction |
| P17 | BodyPanel originX experiment = tech debt to remove (U-5) |
| P18 | **Strict decoupling:** only Factory + SSOT may include PluginDesignDimensions.h |
| P19 | resized() uses ctor-injected members — no SSOT re-read |

**Key Breakthroughs:**

- SSOT is a **construction plan** (atoms + Lego formulas), not a coordinate catalog.
- Parallel to Look/Skins: widgets/panels know **behaviour + layout**, not **project geometry source**.
- Brownfield gap: panels currently include SSOT directly — migration target for Epic U.

**Ideas Captured:**

**[Category #1]**: Lego Composition Law — adjacency graph; panel dims = fold on children + named gaps.

**[Category #2]**: Atoms vs Recipes Split — store atoms + constexpr formulas; static_assert as audit trail.

**[Category #3]**: Compile-Time Ledger — build fails on inconsistency; zero runtime cost.

**[Category #4]**: Factory Injection Mirror — fourth SSOT pillar; reusable widgets outside Matrix-Control.

**User Creative Strengths:** Precise taxonomy (simple/composed/dynamic); strong architectural consistency with existing Descriptor pattern; pedagogical clarity as design driver.

**Energy Level:** High — detailed, decisive answers; ready for structural exploration.

### Morphological Analysis (Phase 2 — complete)

**Axis 1 — SSOT file topology:** User preference **M2 (role-based split)**; facilitator recommendation aligned.

**Proposed M2+ structure (4 files):**

| File | Role |
|------|------|
| `DesignAtoms.h` | Fixed spacing + base widget dimensions (no sums) |
| `DesignRecipes.h` | Lego formulas on atoms (ParameterCell, ModulationBusCell, module blocks) |
| `DesignPanels.h` | High-level panel/GUI aggregations |
| `DesignChecks.h` | All `static_assert` cross-checks (included last) |

**Figma-only components (ParameterCore, ModulationBusCore):** exist in Figma for component design; **no C++ classes** in Matrix-Control — recipes may reference them in comments or as intermediate `constexpr` aliases for pedagogy, but runtime code uses `ParameterCell` / `ModulationBusCell` only. User confirmed this is intentional.

**[Category #5]**: M2 Role Split
_Concept_: Four headers mirror figma-mockup.md reading order (spacing/atoms → assembly recipes → panels → compile audit).
_Novelty_: Strict Factory decoupling preserved; monolith readability problem solved without zone-based shard duplication risk.

**[Category #6]**: DesignChecks.h Audit Layer
_Concept_: All compile-time identity checks in one include-last file — SSOT files stay readable; build fails fast on recipe drift.
_Novelty_: Separates "construction plan" from "accounting reconciliation" like a test file for dimensions.

**[Category #7–8]**: I4 Dimensions-as-Look + D1 central `WidgetDimensions.h` / `PanelDimensions.h`.

**[Category #9]**: Design plan under `Source/GUI/Layout/Design/` (not Shared).

**[Category #10]**: Path A — remove `buttonWidth` from Descriptors; Factory maps `widgetId` → DesignAtoms via GUI-only registry.

**PluginEditor:** inject `PluginEditorDimensions` from Factory at construction; no Design headers in editor.

---

## Decision Synthesis (Session Close — 2026-06-07)

**Status:** Approved by Guillaume. Ready to drive Epic U and a new story **U-IDs** (widget ID harmonization).

**Note:** SCAMPER phase was not executed separately — decisions converged during First Principles and Morphological Analysis.

### 1. Architecture — Geometry SSOT

| Decision | Choice |
|----------|--------|
| SSOT model | **Option A** — compile-time constexpr plan (not runtime magic) |
| File topology | **M2+** — four role-based headers under `Source/GUI/Layout/Design/` |
| Consumer model | **Strict Factory** — only Factory + Design headers include the plan |
| Widget/panel coupling | **I4 + D1** — per-type `*Dimensions` structs in `WidgetDimensions.h` / `PanelDimensions.h` (mirror `WidgetLooks.h`) |
| Layout in panels | Ctor-injected dimensions; `resized()` uses `removeFrom*` + `ScaledLayout`; no SSOT re-read |
| Figma reference | `_local/Documents/Notes/figma-mockup.md` → reconciled into Design headers in **U-0** |
| Legacy | No migration layer — plugin not public; rename freely, remove dead code |

**Design file roles:**

```
Source/GUI/Layout/Design/
  DesignAtoms.h      — spacing tokens + fixed widget atoms (no sums)
  DesignRecipes.h    — Lego formulas (ParameterCell, modules…); Figma Core steps as comments/constexpr aliases only
  DesignPanels.h     — panel + GUI root aggregations
  DesignChecks.h     — all static_assert (included last)

Source/GUI/Layout/
  WidgetDimensions.h — runtime structs for widgets
  PanelDimensions.h  — runtime structs for panels
  ScaledLayout.h     — (existing)
```

**Shared/Definitions/** keeps `PluginIDs`, `PluginDisplayNames`, `PluginDescriptors*` — **no pixel fields, no Design includes**.

### 2. Descriptors — Path A (no buttonWidth in Shared)

- Remove `std::optional<int> buttonWidth` from `StandaloneWidgetDescriptor`.
- Remove all `#include PluginDesignDimensions.h` from `PluginDescriptors*.cpp`.
- **WidgetDimensionRegistry** (GUI, used by Factory): hybrid rules + explicit exceptions + compile/test guard covering every standalone button.
- `PluginEditor` receives `PluginEditorDimensions` via `WidgetFactory::getRootGuiDimensions()` — no Design header in editor.

### 3. Widget ID naming convention (all widget IDs)

**Grammar:** `{moduleScope}{actionVerb}{optionalTarget}`

- **moduleScope** first — disambiguates zones (`internalPatches` vs `patchMutator` vs `bankUtility`).
- **actionVerb** — Init, Copy, Paste, Load, Save, Lock, Select, Mutate, Enable…
- **optionalTarget** omitted when scope is sufficient (`internalPatchesInit`, not `internalPatchesInitPatch`).

**Applies to:**

| Family | Field | Grammar example |
|--------|-------|-----------------|
| Standalone widgets | `widgetId` | `bankUtilityLockBank`, `patchMutatorEnableDco1` |
| APVTS parameters | `parameterId` | `dco1Frequency`, `miscBankLockEnable` |
| APVTS groups | `groupId` | mostly unchanged (`dco1Module`, …) |

**Validated button renames:** 33 (see table in session chat). **34 buttons unchanged** (already `{module}{Init|Copy|Paste}` or scoped Matrix Mod ids).

**Notable semantic changes:**

- `unlockBank` → **`bankUtilityLockBank`** (display **BANK LOCK** — lock current bank, not unlock).
- `bankLockEnable` → **`miscBankLockEnable`** (Misc APVTS parameter — distinct from button).
- Patch Mutator module toggles: `patchMutatorDco1` → **`patchMutatorEnableDco1`** (etc.).

**Standalone non-button annex (same story):**

- `currentBankNumber` → `internalPatchesCurrentBankNumber`
- `currentPatchNumber` → `internalPatchesCurrentPatchNumber`
- `selectPatchFile` → `computerPatchesSelectPatch`

### 4. Brownfield migration targets (Epic U)

| Current violation | Target |
|-------------------|--------|
| Panels/widgets `#include PluginDesignDimensions.h` | Factory injection only |
| `PluginDescriptors*.cpp` button widths | Registry + Path A |
| `BodyPanel` originX float workaround | Standard removeFrom* / ScaledLayout (U-5) |
| Monolithic stale `PluginDesignDimensions.h` | Replace with M2+ under `GUI/Layout/Design/` |

### 5. Recommended Epic U story order

| Story | Title | Depends on |
|-------|-------|------------|
| **U-IDs** | Widget ID harmonization (`PluginIDs`, DisplayNames, Descriptors, references) | This synthesis |
| **U-0** | Figma intake → DesignAtoms/Recipes/Panels/Checks | U-IDs recommended first |
| **U-0b** | Factory dimension registry + remove buttonWidth + PluginEditorDimensions | U-0 + U-IDs |
| **U-1** | TestComponent enrichment | parallel with U-0 |
| **U-2…U-10** | As in epic-ui-scale-audit-pixel-perfect-layout.md | U-0b |

### 6. Implementation inventory (done)

**File:** `_bmad-output/implementation-artifacts/u-ids-widget-id-harmonization-inventory.md`

**Counts:** 159 IDs reviewed — **75 renames**, remainder unchanged (already conformant).

### Session Highlights

**User:** Perfectionist clarity driver; module-scoped IDs; strict decoupling aligned with Look/Skins mental model.

**Breakthrough:** SSOT = construction plan + Factory as assembler; Shared = domain identity only; GUI = appearance + geometry.

