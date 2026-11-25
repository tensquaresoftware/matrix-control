---
title: 'CAP-3 Fix Undo/Redo keyboard shortcuts'
type: 'bugfix'
created: '2026-08-24'
status: 'done'
baseline_commit: 'fbf22ed188f4d3cdc593dd69ddaa34e4d292b61d'
review_loop_iteration: 0
context:
  - _bmad-output/specs/spec-undo-redo-patch-editing/keyboard-shortcuts.md
  - _bmad-output/specs/spec-undo-redo-patch-editing/SPEC.md
  - _bmad-output/implementation-artifacts/spec-12-4-matrix-mod-reorder-undo-and-keyboard-shortcuts.md
  - _bmad-output/implementation-artifacts/epic-12-context.md
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Editorial Undo/Redo shortcuts (⌘Z / ⌘⇧Z and Ctrl variants) do not work in real use: Standalone on macOS beeps without undoing, and Ableton Live keeps host Undo even when Matrix-Control’s GUI has focus. Epic 12 marked CAP-3 done, but routing never matches the key on macOS and the plugin format does not request keyboard focus from hosts.

**Approach:** Fix shortcut recognition to use `KeyPress` equality with key code + `commandModifier` (same pattern as `TestComponent` zoom), enable `EDITOR_WANTS_KEYBOARD_FOCUS` so hosts deliver keys to the editor, keep existing text-field / modal / Compare guards, and add a small pure-match unit test. Do not touch UndoManager Core logic.

## Boundaries & Constraints

**Always:**
- Match undo as `KeyPress('z', commandModifier, 0)` and redo as `KeyPress('z', commandModifier | shiftModifier, 0)` (or equivalent equality that allows `textCharacter == 0` when Command is down on macOS).
- Keep `isEditorialUndoBlockedByTextFocus`, modal overlay consume-without-perform, and Core Compare-disabled `performEditorialUndo/Redo`.
- Set `EDITOR_WANTS_KEYBOARD_FOCUS TRUE` in `juce_add_plugin` so Standalone and hosted formats share shortcut delivery.
- Document briefly in Design Notes / user-facing note if host focus policy changes (plugin may receive more keys when focused).
- English-only source; `lint_touched.py` on touched C++.

**Ask First:**
- If enabling host keyboard focus causes a clear regression (e.g. Live shortcuts permanently stolen when plugin should not own them beyond undo/redo), halt before shipping a broader key-forwarding redesign — stay on the CMake flag + editor match fix only.

**Never:**
- Rework `UndoManager`, transaction grouping, or dirty-tracker Core paths.
- Undo/Redo menu or toolbar buttons.
- Host DAW undo-stack delegation (plugin keeps its own editorial stack).
- Stealing Cmd/Ctrl+Z from active text editors or modal dialogs.
- French in source.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| macOS Standalone undo | Editorial stack has one slider transaction; focus on editor/slider (not text); ⌘Z | Match succeeds; `performEditorialUndo`; key consumed (no system beep) | Silent no-op if `!canUndo` but still consume when matched and not text-blocked |
| macOS Standalone redo | After undo; ⌘⇧Z | `performEditorialRedo`; key consumed | Same |
| Hosted (Live) | Plugin editor focused; ⌘Z with stack | Host delivers key; editorial undo runs | Requires rebuild after CMake flag flip |
| Text focus | Patch name editing or Settings `TextEditor` focused; ⌘Z | No patch undo; return false so text/native handling can proceed | N/A |
| Modal overlay | About / Master Init / Bank transfer visible; ⌘Z | Consume key; no perform | N/A |
| Compare active | `kCompareActive`; ⌘Z | Match + prepare; Core no-ops undo; still consume | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/PluginEditor.cpp:215-234` — **bug:** `getTextCharacter() == 'z'` never true under Command on macOS (JUCE peer zeros textCharacter); replace with `KeyPress` equality.
- `Source/GUI/PluginEditor.cpp:133-159` — `keyPressed` Escape then `tryHandleEditorialUndoRedoKey`; keep order.
- `Source/GUI/PluginEditor.cpp:162-198` — modal + text-focus guards; keep.
- `Source/GUI/Tests/TestComponent.cpp:271-285` — **reuse:** `key == KeyPress('+', commandModifier, 0)` pattern that tolerates `textCharacter == 0`.
- `CMakeLists.txt:713` — `EDITOR_WANTS_KEYBOARD_FOCUS FALSE` → `TRUE` (maps to `JucePlugin_EditorRequiresKeyboardFocus`); rebuild AU/VST3/Standalone required for Live.
- `Source/GUI/PluginEditorUiConstruction.cpp:31,227` — runtime `setWantsKeyboardFocus`; necessary but insufficient alone for hosts.
- `Source/Core/PluginProcessorValueTree.cpp` — `performEditorialUndo/Redo` — **read-only** for this fix.
- `_bmad-output/specs/spec-undo-redo-patch-editing/keyboard-shortcuts.md` — CAP-3 focus rules SSOT.
- `_bmad-output/implementation-artifacts/deferred-work.md:15,37-38` — prior deferrals: no GUI keyPressed test; host focus flag still 0.
- Prefer extractable pure helper (e.g. free function or anonymous-namespace match) so unit tests assert KeyPress equality without instantiating `PluginEditor`.

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/PluginEditor.cpp` (+ `.h` if helper declared) — replace textCharacter match with `KeyPress` + modifiers; keep guards/prepare/perform — fix Standalone macOS recognition.
- [x] Optional small helper in GUI (or Shared) — `isEditorialUndoRedoShortcut` / undo-vs-redo classification — enables unit test without editor harness.
- [x] `CMakeLists.txt` — `EDITOR_WANTS_KEYBOARD_FOCUS TRUE` — host key delivery (Live).
- [x] `Tests/Unit/…` + `Tests/CMakeLists.txt` — unit-test KeyPress match for ⌘Z / ⌘⇧Z with `textCharacter == 0` and Ctrl variants — locks CAP-3 recognition.
- [x] Brief note (Design Notes already; optional one line in `manuel-utilisateur.md` §7 only if focus policy wording is wrong) — document that plugin requests keyboard focus when editor is focused.
- [x] `python3 Scripts/quality/lint_touched.py` — zero findings on touched C++.

**Acceptance Criteria:**
- Given a `KeyPress` with keyCode `'z'`, `commandModifier`, and `textCharacter == 0`, when the match helper / `tryHandleEditorialUndoRedoKey` runs with an empty-guard editor path, then undo is selected (not ignored).
- Given the same with `shiftModifier` also set, then redo is selected.
- Given `EDITOR_WANTS_KEYBOARD_FOCUS TRUE` in CMake after reconfigure, when the plugin is rebuilt, then `JucePlugin_EditorRequiresKeyboardFocus` is 1 for the plugin target.
- Given text-editor focus or PatchName editing, when ⌘Z is pressed, then editorial undo is not performed.
- Given manual Standalone macOS: edit a slider, press ⌘Z → value restores, no system beep; ⌘⇧Z redoes.
- Given manual Ableton Live (AU or VST3): plugin GUI focused, same undo/redo works without Live’s own undo stealing the shortcut.

## Spec Change Log

## Design Notes

**Why `getTextCharacter` fails on macOS:** JUCE’s macOS NSView peer clears `textCharacter` whenever Command is down, so matching on the character never sees `'z'`. `KeyPress` operator== compares keyCode + modifiers and allows a zero text character — already proven by TestComponent zoom shortcuts.

**Why CMake flag matters for Live:** Runtime `setWantsKeyboardFocus(true)` on the editor does not set `JucePlugin_EditorRequiresKeyboardFocus`. With the format flag false, many hosts never forward Cmd+Z to the plugin. Enabling the flag means when the editor has focus, more keys may reach Matrix-Control (Escape for clipboard already relies on focus); text fields and modals still own their shortcuts via existing guards.

**Golden match examples:**
```cpp
using juce::ModifierKeys;
const auto undo = juce::KeyPress('z', ModifierKeys::commandModifier, 0);
const auto redo = juce::KeyPress('z', ModifierKeys::commandModifier | ModifierKeys::shiftModifier, 0);
// key == undo / key == redo
```

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` — expected: build success after CMake reconfigure if needed.
- `ctest --preset macos-debug-arm64 -R Undo` (or new test binary name) — expected: match unit test passes; existing UndoManager tests still pass.
- `python3 Scripts/quality/lint_touched.py` — expected: zero findings.

**Manual checks (if no CLI):**
- Standalone: slider edit → ⌘Z restores → ⌘⇧Z redoes; no beep.
- Ableton Live: focus plugin UI → same; Live must not consume ⌘Z for its own undo while plugin editor owns focus.
- Patch name edit session: ⌘Z must not revert patch parameters.

## Suggested Review Order

**Shortcut recognition**

- KeyPress equality (tolerates macOS textCharacter == 0 under Command)
  [`EditorialUndoRedoShortcuts.h:19`](../../Source/GUI/Helpers/EditorialUndoRedoShortcuts.h#L19)

- Editor routes match result to undo/redo without getTextCharacter
  [`PluginEditor.cpp:216`](../../Source/GUI/PluginEditor.cpp#L216)

**Host keyboard delivery**

- Plugin formats request editor keyboard focus from the host
  [`CMakeLists.txt:714`](../../CMakeLists.txt#L714)

**Docs**

- User-facing note that the plugin asks the host for the keyboard
  [`manuel-utilisateur.md:253`](../../Documentation/Users/manuel-utilisateur.md#L253)

- Spec companion: KeyPress match + EDITOR_WANTS_KEYBOARD_FOCUS
  [`keyboard-shortcuts.md:34`](../specs/spec-undo-redo-patch-editing/keyboard-shortcuts.md#L34)

**Tests**

- Unit coverage for zero-textCharacter undo/redo and rejects
  [`EditorialUndoRedoShortcutTests.cpp:19`](../../Tests/Unit/EditorialUndoRedoShortcutTests.cpp#L19)
