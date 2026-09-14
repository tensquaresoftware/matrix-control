---
title: 'Settings / Audio-MIDI / UI Scale keyboard shortcuts'
type: 'feature'
created: '2026-09-14'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
baseline_commit: 'b2fd720b04dd574db31e8d19b0334ba19ef9dbcf'
context:
  - '{project-root}/Source/GUI/Helpers/EditorialUndoRedoShortcuts.h'
  - '{project-root}/Documentation/User/manuel-utilisateur.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Settings, standalone Audio/MIDI device settings, and UI Scale are only reachable via logo clicks/menus, while Matrix-Control already captures keyboard focus well enough for standard DAW-style shortcuts.

**Approach:** Add Cmd/Ctrl+, (open/bring Settings to front), Option/Alt+Cmd/Ctrl+, (Audio/MIDI, standalone only), and Cmd/Ctrl + / − / 0 (step UI Scale within 50–200% presets / reset to 100%). Keep existing logo click behaviors. Do not steal shortcuts while text is being edited. Surface the shortcuts in the logo popup labels, in Settings, and in the user manual.

</frozen-after-approval>

## Implementation Notes

### Agent decisions (user would not notice as product forks)
- Keyboard Settings path calls `PluginEditor::openSettingsWindow()` only (visible → `toFront` / focus). Shift-click toggle-close stays unchanged.
- Plugin mode: ignore Alt/Option+Cmd/Ctrl+, (`return false`). Standalone: call existing `StandaloneAudioInputRouter::showAudioMidiSettingsDialog()`.
- UI Scale steps only through `PluginIDs::Settings::ScaleLevels` ids `k50…k200`; at min/max still consume the key when matched. Reset always `k100`.
- Match `+` via exact KeyPress peers (`+`, `=`, Shift variants, numberPadAdd); `-` / `0` with command only (no Alt).
- Text-edit guard: reuse `PluginEditor::isEditorialUndoBlockedByTextFocus()` → do not consume. Do not apply undo’s modal-overlay consume-without-action.
- Discoverability: logo popup right-aligned shortcut hints; Settings KEYBOARD section; manual §14/§15 (logo tooltip removed — product decision).
- JUCE 9: n/a for tooltip (logo no longer uses SettableTooltipClient).

### Files touched
- `Source/GUI/Helpers/EditorChromeShortcuts.h` (new)
- `Tests/Unit/EditorChromeShortcutTests.cpp` (new) + CMake registration
- `PluginEditor` key routing (no TooltipWindow)
- Logo / HeaderPanel / HeaderLogoPopupMenu / SettingsPanel / PluginDisplayNames / DesignPanels column width
- `Documentation/User/manuel-utilisateur.md`

### Review patches
- Compact right-aligned logo popup shortcuts (`⌘ +/-`, `⌘ 0`, spaced `⌘ ,` / `⌥ ⌘ ,`); columns back to 80
- Logo tooltip removed (product decision)
- Exact modifier matching for scale keys; extra tests for textCharacter comma + zoom peers
- Manual §15 + Settings table KEYBOARD row; Windows zoom-in spelled `Ctrl+Plus`

## Review Triage Log

- Settings KEYBOARD group glyph unclear — **medium** — patched: spell `⌘+ / ⌘- / ⌘0` (and Win `Ctrl+Plus / …`).
- Fixed Settings height empty space in plugin mode — **defer** — recorded in deferred-work.md (content-driven height debt).
- Logo popup clipping long shortcut titles — **medium** — patched: column width 80→120.
- Unit tests missing comma textCharacter branch — **low** — patched: added coverage + Shift+=.
- Manual §15 / Settings table incomplete — **medium** — patched.
- Scale keys accept extra modifiers loosely — **medium** — patched: exact KeyPress matching (command / command+shift for + only).
- Logo tooltip duplicate Settings / missing text-field note — **low** — patched.
- `Ctrl++` ambiguous on non-Mac — **low** — patched: `Ctrl+Plus`.

### Review Findings

- [x] [Review][Decision] Unison deferred notes mixed into this chantier’s deferred-work edit — resolved: keep only Settings height deferral (strip Unison notes from this diff).
- [x] [Review][Decision] Windows/Linux UI Scale shortcut strings still too long for menu/Settings columns — resolved: compact right-aligned hints (`⌘ +/-` on UI SCALE, `⌘ 0` on 100%, spaced `⌘ ,` / `⌥ ⌘ ,`); logo popup columns back to 80; logo tooltip removed.
- [x] [Review][Patch] Align manual Windows zoom-in spelling with UI labels (`Ctrl+Plus`) [`Documentation/User/manuel-utilisateur.md:491`]
- [x] [Review][Patch] Move logo tooltip text-field ignore note after Audio/MIDI lines — superseded: logo tooltip removed entirely.
- [x] [Review][Patch] Extend unit tests: non-zero `textCharacter` for UI Scale peers (incl. Shift+`+`) [`Tests/Unit/EditorChromeShortcutTests.cpp`]
- [x] [Review][Defer] No automated check that PluginEditor chrome actions fire [`Source/GUI/PluginEditor.cpp:229`] — deferred: CONVENTIONS forbid GUI component unit tests; classifier-only coverage matches editorial undo pattern; needs harness approach later

#### Rejected

- Modal overlays do not block chrome shortcuts — false: frozen Implementation Notes explicitly skip undo’s modal-overlay consume-without-action; working as approved.
- Command+Shift on −/0 ignored after Shift-based zoom-in — false: exact command-only matching for −/0 was an intentional review patch; matches Implementation Notes.
- Plugin Alt+, returns false / may leak to host — false: standalone-only gate is specified (`return false` in plugin mode).
- Settings KEYBOARD omits text-field exception — low: exception remains in the manual; Settings is a compact key reminder; not worth extra Settings copy now.
- Settings fixed height empty space in plugin mode — false: already deferred in this oneshot’s deferred-work entry.
- Decrease path without jlimit when scale id > kMax — false: no demonstrated path to an out-of-range id in normal use; getUiScale already falls back for paint.
- Manual/product note that min/max scale still consumes the key — low: everyday users rarely need that nuance; not worth extra doc complexity.
- Missing dedicated assertion for command+shift+'+' alone — low: folded into the broader UI Scale `textCharacter` / Shift peer test patch above.
