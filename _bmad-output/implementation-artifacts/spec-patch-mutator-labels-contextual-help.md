---
title: 'Patch Mutator MODE/PITCH/HISTORY label contextual help'
type: 'bugfix'
created: '2026-10-09'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
baseline_commit: 'dc3cefb3da58e9550c894b9a6e712612db4c4ec4'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-patch-mutator-footer-contextual-help.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Hovering the MODE, PITCH, and HISTORY text labels in Patch Mutator does not show footer contextual help, even though the matching combo boxes already do. Users often aim at the label, so help feels broken for those three rows.

**Approach:** Bind the three existing label components to the same Mutator help strings already used by their combo boxes, matching the Settings label+control pattern. No new copy, no footer API changes.

</frozen-after-approval>

## Implementation Notes

- Root cause: `PatchMutatorPanel::registerContextualHelp` bound only the three combo boxes; `modeLabel_` / `pitchLabel_` / `historyLabel_` are separate `TSS::Label` children with no binder.
- Change: `PatchMutatorPanelContextualHelp.cpp` — bind each label to the same `MutatorHelp::kMode` / `kPitch` / `kHistory` string as its combo; add `#include "GUI/Widgets/Label.h"`.
- Precedents: `SettingsPanel.cpp` label+combo binds; also Patch Manager `ComputerPatchesPanelContextualHelp.cpp` / `InternalPatchesPanel.cpp`.
- Left alone: Footer overlay API, binder core, `PluginDisplayNames` Mutator help constants, Compare film.
- Manual smoke: hover MODE / PITCH / HISTORY labels → same HELP line as the matching combo; leave restores sticky; skim label→combo on one row (anti-flicker should hold).
- Quality: `python3 Scripts/quality/lint_touched.py` OK on the touched `.cpp`.

## Review Triage Log

- Missing smoke / Label.h / Patch Manager precedent in Implementation Notes — `low`; notes updated in finalize.
- Parent done-spec still says “22 controls” without face labels — `false` for this bugfix; parent Intent stays historical; this oneshot documents the gap.
- Disabled PITCH label/combo may not receive mouse enter — `medium` (pre-existing enablement vs help); deferred (see deferred-work.md).
- Spec still `in-progress` mid-oneshot — `false`; process state; finalized to `done`.
- Label→combo traversal sticky flash — `false`; same binder anti-flicker delay already covers adjacent bound controls.
- Weak `baseline_commit` vs original Mutator help commit — `low` rejected; HEAD baseline is valid for this freeform bugfix.
