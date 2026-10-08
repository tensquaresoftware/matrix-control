---
title: 'Getting Started STEP 4 Audio matches Settings AUDIO'
type: 'feature'
created: '2026-10-09'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Getting Started STEP 4 (Standalone Audio) only shows Driver Type, Input Device, and Synth From. Users cannot finish listening setup in the wizard the same way they can in Settings → AUDIO.

**Approach:** Make STEP 4 show the same controls as Settings → AUDIO, in the same order, including the same blank spacer rows. Grow the wizard dialog as needed (Audio step may exceed the old “shorter than Settings” height rule). Prefer embedding `SettingsAudioPage` in the control band so layout and behavior stay identical.

</frozen-after-approval>

## Implementation Notes

- Embedded `SettingsAudioPage` in STEP 4; removed digeste-only driver/input/synth combos.
- `kRowsAudio = SettingsShellMetrics::kTallestPageRows`; `mayExceedSettingsDialogHeight` for Audio only.
- Peak via `peakLevelProvider`; `enableInputMonitoring` on wizard open; SYNTH FROM refresh uses `applyAudioCatalogToSettings`.
- Updated journey/ui-copy/AC digeste wording; geometry contract tests allow Audio taller than Settings.

## Review Triage Log

- Metrics header still said always below Settings — **patch** (comment fixed).
- Magic `+240` body slack — **patch** (`kAudioBodyWrapSlackDesignPx`).
- `kRowsAudio` tied to tallest page — **false** for harm (AUDIO is that SSOT by design); clarified in comment.
- Spec Implementation Notes empty — **patch** (filled; status done).
- Parent SSOT still digeste — **patch** (journey/ui-copy/AC updated).
- Body copy incomplete for full Audio — **patch** (ui-copy).
- Audio height tests only `> 0` — **patch** (assert Audio exceeds Settings + 11 rows).
- Flags test title stale — **patch**.
- First paint skipped catalog sync — **patch** (`refreshGettingStartedWizardSynthFrom` after show).
- `populateSynthFromChannels` no-op without page — **patch** (`ensureAudioPage` first).
- No contextual help on embedded page — **defer** (wizard steps do not bind footer CH today).
- Missing `enableInputMonitoring` — **patch**.
- create-once deviceManager stale — **false** (manager is process-lifetime Standalone singleton).
- `getAudioPage()` exposes Settings type — **low** rejected (editor needs shared catalog apply helper).
