---
title: 'Structure SkinEditorMappingsPanel'
type: 'refactor'
created: '2026-10-24'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
baseline_commit: 'ec13985ababd87223b5cb9fd40ab6b2fe7e1217d'
context:
  - '{project-root}/_bmad-output/specs/spec-skin-editor-clean-code-cleanup/SPEC.md'
  - '{project-root}/_bmad-output/specs/spec-skin-editor-clean-code-cleanup/slices.md'
  - '{project-root}/_bmad-output/specs/spec-skin-editor-clean-code-cleanup/inventory.md'
  - '{project-root}/CONVENTIONS.md'
  - '{project-root}/.cursor/rules/clean-code.mdc'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The Mappings panel is the largest Skin Editor panel unit; paint, row rebuild, and bind logic sit together while the header nests several private types, which slows reading without changing product behavior.

**Approach:** Clarify paint vs rebuild vs bind boundaries in place (named helpers, thinner paint-only nesting), keep the existing Rows split, and leave bind / sticky legend / object jump / focus silhouette behavior identical.

</frozen-after-approval>

## Implementation Notes

- Kept `Row` / `ObjectTable` / `Content` / `PaintRowSpec` in the header (`PaintRowSpec` retained to stay under the parameter gate); removed unused `<optional>` include.
- Grouped private API and `.cpp` bodies with Rebuild / Paint / Bind / Layout section comments; row builders stay in `SkinEditorMappingsPanelRows.cpp`.
- Extracted `paintMappingCells`, `paintFocusedObjectOutline`, and `isMappedColumnClick` (focus-pad inset owned by the helper).
- No new translation unit — already under the useful-line budget (~337 useful in the main `.cpp`).
- Build `macos-debug-arm64` target `Matrix-Control_SkinEditor` OK; `lint_touched.py` green.
- Commit skipped: standing rule — commit only on Guillaume’s explicit ask.
- Post-review patches: honour `cellArea.getX()`; mirror section order in `.cpp`; fold `kFocusPad` into `isMappedColumnClick`.

## Review Triage Log

- `paintMappingCells` ignored `cellArea.getX()` → patch: use x-origin; call site stays table-local `{0, …}`.
- Header sections not mirrored in `.cpp` → patch: reorder + section comments.
- `isMappedColumnClick` left focus-pad subtraction on the caller → patch: helper takes content x and subtracts inset.
- Further thin `Content::paint` clip/roundBottom loop → false: story asked for helpers only where density warranted; more extract would be a list framework.
- DRY sticky legend vs `paintMappingCells` → false: header uses bold/section fill; body uses regular/swatch — not identical copy.
- Main `.cpp` near ~400 total lines → false: useful ≈337 under the analyser useful-line gate.
- Missing docs on new helpers / raw `PaintRowSpec::item` → rejected low: project prefers intentful names over narrative comments; pointer lifetime matches prior `PaintRowSpec` usage.
