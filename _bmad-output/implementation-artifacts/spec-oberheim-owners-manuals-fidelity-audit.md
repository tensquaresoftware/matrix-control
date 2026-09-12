---
title: 'Oberheim owners manuals fidelity audit (Preview OCR)'
type: 'chore'
created: '2026-09-12'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'f146e2d70015ef4187cd0d7fc653a7cf5ec2c5e7'
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After the prior surgical repair, we need a verification-only fidelity check of the Oberheim owners-manual Markdown against macOS Preview “Integrate Text” regenerated PDFs (native OCR text layer), producing a short actionable gap list—not a full line-by-line retranscription.

**Approach:** Extract text from the Preview-regenerated PDFs via PyMuPDF (`fitz`), compare structure/params/tables/model callouts against the merged 6/6R MD and the Matrix-1000 owners MD, and report only actionable divergences (missing, condensed, wrong model, wrong table). Ignore Preview OCR noise unless it blocks a judgment.

## Boundaries & Constraints

**Always:**
- Audit only: do not rewrite MD unless Guillaume later explicitly asks for patches from the findings.
- French chat + clarity bar; English artifact for the gap list if written to disk.
- Prefer print intent from Preview PDF text; ignore OCR typos (`orto`, `MIDl`, etc.) when meaning is clear.
- Coverage baseline matches prior repair: 6/6R through Creating A Patch / before NOTES & worksheets; Matrix-1000 Intro→Ch.4 (Ch.5 SysEx pointer OK; Warranty omitted); Warranty/NOTES/worksheets omitted on purpose.
- Leave unrelated dirty Source/ files untouched; no commits/PRs.
- **Decision:** Include Matrix-1000 in this audit using `/Users/Guillaume/Downloads/Oberheim Matrix-1000 - Owner's Manual (Régénération Aperçu).pdf` (64 pages, text layer present).

**Never:**
- Reconstruct or “improve” owners-manual prose in this run.
- Audit or modify `oberheim-matrix-1000-midi-sysex-implementation.md` (or other SysEx companions).
- Treat Warranty / NOTES / worksheets / illustrations as in-scope gaps.
- Full from-scratch re-OCR; use the regenerated PDF text layer as-is.
- Signal non-actionable OCR noise as findings.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Actionable MD vs PDF gap | Missing section, condensation, wrong-model callout, bad table | Finding with severity (decision / patch / defer / dismiss), PDF page + MD anchor, recommendation | N/A |
| Preview OCR typo only | `MIDl`, `orto`, garbled glyph but meaning clear | No finding | Ignore noise |
| Out-of-coverage zone | Warranty, NOTES, worksheets, diagrams | No finding | Explicitly out of scope |
| Empty / unreadable PDF page | e.g. Matrix-6R p.4 empty | Note in audit notes; do not invent content | Skip page; do not invent |
| Prior repair conflict notes | MD already has `source conflict` annotations | Confirm still matches print ambiguity; do not invent hardware truth | Keep annotation unless PDF resolves it |

</frozen-after-approval>

## Code Map

**Audit targets (read-only this run):**
- `_local/References/Oberheim/oberheim-matrix-6-6r-owners-manual.md` — primary (~2500 lines); merged 6/6R; callouts `**Matrix-6:**` / `**Matrix-6R:**`; coverage through Creating A Patch; Warranty/NOTES/worksheets omitted by design.
- `_local/References/Oberheim/oberheim-matrix-1000-owners-manual.md` — in scope (~576 lines; Intro→Ch.4; Ch.5 → SysEx pointer; Ch.6 Warranty omitted).

**PDF SSOT (Preview Integrate Text — extract only):**
- `/Users/Guillaume/Downloads/Oberheim Matrix 6 - Owner's Manual (Régénération Aperçu).pdf` — 97 pages, all with text.
- `/Users/Guillaume/Downloads/Oberheim Matrix 6R - Owner's Manual (Régénération Aperçu).pdf` — 115 pages, 114 with text; p.4 empty.
- `/Users/Guillaume/Downloads/Oberheim Matrix-1000 - Owner's Manual (Régénération Aperçu).pdf` — 64 pages, all with text.
- Extract with PyMuPDF `fitz` (available in agent env). Do not re-OCR from scratch.

**Prior repair context (do not reopen as implementation):**
- `_bmad-output/implementation-artifacts/spec-oberheim-owners-manuals-fidelity-repair.md` — `status: done`; known residual risks (SPLIT EDIT OCR outside primary zones; Spillover mushy cells annotated).

**Do not touch:**
- `_local/References/Oberheim/oberheim-matrix-1000-midi-sysex-implementation.md` and other SysEx/companion docs.
- `Source/` dirty files (`RampPortamentoPanel.cpp`, `PluginDisplayNames.h`).

**Deliverable artifact (English, optional but preferred):**
- `_bmad-output/implementation-artifacts/audit-oberheim-owners-manuals-fidelity-preview.md` — short gap list mirroring chat triage.

## Tasks & Acceptance

**Execution:**
- [x] Extract Preview PDF text (Matrix-6 + Matrix-6R + Matrix-1000) via `fitz` into a working comparison corpus (temp OK; not committed).
- [x] Spot-compare 6/6R MD vs PDF on high-value zones: TOC/structure, param sequences, Master MIDI/Cassette/Vibrato/Misc tables, Keypad/Mode Select, model callouts, known residual SPLIT EDIT / Spillover areas — not full line-by-line.
- [x] Spot-compare Matrix-1000 MD vs its Preview PDF on high-value zones: TOC/hierarchy (Basic Modes nesting, Special Modes → Units), Bend Range ambiguity note, Data Dump structure, Intro→Ch.4 completeness vs intentional Ch.5 pointer / Ch.6 Warranty omit — not full line-by-line.
- [x] Produce French chat triage (decision / patch / defer / dismiss) + English audit artifact; global verdict usable-as-is vs retouch zones.

**Acceptance Criteria:**
- Given Preview PDF text and the 6/6R + Matrix-1000 owners MDs, when the audit finishes, then findings are only actionable divergences with PDF page + MD anchor + recommendation.
- Given OCR noise that does not change meaning, when classified, then it is dismissed (sans action), not listed as a repair.
- Given out-of-scope zones (Warranty/NOTES/worksheets/SysEx companion), when scanned, then no findings and no file edits.
- Given the run completes, when checked, then owners MD files are byte-unchanged unless Guillaume later requests patches.

## Implementation Notes

- 2026-09-12: Spot audit only. Corpus at `/tmp/oberheim-preview-ocr/{matrix6,matrix6r,matrix1000}.txt` (Matrix-6 97 pp, Matrix-6R 115 pp with p.4 empty, Matrix-1000 64 pp).
- Deliverable: `_bmad-output/implementation-artifacts/audit-oberheim-owners-manuals-fidelity-preview.md` — **2 patch**, **2 defer**, **0 dismiss-as-finding** (pass checks under Audit notes), **0 decision**.
- Review patches (same run): moved non-actionable dismiss/OOS/OCR rows out of Findings; added zone-checked notes; quoted F1/F2 evidence; added defer unblock criteria + SHA embeds.
- Owners MDs + SysEx companion left byte-unchanged (SHA-256):
  - `oberheim-matrix-6-6r-owners-manual.md` `07cd4d83e36472bce6444d9f33dbcada372b9e75472110954f65e10491de8b71`
  - `oberheim-matrix-1000-owners-manual.md` `13a896eb235ae9b1292056fe88fbc1e126dae9d506c825dfdd8c841605079bad`
  - `oberheim-matrix-1000-midi-sysex-implementation.md` `1a7396aa7dc9c65cf2c92152ecc8e7176968ab259245bcce08d806c28f0efd0e`
- Unrelated dirty `Source/` files left untouched; no commit/PR.
- French chat triage delivered in Build presentation (step-05).

## Spec Change Log

## Review Triage Log

- preview-audit: Matrix-6R Creating A Patch `17. DC2 LEV` MD says ON vs Preview `VALUE = BOTH` (+ chart) — **patch** (F1) — carried: still true at MD ~L2282–2284 / Preview p.99; deferred to deferred-work (owners MD out of scope this run)
- preview-audit: Matrix-6R `18. MIDI MONO` missing intro / Using MIDI MONO / SPLIT note vs Preview p.77–79 — **patch** (F2) — carried: still true; deferred to deferred-work
- preview-audit: Spillover Split chart still mushy on Preview p.71 — **defer** (F3) — carried: appended deferred-work with unblock criteria
- preview-audit: Patch Map Echo combination chart flattened vs Preview p.77 — **defer** (F4) — carried: appended deferred-work with unblock criteria
- preview-audit: SPLIT EDIT / VCF-Cassette / M1000 / OCR-OOS as Findings F5–F8 — **false** after review patch: non-actionable pass/OOS moved to Audit notes (frozen Never + AC “actionable only”)
- blind: Method zones (Master MIDI / Keypad / Vibrato / Misc / ACT SENSE / 6/6R TOC) lacked recorded pass — **medium** → **patch**: added Audit notes checked-OK rows
- blind: F1/F2 lacked quotes / missing-block inventory — **medium** → **patch**: evidence + page inventory added
- blind: F8 bundled noise/OOS as finding — **high** → **patch**: removed from Findings (edge-case agrees)
- blind: F7 optional deeper TOC as decision while Counts=0 — **false**: preference note is not a decision-needed fidelity gap
- blind: Matrix-6 MIDI MONO unchecked — **false**: Matrix-6 has no MIDI MONO Master param (6R-only)
- blind: empty Spec Change Log / review_loop metadata / edit-spec hygiene — **false**: rejected (fix would edit this build’s spec)
- blind: French chat triage not in artifact — **false**: chat presentation is the French triage channel per frozen Always
- blind: global verdict underplays F2 — **low** → **patch**: verdict now names both retouch zones explicitly
- blind: defer without unblock criteria — **low** → **patch**: Unblock rows on F3/F4
- blind: ephemeral corpus without reproduce hint — **low** → **patch**: reproduce field on audit frontmatter
- edge: F8 / dismiss rows violate I/O “No finding” for OCR noise and OOS — **high** → **patch**: same as F8 removal
- edge: AC “only actionable divergences” vs F5–F8 — **high** → **patch**: Findings now F1–F4 only
- verification-gap: no gaps — N/A (doc-only chore)