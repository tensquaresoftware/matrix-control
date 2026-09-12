---
title: 'Oberheim owners manuals fidelity audit (Preview OCR)'
type: 'audit'
created: '2026-09-12'
status: 'done'
baseline_commit: 'f146e2d70015ef4187cd0d7fc653a7cf5ec2c5e7'
sources:
  pdf_preview:
    - "/Users/Guillaume/Downloads/Oberheim Matrix 6 - Owner's Manual (Régénération Aperçu).pdf"
    - "/Users/Guillaume/Downloads/Oberheim Matrix 6R - Owner's Manual (Régénération Aperçu).pdf"
    - "/Users/Guillaume/Downloads/Oberheim Matrix-1000 - Owner's Manual (Régénération Aperçu).pdf"
  md:
    - '_local/References/Oberheim/oberheim-matrix-6-6r-owners-manual.md'
    - '_local/References/Oberheim/oberheim-matrix-1000-owners-manual.md'
extract_corpus: '/tmp/oberheim-preview-ocr/{matrix6,matrix6r,matrix1000}.txt'
reproduce: 'python3 -c "import fitz; …" page.get_text() per PDF into that corpus (Preview text layer only; do not re-OCR)'
---

# Oberheim owners manuals — Preview fidelity audit

## Global verdict

**Usable as-is.** F1 and F2 were patched into the merged 6/6R MD (2026-09-12 follow-up). Matrix-1000 Intro→Ch.4 remains OK on the high-value spots checked. SysEx companion untouched.

## Findings status (after F1/F2 patch)

| ID | Status |
| --- | --- |
| F1 | **patched** — Matrix-6R Creating A Patch `17. DC2 LEV` callout set to BOTH |
| F2 | **patched** — restored MIDI MONO intro / Using MIDI MONO / BASIC CHANNEL lead-in / SPLIT note; kept channel table + auto-change bullets |
| F3 | **patched** — Spillover Split chart corrected from Guillaume’s clean crop (row order 2/4→4/2→0/6→6/0; MIDI OUT always N / N+1; OCR `—` cells removed) |
| F4 | **dismiss (clarified)** — CloudConverter “picture text” is a simple ON/OFF checklist for `03`/`12`/`15`/`17`, which the MD already has; Preview multi-state dump was OCR noise, not a missing matrix |

## Method (spot audit)

- Extracted Preview “Integrate Text” layers via PyMuPDF `fitz` (not re-OCR).
- Compared high-value zones only — not full line-by-line.
- Ignored Preview OCR typos (`MIDl`, `orto`, glyph mush) when meaning was clear.
- Out of scope by design: Warranty / NOTES / worksheets / diagrams / SysEx companion body.

## Audit notes (checked OK / non-findings)

| Note | Detail |
| --- | --- |
| Matrix-6R Preview p.4 | Empty text layer (no content invented). |
| Preview font warnings | MuPDF `unknown cid font type` on 6/6R extracts; text still usable for judgment. |
| Coverage markers | Both MDs correctly end before Warranty / NOTES / worksheets; M1000 Ch.5 remains a SysEx pointer. |
| Prior `source conflict` annotations | Pedal 1 vs Pedal 2 External Trigger (Matrix-6) and `56 STEREO` vs “56–99 no function” still match print ambiguity; Preview does not resolve hardware truth. |
| 6/6R TOC / Part structure | Spot TOC vs Preview section order OK for in-coverage parts; no actionable gap. |
| Master MIDI `00`–`05` / controller defaults | Spot-aligned with Preview intent after prior repair. |
| Keypad / Mode Select sample | Spot-aligned; no new actionable condensation vs Preview. |
| Vibrato `30`–`36` / Misc EDIT RECALL / INIT PATCH | Nesting and STEPs match Preview intent on spot check. |
| ACT SENSE `14` | Prose present; no new gap vs Preview. |
| VCF `23`–`25` / Cassette `21`–`22` | Spot compare matches Preview (prior condensation concerns closed). |
| SPLIT EDIT STEPs 1–6 prose | Matches Matrix-6 Preview print intent; diagram pages omitted by design. |
| Matrix-1000 | Basic Modes nesting + Special Modes → Units OK; Bend Range “eight / 0–7” vs `#8` ambiguity note still correct; Data Dump table/flow OK; Ch.5 pointer / Ch.6 Warranty omit intentional. Optional deeper TOC child listing is product preference, not a fidelity gap. |
| MIDI MONO on Matrix-6 | Not on Matrix-6 Master MIDI list (6R-only); no Matrix-6 prose expected. |
| OCR noise / OOS zones | Typos ignored when meaning clear; Warranty / NOTES / worksheets / SysEx companion not treated as gaps. |
| Owners MD / SysEx SHA-256 (unchanged) | `oberheim-matrix-6-6r-owners-manual.md` `07cd4d83e36472bce6444d9f33dbcada372b9e75472110954f65e10491de8b71`; `oberheim-matrix-1000-owners-manual.md` `13a896eb235ae9b1292056fe88fbc1e126dae9d506c825dfdd8c841605079bad`; `oberheim-matrix-1000-midi-sysex-implementation.md` `1a7396aa7dc9c65cf2c92152ecc8e7176968ab259245bcce08d806c28f0efd0e` |

---

## Findings (actionable only)

### F1 — Matrix-6R Creating A Patch: `17. DC2 LEV` default wrong

| Field | Value |
| --- | --- |
| Severity | **patch** |
| PDF | Matrix-6R Preview p.99 |
| MD anchor | `oberheim-matrix-6-6r-owners-manual.md` ~L2282–2284 (`### STEP 3` / `**17. DC2 LEV**`); chart Levers cell ~L2266 |
| Evidence | Preview: `17 DC2 LEV - … VALUE = BOTH.` Chart header row prints `Wsel Levers Keybd` / `BOTH BOTH KYBD`. MD callout: `**Matrix-6R:** Default setting = ON.` while the STEP 3 DCO2 chart already has Levers `BOTH`. Matrix-6 callout `BOTH` matches Matrix-6 Preview. |
| Recommendation | When patching: set Matrix-6R `17. DC2 LEV` callout to **BOTH**; leave Matrix-6 `BOTH` and Matrix-6R `07. DC1 LEV` = **ON** as-is. |

### F2 — Matrix-6R `18. MIDI MONO`: intro / “Using MIDI MONO” condensed

| Field | Value |
| --- | --- |
| Severity | **patch** |
| PDF | Matrix-6R Preview p.77–79 |
| MD anchor | `### 18. MIDI MONO — Channel Map` (~L1673+) |
| Evidence | MD keeps channel table + automatic-change bullets 1–4. Missing vs Preview: (a) polyphonic Single vs SPLIT vs MONO intro and ON/OFF behavior (p.77); (b) heading **Using MIDI MONO** + sequencer / MIDI-guitar use cases (p.77); (c) BASIC CHANNEL `N` assignment lead-in before the table (p.77); (d) SPLIT note that 2/4 & 4/2 follow the table while 0/6 & 6/0 ignore the zero-voice zone (p.78). Channel table itself is usable; Preview’s numeric dump is noisier than the MD table. |
| Recommendation | Restore the missing Oberheim prose from Preview (prefer meaning over OCR typos); do not invent wrap rules beyond print. |

### F3 — Spillover Split chart residual mush

| Field | Value |
| --- | --- |
| Severity | **defer** |
| PDF | Matrix-6R Preview p.71 |
| MD anchor | Spillover chart under `#### SPILLOVER in Split Mode` + footnote |
| Evidence | Preview VOICE/ZONE / MIDI OUT / SPILLOVER grid still column-scrambled. MD already records `0/6` Upper spill = `N+3` and annotates unmarked MIDI OUT Lower for `0/6`. |
| Unblock | Cleaner table crop or scan where column headers and all six VOICE/ZONE rows are readable without inventing cells. |
| Recommendation | Leave annotated chart until that source exists. |

### F4 — Patch Map Echo ON/OFF combination chart flattened

| Field | Value |
| --- | --- |
| Severity | **defer** |
| PDF | Matrix-6R Preview p.77 |
| MD anchor | Checklist table after `### 17. P MAP ECHO` |
| Evidence | Preview shows a multi-state combination chart for `03` / `12` / `15` / `17`; MD lists the four parameters with a simple ON/OFF column. OCR of the full matrix is not reliable enough to reconstruct without inventing cells. Surrounding Patch Map prose looks adequate on spot check. |
| Unblock | Clean table crop with complete ON/OFF matrix cells for the four parameters. |
| Recommendation | Optional later restore; not blocking daily use. |

---

## Counts

| Severity | Count |
| --- | --- |
| decision | 0 |
| patch (open) | 0 (F1, F2 applied 2026-09-12) |
| defer / best-effort | 0 |
| dismiss | 1 (F4 checklist clarified) |

## Suggested next step (human)

Owners MD retouches F1/F2/F3 are done. F4 was a false alarm. No SysEx companion edits.
