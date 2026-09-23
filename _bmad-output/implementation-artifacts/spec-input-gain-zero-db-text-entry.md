---
title: 'Input gain typed 0 becomes 0 dB'
type: 'bugfix'
created: '2026-09-23'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Double-click editing the header INPUT GAIN slider and typing `0` commits silence (−inf) instead of 0 dB, because the slider stores a discrete index while the displayed text is in dB.

**Approach:** When the slider uses a custom display formatter, typed text must parse in the same units as that display (dB for INPUT GAIN), then map to the internal index before commit.

</frozen-after-approval>

## Implementation Notes

- Root cause: INPUT GAIN slider stores discrete indices (0 = silence); typed Enter used raw numeric parse, so `"0"` became silence index.
- Added optional `SliderConfig::parseValue` + `allowedEditCharacters`; `handleEditorReturn` uses `parseValue` when set.
- Shared helpers in `Source/GUI/Helpers/InputGainSliderText.h` (`format` / `parse` / `makeInputGainSliderConfig`); HeaderPanel uses the factory.
- Silent decisions: typed values are dB; optional `dB` suffix; only `-inf` / `-∞` for silence; optional leading `+`; out-of-range dB snaps via `inputGainDbToIndex`.
- Review patches: per-slider allow-list (not Input-Gain-hardcoded for all parseValue); stricter finite parse; `+` / glyph silence tests; illegal Enter keeps previous value.
- Tests: `SliderValueEntry` covers `"0"`, `"0 dB"`, `"-inf"`, `"-∞"`, `"-12"`, `"+6"`, illegal `"1.2.3"`.

## Review Triage Log

- formatValue without parseValue can revive unit mismatch — defer — only INPUT GAIN uses formatValue today and pairs parseValue; general coupling is future-proofing.
- editorAllowedCharacters Input-Gain-specific for any parseValue — patch — added SliderConfig::allowedEditCharacters.
- Loose finite parse (`1.2.3`, `...`) — patch — digit/dot loop mirrors tryParseEditText.
- Missing `-∞` glyph coverage — patch — test added.
- `+` boost not typeable — patch — allow `+` and strip in parser.
- Illegal Input Gain Enter untested — patch — `"1.2.3"` keeps previous value.
- Out-of-range dB snap undocumented in tests — reject low — snap lives in PluginAudioConstants; not worth expanding this bugfix.
- Positional SliderConfig call sites warning after new fields — defer — TestSliders / SettingsPanelSetup still positional; designated init elsewhere is safe.
- Bare `inf` treated as silence — patch — silence tokens limited to `-inf` / `-∞`.
- No direct helper unit tests — reject low — SliderValueEntry harness covers the shipped grammar.
- Spec still in-progress while code landed — false — finalized to done in this pass.
