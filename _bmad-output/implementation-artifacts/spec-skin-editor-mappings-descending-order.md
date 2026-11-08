---
title: 'Skin Editor — Mappings descending table order + Object combo A–Z'
type: 'feature'
created: '2026-11-01'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** In Skin Editor Mappings, object tables follow inventory first-seen order, which does not match a top-down GUI reading order, and the Object combo mirrors that same order so jumping to a known type is awkward.

**Approach:** Drive Mappings table/group order from an editor-only descending display list (panels → dialogs → displays → titles → widgets → transverse roles). Keep Object combo limited to currently visible groups but sort its labels alphabetically, mapping selection by object index id so scroll-to-table still works.

</frozen-after-approval>

## Implementation Notes

- Target descending order (product intent from GPC 1/5 / local ColourSets recap): HeaderPanel, BodyPanel, FooterPanel, FooterMessage, Dialog, WindowClose, Led, EnvelopeDisplay, TrackGeneratorDisplay, PatchNameDisplay, SectionHeader, ModuleHeader, GroupLabel, Label, Slider, SliderButtonLike, ComboBox, ComboBoxButtonLike, PopupMenu, PopupMenuButtonLike, NumberBox, Button, Toggle, HorizontalSeparator, DarkPanel, ReadOnlyValueField.
- Do not reorder `SkinObjectType` enum or `DevSkinColourElementList` inventory push order.
- Any inventory object missing from the display list must still appear (append after known order) so nothing is silently dropped.
- ModulationBusHeader / ColourSet / ButtonLike unification / DarkPanel rename: out of scope.
- Added `mappingsObjectDisplayOrder()` and routed `filteredObjectGroups()` through it; unknown inventory types still append in first-seen order.
- Object combo: alphabetical labels via `compareIgnoreCase`; ComboBox item IDs remain 1-based `objects_` indices; `onChange` / `setFocusedObject` use `getSelectedId` / `setSelectedId`.
- Tests lock full empty-filter group order against the display list and reject duplicate display entries.
- Review: strengthened order tests; deferred automated Object-combo coverage to deferred-work.md.
- Verification: `Matrix-Control_Tests --category SkinEditorMappingsColumnFilter` green; `lint_touched.py` OK; Skin Editor rebuilt. Manual smoke: open Mappings, confirm top-down tables and A–Z Object combo jump.

## Review Triage Log

- Blind Hunter: no Object-combo automated coverage → defer (manual smoke; deferred-work entry).
- Blind Hunter: `groups.size() <= order.size()` contradicted append contract → patch (assert exact order equality under full inventory coverage).
- Blind Hunter: middle sequence not locked → patch (index-by-index vs display list).
- Blind Hunter: duplicates in display list unchecked → patch (pairwise uniqueness).
- Blind Hunter: no enum exhaustiveness beyond inventory → false (inventory types are the UI population SSOT).
- Blind Hunter: `ObjectGroupBuildState` public helpers → patch (inlined lambda inside `filteredObjectGroups`).
- Blind Hunter: oneshot spec lacked done bar → patch (status done + verification notes).
- Blind Hunter: preferName exact `==` vs case-insensitive sort → false (`toString` casing is fixed).
- Blind Hunter: dual order SSOT confusion → false (editor display list is intentional; inventory stays key source).
