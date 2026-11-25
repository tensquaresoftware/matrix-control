---
title: 'FR user manual — finishing pass (lexicon, multi-instance, hygiene, placeholders)'
type: 'chore'
created: '2026-11-24'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
baseline_commit: '0bdcfdf8c613feb04ce878dbfd3b67e1e52e057b'
context:
  - '{project-root}/_bmad/custom/manuel-utilisateur-redaction-fr.md'
  - '{project-root}/_bmad-output/project-context.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After lots 1–4, the FR user manual still has lexicon leftovers (Unlock as a façade action, MIX under-defined), no multi-instance MIDI guidance, a few stale contradictions (e.g. footer quote vs live copy), and screenshot placeholders that need a clean inventory — without inventing images.

**Approach:** One finishing pass on `Documentation/Users/Manuals/Matrix-Control-Manuel-Utilisateur.md` (FR only): align Afficheur / PATCH NAME / skins / MIX / remove or reframe Unlock; add a short multi-instance port recommendation; re-check TOC/cross-refs and §16–17 for obsolete remnants; leave or refresh `Capture d’écran — À placer ici` captions; optionally sync local debts in `_bmad/custom/manuel-utilisateur-redaction-fr.md` (path + PATCH NAME not read-only). No EN translation, no real screenshots, no Skin Designer, no commit/PR unless asked.

**Decisions:**
- FR only; EN is a later pass.
- Do not invent captures; keep/clarify placeholders; list remaining placeholders in the Build chat end.
- Multi-instance: one MIDI port triple (or distinct pairs) per Matrix-1000 — place under §6.
- Unlock lexicon: remove user-facing Unlock row (no UI control; bank unlock SysEx is internal).
- Frontmatter stays Version `1.0` / Version-Produit `1.0.0-alpha.8` unless already bumped.
- Local lot 1–4 debt OK: redaction SSOT path + Afficheur/PATCH NAME editable wording.

</frozen-after-approval>

## Implementation Notes

- Route oneshot: single-file manual hygiene + optional redaction SSOT sync; no irreversibles.
- Product check: footer copy is `Settings > MIDI & DEVICE` (`PluginDisplayNames.h`); fixed stale quote that still said `MIDI SYNTH FROM / SYNTH TO`.
- Product check: Unlock bank SysEx remains internal (`sendUnlockBank`); no Unlock UI label — removed lexicon row.
- Multi-instance: short §6 paragraph + matching §17 troubleshooting row (one distinct port triple / pairs per Matrix-1000).
- Lexicon: Afficheur / PATCH NAME / SKIN / `DCO 2 | DCO 1 MIX`; Afficheur bullet in §7 aligned; two screenshot captions refreshed.
- Local lot debt: `_bmad/custom/manuel-utilisateur-redaction-fr.md` path → `Manuals/…` and PATCH NAME no longer “lecture seule”.
- Frontmatter left at Version 1.0 / 1.0.0-alpha.8 / Updated 2026-11-24.
- Files: `Documentation/Users/Manuals/Matrix-Control-Manuel-Utilisateur.md`, `_bmad/custom/manuel-utilisateur-redaction-fr.md`, this spec.
- Commit/PR deferred per user hors-scope unless later asked.
- Placeholders remaining: 13 (inventory delivered in Build chat Present only — intentional).
- Review patches: clarified multi-instance (no opaque “triplet”; distinct `SYNTH FROM`/`SYNTH TO`; KEYBOARD/HOST; Matrix-6/6R); unified Afficheur deep-link; `PATCH NAME` casing in §15; redaction Groupe vs Zone + parcours TOC alignment.

## Review Triage Log

- Blind Hunter — multi-instance “triplet” opaque / KEYBOARD FROM missing → **medium** → **patch** (rewrote §6 + §17 row).
- Blind Hunter — Matrix-1000-only framing / two cases mixed → **medium** → **patch** (instances vs two machines; 1000 or 6/6R).
- Blind Hunter — “paires qui ne se croisent pas” ambiguous → **medium** → **patch** (“ne partagez pas les mêmes ports”).
- Blind Hunter — Afficheur §7 vs lexicon deep-link mismatch → **low** → **patch** (both → `#nom-du-patch-patch-name`).
- Blind Hunter — redaction Zone vs manuel Groupe → **low** → **patch** (Groupe in SSOT table + checklist).
- Blind Hunter — placeholder inventory not versioned in spec → **false** (Intent: list in chat end only).
- Blind Hunter — MEMORY PROTECT/BANK LOCK need Unlock glose after lexicon removal → **false** (distinct Master `MISC` hardware switches; still named correctly in §17).
- Blind Hunter — “le Patch Name affiche” residual → **low** → **patch** (`PATCH NAME` backticks).
- Blind Hunter — redaction “Parcours type” numbering drift → **low** → **patch** (aligned to real TOC; no chapter refactor).
