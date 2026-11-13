---
id: SPEC-skin-designer-zones-panel
companions:
  - zone-registry-contract.md
  - io-and-live-bridge.md
  - ../../implementation-artifacts/spec-zones-skins-black-duo-cream.md
  - ../spec-skin-editor/thin-bridge.md
  - ../../../Documentation/Development/Plans/2026/11/2026-11-13-Skin-Designer-Zones-Panel-Vision.md
sources: []
---

> **Canonical contract.** This SPEC and the files in `companions:` are the complete, preservation-validated contract for what to build, test, and validate. Source documents listed in frontmatter are for traceability — consult them only if you need narrative rationale or prose color this contract intentionally omits.

# Skin Designer — Zones panel (V1)

## Why

**Pain + vision.** Skin Designer already lets an author paint drawers (`cs1…csN`) in Mappings and push palette/mappings live into connected Matrix-Control Standalone. Product geography (Patch / Master / Header / Modales) still resolves only from a frozen code matrix. Third-party authors cannot assign which drawer each GUI zone reads, nor see that assignment live. This spec adds the Zones work surface and a per-skin zone map that the existing Designer→Standalone bridge consumes, without replacing Mappings or inventing cascade Module/Instance yet.

## Capabilities

- **CAP-1**
  - **intent:** Author can open a Zones panel beside Palette and Mappings and see, for the active Designer skin, the MCS object×zone matrix, a read-only UCS/Shared block, and a read-only Exceptions list from the shared product registry.
  - **success:** With Designer running, selecting Zones shows five MCS rows (Slider, ComboBox, PopupMenu, Led, PeakIndicator), four zone columns, em-dash cells where the registry marks unused, a UCS list matching inventory, and the named exception rows from the registry — without editing Mappings layout beyond optional jump (CAP-4).

- **CAP-2**
  - **intent:** Author can change an MCS matrix combo to reassign a used zone cell to another valid drawer for that object on the active skin, and a connected Standalone Dev instance repaints geography from that map via the existing live bridge.
  - **success:** Changing e.g. Patch→Slider from `cs1` to `cs2`, then pushing (same cadence as mapping edits today), makes Patch sliders in Standalone resolve the Slider `cs2` drawer colours; Mappings pastilles for those drawers are unchanged.

- **CAP-3**
  - **intent:** Author can reset the active skin’s zone map to product registry defaults in one action.
  - **success:** After arbitrary matrix edits, Reset restores every used cell to the registry default `zoneColourSet` values; Standalone push reflects defaults; unused cells remain em-dash.

- **CAP-4**
  - **intent:** Author can optionally jump from a Zones MCS object name into Mappings filtered on that object.
  - **success:** Activating the object-name control focuses Mappings on that object’s rows; no Zone column is added to Mappings.

- **CAP-5**
  - **intent:** Designer and plugin share one registry surface for MCS occupancy, default zone→cs matrix, UCS listing basis, and named exception rows (see `zone-registry-contract.md`).
  - **success:** Unit tests pin occupancy, defaults, and exception rows against the registry; Designer UI reads only that surface for those tables — no duplicated hard-coded exception lists in Tools/SkinDesigner.

- **CAP-6**
  - **intent:** When a Dev overlay is active from a Designer push, Look-for-zone resolution uses the payload zone map for MCS used cells; when no overlay map is present, resolution uses product registry defaults.
  - **success:** Wiring/unit tests show overlay map wins for used pairs; compiled / no-overlay path still matches today’s registry defaults; unsupported pairs stay non-assignable and do not invent drawers.

## Constraints

- Two surfaces: Mappings paints drawers; Zones assigns geography. Do not replace the Colour set column with “GUI Zone”; do not add a Zone column on every Mappings row in V1.
- `cs1…csN` remain technical drawer identities in JSON and runtime; never rename them to Patch/Master/Header/Modales.
- Zone assignment is a separate free map (not a fixed zone≡cs identity), limited to drawers that exist for the object and to cells marked used by the registry.
- Zone map is owned **per Designer skin** (same ownership model as palette-per-skin).
- Zone map **must** be included in `PushPayload` and applied by Standalone Dev (unlike Designer-only `colourSetLabels`).
- Exceptions block in V1 is **read-only** mirror of the registry; authors cannot invent exceptions the plugin ignores.
- UCS / Shared is imposed by code inventory; dedicated read-only block; no empty Shared column on MCS rows; Footer has no MCS zone column.
- Primary matrix gesture is combos — not Palette→Mappings Hold/bind.
- Cascade Module/Instance is out of V1.
- Live path stays the existing local Designer→Standalone Dev bridge; Release/DAW compiled path remains registry-default geography in V1.
- Zones panel useful width comparable to Mappings; third work surface: `[ Palette ] [ Mappings ] [ Zones ]`.

## Non-goals

- Replacing Colour set in Mappings or adding a Zone column on every Mappings row.
- Editable exceptions / cascade Module / Instance / GUI click-inspector.
- Renaming technical `cs` indices to zone names in JSON.
- Shipping live zone overlay into Release plugin packaging.
- Parallel TONES / PhotoCraft / Trials work.
- Palette redesign (already shipped).

## Success signal

An author paints drawers in Mappings, rewires Patch/Master/Header/Modales to different drawers in Zones, and sees the connected Standalone Matrix-Control geography update without restarting — while exceptions and UCS remain honest read-only mirrors of the product registry. Offline, save/reload of the Designer pack preserves the per-skin zone map.

## Assumptions

- Existing bridge (push + ack) remains the live pipe; Zones extends payload rather than adding a second preview stack.
- “Temps réel” means the same push-on-edit / push-on-connect behaviour already used for palette and mappings — not a new continuous streaming protocol.
- Product default matrix and exception set remain those shipped by the zones Black/Duo/Cream product work unless Guillaume renegotiates the registry content.
- CAP-4 (jump to Mappings) may ship in the same build or immediately after the live matrix; it must not block CAP-1–3 / CAP-5–6.
