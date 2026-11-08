---
id: SPEC-skin-editor-clean-code-cleanup
companions:
  - inventory.md
  - slices.md
  - ../../../CONVENTIONS.md
  - ../../../.cursor/rules/clean-code.mdc
  - ../../project-context.md
sources: []
---

> **Canonical contract.** This SPEC and the files in `companions:` are the complete, preservation-validated contract for what to build, test, and validate. Source documents listed in frontmatter are for traceability — consult them only if you need narrative rationale or prose color this contract intentionally omits.

# Skin Editor clean-code structural cleanup

## Why

**Pain to solve + mandate to meet.** Skin Editor UI polish and its targeted review are done; `lint_touched.py` is green on `Tools/SkinEditor/` (including `--all`: zero Skin Editor findings). The quality gate was enabled late, so the tree can sit under analyser thresholds while remaining hard to maintain: a ~500-line chrome header mixing tokens and paint, a nested window shell in `Main.cpp`, and panel/orchestrator glue that is readable enough to ship but not “as clean as the repo’s Clean Code bar allows.” This spec is the SSOT for **behavior-preserving** structural cleanup delivered as short `/bmad-build` slices — not a mega-PR, and not more polish UI.

## Capabilities

- **CAP-1**
  - **intent:** Maintainers can rely on an honest inventory of Skin Editor structural debt beyond green lint (roles, sizes, hotspots, severity).
  - **success:** `inventory.md` lists every `Tools/SkinEditor` unit with useful-line evidence, marks MUST/SHOULD/NICE, and records the 2026-10-24 zero-finding linter baseline without inventing violations.

- **CAP-2**
  - **intent:** Implementers can execute cleanup as ordered, independently shippable slices with shared DoD and per-slice AC.
  - **success:** `slices.md` + `stories.yaml` define six slices (Chrome → Main shell → MainComponent → Mappings → remaining panels/LnF → close-out); each Build owns only its zone.

- **CAP-3**
  - **intent:** Developers can work with Skin Editor chrome tokens/fonts/measure separately from panel/table paint without changing on-screen chrome.
  - **success:** Slice 1 lands; H1 closed; paint/layout parity verified by smoke on panels, mappings tabs, focus stroke, swatches, badges.

- **CAP-4**
  - **intent:** Developers can read application bootstrap separately from document-window placement/persist/zoom logic.
  - **success:** Slice 2 lands; H2 closed; restore/resize/zoom/dirty-quit smoke passes.

- **CAP-5**
  - **intent:** Developers can navigate MainComponent session vs actions vs layout without a kitchen-sink mental model.
  - **success:** Slice 3 lands; public surface stays small; skin/session smoke passes; no UX change.

- **CAP-6**
  - **intent:** Developers can maintain Mappings and the lighter panels with clear boundaries and intentful names, extracting only stable duplication.
  - **success:** Slices 4–5 land; H4 addressed; H5–H6 either cleaned or deferred with reason; bind/palette/picker smoke passes.

- **CAP-7**
  - **intent:** Guillaume can treat the cleanup campaign as finished with no silent leftover must-fix debt.
  - **success:** Slice 6 records closed must-fix hotspots and an explicit wontfix/defer table; Skin Editor remains 0 findings on `lint_touched.py --all` for that tree.

## Constraints

- Scope is `Tools/SkinEditor/` only; `CMakeLists.txt` only when registering new Skin Editor sources. No Boy Scout of plugin `Source/` or `Tests/` lint debt.
- Product behavior and UI must remain unchanged vs post-polish + post-CR baseline (including commit `2c459c58` and related polish). No new polish, features, copy tweaks, or chrome redesign “while we are here.”
- Quality bar is `CONVENTIONS.md` §3, `.cursor/rules/clean-code.mdc`, and `project-context.md` (Skin Editor shares the plugin GUI gate). Green lint is necessary, not sufficient.
- Boy Scout is scoped per slice; one Build (or story) per fresh chat after this spec is accepted.
- DRY only on stable duplication (2nd–3rd occurrence). Prefer names over narrative comments. English in source; Allman braces; project C++ standard from CMake.
- Do not reopen closed polish-review findings unless a cleanup slice introduces a regression.
- Dev skin runtime under `Source/GUI/Skins/Dev/` stays untouched unless a **proven** coupling blocks Tools-only work; any micro-touch must be documented as a motivated exception in that slice.
- Automated Skin Editor E2E is not required; fix or extend unit/smoke only if existing tests break or a real need appears.
- clang-tidy is optional, not mandated.
- Commits / PRs only on Guillaume’s explicit ask.

## Non-goals

- New Skin Editor UI polish, features, or visual redesign.
- Greening the whole plugin tree / historical `Source/` / `Tests/` debt.
- Relitigating the closed polish UI code review.
- Refactoring Dev skin runtime architecture by default.
- Mandatory clang-tidy or a new E2E harness.
- Implementing cleanup in the same chat that authors this spec.
- A single mega-PR that rewrites all of Skin Editor at once.

## Success signal

After slices 1–6 (or an agreed subset with deferred leftovers documented): every MUST-FIX hotspot in `inventory.md` is closed or explicitly deferred with reason; Skin Editor still shows **0** findings under `Tools/SkinEditor/` on `lint_touched.py --all`; macOS debug arm64 Skin Editor target builds; critical manual smoke (open, panels, bind/unmap, window restore when touched) matches the post-polish baseline with no intentional UX delta.

## Assumptions

- No dedicated automated Skin Editor unit/E2E suite is required for this campaign; verification is compile + lint_touched + manual smoke unless existing tests break.
- Dev skin runtime code is not a cleanup target unless a proven Tools→Dev coupling blocks a slice.
- Prior feature SSOT `specs/spec-skin-editor/` remains the product/architecture reference; this folder is the cleanup campaign SSOT only.

## Resolved decisions (Guillaume 2026-10-24)

- Scope confirmed: `Tools/SkinEditor/` only; `CMakeLists.txt` only for new Skin Editor sources; no plugin `Source/` / `Tests/` Boy Scout.
- Slice order confirmed: Chrome → Main.cpp shell → MainComponent → Mappings → remaining panels/LnF → close-out.
- Chrome shape confirmed: thin umbrella + content-typed headers; optional `.cpp` for heavy path paint; call sites kept stable via umbrella.
