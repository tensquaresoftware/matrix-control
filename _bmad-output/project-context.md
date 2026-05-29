---
organization: Ten Square Software
project: Matrix-Control
title: Project Context
author: BMad Agent
version: "1.0"
sources:
  - planning-artifacts/briefs/brief-Matrix-Control-2026-05-22/brief.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - reference-docs/oberheim/index.md
created: 2026-05-23
updated: 2026-05-29
---

# Project Context

**Purpose:** Implementation constitution for BMad agents working on this repository.  
**Baseline code tag:** `v0.0.66-alpha-pre-bmad`  
**Last updated:** 2026-05-29

---

## BMad Document Metadata

Every Markdown artifact under `_bmad-output/` MUST open with YAML frontmatter in this order:

1. `organization` — `Ten Square Software`
2. `project` — `Matrix-Control`
3. `title` — document type only; do **not** repeat the project name (already in `project`). Same rule for the H1 heading.
4. `author` — `BMad Agent` for agent-authored artifacts; owner name for user source inputs (e.g. vision inputs)
5. **Remaining fields** (omit when not applicable), in this order:
   - `status` — workflow state (`draft`, `ready`, `final`, or a descriptive phase label for decision logs)
   - `version` — see **Versioning tiers** below; omit when not applicable
   - `sources` — relative paths to upstream or companion documents (BMad traceability convention)
   - other domain-specific keys (e.g. `language`, `baseline_tag`, `target_release`)
   - `created` — ISO date `YYYY-MM-DD`
   - `updated` — ISO date `YYYY-MM-DD` (always last when present)

### Versioning tiers

Document `version` labels a **content milestone** for humans and downstream workflows. Git remains the technical source of truth — bump `version` only on **significant** contractual changes, not on every edit.

| Tier | Documents | Rule |
|---|---|---|
| **Deliverables** | `brief.md`, `prd.md`, future UX / architecture / SPEC | Own semver: `0.x` while drafting, `1.0` when finalized (`status: final`), then increment on major revisions |
| **Companion** | `addendum.md` (PRD or brief) | Same `version` as the parent deliverable |
| **Living constitution** | `project-context.md` | Own semver; bump major on breaking convention changes |
| **Process / audit** | `.decision-log.md`, kickoff prompts, inventories, vision inputs | **No** `version` — use `created` / `updated` and chronological entries |

When finalizing a PRD (`bmad-prd` Finalize), set `version: "1.0"` alongside `status: final`.

---

## Project Summary

Matrix-Control is a cross-platform **JUCE 8** VST3/AU/Standalone MIDI editor for the **Oberheim Matrix-1000**. The project has two goals:

1. **Product:** Professional UX/UI plugin that fully exploits the Matrix-1000 via SysEx.
2. **Reference repo:** Exemplary open-source JUCE plugin — Clean Code, Clean Architecture, SOLID, tests, and thorough public documentation.

This is a **brownfield** project. Do **not** restart from scratch. Extend and refine existing architecture.

---

## Technology Stack & Versions

| Layer | Choice |
|---|---|
| Language | C++17 minimum |
| Framework | JUCE 8.0.12 (`/Applications/JUCE`) |
| Build | CMake, outputs under `builds/` (`macos/`, `windows/`, `linux/`) — gitignored; never `build/` at root alone |
| Compiler (macOS) | Xcode 26 |
| Plugin formats | AU, VST3, Standalone |
| Target synth | Oberheim Matrix-1000 (SysEx) |
| IDE / AI | Cursor with BMad Method (BMM) |

---

## Repository Layout

**Current layout (baseline `v0.0.66-alpha-pre-bmad`):**

```
Matrix-Control/
├── Source/                  # Application code (JUCE convention — keep capital S)
│   ├── Core/                # Business logic, MIDI, APVTS factories — NO GUI deps
│   ├── GUI/                 # Presentation: widgets, panels, looks, skins, layout
│   └── Shared/Definitions/  # Descriptors, IDs, display names, design dimensions
├── Assets/                  # Public assets (fonts, README screenshots)
├── Documentation/           # Public project knowledge (specs, architecture, GUI guides)
├── Documentation/Development/Plans/  # Legacy pre-BMad plans — archive, do not delete
├── _bmad-output/            # BMad active artifacts (PRD, epics, stories, this file)
├── Quality/                 # Private working files (mockups, journal) — not public docs
├── Workbench/               # Dev test data (SysEx, Ableton Live projects)
└── Builds/                  # CMake build trees (gitignored)
```

### Planned migration (P-001 — see PRD decision log)

Guillaume plans to **realign the project tree** with professional JUCE open-source conventions (folder/file naming, public vs private artifacts). This is **not started yet**.

**Target root layout (validated draft):**

| Path | Role | Git |
|---|---|---|
| `src/` | Application code | versioned |
| `assets/` | Public assets (fonts, README screenshots) | versioned |
| `docs/` | Public documentation (`.md`, kebab-case filenames) | versioned |
| `tests/` | Unit tests | versioned |
| `builds/macos/`, `builds/windows/`, `builds/linux/` | CMake output trees | **gitignored** |
| `_bmad/`, `_bmad-output/` | BMad tooling and artifacts | versioned |
| `_bmad-output/reference-docs/oberheim/` | Archived Oberheim MIDI/SysEx references (M-1000 official transcription + M-6/6R complement) | versioned |
| `_local/` | Personal workspace (ex-`Quality/`, `Workbench/`) | **gitignored** |

References: no Oberheim manual PDFs in git — Markdown + external links only.

**Until the target layout is documented and approved:**

- Do **not** reorganize `Source/`, `Documentation/`, or `Quality/` ad hoc during feature work.
- Do **not** commit `_local/` contents (journal, mockups, workbench data).
- During **`bmad-create-architecture`**, capture the **full directory map** and migration steps; update this section when frozen.

**Hard rule until P-001 is executed:** do not rename or flatten `Source/` → `src/` without an explicit approved migration plan and CMake updates.

---

## Architecture Invariants (Non-Negotiable)

### Dependency direction

```
GUI → Core
GUI → Shared/Definitions
Core → Shared/Definitions
Core ↛ GUI   (strict — Core must never include GUI headers)
```

### Single sources of truth

| Concern | Location | Rule |
|---|---|---|
| Synth hierarchy (Master/Patch, modules, parameters, SysEx) | `Source/Shared/Definitions/PluginDescriptors*.cpp/.h` | **Never** duplicate descriptor data elsewhere |
| Parameter/group string IDs | `Source/Shared/Definitions/PluginIDs.h` | APVTS identifiers only here |
| UI display strings | `Source/Shared/Definitions/PluginDisplayNames.h` | All user-visible labels here |
| GUI design dimensions (100% reference) | `Source/Shared/Definitions/PluginDesignDimensions.h` | Design-time sizes; runtime scaling via helpers |
| APVTS + widget construction | `Source/GUI/Factories/WidgetFactory.cpp/.h` | Factory builds UI and APVTS in parallel from descriptors |

### WidgetFactory pattern

- Descriptors drive **both** APVTS structure and GUI widget creation.
- Do not hand-wire individual parameters in panels when a descriptor exists.
- New modules/parameters: update descriptors first, then factory/panels.

### Threading (JUCE audio plugin rules)

- `processBlock()` — no blocking on SysEx; may forward notes/CC to outbound queue only.
- APVTS listeners may fire on audio thread — never call `repaint()` directly from them.
- GUI updates: `MessageManager::callAsync()`, atomics, or lock-free FIFO patterns.
- Dedicated MIDI thread for editor SysEx send/receive and **MIDI From** listening.

### MIDI architecture (v1 — dual role Instrument + Editor)

Matrix-Control is a **virtual instrument** (MIDI in + audio out), not a MIDI effect only.

| Role | Inbound | Outbound | Thread |
|---|---|---|---|
| **Instrument** | Notes, CC, Pitch Bend — from host `processBlock()` (plugin) or **Keyboard From** (standalone) | → **MIDI To** | Audio thread forwards; unified send queue |
| **Editor** | SysEx only ← **MIDI From** | SysEx, Program Change (patch select) → **MIDI To** | **Dedicated MIDI thread** — never audio thread |

**Filters:** Instrument path strips Program Change and SysEx. Editor **MIDI From** accepts SysEx only (no inbound PC).

**Header controls:** MIDI From, MIDI To, Keyboard From (grayed **Host** in plugin), Audio From, Input Gain + meter, LEDs **Instrument** / **Editor**.

**Multi-instance:** one port triple per Matrix-1000; distinct ports recommended in user manual.

**Queue:** Unified outbound to MIDI To — notes/CC prioritized; SysEx rate-limited (reliability #1).

Do not route editor SysEx through `processBlock()` midiBuffer.

---

## GUI & UI Scale Rules

### UI Scale vs Display Scale

- **UI Scale:** Project business scale (50%, 75%, 100%, 125%, 150%, 175%, 200%) — user preset via header combo.
- **Display Scale:** OS/screen DPI factor (Retina, Windows scaling).
- Helpers: `Source/GUI/Layout/ScaledLayout.h`, `Source/GUI/Layout/ScaledDrawing.h`.

### Scaling strategy (mandatory)

- Design reference at **100%**; all layout dimensions derive from design values × UI Scale.
- **No global `AffineTransform`** on the component tree for user resize — causes blur and clipping.
- Recalculate layout in `resized()` with a **single rounding policy** (`ScaledLayout::scaledInt`, distribute helpers).
- Design dimensions must be **divisible by 4** so integer scales land cleanly.
- Separate **layout geometry** (bounds, margins) from **stroke thickness** (borders, separators) — see `Documentation/Development/GUI/GUI-Scaling-Strategy.md` and `Documentation/Development/Plans/2026/04/2026-04-20-Widget-Drawing-UiScale-DisplayScale-Snapping-Reference.md`.

### Widget conventions

- Custom widgets live in `Source/GUI/Widgets/` (e.g. `Button`, `Slider`, `ParameterCell`, `ModulationBusCell`).
- Looks/skins: `Source/GUI/Looks/`, `Source/GUI/Skins/` — widgets receive looks, not raw colours.
- Typography: `Source/GUI/Looks/TypographyStyles.h`.
- Panels compose widgets; panels do not embed drawing logic that belongs in widgets.

### Dev test harness

- `Source/GUI/Tests/TestComponent` — temporary widget visual testing at multiple scales.
- Toggle via **UI Elements** button in `HeaderPanel` (right side).
- Do not remove until UI scaling is validated across all widget types.

---

## Clean Code Limits (Hard — Uncle Bob)

| Metric | Maximum |
|---|---|
| Function/method | 15 lines (20 for orchestration `show()`-style only) |
| Class | 200 lines |
| Function parameters | 3 (use a struct if more) |
| Cyclomatic complexity | < 5 |
| Indentation nesting | 2 levels max |
| Code duplication | Zero tolerance — extract at 3 similar lines |

If a limit is exceeded, **stop and refactor** before continuing.

---

## C++ & JUCE Coding Standards

### Naming

- Classes: `PascalCase`
- Methods/variables: `lowerCamelCase`
- Private members/constants: `underscoreSuffix_` (e.g. `apvts_`, `kWidth_`)
- Enums: `PascalCase` type, `k` prefix on values (`ParameterType::kInt`)
- Namespaces: `PascalCase` (`Core`, `GUI`, `PluginDescriptors`, `tss`)

### Style

- **Allman braces**, 4 spaces, no tabs.
- `#pragma once` on all headers.
- Includes: system → JUCE → project (relative from root, never `../../../`).
- **English only** in code, comments, commit messages, GitHub issues.
- **No magic numbers** — named `constexpr` constants.
- **`nullptr` only** — never `NULL` or `0` for pointers.
- Prefer `enum class`, smart pointers (`unique_ptr` default), RAII, early returns.
- Mark overrides with `override`; use `= delete` / `= default` explicitly.

### JUCE specifics

- Processor: `PluginProcessor`, Editor: `PluginEditor`.
- Avoid deprecated JUCE APIs (see `.cursorrules` section 8.2).
- No allocations in `paint()` or `processBlock()`.
- Pass JUCE value types (`String`, `Colour`, `Rectangle`, etc.) by value.

---

## Module Map (Current GUI Hierarchy)

```
PluginEditor
└── MainComponent
    ├── HeaderPanel      (skin, UI scale, UI Elements toggle)
    ├── BodyPanel
    │   ├── PatchEditPanel (top modules, displays, bottom modules)
    │   ├── SharedPanel (Matrix Modulation + Patch Manager)
    │   └── MasterEditPanel
    └── FooterPanel
```

Legacy panel names (`TopPanel`, `BottomPanel`, `MiddlePanel`) were renamed — use current paths under `PatchEditTopModulesPanel/`, `PatchEditDisplaysPanel/`, `PatchEditBottomModulesPanel/`, `SharedPanel/`.

---

## Documentation Map

| Document | Path | Role |
|---|---|---|
| Functional spec | `Documentation/Development/Specifications/Functional-Specification.md` | Product requirements input |
| Technical spec | `Documentation/Development/Specifications/Technical-Specification.md` | Architecture & threading input |
| Class inventory | `Documentation/Development/Architecture/Classes.md` | Planned Core classes |
| GUI scaling | `Documentation/Development/GUI/GUI-Scaling-Strategy.md` | Scaling reference |
| Oberheim SysEx | `Documentation/References/MD/Oberheim-Matrix-1000-MIDI-SysEx-Implementation.md` | Protocol reference |
| Legacy plans | `Documentation/Development/Plans/` | Pre-BMad archive — input for epics, not active sprint docs |

BMad planning artifacts live in `_bmad-output/planning-artifacts/`. Promote validated decisions back to `Documentation/` when stable.

---

## Testing Strategy (Target State)

- **Unit tests:** Core business classes (SysEx parser, patch model, parameter mapping) — not GUI components.
- **Framework:** Catch2 or Google Test (to be confirmed in architecture epic).
- **No tests in audio thread or paint paths.**
- GUI validation: manual via `TestComponent` harness until visual regression tooling exists.

---

## Git & Release Conventions

- Commits / PRs / issues: **English**, imperative summary + bullet list for significant changes.
- Tags: annotated, format `v0.0.xx-alpha[-suffix]`, e.g. `v0.0.66-alpha-pre-bmad`.
- **Do not commit** unless explicitly requested by the project owner.
- Do not force-push `main`.

---

## What Agents Must NOT Do

- Rewrite or scatter `PluginDescriptors` data.
- Introduce GUI dependencies into `Source/Core/`.
- Use global AffineTransform scaling for the main UI.
- Create a parallel string/ID system outside `PluginIDs` / `PluginDisplayNames`.
- Delete `Documentation/Development/Plans/` archive.
- Reorganize the root folder structure or rename `Source/`.
- Add French text in source code or public documentation.
- Skip refactors when Clean Code limits are exceeded.

---

## Current Priority (Post Pre-BMad Tag)

## Current Priority (Post Brief v0.3)

1. **PRD Update** from finalized product brief v0.3 (reconcile legacy Functional Spec)
2. **Architecture** workflow — dual-role MIDI, virtual instrument bus layout
3. Complete **UI Scale** refactor across remaining widgets
4. Create epics/stories; Patch Mutator brainstorming before mutator epic
5. Introduce Core unit tests incrementally

---

## Key Reference Files (Read Before Implementing)

- `.cursorrules` — full coding standards (authoritative for style disputes)
- `Source/Shared/Definitions/PluginDescriptors.h`
- `Source/GUI/Factories/WidgetFactory.h`
- `Source/GUI/Layout/ScaledLayout.h`
- `Source/GUI/PluginEditor.cpp` — UI Scale + test view toggle wiring
- `CMakeLists.txt` — source file registration
