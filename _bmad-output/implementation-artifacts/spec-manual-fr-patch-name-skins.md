---
title: 'FR user manual — PATCH NAME rename + six skins'
type: 'chore'
created: '2026-11-24'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad/custom/manuel-utilisateur-redaction-fr.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The French user manual still describes `PATCH NAME` as read-only and only two skins (`BLACK` / `CREAM`), which no longer matches Matrix-Control 1.0.0-alpha.8.

**Approach:** Update only `Documentation/Users/Manuals/Matrix-Control-Manuel-Utilisateur.md` from the live product (code/UI): document when `PATCH NAME` is editable via double-click and the six shipping skins with shortcuts ⌘/Ctrl+1…6 and arrow cycling. Keep existing frontmatter version fields unchanged.

</frozen-after-approval>

## Implementation Notes

- Source of truth: `canEditPatchName()` (Compare off; computer file always editable; device memory blocked on ROM banks 2–9; Hardware Names + banked Matrix-1000 blocked; otherwise editable). Help copy: double-click when editable.
- Skin combo / shortcut order (not internal IDs): 1 `BLACK/GREEN`, 2 `BLACK/CHROMA`, 3 `BLACK/MINIMAL`, 4 `CREAM/GREEN`, 5 `CREAM/CHROMA`, 6 `CREAM/MINIMAL`. Family cycle: Cmd/Ctrl+arrows (up/left previous, down/right next); global cycle: Cmd/Ctrl+Shift+arrows. Logo menu: `SKIN` with BLACK / CREAM / TONES family flyouts; digit shortcuts only on the six bases.
- Do not invent UI labels; copy from `PluginDisplayNames::SkinVariants` / Settings. Do not claim always-editable. Frontmatter already at Version 1.0 / 1.0.0-alpha.8 / 2026-11-24 — leave as-is (pre-existing dirty tree; this oneshot did not re-edit those fields).
- Redaction SSOT `_bmad/custom/manuel-utilisateur-redaction-fr.md` still says `PATCH NAME` read-only — deferred.
- Implemented 2026-11-24: rewrote § PATCH NAME (editable gates, Compare, persistence STORE/SAVE, blocked double-click UX), §15 logo/SKIN/shortcuts (six bases + TONES + arrow directions), Settings SKIN row, lexicon Afficheur.
- Review patches applied: Compare on allowed list; Matrix-1000 vs Matrix-6 Hardware Names split; STORE/SAVE persistence; arrow directions; TONES count + family cycle; Settings-order wording for digits.
- Branch: `docs/manual-fr-patch-name-skins`. Files touched: manual, this spec, deferred-work.md.

## Review Triage Log

- Blind Hunter — Compare missing on allowed bullets: medium → patched (hors Compare on allowed list).
- Blind Hunter — Hardware Names grammar tied to Matrix-6: medium → patched (split bullets).
- Blind Hunter — no persistence after rename: medium → patched (STORE / SAVE / SAVE AS + PATCH NAME vs filename).
- Blind Hunter — arrow cycle direction unspecified: medium → patched (up/left previous, down/right next).
- Blind Hunter — TONES only as ellipsis: low → patched (fifteen named variants CHERRY…FUCHSIA).
- Blind Hunter — family cycle within TONES omitted: medium → patched.
- Blind Hunter — digit order “logo menu” imprecise: low → patched (Settings flat combo; logo families).
- Blind Hunter — no cross-ref rename vs SAVE AS filename: low → patched (one sentence).
- Blind Hunter — blocked double-click UX unexplained: medium → patched.
- Blind Hunter — frontmatter vs Intent contradiction: false — frontmatter was already updated before this oneshot (user decision); Intent forbids changing it further; oneshot did not re-edit those fields.
- Blind Hunter — Implementation Notes omitted TONES in logo: low → patched in notes.
- Blind Hunter — redaction SSOT still says PATCH NAME read-only: defer → deferred-work.md.
