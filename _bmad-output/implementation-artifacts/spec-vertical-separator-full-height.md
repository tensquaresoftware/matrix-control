---
title: 'VerticalSeparator full-height line (remove paddings)'
type: 'refactor'
created: '2026-07-17'
status: 'done'
route: 'one-shot'
updated: '2026-07-17'
---

## Intent

**Problem:** The thick vertical line drawn by `VerticalSeparator` was inset from the component’s top and bottom via design paddings (4 / 2), so it did not span the full widget height. Even after full-height paint, BodyPanel still placed separators in the content band (704 px, inset by 12 px padding), so the lines did not meet the Header/Footer thick borders.

**Approach:** Draw the line across the full local bounds height, remove unused padding tokens, and lay out BodyPanel separators at full Body height (728 px, Y = 0) so they join the Header/Footer panel-edge borders.

## Spec Change Log

- 2026-07-17 — After paint-only pass, visual verification showed 12 px gaps above/below from Body padding. Extended layout to full Body height (`kVerticalSeparator` = `Panels::Body::kHeight`) and placed separators at `bounds.getY()`.

## Suggested Review Order

**Paint**

- Full-height line: keep component bounds height, only center width
  [`VerticalSeparator.cpp:44`](../../Source/GUI/Widgets/VerticalSeparator.cpp#L44)

**Layout (Header/Footer join)**

- Separators at Body Y=0 with full Body height
  [`BodyPanel.cpp:91`](../../Source/GUI/Panels/MainComponent/BodyPanel/BodyPanel.cpp#L91)

- Design height = Body panel height (not content band)
  [`DesignPanels.h:258`](../../Source/GUI/Layout/Design/DesignPanels.h#L258)

- Compile-time guard for full-Body separator height
  [`DesignChecks.h:129`](../../Source/GUI/Layout/Design/DesignChecks.h#L129)

**Token cleanup**

- Drop `kTopPadding` / `kBottomPadding` from design atoms
  [`DesignAtoms.h:166`](../../Source/GUI/Layout/Design/DesignAtoms.h#L166)

- Remove padding fields from `SeparatorDimensions`
  [`WidgetDimensions.h:118`](../../Source/GUI/Layout/WidgetDimensions.h#L118)

- Stop wiring paddings in the dimension factory
  [`DimensionFactory.cpp:403`](../../Source/GUI/Factories/DimensionFactory.cpp#L403)
