---
title: 'Contextual HELP includes control name'
type: 'feature'
created: '2026-11-25'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Footer HELP on hover shows the module name and a description, but not the control face name. After looking down at HELP, the user must look back at the panel to remember which knob they were on.

**Approach:** Rewrite furtive ContextualHelp copy so each control message reads `MODULE: CONTROL_LABEL verb…` (example: `VCF/VCA: FREQUENCY sets the filter cutoff frequency.`). Module/section title bands stay as today (no duplicated module name). Prefer on-screen face labels; choose natural English verbs per message.

</frozen-after-approval>

## Implementation Notes

- Source of truth for strings: `Source/Shared/Definitions/PluginDisplayNames.h` ContextualHelp namespaces.
- One-shot rewrite helper: `Scripts/dev/_rewrite_contextual_help_labels.py` (maps help keys to ParameterWidgets / StandaloneWidgets face labels; idempotent).
- Module / section title bands unchanged (no duplicated module name).
- Hoverable controls including Header logo-menu items get face labels; trailing menu `...` stripped in mid-sentence (`SETTINGS`, `ABOUT`).
- Footer centre-band / DEVICE HELP unchanged (zone labels, not face controls).
- `Tests/Unit/ContextualHelpOverlayTests.cpp` expectations updated for rewritten header + Internal/Computer group HELP.
- ASCII-only display strings (no em dash / typographic punctuation).
- Review: fixed DETUNE grammar, group-label verbs, SETTINGS/ABOUT ellipsis, test drift, script idempotency. Deferred: exhaustive label-presence unit test; inventory markdown refresh.

## Review Triage Log

- Blind Hunter: Wave 2/Internal/Computer/Mutator test expectations stale — high — patched expectations to new copy.
- Blind Hunter: DETUNE "fine-tunes of" grammar — high — patched rewrite rule + reapplied.
- Blind Hunter: BROWSER/MEMORY/STORAGE missing verbs — medium — patched noun/open polish + MEMORY/STORAGE wording.
- Blind Hunter: script not idempotent — medium — patched already-has-label guard (dry-run rewritten: 0).
- Blind Hunter: SETTINGS.../ABOUT... awkward mid-sentence — medium — strip trailing `...` in face label.
- Blind Hunter: header logo-menu rewritten "against title-band note" — false — logo-menu items are hoverable controls; title bands remain unchanged; Implementation Notes corrected.
- Blind Hunter: single-letter I/C/P/# labels noisy — low rejected — on-screen face labels are intentional for gaze-break HELP.
- Blind Hunter: no exhaustive label-presence test — defer — inventory + golden sample tests cover ship risk.
- Blind Hunter: inventory markdown still old copy — defer — production SSOT is PluginDisplayNames.h.
- Blind Hunter: script is throwaway / dead helpers — low rejected — kept as idempotent regen helper under Scripts/dev.
