---
title: 'Remove Settings KEYBOARD section'
type: 'chore'
created: '2026-09-16'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
baseline_commit: 'da4e11bfae0e94a591d557b7651361ecc26f6f41'
context:
  - '{project-root}/Source/GUI/Settings/SettingsPanel.h'
  - '{project-root}/Documentation/User/manuel-utilisateur.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The Settings modal includes a KEYBOARD section that only lists keyboard shortcuts; it is not a setting users configure, and it does not belong in Settings.

**Approach:** Remove the KEYBOARD section from the Settings UI entirely, shrink the dialog to match remaining content, clean unused Settings-only strings/helpers, and update the user manual so it no longer claims Settings shows those shortcuts. Keep the shortcuts themselves and the logo-menu hints.

</frozen-after-approval>

## Implementation Notes

### Agent decisions (user would not notice as product forks)
- Recalculate `SettingsPanel::kDesignHeight` by subtracting the pre-KEYBOARD section gap plus KEYBOARD header + up to 4 shortcut rows (design height previously sized for standalone Audio/MIDI row): `633 - 12 - (20+4+1+12) - 4*(20+12) = 456`.
- Delete Settings-only display/help strings and `EditorChromeShortcutLabels::uiScaleGroup` / `skinGroup` once unused; leave logo-facing label helpers and shortcut dispatch untouched.
- Manual: drop the Settings KEYBOARD discoverability sentence and the Settings table KEYBOARD row; keep the shortcut tables and logo-menu mention.

### Files touched
- `Source/GUI/Settings/SettingsPanel.h` / `.cpp` / `SettingsPanelSetup.cpp` — remove KEYBOARD widgets, layout, looks, help binds; `kDesignHeight` 633 → 456
- `Source/Shared/Definitions/PluginDisplayNames.h` — remove Settings KEYBOARD labels + contextual help
- `Source/GUI/Helpers/EditorChromeShortcuts.h` — remove `uiScaleGroup` / `skinGroup`
- `Documentation/User/manuel-utilisateur.md` — remove Settings KEYBOARD references
- `_bmad-output/implementation-artifacts/deferred-work.md` — mark KEYBOARD fixed-height deferral resolved

### Verification
- `python3 Scripts/quality/lint_touched.py` — OK
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Standalone` — OK

### Review patches
- Restored `kDesignHeight` comment noting plugin HARDWARE LATENCY sizing
- Closed stale deferred-work entry about KEYBOARD-row empty space

## Review Triage Log

- Spec still `in-progress` mid-oneshot — **false** — finalize sets `done` after review.
- Standalone empty bottom vs plugin-sized height — **low** (rejected) — same fixed-height pattern as before; HW-latency gap already deferred 2026-09-03.
- Stale deferred-work KEYBOARD empty-space entry — **medium** — patched: marked resolved; residual gap points at 2026-09-03 deferral.
- Done sister spec still mentions Settings KEYBOARD — **false** — historical record of what shipped then; not rewritten.
- Missing height comment after KEYBOARD removal — **low** — patched: comment restored for plugin HARDWARE LATENCY sizing.
- Logo menu not mirroring old Settings combined shortcut lines — **false** — intentional; discoverability stays logo + manual tables.
- Spec verification not auditable from artifact — **low** (rejected) — commands recorded in Implementation Notes; oneshot does not require a checklist template.
- No `static_assert` tying height to section metrics — **low** (rejected) — would add mechanism beyond this chore; existing deferral covers content-driven height.
- Oneshot missing AC / out-of-scope blocks — **false** — oneshot template deletes those sections by design.
