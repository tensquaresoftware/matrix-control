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

## Review Findings

### Code review 2026-09-23 (`0080f245`)

- [x] [Review][Patch] Assert INPUT GAIN editor allow-list after openValueEditor — `setText` bypasses `setInputRestrictions`; without assertion the allow-list can regress while parse tests stay green. [Tests/Unit/SliderValueEntryTests.cpp] — applied: insertTextAtCaret probe for allowed sample + reject `@`.
- [x] [Review][Patch] Assert bare `inf` / `INF` keep previous value (not silence) — silence tokens are limited to `-inf` / `-∞` but no commit test locks the rejection. [Tests/Unit/SliderValueEntryTests.cpp] — applied: `testInputGainBareInfKeepsPreviousValue`.
- [x] [Review][Defer] HeaderPanel production wiring of `makeInputGainSliderConfig` untested — deferred: constructing HeaderPanel is outside SliderValueEntry harness style; factory SSOT already covered by unit tests.
- [x] [Review][Defer] Couple `formatValue` with `parseValue` at Slider level — deferred: pre-existing intentional opt-in; already logged in deferred-work.md; only INPUT GAIN uses formatValue today and pairs parseValue.
- [x] [Review][Defer] Convert remaining positional `SliderConfig` aggregates — deferred: already logged in deferred-work.md; not caused by this bugfix beyond the new optional fields.

### Rejected (code review 2026-09-23)

- Blind: missing test for painted `"-∞ dB"` paste — low; editor opens empty; suffix strip already maps that string to silence; not everyday typing.
- Blind: snap cliffs for free dB typing undocumented — reject low (prior triage); behavior lives in `PluginAudioConstants::snapInputGainDb`, silent decision already recorded.
- Blind: fractional dB vs integer display — low; discrete index snap is intentional; expanding this bugfix not worth it.
- Blind: `parseValue` without `allowedEditCharacters` keeps default digit set — low/defer-adjacent; only INPUT GAIN uses parseValue and pairs the allow-list today.
- Blind: Unicode minus (U+2212) rejected — low; unlikely everyday; silence grammar is ASCII `-` + token.
- Blind: no canary test that formatValue-only still mismatches units — covered by existing format/parse coupling defer.
- Blind: missing typed variants (`0.0`, `0dB`, whitespace, case) — low; core happy paths covered; grammar already accepts several of these.
- Blind: bundled assertions in one test case — low; cosmetic isolation, not a product defect.
- Blind: `review_loop_iteration: 0` vs triage log — reject; fix would edit the spec under review.
- Blind: `InputGainSliderText.h` includes full `Slider.h` — low; acceptable for factory returning `SliderConfig`; no everyday harm.
- Acceptance: Approach text broader than opt-in `parseValue` — reject-as-actionable-here / already deferred coupling; frozen Approach is aspirational future-proofing, not a ship blocker for this bugfix.
