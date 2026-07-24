---
organization: Ten Square Software
project: Matrix-Control
title: Story U-10 — Release Gate — Prod Audit Sign-Off & D-062/D-063
author: BMad Agent
status: done
baseline_commit: a1398de
sources:
  - planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md (D-062, D-063, D-013, D-065)
  - implementation-artifacts/u-0-zone-dimension-tables.md
  - implementation-artifacts/u-1-testcomponent-enrichment-d-064.md
  - implementation-artifacts/u-2-transversal-widgets-scale-audit.md
  - implementation-artifacts/u-3-header-right-cluster-layout-and-sign-off.md
  - implementation-artifacts/u-4-footer-panel-layout-audit.md
  - implementation-artifacts/u-5-body-shell-padding-separators-and-column-gaps.md
  - implementation-artifacts/u-6-patch-edit-panels-layout-audit.md
  - implementation-artifacts/u-7-matrix-modulation-panel-layout-audit.md
  - implementation-artifacts/u-8-patch-manager-panels-layout-audit.md
  - implementation-artifacts/u-9-master-edit-panel-layout-audit.md
  - implementation-artifacts/deferred-work.md
  - implementation-artifacts/11-2-cd-release-pipeline.md
  - _bmad-output/project-context.md
  - CMakeLists.txt
  - CONTRIBUTING.md
created: 2026-07-24
updated: 2026-07-24
---

# Story U.10: Release Gate — Prod Audit Sign-Off & D-062/D-063

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

<!--
Sprint key: u-10-release-gate-prod-audit-sign-off-and-d-062-d-063
Prerequisites: U-2…U-9 done (owner-narrowed smoke + residual → here); U-1 sandbox enrichment done; Epic 11 CD pipeline ready but blocked on this gate for public v1.
This is the FINAL story of Epic U. Closing it unlocks epic-u → done (manual) and clears the CONTRIBUTING.md / 11-2 release-hygiene blocker.
Next: optional epic-u-retrospective; first public tag hygiene verification.
-->

## Story

As a product owner,
I want a documented sign-off that prod UI is pixel-validated at 50–200 % and the sandbox is release-safe,
so that v1 ships without debug UI clutter (D-062, D-063).

## Problem Statement / Errata

### What this story owns

Two coupled deliverables — both required for D-062/D-063:

1. **Aggregate residual UAT + consolidated audit report** — run (or formally accept) the full 7-preset visual matrix that U-3…U-9 deferred via owner-narrowed smoke; record zones passed, known acceptable deltas, owner sign-off date.
2. **Release sandbox exclusion (D-063)** — `TestComponent` and all `Source/GUI/Tests/Test*.cpp` leave Release binaries; Debug keeps the enriched harness for post-release visual regression. Access gesture must be absent / no-op in Release.

### Epic / brownfield errata (map before coding)

| Epic / older wording | Live reality | Action |
|----------------------|--------------|--------|
| “no **UI Elements button** in release” | There is **no** visible menu item or button. Toggle is a **hidden Shift+Ctrl+click on the logo** (`Logo::mouseUp` → `HeaderPanel::onUiTestsToggleRequested` → `PluginEditor::setUiElementsTestVisible`) | Treat “no UI Elements access in Release” as the AC intent — guard the gesture + editor wiring, not invent a button to remove |
| “Given U-2…U-9 signed manual UAT sheets” | Most zone stories closed on **owner-narrowed 100 % smoke**; full 7-preset matrix was explicitly deferred **to U-10** | U-10 **is** the formal zone sheet — do not treat prior smoke as already satisfying the aggregate matrix |
| U-1 “no requirement to exclude Test sources” | Intentional — CMake still lists all ~24 `Test*.cpp` unconditionally in `PLUGIN_SOURCES` | U-10 owns 100 % of exclusion |
| `project-context.md` § Dev test harness | Still says “Do not remove until UI scaling is validated…” | Must rewrite to Debug-only policy after exclusion |
| D-065 (>200 % scale) | Debug-only study; epic Out of scope for U-10 | Do **not** expand prod presets |

### Critical brownfield debt — sandbox always in every config

Today (baseline `a1398de`):

- `CMakeLists.txt` lines ~226–249 list every `Source/GUI/Tests/Test*.cpp` inside `PLUGIN_SOURCES` with **no** Debug/Release gate.
- `PluginEditor.h` unconditionally `#include "Tests/TestComponent.h"` and owns `testComponent_`.
- `Logo.cpp` Shift+Ctrl gesture always available.
- Zero `#if JUCE_DEBUG` / `NDEBUG` guards on this path.

**Implication:** building `macos-release-arm64` (or any Release preset) still ships the full sandbox. D-063 is unimplemented.

### Aggregate residual inventory (must appear in audit report)

These are the deferred-work / story notes U-10 owns as **UAT scrutiny or accepted deltas** — not a mandate to rewrite layout unless a preset shows a **blocking** defect (clip, overlap, unusable control):

| Zone / source | Residual class |
|---------------|----------------|
| U-5 Body / Shared | Column-chain leftover unchecked; Shared children sized to design width while parent uses `removeFromTop` |
| U-6 Patch Edit | No runtime leftover assert on 5-column / vertical stack; DesignChecks @ 100 % only |
| U-7 Matrix Mod | Fixed-width strips; non-preset scales can leave ±1–2 px Init vs Paste X drift |
| U-8 Patch Manager | Undersized Internal/Computer strips clamp via `removeFromLeft` (harmless at design width) |
| U-9 Master Edit | Odd-scale strip sum slack/starve; parent module height vs BaseModulePanel interior ±1 px; unused `sectionHeaderWidth` (latent, not UAT-blocking) |
| U-3 / U-4 | Formal sheet counts prior header/footer sign-off; include in consolidated report |
| U-2 / U-2b | Widget + D-013 hairline policy already signed; spot-check hairlines in aggregate pass |

**Policy:** Prefer documenting **acceptable ±1–2 px integer-scale deltas** at odd presets over inventing a new remainder-distribute API (out of scope unless UAT fails usability).

### Scope split vs neighbours

| Concern | Owner |
|---------|-------|
| Per-zone layout algorithms (Body, PE, MM, PM, ME, Header, Footer, widgets) | U-2…U-9 (**done** — reopen only if aggregate UAT finds a blocking defect) |
| Aggregate 7-preset UAT + consolidated sign-off report | **U-10** |
| TestComponent enrichment / pages | U-1 (**done**) |
| Release exclusion of sandbox + Debug retain | **U-10** (D-063) |
| D-062 “prod panels + widgets = visual reference” | **U-10** (met when audit report + owner sign-off land) |
| UI Scale persistence / wiring | Story 7.8 (**not** this story) |
| CD codesign / notarization / tags | Epic 11-2 (**done**) — only verify Release binary hygiene here |
| Prod scale > 200 % | D-065 — **out of scope** |

## Acceptance Criteria

### AC 1 — Aggregate residual UAT & consolidated audit report (D-062)

**Given** Stories U-2 through U-9 are done (smoke and/or formal sheets as recorded in each story)  
**When** the product owner (Guillaume) completes the formal aggregate zone UAT  
**Then**

1. A **consolidated audit report** lives in this story file (section **Consolidated Audit Report** below) listing:
   - each zone (Header, Footer, Body shell, Patch Edit, Matrix Modulation, Patch Manager, Master Edit, transversal widgets / hairlines);
   - result per UI Scale preset **50 / 75 / 100 / 125 / 150 / 175 / 200 %** (pass / fail / acceptable delta);
   - known acceptable deltas (if any) with short rationale;
   - **owner sign-off date** and tester name.
2. Blocking defects (clipped controls, unusable hit targets, broken column alignment at a v1 preset) are **fixed in the owning panel** with Completion Notes — or the story stays open. Cosmetic ±1–2 px integer leftovers at odd presets may be recorded as acceptable deltas (U-5…U-9 policy).
3. D-062 is met: **prod panels + widgets** are the signed visual reference; the sandbox is not used as the release visual authority.
4. Optional screenshots preferred @ 100 + 150 + 175 % (Black skin; Cream spot-check @ 100 %).

### AC 2 — Release excludes sandbox; Debug retains it (D-063)

**Given** a Release configuration build (`NDEBUG` / `CMAKE_BUILD_TYPE=Release` / Release preset)  
**When** the plugin / Standalone target is compiled and linked  
**Then**

1. All `Source/GUI/Tests/Test*.cpp` (and related Test headers’ translation units) are **excluded** from the Release binary — via CMake source-list gating **and/or** equivalent that yields zero TestComponent object code in Release.
2. Editor / header wiring cannot construct or show the sandbox in Release:
   - `#include "Tests/TestComponent.h"`, `testComponent_` member, `layoutUiElementsTestComponent`, `setUiElementsTestVisible`, mouse/key sandbox branches — wrapped in `#if JUCE_DEBUG` (or equivalent compile-time exclusion).
   - Logo **Shift+Ctrl** UI-test gesture is compiled out or hard no-op under Release (`#if JUCE_DEBUG` on callback wiring in `Logo` / `HeaderPanel` / `PluginEditor`).
3. **Debug** configuration still builds and runs the enriched TestComponent (U-1 pages intact); Shift+Ctrl logo toggle still works in Debug.
4. Proof in Completion Notes (pick at least one):
   - Release object / binary does not contain `TestComponent` symbol (e.g. `nm` / `strings` / link map check on macOS Release artefact), **and**
   - Debug Standalone still opens UI Elements harness via Shift+Ctrl+logo.

### AC 3 — Docs & policy sync

**Given** AC 1–2 complete  
**When** docs are updated  
**Then**

1. `_bmad-output/project-context.md` § **Dev test harness** reflects Debug-only policy (sandbox retained in Debug; absent from Release; gesture Debug-only). Remove the obsolete “Do not remove until validated” wording.
2. Do **not** weaken `CONTRIBUTING.md` release-blocker language until AC 2 is proven — after proof, the U-10 gate item is satisfied (no need to delete the historical mention unless editing that section anyway).
3. Close or annotate U-10-owned rows in `deferred-work.md` that this aggregate UAT accepts as known deltas (do not silently drop them — mark accepted with date / link to this story’s audit report).

### AC 4 — Preserve contracts & out of scope

1. No global `AffineTransform` on the prod UI tree.
2. Do **not** expand prod UI Scale presets beyond 200 % (D-065).
3. Do **not** delete `Source/GUI/Tests/` from the repo — Debug must keep the harness.
4. Do **not** reopen zone layout algorithms unless aggregate UAT finds a **blocking** defect.
5. Do **not** change APVTS, descriptors, IDs, MIDI, Epic 8 device gating, or CD workflow secrets.
6. Do **not** add automated GUI pixel / screenshot tests (manual UAT only — project policy).
7. `Matrix-Control_Tests` unit-test target must remain green and must **not** gain a hard dependency on GUI Test sandbox sources.

### AC 5 — Build, proof, sprint

1. Debug build green: `cmake --build --preset macos-debug-arm64` (+ `Matrix-Control_Tests`).
2. Release build green: `cmake --build --preset macos-release-arm64` (or equivalent local Release preset) **with** AC 2 exclusion proven.
3. Sprint key `u-10-release-gate-prod-audit-sign-off-and-d-062-d-063` → `review` after implementation + signed audit report.
4. After code review → `done`; then epic-u may be marked `done` manually (all Epic U stories complete).

## Tasks / Subtasks

- [x] **T1 — Baseline & inventory** (AC: #1, #2, #4)
  - [x] Confirm U-2…U-9 all `done` in `sprint-status.yaml`
  - [x] List every `Source/GUI/Tests/Test*.cpp` currently in `PLUGIN_SOURCES` (~24 files)
  - [x] Map all `#include` / members / callbacks for sandbox wiring (`PluginEditor`, `MainComponent`, `HeaderPanel`, `Logo`)
  - [x] Collect residual bullets from `deferred-work.md` + U-5…U-9 UAT notes into the audit report draft tables

- [x] **T2 — CMake Release exclusion** (AC: #2, #5)
  - [x] Move Test sandbox `.cpp` files out of the unconditional `PLUGIN_SOURCES` list into a dedicated list (e.g. `TEST_SANDBOX_SOURCES`)
  - [x] Attach them **only** to Debug configurations. Prefer `$<$<CONFIG:Debug>:…>` generator expressions so multi-config Windows VS presets stay correct; single-config Ninja macOS/Linux presets must also exclude them when `CMAKE_BUILD_TYPE=Release`
  - [x] Verify Release configure+build does not compile any `Source/GUI/Tests/*.cpp`
  - [x] Verify Debug still compiles all Test pages

- [x] **T3 — Compile-time wiring guards** (AC: #2, #4)
  - [x] Guard `PluginEditor` include / members / construct / layout / input routing with `#if JUCE_DEBUG`
  - [x] Guard `Logo` Shift+Ctrl gesture and `HeaderPanel` relay similarly (Release = no toggle path)
  - [x] Guard `MainComponent` UI-elements helpers if they only exist for the overlay (keep prod layout paths intact)
  - [x] Ensure Release build compiles with **zero** references to `TestComponent` symbols

- [x] **T4 — Aggregate UAT + audit report** (AC: #1, #3)
  - [x] Run formal 7-preset matrix across zones (or owner-narrow with explicit remaining gaps — **prefer full matrix**; this story is the last chance before v1 gate)
  - [x] Fill **Consolidated Audit Report** tables below; record acceptable deltas
  - [x] Annotate accepted residuals in `deferred-work.md`
  - [x] Update `project-context.md` § Dev test harness

- [x] **T5 — Proof + green builds** (AC: #2, #5)
  - [x] Debug: Shift+Ctrl+logo still opens harness; smoke one Test page
  - [x] Release: binary hygiene proof (no `TestComponent` in artefact) + Standalone launches without sandbox access
  - [x] Unit tests green
  - [x] Completion Notes: CMake approach chosen, grep/`nm` proof, sign-off date, any layout fixes forced by UAT

### Review Findings

- [x] [Review][Patch] Release Shift+Ctrl on logo must hard no-op before Settings [Source/GUI/Widgets/Logo.cpp:54] — Fixed: Shift+Ctrl always returns; Debug still toggles harness; Release no longer opens Settings.
- [x] [Review][Defer] No CI/Release-workflow regression check for TestComponent re-entry [CMakeLists.txt / CD] — deferred, hardening beyond AC2 local nm/strings proof
- [x] [Review][Defer] No compile-time sync between JUCE_DEBUG and CONFIG:Debug sandbox sources [CMakeLists.txt:587-589] — deferred, theoretical mismatch; local Debug+Release builds proven
- [x] [Review][Defer] CONVENTIONS.md E2E/GUI row still cites TestComponent without Debug-only qualifier [CONVENTIONS.md:~876] — deferred, AC3 only required project-context.md

## Consolidated Audit Report

**Tester:** Guillaume · **Build:** Standalone Debug for visual UAT; Release used for binary hygiene only · **Skin:** Black; Cream spot-check @ 100 %  
**Owner sign-off date:** 2026-07-24  
**D-062 met:** ☑ yes (prod panels + widgets = visual reference)

### Zone results (fill during T4)

| Zone | 50% | 75% | 100% | 125% | 150% | 175% | 200% | Notes / prior story |
|------|-----|------|------|------|------|------|------|---------------------|
| Widgets / hairlines (U-2 / U-2b) | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | Prior signed policy; aggregate OK |
| Header (U-3) | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | Prior sign-off + aggregate OK |
| Footer (U-4) | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | Prior sign-off + aggregate OK |
| Body shell (U-5) | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | Known class Δ-U5 (acceptable) |
| Patch Edit (U-6) | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | Known class Δ-U6 (acceptable) |
| Matrix Modulation (U-7) | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | Known class Δ-U7 (acceptable) |
| Patch Manager (U-8) | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | Known class Δ-U8 (acceptable) |
| Master Edit (U-9) | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | ☑ | Known class Δ-U9 (acceptable) |

**Per-cell legend:** `☑` pass · `Δ` acceptable delta (document below) · `✗` blocking (must fix) · `☐` not run

### Known acceptable deltas

| ID | Zone / preset | Delta | Why acceptable |
|----|---------------|-------|----------------|
| Δ-U5a | Body shell / odd presets | Column-chain leftover unchecked after `removeFromLeft` | DesignChecks @ 100 %; ±1–2 px cosmetic; no usability failure |
| Δ-U5b | Shared / design width | Children sized to design width while parent uses `removeFromTop` | Harmless while Shared matches design width |
| Δ-U6 | Patch Edit / odd presets | No runtime leftover assert on 5-col / vertical stack | DesignChecks @ 100 %; policy prefers document over remainder API |
| Δ-U7 | Matrix Mod / non-preset scales | ±1–2 px Init vs Paste X drift possible | Seven v1 presets OK in aggregate UAT |
| Δ-U8 | Patch Manager / undersized width | `removeFromLeft` clamps later strips | Harmless at design width |
| Δ-U9a | Master Edit / odd presets | Strip sum slack/starve ±1–2 px | Fixed scaled strips policy |
| Δ-U9b | Master Edit modules / non-100% | Parent module height vs interior sum ±1 px | Latent; not blocking |
| Δ-U9c | Master Edit | Unused `sectionHeaderWidth` vs full-strip header | Latent coupling; not UAT-blocking |

### Release binary hygiene

| Check | Result |
|-------|--------|
| Release preset build green | ☑ `macos-release-arm64` |
| No `TestComponent` (and related Test*) symbols / TU in Release artefact | ☑ `nm` + `strings` empty; no `Source/GUI/Tests` objects in Release build tree |
| Debug harness still toggles via Shift+Ctrl+logo | ☑ owner smoke 2026-07-24; Debug-only verified |

## Dev Notes

### Depends on

| Dependency | Status | Note |
|------------|--------|------|
| U-1 | done | Enriched sandbox — keep in Debug |
| U-2…U-9 | done | Layout audits; residual UAT deferred here |
| Epic 11-2 | done | CD ready; blocked on this hygiene gate for public v1 |
| Story 7.8 | separate | UI Scale persistence — out of scope |

### Recommended order

1. Inventory + draft audit tables (T1)  
2. CMake split + `#if JUCE_DEBUG` wiring (T2–T3) — prove Debug+Release builds  
3. Aggregate UAT / sign-off (T4) — may force tiny layout patches  
4. Docs + deferred-work annotations + proofs (T5)

Do **not** start by deleting Test files from disk.

### Key files (Code Map)

| Path | Change |
|------|--------|
| `CMakeLists.txt` | **Primary:** gate `Source/GUI/Tests/Test*.cpp` to Debug only |
| `Source/GUI/PluginEditor.h` / `.cpp` | `#if JUCE_DEBUG` around TestComponent include, member, construct, layout, input |
| `Source/GUI/Widgets/Logo.h` / `.cpp` | Guard Shift+Ctrl UI-test gesture / `onUiTestsToggleRequested` |
| `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h` / `.cpp` | Guard relay callback |
| `Source/GUI/MainComponent.h` / `.cpp` | Guard UI-elements overlay helpers if Release-unused |
| `_bmad-output/project-context.md` | Rewrite § Dev test harness (Debug-only policy) |
| `deferred-work.md` | Annotate U-10-accepted residuals |
| This story file | Fill Consolidated Audit Report |

**Do not edit unless UAT blocking:** zone panel `resized()` algorithms, Design*, DimensionFactory, widget paint/skins, CD workflows.

**Do not delete:** `Source/GUI/Tests/**` (Debug retain).

### Implementation guidance — CMake + JUCE_DEBUG (avoid the footgun)

**Footgun:** Excluding Test `.cpp` from Release via CMake while leaving `#include "Tests/TestComponent.h"` and `std::unique_ptr<TestComponent>` unguarded → **Release compile failure**.

**Required pairing:**

1. CMake excludes Test translation units from Release.  
2. All editor/header references are behind `#if JUCE_DEBUG`.

**Preferred CMake shape** (illustrative — match local style):

```cmake
set(TEST_SANDBOX_SOURCES
    Source/GUI/Tests/TestComponent.cpp
    # … all other Test*.cpp currently in PLUGIN_SOURCES …
)

# PLUGIN_SOURCES no longer lists Test*.cpp
target_sources(Matrix-Control PRIVATE
    ${PLUGIN_SOURCES}
    $<$<CONFIG:Debug>:${TEST_SANDBOX_SOURCES}>
)
```

For single-config Ninja Release presets, `$<$<CONFIG:Debug>:…>` evaluates empty when configuring Release — correct. For multi-config VS, the generator expression selects per-config — correct. Avoid `if(CMAKE_BUILD_TYPE STREQUAL "Debug")` alone if Windows multi-config presets must stay healthy.

**JUCE note:** `JUCE_DEBUG` is 1 in Debug builds; Release sets `NDEBUG` and `JUCE_DEBUG=0`. Guard production wiring with `#if JUCE_DEBUG`, not `#ifndef NDEBUG` alone if matching JUCE conventions in this codebase.

### Previous story intelligence (U-9 → U-10)

- U-9 closed Master Edit `y +=` → `removeFromTop`; owner smoke @ 100 %; full matrix → **U-10**.
- Pattern across U-5…U-9: fixed scaled strips, DesignChecks @ 100 %, odd-scale residual owned here.
- Do not invent remainder-distribute helpers unless UAT proves usability failure.
- Completion Notes must be grep-/proof-friendly (U-9 style).

### Git intelligence (recent)

- `a1398de` — Close U-9 Master Edit section stack  
- `4eaa743` — Close U-8 Patch Manager layout audit  
- Epic U layout work is complete; this story is hygiene + sign-off, not another zone rewrite

### Testing requirements

| Layer | Expectation |
|-------|-------------|
| Unit tests | Existing `Matrix-Control_Tests` green — no new GUI pixel tests |
| Manual UAT | Consolidated 7-preset matrix in this file (AC 1) |
| Release proof | Binary hygiene check (AC 2) |
| Debug regression | Harness still opens; one Test page smoke |

### Project context reference

- Follow `_bmad-output/project-context.md`: ScaledLayout, no global AffineTransform, Design* SSOT, English source comments only.
- After this story, § Dev test harness must describe Debug-only sandbox policy.

### References

- [Source: `_bmad-output/planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md` — Story U-10]
- [Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md` — D-062, D-063]
- [Source: `_bmad-output/implementation-artifacts/deferred-work.md` — U-5…U-9 residuals]
- [Source: `CONTRIBUTING.md` — Releasing blockers]
- [Source: `_bmad-output/implementation-artifacts/11-2-cd-release-pipeline.md` — Release binary hygiene cross-link]

## Dev Agent Record

### Agent Model Used

Composer (Cursor agent router)

### Debug Log References

- Release hygiene: `nm` + `strings` on Standalone Release binary — no `TestComponent`; no `Builds/.../Release/.../Source/GUI/Tests` objects.
- Debug retains harness: `strings`/`nm` show `TestComponent`; 24 `Test*.cpp.o` under Debug build tree.
- Unit tests: `Matrix-Control_Tests` exit 0 (2026-07-24).

### Completion Notes List

- CMake: moved 24 `Source/GUI/Tests/Test*.cpp` into `TEST_SANDBOX_SOURCES`; attached via `$<$<CONFIG:Debug>:${TEST_SANDBOX_SOURCES}>` on `Matrix-Control`.
- Wiring: `#if JUCE_DEBUG` on `PluginEditor`, `MainComponent` overlay helpers, `Logo` Shift+Ctrl, `HeaderPanel` relay.
- Docs: `project-context.md` § Dev test harness → Debug-only policy (v1.4); `deferred-work.md` U-5…U-9 residuals annotated **U-10 accepted delta**.
- Layout: no panel algorithm changes (no blocking UAT defect).
- Release proof: `nm`/`strings` on Standalone Release — no `TestComponent`; no Tests TUs in Release build tree.
- Debug proof: owner smoke 2026-07-24 — Shift+Ctrl+logo opens harness; Debug-only access verified.
- D-062: owner signed full 7-preset aggregate matrix 2026-07-24 (all zones pass; known Δ-U5…Δ-U9 acceptable).
- D-063: Release excludes sandbox; Debug retains it.
- Code review: Release Shift+Ctrl on logo is a hard no-op (does not open Settings); Debug still toggles harness.

### File List

- `CMakeLists.txt`
- `Source/GUI/PluginEditor.h`
- `Source/GUI/PluginEditor.cpp`
- `Source/GUI/MainComponent.h`
- `Source/GUI/MainComponent.cpp`
- `Source/GUI/Widgets/Logo.h`
- `Source/GUI/Widgets/Logo.cpp`
- `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h`
- `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp`
- `_bmad-output/project-context.md`
- `_bmad-output/implementation-artifacts/deferred-work.md`
- `_bmad-output/implementation-artifacts/u-10-release-gate-prod-audit-sign-off-and-d-062-d-063.md`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`

## Change Log

| Date | Note |
|------|------|
| 2026-07-24 | Story context created (ready-for-dev) — ultimate context engine analysis completed |
| 2026-07-24 | D-063 implemented (CMake Debug-only sandbox + JUCE_DEBUG wiring); Release hygiene proven |
| 2026-07-24 | Owner signed aggregate 7-preset UAT + Debug harness smoke; D-062/D-063 met; status → review |
| 2026-07-24 | Code review: Release Shift+Ctrl logo hard no-op (no Settings fallthrough); status → done |
