---
title: 'Skin Editor OPERATION MAPPING badge red for pending Temp colour'
type: 'feature'
created: '2026-10-31'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-editor-bridge-badge-disconnected-red.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-editor-palette-add-pending.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** When the operator holds a new Editor colour that is still pending (Temp name, empty round pastille) and cannot be mapped until calibrated, the OPERATION MAPPING badge stays muted grey, so the blocked mapping state is easy to miss.

**Approach:** While the OPERATION mapping witness shows a pending held/edited colour (`mappingWitnessPending_`), paint the MAPPING badge with existing `SkinEditorChrome::danger()` red — same token as BRIDGE disconnected and SAME COLOUR. Keep the badge label `MAPPING` (do not rename). When the pastille is no longer pending, or mode is Idle / ArmedRow without a pending swatch, restore the muted grey MAPPING fill. Do not invent a new colour token; do not change bind policy or DevSkin pending semantics.

</frozen-after-approval>

## Implementation Notes

Agent decisions (planning):
- Gate on existing `mappingWitnessPending_` in `paintMappingWitnessDecorations` (set by `fillWitnessSwatchForSelectedPastille` for HeldColour; EditingColour also fills the swatch but pending+EditingColour is effectively unreachable because EditingColour is entered only from an already-mapped colour cell). ArmedRow leaves pending false → grey MAPPING.
- SAME COLOUR path already mutually exclusive with pending in `fillHeldColourWitness`; leave that branch unchanged (priority: sameColour → pending → muted).
- Use `drawChromeBadge(..., "MAPPING", danger())` like BRIDGE/SAME COLOUR — no new badge helper overload.
- No JSON / DevSkin model / MappingPolicy bind-rule changes; paint-only chrome cue.

Agent decisions (oneshot):
- Patched paint in `SkinEditorMainComponentMapping.cpp`; commented priority branch; softened `mappingWitnessPending_` / `danger()` docs.
- Touched: `SkinEditorMainComponentMapping.cpp`, `SkinEditorChromeTokens.h`, `SkinEditorMainComponent.h`.
- lint_touched.py OK; built `Matrix-Control_SkinEditor` (macos-debug-arm64).
- Manual smoke: New pending Temp → click pastille → OPERATION MAPPING red + round empty swatch; set a real ARGB → badge muted grey while still HeldColour; Esc → Idle clears witness; ArmedRow → grey MAPPING; non-pending same-RGB collision → SAME COLOUR red (unchanged).

## Review Triage Log

- Unnecessary `drawMappingModeBadge(fill)` overload vs BRIDGE `drawChromeBadge` — **medium**, patched (removed overload; pending uses `drawChromeBadge`).
- Mixed coloured-badge call styles — **medium**, same root as above, patched.
- Spec missing smoke / still in-progress — **low**, patched (smoke in Implementation Notes; status `done` at finalize).
- EditingColour+pending overclaimed — **false** as code bug (flag-gated paint is correct); notes clarify HeldColour is the real pending path.
- Red without renaming badge weaker than SAME COLOUR — **false** (Intent keeps label `MAPPING`).
- `mappingWitnessPending_` comment hard-coded chrome recipe — **low**, patched (state-meaning comment).
- No unit test locking fill priority — **low**, rejected (paint-only; same as BRIDGE-red oneshot).
- Spec notes disagreed on helper vs `drawChromeBadge` — **medium**, patched (notes + code use `drawChromeBadge`).
- `danger()` comment implied a distinct pending badge string — **low**, patched (wording: fill for pending MAPPING badge).
- Second colour-only signal without new vocabulary — **false** (Intent; calibrate next-step copy already exists).
