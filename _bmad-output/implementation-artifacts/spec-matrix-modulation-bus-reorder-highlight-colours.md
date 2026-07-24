---
title: 'Matrix Modulation bus reorder highlight colours'
type: 'chore'
created: '2026-07-24'
status: 'done'
route: 'one-shot'
---

## Intent

**Problem:** During Matrix Modulation bus reorder drag, the source placeholder used a translucent grey wash and the drop target used a translucent red, which did not match the product colour language.

**Approach:** Paint the drag source with opaque `ColourChart::kBlue` (`#004FB0`) and the drop target with opaque `ColourChart::kRed`, removing the temporary alpha constants.

## Suggested Review Order

- Opaque blue source / red target fills from ColourChart during reorder drag
  [`ModulationBusCell.cpp:183`](../../Source/GUI/Widgets/ModulationBusCell.cpp#L183)
