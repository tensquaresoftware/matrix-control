---
title: 'Skin Editor empty state without editor skin'
type: 'feature'
created: '2026-10-27'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** With no editor skin selected, Palette and Mappings still show the product skin (read-only), so the UI looks editable when the only useful next step is New or Duplicate.

**Approach:** When there is no editor skin, show Palette and Mappings empty the same way Color shows idle wells (no content objects), plus one clear coaching message telling the user to create or duplicate a skin. Keep the current shell and Skins panel; do not add a separate startup layout.

</frozen-after-approval>

## Implementation Notes

- Coaching copy (agent decision, then review patch): `SkinEditorUi::kNoEditorSkinHint` = `"Create a skin or duplicate the product skin to start."` so Duplicate maps to the Product action that stays enabled.
- `refreshAll` gates panel skin id on `editorSkinId.isNotEmpty()` so product fallback no longer fills Palette/Mappings when no editor skin; Color stays idle via existing `syncColourPickerToSelection` / `clearMappingModeToIdle`.
- Palette reuses `kEmptyHint` paint; Mappings adds `RowKind::kEmptyHint` with the same quiet grey tone; Object combo disabled when no objects.
- Compare-with-editor still shows product in panels via `visibleSkinId()`; empty editor skin always empties panels.
- Verified: `cmake --build --preset macos-debug-arm64 --target Matrix-Control_SkinEditor`; `lint_touched.py` OK.

## Review Triage Log

- Coaching vs disabled Editor Duplicate — medium → patched copy to name product Duplicate.
- Mappings sticky legend / Object chrome vs Color idle — false: Color keeps title and #/RGB chrome with empty wells; empty content objects was the intent.
- Same hint for every null skin — low rejected: rare stale-id path; delete clears editor id.
- Object combo still interactive when empty — low → patched `objectCombo_.setEnabled(! objects_.empty())`.
- Compare toggleable with no editor skin — defer: pre-existing session chrome quirk.
- Empty-hint missing kFocusPad vs table rebuild — false: Content paints rows from y=0; kFocusPad in rebuildRows is ObjectTable metadata, not a painted inset.
- Parallel headerText vs emptyHintText — low rejected: not worth unifying models in this oneshot.
- Duplicated 0xff666666 hint colour — low rejected: matches existing Palette literal; extract later if a third caller appears.
- No unit coverage for empty editor panels — defer: Skin Editor GUI empty-state outside current unit pyramid.
