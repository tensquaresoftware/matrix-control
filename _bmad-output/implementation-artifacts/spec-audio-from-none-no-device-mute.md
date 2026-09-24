---
organization: Ten Square Software
project: Matrix-Control
title: Spec — Audio From None Without Device Mute
author: BMad Agent
type: bugfix
created: '2026-09-24'
status: done
route: oneshot
baseline_commit: '72b49f116a9ad8be0e70dd9b6beb976864e38b14'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** In standalone, choosing Audio From **NO INPUT** flips JUCE `StandalonePluginHolder` input mute (`muteInput`), which shows the sticky “Audio input is muted…” banner above the header and mirrors mute in Logo → Audio/MIDI — even though the user only wanted to temporarily stop Matrix-Control’s input passthrough and then pick another source from the combo.

**Approach:** Treat **NO INPUT** as a Matrix-Control software gate (silence passthrough / peak) while leaving JUCE device monitoring unmuted so the banner and native mute checkbox stay off; keep clearing `muteInput` when a real source is selected so default JUCE feedback protection does not strand users.

</frozen-after-approval>

## Implementation Notes

- Root cause: empty `audioFromSourceId` called `disableInputMonitoring()` → `getMuteInputValue().setValue(true)` (sticky JUCE banner). Passthrough still mapped channels for empty ids.
- Fix: `AudioPassthroughProcessor::setPassthroughActive(false)` for empty source id; standalone clears `muteInput` only when a non-empty source is applied; never `disableInputMonitoring` for Audio From.
- Review patches: skip writing `audioFromChannelMode` on empty id (preserve last mode); route maps before re-arming passthrough; silence before deactivating; round-trip unit test.
- Manual UAT: Standalone → pick a live Audio From source (banner off, peak moves) → **NO INPUT** (silence, no banner, Settings mute stays off) → pick a source again (audio returns).
- Out of scope (chat only): replacing Logo → Audio/MIDI native dialog.

## Review Triage Log

- BH: empty `setAudioFromSourceId` clobbers `audioFromChannelMode` via `channelModeForSourceId("")` — **medium/patch** — skip property write when empty.
- BH: always `enableInputMonitoring` on empty conflicts with “don’t flip mute” — **medium/patch** — enable only when source selected; never disable for Audio From.
- BH: `setPassthroughActive(true)` before channel maps races audio thread — **medium/patch** — map then arm; disarm first on empty.
- BH: no reactivation unit test — **medium/patch** — added round-trip test.
- BH: no automated muteInput regression harness — **medium/defer** — needs StandalonePluginHolder seam.
- BH: thin oneshot acceptance / empty change log while in-progress — **low/reject** — oneshot omits full AC; status finalized after review; manual UAT note added.
- BH: cold start invents stereo/mono when source id empty — **medium/defer** — pre-existing restore; not this banner fix.
- BH: mono index lingers while inactive — **low/reject** — unused while passthrough inactive; next real source rewrites.
- BH: `disableInputMonitoring` API remains as footgun — **low/reject** — call sites for Audio From removed; API still valid for other use.
- BH: header comment ties Core API to UI label — **low/patch** — comment now states empty `audioFromSourceId` contract.

## Spec Change Log

## Design Notes
