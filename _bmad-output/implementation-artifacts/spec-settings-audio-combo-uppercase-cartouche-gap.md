---
title: 'Settings AUDIO combo uppercase and header cartouche gap 28px'
type: 'chore'
created: '2026-10-06'
status: 'done'
route: 'oneshot'
baseline_commit: 'eb4cab7c1eb3f9ede479e4edcd7179f54f7ea5df'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-settings-audio-tab.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-settings-audio-channel-pair-combos.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** In Settings > AUDIO, Driver Type / Input Device / Output Device combo items still show mixed-case OS or driver names, unlike other Matrix combos that display uppercase. The two gaps between the header EDIT / MIDI / AUDIO cartouches are still 16 design px instead of the intended 28.

**Approach:** Uppercase those three AUDIO combo item texts on populate (keep API/device identity strings unchanged for apply), and set the shared header cartouche gap token to 28 design px.

</frozen-after-approval>

## Implementation Notes

- Display uppercase on Driver Type + Input/Output Device combo items (`SettingsAudioPageDevices.cpp`).
- Driver `onChange` resolves type by selected id from `getAvailableDeviceTypes()` (not combo text).
- Device apply keeps scanned original-case names by id; unexpected/orphan selection keeps live setup name (never uppercased display text).
- `kCartoucheGap = 28` in `DesignPanels.h`; `DesignChecks.h` pins `== 28` and ÷4.
- Quality gate: `lint_touched.py` OK.
- Review: patched unexpected-id fallback + gap assert; deferred missing unit tests and 50% UI Scale cartouche smoke.

## Review Triage Log

- low — Spec notes contradicted themselves on orphan `getText()`. Rejected as code issue; cleaned notes on finalize.
- low — Spec status still `in-progress` mid-oneshot. Process timing; set `done` on finalize.
- medium — Unexpected combo id returned `{}` and could clear an endpoint. Patched: fall back to live setup name.
- low — Hard-coded combo ids vs `kFirstDeviceItemId`. Rejected: pre-existing pattern; refactor not worth this polish.
- low — Two anonymous namespaces in one `.cpp`. Rejected: cosmetic, not user-facing.
- medium — Gap only checked `% 4`, not exact 28. Patched: `static_assert(kCartoucheGap == 28)`.
- medium — No unit tests for driver-by-id / orphan apply. Deferred (manual smoke).
- maybe-false — Cluster shift at smallest UI Scale after +12 px gaps. Deferred unverified visual smoke.
