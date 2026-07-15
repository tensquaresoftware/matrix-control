---
title: 'Patch Mutator History Control Widths'
type: 'feature'
created: '2026-07-15'
status: 'done'
route: 'one-shot'
baseline_commit: df0c96ba69985c35622c4ce77183c194fdb13f4e
---

# Patch Mutator History Control Widths

## Intent

**Problem:** Closed HISTORY ComboBox labels (`Mxx-Ryy`) are truncated because the control is too narrow at 48 px.

**Approach:** Reconcile Patch Mutator row layout with the Figma mockup — flush AMOUNT/RANDOM/HISTORY labels against their controls (remove the 4 px label gap), widen sliders and HISTORY to 56 px, and shrink MUTATE/RETRY/COMPARE to 48 px so net row width stays unchanged.

## Suggested Review Order

- Design atoms encode the Figma width trade-off (controls +8, action buttons −4).
  [`DesignAtoms.h:67`](../../Source/GUI/Layout/Design/DesignAtoms.h#L67)

- Label step omits inter-control gap; sliders/combos stay gap-separated from buttons.
  [`PatchMutatorPanel.cpp:828`](../../Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp#L828)

- History row uses the same flush-label rule as amount/random rows.
  [`PatchMutatorPanel.cpp:875`](../../Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp#L875)

- Widget dimension registry tests inherit atom values automatically.
  [`WidgetDimensionRegistryTests.cpp:125`](../../Tests/Unit/WidgetDimensionRegistryTests.cpp#L125)
