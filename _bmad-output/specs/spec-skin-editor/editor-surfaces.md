# Editor surfaces (V1 inventory)

Native JUCE UI in **Matrix-Control Skin Editor**. Model editing only — no product GUI preview pane.

## Required surfaces (editor V1 done)

1. **Connection / lifecycle status**
   - Connected / disconnected to Standalone Dev.
   - Last push ack or error (short text).

2. **Skin list**
   - Compiled **Black** and **Cream** (read-only badge).
   - Named **Dev** skins with **free-text** names (prefer definitive future UI labels).
   - Actions: Duplicate (from BC or Dev), Create (if kept), Delete (Dev only), Select / activate, rename Dev skin.

3. **Palette**
   - Pastille list for the active skin.
   - Edit hex/value on Dev skins only.
   - Add **Dev-only pastille names** beyond compiled ColourChart (locked).

4. **Mappings**
   - Browse / edit `(ObjectType, Property, State)` → palette name for the active Dev skin.
   - Read-only when Black/Cream compiled is selected.

5. **Unbound**
   - Text list of unbound structured keys for the active Dev skin.
   - Bind action: choose palette pastille → mapping created; list updates; push on apply.

6. **JSON export / import**
   - Required for editor V1 done: save/load Dev layer file; round-trip restores skins/palette/mappings.

## Nice-to-have (not ship blockers)

- Fancy colour pickers beyond a working pastille editor — not a second colour system.

## Explicitly out of this inventory

- Reconstituted Matrix-Control layout / pixel-near preview.
- Unbound overlay on Standalone paint.
- In-app edit panel inside product MainComponent Release layout.
- Skin Studio HTML shell / Lot C.
- Contributor / multi-user collaboration UI.

## Workflow reminder

Typical: Standalone Dev fullscreen on laptop; Skin Editor on external display; push after edits; judge truth only on Standalone paint.
