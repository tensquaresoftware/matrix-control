---
title: 'Settings INTERFACE Contextual Help SHOW/HIDE'
type: 'feature'
created: '2026-09-27'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
context: []
baseline_commit: 'ca838ca659e0146d72a33f7fbf5bb10654a0d2c3'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Settings has DEVICE / PATCH / PATCH MUTATOR / MASTER, but no user preference to turn off furtive footer HELP (hover/focus overlay). Sticky ERROR / INFO / WARN messages must stay unaffected.

**Approach:** Add an INTERFACE section first in Settings with CONTEXTUAL HELP (SHOW / HIDE), persist it like other Settings combos, and gate the HELP overlay path so HIDE clears and blocks furtive help immediately without restarting the plugin.

## Boundaries & Constraints

**Always:**
- Section order: INTERFACE → DEVICE → PATCH → PATCH MUTATOR → MASTER.
- Labels ASCII-only: section `INTERFACE`, row `CONTEXTUAL HELP`, items `SHOW` / `HIDE`.
- Persist via APVTS `state` int property (stable new key, e.g. `settingsContextualHelp`); nested ids `kShow=1`, `kHide=2`, `kDefault=kShow`.
- Missing key / invalid id → SHOW (compat with existing sessions).
- SHOW = current furtive HELP behaviour; HIDE = no HELP overlay anywhere binders are registered (including Settings itself).
- On change to HIDE: clear overlay unconditionally (bypass popup defer / delayed idle clear); keep show path gated so hover/focus cannot re-show.
- Sticky footer messages (`uiMessageText` / severity) unchanged.
- Compact non-scrolling Settings: bump `kDesignHeight` for one section + one row (~+67 design px from current 460); no vertical tabs in this chantier.
- Immediate restore/wire on Settings open and combo change (no plugin restart).

**Never:**
- Other INTERFACE prefs (theme, UI scale, shortcuts, etc.).
- Cut sticky ERROR / WARN / INFO or change HELP-vs-sticky paint priority rules beyond preference gating.
- Vertical Settings tabs unless overflow proves unfixable by height bump alone (then decide compact vs height — not tabs by default).
- French in source or UI strings; Core → GUI dependency.
- Refactor binder multi-panel inventory / Mutator-era helper renames (deferred elsewhere).

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Fresh / missing key | No property | Default SHOW; furtive HELP works as today | N/A |
| User picks HIDE | Combo change | Persist id; overlay clears; further hover/focus show nothing | Clamp invalid → SHOW |
| User picks SHOW | Was HIDE | Persist id; HELP resumes on next hover/focus | N/A |
| HIDE while hover/focus | Bound control still active | Overlay gone; no re-show until SHOW | Gate set path after clear |
| Sticky ERROR while SHOW | Overlay would paint | ERROR still wins over HELP (existing rule) | N/A |
| Sticky INFO while HIDE | Sticky message set | Sticky still paints; no HELP badge | N/A |
| Settings open | Restored state | Combo shows stored SHOW/HIDE | N/A |

</frozen-after-approval>

## Code Map

- `Source/Shared/Definitions/PluginIDs.h` — add `kContextualHelp` + `ContextualHelp` nested ints (`kShow`/`kHide`/`kDefault`).
- `Source/Shared/Definitions/PluginDisplayNames.h` — `kInterfaceSection`, `kContextualHelpLabel`, `kShow`/`kHide` (or Settings-scoped names); optional Settings row help string if peers have one.
- `Source/GUI/Settings/SettingsPanel.h` / `.cpp` / `SettingsPanelSetup.cpp` — INTERFACE section first; setup/layout/populate/looks/help bind; bump `kDesignHeight` (~527).
- `Source/GUI/PluginEditorSettings.cpp` — restore + wire combo like Unsaved State / Delete Warning (no EPROM side effects).
- `Source/Core/PluginProcessorClipboard.cpp` (+ decl / `PluginProcessorConstruction.cpp`) — `initializeContextualHelpProperty` default SHOW.
- `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.{h,cpp}` — choke-point: gate `setContextualHelpOverlay`; clear on preference → HIDE (ValueTree listener already present); read preference from existing `apvts`.
- `Source/GUI/Helpers/ContextualHelpOverlay.h` — small pure predicate (e.g. enabled when id == SHOW) next to existing helpers.
- `Tests/Unit/ContextualHelpOverlayTests.cpp` — cover preference enable/disable predicate (+ clear-on-hide policy if extracted).
- Reuse: UnsavedStatePolicy combo pattern; FooterPanel APVTS ref; binder → `setContextualHelpOverlay` funnel.
- Do not change: sticky message writers; popup defer helpers’ semantics for SHOW mode; existing Settings key string values; DEVICE SETUP / EPROM.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Shared/Definitions/PluginIDs.h` / `PluginDisplayNames.h` — IDs, defaults, ASCII display strings for INTERFACE + CONTEXTUAL HELP SHOW/HIDE.
- [x] `Source/Core/PluginProcessor*.{h,cpp}` — initialize APVTS property to SHOW when absent.
- [x] `Source/GUI/Settings/SettingsPanel*` — INTERFACE section first + CONTEXTUAL HELP combo; recompute `kDesignHeight`; keep non-scrolling.
- [x] `Source/GUI/PluginEditorSettings.cpp` — restore/wire combo with immediate persistence.
- [x] `Source/GUI/Helpers/ContextualHelpOverlay.h` + `FooterPanel.*` — gate set path; clear on HIDE; sticky path untouched.
- [x] `Tests/Unit/ContextualHelpOverlayTests.cpp` — unit-test preference predicate / I/O edges that are pure.

**Acceptance Criteria:**
- Given Settings open, when the user inspects section order, then INTERFACE is first, then DEVICE → PATCH → PATCH MUTATOR → MASTER.
- Given preference SHOW (default), when hovering/focusing a bound control, then furtive HELP appears as today.
- Given preference HIDE, when hovering/focusing any bound control (including after HIDE with focus still held), then no HELP overlay appears and any prior HELP overlay is cleared.
- Given a sticky ERROR/INFO/WARN footer message and HIDE, when the UI updates, then the sticky message still displays.
- Given a session without the new key, when the plugin loads, then behaviour matches SHOW and the Settings combo shows SHOW.
- Given Settings height after the new section, when the modal opens, then content fits without a general scroll bar.

## Implementation Notes

- Key `settingsContextualHelp` with `ContextualHelp::{kShow=1,kHide=2,kDefault=kShow}`; processor seeds when absent.
- Settings: INTERFACE first; `kDesignHeight` 460 → 527; restore/wire via extracted `restoreSettingsPolicyCombosFromState` / `wireSettingsContextualHelpCombo`.
- Footer choke-point: `setContextualHelpOverlay` gated by `isContextualHelpPreferenceEnabled`; ValueTree change to HIDE calls `clearContextualHelpOverlay` immediately.
- Pure helpers + unit tests in `ContextualHelpOverlay` (SHOW/HIDE, invalid→SHOW, clear-on-HIDE via `shouldClearContextualHelpOverlayForPreference` used by FooterPanel). Manual UI smoke still recommended.
- Light extract: `initializeSettingsPolicyProperties` groups Settings policy seed calls (includes new contextual help init).
- Review patch: bind clear-on-HIDE test to the same helper FooterPanel calls.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence |
|---------|---------|----------|
| BH: Code Map cites PluginProcessorClipboard for init | false | Init lives in Construction.cpp; sibling policy seeds remain in Clipboard by pre-existing split. Fix would only edit this build's spec Code Map — rejected. |
| BH: kShow/kHide unscoped under Settings | false | Peers (`kAlwaysWarn`, `kDisplayMusicalNames`, etc.) use the same Settings-root pattern; no incorrect reuse demonstrated. |
| BH: HIDE set path returns epoch 0 untested | low | Real return-0 when gated; `shouldClear…ForEpoch(0,…)` already refuses clear. Everyday harm unlikely; rejected as low. |
| BH: HIDE→SHOW with focus still active unspecified | false | Matrix row for SHOW already says HELP resumes on next hover/focus — matches binder behaviour. |
| BH: kDesignHeight +67 vs ~65 layout math | false | Header is 20+2+1+8 (=31) + row 20+8 + inter-section 8 = 67; matches bump 460→527. |
| BH: hideClears test mirrors FooterPanel clear | medium | Confirmed: test hardcodes preference + `overlay.clear()`; deleting FooterPanel listener branch would still pass. Grouped with VG1. |
| BH: Verification ctest placeholder / empty logs | false | Process artifact; fix is edit this build's spec — rejected. |
| BH: Processor does not coerce invalid stored id at startup | false | Same seed-if-missing pattern as UnsavedState/DeleteWarning; footer/helpers normalize for behaviour. |
| EC: (none) | — | Empty finding list. |
| VG1: hideClearsOverlayDetailPolicy broken verification | medium | Pre-verified gap; FooterPanel clear-on-HIDE not bound to production decision helper. |
| VG2: FooterPanel show-gate no GUI unit test | medium | Pre-verified; CONVENTIONS forbid GUI component tests; pure predicates already covered — defer. |

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `ctest --test-dir Builds/macOS/...` (or project unit-test target for ContextualHelpOverlay) -- expected: new/updated overlay policy tests pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++

**Manual checks:**
- Settings: INTERFACE + CONTEXTUAL HELP SHOW/HIDE; flip both ways without restart; sticky messages still visible under HIDE; no HELP leak while a control stays hovered/focused after HIDE.
