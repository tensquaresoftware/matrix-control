---
title: 'VerticalSeparator full-height line (remove paddings)'
type: 'refactor'
created: '2026-07-17'
status: 'done'
route: 'one-shot'
---

## Intent

**Problem:** The thick vertical line drawn by `VerticalSeparator` was inset from the component’s top and bottom via design paddings (4 / 2), so it did not span the full widget height.

**Approach:** Draw the line across the full local bounds height, and remove the unused padding atoms and `SeparatorDimensions` fields.

## Suggested Review Order

**Paint**

- Full-height line: keep component bounds height, only center width
  [`VerticalSeparator.cpp:44`](../../Source/GUI/Widgets/VerticalSeparator.cpp#L44)

**Token cleanup**

- Drop `kTopPadding` / `kBottomPadding` from design atoms
  [`DesignAtoms.h:166`](../../Source/GUI/Layout/Design/DesignAtoms.h#L166)

- Remove padding fields from `SeparatorDimensions`
  [`WidgetDimensions.h:118`](../../Source/GUI/Layout/WidgetDimensions.h#L118)

- Stop wiring paddings in the dimension factory
  [`DimensionFactory.cpp:403`](../../Source/GUI/Factories/DimensionFactory.cpp#L403)
