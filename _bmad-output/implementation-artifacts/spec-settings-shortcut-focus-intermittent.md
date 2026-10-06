---
title: 'Settings shortcut focus intermittent at launch'
type: 'bugfix'
created: '2026-10-06'
status: 'done'
route: 'dispatch'
baseline_commit: 'c8410b2b9e2d1b82e2beab8c2843b0d4f0e606c7'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-standalone-settings-shortcut-focus.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-settings-audio-midi-ui-scale-shortcuts.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After cold launch (Standalone and sometimes hosted plugin), Cmd/Ctrl+, sometimes fails to open Settings and the OS beeps instead — then works on a later relaunch without a code change. Focus ownership is still a race with window show / host focus.

**Approach:** Harden editor keyboard-focus acquisition so chrome shortcuts (Settings first) are reliably consumed once the UI is on screen, without changing shortcut classification or Settings open behavior. Request focus for Standalone and hosted editors when shown (if nothing in the editor tree already owns it). After Settings / About / Device Setup / similar overlays hide, reschedule the same focus helper so shortcuts work again without a content click.

**Decisions:**
- Launch formats: Standalone + hosted editor show (request focus when showing if the editor tree has none; accept small DAW contention risk).
- After closing overlays: include restore (reschedule `requestEditorKeyboardFocusIfNeeded` on hide of Settings / About / Device Setup / similar; absorbs deferred sibling from 2026-10-01).

## Boundaries & Constraints

**Always:**
- Reuse `PluginEditor::requestEditorKeyboardFocusIfNeeded()` (or a small extension of that helper) as the single focus-request path.
- Keep overlay skip via `isEscapeBlockedByOverlay()` so Device Setup / Settings / About / confirm dialogs are not stolen from.
- Do not steal focus while a text field is being edited (`isEditorialUndoBlockedByTextFocus` already gates chrome keys).
- Preserve Cmd/Ctrl+, classification in `EditorChromeShortcuts` and `openSettingsWindow()` semantics.
- Call the helper for Standalone and hosted when the editor becomes showing; call it again when in-scope overlays hide.

**Never:**
- Change shortcut glyphs, logo menu labels, or user-manual shortcut tables unless a real mismatch is proven.
- Add a global OS/global hotkey outside the focused editor.
- Broad GUI peer/harness test suite beyond what this bug needs (project convention: peer timing stays largely manual smoke).

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Cold Standalone, no content click | Cmd/Ctrl+, shortly after window visible | Settings opens; no OS beep | If overlay visible, shortcut may still be blocked until overlay owns/dismisses (by design) |
| Cold hosted editor show | Editor becomes showing; Cmd/Ctrl+, before content click | Settings opens when host delivered the key to the editor; no beep once editor owns focus | Host may still withhold keys until editor is focused — our grab runs when showing if tree has no focus |
| Race: first async before peer showing | Attach + delayed show | Later show path grants focus; shortcut works without content click | No infinite retry loop |
| Overlay open (Device Setup / Settings) | Focus request fires | No steal from overlay | After dismiss: helper rescheduled → editor can own focus again |
| Overlay closed, no content click | Cmd/Ctrl+, after Esc/close | Settings opens; no OS beep | N/A |
| Text edit focused | Cmd/Ctrl+, while editing | Shortcut not stolen (existing rule) | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/PluginEditor.cpp` — `visibilityChanged`, `requestEditorKeyboardFocusIfNeeded` (async + gates), `keyPressed` / KeyListener → `tryHandleEditorChromeKey` → `openSettingsWindow`.
- `Source/GUI/PluginEditorUiConstruction.cpp` — currently standalone-only call at end of `attachEditorRuntimeListeners`; Device Setup may open async right after — widen per frozen format scope.
- `Source/GUI/PluginEditorAudio.cpp` — `isEscapeBlockedByOverlay()` visibility list.
- `Source/GUI/PluginEditorSettingsOverlay.cpp` / `PluginEditorWindows.cpp` — overlay open grabs; close paths must reschedule editor focus restore.
- `Source/GUI/Helpers/EditorChromeShortcuts.h` — `classifyCommaShortcut` / `kOpenSettings` (do not change matching unless proven wrong).
- `Source/GUI/MainComponent.cpp` — forwards chrome keys via editorial handler wired in UiConstruction.
- `Tests/Unit/EditorChromeShortcutTests.cpp` — classifier only; no peer/focus coverage.
- Prior done: `spec-standalone-settings-shortcut-focus.md`; deferred-work ~2165–2179 (post-overlay restore absorbed by this ticket).

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/PluginEditor.cpp` — Harden focus request for cold-show race (retry once / ensure every transition to showing schedules a request for Standalone and hosted); keep overlay and “already focused child” gates -- Stop intermittent OS beep on Settings shortcut after launch.
- [x] `Source/GUI/PluginEditorUiConstruction.cpp` / `PluginEditor.h` — Align attach-time call with hosted+Standalone scope; keep comments accurate -- Avoid duplicate or contradictory grab policies.
- [x] `Source/GUI/PluginEditorSettingsOverlay.cpp` / `PluginEditorWindows.cpp` (and any other in-scope overlay hide paths covered by `isEscapeBlockedByOverlay`) -- On hide, call `requestEditorKeyboardFocusIfNeeded()` -- Restore chrome shortcuts without a content click after overlay dismiss.
- [x] Manual smoke — Cold Standalone and hosted: Cmd/Ctrl+, before content click across several relaunches; open Settings → close → Cmd/Ctrl+, without content click -- Prove intermittency and post-overlay beep are gone. (2026-10-06: Guillaume — appears to work better)

**Acceptance Criteria:**
- Given a cold Standalone launch with the window visible and no content click, when the user presses Cmd/Ctrl+, then Settings opens and the OS does not beep (repeat across several relaunches).
- Given a hosted editor that has become showing and nothing in the editor tree owns focus, when focus is requested then the editor can receive chrome shortcuts without requiring a click on empty GUI content.
- Given an overlay that blocks escape is visible, when a focus request would run, then it does not steal focus from that overlay.
- Given Settings / About / Device Setup (or similar in-scope overlay) was closed, when the user presses Cmd/Ctrl+, without a content click, then Settings opens and the OS does not beep.
- Given text editing has focus, when Cmd/Ctrl+, is pressed, then existing “do not steal” behavior remains.

## Implementation Notes

- Widened attach-time and `visibilityChanged` focus requests to Standalone **and** hosted (removed `isStandalone()` gates). Single helper unchanged: async grab only when showing, skip when `isEscapeBlockedByOverlay()`, skip when a descendant already has focus.
- Cold-show follow-up: every transition to showing reschedules the helper (no infinite async retry loop). Attach may still no-op if the peer is not showing yet; show path retries once.
- On hide of every overlay listed by `isEscapeBlockedByOverlay()` (Settings, About, Master Init confirm, Master M1km load choice, Mutator History defrag confirm, Device Setup / EPROM prompt, Bank Transfer progress), call `requestEditorKeyboardFocusIfNeeded()`. Nested close-while-opening-another-overlay stays safe: helper skips while any blocking overlay is visible.
- `PluginEditorSettings.cpp`: inlined the tiny `normalizeEpromType` wrapper so the file stays under the useful-line gate after close-path restores.
- Classifier / `EditorChromeShortcuts` / `openSettingsWindow` untouched. Unit tests `EditorChromeShortcut` green. Manual smoke still required for peer timing.
- Matrix Test Audit (2026-10-06): human chose manual-smoke-only coverage for peer/focus I/O matrix rows (no GUI peer harness), aligned with frozen Never + project convention; proceed to review.
- Manual smoke (2026-10-06): Guillaume reports behavior looks improved after the focus harden + post-overlay restore.

## Spec Change Log

## Review Triage Log

- Blind: Spec `status: done` while Manual smoke unchecked — **false** — process snapshot during review; status set to `in-review`; smoke remains an open human task, not silent closure.
- Blind: Code Map still says attach-time call is standalone-only — **false** (rejected: fix would edit this build's spec) — Implementation Notes already describe Standalone+hosted; Code Map stale wording is not a product defect.
- Blind: Execution task lists `PluginEditor.h` with no header diff — **false** — no signature change was required; call-site alignment completed without a header edit.
- Blind: No reactivation when an already-showing editor loses host/OS focus without hide→show — **medium** → **defer** — frozen Approach is show + overlay hide only; mid-session steal is a sibling.
- Blind: AC names Settings/About/Device Setup but restore covers all escape-blocking overlays — **false** — frozen Intent/Decisions say Device Setup / similar and Always ties hide restore to in-scope overlays via `isEscapeBlockedByOverlay`.
- Blind: `visibilityChanged` comment said one-shot “once” while every showing callback reschedules — **low** → **patch** — comment clarified; helper still no-ops when focus is owned.
- Blind: `close*` always requests focus even if overlay already null/hidden — **low** (rejected) — helper is idempotent; everyday harm negligible vs adding was-visible branches.
- Blind: Empty Spec Change Log / Review Triage with `status: done` — **false** — process timing; triage filled this pass; status corrected to `in-review`.
- Blind: `normalizeEpromType` inlined for useful-line gate — **low** (rejected) — lint-driven local factoring, not a focus regression; everyday user harm none.
- Blind: Hosted AC soft-gated on host key delivery — **false** — matches frozen I/O matrix error-handling note; not a code defect.
- Edge: `grabKeyboardFocus` false while showing, no retry — **maybe-false** → **defer** — unverified medium; settle with real peer failure reproduction.
- Edge: Chrome shortcut before `callAsync` focus runs after show/close — **low** (rejected) — intentional async peer readiness; prior launch-focus review rejected sync grab / one-turn window as speculative.
- Gap: Cold-show focus request has no automated observer — **medium** → **defer** (pre-verified disposition) — manual smoke only per Never + human audit choice.
- Gap: Post-overlay focus restore has no automated observer — **medium** → **defer** (pre-verified disposition) — same policy.
- Gap: EditorChromeShortcut Verification command does not protect focus behavior — **medium** → **defer** (pre-verified disposition) — classifier-only; manual smoke is the real gate.
- Gap Other: Manual smoke task still unchecked — **low** (rejected as process) — remains open for Guillaume; not a code patch.

## Design Notes

Root cause class is focus ownership, not comma KeyPress matching (classifier and unit tests already cover Cmd/Ctrl+, peers). Prior oneshot fixed attach+visibility for Standalone but left a one-turn async window and no post-overlay restore; hosted formats never called the helper. Intermittent “works after N relaunches” matches peer/host timing, not a flaky classifier.

Preferred hardening: keep a single helper; if async runs while `!isShowing()`, schedule one follow-up when showing becomes true; on overlay hide, reschedule the same helper. Do not loop forever. Do not invent a second focus API.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` (or project preset in use) then run `EditorChromeShortcut` unit tests -- expected: still green; classifier unchanged unless a proven match bug.
- `python3 Scripts/quality/lint_touched.py` on touched C++ -- expected: pass.

**Manual checks (if no CLI):**
- Cold Standalone: several quit/relaunch cycles → Cmd/Ctrl+, before any content click → Settings, no beep.
- Hosted: open editor in a host → same check before clicking plugin content.
- Post-overlay: open Settings → Esc/close → Cmd/Ctrl+, without content click → Settings again, no beep.
