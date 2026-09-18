---
title: 'Sticky footer messages inventory'
type: 'chore'
created: '2026-09-16'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Sticky left-band footer messages (APVTS `uiMessageText`) sometimes use Title Case module names (e..g. `Patch Mutator`) while the product convention elsewhere is UPPERCASE (`PATCH MUTATOR`, `DCO 1`, `MATRIX MODULATION`). Guillaume needs a complete inventory before choosing which strings to fix.

**Approach:** Produce one Markdown inventory file on the macOS Desktop listing every sticky footer message (templates included), grouped in tables by usage, with symbol/source hints and a short casing note. No plugin code or display-string changes in this pass.

</frozen-after-approval>

## Implementation Notes

- Deliverable path: `/Users/Guillaume/Desktop/Matrix-Control-sticky-footer-messages.md`
- Grouped sticky messages by usage (device/MIDI, settings/init, clipboard, gates, patch name, computer patches, bank utility, Patch Mutator, PatchFileService bare errors, widget factory).
- Excluded HELP overlay, bank progress dialogs, DEVICE badge, MIDI queue centre-band.
- Highlighted casing hotspots: `Patch Mutator:` prefix, `Matrix Modulation` clipboard name, Master Unison / Keyboard Mode Title Case, Settings Master sentence case.
- Post-review: added PatchFileService bare errorMessage inventory, Mutator passthrough concrete wordings, dead `MatrixModulationSection::Header::kIncompatiblePasteFooter`, `InvalidWidgetTypeException`, Windows MIDI open template, first-load reconciliation composition note.
- No Source/ string or behavior changes in this pass.
- Spec Intent frozen block has a typo `e..g.` left untouched (frozen-after-approval).
- No git commit (user standing rule: commit only on explicit request).

## Review Triage Log

- Missing PatchFileService bare `errorMessage` sticky strings — medium/patch — verified via `PatchFileService.cpp` + computer load/save / bank export / init write / Mutator passthrough; added section 9.
- Mutator export passthrough too vague — medium/patch — listed concrete prefixed wordings and near-duplicates.
- `File not found` vs `Patch file not found` — medium/patch — documented near-duplicate pair in section 9.
- Missing `MatrixModulationSection::Header::kIncompatiblePasteFooter` — medium/patch — added as dead constant with Title Case note.
- Missing `InvalidWidgetTypeException` — medium/patch — added to widget factory section.
- MIDI open-failure rows unclear / missing Windows template — medium/patch — rewritten with `kNotFound` / `kOpenRejected` variants.
- ShortLabels unused marked with `?` — low/patch — marked definitively unused.
- Severity per message not inventoried — low/reject — user goal is casing inventory; severity is out of requested scope.
- Incomplete Bank/Mutator symbol namespaces — medium/patch — added namespace hints for Bank Utility and Mutator sections.
- First-load + reconciliation composition omitted — medium/patch — updated computer-patches row.
- Spec status/typo/verification gaps — low/patch for status+notes; frozen typo left (cannot edit frozen); verification section not required for oneshot minimal.
- No in-repo durable inventory copy — defer — user asked Desktop only; recorded for optional later SSOT if desired.

