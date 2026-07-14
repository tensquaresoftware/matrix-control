# Correct Course — INIT SysEx Device Rules & Hierarchical History ComboBox

**Date:** 2026-07-14  
**Status:** Approved  
**Author:** BMad Agent (Correct Course)  
**Trigger:** Story 7-6 UAT — INIT editor-only; Mutator History dual combobox layout failure

## Summary

Two sprint changes approved during 7-6 smoke testing:

1. **INIT outbound SysEx (7-11)** — Matrix-1000: opcode 0x0D edit buffer; Matrix-6/6R: opcode 0x01 to current patch. STORE unchanged (0x01).
2. **Hierarchical History UI (U-12 + 6-14)** — New `TSS::HierarchicalComboBox`; single 48 px control (RANDOM slider width) replaces dual 24 px comboboxes.

## Artifacts

| Document | Path |
|----------|------|
| Sprint Change Proposal | `_bmad-output/planning-artifacts/sprint-change-proposal-2026-07-14-init-sysex-and-hierarchical-history-combobox.md` |
| Story 7-11 | `_bmad-output/implementation-artifacts/7-11-internal-patches-init-sysex-device-rules.md` |
| Story U-12 | `_bmad-output/implementation-artifacts/u-12-hierarchical-combobox-widget.md` |
| Story 6-14 | `_bmad-output/implementation-artifacts/6-14-patch-mutator-hierarchical-history-combobox.md` |

## Implementation order

1. `/bmad-dev-story 7-11`
2. `/bmad-dev-story u-12`
3. `/bmad-dev-story 6-14`

## Decisions

- **D-044-R2** — INIT device-aware SysEx
- **D-082-R2** — `HierarchicalComboBox` replaces dual History comboboxes
