---
title: 'Matrix Modulation bus reorder highlight colours'
type: 'chore'
created: '2026-07-24'
status: 'done'
route: 'one-shot'
---

## Intent

**Problem:** During Matrix Modulation bus reorder drag, the source placeholder used a translucent grey wash and the drop target used a translucent red, which did not match the product colour language.

**Approach:** Paint the drag source with `ColourChart::kBlue` (`#004FB0`) and the drop target with `ColourChart::kRed`, each at 75% opacity, limited to the bus-number label bounds (not the full cell / separator). While either highlight is active, switch that label to the skin bold base font; restore the default label look when the highlight clears.

## Suggested Review Order

- Blue/red fill at 75% opacity on bus-number label bounds during reorder drag
  [`ModulationBusCell.cpp:199`](../../Source/GUI/Widgets/ModulationBusCell.cpp#L199)

- Bold bus-number font while source or drop highlight is active
  [`ModulationBusCell.cpp:179`](../../Source/GUI/Widgets/ModulationBusCell.cpp#L179)
