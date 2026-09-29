---
title: 'DEVICE SETUP welcome intro copy'
type: 'feature'
created: '2026-09-29'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '06623c269c0e765f63fe6b9ca86f275ac3f4e78c'
context:
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The one-shot DEVICE SETUP modal opens with a purely instructional body. First launch should greet Matrix owners more warmly while still explaining why a quick setup matters and what to configure.

**Approach:** Replace the DEVICE SETUP body copy with an approved English intro (welcome + GitHub product line + need for quick setup) followed by the existing ports/EPROM instructional sentences. Keep the optional firmware suggestion suffix. Increase dialog height as needed so the longer body fits without clipping controls.

**Decisions:**
- Add welcome intro; rewrite full body (not a separate label widget).
- Approved body (ASCII):
  `Welcome to Matrix-Control - a modern SysEx MIDI editor for the Oberheim Matrix-1000/6/6R synthesizers. A quick setup is needed so you can use Matrix-Control optimally with your synth. Select MIDI ports and the EPROM type installed in your synth. This affects MIDI timing and future features.`
- Keep `kBodySuggestionSuffix` unchanged when firmware hint is on.
- Dialog height may increase; no open/persist / row-behavior changes.

## Boundaries & Constraints

**Always:**
- Keep English UI copy in `PluginDisplayNames::Dialogs::EpromTypePrompt`.
- ASCII-only display punctuation (hyphen / `...`, no em dash) per ascii-display-strings.
- Preserve CONFIRM / SPECIFY LATER, four control rows, title `DEVICE SETUP`, and the optional firmware suggestion suffix behavior.
- Keep one painted body block via `bodyText()` (welcome + instructions in `kBody`).

**Never:**
- Reopen DEVICE SETUP after machine `promptDone`, add a Settings "run again" control, or change open/persist logic.
- Change MIDI / DEVICE / EPROM row behavior or searching animation.
- Translate product labels; French belongs in chat only.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| First open, no firmware hint | Assistant shows, `includeFirmwareSuggestionHint` false | Body shows the approved full `kBody` text | N/A |
| Firmware suggestion active | `includeFirmwareSuggestionHint` true | Approved body plus existing suggestion suffix | N/A |
| Longer copy at UI scale | New body (+ suffix) at 1.0 and common scales | Dialog height / max fitted lines adjusted so rows and buttons stay usable | Prefer `kDesignHeight` bump over truncated text |

</frozen-after-approval>

## Code Map

- `Source/Shared/Definitions/PluginDisplayNames.h` (`Dialogs::EpromTypePrompt`) — replace `kBody` with approved copy; leave title, labels, buttons, suffix unless layout-only constants change.
- `Source/GUI/Dialogs/EpromTypePromptDialog.cpp` — `bodyText()` concatenates `kBody` + optional suffix (likely unchanged).
- `Source/GUI/Dialogs/EpromTypePromptDialogLayout.cpp` — raise max fitted lines and/or body allocation as needed for longer copy.
- `Source/GUI/Dialogs/EpromTypePromptDialog.h` — bump `kDesignHeight` (currently 280) as needed.
- Out of scope: `DeviceSetupDeviceRow.h`, machine defaults, `PluginEditorUiConstruction` open guard.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Shared/Definitions/PluginDisplayNames.h` -- Set `kBody` to the approved English ASCII string -- Single source for dialog body.
- [x] `Source/GUI/Dialogs/EpromTypePromptDialog.h` + `EpromTypePromptDialogLayout.cpp` -- Increase design height and/or body fit line budget so full body (+ suffix) paints without clipping rows/buttons -- Height increase is explicitly allowed.
- [x] `Source/Core/Services/DeviceSetupDeviceRow.h` + `Tests/Unit/DeviceSetupDeviceRowTests.cpp` -- Pure `buildDeviceSetupAssistantBodyText` + unit coverage for body with/without suffix -- Matrix rows for copy assembly.
- [ ] Manual smoke -- Open DEVICE SETUP once (fresh `promptDone` false path or test harness) with and without firmware suggestion suffix -- Confirm copy and layout (human).

**Acceptance Criteria:**
- Given DEVICE SETUP is shown, when the body paints, then the approved English body appears above the four rows, ASCII-safe.
- Given the firmware suggestion hint is on, when the body paints, then the existing suggestion suffix still appends after the body.
- Given typical UI scales, when the dialog lays out, then MIDI/DEVICE/EPROM rows and CONFIRM / SPECIFY LATER remain fully usable (no overlap or clipped buttons).

## Implementation Notes

- `kBody` replaced with approved welcome + GitHub line + setup need + ports/EPROM instructions; `kBodySuggestionSuffix` unchanged.
- `kDesignHeight` 280 → 310; `kMaxBodyFittedLines_` = 8 (was hardcoded 5).
- Dialog `bodyText()` now calls `Core::buildDeviceSetupAssistantBodyText`.
- Unit tests: `DeviceSetupDeviceRow` category green (includes new assistant body test). Lint OK vs baseline. Standalone/Tests Debug build OK.
- Layout-at-scale matrix row remains human visual smoke (GUI not unit-tested per project policy).

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence / route |
|---|---|---|
| Blind: Approach says “GitHub product line” but shipped string has no “GitHub” | false | Approach means the GitHub *description* was the source wording; approved frozen `kBody` matches intent. Fix would only edit the spec → rejected. |
| Blind: Code Map still says DeviceSetupDeviceRow out of scope / bodyText unchanged | false | Spec-doc drift only; “reject findings whose fix is to edit this build’s spec.” |
| Blind: About `kTagline` not shared as SSOT with new body | false | Frozen approved body uses “a modern SysEx…” in welcome context; About tagline is a separate sentence without leading “a”. Sharing would renegotiate approved copy. |
| Blind: No recorded layout math / truncation risk at 8 lines | maybe-false → defer | Real verification gap for height/fit; unverified at medium for clipping. Settled by human smoke. Deferred. |
| Blind: Unit test only bans em dash, not en dash / ellipsis | low → reject | Unlikely everyday defect after ASCII-approved copy; rejected (more than a null risk). Partial ASCII checks added with the VG patch anyway. |
| Blind: “optimally” vague marketing | false | Exact phrasing approved in frozen Decisions. |
| Blind: Empty Spec Change / Triage logs mid-run | false | Expected before this triage pass. |
| Blind: +30 height may be insufficient vs content growth | maybe-false → defer | Same as layout gap; needs visual smoke. Deferred with VG layout finding. |
| Edge: (none) | — | Empty finding list. |
| VG: Body unit test under-pins approved copy (substring-only) | medium → patch | Confirmed: three contains allowed truncated copy. Patched to `expectEquals` full approved `kBody` + full suffix + en-dash/ellipsis ban. Tests green. |
| VG: Dialog height / fitted lines no automated check | medium → defer | Confirmed under CONVENTIONS GUI ban; deferred to human smoke. |
