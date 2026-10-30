---
title: 'Skin Editor Palette mapping counts'
type: 'feature'
created: '2026-10-31'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Palette colour rows do not show how many Mappings use each colour, so authors cannot see usage at a glance.

**Approach:** On each Palette colour row, show the mapping reference count as right-aligned amber text (same `attention()` colour as Mappings "Unbound" label text). When unused (count 0), leave the trailing slot empty — do not paint `0`. Match the attached mock for non-zero counts: digits at the trailing edge of the paint row; name elides before the count column.

</frozen-after-approval>

## Implementation Notes

- Agent decisions: plain `attention()` digits (not Unbound badge capsules); cache count on `Row` via `Layer::countMappingsReferencing` in `rebuildRows`; reserve trailing width so names elide; avoid widening `kPalettePanelWidth` unless layout proves insufficient for two-digit counts.
- Follow-up: unused colours paint no digit (empty), still reserve the count column so names do not reflow when usage goes 0↔N.
- Files: `Tools/SkinEditor/SkinEditorPalettePanel.h`, `Tools/SkinEditor/SkinEditorPalettePanel.cpp`.
- Extracted `paintColourRow` to satisfy function-length gate; `mappingCountColumnWidth_` sized to widest digit string so counts right-align.
- Build: `Matrix-Control_SkinEditor` (macos-debug-arm64) OK; `lint_touched.py` OK.
- Review patches: exclude count from rename hit-test; floor count column at `"99"` width; shared layout constants for paint/hit; drop unused `textBounds` on colour path.

## Review Triage Log

- Rename hit includes count digits — **medium / patch**: exclude trailing count from `listBoxItemDoubleClicked`.
- Count column reflows at 9→10 — **medium / patch**: floor `mappingCountColumnWidth_` at width of `"99"`.
- Spec missing mock path / AC — **false**: oneshot route intentionally omits those sections; Intent + mock in chat are the contract.
- Always paint `0` in `attention()` — **false**: required by Intent (show 0; same amber as Unbound text).
- No tooltip / a11y name — **low / rejected**: not requested; glanceable digit matches mock.
- Duplicated swatch/name insets vs count geometry — **low / patch**: shared anonymous constants + `mappingCountLeft`.
- `textBounds` built for colour rows unused — **low / patch**: compute only on section/empty-hint paths.
- No measured baseline for panel widen gate — **low / rejected**: two-digit floor + grow-for-wider counts is enough without widening panel.
