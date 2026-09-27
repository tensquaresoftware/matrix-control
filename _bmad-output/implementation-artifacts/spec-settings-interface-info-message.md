---
title: 'Settings INTERFACE Info Message KEEP/AUTO CLEAR and sticky badge dismiss'
type: 'feature'
created: '2026-09-27'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
context: []
baseline_commit: '94052c61df039190f9dc35a19346bf27ee99860c'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Sticky footer INFO messages stay until the next sticky write, with no way to auto-clear them or dismiss any sticky severity without waiting for a replacement message.

**Approach:** Add INTERFACE **INFO MESSAGE** (KEEP / AUTO CLEAR, default KEEP, fixed 5 s) above CONTEXTUAL HELP; auto-clear INFO only (pause while furtive HELP overlays); make the sticky severity badge clickable with a stable square picto that becomes a close cross on hover to clear sticky APVTS state.

## Boundaries & Constraints

**Always:**
- INTERFACE row order: **INFO MESSAGE** then **CONTEXTUAL HELP**; section order unchanged (INTERFACE first).
- ASCII labels: `INFO MESSAGE`, items `KEEP` / `AUTO CLEAR`; default KEEP; duration fixed 5 s (no duration combo).
- Persist via APVTS `state` int property (stable key, e.g. `settingsInfoMessage`); nested ids `kKeep=1`, `kAutoClear=2`, `kDefault=kKeep`; missing/invalid → KEEP.
- AUTO CLEAR applies only to sticky severity INFO; WARNING and ERROR never auto-clear from this preference.
- Manual clear: click the sticky severity badge (INFO / WARNING / ERROR); hit-zone = badge square only — not the message text, not a separate trailing cross.
- Badge chrome: fixed square reserved width (no message reflow on hover); fill = severity colour used for badge chrome; glyph = contrasting badge colour (footer/background tone as today). Rest = geometric severity picto; hover = geometric close cross (Settings/About Path pattern); leave without click restores picto, message unchanged.
- No furtive HELP on the severity badge hover/focus.
- Auto-clear timer pauses while a furtive HELP overlay is painted over the sticky band; resume the **remaining** delay when HELP clears (no surprise clear).
- Clear sticky = empty `uiMessageText` + `uiMessageSeverity` via existing clear path (`ExceptionPropagator::clearMessage` or equivalent), so all listeners sync.
- Compact non-scrolling Settings: bump `kDesignHeight` by one INTERFACE row (~+28 design px from 527); no vertical tabs.
- Immediate restore/wire on Settings open and combo change.

**Never:**
- Separate dismiss control to the right of the message text; click-to-dismiss on the message body.
- Duration combo (2/5/10/30 s); auto-clear WARNING / ERROR.
- Vertical Settings tabs; theme / UI scale / shortcut prefs in this chantier.
- Icon font, emoji, or Unicode glyphs for pictos/cross.
- Change ERROR-vs-HELP paint ranking except as needed for clear/timer correctness.
- Global sticky string casing inventory; French in source/UI strings; Core → GUI dependency.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Fresh / missing key | No property | KEEP; sticky INFO stays until next sticky or badge dismiss | Invalid id → KEEP |
| AUTO CLEAR + sticky INFO | Preference AUTO CLEAR; INFO set | Clears sticky after 5 s if HELP not covering | N/A |
| AUTO CLEAR + WARNING/ERROR | Preference AUTO CLEAR | Sticky stays (no auto-clear) | N/A |
| Timer + HELP overlay | INFO auto-clear running; HELP shows | Timer pauses; resumes remainder when HELP ends | No clear while HELP covers |
| Badge click | Sticky INFO/WARNING/ERROR visible | Clears sticky APVTS; band empty (or HELP if active) | Ignore click if no sticky |
| Badge hover | Pointer over square | Picto → cross; message text layout unchanged | Leave → picto back |
| CONTEXTUAL HELP HIDE | Preference HIDE | Sticky + badge dismiss still work; no HELP overlay | N/A |
| KEEP after AUTO CLEAR | User switches to KEEP mid-timer | Cancel pending auto-clear; sticky remains | N/A |
| New sticky while timer | New INFO/WARN/ERROR written | Restart policy for new message (INFO+AUTO CLEAR → new 5 s) | N/A |

</frozen-after-approval>

## Code Map

- `Source/Shared/Definitions/PluginIDs.h` — add `kInfoMessage` + `InfoMessage::{kKeep,kAutoClear,kDefault}`; optional `kAutoClearDurationMs = 5000` next to nested ids or in footer helper.
- `Source/Shared/Definitions/PluginDisplayNames.h` — `kInfoMessageLabel`, `kKeep`, `kAutoClear` (ASCII); optional Settings row help string peer of Contextual Help.
- `Source/Core/PluginProcessorConstruction.cpp` — `initializeInfoMessageProperty` + call from `initializeSettingsPolicyProperties` (mirror Contextual Help seed).
- `Source/GUI/Settings/SettingsPanel.h` / `SettingsPanelSetup.cpp` / `SettingsPanel.cpp` — INTERFACE members, setup/layout/populate/looks/help bind; **INFO MESSAGE row above** Contextual Help; `kDesignHeight` 527 → ~555.
- `Source/GUI/PluginEditorSettings.cpp` (+ `PluginEditor.h` decls) — restore in `restoreSettingsPolicyCombosFromState`; wire combo like Contextual Help.
- `Source/GUI/Helpers/ContextualHelpOverlay.h` (or sibling pure header e.g. sticky-message policy) — normalize preference; `shouldAutoClearStickySeverity(severity, preference)`; remaining-delay / pause helpers testable without GUI.
- `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.{h,cpp}` — timer for INFO auto-clear; pause when HELP covers sticky (`shouldPaintContextualHelpOverSticky` / overlay active); fixed square severity badge paint (pictos + hover cross); left-band hit area for badge only (mirror `deviceHitArea_`); dismiss via `ExceptionPropagator::clearMessage`; no HELP binder on badge.
- `Source/GUI/Settings/SettingsWindow.cpp` / `AboutWindow.cpp` — reuse `makeCloseCrossShape` Path pattern (copy geometry, do not couple windows).
- `Source/Shared/Design/DesignPanels.h` — `Panels::Footer::kIconSize` (14) for badge square sizing via existing Footer dimensions.
- `Tests/Unit/` — new or extended pure tests for normalize / INFO-only gate / pause-resume remaining ms (no GUI component tests).
- Reuse: Contextual Help Settings combo pattern; `ExceptionPropagator::clearMessage`; FooterPanel APVTS listeners; HELP overlay activity as pause signal.
- Do not change: sticky writers' message catalog; DEVICE/MIDI queue badge paint paths beyond shared helper flags; Contextual Help SHOW/HIDE semantics; ERROR-over-HELP ranking rule.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Shared/Definitions/PluginIDs.h` / `PluginDisplayNames.h` — IDs, defaults, ASCII INFO MESSAGE KEEP/AUTO CLEAR strings.
- [x] `Source/Core/PluginProcessorConstruction.cpp` — seed APVTS property to KEEP when absent.
- [x] `Source/GUI/Settings/SettingsPanel*` — INTERFACE INFO MESSAGE combo above CONTEXTUAL HELP; bump `kDesignHeight`; keep non-scrolling.
- [x] `Source/GUI/PluginEditorSettings.cpp` — restore/wire combo with immediate persistence.
- [x] Pure sticky/info-message policy helpers (+ unit tests) — normalize, INFO-only auto-clear gate, pause/resume remaining delay.
- [x] `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.*` — square badge pictos/cross, click clear, INFO auto-clear timer with HELP pause.
- [ ] Manual matrix — SHOW/HIDE HELP × KEEP/AUTO CLEAR × badge click on INFO/WARNING/ERROR.

**Acceptance Criteria:**
- Given Settings INTERFACE, when the user inspects row order, then INFO MESSAGE is above CONTEXTUAL HELP.
- Given KEEP (default / missing key), when a sticky INFO appears, then it stays until the next sticky write or badge dismiss.
- Given AUTO CLEAR and a sticky INFO with no HELP overlay, when 5 s elapse, then sticky APVTS is cleared.
- Given AUTO CLEAR and sticky WARNING or ERROR, when time elapses, then the sticky remains.
- Given AUTO CLEAR INFO timer running, when HELP overlays the band then clears, then auto-clear waits for the remaining time only (not a full restart, not an immediate clear).
- Given any sticky severity, when the user clicks the severity badge square, then sticky text and severity clear; clicking the message text does nothing.
- Given badge hover, when the pointer enters then leaves without click, then picto becomes cross then returns, and the message text does not reflow.
- Given CONTEXTUAL HELP HIDE, when sticky messages show, then badge dismiss and KEEP/AUTO CLEAR still behave as above.
- Given Settings after the new row, when the modal opens, then content fits without a general scroll bar.

## Implementation Notes

- APVTS key `settingsInfoMessage`; `InfoMessage::{kKeep=1,kAutoClear=2,kDefault=kKeep,kAutoClearDurationMs=5000}`.
- Settings: INFO MESSAGE row above CONTEXTUAL HELP; `kDesignHeight` 527 → 555; restore/wire in `PluginEditorSettings` with other policy combos.
- Pure policy: `StickyInfoMessagePolicy.h` (+ hit-area gate); badge geometry: `FooterSeverityBadge.h`; Footer timer/hit/paint: `FooterPanelStickyMessage.cpp`.
- Unit tests: `StickyInfoMessagePolicyTests` (normalize, INFO-only, remaining delay, HELP pause/resume/fire, badge hit-area, 5 s constant). GUI paint/click/HIDE matrix rows remain manual (CONVENTIONS: no GUI component unit tests).
- Manual smoke still required for SHOW/HIDE × KEEP/AUTO CLEAR × badge click on three severities.
- Review patches: pause uses `remainingAutoClearDelayMs`; `valueTreeRedirected` cancels timer before sync; HELP-at-fire via `autoClearTimerFireWhileHelpCovers`; badge `PointingHandCursor`; INFO MESSAGE help covers INFO-only AUTO CLEAR + badge dismiss.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence |
|---------|---------|----------|
| BH: `remainingAutoClearDelayMs` unused by Footer pause | medium | Confirmed: pause subtracts deadline inline; helper only in tests. Patch: call helper. |
| BH: Settings HELP omits INFO-only + badge dismiss | medium | Help string only mentions stay-vs-5s; users can misread AUTO CLEAR scope. Patch: expand ASCII help. |
| BH: no PointingHandCursor on severity badge | low | Spec affordance is picto→cross; Settings/About close sets hand cursor. Patch: set cursor on hit area. |
| BH: dual setProperty re-arms timer twice | low | Pre-existing ExceptionPropagator split writes; double arm ends at full 5 s. Rejected: everyday harm unlikely; coalesce would add complexity. |
| BH: KEEP cancel composition untested | false | `shouldAutoClearStickySeverity(Info, KEEP)` is false and tested; Footer cancel is that gate. |
| BH: hover reflow test vacuous | low | Test quality only; square bounds ignore hover by design. Rejected: no product defect. |
| BH: manual matrix task unchecked | false | Intentional human smoke; not a code defect. |
| BH: editor recreate resets 5 s | false | New FooterPanel reasonably re-arms; intent does not require persisting countdown across editor lifetime. |
| BH: wire folded into Contextual Help helper name | low | Works; rename-only. Rejected: everyday harm unlikely. |
| BH: info picto filled vs cross stroke language | low | Visual polish at 14 px. Rejected: function OK; redesign not a direct fix. |
| EC: uint32 millisecond wrap zeros remainder | low | ~49-day uptime edge. Rejected: unlikely everyday use. |
| EC: replaceState + paused remaining 0 clears restored sticky | medium | `valueTreeRedirected` may `resume`→`clearStickyMessage` before sync. Patch: cancel timer before HELP clear/sync. |
| VG: remaining helper unused (same as BH) | medium | Pre-verified; same patch as BH unused helper. |
| VG: Footer HELP/timer composition untested | medium | Pre-verified; `timerCallback` HELP branch mutates remaining without pure coverage. Patch: extract transition helper + tests. |
| VG other: timerCallback zeros remaining under HELP | false | Timer already exhausted; remaining 0 then fire after HELP leaves matches "no clear while HELP covers". |

## Design Notes

- Sticky left-band severity chrome becomes a **fixed square** (prefer `Footer::kIconSize` / badge height) with geometric pictos; HELP / DEVICE / MIDI queue keep text badges via a paint-mode flag on the shared helper — do not force icon mode on those bands.
- Invert fill/glyph like Settings close: fill = severity colour, glyph = footer background (or current badge text colour), so hover cross remains readable.
- Timer ownership: FooterPanel (message thread); policy math in pure helpers. On sticky property change: cancel/restart. On preference → KEEP: cancel. On HELP covering: pause; on uncover: schedule remaining.
- Picto shapes: simple Path geometry (info "i"/dot+stem, warning triangle, error octagon or "X" circle) — keep strokes consistent with close-cross thickness scale; no Unicode.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` — expected: build success for plugin + tests target used by project.
- `ctest --preset macos-debug-arm64 -R InfoMessage` (or the chosen test binary filter) — expected: new policy unit tests pass.
- `python3 Scripts/quality/lint_touched.py` — expected: clean on touched C++ under `Source/` / `Tests/`.

**Manual checks:**
- Settings: INFO MESSAGE above CONTEXTUAL HELP; KEEP default; switch AUTO CLEAR persists across reopen.
- Sticky INFO + AUTO CLEAR: clears ~5 s; HELP hover during countdown pauses; after HELP leaves, remaining delay then clear.
- Sticky WARNING/ERROR + AUTO CLEAR: no auto-clear; badge click clears each.
- Badge hover: picto ↔ cross, no text jump; message-body click does not clear.
- CONTEXTUAL HELP HIDE: no HELP overlay; sticky + badge dismiss still work.
