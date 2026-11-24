---
title: 'FR user manual — BANK UTILITY module coverage'
type: 'chore'
created: '2026-11-24'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'fe13ba2367a863ebb4dea2be663b00a794e58259'
context:
  - '{project-root}/_bmad/custom/manuel-utilisateur-redaction-fr.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The FR user manual §11 only documents bank buttons `0`–`9`. The live Bank Utility module also offers `COPY`, `PASTE`, `IMPORT`, and `EXPORT`, with RAM/ROM limits and Matrix-6/6R rules that readers cannot discover from the manual. §12 already mentions bank export/import in ways that contradict or orphan §11. The lexicon still defines a stale `Unlock` term that does not match the current UI.

**Approach:** Rewrite and extend §11 from code truth so it covers the real module actions, limits, Matrix-6/6R grey states, EXPORT folder flow, useful confirmations, and a short single-control-surface session note. Align the conflicting §12 bank I/O sentences so Computer Patches vs Bank Utility stay distinct. Replace the stale Lexique `Unlock` row with a correct `BANK LOCK` entry (Master Edit / machine lock — not Bank Utility). FR only; no EN pass.

**Decisions:**
- Lexicon: replace `Unlock` with `BANK LOCK` (Master Edit / hardware lock; not a Bank Utility control).
- Process: no draft PR after first push unless Guillaume asks later.

## Boundaries & Constraints

**Always:**
- Code / live UI is product truth (`BankUtilityPanel`, handlers, `PluginDisplayNames`); beta FR guide is a validated journey aid, not a text to paste.
- Keep UI labels in English with backticks; write body prose in French per `manuel-utilisateur-redaction-fr.md`.
- Document Matrix-6/6R accurately: bank buttons + `COPY` + `PASTE` greyed; `IMPORT` / `EXPORT` remain usable (child folder `PATCHES`, not `BANK N`). Do not claim the whole module is inert.
- Paste/import destinations on Matrix-1000: RAM banks `0`–`1` only; ROM `2`–`9` readable for select/copy/export; same-bank paste blocked.
- Distinguish Bank Utility `IMPORT` (folder → up to 100 single-patch files → device RAM) from Computer Patches `OPEN` (one single-patch file into the editor).
- Short session policy note (D-022-R6): during a Matrix-Control session, drive the synth via the plugin; front-panel bank/lock changes can desynchronize.
- Keep `BANK LOCK` (Master Edit) clearly separate from Bank Utility; never reintroduce an `UNLOCK` Bank Utility control as the primary path.
- Lexicon: replace `Unlock` with a correct `BANK LOCK` definition pointing at Master Edit / machine lock.

**Never:**
- EN manual; PATCH NAME / skins; Mutator; ASIO/JACK; new screenshots.
- Document the removed Bank Utility `UNLOCK` button / lock-dot as current UX.
- Invent SysEx opcode tours beyond what §12 already needs for Edit Buffer context.
- Commit / PR unless Guillaume asks.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Reader opens §11 | Current thin §11 | Full module coverage: `0`–`9`, `COPY`, `PASTE`, `IMPORT`, `EXPORT`, RAM/ROM, confirmations, session note | N/A |
| Matrix-6/6R reader | §11 intro | Banks/`COPY`/`PASTE` grey; `IMPORT`/`EXPORT` still described as usable | N/A |
| Reader compares §11 vs §12 | Bank whole-bank I/O | No contradiction: Bank Utility is the whole-bank path; Computer Patches is per-file | N/A |
| Lexicon lock term | Stale `Unlock` row | Row replaced by correct `BANK LOCK` (Master Edit / machine) | N/A |

</frozen-after-approval>

## Code Map

- `Documentation/User/Manuals/Matrix-Control-Manuel-Utilisateur.md` — §11 (~467–475), §12 bank I/O (~501–509), Lexique `Unlock` (~735); primary edit target
- `_bmad/custom/manuel-utilisateur-redaction-fr.md` — FR manual redaction SSOT (tone, labels, lists, no invented behavior)
- `Documentation/User/Beta/Matrix-Control-Guide-de-tests-beta-FR.txt` — validated Bank Utility journeys (facts only; do not copy verbatim); beta “fully grey on 6/6R” is stronger than code — prefer code
- `Source/GUI/Panels/.../BankUtilityPanel.{h,cpp}` — `setBankUtilityGrayed`, `refreshImportBankEnabled`, `refreshUtilityEnabled` (grey banks/COPY/PASTE; IMPORT/EXPORT stay)
- `Source/Shared/Definitions/PluginDisplayNames.h` — `BankUtilityModule` help / dialog strings (`BankImportConfirm`, `BankPasteConfirm`, `BankExportOverwriteConfirm`)
- `Source/Core/...` / PatchManager bank handlers — copy/paste/import/export limits; `DeviceMemoryLimits::{isRomBank,isPasteStoreAllowed,hasBankConcept}`
- `ClipboardService::canPasteBank` — same-bank paste blocked
- `PatchFileNameSanitizer::bankExportFileStem` — export names `Pxx. Name.syx`
- `_bmad-output/planning-artifacts/prds/.../.decision-log.md` — **D-022-R6** single control-surface policy (document in manual)
- Do **not** change: plugin source, EN manual, beta guide text, Master Edit UI

## Tasks & Acceptance

**Execution:**
- [x] `Documentation/User/Manuals/Matrix-Control-Manuel-Utilisateur.md` — Rewrite/extend §11 for full Bank Utility module (buttons, clipboard, import/export, RAM/ROM, Matrix-6/6R grey rules, EXPORT parent-folder flow + overwrite confirm, useful paste/import confirms, short D-022-R6 session note) — closes the coverage gap
- [x] `Documentation/User/Manuals/Matrix-Control-Manuel-Utilisateur.md` — Fix §12 sentences that deny whole-bank import and orphan Bank Utility export so §11 and §12 agree — removes contradiction
- [x] `Documentation/User/Manuals/Matrix-Control-Manuel-Utilisateur.md` — Replace Lexique `Unlock` with correct `BANK LOCK` (Master Edit / machine lock; not Bank Utility) — stops false UI vocabulary

**Acceptance Criteria:**
- Given a Matrix-1000 reader, when they read §11, then they understand `0`–`9`, `COPY`, `PASTE`, `IMPORT`, `EXPORT`, that paste/import target only RAM `0`–`1`, and that ROM `2`–`9` remain selectable/copyable/exportable.
- Given a Matrix-6/6R reader, when they read §11, then they learn banks/`COPY`/`PASTE` are greyed while `IMPORT`/`EXPORT` remain usable with a `PATCHES` child folder.
- Given §12, when compared to §11, then whole-bank write/export is attributed to Bank Utility and Computer Patches remains per-file (`OPEN` ≠ bank import).
- Given the Lexique, when scanned for Unlock/Bank Utility lock wording, then no entry presents a Bank Utility `UNLOCK` control as current UI.
- Given §11, when read end-to-end, then a short session note warns that front-panel bank/lock changes during a plugin session can desynchronize.

## Implementation Notes

- Implemented directly after subagent model quota failure; loaded `manuel-utilisateur-redaction-fr.md` first.
- Also aligned §3 comparison table, the §3 note, and the troubleshooting Matrix-6/6R row so they no longer claim Bank Utility is fully unavailable (would contradict §11 / code).
- I/O matrix rows are documentation reader outcomes — verified by inspecting the rewritten manual sections (no automated unit tests apply).
- Review patches: Matrix-6/6R IMPORT destination + folder-level pick; partial/over-100 import leftovers; PASTE grey rules; EXPORT cancel; §12 6/6R clause; troubleshooting split (no device vs 6/6R).

## Spec Change Log

## Review Triage Log

- Blind Hunter — IMPORT § only Matrix-1000 while intro promises Matrix-6/6R — **medium** — confirmed: subsection omitted 6/6R destination → **patch** (group A)
- Edge Case Hunter — Matrix-6/6R IMPORT with no bank concept — **medium** — same root as above → **patch** (group A)
- Blind Hunter — IMPORT « un dossier » ambiguous vs EXPORT parent/child — **medium** — confirmed: reader can pick wrong level → **patch** (group B)
- Blind Hunter — partial / >100 valid files leftover behaviour missing — **medium** — confirmed via `BankImportPlanner` cap + slot write loop → **patch** (group C)
- Edge Case Hunter — IMPORT 1–99 leftovers — **medium** — same as above → **patch** (group C)
- Edge Case Hunter — IMPORT >100 extras ignored — **medium** — same as above → **patch** (group C)
- Edge Case Hunter — IMPORT zero valid files / confirm order — **false** — confirm runs before scan; zero-valid aborts after confirm with footer, not « no confirm » as claimed
- Blind Hunter — title/TOC still Matrix-1000-centric — **low** — rejected (cosmetic rename beyond brief lot; intro already covers 6/6R)
- Blind Hunter — opening sentence Matrix-1000-centric — **low** — rejected (Matrix-6 paragraph immediately follows)
- Blind Hunter — empty clipboard PASTE omitted — **low** — real; smallest fix is one grey-state sentence → **patch** (group D)
- Edge Case Hunter — PASTE empty clipboard — **low** — same → **patch** (group D)
- Edge Case Hunter — PASTE ROM not stated as greyed — **medium** — text said write-if-RAM only; grey state now mirrored → **patch** (group D)
- Edge Case Hunter — EXPORT cancel/failure undocumented — **low** — cancel footer exists in code; brief cancel note added → **patch** (group E)
- Blind Hunter — §12 bank-centric vs §11 Matrix-6 IMPORT — **medium** — confirmed → **patch** (group A/F)
- Edge Case Hunter — §12 bank IMPORT bullet Matrix-6 — **medium** — same → **patch** (group A/F)
- Blind Hunter — troubleshooting missing ROM IMPORT line — **low** — rejected (already in §11; everyday path covered)
- Edge Case Hunter — troubleshooting conflates no-device and Matrix-6 — **medium** — confirmed: IMPORT/EXPORT disabled without device → **patch** (group G)
- Blind Hunter — screenshot legend ignores grey states — **low** — rejected (placeholders / captures out of scope)
- Blind Hunter — spec Code Map / empty change logs — **false** — rejected (fix would edit this build’s spec)
- Verification Gap — no findings — n/a
