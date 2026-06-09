---
organization: Ten Square Software
project: Matrix-Control
title: Story U-0 — Figma Intake & Design* Reconciliation
author: BMad Agent
status: review
baseline_commit: 1fbd9cec1d0253fceb3582b2951f56c7d6321e06
sources:
  - planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md
  - brainstorming/brainstorming-session-2026-06-07-ui-dimensions-strategy.md
  - _local/Documents/Notes/figma-mockup.md
  - Source/GUI/Layout/Design/DesignAtoms.h
  - implementation-artifacts/u-ids-widget-id-harmonization.md
created: 2026-06-07
updated: 2026-06-09
---

# Story U-0: Figma Intake & Design* Reconciliation

Status: review

<!-- Prerequisite: U-IDs recommended (harmonized widget IDs for registry keys in U-0b). Blocks U-0b and all layout audit stories U-2…U-9. Does NOT wire Factory injection — compile-time plan only. -->

## Story

As a product owner,
I want Figma mockup dimensions captured in a compile-time design plan split by role,
so that every panel and widget audit has authoritative 100 % reference values on the ÷4 grid and the monolithic stale header is replaced.

## Acceptance Criteria

### AC 1 — M2+ header structure created

1. Replace monolith `Source/Shared/Definitions/PluginDesignDimensions.h` with four role-based headers under `Source/GUI/Layout/Design/`:

| File | Content rule |
|------|----------------|
| `DesignAtoms.h` | Spacing tokens (`kStandard`/`kMedium`/`kLarge` = 4/8/12) + fixed widget atoms — **no sums across unrelated atoms** |
| `DesignRecipes.h` | Lego formulas: `ParameterCell`, `ModulationBusRow`, module blocks, Figma-only `ParameterCore` / `ModulationBusCore` as **comments or constexpr aliases only** (no C++ classes) |
| `DesignPanels.h` | Panel + GUI root aggregations (Body, Header, Footer, zone sections) |
| `DesignChecks.h` | **All** `static_assert` cross-checks — included last |

2. Namespace: prefer `tss::design` or nested namespaces mirroring Figma reading order (atoms → recipes → panels). Document choice in Completion Notes. **Do not** scatter magic numbers outside Design*.

### AC 2 — Figma reconciliation @ 100 %

1. **Authority:** `_local/Documents/Notes/figma-mockup.md` (updated 2026-06-07). Owner Figma deltas → update mockup first, then Design*.
2. Every container dimension @ 100 % **divisible by 4** (brief ÷4 grid). Internal stroke thickness handled in widget paint (`ScaledDrawing`) — not in atom totals.
3. Key identities preserved or explicitly changed with delta doc (see AC 4):
   - GUI root: **1300 × 792** (MainComponent); Body inner **808 + 24 + 268 + 24 + 152** + padding 12
   - Patch Edit column **808 × 704**; Shared column stack **704** (Matrix Mod **304** + Patch Manager **400**)
   - Patch Manager modules: Bank/Internal/Computer **84** each; Mutator **100** (+16 closure in current brownfield — reconcile vs mockup note « il faut que ça rentre »)
   - SharedPanel inter-panel gap **24** (mockup flag: may adapt after Patch Manager complete)
   - Modulation bus row **268** wide; Parameter row **152** (92+60)

### AC 3 — Migration from brownfield monolith

1. Port **all** constexpr from current `PluginDesignDimensions.h` (~420 lines, 12 `static_assert`) into Design* — no silent drops.
2. `DesignChecks.h` compiles and passes all checks currently in monolith (parameter row, bus row 268, patch edit grid, master/patch height parity, shared column stack, GUI width/height stack, patch manager module sum).
3. **Transitional compile strategy (pick one, document in Completion Notes):**
   - **Option A (preferred):** thin compatibility header at old path re-exports Design* (`#include` chain ending in `DesignChecks.h`) so existing `#include PluginDesignDimensions.h` still build until U-0b removes them.
   - **Option B:** delete old header immediately and update every include in same PR — **only if** grep shows manageable touch set.
4. **Explicit non-goal:** removing `#include PluginDesignDimensions.h` from panels/widgets — **U-0b**. U-0 may leave ~25 GUI + 5 Descriptor includes on compatibility shim.

### AC 4 — Delta document

1. Append to this story file (§ Figma deltas) or create `implementation-artifacts/u-0-design-reconciliation-deltas.md`:
   - Table: **symbol / zone | brownfield px @ 100 % | figma px @ 100 % | action (keep / change / TBD)**
   - Minimum rows: PatchMutatorModule height (+16 closure), SharedPanel gap 24, any header/footer dims if mockup differs from 2.11 implementation, button width names post U-IDs (`kPatchManagerUnlockBank` → align with `bankUtilityLockBank` semantic in comments only — pixel value unchanged unless mockup says otherwise).

### AC 5 — Per-zone dimension tables for U-2…U-9

1. Append to `planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md` (new § **Dimension tables post U-0**) **or** link standalone `implementation-artifacts/u-0-zone-dimension-tables.md` with tables for:
   - Shell (Body, MainComponent, separators)
   - Header / Footer
   - Patch Edit (top / middle / bottom)
   - Matrix Mod
   - Patch Manager (4 modules)
   - Master Edit
   - Transversal widgets (heights/widths used in U-2)
2. Each table: design px @ 100 %, Design* symbol path, UAT story reference (U-2…U-9).

### AC 6 — Build & verification

1. Standalone Debug + unit tests compile.
2. `grep PluginDesignDimensions` in `Source/` — if compatibility shim used, **only** shim + U-0-allowed consumers (Factory, Descriptors, panels) — no new direct reads in new Design* files from widgets (widgets still on shim OK).
3. Completion Notes: confirm all 12 legacy `static_assert` messages reproduced in `DesignChecks.h`.

## Tasks / Subtasks

- [x] **Read & reconcile** (AC: #2, #4)
  - [x] Walk `figma-mockup.md` top → bottom; annotate mismatches vs `PluginDesignDimensions.h`
  - [x] Owner sign-off on intentional deltas (Mutator +16, gap 24, etc.)

- [x] **DesignAtoms.h** (AC: #1, #2)
  - [x] Spacing tokens from mockup § Spacing
  - [x] Widget heights/widths (Label 92×16, Button 20, ModuleHeader 32, …)
  - [x] No composed panel totals in Atoms

- [x] **DesignRecipes.h** (AC: #1, #2)
  - [x] ParameterCell, ModulationBusCell, ModuleHeader stack, PatchNameColumn vertical stack
  - [x] Figma-only cores as comments/aliases

- [x] **DesignPanels.h** (AC: #1, #2, #3)
  - [x] Body sections, SharedColumn, GUI root constants
  - [x] Header/Footer/Body panel aliases

- [x] **DesignChecks.h** (AC: #3, #6)
  - [x] Port all 12 `static_assert` from monolith
  - [x] Include order: Atoms → Recipes → Panels → Checks

- [x] **Compatibility / removal** (AC: #3)
  - [x] Implement Option A shim or Option B mass include update
  - [x] Update `WidgetFactory.cpp` to include Design* if shim not used at Factory layer

- [x] **Documentation** (AC: #4, #5)
  - [x] Delta table artifact
  - [x] Zone dimension tables for U-2…U-9

- [x] **Build** (AC: #6)

## Dev Notes

### Depends on

| Story | Relationship |
|-------|----------------|
| **U-IDs** | Recommended first — button atom comments align with new widget IDs; not blocking compile |
| **U-0b** | Blocked until U-0 merges — consumes Design* + removes shim/includes |

### Brownfield monolith map (migration guide)

Current `PluginDesignDimensions.h` structure → target file:

| Monolith section | Target |
|------------------|--------|
| `Widgets::Heights/Widths` (lines 10–121) | `DesignAtoms.h` + width formulas in `DesignRecipes.h` (`ModulationBusRow`, `ParameterRow`) |
| `Panels::Body::*` (124–268) | `DesignPanels.h` |
| `GUI::*` (271–286) | `DesignPanels.h` (GUI root) |
| `Panels::Header/Footer/Body` aliases (288–307) | `DesignPanels.h` |
| Widget widths derived from panels (309–345) | `DesignRecipes.h` or `DesignPanels.h` (panel-scoped widget widths) |
| `static_assert` block (347–418) | `DesignChecks.h` |

### Known stale / debt items (document, do not fix layout here)

| Item | Notes | Fixed in |
|------|-------|----------|
| `BodyPanel` originX float | Scaling workaround | U-5 |
| Panels read SSOT in `resized()` | Factory injection | U-0b |
| `buttonWidth` in Descriptors | Registry | U-0b |
| `PluginEditor.h` includes design header | `getRootGuiDimensions()` | U-0b |

### Figma-only components (no C++ class)

- `ParameterCore`, `ModulationBusCore` — recipe steps only; runtime uses `ParameterCell`, `ModulationBusCell`.

### Principles (from brainstorming P1–P19)

- SSOT = construction plan, not coordinate catalog (P3, P14).
- Atoms fixed; derived = named sums (P5).
- Formulas visible in repo (P11).
- Design* lives under **GUI**, not Shared (category #9).

### CMake

- Add new headers to IDE project if required by generator (header-only — usually picked up automatically).

### References

- [Source: _local/Documents/Notes/figma-mockup.md]
- [Source: Source/GUI/Layout/Design/DesignAtoms.h]
- [Source: _bmad-output/brainstorming/brainstorming-session-2026-06-07-ui-dimensions-strategy.md § Decision Synthesis]
- [Source: planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md § Architecture dimensions & IDs]

## Figma deltas

| Symbol / zone | Brownfield @ 100 % | Figma @ 100 % | Action |
|---------------|-------------------|---------------|--------|
| GUI root (MainComponent) | 1300 × 792 | 1300 × 792 | keep |
| Body inner width stack | 808 + 24 + 268 + 24 + 152 | 808 + 24 + 268 + 24 + 152 | keep |
| Patch Edit column | 808 × 704 | 808 × 704 | keep |
| Shared column stack | 704 (304 + 400) | 704 (304 + 400) | keep |
| PatchMutatorModule content | 100 | 100 | keep |
| PatchMutatorModule total (+16 closure) | 116 | 100 (note: « il faut que ça rentre ») | keep brownfield closure — TBD in U-8 audit |
| SharedPanel inter-panel gap | 24 | 24 (flag: may adapt post Patch Manager) | keep |
| Bank/Internal/Computer module height | 84 each | 84 each | keep |
| Modulation bus row width | 268 | 268 | keep |
| Parameter row width | 152 (92+60) | 152 (92+60) | keep |
| HeaderPanel height | 32 | 32 | keep |
| FooterPanel height | 32 | 32 | keep |
| `kPatchManagerUnlockBank` width | 76 px | 76 px | keep value; comment aligned to `bankUtilityLockBank` (U-IDs) |

## Dev Agent Record

### Agent Model Used

Claude (Cursor Agent)

### Debug Log References

- Build: `cmake --preset default-macos-arm64 && cmake --build Builds/macOS/ARM` — success
- Tests: `Matrix-Control_Tests` — all tests passed (pre-existing JUCE leak assertions at shutdown)

### Completion Notes List

- **Namespace choice:** `tss::design` with nested `Spacing`, `Atoms`, `Recipes`, `Panels`, `GUI`, `PanelWidgets` mirroring Figma reading order (atoms → recipes → panels).
- **Option A shim:** `PluginDesignDimensions.h` now includes `DesignLegacyCompat.h` which aliases every legacy symbol to `tss::design`. No consumer include changes required.
- **WidgetFactory:** unchanged — continues to include shim via `PluginDesignDimensions.h` (Option A).
- **static_assert:** all 14 legacy checks ported to `DesignChecks.h` with identical message strings (story cited 12; monolith had 14).
- **grep AC6:** Design* files reference `tss::design` only; `PluginDesignDimensions` appears in shim + ~25 allowed consumers (panels, Factory, Descriptors).
- **Zone tables:** standalone `u-0-zone-dimension-tables.md`; linked from epic file § Dimension tables post U-0.

### File List

- `Source/GUI/Layout/Design/DesignAtoms.h` (new)
- `Source/GUI/Layout/Design/DesignRecipes.h` (new)
- `Source/GUI/Layout/Design/DesignPanels.h` (new)
- `Source/GUI/Layout/Design/DesignChecks.h` (new)
- `Source/GUI/Layout/Design/DesignLegacyCompat.h` (new)
- `Source/Shared/Definitions/PluginDesignDimensions.h` (shim — was monolith)
- `_bmad-output/implementation-artifacts/u-0-zone-dimension-tables.md` (new)
- `_bmad-output/implementation-artifacts/u-0-figma-intake-and-design-reconciliation.md` (updated)
- `_bmad-output/planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md` (link added)
- `_bmad-output/implementation-artifacts/sprint-status.yaml` (updated)

## Change Log

- 2026-06-09: Story U-0 implemented — M2+ Design* split under `Source/GUI/Layout/Design/`, Option A compatibility shim, Figma delta table, zone dimension tables for U-2…U-9.
