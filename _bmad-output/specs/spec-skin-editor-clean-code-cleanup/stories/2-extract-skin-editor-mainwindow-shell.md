---
title: 'Extract Skin Editor MainWindow shell'
type: 'refactor'
created: '2026-10-24'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
baseline_commit: '2c459c586f3b0b203eadeec7604d72c7c16a2021'
context:
  - '{project-root}/_bmad-output/specs/spec-skin-editor-clean-code-cleanup/SPEC.md'
  - '{project-root}/_bmad-output/specs/spec-skin-editor-clean-code-cleanup/slices.md'
  - '{project-root}/_bmad-output/specs/spec-skin-editor-clean-code-cleanup/inventory.md'
  - '{project-root}/CONVENTIONS.md'
  - '{project-root}/.cursor/rules/clean-code.mdc'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Skin Editor window placement, restore, zoom, and persist logic live nested inside the JUCE application startup file, so bootstrap and window shell are hard to read and own separately.

**Approach:** Lift the nested MainWindow (and its window-state helpers) into a named Skin Editor unit; leave Main.cpp as application lifecycle plus window create/ownership, with identical window behavior.

</frozen-after-approval>

## Implementation Notes

- Named the extracted type `SkinEditorMainWindow` (header + cpp); moved `skinEditorWindowStateFile` into the window TU anonymous namespace.
- `Main.cpp` keeps only `SkinEditorApplication` lifecycle, dirty-quit via content `tryQuitWithDirtyGate`, and `persistWindowState` on shutdown/quit.
- Registered `Tools/SkinEditor/SkinEditorMainWindow.cpp` in `Matrix-Control_SkinEditor` `target_sources`.
- Build `macos-debug-arm64` target Skin Editor OK; `lint_touched.py` green.
- Commit skipped: standing rule — commit only on Guillaume’s explicit ask (also avoids mixing uncommitted story 1 Chrome split).
- Post-review patches: include `SkinEditorChromeTokens.h` (not umbrella); restore width/persist header comment; `zoomToMaxHeightAndCentre` private; shared `kMinContentHeight`; `#include <memory>` in `Main.cpp`.

## Review Triage Log

- Umbrella Chrome include → patch (medium): switched to `SkinEditorChromeTokens.h`.
- Lost width/persist comment → patch (low): restored original DocumentWindow comment.
- Dirty-quit still via MainComponent cast → false: intentional; story keeps quit gate on content component.
- Public `zoomToMaxHeightAndCentre` → patch (low): made private.
- Unnamed magic 820/400/50/12/4096 → low rejected: move-only extraction; naming all would exceed Boy Scout without behavior need.
- CMake mixed with story 1 Chrome sources → false: expected from continuing on uncommitted story 1 tree.
- Hard-coded 480 vs local `kMinH` → patch (low): unified as `kMinContentHeight`.
- Missing `<memory>` in thin Main.cpp → patch (medium): added explicit include.

