---
organization: Ten Square Software
project: Matrix-Control
title: Epic U — UI Scale Audit & Pixel-Perfect Layout
author: BMad Agent
status: ready
version: "1.1"
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md
  - planning-artifacts/briefs/brief-Matrix-Control-2026-05-22/brief.md
  - planning-artifacts/briefs/brief-Matrix-Control-2026-05-22/brownfield-inventory-for-prd.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - project-context.md
  - implementation-artifacts/2-11-header-panel-layout-and-widget-styling.md
  - brainstorming/brainstorming-session-2026-06-07-ui-dimensions-strategy.md
  - implementation-artifacts/u-ids-widget-id-harmonization-inventory.md
  - _local/Documents/Notes/figma-mockup.md
created: 2026-06-07
updated: 2026-06-07
---

# Epic U: UI Scale Audit & Pixel-Perfect Layout

## Overview

**Epic U** formalise l’application **exhaustive** du système de mise à l’échelle existant (`ScaledLayout`, `ScaledDrawing`, plan de dimensions compile-time) sur **toute l’UI de production** — widgets, panneaux, gaps et paddings — avec validation manuelle à chaque preset UI Scale **50–200 %**, enrichissement progressif de `TestComponent` en debug (D-064), et critère de sortie release (D-062 / D-063).

**Prérequis architecture (brainstorming 2026-06-07) :** avant l’audit pixel, harmoniser les **IDs widgets** (U-IDs), restructurer le **SSOT dimensions** (U-0 / U-0b), puis auditer zone par zone (U-2…U-9).

**Références décisionnelles :**

| ID | Portée Epic U |
|---|---|
| **D-013** | Presets prod 50–200 % ; audit hairlines (`ScaledDrawing`) ; >200 % hors v1 (D-065) |
| **D-062** | Retrait sandbox du binaire **release** quand audit prod terminé |
| **D-063** | `TestComponent` + bouton **UI Elements** réservés au debug (`JUCE_DEBUG`) post-release |
| **D-064** | Enrichissement sandbox : tous les widgets custom × chaque preset |
| **Brief Figma** | Grille **÷4** : toutes les dimensions design divisibles par 4 à 100 % |

**FR / UX couverts (aspect layout uniquement) :** FR-43 (geometry, pas wiring), UX-DR8.

**Hors périmètre — ne pas dupliquer :**

| Artefact | Raison |
|---|---|
| **Story 7.8** | Shell fonctionnel : persistance prefs (FR-3), wiring skin/scale (FR-42/43), logo popup (FR-41), footer messages (FR-44), graying (FR-45) |
| **Stories Epic 2 done (2.7, 2.8, 2.9b, 2.11)** | Header routing cluster, audio/peak, activity LEDs — **gelées** ; Epic U = audit/sign-off + cluster droit header uniquement |
| **Story 7.7** | Contenu fonctionnel Settings (pas re-layout Settings sauf gaps si owner le demande explicitement) |
| **Epic 10** | Géométrie interactive EnvelopeDisplay / TrackGeneratorDisplay (drag rules) — Epic U couvre bounds/gaps/paint scale |

**Dépendances :**

- **Soft-start** dès maintenant — ne bloque pas D-058 (Core Epics 3–6) ; travail GUI en parallèle.
- **Recommandé avant** merge release v1 et **avant** clôture 7.8 sur le volet layout shell.
- **Gate release v1 :** Story U-10 done.

**Livrable owner (input Figma) :** dimensions exactes widgets/panneaux/gaps — référence `_local/Documents/Notes/figma-mockup.md`, injectées en Story U-0 dans les en-têtes `Design*` et checklists UAT par zone.

---

## Architecture dimensions & IDs (décisions 2026-06-07)

Synthèse complète : `brainstorming/brainstorming-session-2026-06-07-ui-dimensions-strategy.md`

### Plan de dimensions (SSOT compile-time)

Remplace le monolithe `Source/Shared/Definitions/PluginDesignDimensions.h` :

```
Source/GUI/Layout/Design/
  DesignAtoms.h      — espacements + atomes widgets (pas de sommes)
  DesignRecipes.h    — formules Lego (ParameterCell, modules…)
  DesignPanels.h     — agrégations panneaux + GUI racine
  DesignChecks.h     — tous les static_assert (inclus en dernier)
```

Référence produit : `figma-mockup.md` (réconciliée en U-0).

### Contrat runtime (miroir Looks)

```
Source/GUI/Layout/
  WidgetDimensions.h   — structs injectées aux widgets
  PanelDimensions.h    — structs injectées aux panneaux
  ScaledLayout.h       — (existant)
```

### Modèle strict Factory

| Couche | Rôle |
|--------|------|
| `Design*.h` | Plan @ 100 % (constexpr) |
| `WidgetFactory` | **Seul assembleur** — lit Design*, remplit `*Dimensions`, injecte Look + dims aux constructeurs |
| `PluginDescriptors*` (Shared) | Identité domaine uniquement — **pas de pixels**, pas de `#include Design*` |
| Widgets / panneaux | Layout interne ; dims membres injectées au ctor ; `resized()` sans relire le plan |

`PluginEditor` reçoit `PluginEditorDimensions` via `WidgetFactory::getRootGuiDimensions()` — aucun en-tête Design dans l’éditeur.

### Convention IDs (`PluginIDs.h`)

**Grammaire :** `{scopeModule}{verbe}{cible optionnelle}` (standalone) ; `{scopeModule}{nomSémantique}` (paramètres APVTS).

Exemples : `bankUtilityLockBank`, `internalPatchesInit`, `miscBankLockEnable`, `vcfVcaVca1Volume`, `patchEditPatchName`.

**Inventaire rename (75 ids) :** `implementation-artifacts/u-ids-widget-id-harmonization-inventory.md`

**Sémantique produit :** `unlockBank` → `bankUtilityLockBank` (libellé **BANK LOCK** — verrouiller la banque courante).

**Migration :** aucune — plugin non public ; rename direct, pas de code legacy.

### Dette brownfield ciblée

| Violation actuelle | Story corrective |
|--------------------|----------------|
| `#include PluginDesignDimensions.h` dans panneaux/widgets | U-0b + U-2…U-9 |
| `.buttonWidth` dans `PluginDescriptors*` | U-0b |
| `BodyPanel` originX float (expériment scaling) | U-5 |
| Monolithe `PluginDesignDimensions.h` stale vs mockup | U-0 |

## Principes d’implémentation (SSOT)

1. **100 % design** = valeurs Figma **÷4** → `DesignAtoms.h` / `DesignRecipes.h` / `DesignPanels.h` ; garde-fous dans `DesignChecks.h`.
2. **Runtime** = `ScaledLayout::scaledInt(designPx, uiScale)` dans `resized()` ; **jamais** `AffineTransform` global sur l’arbre prod.
3. **Strokes** = `ScaledDrawing` (hairline 1 px @ 50–150 %, 2 px @ 200 % — D-013).
4. **Gaps** = atomes ou recettes nommées dans Design* ; **pas de magic numbers** dans panneaux/widgets.
5. **Injection** = Factory seule consommatrice du plan ; panneaux/widgets reçoivent `*Dimensions` au constructeur ; `resized()` utilise membres + `removeFrom*` / helpers `ScaledLayout`.
6. **Propagation** = `uiScale_` depuis `PluginEditor` → `MainComponent` → panneaux → widgets (pattern brownfield existant).
7. **Validation** = checklist manuelle **7 presets** (50, 75, 100, 125, 150, 175, 200 %) × zone ; captures Preview/Photoshop agrandies (D-064).

---

## Inventaire brownfield (cibles audit)

| Zone | Fichiers principaux | État layout pré-audit |
|---|---|---|
| Shell | `MainComponent`, `BodyPanel`, `PluginEditor` | Partiel — injection dims Factory ; retirer originX float (U-5) |
| Header | `HeaderPanel` | Gauche : **done** (2.11) ; droite SCALE/SKIN/UI Elements : à auditer |
| Footer | `FooterPanel` | Partiel |
| Patch Edit | `PatchEditPanel`, `PatchEditTop/Bottom/Displays` | Layout présent ; gaps Figma à confirmer |
| Matrix Mod | `MatrixModulationPanel`, `ModulationBusCell/Header` | Partiel |
| Patch Manager | `PatchManagerPanel`, 4× modules | Partiel |
| Master Edit | `MasterEditPanel`, `BaseModulePanel` | Partiel |
| Widgets | `Source/GUI/Widgets/*` | Hétérogène — priorité transversale U-2 |

---

## Story list (ordre recommandé)

| Story | Titre | Zone |
|---|---|---|
| **U-IDs** | Widget ID harmonization (`PluginIDs`, DisplayNames, Descriptors) | Identité / Shared |
| **U-0** | Figma intake & `Design*` reconciliation | SSOT dimensions |
| **U-0b** | Factory dimension registry + Descriptor decoupling | Factory / injection |
| **U-1** | TestComponent enrichment (D-064) | Debug harness |
| **U-2** | Transversal widgets scale audit | Widgets |
| **U-3** | Header right cluster & sign-off | Header |
| **U-4** | Footer panel layout audit | Footer |
| **U-5** | Body shell — padding, separators, column gaps | Shell |
| **U-6** | Patch Edit panels layout audit | Patch Edit |
| **U-7** | Matrix Modulation panel layout audit | Matrix Mod |
| **U-7b** | Matrix Mod reorder drag colours | Matrix Mod |
| **U-8** | Patch Manager panels layout audit | Patch Manager |
| **U-9** | Master Edit panel layout audit | Master Edit |
| **U-10** | Release gate — prod audit sign-off & D-062/D-063 | Gate |

**Parallélisation :**

- **U-IDs** en premier (rename transversal ; inventaire prêt).
- **U-0** après U-IDs (Design* aligné mockup).
- **U-0b** après U-0 (registre boutons + retrait `buttonWidth` + `PluginEditorDimensions`).
- **U-1** en parallèle de U-0 / U-0b (dimensions provisoires OK).
- **U-2** après U-0b ; **U-5** avant U-6…U-9 ; U-6…U-9 parallélisables après U-5.

---

## Story U-IDs: Widget ID Harmonization

As a maintainer,
I want all widget and parameter IDs to follow a single module-scoped naming convention,
So that Shared descriptors stay pixel-free, code reads unambiguously, and the Factory registry stays maintainable.

**Acceptance Criteria:**

**Given** the rename inventory in `implementation-artifacts/u-ids-widget-id-harmonization-inventory.md` (75 renames, 159 IDs reviewed)
**When** Story U-IDs merges
**Then** all listed `widgetId` and `parameterId` strings in `PluginIDs.h` are updated ; `PluginDisplayNames.h` and `PluginDescriptors*.cpp` follow
**And** convention documented in epic § Architecture dimensions & IDs : `{scopeModule}{verbe}{cible?}` / `{scopeModule}{nomSémantique}`
**And** `bankUtilityLockBank` replaces `unlockBank` (display **BANK LOCK** — lock current bank)
**And** `miscBankLockEnable` replaces `bankLockEnable` (distinct from button ID)
**And** `grep` on every **old** ID string returns zero hits in `Source/`
**And** APVTS mappers / Factory validator maps updated ; build + smoke test pass

**Out of scope:** Design* headers (U-0), Factory dimension registry (U-0b), layout audit (U-2…U-9).

**Inventory:** `implementation-artifacts/u-ids-widget-id-harmonization-inventory.md`

---

## Story U-0: Figma Dimension Intake & Design* Reconciliation

As a product owner,
I want Figma mockup dimensions captured in the compile-time design plan,
So that every panel and widget audit has authoritative 100 % reference values (÷4 grid).

**Acceptance Criteria:**

**Given** owner reference `_local/Documents/Notes/figma-mockup.md` (and Figma if deltas)
**When** Story U-0 merges
**Then** monolithic `PluginDesignDimensions.h` is replaced by `Source/GUI/Layout/Design/DesignAtoms.h`, `DesignRecipes.h`, `DesignPanels.h`, `DesignChecks.h`
**And** every atom is **divisible by 4** @ 100 % ; composed sizes are constexpr formulas (Lego), not magic totals
**And** `DesignChecks.h` contains all `static_assert` cross-checks ; no duplicate dimension tables in panel `.cpp` files
**And** a per-zone dimension table is appended to this epic artifact (or linked UAT sheet) for U-2…U-9 checklists
**And** discrepancies vs brownfield layout are listed as explicit deltas (before/after design px)
**And** old `PluginDesignDimensions.h` removed ; no references remain outside Factory + Design* (post U-0b)

**Out of scope:** Factory injection (U-0b), functional wiring, skin tokens, popup behaviour (7.8).

---

## Story U-0b: Factory Dimension Registry & Strict Decoupling

As a developer,
I want the Factory to be the sole consumer of the design plan and inject dimensions at construction,
So that widgets and panels stay generic and Shared descriptors contain no pixel fields.

**Acceptance Criteria:**

**Given** U-0 Design* headers and U-IDs harmonized widget IDs
**When** Story U-0b merges
**Then** `WidgetDimensions.h` and `PanelDimensions.h` define runtime structs (mirror `WidgetLooks.h` pattern)
**And** `WidgetFactory` builds and injects `*Dimensions` + Look at widget/panel construction
**And** `WidgetDimensionRegistry` (hybrid rules + explicit exceptions + guard test) replaces `StandaloneWidgetDescriptor::buttonWidth`
**And** `buttonWidth` field removed from `PluginDescriptors.h` ; no `#include Design*` in `PluginDescriptors*.cpp`
**And** `PluginEditor` uses `WidgetFactory::getRootGuiDimensions()` — no Design header in editor
**And** `grep PluginDesignDimensions` / `grep DesignAtoms` in panels and widgets returns zero hits

**Out of scope:** zone layout sign-off (U-2…U-9), TestComponent (U-1).

---

## Story U-1: TestComponent Enrichment (D-064)

As a developer,
I want TestComponent to showcase every custom widget at all UI scale presets,
So that I can validate sizing and paint in isolation before prod panel sign-off (D-064).

**Acceptance Criteria:**

**Given** `JUCE_DEBUG` build and UI Elements toggle in `HeaderPanel`
**When** TestComponent is shown
**Then** pages exist (or tabs) for: `Button`, `Slider`, `ComboBox`, `NumberBox`, `Toggle`, `Label`, `GroupLabel`, `ParameterCell`, `ModuleHeader`, `ModulationBusCell`, `ModulationBusHeader`, `SectionHeader`, `HorizontalSeparator`, `VerticalSeparator`, `ActivityLed`, `PeakIndicator`, `PatchNameDisplay`, `EnvelopeDisplay`, `TrackGeneratorDisplay`, popup menu samples
**And** switching UI Scale preset 50–200 % re-layouts sandbox content using same `ScaledLayout` helpers as prod
**And** release build (`NDEBUG`) compiles **without** TestComponent sources or UI Elements button (D-063 prep — final removal in U-10)
**And** documented in Dev Notes: optional debug-only study >200 % (D-065) — no prod `ScaleLevels` change

**Out of scope:** prod panel geometry (covered U-2…U-9).

---

## Story U-2: Transversal Widgets Scale Audit

As a performer,
I want every custom widget to respect design dimensions and ScaledDrawing at all UI scale presets,
So that controls look crisp and aligned inside any panel (D-013, UX-DR8).

**Acceptance Criteria:**

**Given** Story U-0 dimensions and U-0b Factory injection complete
**When** each widget in `Source/GUI/Widgets/` is reviewed
**Then** `resized()` / `paint()` use `ScaledLayout` + `ScaledDrawing` ; no raw pixel assumptions tied to 100 % only
**And** widgets do not `#include Design*` ; dimensions arrive via ctor-injected `*Dimensions` from Factory
**And** manual UAT passes at 50, 75, 100, 125, 150, 175, 200 % for each widget type (checklist in story file)
**And** popup menus (`MultiColumnPopupMenu`, `ScrollablePopupMenu`) scale item height, padding, and column gaps consistently

**Brownfield files (minimum):** `Button`, `Slider`, `ComboBox`, `NumberBox`, `Toggle`, `Label`, `GroupLabel`, `ParameterCell`, `ModuleHeader`, `ModulationBusCell`, `ModulationBusHeader`, `SectionHeader`, separators, `ActivityLed`, `PeakIndicator`, `PatchNameDisplay`, `EnvelopeDisplay`, `TrackGeneratorDisplay`.

**Out of scope:** panel-level placement (U-6…U-9). Paint hairlines (D-013) follow-up: **Story U-2b**.

---

## Story U-2b: D-013 Paint Hairlines Compliance

As a performer,
I want every hairline and decorative stroke in custom widgets to stay crisp and centred at all UI scale presets,
So that separators and header rules match D-013 (1 physical px @ 50–150 %, 2 px @ 200 % on Retina).

**Acceptance Criteria:**

**Given** Story U-2 layout/bounds scaling complete
**When** each widget `paint()` path using 1 px design strokes is reviewed
**Then** hairlines use `ScaledDrawing::snappedStrokeThicknessFromDesign` (not manual `* uiScale_`)
**And** HorizontalSeparator and SectionHeader migrated; display curve strokes aligned
**And** manual UAT hairline checklist passes @ 50–200 %

**Brownfield priority:** `HorizontalSeparator`, `SectionHeader`, `EnvelopeDisplay`, `TrackGeneratorDisplay`.

**Out of scope:** panel placement (U-5…U-9).

---

## Story U-3: Header Right Cluster Layout & Sign-Off

As a performer,
I want the header SCALE/SKIN/UI Elements cluster and overall bar alignment verified against Figma at every scale,
So that the bar matches the mockup without re-opening Epic 2 routing work (FR-43 geometry).

**Acceptance Criteria:**

**Given** Stories 2.11 (left routing cluster **frozen — regression only**) and U-0 dimensions
**When** header is audited at all UI scale presets
**Then** right cluster (UI Scale combo, Skin combo, UI Elements button) uses `kGap_` / `2×kGap_` semantics consistent with 2.11
**And** control sizes match Figma ÷4 (combo widths, header height from injected `PanelDimensions` / Design `GUI::kHeaderHeight`)
**And** vertical centring of 20 px controls and 12 px LEDs in 32 px header verified at each preset
**And** manual UAT checklist signed for header **full width** (left + right)
**And** layout **slots** documented for future FR-41 logo popup (7.8) — no popup implementation in this story

**Explicitly out of scope:** MIDI/audio routing wiring (2.9b), LED semantics (2.8), logo popup behaviour (7.8), scale **persistence** (7.8).

---

## Story U-4: Footer Panel Layout Audit

As a user,
I want footer zones, typography, and padding aligned to Figma at every UI scale,
So that messages and device identity read clearly (FR-53 layout only).

**Acceptance Criteria:**

**Given** U-0 footer dimensions and `FooterPanel` brownfield layout
**When** footer is resized at each UI scale preset
**Then** left message zone, centre actions (if present), and right device identity zone use scaled padding/gaps from injected panel dimensions (Design `DesignPanels.h` values @ ctor)
**And** text baselines and truncation behaviour verified at 50–200 %
**And** no `AffineTransform` on footer subtree
**And** manual UAT checklist completed

**Out of scope:** `uiMessageText` routing, ExceptionPropagator, device inquiry strings (7.8 / Epic 8).

---

## Story U-5: Body Shell — Padding, Separators & Column Gaps

As a sound designer,
I want the three body columns and vertical separators positioned per Figma at every scale,
So that PATCH EDIT, shared column, and MASTER EDIT align as one grid (UX-DR8).

**Acceptance Criteria:**

**Given** U-0 body shell dimensions (`DesignPanels.h` — padding, `SharedColumn::kInterPanelGap`, separator widths)
**When** `BodyPanel` and `MainComponent` lay out at each preset
**Then** column widths match Design GUI inner-width identity at 100 % and scale proportionally
**And** layout uses `removeFrom*` / `ScaledLayout` helpers — **no** originX float workaround (brownfield debt removed)
**And** `VerticalSeparator` height tracks body effective height via scaled helpers
**And** `SharedPanel` stack gap (`MatrixModulation` ↔ `PatchManager`) matches design
**And** manual UAT at 50–200 % with no 1 px column drift at 150 % / 175 % (common failure modes)

**Files:** `MainComponent.cpp`, `BodyPanel.cpp`, `SharedPanel.cpp`, `VerticalSeparator.cpp`.

---

## Story U-6: Patch Edit Panels Layout Audit

As a sound designer,
I want PATCH EDIT top modules, displays row, and bottom modules pixel-aligned to Figma,
So that the largest screen area matches the approved mockup (brownfield §4.4).

**Acceptance Criteria:**

**Given** U-0 Patch Edit dimensions and U-2 widget audit complete
**When** `PatchEditPanel`, `PatchEditTopModulesPanel`, `PatchEditDisplaysPanel`, `PatchEditBottomModulesPanel` resize at each preset
**Then** 5-column module grid, `kInterModuleGap`, patch name column padding, and display band heights match `DesignPanels.h` Patch Edit section
**And** `distributeFixedDesignRowsWithRemainderOnLast` / module row helpers used consistently in `BaseModulePanel` descendants
**And** envelope/track/patch-name vertical centring in middle row verified at all presets
**And** manual UAT checklist per sub-panel (top / middle / bottom)

**Out of scope:** interactive drag geometry (Epic 10), APVTS sync, module I/C/P handlers.

---

## Story U-7: Matrix Modulation Panel Layout Audit

As a sound designer,
I want the ten modulation bus rows and section header aligned to Figma,
So that Matrix Mod reads as a tight, scalable grid (brownfield §4.5).

**Acceptance Criteria:**

**Given** U-0 Matrix Mod dimensions
**When** `MatrixModulationPanel` and bus cells resize at each preset
**Then** row height, ModulationBusRow width identity (268 px design), inter-control gap (4 px), and init column align with Design recipes
**And** section header + bus header heights match design stack
**And** manual UAT 50–200 % on full 10-row panel

**Out of scope:** bus reorder UX (7.9 done / SysEx 2.10), init SysEx (3.3). Drag highlight colours follow-up: **Story U-7b**.

---

## Story U-7b: Matrix Mod Reorder Drag Colours

As a sound designer,
I want the Matrix Mod bus reorder drag to show a gray placeholder at the source row and a red-tinted highlight on the hovered drop-target row,
So that origin vs destination are visually distinct during drag without aggressive full-saturation red (FR-50 / UX-DR5 refinement).

**Acceptance Criteria:**

**Given** Story 7.9 reorder drag UX complete
**When** user drags a bus number label
**Then** source row keeps subtle gray placeholder overlay; hovered drop-target row uses `ColourChart::kRed` with tunable alpha (`constexpr` in `ModulationBusCell.cpp`)
**And** panel drag state machine and SysEx path unchanged
**And** manual UAT @ 50 / 100 / 200 %

**Out of scope:** row layout (U-7), floating drag ghost, skin tokens, Core/SysEx.

---

## Story U-8: Patch Manager Panels Layout Audit

As a sound designer,
I want Bank Utility, Internal Patches, Computer Patches, and Patch Mutator modules aligned to Figma,
So that the Patch Manager column matches mockup density (brownfield §4.7).

**Acceptance Criteria:**

**Given** U-0 Patch Manager module dimensions (84 px / 100+16 px stacks)
**When** `PatchManagerPanel` and four module panels resize at each preset
**Then** module heights sum to Patch Manager section design height at 100 % ; scaled heights use integer distribution without cumulative gap error
**And** controls use widget sizes from U-2 + Factory-injected dimensions (IDs harmonized in U-IDs)
**And** manual UAT per module × 7 presets

**Files:** `PatchManagerPanel`, `BankUtilityPanel`, `InternalPatchesPanel`, `ComputerPatchesPanel`, `PatchMutatorPanel`.

**Out of scope:** ActionDispatcher, folder scan, mutation logic (Epics 4–7).

---

## Story U-9: Master Edit Panel Layout Audit

As a sound designer,
I want MIDI, Vibrato, and Misc master modules stacked per Figma,
So that MASTER EDIT column height matches PATCH EDIT column (brownfield §4.6).

**Acceptance Criteria:**

**Given** U-0 Master Edit dimensions and `static_assert` height parity with Patch Edit
**When** `MasterEditPanel` resizes at each preset
**Then** three module blocks + inter-module gap match Design Master Edit section stack
**And** parameter rows align with `BaseModulePanel` patterns validated in U-6
**And** manual UAT 50–200 %; M-6 grayed section height unchanged (layout only — gating is Epic 8)

**Out of scope:** master SysEx, init confirmation (3.4).

---

## Story U-10: Release Gate — Prod Audit Sign-Off & D-062/D-063

As a product owner,
I want a documented sign-off that prod UI is pixel-validated at 50–200 % and the sandbox is release-safe,
So that v1 ships without debug UI clutter (D-062, D-063).

**Acceptance Criteria:**

**Given** Stories U-2 through U-9 done with signed manual UAT sheets
**When** Release configuration is built (`NDEBUG`)
**Then** `TestComponent`, `TestButtons`, `TestSliders` (and related) are excluded via `#if JUCE_DEBUG` or CMake source list — **no UI Elements button** in release
**And** Debug configuration retains enriched TestComponent for post-release regression (D-063)
**And** consolidated audit report lists: zones passed, known acceptable deltas (if any), owner sign-off date
**And** `project-context.md` § Dev test harness updated to reflect release vs debug policy
**And** D-062 criterion met: **prod panels + widgets** are the visual reference, not sandbox

**Out of scope:** expanding prod UI scale beyond 200 % (D-065).

---

## Manual UAT template (per story U-2…U-9)

Reusable checklist — copy into each story implementation artifact:

| UI Scale | 50 % | 75 % | 100 % | 125 % | 150 % | 175 % | 200 % |
|---|---|---|---|---|---|---|---|
| No clipped controls | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Hairlines crisp (D-013) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Gaps match Figma (±0 @ 100 %) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Text baseline / truncation OK | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |
| Screenshot captured | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |

**Tester:** Guillaume · **Build:** Standalone Debug unless story notes otherwise · **Skin:** Black + Cream spot-check on 100 %.

---

## FR / decision traceability

| Ref | Epic U coverage |
|---|---|
| FR-43 | Geometry + validation presets (wiring/persistence → 7.8) |
| UX-DR8 | All stories |
| D-013 | U-2, U-10 |
| D-062 | U-10 |
| D-063 | U-1, U-10 |
| D-064 | U-1 |
| D-065 | U-1 Dev Notes only |
| Brief ÷4 grid | U-0, all layout stories |
| Brainstorming SSOT / Factory / IDs | U-IDs, U-0, U-0b |

---

## Sprint placement

- **Status initial :** `backlog` — démarrage par **U-IDs**, puis **U-0** / **U-0b** ; U-1 en parallèle possible.
- **Ne remplace pas** la séquence D-058 ; s’insère en **voie parallèle GUI**.
- **Bloque** critère release v1 UI craftsmanship (brief différentiateur).

---

## Dimension tables post U-0

Per-zone reference tables (design px @ 100 %, `TSS::Design` symbol paths, UAT story mapping): [`implementation-artifacts/u-0-zone-dimension-tables.md`](../implementation-artifacts/u-0-zone-dimension-tables.md).

---

*Epic U v1.1 — 2026-06-07. Brainstorming decisions integrated. Dimension tables per zone after U-0. ID inventory: `u-ids-widget-id-harmonization-inventory.md`.*
