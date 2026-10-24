---
title: 'Clarify SkinEditorMainComponent boundaries'
type: 'refactor'
created: '2026-10-24'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
baseline_commit: '0ddf1c4e5fcadfc41ea4b0eb0a27713de0a5f733'
context:
  - '{project-root}/_bmad-output/specs/spec-skin-editor-clean-code-cleanup/SPEC.md'
  - '{project-root}/_bmad-output/specs/spec-skin-editor-clean-code-cleanup/slices.md'
  - '{project-root}/_bmad-output/specs/spec-skin-editor-clean-code-cleanup/inventory.md'
  - '{project-root}/CONVENTIONS.md'
  - '{project-root}/.cursor/rules/clean-code.mdc'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After the Chrome split, the Skin Editor central component still pulls broad includes and uses weak separator naming, so layout vs actions vs session boundaries are harder to read than they need to be.

**Approach:** Tighten includes and clarify naming across the existing layout / Actions / Session units without changing behavior, public API, or merging those translation units.

</frozen-after-approval>

## Implementation Notes

- Dropped unused `DevSkinJson.h` from the header; Actions.cpp includes it where JSON I/O lives.
- Replaced umbrella `SkinEditorChrome.h` in MainComponent TUs: layout `.cpp` uses Fonts (Tokens via Fonts); Actions uses Tokens only; Session dropped dead Chrome include; header no longer includes Tokens (skins-bar separator height starts at 0, set in layout).
- Renamed skins-bar paint separators to `skinsBarSeparatorXs_/Y_/H_` and local helper to `placeSkinsBarSeparator`; private methods grouped with Session / Actions / Layout comments matching the three TUs.
- Left nested `BlinkTimer` in-header (~9 lines, one use) — extraction would not improve reading.
- Kept Actions/Session split and public API unchanged; no CMake source changes.
- Build `macos-debug-arm64` target `Matrix-Control_SkinEditor` OK; `lint_touched.py` green.
- Commit skipped: standing rule — commit only on Guillaume’s explicit ask.
- Post-review: drop header Tokens + redundant Tokens include in layout `.cpp`; rename `placeSep`; private method section comments; LookAndFeel umbrella thinning deferred to story 5.

## Review Triage Log

- LookAndFeel still pulls full Chrome umbrella → defer (medium): story 5 owns LookAndFeel; recorded in deferred-work.md.
- Tokens in header only for `kButtonHeight` default → patch: init height to 0; drop Tokens from header.
- Redundant Tokens include in layout `.cpp` (Fonts already pulls Tokens) → patch: removed.
- Flat private method block → patch: Session / Actions / Layout section comments aligned to TUs.
- Opaque local `placeSep` after skinsBar rename → patch: renamed `placeSkinsBarSeparator`.
- Session still sees Chrome transitively via LookAndFeel → same root as first finding → defer (grouped).
