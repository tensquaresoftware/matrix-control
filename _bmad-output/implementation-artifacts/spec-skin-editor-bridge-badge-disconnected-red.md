---
title: 'Skin Editor BRIDGE badge red when disconnected'
type: 'feature'
created: '2026-10-30'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** When Matrix-Control is disconnected, the Skin Editor BRIDGE badge stays muted grey like the connected state, so the disconnect is easy to miss at a glance.

**Approach:** Paint the BRIDGE badge with the existing Skin Editor chrome danger red while disconnected; keep the connected fill as the current muted grey. Reuse `SkinEditorChrome::danger()` (already used for SAME COLOUR / Compare blink). Do not invent a new colour. Connected state stays grey (no green fill unless requested later). Adjacent Connected/Disconnected label text colour is unchanged.

</frozen-after-approval>

## Implementation Notes

- Reused `SkinEditorChrome::danger()` (`#ff3b3b`) for disconnected BRIDGE fill — same token as SAME COLOUR badge; text stays `surfaceApp()` via existing `drawChromeBadge` overload.
- Connected fill remains explicit `textMuted()` (same as previous default).
- `refreshStatus()` calls `repaint(connectionBadgeBounds_)` after `resized()` so the custom-painted badge updates on connect/disconnect.
- Manual smoke: disconnect Standalone Dev → BRIDGE red; reconnect → muted grey; Connected/Disconnected label colour unchanged.
- Touched: `Tools/SkinEditor/SkinEditorMainComponent.cpp`, `Tools/SkinEditor/SkinEditorMainComponentSession.cpp`, `Tools/SkinEditor/SkinEditorChromeTokens.h` (danger comment).

## Review Triage Log

- Spec missing Tasks / still in-progress mid-oneshot — false: oneshot route omits Tasks; status set to `done` at finalize.
- `danger()` comment incomplete — high→patched: comment now lists BRIDGE disconnected.
- Spec missing concrete smoke check — low→patched: Implementation Notes include disconnect/reconnect smoke.
- Shared red for SAME COLOUR / Compare / BRIDGE — false: intentional reuse per Intent; accepted overload.
- Full-window `repaint()` — medium→patched: scoped `repaint(connectionBadgeBounds_)`.
- Colour-only badge + label as text cue — false: matches Intent (label colour unchanged; text still Connected/Disconnected).
- Mismatched refreshStatus comment — low→patched: layout vs paint rationale clarified.
- Spec lifecycle not closed — false once status `done` at finalize.
