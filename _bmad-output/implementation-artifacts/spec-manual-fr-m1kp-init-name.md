---
title: 'FR user manual — Computer Patches .m1kp and INIT name gate'
type: 'chore'
created: '2026-11-24'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
baseline_commit: 'fe13ba2367a863ebb4dea2be663b00a794e58259'
context:
  - '{project-root}/_bmad/custom/manuel-utilisateur-redaction-fr.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The FR user manual still says Computer Patches only supports `.syx`, documents `PATCH NAME` as read-only, and says a post-`INIT` rename is required only before `SAVE` / `SAVE AS`. Live product also loads `.m1kp`, can require rename before `STORE`, and may confirm before `SAVE` overwrites a sibling `.syx` after a `.m1kp` load.

**Approach:** Update `Documentation/User/Manuals/Matrix-Control-Manuel-Utilisateur.md` (§10 / `PATCH NAME`, §12, §15, lexicon as needed) from code truth so readers understand `.m1kp` load vs `.syx` save, the overwrite confirm, and the name gate after `INIT` for both `STORE` and file save. Integrate only the minimum editable-`PATCH NAME` wording needed for that gate to make sense; FR only.

**Decisions:**
- FR only; EN later.
- Target file: `Documentation/User/Manuals/Matrix-Control-Manuel-Utilisateur.md`.
- Lot 1 (editable `PATCH NAME`) is a logical prerequisite: if the manual still says read-only, replace with the minimum needed so STORE-after-INIT is understandable — do not reopen the skins lot.
- Native Matrix-Control save remains Edit Buffer `.syx`; never promise editing/rewriting the source `.m1kp`.
- Distinguish displayed name (Settings DISPLAY… / `.m1kp` always uses filename stem) from SysEx / file naming.
- Out of scope: EN; BANK UTILITY; skins; LFO paste; Auto-Scale; ASIO/JACK; full Mutator rewrite; commit/PR unless asked.

</frozen-after-approval>

## Implementation Notes

- Updated FR manual §§ PATCH NAME / 10 / 12 / 15 + lexicon `.m1kp`; aligned redaction SSOT so agents no longer claim PATCH NAME is read-only.
- Documented STORE name-required arm vs SAVE hard-refuse; `.m1kp` load + sibling `.syx` overwrite confirm; SAVE always `.syx`.
- Did not reopen skins / DISPLAY deep dive — one cross-ref sentence to §15 and `.m1kp` always-uses-filename note.
- Blind Hunter follow-ups: clarified Hardware Names edit block + first SAVE after `.m1kp`, mismatch dialog `.syx`-only, drag-drop sentence, MEMORY note, redaction Computer Patches rule.

## Review Triage Log

- Blind: Hardware Names / vague « éditable » / §15 croisé — medium — **patched**: one sentence on Hardware Names + `.m1kp` filename; deep DISPLAY matrix deferred (lot 1).
- Blind: mismatch dialog not scoped to `.syx` — medium — **patched**.
- Blind: lexicon drag-drop vs missing §12 — medium — **patched** (sentence + keep lexicon).
- Blind: first SAVE without sibling `.syx` — medium — **patched**.
- Blind: MEMORY note omits SAVE — low — **patched**.
- Blind: redaction SSOT missing Computer Patches rules — medium — **patched**.
- Blind: full editability criteria catalog — low / out of scope — **rejected** (intent: one renvoi; detail already lot 1).
