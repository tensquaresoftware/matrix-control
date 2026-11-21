---
title: 'Beta test guide (FR + EN TXT)'
type: 'chore'
created: '2026-11-19'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context:
  - '{project-root}/Documentation/User/Matrix-Control-Manuel-Utilisateur.md'
  - '{project-root}/Scripts/release/templates/RELEASE_NOTES.template.md'
  - '{project-root}/CONTRIBUTING.md'
  - '{project-root}/_bmad-output/implementation-artifacts/deferred-work.md'
  - '{project-root}/Source/Shared/Definitions/PluginDisplayNames.h'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Matrix-Control has no actionable beta test guide. Guillaume needs multi-OS smoke scenarios next week, and external beta testers will later receive the same guide in the release ZIP with the docs.

**Approach:** Author two plain `.txt` manuals (French first as reference, then English equivalent) under `Documentation/User/beta/`, structured as checkable test scenarios (preconditions, steps, expected result, failure notes) covering install friction, core panels, Standalone vs plugin, known limitations, bug reporting, plus short smoke and long full checklists.

</frozen-after-approval>

## Implementation Notes

Agent decisions (user would not need to renegotiate these):
- Paths: `Documentation/User/Beta/Matrix-Control-Guide-Beta-Tests-FR.txt` (working); archives `…-remaniee-Cursor.txt` + `…-remaniee-Guillaume.txt` (EN deferred until FR approved)
- Format: working Markdown + delivery TXT — UPPERCASE top titles in TXT, `1.` / `1.1.` numbering, ASCII separators, `[ ]` / `[x]` checkboxes; UI labels stay English; PDF out of scope
- Keep product UI labels in English in both languages (GETTING STARTED, PATCH EDIT, BANK UTILITY, …)
- ASIO: cover Standalone audio with and without ASIO; never present ASIO as guaranteed for first public beta
- Skin Designer: optional one-line "authoring tool, not included"
- Non-notarized macOS / SmartScreen: normal procedures, not bugs
- OS-specific install/friction in dedicated subsections (macOS ARM/Intel, Windows 10/11, Linux Ubuntu)
- Content spine from manuel TOC + release install paths + deferred known limits (Live Auto-Scale, Unison, Keyboard Mode / Untergeek, Master/Bank grayed on 6/6R)
- Do not rewrite `Matrix-Control-Manuel-Utilisateur.md`; guide may point to it in one sentence if useful
- Branch: `docs/beta-test-guide`; draft PR after first push (user confirmed)

Implemented 2026-11-19:
- Created both TXT guides (FR reference then EN equivalent), sections 1–17 matching scope
- UTF-8 with proper French accents in FR file
- No Markdown sources; no Skin Designer content beyond exclusion note
- Post-review patches: N/A convention, RAM backup warning, MEMORY PROTECT, grayed checkboxes in §§9–10, codesign wording, Win non-admin VST3 path, Mutator AMOUNT/RANDOM, Unison/Keyboard Mode where-to-look, bug-channel placeholders, guide version date, FR/EN identity parity, smoke relaunch + owner vs external §17 split, FR manual language note in EN
- 2026-11-19 FR rewrite v1.1 (tester-facing tone): welcoming intro, no owner name, no Skin Designer, no UTF-8/langue meta, plain “sans objet”, identity sheet without underscores, version via ABOUT not build/tag/commit, “parcours rapide” instead of smoke, verbose 1.4 for non-technical testers; EN deferred until FR approved

## Spec Change Log

## Review Triage Log

- Blind Hunter floor N=7 from ~46 kB; 15 findings returned.
- Backup before destructive writes — high/medium → patch (prereq + bank export note).
- Orphaned grayed tick instruction §§9–10 — medium → patch (explicit checkboxes).
- Missing N/A/skip for OS/hardware smoke lines — medium → patch (header convention + smoke N/A).
- AU checkbox looks like fail on Win/Linux — medium → patch (N/A wording; already said macOS-only).
- Expected result mentioned ad-hoc codesign without tester action — medium → patch (clarify CI signs; tester only xattr + Open).
- MEMORY PROTECT / BANK LOCK absent — medium → patch (prereq + §14).
- Unison / Keyboard Mode not actionable — medium → patch (RAMP/PORTAMENTO cues).
- No bug-channel placeholder / guide revision — medium → patch (§15 + header version).
- EN points to French manual without note — medium → patch.
- FR vs EN identity intro mismatch — low → patch (aligned).
- §17 multi-OS as one session — medium → patch (A external / B owner).
- Spec still in-progress — process → patch (status done).
- Windows VST3 admin-only path — medium → patch (user/custom DAW path).
- Mutator AMOUNT/RANDOM not first-class — medium → patch.
- Persistence after relaunch missing — medium → patch (smoke + full path).
- No deferrals.

