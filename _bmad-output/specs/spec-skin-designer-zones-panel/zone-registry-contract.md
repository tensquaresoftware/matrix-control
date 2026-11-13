# Zone registry contract (shared product surface)

Load-bearing companion for CAP-5 and CAP-1/3 defaults. Implementation may live beside or inside the existing skin inventory headers; **what** must be exposed is fixed here.

## Readers

| Consumer | Uses |
|----------|------|
| Skin Designer Zones panel | Occupancy, default matrix, UCS list basis, exception rows |
| Plugin Look-for-zone (no Dev overlay) | Default matrix (= today’s product behaviour) |
| Unit tests | Pin inventory + geography + exceptions |

## Occupancy (used vs em-dash)

A cell `(object, zone)` is **used** only if the product places that MCS object in that geography. Unused cells render `—` and are not assignable.

| Object | Patch | Master | Header | Modales |
|--------|-------|--------|--------|---------|
| Slider | used | used | used | used |
| ComboBox | used | used | — | used |
| PopupMenu | used | used | — | used |
| Led | — | — | used | used |
| PeakIndicator | — | — | used | used |

Footer: no MCS zone column (product rule). HeaderPanel / Dialog chrome are UCS surfaces of Header / Modales geography, not MCS matrix rows.

## Default matrix (product defaults)

Defaults match the shipped `zoneColourSet` behaviour for **used** cells:

| Object | Patch | Master | Header | Modales |
|--------|-------|--------|--------|---------|
| Slider | cs1 | cs2 | cs3 | cs4 |
| ComboBox | cs1 | cs2 | — | cs3 |
| PopupMenu | cs1 | cs2 | — | cs3 |
| Led | — | — | cs1 | cs2 |
| PeakIndicator | — | — | cs1 | cs2 |

UCS objects always resolve technical `cs1` / `k1`; they appear only in the Shared / single-set block, never as MCS matrix rows the author “ticks Shared” for.

Drawer counts (inventory): Slider 4; ComboBox + PopupMenu 3; Led + PeakIndicator 2; all other objects 1.

## Named exceptions (V1 read-only rows)

Each row: **Where** · **Object** · **Uses** (drawer or UCS).

| Where | Object | Uses |
|-------|--------|------|
| Patch Manager — Computer Patches file browser | ComboBox | cs3 (Modales mono drawer of ComboBox) |
| Patch Manager — Computer Patches file browser | PopupMenu | cs3 |
| Header — logo menu | PopupMenu | cs3 |
| Header — Undo / Redo / Panic | Button | UCS cs1 |

Authors cannot add/remove/edit these rows in V1. Registry is the only SSOT the Zones Exceptions block may display.

## API shape (intent, not prescription)

Expose enough for:

1. `isZoneUsed(object, zone) → bool`
2. `defaultColourSet(object, zone) → ColourSet` for used cells (and defined fallback for unused/UCS consistent with today’s helpers)
3. Iterate MCS objects / zones / exception rows
4. Keep `cs` technical; zone names only as geography labels

Do not teach the Designer that Colour set help labels (“Accent A”) are geographic SSOT.
