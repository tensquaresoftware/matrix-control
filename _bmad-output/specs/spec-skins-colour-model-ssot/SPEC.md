---
id: SPEC-skins-colour-model-ssot
companions:
  - brownfield.md
  - target-model.md
  - migration-plan.md
  - code-map.md
  - ../../../CONVENTIONS.md
  - ../../project-context.md
sources:
  - ../../forge/mode-dev-skins/forged-idea.md
  - ../../../Documentation/Devs/Plans/2026/10/2026-10-14-Matrix-Control-Skin-Studio-Brief.md
  - /Volumes/Guillaume/Dev/Projects/JUCE/Matrix-Control-Skin-Studio/_bmad-output/planning-artifacts/architecture/architecture-Matrix-Control-Skin-Studio-2026-10-15/ARCHITECTURE-SPINE.md
  - /Volumes/Guillaume/Dev/Projects/JUCE/Matrix-Control-Skin-Studio/_bmad-output/planning-artifacts/prds/prd-Matrix-Control-Skin-Studio-2026-10-15/prd.md
---

> **Canonical contract.** This SPEC and the files in `companions:` are the complete, preservation-validated contract for what to build, test, and validate. Source documents listed in frontmatter are for traceability — consult them only if you need narrative rationale or prose color this contract intentionally omits.

# Skins colour model SSOT (V1 user GUI gate)

## Why

Matrix-Control already ships Black and Cream skins that are good enough for alpha / a shippable V1, but the colour path mixes a real palette chart, flat token maps, Look injection, and leftover local hex/alpha shortcuts on dialogs, chrome, and fields. Skins are the last-mile V1 surface before multi-OS alpha and Christmas 2026; an authoring editor cannot fix a dirty model. This mandate closes the **model** gate by **2026-11-15** on a dedicated branch — closed palette, **structured** object × property × state mappings, zero local colour on the V1 user GUI path — so a later editor and proud Black/Cream polish sit on a clean, Studio-aligned contract. Missing the date means total park (no partial SSOT merge), not a stretched alpha.

## Capabilities

- **CAP-1**
  - **intent:** The product keeps a single closed named palette as the only durable colour source for the V1 user GUI.
  - **success:** On the gated path, every durable paint resolves to a named palette entry (`k` + shade or absolute); no ad-hoc hex or `juce::Colours::*` literals remain for those paints (palette file itself excepted). Palette names never encode UI role.

- **CAP-2**
  - **intent:** Every durable coloured slot on a gated UI element is addressed by a structured key `(ObjectType, Property, State)` that maps to one palette entry.
  - **success:** Inventory of gated slots uses structured keys as SSOT; Black and Cream maps are complete for those keys; flat `SkinColourId` is not the model authority (derived/internal only if kept). No `patch*` / `master*` object types.

- **CAP-3**
  - **intent:** Black and Cream each expose a complete key→palette mapping so skin switch changes appearance without widget edits.
  - **success:** Toggling Black ↔ Cream in Standalone recolours the gated GUI solely from those maps (no zone left on hard-coded colours).

- **CAP-4**
  - **intent:** Generic widgets and surface panels receive colours via construction-time Looks injected by builders/orchestrators that know product zones (Patch/Master, Blue/Orange accents).
  - **success:** Gated paint uses Look/skin structured getters only; widgets do not embed product-zone colour knowledge or local durable colour constants. Button-like (and similar) = separate object type.

- **CAP-5**
  - **intent:** Durable attenuation that today fakes a second colour via alpha on opaque UI becomes a real palette-mapped solid; true transient effects stay code-owned.
  - **success:** PatchNameDisplay secondary mutator labels (e.g. Mxx-Ryy) use a dedicated solid key→palette mapping (no secondary `withAlpha` shortcut). LEDs use a solid max-brightness palette colour with code-modulated alpha toward off. Lock dimming film and close-cross hover pale remain code-owned. Other opaque alpha-as-darken shortcuts on the gated path are replaced by solids per `migration-plan.md`.

- **CAP-6**
  - **intent:** By 2026-11-15 the dedicated branch delivers a functional model gate: structured SSOT + good-enough Black/Cream parity across the full V1 user GUI with zero local colour on that path.
  - **success:** Checklist in `migration-plan.md` passes, including the automated gated-path colour scan; Tests and Dev-tool paths are excluded; if incomplete by the date, work is parked entirely (no shrink merge of a half-flat / half-structured model).

## Constraints

- Scope is **model refactor in Matrix-Control only** — no Skin Studio HTML, no in-app/remote author editor, no IPC telecommand app in this contract.
- **Deadline 2026-11-15**: ready and functional on a **dedicated branch**, or **total park** (no partial SSOT into V1, no eating the alpha window). Alpha may continue on current trunk skins. Resume 2027 if parked.
- Gate surface = **entire V1 user GUI**: Patch Edit, header/footer chrome, dialogs, Settings, wizards (About treated as user chrome). **Excluded:** `Source/GUI/Tests/**`, `Tests/**`, Dev-only tooling.
- Migration parity is **good-enough** recognisable Black/Cream; micro-gaps OK if SSOT-coherent (including unifying already-divergent borders). No pixel-identity mandate vs trunk or vs any HTML mock.
- **Structured keys are in-scope for this gate** (not deferred after a flat-only migration). Migrate from today’s `ColourChart` / `SkinColourId` / `SkinColours*` / `LookBuilders` brownfield — do not invent a second parallel colour system beside them.
- Widgets stay **generic**; Patch/Master and Blue/Orange accents are **injection only** (`target-model.md`).
- Visual truth is **JUCE Matrix-Control paint**, not browser fidelity.
- **Orbitron** = brand font only; **PatchNameDisplay** stays PT Sans Narrow + chart red (`kRed500`) — do not invent product typography.
- Palette naming per `target-model.md` (`kEmerald*`, `kGrey*`, `kRed500`, absolutes); own hex; Tailwind vocabulary only, not Tailwind hex.
- English-only source and UI strings; JUCE 9.0.3; quality gate via `Scripts/quality/lint_touched.py` on touched C++; follow `CONVENTIONS.md` / `project-context.md`.
- Do not introduce `#if DEV` colour literals on the V1 user path as temporary debt “until the editor”.
- Christmas plan is adaptive outside this gate: model-first; editor/proud polish only if speed allows after CAP-6.
- **Alpha policy (locked):** LEDs = solid max brightness in palette + runtime alpha dim; lock film and close-cross hover dim = code-owned; PatchNameDisplay secondary mutator/Compare labels = solid mapped colour. Detail: `migration-plan.md`.
- **Gate proof (locked):** a small automated check fails the gate on residual local hex / `juce::Colours::*` durable paints under gated `Source/GUI` (excluding Tests / Dev tools). Detail: `migration-plan.md` / `code-map.md`.

## Non-goals

- Author colour editor (in-app compile-flag panel or remote IPC app) and any choice between those forms.
- Skin Studio Lot C / pixel-near HTML GUI; double-maintenance HTML↔JUCE for V1; shipping five Studio seed skins in this gate.
- Aesthetic “proud” recalibration of Black contrasts or Cream beyond good-enough migration unify.
- New skins (Duo, Monochrome, contributor skins) or external Dev contributor distribution.
- Shrink merge of a partial SSOT into V1 if the deadline is missed.
- Requiring zero local colour under Tests / Dev tools to pass the gate.
- Running `bmad-build`, commits, push, or PR from this spec alone.
- Formal planning rewrite for Skin Studio freeze (`bmad-correct-course`) for the Matrix-Control PRD — not required: PRD/epics do not assume Lot C (only the historical Skin Studio brief still describes the old Studio path).
- Snapping Matrix-Control hex values to Tailwind default stops.

## Success signal

On the dedicated branch by 2026-11-15, a reviewer opens Standalone, switches Black ↔ Cream, and walks Patch Edit, chrome, a Matrix modal, Settings, and the Getting Started wizard: every durable colour comes from structured key→palette maps and Looks, nothing looks half-migrated, and the **automated** gated-path colour check is green. Tests/Dev tools may still hardcode colours. If that bar is not met, the branch is parked — current Black/Cream on trunk remain the alpha/V1 safety net.

## Assumptions

- `LookBuilders` + `WidgetLooks` remain the injection layer; they switch to resolving structured keys.
- About window and modal chrome are in the V1 user GUI gate (same class as Settings/dialogs).
- Primary PatchNameDisplay idle/hover alphas (editability affordance on the main name line) stay paint-owned unless a later decision maps them; only the secondary mutator labels were locked to solids.
- Exact C++ encoding of keys (enums + lookup table vs generated flat ids) is an implementation choice so long as `(ObjectType, Property, State)` is the SSOT address.
