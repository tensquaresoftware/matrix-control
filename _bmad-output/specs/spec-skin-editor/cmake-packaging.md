# CMake / packaging — Skin Editor + Dev receiver

Same **Matrix-Control** repository; separate Dev authoring surface. No separate repo in V1.

## Targets

| Target | Purpose |
|--------|---------|
| **Matrix-Control Skin Editor** | New Standalone JUCE application target (Dev/author). Native JUCE widgets. Not installed for end users. |
| **Matrix-Control Standalone (Dev)** | Existing Standalone with a **thin Dev receiver** that applies pushed Dev layer / skin choice and repaints. |

Exact CMake target names are implementer choice; behaviour is not.

## Gating

- Editor binary: built only when a **Dev / editor** CMake option (or preset) is on — **absent from Release user builds** and from end-user artefact packaging.
- Receiver: compiled only into **Standalone Dev** (same option/preset family). **Not** linked into AU/VST3 (or other plugin formats) for authoring. **Not** present in Release Standalone packaging intended for users.
- Release user path continues to ship compiled Black/Cream Skin SSOT only; no Dev overlay, no editor panel in product layout.

## Sharing code

- Prefer sharing Skin SSOT types / palette / mapping concepts from existing `Source/GUI/Skins/` (and related Looks) rather than forking a second colour model.
- Editor may link a **Dev-only** subset of sources; must not pull product Core audio requirements into a fake second host unless needed for a minimal JUCE app shell.
- Do not place an in-layout edit panel on the Release MainComponent path.

## Packaging / copy artefacts

- Dev presets may copy Skin Editor + Dev Standalone into a Dev artefacts folder.
- User Release copy scripts must **not** include the Skin Editor or enable the receiver.

## Alpha

Multi-OS alpha uses current Black/Cream Skin SSOT without requiring the editor or Dev receiver. Editor is desirable, not an alpha blocker.
