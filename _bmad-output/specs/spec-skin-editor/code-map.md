# Code map — Skin Editor (suggested review / build order)

Brownfield pointers for `/bmad-build`. Do not reopen Skin SSOT model work unless a hard contradiction appears.

## Adopted model (read, do not extend here)

- Spec: `_bmad-output/specs/spec-skins-colour-model-ssot/SPEC.md`
- Target model: `…/target-model.md`
- Types: `Source/GUI/Skins/SkinValues.h` (`SkinObjectType`, `SkinColourProperty`, `SkinColourState`, `SkinColourKey`)
- Palette: `Source/GUI/Skins/ColourChart.h`
- Maps / runtime: `Source/GUI/Skins/Skin.h`, `Skin.cpp`, `SkinColours*.h`, `ISkin*.h`
- Injection: `Source/GUI/Looks/` + builders/orchestrators that call `setSkin`
- Active product skin switch today: `Source/GUI/PluginEditorSkinScale.cpp` (Black/Cream only)

## Suggested implementation / review order

1. **Dev layer types** — in-memory named Dev skin (palette + mappings) seeded from compiled Skin SSOT; Duplicate/read-only BC rules (`dev-runtime-layer.md`).
2. **Standalone Dev receiver** — thin apply + repaint path; gated compile; no Release pollution (`thin-bridge.md`, `cmake-packaging.md`).
3. **Skin Editor app shell** — new CMake Standalone JUCE target; connection status; no product preview.
4. **Skin list + Duplicate/select** — push active skin choice; live paint proves apply.
5. **Palette + mapping editors** — Dev-only mutation; push on change/apply.
6. **Unbound list + bind** — text-only; updates mappings + push.
7. **JSON export/import** — required for editor V1 done; round-trip Dev layer (`json-bounds.md`).
8. **Packaging gates** — verify Release builds omit editor + receiver.

## Touch with care

- Do not add local hex / shrink on the V1 user GUI path “for the editor”.
- Do not treat Orbitron as PatchNameDisplay typography.
- Do not widen `Skin::ColourVariant` into shipped third skins as an editor V1 requirement — Dev names live in the runtime layer first.
- Receiver ≠ edit panel in MainComponent.

## Out of this map

- Skin Studio HTML repo Lot C.
- Proud Black/Cream C++ integration after invent (separate later step).
- Plugin-in-DAW author IPC.
