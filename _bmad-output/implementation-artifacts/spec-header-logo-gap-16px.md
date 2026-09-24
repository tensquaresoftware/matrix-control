---
title: 'Header logo gap 16px'
type: 'feature'
created: '2026-09-24'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The 20 px design gap after the header logo crowds the right-side UNDO/REDO cluster; Guillaume wants 4 px more breathing room for those buttons without moving them.

**Approach:** Reduce `Panels::Header::kLogoGapAfter` from 20 to 16 (still ÷4-grid compliant). Leave UNDO/REDO/PANIC placement and widths unchanged so plugin-mode right-anchored buttons stay put while left content starts 4 px earlier.

</frozen-after-approval>

## Implementation Notes

- Agent decision: change only the design constant; do not retarget standalone flow placement of UNDO/REDO (those still follow PacketPlacer and shift 4 px left with content — out of scope).
- Changed `Source/GUI/Layout/Design/DesignPanels.h`: `Panels::Header::kLogoGapAfter` 20 → 16. `DesignChecks.h` static_assert (`% 4 == 0`) still holds. DimensionFactory and layout read the constant unchanged.
- Standalone UAT note: UNDO/REDO follow the left flow (−4 design px); PANIC stays right-anchored, so the visual REDO↔PANIC gap grows by 4 design px. Plugin mode: right cluster X unchanged; left train starts 4 design px earlier (space scales with UI scale).
- Follow-up 2026-09-24: `kLogoGapAfter` → 12; Standalone now uses the same right-anchored UNDO/REDO/PANIC placement as plugin (`placeStandaloneFlowActionButtons` removed) so logo-gap shrinks create air before UNDO in Standalone too.

## Review Triage Log

- Spec missing Tasks/Acceptance/Manual checks — `false`: oneshot template deletes those sections by design.
- Intent overstates logo-gap vs UNDO crowding — `false`: frozen human Intent; plugin right-anchor is unchanged and matches “buttons stay put.”
- Oneshot thinner than peer layout specs — `false`: oneshot route omits Code Map / Boundaries / Verification sections.
- Standalone REDO↔PANIC gap growth undocumented — `low` → patched in Implementation Notes.
- Frontmatter still `in-progress` after code change — `false`: finalize sets `done`.
- Missing why-comment on `kLogoGapAfter` — `low` rejected: cosmetic; nearby constants are not uniformly commented.
- Missing UI-scale verification note — `low` → covered in Implementation Notes UAT line.
- Missing product-level positive outcome in spec — `false`: oneshot Intent is sufficient; outcome recorded in Implementation Notes.
