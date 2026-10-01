---
title: 'Standalone Settings shortcut focus at launch'
type: 'bugfix'
created: '2026-10-01'
status: 'done'
route: 'oneshot'
baseline_commit: '5df2923de02e1d341929c7723ad50870b5d0f7cf'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-settings-audio-midi-ui-scale-shortcuts.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-cap-3-undo-redo-keyboard-shortcuts.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** On standalone launch, Cmd/Ctrl+, often beeps and does not open Settings until the user clicks empty GUI; title-bar clicks do not help. Hosted VST3/AU already works once the editor is shown.

**Approach:** Ensure the editor takes keyboard focus as soon as it is actually showing (not only via a construction-time async that can no-op), so chrome shortcuts including Settings work immediately after launch without requiring a content click.

</frozen-after-approval>

## Implementation Notes

- Root cause: `attachEditorRuntimeListeners` already deferred `grabKeyboardFocus` via `callAsync`, but the callback exits when `!isShowing()`. On standalone the DocumentWindow peer often becomes showing *after* that first async, so no component owns focus → OS beep on Cmd/Ctrl+, until a content `mouseDown` grabs focus.
- Fix: extract `PluginEditor::requestEditorKeyboardFocusIfNeeded()` (async grab only when showing, no visible escape-blocking overlay, and no descendant already focused). Call it from end of `attachEditorRuntimeListeners` and from `visibilityChanged` when standalone and `isShowing()`.
- Standalone-only gate: hosted formats already receive focus when the host shows the editor; avoid extra grab on plugin show.
- `isEscapeBlockedByOverlay()` skips grab while Settings/About/Device Setup/etc. are visible (covers first-run Device Setup before it owns focus). `hasKeyboardFocus(true)` still avoids stealing from a focused child.
- No change to shortcut classification (`EditorChromeShortcuts`) or `openSettingsWindow` path. Same focus ownership unblocks other chrome shortcuts (UI Scale, Audio/MIDI) and Undo/Redo at launch.
- Manual smoke: cold Standalone launch → Cmd/Ctrl+, before any content click (Settings opens, no beep). Title-bar-only click remains insufficient if focus was never granted (expected). First-run Device Setup still owns focus when open.

## Review Triage Log

- Blind: Spec/comment say `createUiShell` but call is `attachEditorRuntimeListeners` — **low** → **patch** (notes + comment).
- Blind: Hosted formats also grab on show; Intent is standalone — **medium** → **patch** (`isStandalone()` gate on both call sites).
- Blind: Overlay visible but not yet focused can lose to editor grab (Device Setup) — **medium** → **patch** (`isEscapeBlockedByOverlay()` early return).
- Blind: One-turn async window after show still allows a beep — **low** rejected (async needed for peer readiness; sync grab was the prior failure mode).
- Blind: Closing Settings never restores editor focus — **medium** → **defer** (pre-existing sibling; deferred-work entry).
- Blind: Oneshot spec thin (no AC/Verification) — **false** (oneshot route is Intent + Implementation Notes only).
- Blind: Other chrome shortcuts / undo not listed for smoke — **low** → **patch** (Implementation Notes smoke line).

### Review Findings

- [x] [Review][Defer] Post-overlay editor focus not restored (Settings/About/Audio-MIDI/Device Setup) [`PluginEditorWindows.cpp` close paths; `PluginEditor.cpp:182-183`] — deferred: pre-existing sibling of launch-focus bug; overlay early-return correctly skips steal while open but does not reschedule after hide; same deferred-work entry widened.
- [x] [Review][Defer] No automated check for standalone launch focus grab [`PluginEditor.cpp:164-188`] — deferred: GUI peer/focus timing stays manual Standalone smoke per project convention.
- [x] [Review][Defer] No automated check for overlay skip in focus request [`PluginEditor.cpp:181-183`] — deferred: same GUI/manual-smoke convention; needs real editor/peer harness.

#### Rejected

- Blind: smoke / Intent / triage wording incomplete for other shortcuts or first-run after Device Setup — rejected (`low` / spec-edit): oneshot Intent is cold launch without content click; post-overlay restore already deferred; do not edit frozen Intent to absorb sibling scope.
- Blind: no window-activation / title-bar focus path — rejected (`false`): title-bar insufficiency is Intent-expected; activation restore is out of oneshot scope.
- Blind: `requestEditorKeyboardFocusIfNeeded` not standalone-gated internally — rejected (`false`): both call sites gate on `isStandalone()`; no current hosted grab.
- Edge: retry when `grabKeyboardFocus()` returns false — rejected (`low`): speculative after `isShowing()` check; retry loop adds complexity without demonstrated failure mode.
- Blind: comment claims overlay gate “covers” Device Setup — rejected (`false` for code comment): comment correctly explains skip-while-visible; post-dismiss is the deferred sibling above.
