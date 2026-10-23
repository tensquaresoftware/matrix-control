# JSON bounds — mandatory for editor V1 done

JSON is **save/load of the Dev runtime layer**, not product SSOT and not the live paint pipe.

## Ship criterion (locked)

**Skin Editor V1 done** requires all of:

1. Named Dev skin CRUD (Duplicate / create / delete / select)
2. Live Standalone Dev paint from the pushed layer
3. Actionable Unbound list / bind in the editor
4. **Dev-layer JSON export / import with round-trip**

Do **not** declare editor V1 done without JSON I/O. Do **not** make JSON the Release skin SSOT.

**Alpha unchanged:** multi-OS alpha may still ship on compiled Black/Cream without the editor.

## Conceptual Dev JSON schema

File describes one or more named Dev skins. Fields (conceptual; exact keys/versioning left to implementer within this shape):

- `schemaVersion`
- `skins[]` each with:
  - `id` / `name`
  - `derivedFrom` (optional: `black` | `cream` | other Dev id)
  - `palette[]`: `{ name, argbOrHex }` (compiled names + Dev-only names)
  - `mappings[]`: `{ object, property, state, paletteName }`

**Round-trip (required):** export then import into a fresh editor session restores the same Dev layer content (names, palette, mappings). After import, Standalone Dev receives the layer via the thin bridge when connected — not by reading JSON on the Release path.

## Handoff

JSON is the portable snapshot for the later **BMad-assisted C++** product integration. It does not auto-write Release SSOT. No parallel “autosave to C++” path inside the editor.

## Forbidden

- Loading JSON skins on the Release user path as product SSOT.
- Replacing `ColourChart` / Black / Cream C++ tables with file skins for shipping.
- Shipping “editor V1 done” without Dev-layer JSON round-trip.
- Treating JSON export as automatic Release integration.
- Holding multi-OS alpha hostage to the editor / JSON (alpha net stays).
