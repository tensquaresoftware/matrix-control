---
title: 'Skin Designer — palette owned per skin'
type: 'feature'
created: '2026-11-13'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'c3d52fe06a3aed7a9a0c839350f21d77de932b95'
context:
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Every Dev/compiled skin is seeded with the full shared ColourChart catalogue, so the Skin Designer palette becomes an unreadable union (Emerald vs Green name collisions, unused pastilles from other skins). Authors and future skin designers need each skin to own a short, local palette.

**Approach:** Keep `NamedSkin.palette` as the SSOT (already per-skin in JSON). Stop injecting a global chart union into every skin; seed and display only that skin’s pastilles; make naming / product-lock rules match skin ownership. Restructure product colour storage so each product family can own colours without a single flat project-wide catalogue forcing artificial names.

**Decisions (Guillaume, 2026-11-13):**
- **Depth:** Restructure / split the product colour chart (`ColourChart.h` and consumers) in this wave so Matrix-Control and Skin Designer share a coherent per-skin (or per-family) ownership model — not Designer-only cosmetics.
- **Trials JSON:** One-shot migration for this wave. Trials currently holds only a disposable `TEST` skin — safe to delete / replace during migration.
- **Starter kit** (new Designer skin): pastilles required by mappings after duplicate/create, plus fixed kit entries `BrandCream`, `BrandBlue`, `BrandOrange`, `Black`, `White` (ColourChart symbols `kBrandCream`, `kBrandBlue`, `kBrandOrange`, `kBlack`, `kWhite`). Not the full catalogue.
- **Product lock:** On Designer (Dev) skins, all pastilles are author-owned — rename/delete allowed (mapped delete still follows existing removeRefs / refuse policy). No global ColourChart reserved-name lock. Drop the Product vs Designer palette split in the UI (single palette list). Compiled product skins stay read-only.

## Boundaries & Constraints

**Always:**
- Mappings continue to store pastille **names** resolved against the **active skin’s** palette only.
- Same pastille name may map to different ARGB in different skins (skin-local namespace).
- Compiled product skins remain read-only in the Designer; Duplicate → Dev skin remains the edit path.
- PushPayload / Compare continue to send the active skin’s palette + mappings.
- ASCII UI chrome; Clean Code gate includes `Tools/SkinDesigner/`.
- Zones panel / GUI Zone assignment stay out of this chantier (see plan doc).
- Starter kit for new Designer skins = mapped pastilles + BrandCream / BrandBlue / BrandOrange / Black / White.
- Trials migration is one-shot; disposable TEST skin may be removed.
- Dev-skin pastilles: author-owned rename/delete; no `isProductPastilleName` global lock.

**Never:**
- Renaming technical `csX` to zone names; Zones panel work.
- Skin menu hierarchy BLACK/CREAM/TONES (separate later).
- Silent data loss of mapped pastilles; deleting a pastille still requires clear ref policy.
- Keeping a Designer UX that still presents one project-wide union catalogue as the skin’s palette.
- Keeping a permanent “Product colours” rename/delete lock sourced from a global chart.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Switch Designer skin | Skin A slim palette → Skin B | Palette panel lists only B’s pastilles | N/A |
| Bind mapping | Pastille name on skin A | Resolves A’s ARGB; unknown name rejected | Fail bind; leave unbound |
| Same name, two skins | `kGreen400` different ARGB on A vs B | Each skin keeps its own ARGB | N/A |
| New Designer skin | Duplicate / blank-start path | Mapped pastilles + BrandCream/Blue/Orange + Black + White | N/A |
| Rename / delete Dev pastille | Any name on Dev skin | Rename OK; delete OK if unmapped or with removeRefs | Refuse unsafe mapped delete per existing policy |
| Trials migration | File with disposable TEST only | One-shot cleanup / replace; TEST may be deleted | Must not invent mapped loss |

</frozen-after-approval>
## Code Map

- `Source/GUI/Skins/Dev/DevSkinTypes.h` — `Pastille`, `NamedSkin.palette` (already per-skin); PushPayload.palette.
- `Source/GUI/Skins/Dev/DevSkinPaletteCatalog.{h,cpp}` — `colourChartPastilles()` global union; `pastilleNameForArgb` / `argbForPastilleName`.
- `Source/GUI/Skins/Dev/DevSkinSsotSeed.cpp` — `seedFromMinimalEntries` / `seedFromCompiled` assign `skin.palette = colourChartPastilles()` (root cause of fat palettes).
- `Source/GUI/Skins/Dev/DevSkinLayer.cpp` — `isProductPastilleName` scans global chart; export/push uses skin palette.
- `Source/GUI/Skins/Dev/DevSkinLayerPastille.cpp` — add/rename/delete/setArgb; product rename/delete guards; ARGB edit allowed on product-named rows today.
- `Source/GUI/Skins/Dev/DevSkinLayerCreate.cpp` — `appendCustomPastillesFrom` skips “product” names via global lock; new-skin merge behaviour.
- `Source/GUI/Skins/Dev/DevSkinJson.cpp` — palette array round-trip per skin (no model change required for ownership).
- `Tools/SkinDesigner/SkinDesignerPalettePanel.cpp` — lists `skin->palette` only, splits Product vs Designer via `isProductPastilleName`.
- `Tools/SkinDesigner/SkinDesignerMainComponentActions.cpp` — add/duplicate/rename/delete pastille; blocks product names.
- `Tools/SkinDesigner/SkinDesignerUiHelpers.h` — unique name / Temp N helpers scoped to one skin already.
- `Source/GUI/Skins/ColourChart.h` + `SkinColours*.h` — **reuse as compiled paint SSOT**; do not restructure unless Depth=C.
- Tests: `Tests/Unit/DevSkinLayerJsonTests.cpp`, `DevSkinPendingPastilleTests.cpp`, `DevSkinMatrixTests.cpp`, palette UI helpers as needed.

**Reuse:** per-skin palette vector + JSON I/O; Designer already switches list with `setActiveSkinId`.

**Do not change:** Zones panel; skin menu taxonomy; Compare protocol shape beyond palette contents; runtime LookBuilders.

## Tasks & Acceptance

**Execution:**
- [x] `DevSkinSsotSeed.cpp` (+ helpers) -- seed each skin palette from used pastilles / approved starter rule, not full `colourChartPastilles()` union -- stop fat defaults.
- [x] `DevSkinLayer.cpp` / `DevSkinLayerPastille.cpp` / `DevSkinLayerCreate.cpp` -- remove global product-name lock on Dev skins; create/merge/starter kit rules -- author-owned palettes.
- [x] `ColourChart.h` (+ consumers) / `DevSkinPaletteCatalog.*` -- restructure chart ownership per Depth decision; stop treating one flat union as Designer catalogue of record -- per-skin/family coherence.
- [x] `SkinDesignerPalettePanel.*` + Actions -- single palette list (no Product/Designer split); rename/delete enabled on Dev -- honest UI.
- [x] Trials one-shot migration -- delete disposable TEST / align file to new palette rules -- no mapped loss.
- [x] `Tests/Unit/DevSkin*` (+ Designer helpers if pure) -- pin slim seed, skin-local same-name ARGB, bind unknown pastille, delete/rename policy -- I/O matrix.

**Acceptance Criteria:**
- Given two Designer skins, when each defines `kGreen400` with different ARGB, then switching skins shows the correct swatch and bindings resolve per active skin.
- Given a newly created Designer skin (post-change), when opening Palette, then the list is not the full ColourChart union (matches starter rule).
- Given a mapping to pastille X, when X is absent from that skin’s palette, then bind/apply fails closed (no silent fallback to another skin’s palette).
- Given Export/Import of a Designer skin, when round-tripped, then palette names+ARGB and mappings match.

## Implementation Notes

- Split `ColourChart.h` into Shared / Green / Chroma / Cafe headers; facade keeps `ColourChart::k*` call sites.
- Compiled/Dev seeds no longer assign full `colourChartPastilles()`; pastilles are added as mappings resolve (`ensurePastilleOnSkin`).
- Duplicate with mappings: prune to mapped names + ensure starter kit; unbound New: starter kit only; merge copies all donor pastilles.
- `isProductPastilleName` always false; Palette UI single "Palette" section.
- Trials Dropbox JSON: backed up then emptied (`skins: []`, `activeSkinId: compiled:cream`); TEST removed.
- Tests updated to `ensureChartPastilleOnSkin` before binding chart names absent from slim palettes.

## Spec Change Log

## Review Triage Log

- false — Blind: duplicate prune drops unmapped colours — intentional (mapped + starter kit); covered by `assertDuplicatePrunesUnmapped`.
- defer — Blind: JSON Load does not auto-prune fat unions — Trials one-shot chosen; deferred-work entry.
- low/patch — Blind+VG: missing starter-kit / prune / chart rename-delete tests — added; comments cleaned.
- medium/patch — Edge: `ensurePastilleOnSkin` case-sensitive — fixed with `equalsIgnoreCase`.
- false — Edge: New+merge exceeds starter — merge of donor pastilles is intentional after product-lock removal.
- defer — Blind: hollow `isProductPastilleName` / `Row::isProduct` — deferred cleanup.
- false — VG title mismatch on delete test — body now deletes chart-named `kGreen100` when unmapped.

## Design Notes

Today the Designer already shows `skin->palette` only — the bug is **contents**, not the list widget. `isProductPastilleName` → global `colourChartPastilles()` is why Emerald/Green cannot share a simple name across skins and why “Product colours” stays a project-wide cast.

All intent questions closed: Depth = restructure ColourChart; Trials = one-shot (TEST disposable); Lock = Dev author-owned (no global product lock); Starter = mapped + BrandCream/Blue/Orange + Black/White.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` then run DevSkin* / SkinDesigner* palette-related tests -- expected: pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched `Source/` + `Tools/SkinDesigner/` + `Tests/`

**Manual checks:**
- Open Trials / Designer: switch BLACK/GREEN vs CREAM/GREEN — palette length and names stay skin-specific; Compare still paints.
