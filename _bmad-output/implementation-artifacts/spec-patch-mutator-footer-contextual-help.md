---
title: 'Patch Mutator furtive footer contextual help'
type: 'feature'
created: '2026-09-15'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '99cbfc0a6df1a63c1d48ce9b7449399a57f94f4b'
context:
  - '{project-root}/_bmad-output/brainstorming/brainstorm-patch-mutator-playable-calibration-2026-08-26/parking-aide-contextuelle-footer.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-patch-mutator-sticky-footer-action-messages.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Patch Mutator controls use short face labels (C/D/F/E, F/T, R/P, …). Floating tooltips were unsatisfactory; users need Ableton-style status-line help while exploring the module, without losing sticky action/Compare messages.

**Approach:** Add a display-only overlay on the left footer message band for all 22 Patch Mutator controls (3 combos + 8 buttons + 11 toggles). On mouse enter and keyboard focus, show a short English help line; on leave/blur, restore the sticky paint from APVTS. Help never writes `uiMessageText` / `uiMessageSeverity`. Centre MIDI-pressure and right device bands stay unchanged. Scope is Patch Mutator only.

## Boundaries & Constraints

**Always:**
- Overlay API lives on `FooterPanel` (local member + repaint), mirroring the non-APVTS pattern of `setMidiQueuePressureAlert` but painting the **left** band.
- Sticky layer remains APVTS-only; Compare exact-clear (`clearCompareLockedFooterIfPresent` / `kCompareLockedFooter`) and Delete/Flush/Export sticky paths stay untouched.
- Wire hover/focus from `PatchMutatorPanel` (single map control → help string); resolve `FooterPanel` via `MainComponent::getFooterPanel()` (parent climb or equivalent — no Core→GUI coupling).
- Include keyboard focus help where controls are focusable; disabled controls still show help on mouse enter when the cursor can reach them (Compare film may block all but C).
- Anti-flicker: short leave-clear delay so traversing adjacent Mutator controls does not flash sticky between them (agent-chosen ~50–100 ms; cancel delay on new enter).
- Help strings are English UI constants under `PluginDisplayNames` (Mutator help namespace); keep lines short enough for the left band (paint-time truncate OK).
- **Help chrome (decided):** left band shows a `HELP` badge + detail text. Badge rectangle fill uses the same white as GUI labels (`SkinColourId::kLabelText`); badge glyph colour is dark (footer background / readable on white). Detail text to the right of the badge uses the same white (`kLabelText`) so help contrasts with INFO's chrome-grey badge/detail and reads as furtive.
- **Overlay priority (decided):** while help is active, always temporarily cover the left sticky paint — including Warning and Error — then restore sticky on leave/blur. Centre system MIDI alert stays independent.
- Exact help copy for v1 (human-renegotiated: same `Patch Mutator: ` prefix as sticky footers):
| Control | Help text |
|---------|-----------|
| MODE | Patch Mutator: Sets how far mutations stray - Kindred, Drift, Warp, or Wild. |
| PITCH | Patch Mutator: Controls how DCO pitch may move - Keep, Consonant, Dissonant, or Free. |
| HISTORY | Patch Mutator: Recalls a mutation or retry from this session. |
| MUTATE | Patch Mutator: Creates a new variation from the current recipe and sends it to the synth. |
| RETRY | Patch Mutator: Rolls again from the same mutation root. |
| `<` | Patch Mutator: Steps backward through session history. |
| `>` | Patch Mutator: Steps forward through session history. |
| C | Patch Mutator: Compares with the origin patch and locks editing until you click C button again. |
| D | Patch Mutator: Deletes the selected history entry. |
| F | Patch Mutator: Flushes the whole session mutation history. |
| E | Patch Mutator: Exports the session mutations as SysEx files. |
| D1 | Patch Mutator: Include DCO 1 module in the mutation recipe. |
| D2 | Patch Mutator: Include DCO 2 module in the mutation recipe. |
| F/A | Patch Mutator: Include VCF/VCA module in the recipe. |
| F/T | Patch Mutator: Include FM/TRACK module in the recipe. |
| R/P | Patch Mutator: Include RAMP/PORTAMENTO module in the recipe. |
| E1 | Patch Mutator: Include ENV 1 module in the recipe. |
| E2 | Patch Mutator: Include ENV 2 module in the recipe. |
| E3 | Patch Mutator: Include ENV 3 module in the recipe. |
| L1 | Patch Mutator: Include LFO 1 module in the recipe. |
| L2 | Patch Mutator: Include LFO 2 module in the recipe. |
| MM | Patch Mutator: Include MATRIX MODULATION module in the recipe. |

**Never:**
- Write help into APVTS sticky properties (Unison-badge `setFooterInfoMessage` pattern is forbidden for this feature).
- Classic JUCE floating tooltips as the primary UX.
- Footer band rename / centre-vs-left redesign; GUI-wide help; sticky Delete/Flush/Export rework; Master Edit / Patch Edit extension; touch `spec-master-pull-on-connect.md`.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Hover help | Mouse enters a Mutator control | Left band shows that control's help; APVTS sticky unchanged | N/A |
| Leave restore | Mouse leaves Mutator controls (after anti-flicker delay) | Left band paints sticky again (or empty) | N/A |
| Focus help | Keyboard focus on Mutator control | Same overlay as hover | N/A |
| Blur restore | Focus leaves Mutator control without another Mutator focus | Sticky restored after delay / immediately if no pending enter | N/A |
| Compare + C | Compare active; hover/focus C | Help overlays Compare sticky visually; sticky text remains in APVTS | N/A |
| Compare + other | Compare film active; cursor over dimmed control | No help (film eats mouse); sticky Compare stays | N/A |
| Adjacent traverse | Mouse moves Mode → Mutate quickly | No sticky flash between helps | N/A |
| Sticky writer during help | Export/Delete/etc. updates APVTS while help overlay active | Overlay still shows help until leave; then new sticky appears | N/A |
| Warning/Error sticky + hover | Left sticky severity Warning or Error; mouse enters Mutator control | Help HELP-badge overlay covers it temporarily; APVTS unchanged; leave restores Warning/Error | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.{h,cpp}` -- add set/clear contextual-help overlay members; when overlay active paint left band with HELP badge (white fill / dark glyph) + white detail via `paintBadgeAndDetail`; never touch APVTS; truncate helpers as needed.
- `Source/GUI/MainComponent.h` -- `getFooterPanel()` already exists; wiring target for Mutator panel.
- `Source/GUI/Panels/.../PatchMutatorPanel.{h,cpp}` (+ Setup/History as needed) -- register mouse enter/exit + focus gained/lost (or panel FocusChangeListener) for all 22 controls; map to help strings; call FooterPanel overlay API; anti-flicker Timer on leave.
- `Source/Shared/Definitions/PluginDisplayNames.h` -- `PatchMutatorModule` help-string constants (new nested namespace).
- `Source/GUI/Helpers/GrayedControlHelper.*` -- do **not** use for help (APVTS writers).
- `Source/GUI/Helpers/CompareLockBinder.cpp` / `PatchMutatorEngineInternal.h` -- leave Compare sticky exact-clear intact.
- `Source/GUI/PluginEditorTimers.cpp` -- precedent for FooterPanel non-APVTS API (`setMidiQueuePressureAlert`).
- `_bmad-output/brainstorming/.../parking-aide-contextuelle-footer.md` -- product SSOT for overlay model.
- Unit tests: prefer a small FooterPanel-facing helper test if extractable (overlay active vs sticky restore without GUI); otherwise Verification = Standalone UAT. Do not invent Core tests for display-only help.

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.{h,cpp}` -- add contextual help overlay API + left-band paint precedence when overlay active -- display-only ephemeral layer
- [x] `Source/Shared/Definitions/PluginDisplayNames.h` -- add the 22 English help string constants -- SSOT UI copy
- [x] `Source/GUI/Panels/.../PatchMutatorPanel*` -- wire all 22 controls to overlay show/clear with anti-flicker -- Mutator-only first slice
- [x] `Tests/Unit/ContextualHelpOverlayTests.cpp` + `Source/GUI/Helpers/ContextualHelpOverlay.h` -- matrix coverage for overlay vs sticky, clear gate, Compare film hole, help copy
- [ ] Standalone UAT -- hover each control; leave restores sticky; Compare + C; Export sticky intact (human)

**Acceptance Criteria:**
- Given a sticky Mutator/Compare/INFO message is stored, when the user hovers a Patch Mutator control, then the left band shows that control's help and APVTS `uiMessageText` is unchanged.
- Given help is showing, when the pointer leaves all Mutator controls (after anti-flicker), then the left band shows the previous sticky again (or empty).
- Given Compare is active, when the user hovers C, then help temporarily replaces the Compare sticky on screen and leaving C restores the Compare sticky text.
- Given keyboard focus moves onto a focusable Mutator control, when focus is gained, then the same help appears as for hover.
- Given the centre MIDI-pressure alert is active, when Mutator help shows, then the centre band is unaffected.

### Review Findings

- [x] [Review][Decision] Keep non-Mutator ASCII collateral in this chantier? — Resolved: keep MIDI + first-load-after-scan ASCII in this livraison; truncate matcher fix remains a patch.
- [x] [Review][Patch] Path-style truncate still looks for em-dash ` — Loaded ` after formatters switched to ASCII ` - Loaded ` [`FooterPanel.cpp:100`] — fixed: accept both ` - Loaded ` and ` — Loaded `
- [x] [Review][Patch] When Mutator panel is not showing, focus handler returns without clearing HELP — overlay can stick [`PatchMutatorPanelContextualHelp.cpp:190-191`] — fixed: schedule clear when panel not showing
- [x] [Review][Patch] Popup clear policy incomplete: HELP can clear while Mode/Pitch/History menu is open (open but unfocused), and early-return on popup focus does not reschedule clear so HELP can stick after dismiss; extract/test a pure keep-overlay predicate [`PatchMutatorPanelContextualHelp.cpp:158-174`] — fixed: defer while focus-in-popup or mutator popup modal; reschedule; helper + unit test
- [x] [Review][Defer] Scenario-named overlay tests exercise helpers only, not FooterPanel/Mutator wiring — deferred: already logged; CONVENTIONS avoid GUI paint tests; Standalone UAT
- [x] [Review][Defer] No automated census that all 22 controls are bound — deferred: already logged; UAT hover-all-22
- [x] [Review][Defer] Compare+other help inactivity not asserted beyond film policy — deferred: already logged; Standalone Compare check
- [x] [Review][Defer] HELP badge paint colours (white fill / dark glyph / white detail) untested — deferred: paint/GUI unit-test habit; visual UAT
- [x] [Review][Defer] `CONVENTIONS.md` lacks ASCII-only UI punctuation section despite project-context / ascii-display-strings SSOT — deferred: agent-context / conventions sync outside product patch
- [x] [Review][Defer] ascii-display-strings checklist omits consumer-side matchers that must track formatter punctuation — deferred: agent-context doc polish

#### Rejected

- `false` — `shouldPaintContextualHelpOverSticky` identity: intentional always-cover policy when overlay active (prior triage + AC).
- `false` — Toggle “mutation recipe” vs “recipe” / Include vs Sets voice / “click C button”: match frozen approved help table; fix would edit frozen copy/spec.
- `false` — Empty Spec Change Log / missing verification transcript: fix would be editing this build’s spec artifact.
- `low` — Prefixed help length / mid-clause truncate: speculative; paint-time truncate already allowed; no measured overflow evidence.
- `false` — Acceptance: formal ACs and Always/Never for Mutator overlay appear met; residual gaps are verification/defer or the decision/patches above.

## Implementation Notes

- FooterPanel uses `TSS::ContextualHelpOverlay` + HELP badge paint (white fill / dark glyph / white detail via `kLabelText`).
- PatchMutatorPanel wires 22 controls (mouse + FocusChangeListener); 75 ms deferred clear; resolves footer via `MainComponent::getFooterPanel()`.
- Review patches: deep mouse listeners + parent walk for help lookup; keep overlay while Mutator popup menus hold focus; prefer focused control over hover when both active; ignore focus churn when panel not showing; removed unused FooterPanel overlay getters.
- Review patches (2026-09-15 code review): ASCII path truncate accepts ` - Loaded ` and legacy em dash; schedule HELP clear when Mutator panel is not showing; defer clear while Mutator popup focus/modal and reschedule; `shouldDeferContextualHelpClearForMutatorPopup` unit-tested.
- ASCII fix (2026-09-15): contextual help MODE/PITCH use ` - ` not em dash; SSOT `_bmad/custom/ascii-display-strings.md`.
- Matrix rows covered by `ContextualHelpOverlayTests` (ran `--category ContextualHelpOverlay`, 0 failures). Standalone visual UAT still for Guillaume.
- Build: `macos-debug-arm64` OK; `lint_touched.py` OK.

## Spec Change Log

## Review Triage Log

- Blind: Mode ComboBox nested chrome misses mouse — **medium** — `addMouseListener(..., false)` skips child label hits; patched to `true` + parent walk in `helpTextForControl`.
- Blind: adjacent MODE/PITCH/HISTORY labels unbound — **false** — intent is the 22 controls only, not face labels beside them.
- Blind + Edge: popup open clears help — **medium** — mouseExit/focus leave cleared overlay while menu open; patched to keep overlay when focus is inside Mutator popup menus.
- Blind + Verif: unit tests restage helpers not FooterPanel/Mutator wiring — **medium** (verification) — pre-verified gap; **defer** to Standalone UAT / GUI-test norms.
- Blind: `shouldPaintContextualHelpOverSticky` is identity — **false** — intentional thin policy hook; Warning/Error cover is paint help whenever overlay active.
- Blind + Verif: Compare+other test only checks film policy — **medium** (verification) — **defer** with UAT Compare+other.
- Blind + Verif: no assert that all 22 binds register — **medium** (verification) — **defer**; UAT hover-all-22.
- Blind: mouseEnter/exit public — **false** — required `Component` overrides.
- Blind: empty Spec Change Log / missing command logs — **false** — reject findings that demand editing this build's spec as the fix.
- Blind: Compare help copy "click C again" on first hover — **false** — approved frozen copy.
- Blind: unused FooterPanel overlay getters — **low** — removed (no callers).
- Blind: toggle vs action help voice inconsistency — **false** — approved frozen table.
- Blind: FocusChangeListener while Mutator not showing — **low** — patched with `isShowing()` early return.
- Blind: Design Notes vs always-cover Warning/Error — **false** — reject edit-spec; frozen decision already recorded.
- Edge: hover+focus on two controls wrong help — **medium** — map iteration order; patched to prefer keyboard focus then mouse-over.
- Verif: three scenario gaps (FooterPanel/Mutator wiring, 22 binds, Compare+other help) — dispositions **defer** as filed.

## Design Notes

- Precedent: centre-band `setMidiQueuePressureAlert` proves FooterPanel can hold ephemeral display state without APVTS. Left-band help follows that pattern.
- Unison “M” badge writes sticky on hover — wrong model; do not copy.
- Help copy uses the same `Patch Mutator: ` prefix as sticky Mutator footers (renegotiated after first ship).
- HELP badge label constant under `PluginDisplayNames::FooterPanel` (e.g. `kContextualHelpBadge = "HELP"`).
- Agent-owned: leave-clear delay ~50–100 ms; cancel on re-enter; no GUI-wide registry in this chantier.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds (adjust preset if needed)
- `ctest --preset macos-debug-arm64` or project unit-test target when tests added -- expected: pass (or N/A if UAT-only)
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++

**Manual checks (if no CLI):**
- Standalone: hover all 22 Mutator controls → readable help; leave → last sticky restored; Compare on → C help then Compare sticky back; run Delete/Flush/Export → sticky still works; no help written into persistent footer after leave.
