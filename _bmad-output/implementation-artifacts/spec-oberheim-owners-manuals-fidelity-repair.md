---
title: 'Oberheim owners manuals fidelity repair'
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

**Problem:** The merged Matrix-6/6R owner-manual Markdown has two quality regimes: Patch Edit through Creating A Patch is mostly faithful, while Keypad/Mode Select and Master Edit (MIDI → Cassette → Vibrato → Misc) still show dirty OCR, condensations, flattened tables, and 6R-only wording presented as shared. Matrix-1000 Intro→Ch.4 is largely faithful but has a few structural/ambiguity defects.

**Approach:** Surgical zone repair against PDF as SSOT, using CloudConverter exports and `/tmp/manual_ocr/{m6,m6r,m1000}/` as reconstruction materials. Preserve already-clean zones. Prefer fidelity to print over editorial polish.

## Boundaries & Constraints

**Always:**
- English body text matching print intent; French only in chat.
- Silence = shared (“the instrument”, “Keyboard / Master”); divergences = `**Matrix-6:**` / `**Matrix-6R:**` callouts.
- Style: no doc H1 title; no `<a id>`; params as `### 48. KEYBOARD MODE`; YAML keys like sibling refs; update `updated` date.
- 6/6R coverage through Creating A Patch; omit Warranty / NOTES / worksheets; state that clearly in provenance + end marker.
- M1000 Ch.5 stays a pointer to `oberheim-matrix-1000-midi-sysex-implementation.md`.
- When CloudConverter and OCR diverge: cross-check; if still doubtful, annotate rather than invent; paste Preview/PDF only for an unreadable chart.
- Leave unrelated dirty Source/ files untouched (`RampPortamentoPanel.cpp`, `PluginDisplayNames.h`).

**Decisions (from audit, applied as-is):**
1. Neutralize bare model names in shared prose + callout true divergences.
2. Restore MIDI Memory Protect from CloudConverter 6R (full text).
3. PEDAL 1 vs PEDAL 2 External Trigger conflict on Matrix-6 print → annotate `source conflict`, do not invent hardware truth.
4. `56 STEREO` on Matrix-6 (listed as 56–99 not listed vs front-panel mention) → annotate `source conflict`.
5. Title hierarchy: MASTER EDIT / SPLIT EDIT as H2 under Programming (align with PATCH EDIT family).
6. Remove invented Creating A Patch STEP 1 Matrix-6-only callout framing.
7. If Fine Tune / DAC Zero lines are edited, mark `reconstructed from damaged OCR`; otherwise leave silent prior fixes alone.

**Never:**
- Rewrite / “improve” Oberheim prose style.
- Convert illustrations, waveforms, blank worksheets.
- Split into two separate 6 and 6R manuals.
- Copy Warranty / NOTES / M1000 Ch.5–6 SysEx body; modify the SysEx companion doc.
- Full from-scratch retranscription of already-OK zones (Keyboard Mode, Ramp/Portamento, Tracking, Creating A Patch body except listed nits).
- Commit / PR unless Guillaume asks.
- Treat M1000 Specs / Fuse / Ext. Funct. tables as condensations to rewrite.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Shared prose with bare MATRIX-6R | OCR/CC dump using 6R name in common section | Neutral instrument wording; model-specific facts in callouts | Keep intentional dual mentions |
| CloudConverter ≠ OCR | Conflicting readable strings | Prefer print via PDF/OCR page; restore full prose | Annotate unresolved conflict |
| Unreadable chart | Spillover / controller table still mush | Reconstruct from best CC+OCR; if still opaque, leave short conflict note | Do not invent values |
| Clean zone | Keyboard Mode / Ramp / Tracking / Creating body | Unchanged except listed nits | N/A |
| Model-only feature (e.g. MIDI 12+) | 6R-only Master MIDI params in “shared” text | Callout or 6R-only subsection; no shared “12 MIDI ECHO” refs on Matrix-6 | Strip false common cross-refs |

</frozen-after-approval>

## Code Map

**Targets (gitignored `_local/` — edit freely; no Source/):**
- `_local/References/Oberheim/oberheim-matrix-6-6r-owners-manual.md` — primary repair (~2260 lines). Frontmatter + provenance blockquote; H1 major parts; `### NN. NAME` params; existing ~28/49 Matrix-6/6R callouts.
- `_local/References/Oberheim/oberheim-matrix-1000-owners-manual.md` — light structural/ambiguity fixes only.

**Sources (read-only):**
- CloudConverter: `/Users/Guillaume/Downloads/Oberheim Matrix 6R - Owner's Manual (CloudConverter).md`, `… Matrix 6 - Owner's Manual (CloudConverter).md` (both present).
- OCR: `/tmp/manual_ocr/m6|m6r|m1000/` (`ALL.txt`, `txt/pNNN.txt`).
- PDF SSOT under `/Volumes/Guillaume/Documents/Notices & Plans/Oberheim/…`.
- Do not modify: `_local/References/Oberheim/oberheim-matrix-1000-midi-sysex-implementation.md`.

**6/6R zone anchors (approx.):**
- Phantom “IF YOU HAVE A PROBLEM” ~L143; STEREO mention ~L159; Pedal 1 Notes ~L215 (verify vs print); Memory Protect ~L243 (**MIDI Memory Protect still missing**).
- Keypad / Mode Select: `# Touring — Part 2` ~L257–508 — OCR garble, bare MATRIX-6R.
- VCF/VCA intro + 21–22 ~L597–620 — condensed.
- MASTER `# MASTER EDIT` / `## MIDI` ~L1291+ — dirty intro, 00–05, thin controller table (Matrix-6 values 7/64/1/2 from OCR/CC6).
- Spillover `13` + ACT SENSE `14` ~L1462–1521 — flat chart / condensed prose.
- Dup Cassette intro after MIDI MONO ~L1618; Errors incomplete before 8/9/A; Vibrato 30–36 mis-nested under Cassette; `50. EDIT RECALL` under Vibrato; INIT PATCH order chaos.
- Creating A Patch nits ~L2028+: STEP 1 framing; `07. DC1 LEV`; ROTROB tip → Matrix-6 callout.
- End: no end-of-coverage marker; provenance omits Warranty note clarity vs M1000 style.

**M1000 anchors:**
- TOC: Transpose/Unison/MIDI Echo sibling of Basic Modes (should nest); `Special Modes — Units` collapsed H2.
- L301 “eight … 0 through 7” vs `#8 Bend Range` — add as-printed ambiguity note.
- Data Dump: numbered list interrupted by table — light structural tidy; TOC MIDI/SysEx stub already OK.
- Fine Tune / DAC Zero already silently repaired — touch only if editing those lines.
- Unison / Units / Data Dump substance / Adv. Ext. = PASS (do not rewrite).

**Reuse:** Existing callout lead-in style; M1000 provenance/end italic pattern for Warranty omit + end marker on 6/6R.

**Do not change:** Keyboard Mode / Ramp / Portamento / Tracking / Creating A Patch body (except nits); MATRIX MOD destination numbers ≠ PATCH EDIT param numbers; ROTROB already correct in Keyboard Mode — keep.

## Tasks & Acceptance

**Execution:**
- [x] `oberheim-matrix-6-6r-owners-manual.md` — Repair Master MIDI zone first: intro + `00`–`05` + controller table (Matrix-6 defaults from OCR/CC6); restore MIDI Memory Protect; add missing Welcome/Pedal/Master-count/TUNE/BASIC CHANNEL TRANSMIT callouts — highest-value fidelity gap.
- [x] `oberheim-matrix-6-6r-owners-manual.md` — Restore `13` Spillover chart + full `14` ACT SENSE prose from OCR/CC; neutralize bare MATRIX-6R in shared MIDI text; no false shared `12+` refs for Matrix-6.
- [x] `oberheim-matrix-6-6r-owners-manual.md` — Cassette: dedupe intro; complete Errors 1–10/A; reparent Vibrato `30`–`36` and Misc EDIT RECALL / INIT PATCH (STEP order) under correct headings.
- [x] `oberheim-matrix-6-6r-owners-manual.md` — Renormalize Keypad / Mode Select from CloudConverter+OCR; consistent NAME/COMPARE/VALUE headings; neutralize shared bare model names.
- [x] `oberheim-matrix-6-6r-owners-manual.md` — Repair VCF/VCA intro + params 21–22; Creating A Patch nits (ROTROB tip callout, `07 DC1 LEV` BOTH vs ON, remove false STEP 1 callout); PEDAL/STEREO source-conflict notes; hierarchy MASTER/SPLIT as H2 under Programming; TOC slug pass; provenance + end marker + remove phantom “IF YOU HAVE A PROBLEM” dependency.
- [x] `oberheim-matrix-1000-owners-manual.md` — Nest Transpose/Unison/MIDI Echo under Basic Modes; parent SPECIAL MODES → Units; ambiguity note eight/0–7 vs `#8 Bend Range`; Data Dump list/table structure; do not rewrite PASS substance or SysEx companion.
- [x] Both targets — Mechanical filet: no `<a id>`, no “Continue with…”, no dup Cassette intro, TOC links resolve, end marker present, Warranty/NOTES omission stated.

**Acceptance Criteria:**
- Given a dirty Master MIDI / Cassette / Keypad zone, when repaired from CC+OCR+PDF, then prose matches print intent (no editorial paraphrase) and model splits use callouts.
- Given Keyboard Mode / Ramp / Tracking / Creating body (minus nits), when the repair finishes, then those sections are unchanged in substance.
- Given M1000 Basic/Special Modes headings, when fixed, then TOC and body hierarchy match print grouping without rewriting Unison/Units/Data Dump/Adv. Ext. content.
- Given finished 6/6R MD, when scanned mechanically, then no `<a id>`, no Cassette double intro, coverage/end marker present, and phantom Problem section is not presented as included.
- Given SysEx companion path, when owners MD is saved, then companion file is byte-identical (untouched).

## Implementation Notes

- 2026-09-12: Implementation subagent repaired both owners MDs per Tasks; status briefly set `in-review` before parent verification.
- 2026-09-12 parent: Moved `## MASTER EDIT` / `## SPLIT EDIT` to sit under `# Programming — Part 3` (before `# Using MATRIX MODULATION™`) so Markdown nesting matches the TOC / frozen hierarchy decision.
- I/O matrix covered by manual MD inspection + `rg` filet (doc-only chore; no unit tests).
- Residual risk noted by implementer: SPLIT EDIT body still has OCR damage outside primary repair zones; Spillover chart annotated where cells were mushy.
- 2026-09-12 review patches: Matrix-6R callout on BASIC CHANNEL MIDI ECHO; SEND PATCH/ALL dual-model STEPs; restored MIDI ECHO prose; cleaned MIDI MONO trailing OCR; Spillover 0/6 Upper=`N+3`; DC2 LEV BOTH/ON split.
- No git commit: frozen intent + Guillaume policy (commits only on explicit ask); `_local/` targets are gitignored; unrelated Source/ dirt left untouched.

## Spec Change Log

## Review Triage Log

- blind: MIDI MONO trailing OCR garbage (`V Whatever`, smashed numbering) — **high** — verified at `### 18. MIDI MONO` after channel map; print OCR has clean numbered bullets. → **patch**
- blind: SPLIT EDIT still OCR-dirty without banner — **medium** — verified residual body; primary repair order excluded Split body. → **defer**
- blind: `12. MIDI ECHO` modern paraphrase — **medium** — verified; OCR/CC has full Oberheim prose. → **patch**
- blind: Patch Map `15`–`17` / `11 SEND ALL` condensations — **low** — real fidelity cliff but beyond surgical MIDI 00–05/Spillover/ACT priority; Patch Map already callout-gated. → **defer**
- blind: Cassette `21`/`22` abbreviated vs `20` — **low** — real but secondary to Errors 1–10/A already restored. → **defer**
- blind: VCF `23`/`24` condensed + `25` typos — **low** — tasks scoped `21`–`22` only. → **defer**
- blind: Creating `17. DC2 LEV` hard-codes BOTH — **medium** — verified; `07. DC1 LEV` already has 6/6R split. → **patch**
- blind: `10. SEND PATCH` Matrix-6R-only unmarked — **high** — verified; Matrix-6 OCR has parallel STEPs. → **patch**
- blind: Spec Code Map / logs stale — **false** — rejected: finding asks to edit this build's spec tracking, not product text.
- blind: STEREO conflict note only in Volume prose — **low** — unlikely everyday miss; note already present early. Rejected (fix cost > benefit).
- blind: Spillover footnote claims cell annotations but none — **medium** — verified; OCR `p071` shows 0/6 upper **N+3** while MD has N+2. → **patch**
- blind: Pre-repair unavailable so clean-zone AC unverifiable — **false** — process limit of gitignored `_local/`, not a product defect.
- blind: Keyboard `42`/`43` unmarked under Matrix-6 intro — **maybe-false** — would need full Master Keyboard page OCR compare; if true would be medium. → **defer** (unverified)
- blind: Basic Patch ENV Amp `+63` vs `63` — **low** — cosmetic; rejected.
- edge: Matrix-6 SEND PATCH/ALL missing — **high** — same root as SEND PATCH finding. → **patch** (grouped)
- edge: Unmarked BASIC CHANNEL cites param 12 MIDI ECHO — **high** — verified L1302 after Matrix-6R transmit block but unmarked continuation. → **patch**
- edge: Spillover 0/6 upper N+2 vs OCR N+3 — **high** — verified vs `/tmp/manual_ocr/m6r/txt/p071.txt`. → **patch** (grouped with footnote)
- edge: Basic channel wrap for N+1/N+3 when >16 — **maybe-false** — print only documents +2 wrap for 15/16; inventing N+1/N+3 wrap would be editorial. → **defer**
- edge: Split Example STEPs truncated — **medium** — same residual Split body. → **defer** (grouped with SPLIT EDIT)
- edge claim: no false shared 12+ refs — **high** — claim falsified by L1302. → **patch** (grouped)
- edge claim: neutralize bare MATRIX-6R in SEND — **high** — claim falsified. → **patch** (grouped)
- edge claim: Spillover cells annotated — **medium** — claim falsified. → **patch** (grouped)
- verification-gap: none — n/a

## Design Notes

**Merge rule:** Unmarked paragraphs = both models. Prefer “the instrument” / “Keyboard / Master” over forcing one product name. Callouts only for real divergences (hardware jack, param count, ROTROB, MIDI 12+, timings).

**CloudConverter hygiene:** Strip HTML/picture noise (`MID!`, `MATRIX-E6R`); restore sentences — never paste converter garbage.

**Repair order (6/6R):** MIDI Master → Spillover/ACT SENSE → Cassette/Vibrato/Misc → Keypad/Mode Select → Welcome/Pedal/Memory Protect callouts → VCF/VCA 21–22 → TOC/provenance/end → Creating A Patch nits.

**Suggested review order:** (1) MIDI 00–05 + Memory Protect + controller table (2) ACT SENSE + Spillover (3) Cassette Errors + heading parents (4) Keypad sample pages (5) M1000 heading + ambiguity note (6) mechanical filet.

## Verification

**Manual checks (if no CLI):**
- Spot-check repaired zones against OCR `pNNN.txt` / CloudConverter headings for the same page numbers listed in the Build intent.
- `rg` filet on both MDs: `<a id`, `Continue with`, duplicate Cassette intro, end-marker / Warranty omit wording.
- Confirm `oberheim-matrix-1000-midi-sysex-implementation.md` unchanged (`git` / checksum if tracked; else mtime/size).
- Do not run plugin build/lint for this doc-only chore.
