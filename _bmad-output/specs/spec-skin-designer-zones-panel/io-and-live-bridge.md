# Zones — data I/O and live bridge

Companion for CAP-2, CAP-3, CAP-6. Extends the existing Designer→Standalone Dev bridge (`../spec-skin-editor/thin-bridge.md`); does not replace it.

## Ownership

- Zone map is stored **per Designer skin** (same document unit as that skin’s palette and mappings).
- Switching active Designer skin shows that skin’s map; Reset restores **that** skin to registry defaults.

## Skin JSON (Designer pack)

- Persist an optional per-skin zone map (name is an implementation choice; must be clearly distinct from `colourSetLabels` and `mappings`).
- Represent only **used** MCS cells: `(SkinObjectType, SkinZone) → ColourSet`.
- Omit unused cells; on load, ignore unknown objects/zones and drop assignments outside `colourSetCount(object)`.
- Missing zone map on load ⇒ treat as registry defaults (equivalent to Reset).
- Round-trip: save → reload restores author assignments for used cells.

## PushPayload (live)

Unlike `colourSetLabels` (Designer-only, omitted from push), the zone map **must** travel on `PushPayload` whenever the active Dev skin is pushed.

Receiver apply path:

1. Apply palette + mappings as today (drawer paints).
2. Install / replace the active Dev **zone overlay** from the payload map (used cells only).
3. Trigger product GUI refresh so Look-for-zone callers pick up new drawers.

Ack/error codes may extend existing bridge errors (e.g. invalid colour set for object) without a second transport.

## Runtime resolution

| Context | MCS used cell | MCS unused / UCS |
|---------|---------------|------------------|
| No Dev overlay (compiled / Release path V1) | Registry default matrix | Unchanged product rules |
| Dev overlay from Designer push | Overlay map value | Unchanged product rules; exceptions stay call-site / registry (not overlay-editable in V1) |

Look-for-zone helpers (or their single resolution entry) must read the overlay when present so Patch/Master/Header/Modales call sites do not each invent local maps.

Exceptions in V1 continue to use the registry / existing hard exception zone behaviour; they are displayed in the Designer but not overridden by the skin zone map.

## Designer interactions tied to I/O

| Action | Persistence | Live |
|--------|-------------|------|
| Change matrix combo | Update active skin map; dirty pack | Push payload including zone map (same cadence as mapping edits) |
| Reset to product defaults | Replace map with registry defaults | Push |
| Save / Save as | Pack includes per-skin zone maps | N/A |
| Connect bridge | N/A | Push-on-connect includes zone map so Standalone matches Designer |

## Offline

Designer remains usable without Standalone; zone edits stay in the Dev layer / pack. Live geography is unavailable until connect — no fake in-Designer product paint.
