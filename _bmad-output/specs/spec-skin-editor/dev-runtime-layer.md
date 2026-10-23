# Dev runtime layer

Authoring sits on a **runtime Dev layer** above compiled Skin SSOT. C++ tables remain the product SSOT. The layer is what the editor edits and what Standalone Dev paints when a Dev skin is active.

## Mental model

```
Compiled Skin SSOT (C++ ColourChart + Black/Cream maps + Looks)
        ↑ loaded / duplicated from
Dev runtime layer (named Dev skins: palette + mappings)
        ↑ edited in Skin Editor; pushed live
Standalone Dev paint (truth)
        ↑ optional
JSON save/load of Dev layer only (mandatory for editor V1 done; see json-bounds.md)
```

**Reset mental:** discarding the Dev layer (or quitting without JSON) returns the product to compiled Black/Cream. Dev never silently becomes Release SSOT.

## Compiled Black / Cream

- **Read-only** in the editor: view palette + mappings; switch between them for reference / starting point.
- **Any mutation** requires **Duplicate** into a **named Dev skin** first.
- No in-place overlay editing of the compiled product skins.

## Named Dev skins

Minimum CRUD for the useful threshold:

| Action | Meaning |
|--------|---------|
| **Duplicate** | Copy Black or Cream (or another Dev skin) into a new named Dev skin |
| **Create** | New named Dev skin (may start empty/minimal or from a template — prefer Duplicate-from-BC as the primary real path) |
| **Delete** | Remove a Dev skin from the layer (not from C++ SSOT) |
| **Select** | Make a Dev skin (or read-only BC view) active; push selection to Standalone Dev when connected |

**Naming (locked):** free-text Dev skin names. Prefer definitive display names intended for later Matrix-Control UI; last-moment rename during C++ integration is allowed. No fixed slot list and no hard concurrent cap in V1.

## Palette in the layer

- Seeded from compiled `ColourChart` on load / Duplicate.
- Editable on **Dev skins only** (hex / pastille values).
- **Locked:** Dev skins may introduce **new pastille names** beyond compiled ColourChart; those names live in the Dev layer (and its JSON) until a later product integration lands them in C++ `ColourChart` / maps.

## Mappings in the layer

- Same Skin SSOT address: `(ObjectType, Property, State)` → palette entry name.
- Editor edits mappings on Dev skins; Standalone Dev resolves colours from the active layer when a Dev skin is selected.
- Looks / injection model from Skin SSOT stay; the layer feeds the same structured resolution path at runtime in Dev builds.

## Unbound

- **Central V1.** Keys in the product structured inventory that lack a mapping (or are marked unbound) for the **active Dev skin** appear as a **text list** in the editor: correct object × property × state names.
- Operator can **bind** an Unbound key to a palette pastille in the editor.
- **No** Unbound overlay, highlight, or reconstituted GUI on Matrix-Control paint.

## Product integration handoff

Invent in the editor → judge on Standalone Dev paint → **promote via a separate BMad-assisted C++ change** in Matrix-Control (palette entries, mappings, skin display name / variant wiring). The editor does not write Release SSOT. JSON (when present) is a portable snapshot of the Dev layer for that handoff, not the ship path.

## What this layer is not

- Not Release SSOT.
- Not a third colour system beside Skin SSOT concepts.
- Not automatic promotion of proud skins into C++ (BMad C++ step after invent).
