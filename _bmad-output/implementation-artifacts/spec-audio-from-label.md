---
title: 'Rename Settings AUDIO listen combo to AUDIO FROM'
type: 'feature'
created: '2026-11-28'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Settings > AUDIO (standalone) and Settings > MIDI & DEVICE both show `SYNTH FROM`, so users (and testers) confuse the audio listen-channel combo with the MIDI synth-input port. Beta guides already need an explicit "not the same" caveat.

**Approach:** Introduce a dedicated `AUDIO FROM` display label for the AUDIO listen combo (and Getting Started STEP 4 / INPUT GAIN help that point at it). Keep MIDI `SYNTH FROM` / `SYNTH TO` unchanged. Align living user docs (FR manual, FR/EN beta guides) and bump product version to `1.0.0-alpha.9` with revision dates to 2026-11-28. Leave dated Development Plans archives as historical; do not rewrite them. Do not rename C++ identifiers or APVTS keys.

</frozen-after-approval>

## Implementation Notes

- Added `Settings::kAudioFromLabel` (`AUDIO FROM`); MIDI keeps `kSynthFromLabel`.
- Wired `SettingsAudioPage` + Getting Started STEP 4 body / INPUT GAIN help to the new label.
- Updated FR manual, FR/EN beta guides, version to `1.0.0-alpha.9`, revision dates `2026-11-28`.
- Left dated `Documentation/Development/Plans/` archives unchanged (historical).
- Review patches: `GettingStartedWizardContractTests`, `SettingsTabsShellTests::headerAudioProductCopy`, nearby comments.

## Review Triage Log

- Wizard contract still expects `SYNTH FROM` — high → patch (test updated).
- Settings shell test still pins MIDI label for AUDIO header copy — medium → patch (`kAudioFromLabel`).
- Stale AUDIO comments in wizard/editor — low → patch.
- Manual header tour omits naming `AUDIO FROM` — low → rejected (Settings/lexicon/troubleshoot already cover it).
- Beta guides lack extra Settings/`AUDIO FROM` exploration outside wizard — low → rejected (wizard STEP 4 checklist updated; expansion is optional).
- Removed dual-label caveat without replacement — false (rename removes the collision; lexicon still distinguishes).
- Beta session notes omit `AUDIO FROM` — low → deferred enrichment.
- Spec still `in-progress` — process → finalized to `done`.

