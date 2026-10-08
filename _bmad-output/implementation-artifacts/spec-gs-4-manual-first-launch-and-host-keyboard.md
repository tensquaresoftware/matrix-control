---
title: 'GS-4 Manual — first launch and host keyboard'
type: 'chore'
created: '2026-10-08'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'b0a7daefd82c82c0e2d8232eef2694613dc7dd0b'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/epic-gs-context.md'
  - '{project-root}/_bmad/custom/manuel-utilisateur-redaction-fr.md'
  - '{project-root}/Documentation/Development/Plans/2026/10/2026-10-08-Getting-Started-Wizard-Decisions.md'
  - '{project-root}/_bmad-output/specs/spec-getting-started/ui-copy.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-gs-3-getting-started-steps-flags-plugin-standalone.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The GETTING STARTED wizard (GS-1…GS-3) ships live; the French user manual still describes first open as logo `AUDIO/MIDI...` / header ports only, has no Getting Started journey, and gives plugin keyboard routing as a one-liner while the wizard STEP 3 points readers to “the user manual for host examples.”

**Approach:** Update the user-facing manual so first launch documents GETTING STARTED (plugin vs Standalone step set, Configure later / Settings re-run), and add concrete host/DAW master-keyboard routing examples that stay out of the wizard body. Refresh any adjacent manual lines that would contradict current Settings / onboarding (Scale, Skin, Getting Started block).

## Boundaries & Constraints

**Always:**
- Follow `_bmad/custom/manuel-utilisateur-redaction-fr.md` (tone, product vocabulary, no developer jargon, UI labels in English backticks, screenshot placeholders OK).
- Cover first launch / GETTING STARTED: auto-open when incomplete, steps 0–4 by format (plugin ends at Keyboard; Standalone includes Audio), `CONFIGURE LATER` / one reminder then silence, Settings → User Interface Getting Started combo (`SHOW WHEN INCOMPLETE` / `NEVER AT LAUNCH`) and `RUN SETUP AGAIN`.
- Cover master-keyboard via host for plugin use with detailed DAW examples in the manual only (wizard keeps the short pointer).
- **Decision — Languages:** French only. Update `Documentation/User/manuel-utilisateur.md`; leave `user-manual.md` as “à venir” (no EN chapter in this story).
- **Decision — DAW example set:** Concrete master-keyboard → Matrix-Control track steps for **Ableton Live**, **Reason**, and **Logic Pro** (hosts Guillaume can verify). Other hosts may be named briefly as “same idea” without full walkthroughs.
- Align §14 (and related) so UI Scale / Skin / Getting Started match GS-1 (Settings User Interface order; logo shortcuts remain).
- Keep MIDI port / detection facts honest vs current UI; do not invent Device Setup as the current first-run path.
- English product UI strings; French manual prose.

**Never:**
- Put detailed DAW walkthroughs into wizard/code copy (`PluginDisplayNames` STEP 3 plugin text stays the short pointer unless a real doc URL is later approved).
- Create or expand `Documentation/User/user-manual.md` in this story.
- Rewrite the whole manual or reopen Epic GS product decisions (pastilles, mega-modal, etc.).
- Copy developer docs (`Documentation/Development/…`) into the user journey.
- Treat Audio Settings Matrix rebuild or code changes as part of this story.
- Invent screenshot assets; placeholders with captions only.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| First Standalone open | Reader follows new first-launch section | Sees GETTING STARTED walk (Scale → Synth → Keyboard optional/Skip → Audio) and where to re-run from Settings | N/A |
| First plugin open | Reader follows first-launch + host keyboard | Sees no Audio step; Keyboard is host/DAW; finds DAW examples for routing notes to the plugin track | N/A |
| Configure later | Reader skips wizard | Manual explains Settings finish path + reminder/silence / RUN SETUP AGAIN without inventing Device Setup | N/A |
| Wizard pointer | Reader clicks through from STEP 3 plugin copy | Manual section answers “host examples” without requiring a code path/URL | N/A |
| Stale §14 | Reader checks Scale/Skin location | Manual no longer claims appearance is logo-only; Settings User Interface listed | N/A |

</frozen-after-approval>

## Code Map

- `Documentation/User/manuel-utilisateur.md` -- Primary deliverable: add/extend first-launch GETTING STARTED; DAW host-keyboard examples (Ableton Live, Reason, Logic Pro); refresh §14 Settings User Interface (Scale/Skin/Getting Started); light §4/§5/§15 cross-links so plugin notes and Standalone audio match wizard reality.
- `Documentation/User/user-manual.md` -- Remains “à venir”; do not create in this story.
- `_bmad/custom/manuel-utilisateur-redaction-fr.md` -- Authoring SSOT (read-only): FR unique target, tone, vocabulary, no Dev paths in user journey.
- `Documentation/Development/Plans/2026/10/2026-10-08-Getting-Started-Wizard-Decisions.md` -- Frozen product journey + STEP 3 plugin pointer copy (read-only).
- `_bmad-output/specs/spec-getting-started/{ui-copy,journey-and-flags,acceptance-criteria}.md` -- Wizard copy and “manual-only DAW examples” constraint (read-only).
- `Source/Shared/Definitions/PluginDisplayNames.h` -- `kBodyMidiKeyboardPlugin` points to user manual; do not expand with DAW steps in this story.
- `_bmad-output/implementation-artifacts/spec-gs-3-*.md` -- Continuity: wizard owns onboarding; GS-4 is docs only.
- `Documentation/Development/windows-midi-multi-client.md` -- Optional cross-link for Windows port exclusivity only; not a substitute for host-keyboard examples.

**Reuse:** Existing §4 install/open, §5 MIDI ports, §15 plugin vs Standalone table as anchors; expand rather than duplicate full MIDI wiring.

**Do not change:** Wizard/dialog C++ (unless a later decision adds a real manual URL — out of default scope); audio-safety docs; Epic GS product direction.

## Tasks & Acceptance

**Execution:**
- [x] `Documentation/User/manuel-utilisateur.md` -- Add first-launch / GETTING STARTED guidance (TOC + section or clear subsections under install/open); document plugin vs Standalone step differences, Configure later, Settings Getting Started controls.
- [x] `Documentation/User/manuel-utilisateur.md` -- Add master-keyboard via host / DAW routing examples for Ableton Live, Reason, and Logic Pro; cross-link from plugin Keyboard / §15.
- [x] `Documentation/User/manuel-utilisateur.md` -- Refresh §14 (and any contradictory §4/§6 logo/Settings lines) for UI Scale, Skin, and Getting Started in Settings User Interface.
- [x] Manual consistency pass -- Lexicon / troubleshooting touch-ups only if new terms or symptoms are introduced; leave EN `user-manual.md` as “à venir”; no wholesale rewrite.

**Acceptance Criteria:**
- Given the shipped GETTING STARTED wizard, when a reader opens the updated manual, then first launch / Getting Started and Settings re-run / suppress controls are described honestly for plugin and Standalone.
- Given plugin STEP 3 copy pointing to the user manual, when the reader follows that promise, then the manual contains detailed host/DAW master-keyboard routing examples (not only a one-liner).
- Given Settings User Interface after GS-1, when the reader checks appearance and Getting Started, then the manual no longer claims Scale/Skin are logo-only and documents the Getting Started block.
- Given the wizard body, when this story ships, then detailed DAW examples remain manual-only (no expansion of wizard STEP 3 into per-host tutorials).

## Implementation Notes

- Updated `Documentation/User/manuel-utilisateur.md`: new §5 GETTING STARTED, host-keyboard examples (Ableton Live / Reason / Logic Pro) under §6, §15 USER INTERFACE refresh, renumbered TOC 5→18; EN `user-manual.md` left “à venir”.
- Parent-pass fix: removed developer wording “one-shot Device Setup” from §5 intro (redaction SSOT).
- Matrix rows covered by manual content inspection (docs chore — no unit tests).
- Build review patches (manuel only): resume-at-incomplete, SKIP completion, SYNTH FROM MIDI vs audio, CONFIGURE LATER intro-only, silence/Esc policy, Reason steps, lexicon order, DAW screenshot placeholder, §4 wording align, drop duplicate Windows note.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence |
|---------|---------|----------|
| BH: auto-reopen resumes at first incomplete step, not intro | medium | Confirmed vs `decideAutoOpen` / journey-and-flags: `hasLeftIntro` → first incomplete; intro only first contact / RUN SETUP AGAIN. Manual §5 omits this. |
| BH: SKIP marks Keyboard done | medium | Confirmed journey-and-flags + Flow `marksContentStepDone` for Skip. Manual only describes “passer”. |
| BH: SYNTH FROM MIDI vs audio channel collision | medium | Confirmed dual UI uses same label (Settings MIDI port vs AUDIO listen channels). Manual mixes without disambiguation. |
| BH: CONFIGURE LATER listed as general nav button | medium | Confirmed Flow: Configure Later only step 0. Manual button list implies always present. |
| BH: “second CONFIGURE LATER (or equivalent)” unexplained | medium | Product SSOT uses “or equivalent”; code silence also via `consumeOneReminder` on reminder auto-open. Manual leaves “équivalent” opaque. |
| BH: GETTING STARTED lexicon out of alpha order | low | Confirmed between DAW and DCO; trivial reorder. |
| BH: Windows MIDI note duplicates following subsection | low | Confirmed near-identical paragraphs under DAW examples and “Windows : port déjà utilisé”. |
| BH+EC: Reason steps too vague vs Live/Logic | medium | Confirmed step 2 is “selon votre version” without concrete rack/MIDI gestures. |
| BH: no screenshot placeholder for DAW keyboard section | low | Redaction SSOT wants placeholders for major new UI topics; section has none. |
| BH: §4 plugin “peut” vs Standalone “guide” certainty mismatch | low | Same auto-open mechanism; wording inconsistency only. |
| BH: Code Map / Implementation Notes stale section numbers & status | false | Fix would edit this build’s spec tracking notes; not a user-manual defect. |
| EC: Esc/outside dismiss does not advance Configure-later arm | medium | Confirmed: Esc/`requestDismiss` closes without `onConfigureLater`; arm unchanged → reopen every launch while incomplete. |
| EC: claim that only a second CONFIGURE LATER silences | medium | Confirmed: OneReminder → Silenced also when reminder auto-open sets `consumeOneReminder`. Manual overstates need for a second button click. |
| VG: no verification gaps | false | Docs-only change; no runtime tests expected. |

## Design Notes

- Prefer weaving GETTING STARTED into the early journey (§4 or a new numbered section inserted after install) rather than a late appendix, so first-time readers hit it before deep editing chapters.
- Host examples: short numbered steps for Ableton Live, Reason, and Logic Pro (create/select track → load Matrix-Control → arm/record or MIDI input from master keyboard → notes reach plugin). Call out that synth audio still returns via the audio interface / DAW input, separate from MIDI editing.
- Do not resurrect Device Setup as the documented first-run vehicle; mention only if needed to say the guided path is now GETTING STARTED.

## Verification

**Manual checks (if no CLI):**
- TOC links resolve to new/updated headings
- FR prose matches redaction SSOT phrase-test; UI labels remain English backticks
- Plugin path: Keyboard section + DAW examples answer the wizard pointer
- Standalone path: Audio step and Settings Audio/MIDI still consistent
- §14 no longer contradicts Settings User Interface (Scale/Skin/Getting Started)
- No developer paths or build jargon in user-facing sections
