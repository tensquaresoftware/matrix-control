# Skin Designer — Zones panel vision (reprise)

**Status:** product / UX vision for a future chantier (not a BMad implementation spec yet)  
**Date:** 2026-11-13  
**Language of this note:** French (decisions + UI labels stay English in product)

This document freezes the **Designer-side** model discussed on 2026-11-13, after the product zone wiring already shipped in Matrix-Control. Use it to start the later **GUI Zone / affectation** chantier.

---

## Related documents (already existed)

| Document | Role |
|----------|------|
| Dropbox `Documents/Plans/Vision-Skin-Editor-pro-cascade-tiroirs-zones.md` | Long-term north star (7 Nov 2026): drawers vs zones vs cascade. Unversioned SSOT across macOS/Windows/Linux (Dropbox Matrix-Control mirror). |
| Dropbox `Documents/Plans/Preparation-assainissement-zones-tiroirs-Black-Duo.md` | Frozen UCS/MCS matrix used for the product Build. Same Dropbox mirror. |

Dropbox root used by Guillaume: `/Users/Guillaume/Library/CloudStorage/Dropbox/Dev/Projects/JUCE/Matrix-Control/`
| `_bmad-output/implementation-artifacts/spec-zones-skins-black-duo-cream.md` | **Done** — product inventory + call-site wiring Patch / Master / Header / Modales. Explicitly **out of scope**: Skin Designer pro UI, GUI Zone column, cascade. |

This file **does not replace** the 7 Nov vision; it **narrows** it to the concrete Zones panel UX and ownership rules agreed later.

---

## Problem

Mappings today answers: *for this object type, in drawer `csX`, which pastille?*  
Colour set + filter + sort + Link + Hold are the right tools for that job.

What is missing: *in the product GUI, which drawer of which object does geography X read?*  
(Patch / Master / Header / Modales / Shared presentation of UCS.)

Do **not** brutally rename or replace the Colour set column with “GUI Zone”. That would destroy a useful drawer-centric workflow and recreate the old label lie under another name.

---

## Two jobs, two surfaces

| Surface | Job |
|---------|-----|
| **Mappings** (keep as-is) | Paint drawers: pastilles, Link, Hold, Colour set filter/sort |
| **Zones** (new panel) | Wire geography → `(object type, csN)` |

Daily flow:

1. Paint `cs1…csN` in Mappings.  
2. Assign zones in Zones.  
3. Handle named exceptions (later editable).  
4. Return to Mappings if a drawer tint is wrong.

---

## Colour sets (`csX`) — model rules

- `cs1…csN` are **drawers / colour variants per object type**, count fixed by Matrix-Control inventory.
- **Never** rename `cs1` → `Patch` (or any zone) in JSON or as the technical identity.
- **Never** presuppose that a given `cs` index is permanently tied to a zone.
- Zone assignment is a **separate map** and may cross freely, e.g.:

```text
Colour sets (Mappings)
  Slider  cs1=green  cs2=cyan  cs3=magenta  cs4=amber
  Combo   cs1=violet cs2=fuchsia cs3=beige   (± cs4 only if inventory allows)

Zones (Zones panel)
  Patch   → Slider/cs1 , Combo/cs4
  Master  → Slider/cs2 , Combo/cs3
  Header  → Slider/cs3 , Combo/cs2
  Modales → Slider/cs4 , Combo/cs1
```

Drawer **count** stays code-imposed (today: Slider 4; ComboBox + PopupMenu 3; Led + PeakIndicator 2; UCS = 1).

Optional Colour set help labels in Mappings may describe drawer roles (“Accent A”, “Mono”) but must not pretend to be the geographic SSOT.

---

## Zones panel — placement

Third work surface beside Palette and Mappings:

```text
[ Palette ] [ Mappings ] [ Zones ]
```

Same useful width as Mappings. Not a floating inspector-only toy.

---

## Zones panel — layout (sketch)

```text
┌─ ZONES ─────────────────────────────────────────────────────────┐
│ Skin: <active designer skin>    [ Reset to product defaults ]   │
│                                                                  │
│ ▼ Object → zone matrix                                           │
│ ┌──────────────┬───────┬────────┬────────┬─────────┐            │
│ │ Object       │ Patch │ Master │ Header │ Modales │            │
│ ├──────────────┼───────┼────────┼────────┼─────────┤            │
│ │ Slider       │ cs▾   │ cs▾    │ cs▾    │ cs▾     │            │
│ │ ComboBox     │ cs▾   │ cs▾    │   —    │ cs▾     │            │
│ │ PopupMenu    │ cs▾   │ cs▾    │   —    │ cs▾     │            │
│ │ Led          │   —   │   —    │ cs▾    │ cs▾     │            │
│ │ PeakIndicator│   —   │   —    │ cs▾    │ cs▾     │            │
│ └──────────────┴───────┴────────┴────────┴─────────┘            │
│                                                                  │
│ ▼ Shared / single-set objects (UCS)                              │
│   Read-only list imposed by product inventory                    │
│                                                                  │
│ ▼ Exceptions (overrides)                                         │
│   Where · Object · Uses (drawer)                                 │
│                                                                  │
│ Hint: Paint drawers in Mappings · Assign geography here          │
└──────────────────────────────────────────────────────────────────┘
```

### Matrix rules

1. One row = one **multi-set** object type (not one row per property).  
2. One column = one product zone: Patch, Master, Header, Modales.  
3. Grey `—` = zone not used for that object in the product matrix.  
4. Combo choices = only drawers that exist for that object.  
5. Changing a combo changes **assignment**, not the pastille painted in Mappings.  
6. Prefer **combos** for this matrix (few cells, must be glanceable). Do **not** reuse Palette→Mappings click-bind as the primary zone gesture.

### Shared / UCS block

**Owner: Matrix-Control source inventory** (not the skin author).

- UCS object types are imposed by code (`colourSetCount` / multi-set lists).  
- The Designer **displays** them as Shared / single-set; the author does **not** tick “this type is Shared”.  
- HeaderPanel / Dialog = **chrome** of Header / Modales (one surface each), not “a control inside Patch”.  
- **Recommendation:** no empty Shared column on every MCS row; keep Shared as this dedicated UCS block.  
- Footer: no MCS zone column for now (align with shipped product matrix).

### Exceptions block

**Today / V1: Matrix-Control code imposes the list** (Computer Patches, logo menu, Header Undo/Redo/Panic, …).

| Phase | Behaviour |
|-------|-----------|
| **V1** | Read-only mirror of the product registry (honest UI). |
| **V2** | Editable only when runtime consumes overrides from data (skin JSON or zone-map document). |

Do not let authors invent exceptions the plugin ignores.

Each row: **Where** (product place) · **Object** · **Uses** (which drawer).  
This is the seed of cascade without opening Module/Instance everywhere yet.

### Interactions (V1)

| Action | Behaviour |
|--------|-----------|
| Change matrix combo | Update zone→drawer map; live Compare/Standalone follow when wired |
| Click object name | Optional: jump to Mappings filtered on that object |
| Hover cell | Short reminder + optional dominant pastille of that drawer |
| Reset to product defaults | Restore code registry matrix + exceptions |
| Exception edit | V1: none (read-only) |

---

## Cascade (later)

```text
V1  Zones panel     → Object × Zone defaults + read-only Exceptions
V2  + Module        → named module overrides (same panel section)
V3  + Instance      → live GUI pick / inspector; click-bind may help here
```

Homogeneity: set 95% at Zone level; descend only for real exceptions.

---

## Explicit non-goals (this vision)

- Replacing the Colour set column with GUI Zone.  
- Adding a Zone column on every Mappings row in V1.  
- Making zone assignment use the same gesture as pastille mapping.  
- Implementing full cascade before a readable Zone matrix exists.  
- Renaming technical `cs` indices to zone names in JSON.

---

## Suggested chantier order (project)

1. **Palette distincte par skin** (Designer + ownership / naming) — first.  
2. **Zones panel** (this document) — second, on product matrix already shipped.  
3. Cascade Module / Instance — later.  
4. Optional trial: PhotoCraft tint → measure → Trials JSON (TONES family) — parallel experiment, not a blocker for (1)/(2).

---

## Checklist — day we start the Zones chantier

- [ ] Recover `_local` vision + preparation docs if still needed for history.  
- [ ] Confirm product registry export surface (what the Designer reads for matrix + exceptions).  
- [ ] Decide V1 exceptions read-only vs blocked until runtime JSON overrides.  
- [ ] Spec BMad: Zones panel UI + data model + tests; Mappings untouched except optional jump link.  
- [ ] Branch dedicated (habit): e.g. `feature/skin-designer-zones-panel`.  
- [ ] Manual smoke: paint drawers in Mappings, rewire zones, verify Standalone geography.
