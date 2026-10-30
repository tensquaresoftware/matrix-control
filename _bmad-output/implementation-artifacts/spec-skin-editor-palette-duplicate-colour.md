---
title: 'Skin Editor Palette: Duplicate colour + same-RGB mapping guard'
type: 'feature'
created: '2026-10-30'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '136f316230106878a1f57cc794e127f6762ea6e6'
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Skin Editor Palette has no way to clone a Product or Editor colour into a tweakable Editor variant. Creating near-twins by hand is slow. Allowing identical RGB in the palette is useful for Duplicate-then-adjust, but mapping two differently named pastilles that share the same RGB makes Palette/Mappings hard to read by eye.

**Approach:** Add Palette **Duplicate** (with **New** / **Del** relabel for fit). Duplicate copies name stem + ARGB into a new non-pending Editor pastille and selects it. Relax palette ARGB uniqueness so twins can exist. **Hard-block mapping** when the candidate pastille's RGB is already used by a *different* pastille that is already mapped; surface that in OPERATION with an explicit red badge and coaching (no export-time consolidation).

## Boundaries & Constraints

**Always:**
- Duplicate requires a selected colour (Product or Editor); otherwise the button is disabled. Requires an editable Editor skin (same gate as New/Delete).
- Copy name via unique stem like skins: preferred free, else `"Name 2"`, `"Name 3"`, … (case-insensitive clash with existing palette names). No rename modal.
- New pastille lands at end of Editor colours, `pending = false` (filled square), same ARGB as source, auto-selected and synced to COLOR.
- New (ex-Add) behaviour unchanged: unique `Temp N` + unique random opaque ARGB + `pending = true`.
- Delete stays Editor-only (Product selection keeps Del disabled); label abbreviated to **Del**.
- Mapping guard: block bind when another pastille with the same ARGB already appears in `mappings` values. Own name does not count as a collision.
- On block: OPERATION shows red badge **`SAME COLOUR`** and next-step coaching; also set status-bar message (mirror pending calibrate path). ASCII only.
- Proposed copy (ASCII): next-step `Next: adjust in COLOR before mapping (same RGB already mapped).` ; status `Adjust in COLOR: this RGB is already mapped under another name.`
- Red fill token for the badge: centralize Compare's `0xffff3b3b` as a chrome danger/error colour (reuse that RGB).

**Never:**
- No export/import JSON consolidation modal or remapping surgery for RGB twins.
- No mapping of Product pastilles into a second Product entry; Duplicate always creates an **Editor** pastille.
- Do not copy mappings onto the new pastille.
- Do not change plugin runtime skin apply semantics (still name → ARGB).
- No keyboard shortcut for Duplicate in this story.
- Do not force-translate product UI labels; keep New / Duplicate / Del / SAME COLOUR in English.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Duplicate Product colour | Editor skin editable; Product pastille selected | New Editor pastille, unique stem name, same ARGB, not pending, selected | Layer add failure → alert; selection restored |
| Duplicate Editor colour | Editor pastille selected | Same as above from that source | Same |
| Duplicate disabled | No selection, or no editable Editor skin, or empty shell | Button disabled / hidden with shell | N/A |
| Map first of two same-RGB pastilles | Twin exists unmapped; candidate not colliding with any *mapped* other | Bind succeeds | N/A |
| Map second same-RGB while first mapped | Other pastille same ARGB already in mappings | Bind refused; OPERATION red `SAME COLOUR` + coaching; status message | No AlertWindow for this guard |
| Adjust copy RGB then map | Copy ARGB differs from all mapped others | Bind succeeds | N/A |
| addPastille / setPastilleArgb same ARGB as existing | Two names, one ARGB | Allowed (uniqueness removed); mapping guard remains the safety net | Name clash still rejected |

</frozen-after-approval>

## Code Map

- `Tools/SkinEditor/SkinEditorPalettePanel.h|.cpp` -- Add/Delete → New/Duplicate/Del; three-button layout (measure widths like Skins); enable Duplicate on any colour selection when editable; keep Del Product-blocked.
- `Tools/SkinEditor/SkinEditorMainComponent.cpp|.h` + `SkinEditorMainComponentActions.cpp` -- wire `onDuplicatePastille`; implement clone flow (name helper, copy ARGB, `pending=false`, select + `syncColourPickerToSelection`).
- `Tools/SkinEditor/SkinEditorUiHelpers.h` -- `suggestUniquePastilleName(skin, preferred)` mirroring `suggestUniqueSkinName`; `isPastilleArgbMappedUnderOtherName` collision probe; keep Temp helper for New.
- `Source/GUI/Skins/Dev/DevSkinLayerPastille.cpp` -- remove ARGB-uniqueness rejects from `addPastille` and `setPastilleArgb` (keep name uniqueness / read-only compiled guards).
- `Tools/SkinEditor/SkinEditorMappingModePolicy.h` -- pure helpers/strings for same-RGB mapped collision + OPERATION copy; extend `rowClick` / `pastilleClick` like pending (`kIgnore` / enter-held instead of bind).
- `Tools/SkinEditor/SkinEditorMainComponentMapping.cpp` -- detect collision against editor skin mappings; applyMapping + row-activated status; witness paint uses red `SAME COLOUR` badge instead of muted `MAPPING` when held colour collides.
- `Tools/SkinEditor/SkinEditorChromeTokens.h` (+ badge draw call sites) -- danger colour token from Compare red.
- `Tests/Unit/DevSkinPendingPastilleTests.cpp` -- replace ARGB-collision expectations with allow-same-ARGB coverage.
- `Tests/Unit/SkinEditorMappingModePolicyTests.cpp` -- collision bind policy + copy constants.
- `Tests/Unit/SkinEditorChromeLayoutTests.cpp` -- only if New/Duplicate/Del geometry assertions are needed; avoid brittle pixel locks unless existing style requires it.
- Do **not** change: export consolidation, DevSkin JSON schema, plugin `DevSkinApplier` name→ARGB resolve, Skins panel Duplicate flow.

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Skins/Dev/DevSkinLayerPastille.cpp` + `Tests/Unit/DevSkinPendingPastilleTests.cpp` -- allow same ARGB on add/set; update unit tests -- palette twins for Duplicate
- [x] `Tools/SkinEditor/SkinEditorUiHelpers.h` -- unique pastille name from preferred stem -- Duplicate naming
- [x] `Tools/SkinEditor/SkinEditorPalettePanel.*` + MainComponent wire/actions -- New/Duplicate/Del UI + `onDuplicatePastille` -- Palette CRUD parity with intent
- [x] `Tools/SkinEditor/SkinEditorMappingModePolicy.h` + `SkinEditorMainComponentMapping.cpp` + chrome token -- mapping block + red OPERATION badge/coaching + status -- authoring guard
- [x] `Tests/Unit/SkinEditorMappingModePolicyTests.cpp` (+ layout tests only if warranted) -- lock policy/copy -- regression net
- [x] Register any new test `.cpp` in `Tests/CMakeLists.txt` if added; run lint on touched Skin Editor / tests paths

**Acceptance Criteria:**
- Given an editable Editor skin and a selected Product or Editor colour, when Duplicate is clicked, then a new non-pending Editor pastille appears with a unique stem-based name, the same ARGB, selection on the copy, and COLOR synced.
- Given no colour selected (or read-only / empty shell), when the Palette action row is shown, then Duplicate is disabled or hidden appropriately; New/Del labels match intent.
- Given pastille B shares ARGB with already-mapped pastille A, when the user attempts to map B, then the mapping is not written and OPERATION shows red `SAME COLOUR` with the agreed coaching; status bar carries the agreed message.
- Given B's ARGB is then changed so it no longer matches any other mapped pastille's ARGB, when mapping B is attempted, then bind succeeds.
- Given two pastilles share ARGB and neither is mapped, when the first is mapped, then bind succeeds.

### Review Findings

- [x] [Review][Defer] COLOR can still align two already-mapped pastilles on one ARGB [`Source/GUI/Skins/Dev/DevSkinLayerPastille.cpp:58`] — deferred: frozen intent gates bind only; already in deferred-work; Color-side refuse needs product decision
- [x] [Review][Defer] Same-ARGB mapped probe uses case-sensitive name equality [`Tools/SkinEditor/SkinEditorUiHelpers.h:107`] — deferred: already in deferred-work; settle with a case-variant mapping fixture (chart names stay exact-case today)

Rejected:
- false — older add-pending spec still requires ARGB uniqueness: historical artifact, not a runtime defect of this diff; fixing would edit another spec
- false — SAME COLOUR path omits Hold-colour suffix: `withHoldColourSuffix` is for bind gestures only; frozen SAME COLOUR copy is intentional
- false — Duplicate leaves selection off-screen: `selectPastille` → `ListBox::selectRow` scrolls by default after refresh
- false — coaching does not name the colliding twin: frozen proposed copy is fixed; changing it would renegotiate the spec
- false — JSON does not prove twin persistence: export/import walks every palette entry with no ARGB uniqueness filter; twins already covered in layer/merge tests
- low (rejected) — RGB vs ARGB coaching wording: cosmetic; symbol rename churn without user-facing gain
- low (rejected) — no unit test Duplicate-from-pending → `pending=false`: production hardcodes `false`; outcome already asserted on calibrated/Product clone paths
- low (rejected) — `kPalettePanelWidth` floor-checked with `>=` not `==` preferred: width locked at 183; leftover stretch on font shrink is cosmetic

## Implementation Notes

Implemented 2026-10-30: palette New/Duplicate/Del; ARGB twins allowed in layer; bind-time same-RGB mapped guard with red SAME COLOUR OPERATION badge; Compare blink uses `SkinEditorChrome::danger()`. Collision probe extracted to `SkinEditorUi::isPastilleArgbMappedUnderOtherName` with unit coverage (duplicate outcome + mapped twin). Review patches: New-skin merge keeps same-ARGB twins (name-only), armed-row blocked bind sets status, Name 3 + Product clone tests. Deferred: Color can still create two mapped same-ARGB pastilles; case-sensitive probe names. Manual UAT still recommended.

## Spec Change Log

## Review Triage Log

- false — Blind: empty Spec Change Log / Review Triage Log — process placeholders filled by this review pass; not a product defect.
- false — Blind: older add-pending spec still requires ARGB uniqueness — historical artifact disagreement is documentation debt, not a runtime bug in this diff; fixing by editing that old spec was rejected by triage rules.
- low (rejected) — Blind: RGB vs ARGB wording drift — cosmetic naming; renaming symbols would churn call sites without user-facing gain.
- low (rejected) — Blind: DevSkinPendingPastille suite now pulls mapping-policy headers — tidy-up only; everyday breakage not shown.
- low (rejected) — Blind: no SkinEditorChromeLayout assertions for New/Duplicate/Del geometry — spec Code Map deliberately avoided brittle layout locks; enablement is covered by panel logic + manual checks.
- low (rejected) — Blind: onDuplicatePastille ~55-line scan — under GUI glue budget; extract helper later if a third caller appears.
- low (rejected) — Blind: no MainComponent paint test for red SAME COLOUR badge — policy strings + probe covered; GUI paint harness out of style.
- maybe-false (deferred) — Blind: case-sensitive `name ==` in probe vs equalsIgnoreCase uniqueness — would need a fixture with case-variant mapping strings to settle; chart names stay exact-case today.
- medium → patch — Blind/Verification: `appendCustomPastillesFrom` still drops same-ARGB custom pastilles on New Editor merge — twins from Duplicate disappear when creating a new Editor skin from the current one; match-by-name only.
- medium → patch — Edge: armed-row blocked bind only calls `enterHeldColour` with no `setStatusMessage` — AC requires status bar on map attempt; row path sets it, armed pastille click does not.
- medium → patch — Verification Gap: `suggestUniquePastilleName` never asserts the `"Name 3"` path — second Duplicate of same source could regress silently.
- medium → defer — Edge: `setPastilleArgb` can still make two *already mapped* pastilles share one ARGB via COLOR — approved frozen intent gates bind only and explicitly allows same-ARGB on set; Color-side refuse is a follow-up product choice, not a bind-guard bug.
- low → patch — Blind: Product-source Duplicate outcome not unit-covered — add one assert that a Product chart ARGB cloned under a stem name is non-product and not pending.

## Design Notes

Palette may hold RGB twins; mappings stay name-keyed. The dangerous state is **two mapped names, one RGB**. Guard at bind time (proactive OPERATION coaching while holding a colliding colour, same shape as pending calibrate). Export cleanup was rejected as too late for the design session and too heavy.

Duplicate intentionally does **not** nudge ARGB; New keeps suggesting unique random ARGB so everyday adds stay distinct until the user duplicates on purpose.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-ControlSkinEditor Matrix-ControlTests` -- Skin Editor + tests link
- `ctest --preset macos-debug-arm64 -R 'DevSkinPendingPastille|SkinEditorMappingModePolicy|SkinEditorChromeLayout' --output-on-failure` -- unit nets green (adjust regex if targets differ)
- `python3 Scripts/quality/lint_touched.py` -- Clean Code gate on touched C++ under `Tools/SkinEditor/`, `Source/`, `Tests/`

**Manual checks:**
- Duplicate Product and Editor colours; confirm square swatch, name stem, COLOR selection.
- Map original, Duplicate, try map copy without adjusting → red `SAME COLOUR` in OPERATION, no new mapping; adjust copy → map OK.
- Confirm New still creates pending Temp circle; Del still blocked on Product.
