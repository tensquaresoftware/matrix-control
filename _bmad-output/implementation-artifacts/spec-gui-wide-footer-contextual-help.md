---
title: 'GUI-wide furtive footer contextual help'
type: 'feature'
created: '2026-09-16'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'a0d866002f3e1e2f31cc1eeed2e3a39f6ed94c96'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-patch-mutator-footer-contextual-help.md'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Only Patch Mutator shows Ableton-style furtive HELP in the left footer band. Everywhere else, short face labels still leave users guessing while sticky Compare / action messages must not be lost.

**Approach:** Reuse the existing FooterPanel HELP overlay (badge + white detail, never writes APVTS sticky) and bind the same enter/focus/leave behaviour across main-editor controls, Header, interactive curve displays, Settings, and About. Help lines are English ASCII `MODULE: detail`.

**Decided (2026-09-16):**
- Header HELP prefix: `SESSION`
- Patch Mutator HELP prefix: `PATCH MUTATOR:` (GUI casing). Sticky Mutator footers stay `Patch Mutator:` this wave.
- Copy: Desktop inventory frozen at `/Users/Guillaume/Desktop/Matrix-Control-aide-contextuelle-messages.md` (post SAMPLE INPUT / BANK / INIT / MATRIX P / PATCH NAME revisions).
- Scope includes EnvelopeDisplay x3, Track Generator curve, Settings modal controls, and About hyperlinks.
- Settings HELP prefix: `SETTINGS`. About HELP prefix: `ABOUT`.

## Boundaries & Constraints

**Always:**
- Keep HELP chrome and sticky restore behaviour identical to Patch Mutator (overlay covers sticky including Warning/Error; leave restores APVTS sticky; anti-flicker clear delay; keyboard focus where focusable).
- Put new strings in `PluginDisplayNames` ContextualHelp namespaces; ASCII punctuation only (`-`, `...`).
- Resolve FooterPanel via MainComponent climb for editor panels; for Settings/About windows inject a FooterPanel accessor from PluginEditor (no Core→GUI).
- Bind every control listed in the frozen Desktop inventory plus Settings/About tables below.
- Update existing Mutator ContextualHelp string literals to `PATCH MUTATOR:` prefix only (not sticky Messages).

**Never:**
- Write help into APVTS sticky properties.
- Classic floating tooltips as primary UX.
- Change sticky Compare / Delete / Flush / Export writers or their `Patch Mutator:` sticky prefix.
- Touch `spec-master-pull-on-connect.md`.
- Bind non-interactive Header LEDs / peak meters or Footer DEVICE band.

## Approved copy — Settings / About (added after Desktop freeze)

| Control | Help text |
|---------|-----------|
| MATRIX-1000 PATCHES | SETTINGS: Chooses how Matrix-1000 patch names are displayed. |
| COMPUTER PATCHES | SETTINGS: Chooses how computer .syx patch names are displayed. |
| UNSAVED STATE | SETTINGS: Chooses when to warn about unsaved patch changes. |
| SAVE AS INIT (patch) | SETTINGS: Saves the current patch as the patch init template. |
| DELETE (patch init) | SETTINGS: Deletes the saved patch init template. |
| DELETE WARNING | SETTINGS: Chooses when Patch Mutator delete asks for confirmation. |
| DEFRAG HISTORY | SETTINGS: Coming soon - renumber mutation history when full. |
| HARDWARE LATENCY | SETTINGS: Sets inter-SysEx delay for reliable hardware transfers (plugin). |
| LOAD (master) | SETTINGS: Loads a Master settings file into the editor. |
| SAVE AS (master) | SETTINGS: Saves current Master settings as a new file. |
| INIT (master) | SETTINGS: Resets Master settings to the master init template. |
| SAVE AS INIT (master) | SETTINGS: Saves current Master settings as the master init template. |
| DELETE (master init) | SETTINGS: Deletes the saved master init template. |
| SETTINGS shortcut row | SETTINGS: Shows the keyboard shortcut that opens Settings. |
| AUDIO/MIDI shortcut row | SETTINGS: Shows the keyboard shortcut that opens Audio/MIDI. |
| UI SCALE shortcut row | SETTINGS: Shows the keyboard shortcut cycle for UI Scale. |
| SKIN shortcut row | SETTINGS: Shows the keyboard shortcut cycle for Skin. |
| Email link | ABOUT: Opens email to Ten Square Software. |
| GitHub link | ABOUT: Opens the Matrix-Control GitHub repository. |
| LinkedIn link | ABOUT: Opens the author LinkedIn profile. |

Main-editor copy: use the frozen Desktop file verbatim (SESSION Header, modules, BANK RAM/ROM split, INIT without edit-buffer wording, SAMPLE INPUT SAMPLED wording, MATRIX P buses wording, PATCH MUTATOR uppercase HELP).

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Hover help | Mouse enters a bound control | Left band shows that control HELP; sticky APVTS unchanged | N/A |
| Leave restore | Mouse leaves bound controls after delay | Sticky paint restored | N/A |
| Cross-module traverse | Mouse moves Mutator → DCO 1 quickly | No sticky flash between helps | N/A |
| Sticky during help | Sticky updates while overlay active | Overlay stays until leave; then new sticky | N/A |
| Warning sticky + hover | Warning/Error sticky present | HELP covers it temporarily; leave restores it | N/A |
| Settings/About hover | Mouse enters bound control in modal | Same left-band HELP on main editor footer | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Helpers/ContextualHelpOverlay.h` -- overlay state + clear/paint predicates (reuse; generalize naming away from Mutator-only comments where touching)
- `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.{h,cpp}` -- `setContextualHelpOverlay` / `clearContextualHelpOverlay` / HELP paint (reuse)
- `Source/GUI/Panels/.../PatchMutatorPanelContextualHelp.cpp` -- reference pattern; migrate to shared binder helper; update HELP strings to `PATCH MUTATOR:`
- New shared helper under `Source/GUI/Helpers/` (e.g. `ContextualHelpBinder`) -- map control→string, mouse/focus, delayed clear, FooterPanel resolve; optional popup-defer hook for combo menus
- `Source/Shared/Definitions/PluginDisplayNames.h` -- ContextualHelp namespaces for all modules; Desktop + Settings/About tables
- Panels: HeaderPanel; Patch Edit modules; PatchNameDisplayPanel; EnvelopeDisplay; TrackGeneratorDisplay; MatrixModulationPanel; Master Midi/Vibrato/Misc; BankUtility; InternalPatches; ComputerPatches; PatchMutator; SettingsPanel; AboutPanel
- Settings/About: PluginEditor passes FooterPanel reference or resolver into window/panel setup
- Register any new `.cpp` in `CMakeLists.txt` `PLUGIN_SOURCES`
- Do not change Core services for help text

## Tasks & Acceptance

**Execution:**
- [x] `PluginDisplayNames.h` -- ContextualHelp constants for all approved modules (Desktop + Settings/About); Mutator HELP → `PATCH MUTATOR:`
- [x] Shared `ContextualHelpBinder` (or equivalent) -- extract Mutator bind/show/clear; reuse everywhere
- [x] Wire all approved panels + curve displays + Header
- [x] Wire SettingsPanel + AboutPanel via PluginEditor FooterPanel accessor
- [x] `PatchMutatorPanelContextualHelp.cpp` -- migrate to helper; keep sticky Messages title-case
- [x] `CMakeLists.txt` -- register new sources
- [x] `python3 Scripts/quality/lint_touched.py` on touched C++
- [x] Prefer unit tests for binder clear-generation / overlay predicates if extractable; otherwise document manual hover matrix coverage

**Acceptance Criteria:**
- Given sticky Compare or Warning on the left band, when hovering any bound control, then HELP overlay shows and sticky text remains in APVTS; when leaving, sticky paint returns.
- Given approved copy, when hovering a control, then left-band detail matches the frozen `MODULE: ...` string (Header=`SESSION`, Mutator HELP=`PATCH MUTATOR`).
- Given focusable bound controls, when keyboard focus enters/leaves, then HELP shows/clears like hover.
- Given Settings or About open, when hovering a bound control there, then the main editor left footer shows HELP the same way.
- Given Patch Mutator sticky footers, when an action writes sticky text, then prefix remains `Patch Mutator:` (unchanged this wave).

## Implementation Notes

- Review patches (2026-09-16): Footer declared before Header/Body; Header logo modal detaches binder on Header teardown; epoch wrap + `shouldClearContextualHelpOverlayForEpoch` + unit test. Manual Standalone hover UAT still recommended.
- Follow-up (2026-09-16): Header activity LEDs + audio peak HELP; Footer DEVICE right-band hit area with `DEVICE:` HELP.
- Compile fixes after initial dispatch: `namespace Help =` (not `using Help =`) for ContextualHelp namespaces; include concrete widget headers where panels only forward-declare types; Footer epoch on FooterPanel for multi-binder coexistence.
- Extracted MatrixModulation / ComputerPatches HELP wiring to `*ContextualHelp.cpp` to keep file-size gate green.
- Patch NAME HELP bound from `PatchEditDisplaysPanel` (avoids PatchNameDisplayPanel ctor param lint).
- Verification: `macos-debug-arm64` build OK; `lint_touched.py` OK; `Matrix-Control_Tests ContextualHelpOverlay` 0 failures.
- Shared `TSS::ContextualHelpBinder` (`Source/GUI/Helpers/ContextualHelpBinder.{h,cpp}`) owns control→HELP map, mouse/focus, 75 ms clear delay, and FooterPanel epoch-scoped clear so multi-panel binders do not wipe each other.
- `FooterPanel::setContextualHelpOverlay` returns a monotonic epoch; `clearContextualHelpOverlayIfEpoch` clears only if still the owner.
- HELP copy: Desktop SSOT verbatim for main editor (`SESSION` / modules / `PATCH MUTATOR:`); Settings/About from frozen Spec tables. Sticky Mutator Messages remain `Patch Mutator:`.
- Wired: Header (+ logo popup rows), all Patch Edit / Master Edit BaseModule panels, Envelope/Track displays, Patch Name, Matrix Modulation, Bank Utility, Internal/Computer Patches, Patch Mutator (migrated off panel Mouse/Focus listeners), Settings/About via PluginEditor FooterResolver inject.
- Accessors added: `ModuleHeader` I/C/P getters; `ModulationBusCell` control getters.

## Spec Change Log

## Review Triage Log

- 2026-09-16 — Edge: Footer destroyed before binders (UAF on teardown) → **patch** — `MainComponent.h` declares `footerPanel` before Header/Body; ctor takes `MainComponentConstructionArgs` so init-list order matches (no `-Wreorder-ctor`, lint params OK).
- 2026-09-16 — Edge: Logo popup holds raw binder pointer after Header teardown → **patch** — `HeaderPanel` dtor detaches binder and dismisses modal `HeaderLogoPopupMenu`; `detachContextualHelpBinder()`.
- 2026-09-16 — Edge: signed epoch overflow can stick HELP → **patch** — wrap `contextualHelpEpoch_` before increment; clear via `shouldClearContextualHelpOverlayForEpoch`.
- 2026-09-16 — Verification: no epoch-ownership unit test → **patch** — predicate + `ContextualHelpOverlayTests::epochOwnershipGatesClear`.
- 2026-09-16 — Blind: `PluginEditorSettings` dead MainComponent/Footer includes → **dismiss** — already removed.
- 2026-09-16 — Blind: ENV “behaviour” spelling → **dismiss** — matches frozen Desktop SSOT.
- 2026-09-16 — Blind: project-popup defer only covered by Mutator bool helper; rename Mutator helper; multi-binder FocusChangeListener; hostShowing only Mutator; Bank header / Ramp Master Override / Settings row labels / array-length assert / MatrixModulation include hygiene / duplicate Mutator comment → **defer** (see deferred-work.md).
- 2026-09-16 — Decision: Header LED/peak + Footer DEVICE HELP vs frozen Never → **keep** (Guillaume); Never superseded for this wave by Desktop SSOT.
- 2026-09-16 — Patch: Settings/About `setHostShowingPredicate` → applied.
- 2026-09-16 — Patch: epoch Footer handshake helper + unit test → applied.

### Review Findings

- [x] [Review][Decision] Validate Header LED/peak + Footer DEVICE HELP vs frozen Never — resolved 2026-09-16: keep bindings (Desktop SSOT + wave intent); frozen Never treated as superseded for this wave.
- [x] [Review][Patch] Settings/About HELP can linger after window hide [Source/GUI/Settings/SettingsPanel.cpp:25] — register `setHostShowingPredicate` (or scheduleClear on close) like Mutator; windows are new hosts without a visibility gate.
- [x] [Review][Patch] Epoch clear not covered through FooterPanel API [Tests/Unit/ContextualHelpOverlayTests.cpp:186] — extend test: set A → set B → clearIfEpoch(A) must leave B; clearIfEpoch(B) must clear.
- [x] [Review][Defer] Project-popup defer unit coverage [Source/GUI/Helpers/ContextualHelpBinder.cpp:14] — deferred: already in deferred-work.md
- [x] [Review][Defer] Rename Mutator-era popup defer helper — deferred: already in deferred-work.md
- [x] [Review][Defer] Shared FocusChangeListener / multi-binder consolidation — deferred: already in deferred-work.md (also out of scope for this review)
- [x] [Review][Defer] setHostShowingPredicate on hideable Patch Manager tabs — deferred: already in deferred-work.md (Settings/About handled as Patch above)
- [x] [Review][Defer] Optional HELP targets outside Desktop inventory — deferred: already in deferred-work.md
- [x] [Review][Defer] bindParameterCellHelps length assert — deferred: already in deferred-work.md
- [x] [Review][Defer] Mutator ContextualHelp duplicate banner comment — deferred: already in deferred-work.md

#### Rejected

- false — Never vs LED/DEVICE as *code* defect alone: Desktop Always + SSOT require those rows; treated as Decision instead of auto-revert.
- false — “synth” vs “synthesizer” on MISC MEMORY PROTECT: matches Desktop SSOT verbatim.
- false — Compare HELP “[C] button again”: matches Desktop SSOT verbatim.
- reject — Spec Design Notes still say Mutator-only popup defer: fix would edit the spec under review.
- reject — No checked-in Desktop inventory / absolute Desktop path: fix would edit spec/agent-context artifacts.
- low — Logo popup spacer/rule keeps prior HELP while modal: anti-flicker defer by design; everyday impact negligible.
- low — Expand helpCopyConstantsMatchSpec beyond Mutator: Spec already accepted Mutator asserts + manual hover matrix.
- low — Hover/focus unit test names do not drive ContextualHelpBinder: historical pure-gate style; not a new defect in this wave.

## Design Notes

Desktop SSOT (main editor): `/Users/Guillaume/Desktop/Matrix-Control-aide-contextuelle-messages.md`

Extract shared binder once; Mutator popup-defer remains Mutator-specific optional callback. Settings/About are DocumentWindows — inject footer resolver from PluginEditor.

## Verification

**Commands:**
- `python3 Scripts/quality/lint_touched.py` -- expected: pass on touched Source C++
- `cmake --build --preset macos-debug-arm64` -- expected: build success when toolchain allows

**Manual checks:**
- Hover Header MIDI FROM, DCO 1 FREQUENCY, envelope curve, Matrix Mod AMOUNT, Internal STORE, Settings combo, About GitHub: HELP then sticky restore
- Compare lock + hover Mutator C then DCO: sticky Compare returns after leave
