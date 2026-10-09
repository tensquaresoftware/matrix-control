# Epic GS Context: Getting Started wizard

<!-- Compiled from planning artifacts. Edit freely. Regenerate with compile-epic-context if planning docs change. -->

## Goal

Ship a dedicated multi-step **GETTING STARTED** assistant that unlocks edit, play, and hear on first useful contact — by format (plugin vs Standalone) — and replace the Device Setup one-shot as the first-run path. Settings → User Interface gains UI Scale and Skin; Settings → GETTING STARTED holds SETUP WIZARD (auto-open / RUN SETUP AGAIN); MIDI & DEVICE merges ports, DEVICE, and EPROM. Completion is tracked per applicable step, not one shared “setup done” flag for both formats.

## Stories

- Story GS-1: Settings User Interface — Scale, Skin, Getting Started block
- Story GS-2: Getting Started wizard shell and navigation
- Story GS-3: Getting Started steps, flags, plugin vs Standalone
- Story GS-4: Manual — first launch and host keyboard

## Requirements & Constraints

- Unlock usage by format: Standalone needs appearance, synth MIDI/EPROM, optional keyboard, and audio monitoring; plugin must not walk OS Audio or Keyboard From like Standalone.
- Settings → User Interface option order: UI SCALE, SKIN, INFO MESSAGE, CONTEXTUAL HELP. Logo menu Scale/Skin shortcuts remain.
- Settings → GETTING STARTED (first tab): SETUP WIZARD combo `SHOW WHEN INCOMPLETE` (default) / `NEVER SHOW AT LAUNCH`, with `RUN SETUP AGAIN` below (sole re-run control).
- Settings → MIDI & DEVICE merges synth ports, DEVICE (read-only), EPROM TYPE, and plugin HARDWARE LATENCY.
- Wizard steps (high level): 0 intro → 1 Scale/Skin → 2 SYNTH FROM/TO + DEVICE + EPROM → 3 keyboard (Standalone combo+Skip vs plugin host-informative) → 4 Audio Standalone only (Settings AUDIO embed).
- Per-step durable flags drive auto-open and targeted resume; plugin and Standalone must not share a single setup-done flag. Distinct from the audio-safety first-run Input None gate.
- Configure later from intro: one same-format auto reminder then silence (other format does not open while armed) unless a newly applicable step appears (e.g. first Standalone Audio).
- Product direction is frozen: no pastilles, mega-modal, Settings-only auto-open, manual-only, or Device Setup-only Standalone paths. No detailed DAW examples in the wizard body (manual only).
- English UI strings only; ASCII display-string rules apply. Frozen button vocabulary: CONFIGURE LATER / CONTINUE / PREVIOUS / NEXT / SKIP / FINISH — no QUIT, no SPECIFY LATER.
- GS-4 is a documentation deliverable (first launch + host keyboard routing); not a code Spec detail. Audio Settings Matrix rebuild stays a separate chantier. Do not reopen Epic 7/8 as incomplete.

## Technical Decisions

- Absorb Device Setup onboarding into wizard STEP 2; retire CONFIRM / SPECIFY LATER / sole `promptDone`-style completion as the first-run gate. Historical Device Setup specs remain implementation record only.
- Reuse Settings/header MIDI and audio population — never fork a second device-list source of truth inside the wizard. STEP 2 writes live like Settings/header; Next marks Synth Communication done (no dedicated Confirm).
- Machine prefs: per-step flags + auto-open combo + Configure-later silence, with plugin vs Standalone applicability. Exact property key names are a Build naming choice consistent with existing machine-defaults style.
- Migration assumption: if legacy EPROM-prompt-done is already true, mark Synth Communication complete and leave other step flags incomplete so Scale / Keyboard / Audio can still auto-open under `SHOW WHEN INCOMPLETE`.
- `RUN SETUP AGAIN` resets flags applicable to the current format and opens step 0; launch auto-open only resumes at the first applicable incomplete step (no reset).

## UX & Interaction Patterns

- Chrome matches Settings (Matrix monochrome); design width equals Settings; per-step height is normally lower than Settings (STEP 2 with firmware suffix and STEP 4 AUDIO embed may exceed). Step titles live in the title band; body carries short help copy and few controls.
- STEP 1 exists so UltraWide/HiDPI users can fix Scale before denser steps.
- STEP 4 embeds the same controls as Settings → AUDIO (full order including blank spacers).
- Last applicable step uses FINISH (plugin ends at STEP 3; Standalone at STEP 4).

## Cross-Story Dependencies

- Recommended order: GS-1 (Settings UI) before or as opening work of the wizard; then GS-2 shell; then GS-3 steps/flags/absorb Device Setup. GS-4 (manual) may proceed in parallel as docs.
- Within epic: shell and navigation (GS-2) underpin step content and flag behaviour (GS-3); Settings block (GS-1) owns auto-open preference and re-run entry.
- Outside epic: supersedes future Device Setup–as-onboarding work; does not roll back shipped Settings unification; smoke Audio Settings Matrix rebuild stays after / outside this epic.
