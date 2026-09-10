# Deferred Work

## Deferred from: code review of spec-computer-patches-save-as-naming.md (2026-09-04)

- source_spec: `_bmad-output/implementation-artifacts/spec-computer-patches-save-as-naming.md`
  summary: SAVE AS picker may skip a second overwrite confirm when the final uppercase sibling differs from the path the native dialog already confirmed.
  evidence: dialog warns on chosen path; code returns siblingFile(normalized) which can be a different case-sensitive path.

<!-- Resolved 2026-09-04: case-fold twin `.syx` removal via removeCaseFoldTwinSyxFiles after successful SAVE. -->

## Deferred from: Settings modal reorg (2026-09-03)

- **Release diagnostic logging toggle** — Settings no longer shows a LOGGING row (hidden until designed). Revisit later: optional user-facing Support/Diagnostic log (single on/off for MIDI + APVTS + future loggers), file location, size/rotation limits, and release performance cost before exposing in the Settings modal.
- **Ask-once / footer vocabulary vs Settings** — Settings now says SYSEX / FILE NAMES; the ask-once dialog and Computer Patches load footer still say “Internal name” / “Filename”. Align product copy in a follow-up.
- **Core policy header filenames** — `UnsavedEditWarningPolicy.h`, `MutatorDeleteWarningPolicy.h`, `PatchNameDisplayMode.h` still use older names while Settings IDs speak Unsaved State / Delete Warning / Matrix-1000 Patches Names. Rename files/namespaces when convenient.
- **Unused Settings display strings** — SKIN / UI SCALE (and related) remain in `PluginDisplayNames::Settings` but are not shown in the reorganized modal; cleanup with the next Settings surface pass.
- **Settings GUI regression tests** — no automated coverage for section order, combo labels/IDs, or design width/height.

## Deferred from: code review of spec-settings-modal-sections-and-naming.md (2026-09-03)

- **Fixed Settings height vs plugin/standalone** — `kDesignHeight = 420` does not shrink when HARDWARE LATENCY is hidden in standalone; empty vertical space remains. Pre-existing layout debt; revisit with a content-driven height pass.

## Deferred from: code review of spec-patch-mutator-playable-calibration.md (2026-08-29)

- **Incomplete calibration / anti-silence tests** — motion guarantee, Preserve destination invent, choice-step limits, Consonant/Dissonant set membership, active-DCO cardinality, escalated ENV floors and FM nudge are only partly covered; re-open in the tests review chunk.
- **Kindred LFO “rates that breathe”** — addendum A4 #4 is SHOULD; Kindred soft-avoid of very-slow + deep toward volume not implemented in this Must ship.
- **Unused `amplitudeDestinations` catalog** — built in `MatrixModChoiceCatalog` but never read; cleanup after Must patches land.
- **Engine boundary tests** — APVTS MODE/PITCH/octaves → recipe, forced RETRY diversity re-roll, and no-change footer copy lack direct assertions; cover in tests chunk.
- **Legacy Amount/Random surface** — descriptors and session defaults still present for migration; not primary UI; cleanup follow-on.
- **MODE-aware RETRY diversity threshold** — fixed 3-byte bar for all MODEs; Kindred may hit fallback often (A4 #5 SHOULD).
- **Pitch UI helper / recipe-property / Compare-lock tests** — pure id helpers and property list untested; cover in tests chunk.
- **MODE/PITCH component ids** — hand-built combos lack WidgetFactory SSOT ids; harness tooling follow-on.
- **Compare inactive paint on HierarchicalComboBox** — Compare lock uses setEnabled only (same as History); optional setInactiveAppearance polish.
- **Remaining calibration test gaps (after chunk 4 triage)** — Consonant/Dissonant set membership + octave window; choice-step / WaveSelect CHOIX / active-DCO cardinality; escalated ENV + FM nudge; filter/Mix through full apply; engine buildRecipeFromApvts A/R + octaves; pitch UI id helpers / compact labels; soft probabilistic gates; Preserve empty-timbre invent.

## Deferred from: smoke playable calibration (2026-08-29)

- None outstanding from silence cases 1–7 (ENV 2 sustain/release; MM amplitude openers; FREQ&lt;ENV1 negative depth; near-zero resonance + MM→VCF freq; velocity soft-kill + LFO→VCA tremolo cap; Banjo-style filter openers + ENV 1 amp; ENV 2 external trigger / DADR / long delay → keyboard volume path). UI: PRESERVE→KEEP shipped; ± glyph OK on PT Sans Narrow; KINDRED at width limit (monitor only).
- **PITCH Consonant/Dissonant contract** — relative DCO1↔DCO2 intervals + MODE repick ladder shipped; PITCH combo greys when no DCO is both mutable and audible (or Compare on). Optional: joint transpose under Warp/Wild.
- **MutationMatrixModPolicy.cpp over light file-size gate** — useful lines ~430 after Cas 5–6 openers; split helpers on next MM policy touch.

## Deferred from: code review of commit d8e20d0 — editorial undo MIDI resync + stack depth (2026-08-26)

- **Tests sur le vrai `PluginProcessor`** — la politique MIDI editorial (quiet window, coalesce cancel, ordre resync/flush) est vérifiée via `EditorialUndoMidiHarness` dupliqué, pas via `PluginProcessor::performEditorialUndo()` ; une régression production pourrait repasser inaperçue malgré le smoke MIDI Monitor validé manuellement.
- **Limites undo production non épinglées** — `fullApvtsLayoutWithProductionUndoLimitsAllowsThreeSequentialUndos` injecte `(100, 100)` dans le harness ; un revert de la ligne `PluginProcessor.cpp` ne ferait pas échouer les tests.
- **Params master absents des tests MIDI editorial** — `EditorialUndoMidiHarness` n’exerce pas `suppressMasterParameterSysEx_` ni `apvtsMasterMapper` dans le resync ; branche master non couverte par les nouveaux tests.
- **Doc `undo-policy.md` incomplète** — pas de scénarios MIDI dans la edge matrix ; pas de doc du couplage `setMaxNumberOfStoredUnits(max, minTransactionsToKeep)` ; pas de note sur le blocage granular SysEx pendant la fenêtre quiet ~500 ms après undo/redo.
- **Test d’éviction à 101 transactions** — contrainte SPEC « 100 transactions avec éviction du plus ancien » non vérifiée par ce commit (hors périmètre du bugfix 3-undo).

## Deferred from: code review of spec-undo-redo-patch-editing epic transversal (2026-08-24)

- Master-parameter editorial undo/redo has no `UndoManager*` test coverage (CAP-1 master scope).
- Matrix Mod module Init/Paste `beginEditorialTransaction` paths untested (patch-module bulk tests only).
- Patch load checkpoints (device dump, computer `.syx`, Internal Patches navigation) not asserted end-to-end via `PatchManagerActionHandler` — `UndoManagerCheckpointPolicyTests` seam test manually clears stack instead of calling `establishEditorialCheckpoint()`.
- Slider/display widget undo tests simulate harness writes or run keys without stack assertions — production `TSS::Slider` / display binding paths not regression-protected.
- Undo-policy edge rows untested: two edits + one undo → still dirty; mutate → tweak → undo reverts tweak only; display redo SysEx resync; compare-active blocks redo (undo-only test today).
- Envelope display grouping: multi-parameter drag session tests only `kAttack` within one undo step.
- `PluginProcessor::swapMatrixModBusContents` reorder undo covered only via local `UndoReorderHarness`, not processor integration.

## Deferred from: code review of spec-12-4-matrix-mod-reorder-undo-and-keyboard-shortcuts (2026-08-24)

- No automated `PluginEditor::keyPressed` undo/redo test — AC4 is covered by manual verification in spec; GUI harness cost deferred for v1.
- Envelope/TrackGenerator binding `getUndoManager()` → `apvts_.undoManager` — minor supporting fix outside listed tasks.
- Manual §7 omits undo-policy extras (checkpoint clears, stack depth, dirty state) — AC6 satisfied; fuller manual alignment deferred to story 12-5 stack-clear work.

## Deferred from: code review of spec-12-3-interactive-display-gesture-transactions (2026-08-24)

- I/O matrix “external sync during drag” row not covered — no test asserts listener-only refresh of a non-active parameter leaves undo stack unchanged during an open gesture.

## Deferred from: code review of spec-12-2-bulk-editorial-transactions-for-init-and-paste-module (2026-08-24)

- PluginProcessorConstruction `beginEditorialTransaction` hook wiring has no dedicated test; `UndoManagerModuleBulkTests` harness duplicates the lambda.
- Init redo not covered in bulk undo tests (paste redo is covered).
- Temp init fixture directories created in bulk tests are not deleted after runs.
- Duplicate `valueTreePropertyChanged` logic between `PasteUndoHarness` and `InitUndoHarness`.

## Deferred from: quick-dev spec-clipboard-compatible-blink-feedback (2026-07-25)

- source_spec: `_bmad-output/implementation-artifacts/spec-clipboard-compatible-blink-feedback.md`
  summary: No PluginProcessor integration tests for arm/disarm, Escape clear, or cross-patch unlock via onPatchLoaded.
  evidence: Blind Hunter; resolver unit tests cover blink roles; processor lifecycle verified manually / by wiring.

- source_spec: `_bmad-output/implementation-artifacts/spec-clipboard-compatible-blink-feedback.md`
  summary: Escape may not reach PluginEditor when a child component keeps keyboard focus (text fields / host focus policy).
  evidence: Edge Case Hunter; mitigated by grabKeyboardFocus on mouse down; JucePlugin_EditorRequiresKeyboardFocus is still 0 for hosts.

- source_spec: `_bmad-output/implementation-artifacts/spec-clipboard-compatible-blink-feedback.md`
  summary: Compare lock parent alpha can fight per-button blink alpha on C/P during Compare.
  evidence: Blind Hunter; Compare and clipboard feedback are rare simultaneous; revisit only if UX conflict appears.

## Deferred from: quick-dev spec-clipboard-lfo-skip-incompatible-mods (2026-07-25)

- source_spec: `_bmad-output/implementation-artifacts/spec-clipboard-lfo-skip-incompatible-mods.md`
  summary: No footer / UX cue when LFO (or DCO) cross-paste intentionally leaves module-specific params unchanged.
  evidence: Blind Hunter; D-060 already specifies implicit UX — user manual EN/FR still to mention skip-vs-map for LFO mods.

- source_spec: `_bmad-output/implementation-artifacts/spec-clipboard-lfo-skip-incompatible-mods.md`
  summary: Cross-LFO unit tests cover Speed + Amplitude shared params, not the full shared choice/int set (waveform, lag, etc.).
  evidence: Blind Hunter; sufficient for this rule change; broaden only if displayName drift becomes a regression risk.

## Deferred from: quick-dev spec-v1-polish-controls-footer (2026-07-25)

- source_spec: `_bmad-output/implementation-artifacts/spec-v1-polish-controls-footer.md`
  summary: No automated unit/GUI regression tests for ComboBox open-flag fail path, ROM footer clear/hover, or Bank Utility Matrix-6 lock.
  evidence: Review Blind Hunter; verification remains Standalone + Matrix-Simulator manual and existing PatchManager ROM handler tests.

## Deferred from: code review of v1-2-device-connection-and-ports (2026-07-25)

- `DeviceMemoryLimits::resolve(kUnknown)` still returns Matrix-1000 limits — known Project Map warning; V1.2 locks Unknown via CompareLockBinder / outbound gate instead of neutral limits.
- Brief Header combo can show an older port than APVTS under rapid port-id changes before `callAsync` runs (`PluginEditor.cpp` property listener).
- Interactive MIDI From/To open failure leaves a short window with previous port closed and no Device Inquiry refresh until onChange restores the prior selection.
- `EditorOutboundGate::maySendEditorSysEx` is not wired into production `sendSysExWithDelay` / queue drain; Device Inquiry remains ungated by design; allow predicates still use `isEditorOutboundAllowed`.
- No MidiManager end-to-end inquiry-success fixture asserting Unknown → lock/footer (decoder/registry + Matrix-Simulator Unknown Device cover the live path).

## Deferred from: code review of v1-1-unsaved-navigation-consistency (2026-07-25)

- Mid-window dump abort uses full-buffer `memcmp` after APVTS sync: host automation or parameter SysEx echo during the settle window can false-abort a good dump (keeps edits + dirty + footer). Accepted V1 strictness vs silent overwrite; revisit only if false aborts show up in real use.
- `normalizeNameEncoding` is applied on dump/file load and `captureCleanSnapshot`, but PluginProcessor construction and DAW-restore still `captureSnapshot` without normalizing. Residual false-dirty risk outside the story’s dump/load navigation path.

## Deferred from: code review of u-10-release-gate-prod-audit-sign-off-and-d-062-d-063 (2026-07-24)

- ~~No CI / Release-workflow regression check that fails if `TestComponent` / `Source/GUI/Tests` re-enter Release artefacts — AC2 only required a local nm/strings (or equivalent) proof; consider a workflow hygiene step later.~~ **Resolved 2026-07-25 (v1-3)**: `Scripts/release/check_release_hygiene.py` scans Release artefacts for `TestComponent`; wired in `.github/workflows/release.yml` after build/tests and before codesign/pack.
- No compile-time sync between `JUCE_DEBUG` and CMake `$<CONFIG:Debug>` for sandbox sources — mismatched custom configs could theoretically link-fail; local Debug+Release presets already proven green.
- ~~`CONVENTIONS.md` E2E/GUI testing row still cites Standalone / `TestComponent` without a Debug-only qualifier — AC3 updated `project-context.md` only; align conventions SSOT in a later docs pass.~~ **Resolved 2026-07-25 (v1-3)**: E2E/GUI row states Debug-only harness excluded from Release (D-063).

## Accepted by U-10 aggregate UAT (2026-07-24)

The following Epic U residuals were reviewed as **known acceptable deltas** (cosmetic ±1–2 px / latent non-blocking) under story U-10 policy. See Consolidated Audit Report in `_bmad-output/implementation-artifacts/u-10-release-gate-prod-audit-sign-off-and-d-062-d-063.md`. No layout rewrite unless a later UAT finds a blocking defect.

- **U-5 Body / Shared** — column-chain leftover unchecked; Shared children sized to design width while parent uses `removeFromTop` (harmless at design width).
- **U-6 Patch Edit** — no runtime leftover assert on 5-column / vertical stack; DesignChecks @ 100 % only.
- **U-7 Matrix Mod** — fixed-width strips; non-preset scales can leave ±1–2 px Init vs Paste X drift.
- **U-8 Patch Manager** — undersized Internal/Computer strips clamp via `removeFromLeft` (harmless at design width).
- **U-9 Master Edit** — odd-scale strip sum slack/starve; parent module height vs BaseModulePanel interior ±1 px; unused `sectionHeaderWidth` (latent, not UAT-blocking).

Original review bullets below remain for history; status for U-10-owned residuals: **accepted 2026-07-24** (link: story U-10 audit report).

## Deferred from: code review of u-8-patch-manager-panels-layout-audit (2026-07-24)

- source_spec: `_bmad-output/implementation-artifacts/u-8-patch-manager-panels-layout-audit.md`
  summary: Internal/Computer Patches integer `removeFromLeft` strips clamp to remaining panel width when the panel is narrower than the design control sum, so later controls can shrink instead of keeping full design widths (old absolute `setBounds` overflowed). Harmless while Patch Manager modules are sized to design width; odd/undersized residual scrutiny remains on U-10 (same class as U-5 Shared overhang defer). **U-10 accepted delta (2026-07-24).**
  evidence: Blind Hunter + Edge Case Hunter; InternalPatchesPanel.cpp / ComputerPatchesPanel.cpp resized browser/storage rows.

## Deferred from: code review of u-7-matrix-modulation-panel-layout-audit (2026-07-24)

- source_spec: `_bmad-output/implementation-artifacts/u-7-matrix-modulation-panel-layout-audit.md`
  summary: Fixed-width integer column strip (no last-column remainder absorption) keeps Init @ 248 @ 100 % and aligns Init/Paste on all seven UI Scale presets; at some non-preset scales successive `scaledInt` can leave ±1–2 px leftover so bus Init X and section Paste X diverge. Residual odd-scale scrutiny already owned by U-10 (same class as U-5/U-6 leftover defers). **U-10 accepted delta (2026-07-24).**
  evidence: Blind Hunter + Edge Case Hunter; ModulationBusRowLayout.h, ModulationBusCell.cpp, MatrixModulationPanel::layoutSectionActionButtons.

## Deferred from: code review of u-6-patch-edit-panels-layout-audit (2026-07-24)

- source_spec: `_bmad-output/implementation-artifacts/u-6-patch-edit-panels-layout-audit.md`
  summary: No runtime assert that five-column last-column width ≈ scaled childW, or that PatchEditPanel vertical stack leftover ≈ 0 after four `removeFromTop` strips. DesignChecks cover @100 % identities; odd-scale residual scrutiny remains on U-10 (same class as U-5 Body leftover defer). **U-10 accepted delta (2026-07-24).**
  evidence: Blind Hunter + Edge Case Hunter; PatchEditFiveColumnLayout.h, PatchEditPanel.cpp.

## Deferred from: code review of u-5-body-shell-padding-separators-and-column-gaps (2026-07-23)

- source_spec: `_bmad-output/implementation-artifacts/u-5-body-shell-padding-separators-and-column-gaps.md`
  summary: SharedPanel still sizes Matrix Mod / Patch Manager children with scaled `dims_.width` while vertical placement uses `removeFromTop` rectangles. Harmless while BodyPanel sizes Shared to the matching design width; children can overhang only if Shared itself is truncated. **U-10 accepted delta (2026-07-24).**
  evidence: Blind Hunter + Edge Case Hunter; SharedPanel.cpp resized.

- source_spec: `_bmad-output/implementation-artifacts/u-5-body-shell-padding-separators-and-column-gaps.md`
  summary: BodyPanel `removeFromLeft` column chain leaves any remainder (or shortfall on Master Edit) unchecked — no `jassert` that leftover width ≈ 0 after the five segments. Compile-time DesignChecks cover design identity; runtime odd-scale leftover detection deferred (U-10 aggregate UAT). **U-10 accepted delta (2026-07-24).**
  evidence: Blind Hunter + Edge Case Hunter; BodyPanel.cpp resized.

## Deferred from: code review of u-4-footer-panel-layout-audit (2026-07-22)

- source_spec: `_bmad-output/implementation-artifacts/u-4-footer-panel-layout-audit.md`
  summary: Pathological footer paint bounds at extreme/invalid scales — identity min-width lacks jmax(1) floor; oversized padding can empty bounds; iconSize+padding can exceed leftover message width. Pre-U-4 math preserved; normal UI Scale presets unaffected.
  evidence: Blind Hunter + Edge Case Hunter; FooterPanel.cpp paint.

- source_spec: `_bmad-output/implementation-artifacts/u-4-footer-panel-layout-audit.md`
  summary: Icon-to-message spacing reuses chrome padding (iconSize + padding) with no dedicated gap Design token; Figma may want separate inset vs gap. Out of injection-only scope.
  evidence: Blind Hunter; FooterPanel.cpp paint.

## Deferred from: code review of u-3-header-right-cluster-layout-and-sign-off (2026-07-21)

- source_spec: `_bmad-output/implementation-artifacts/u-3-header-right-cluster-layout-and-sign-off.md`
  summary: Newly promoted Header packet Design tokens (left padding, gaps, label/combo/slider/peak widths) have no matching `static_assert`s in DesignChecks.h; existing header checks cover height/logo W/H/font/gap-after only.
  evidence: Blind Hunter; private HeaderPanel constants also lacked ÷4 asserts before promotion.

- source_spec: `_bmad-output/implementation-artifacts/u-3-header-right-cluster-layout-and-sign-off.md`
  summary: Other planning artifacts still cite HeaderPanel height 32 (e.g. u-0-figma-tokenize-and-design-reconciliation.md); AC4 only updated u-0-zone-dimension-tables.md and project-context.
  evidence: Blind Hunter; out of AC4 file list.

- source_spec: `_bmad-output/implementation-artifacts/u-3-header-right-cluster-layout-and-sign-off.md`
  summary: `logoPopupColumnWidth` is filled on HeaderPanelDimensions but HeaderLogoPopupMenu still hard-wires Design `kLogoPopupColumnWidth`; AC2 marked injection optional and Completion Notes deferred the wire-up.
  evidence: Blind Hunter + Acceptance Auditor; intentional optional deferral.

## Deferred from: code review of u-13-combobox-popup-infrastructure-dedup (2026-07-21)

- ~~ComboBox can leave `isPopupOpen_` stuck true if `ScrollablePopupMenu::show` / `MultiColumnPopupMenu::show` early-returns after `notifyPopupOpened` (null top-level, or empty items race).~~ **Resolved 2026-07-25 (v1 polish controls/footer)**: open notify moved after empty/null guards inside both `::show` (Hierarchical order); `ComboBox::showPopupAsynchronously` no longer notifies before `show`.
  evidence: Blind Hunter + Edge Case Hunter; `ComboBox.cpp` `showPopupAsynchronously`.

## Deferred from: code review of 9-2-unsaved-edit-confirmation-dialog (2026-07-20)

- source_spec: `_bmad-output/implementation-artifacts/9-2-unsaved-edit-confirmation-dialog.md`
  summary: ~~After dirty Continue on internal navigation, `loadCurrentPatchFromDevice` can bail (`!isDeviceDumpAvailable` / failed dump) leaving advanced bank/patch coordinates with edits already accepted for discard.~~ **Resolved 2026-07-25 (v1-1)**: pending-load stash + coordinate rollback on dump unavailable / empty / wrong-size; dirty kept; no `onPatchLoaded`.
  evidence: Blind Hunter; pre-existing async dump pattern; FR-51 gate correctly runs before request.

- source_spec: `_bmad-output/implementation-artifacts/9-2-unsaved-edit-confirmation-dialog.md`
  summary: ~~Edits made between dirty Continue and async dump completion are overwritten then `captureCleanSnapshot` marks clean with no second FR-51 warning.~~ **Resolved 2026-07-25 (v1-1)**: pending-load generation + buffer-at-request compare; mid-window edits abort apply, restore coords, keep dirty, footer.
  evidence: Blind Hunter; pre-existing settle window on async dump callback.

- source_spec: `_bmad-output/implementation-artifacts/9-2-unsaved-edit-confirmation-dialog.md`
  summary: PASTE still uses `pushPatchModelToApvtsWithSuppress(..., nullptr)` for PatchNameSyncer; post-paste dirty compare can skew vs INIT path that now syncs the name.
  evidence: Blind Hunter; pre-existing PASTE path; AC intentionally excludes FR-51 modal for PASTE.

- source_spec: `_bmad-output/implementation-artifacts/9-2-unsaved-edit-confirmation-dialog.md`
  summary: ~~Name-reconciliation Cancel after user already Continued through dirty + Mutator history Discard cannot restore history — Cancel only reverts combo selection.~~ **Resolved 2026-07-25 (v1-1)**: Discard/Export no longer call `resetSessionForPatchLoad` in the gate; history clears only via `onPatchLoaded` after successful apply.
  evidence: Blind Hunter; pre-existing composition of sequential gates.

- source_spec: `_bmad-output/implementation-artifacts/9-2-unsaved-edit-confirmation-dialog.md`
  summary: ~~`lastCommittedComputerPatchesSelectedId_` defaults to 0 until a successful load/reset path remembers an id; early Cancel revert target can be wrong.~~ **Resolved 2026-07-25 (v1-1)**: seed committed id from current non-sentinel selection before OPEN / Prev-Next navigation.
  evidence: Blind Hunter; bootstrap edge.

- source_spec: `_bmad-output/implementation-artifacts/9-2-unsaved-edit-confirmation-dialog.md`
  summary: INIT always captures a clean snapshot after `initFullPatch`, including fallback/invalid template landing.
  evidence: Blind Hunter; intentional for FR-51 smoke after INIT (completion notes). Out of v1-1 scope.

- source_spec: `_bmad-output/implementation-artifacts/9-2-unsaved-edit-confirmation-dialog.md`
  summary: Broader automated coverage still missing for OPEN Cancel folder+selection restore, NumberBox+chained dirty gate, and never-warn vs real modal (beyond handler-level Prev/Next Cancel regression test).
  evidence: Blind Hunter; T5 left some paths as optional / smoke. OPEN Cancel covered in 9.2 / v1-1 handler tests; NumberBox+modal still manual/smoke.

## Deferred from: code review of spec-8-4-virtual-instrument-registration-and-bus-layout (2026-07-19)

- source_spec: `_bmad-output/implementation-artifacts/spec-8-4-virtual-instrument-registration-and-bus-layout.md`
  summary: ~~When device is detected but type is Unknown (neither Matrix-1000 nor Matrix-6/6R), gray the entire GUI and show an explicit footer — product rule confirmed in 8-4 review; out of 8-4 scope (MASTER keep allowlist; footer wording patched in 8-4).~~ **Resolved 2026-07-25 (v1-2)**: family+unknown member reaches `deviceDetected` + `deviceType=Unknown`; `CompareLockBinder` / outbound gate lock supported-device-only; distinct footer `kUnsupportedMatrixDeviceFooter`.
  evidence: Decision 1 resolution 2026-07-19; follow-up story / correct course.

- source_spec: `_bmad-output/implementation-artifacts/spec-8-4-virtual-instrument-registration-and-bus-layout.md`
  summary: `MidiManager::updateDeviceStatus` sets `deviceDetected` before `deviceType`, so new MASTER/Audio From listeners can briefly see detected+stale/unknown type.
  evidence: Blind Hunter; pre-existing property order, surfaced by 8.4 listeners.

- source_spec: `_bmad-output/implementation-artifacts/spec-8-4-virtual-instrument-registration-and-bus-layout.md`
  summary: ~~`MidiManager::sendMaster` still gates only on editor outbound (`deviceDetected`), not `isMasterEditAllowed`; defense-in-depth hardening if new call sites appear.~~ **Resolved 2026-07-25 (v1-3)**: `sendMaster` fail-closes via `isMasterEditOutboundAllowed` / `MasterEditGate`; unit coverage for Matrix-6 / Unknown / undetected.
  evidence: Blind Hunter; current INIT/parameter paths already gated upstream.

- source_spec: `_bmad-output/implementation-artifacts/spec-8-4-virtual-instrument-registration-and-bus-layout.md`
  summary: Unit coverage stops at pure helpers (`preferredAudioFromKind` / `isMasterEditAllowed`); no harness that `PluginProcessor` skips mapper/dispatch or INIT no-ops on M-6.
  evidence: Acceptance Auditor + Blind Hunter; helper tests lock the shared predicate.

## Deferred from: code review of spec-9-1-dirtypatchtracker (2026-07-19)

- source_spec: `_bmad-output/implementation-artifacts/spec-9-1-dirtypatchtracker.md`
  summary: ~~PatchModel name encode (`setName` 7-bit ASCII) vs decode (`getName` 6-bit Matrix) asymmetry can mark dirty after hardware dump capture then APVTS name round-trip with no user rename; Story 9.2 capture/sync paths should account for this.~~ **Resolved 2026-07-25 (v1-1)**: `PatchModel::normalizeNameEncoding` on dump/file load and before `captureCleanSnapshot`; unit regression locks 6-bit → clean APVTS round-trip.
  evidence: Blind Hunter; `PatchModel.cpp` setName/getName; pre-existing, not introduced by DirtyPatchTracker.

## Deferred from: quick-dev review of spec-9-1-dirtypatchtracker (2026-07-19)

- source_spec: `_bmad-output/implementation-artifacts/spec-9-1-dirtypatchtracker.md`
  summary: PluginProcessor owns DirtyPatchTracker but exposes no getter and has no production captureSnapshot call sites (load/save/STORE); Story 9.2 must wire access and snapshot refresh.
  evidence: Blind Hunter + Edge Case Hunter; intentional 9.1 Ask First default left capture out of handlers; member only constructed in PluginProcessor init list.

- source_spec: `_bmad-output/implementation-artifacts/spec-9-1-dirtypatchtracker.md`
  summary: Story 9.2 capture sites must sync APVTS→PatchModel (mapper + PatchNameSyncer) before captureSnapshot so the baseline matches live UI state.
  evidence: Edge Case Hunter; captureSnapshot stores whatever bytes are in the model; unsynced capture can skew dirty checks until handlers define sync-before-capture.

## Deferred from: code review of spec-compare-copy-button-red-active-text (2026-07-17)

- source_spec: `_bmad-output/implementation-artifacts/spec-compare-copy-button-red-active-text.md`
  summary: Story 7-5 still tells agents not to use `setToggleState` because Button `textOn` was not red — that guidance is now obsolete.
  evidence: `7-5-bank-utility-ui-wiring.md` Design Notes; Bank Utility still uses look overrides instead of toggle-on.

## Deferred from: quick-dev plugin-patch-push-edit-buffer hardware UAT (2026-07-17)

- source_spec: `_bmad-output/implementation-artifacts/spec-plugin-patch-push-edit-buffer.md`
  summary: ~~Matrix-1000 0x0D inaudible~~ **Resolved 2026-07-17**: missing literal header byte `0` after opcode `0DH` (encoder emitted 274-byte message). Docs + encoder restored to `F0 10 06 0D 00 …` (275 bytes). Hardware re-UAT still recommended for audible Mutate/INIT on RAM and ROM.
  evidence: Official Oberheim manual + Edit Buffer Lab round-trip (WITH `0` ≈ name-only diffs; WITHOUT `0` = massive misalignment). Code fix: `SysExEncoder::buildHeader` + `kPatchToEditBufferMessageLength` + `sendFullPatchForAudition` → 0x0D on M-1000.

## Deferred from: code review of spec-plugin-patch-push-edit-buffer (2026-07-17)

- source_spec: `_bmad-output/implementation-artifacts/spec-plugin-patch-push-edit-buffer.md`
  summary: No dedicated MidiManager unit test for sendFullPatchForAudition (null / branch / enqueue).
  evidence: Blind Hunter; behavior covered only via PatchManager and Mutator harnesses.

- source_spec: `_bmad-output/implementation-artifacts/spec-plugin-patch-push-edit-buffer.md`
  summary: History audition / Compare Mutator paths still assert combined full-patch SysEx counts, not exclusive 0x0D vs 0x01 opcodes.
  evidence: Blind Hunter; Mutate and Retry now have opcode asserts; remaining push paths still use broadened countPatchSysExMessages.

## Deferred from: quick-dev plugin-patch-push-edit-buffer (2026-07-17)

- source_spec: none
  summary: DirtyPatchTracker + unsaved-edit confirmation modal (FR-51 / Epic 9) before navigating away from an edited patch.
  evidence: Split from edit-buffer SysEx alignment quick-dev; Guillaume chose [S] — keep modal for Epic 9 (`9-1-dirtypatchtracker`, `9-2-unsaved-edit-confirmation-dialog`).

## Deferred from: code review of spec-computer-patches-open-auto-select-first (2026-07-17)

- source_spec: `_bmad-output/implementation-artifacts/spec-computer-patches-open-auto-select-first.md`
  summary: Mutator history gate Cancel after Open can leave combo on first file while the edit buffer stays on the previous patch (same pattern as manual combo load cancel; Open makes it more frequent).
  evidence: Blind Hunter; Open commits scan + `kSelectPatchFile=1` before `confirmPatchContextChange`; cancel aborts load without reverting selection.

## Deferred from: quick-dev computer-patches-open-auto-select-first (2026-07-17)

- source_spec: none (clarified during quick-dev; implement with Epic 9)
  summary: FR-51 / DirtyPatchTracker — before OPEN (and other navigation that would replace the current patch), if the loaded patch has unsaved edits, show a two-button modal: **Cancel** (abort Open, keep editing so the user can STORE or SAVE AS themselves) and **Continue** (discard unsaved edits and proceed with Open). No in-modal save path.
  evidence: Product intent captured in quick-dev; DirtyPatchTracker and FR-51 dialog are still backlog (`9-1-dirtypatchtracker`, `9-2-unsaved-edit-confirmation-dialog`). OPEN today has no dirty gate; auto-load-after-Open increases the need for this guard. Prefer Cancel/Continue over a Save-first modal so STORE vs SAVE AS stays user-chosen.

## Deferred from: code review of 6-17-history-prev-next-and-compact-action-labels (2026-07-16)

- **No end-to-end test for APVTS stamp → handler → selection → audition chain** — Unit tests cover `advanceHistorySelection` on the engine and handler→mock engine only; the thin-panel property stamp path is unverified end-to-end.
- **Panel `countFlatHistoryEntries` duplicates engine flat-order logic via APVTS mirrors** — GUI counts retry-list mirrors while Core walks `MutationHistoryStore`; drift risk if mirror format changes. Normal `syncHistoryUiProperties` path keeps them aligned today.

## Deferred from: code review of 1-6-wire-patch-name-display (2026-07-16)

- **Public `getPatchNameDisplay()` remains an APVTS bypass** — Pre-existing accessor; story 1-6 kept the widget API unchanged. External `setPatchName` can paint a name that drifts from `apvts.state["patchEditPatchName"]` until the next listener fire. Revisit if a future editor or handler starts calling it.

## Deferred from: spec-patch-mutator-mutate-disabled-without-module-toggle (2026-07-15)

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-mutate-disabled-without-module-toggle.md`
  summary: Dual “any module enabled” encodings (MutationRecipe fields vs ValueTree toggle-ID table) can drift if a toggle is added later.
  evidence: Blind Hunter; `hasAnyModuleEnabled` and `anyRecipeModuleToggleEnabled` / `buildRecipeFromApvts` are parallel maps.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-mutate-disabled-without-module-toggle.md`
  summary: No automated test covers PluginProcessor live listener path that refreshes mirrors on toggle change.
  evidence: Blind Hunter; unit tests call `refreshActionEnabledMirrors` directly after setProperty.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-mutate-disabled-without-module-toggle.md`
  summary: PatchMutatorPanel keeps a separate hand-maintained recipe enable-ID list for UI hydration.
  evidence: Blind Hunter; pre-existing fork with Core toggle tables; out of oneshot scope to consolidate.

## Deferred from: spec-patch-mutator-amount-random-range (2026-07-15)

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-amount-random-range.md`
  summary: Fresh-session Mutate can still feel dead when all module toggles remain off (defaults false) even with Amount/Random > 0.
  evidence: Spec Ask First / Design Notes; A×R no-op is fixed but algorithm “no change” path with empty module mask remains a smoke-test ambiguity. *Superseded for UX greying by spec-patch-mutator-mutate-disabled-without-module-toggle (button disabled + engine footer); leave until next persistence cleanup if still relevant.*


- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-amount-random-range.md`
  summary: Deduplicate Amount/Random floor/default literals across descriptors, session init, panel hydrate, and buildRecipeFromApvts.
  evidence: Review found parallel 50/25/1 constants; not user-facing, risk only on future default retune.

## Deferred from: code review of 6-16-matrix-mod-recipe-toggle (2026-07-15)

- **No automated History-row geometry assert for MM under L2 / CLEAR=20** — AC #4 and manual UAT cover scale 1.0 and non-1.0; no registry/unit lock on EXPORT trailing edge or MM×L2 x-alignment. Same manual-smoke class as prior Mutator layout stories.

## Deferred from: story 6-16-matrix-mod-recipe-toggle (2026-07-15)

- **FR-30 / planning copy still says “ten module toggles”** — recipe now has eleven enables (D1…L2 + MM). Epics FR prose and older stories keep “ten” as historical Patch Edit count; MM is the separate Matrix Mod gate. Update consumer-facing FR wording when next editing PRD/addendum for Epic 6.

## Deferred from: spec-patch-mutator-history-control-widths (2026-07-15)

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-history-control-widths.md`
  summary: Manual UAT — verify closed HISTORY shows full `M99-R99` at 100% and 150% scale without clipping.
  evidence: Closed ComboBox text area is ~41 px after padding/chevron; no automated GUI metric test ties worst-case label to `kPatchMutatorHistory = 56`.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-history-control-widths.md`
  summary: Manual UAT — verify MUTATE/RETRY/COMPARE labels remain readable at 48 px button width.
  evidence: Buttons paint centered with no ellipsis; COMPARE (7 chars) is the tightest case after 52→48 shrink.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-history-control-widths.md`
  summary: Reconcile planning docs (epics 6-14/6-15, U-12) still documenting 48 px History/slider width.
  evidence: Atoms now use 56 px; stale AC text risks partial revert on future stories.

## Deferred from: code review of 6-15-history-combobox-popup-ux (2026-07-15)

- ~~**Duplicated `CustomScrollBar` in `HierarchicalPopupMenu`** — ~copy of `ScrollablePopupMenu` thumb/timer/wheel math; story 6-15 allowed reuse patterns without full U-13 DRY. Extract shared helper in U-13.~~ **Cleared by U-13** (`PopupMenuCustomScrollBar` + scroll models).
- ~~**Permanent 20 Hz scrollbar timers while hierarchical popup is open** — inherited from ScrollablePopupMenu CustomScrollBar pattern (`startTimerHz(20)` + idle `repaint`); fix with shared scroll infra in U-13.~~ **Cleared by U-13** (event-driven repaint only).

## Deferred from: code review of 6-14-patch-mutator-hierarchical-history-combobox (2026-07-15)

- **Fragile Mxx/Rxx index parsing via `substring(1,3)`** (`PatchMutatorPanel.cpp`) — inherited from 6.6 dual-combo label convention; bogus labels → index 0.
- **No automated tests for hierarchy cache / defer / `onBeforeShowPopup`** — panel UI risk paths covered only by manual smoke.
- **No automated layout/scale assertions for COMPARE alignment** — AC #5 relies on manual smoke at 0.5 / 1.0 / 1.5.
- **Unguarded short/non-numeric mutate labels** (`PatchMutatorPanel.cpp:645`) — same pre-existing label contract as above.

## Deferred from: code review of 7-8-header-footer-shell-and-persistence (2026-07-14)

- **Hand-maintained parameter ID collection in `SessionPersistencePolicy`** — new modules/parameters must be added manually to `collectManagedParameterIds()`; omission silently defeats strip policy.
- **`RampPortamentoPanel` hard-coded `getParameterCellAt(7)`** — LEGATO PORTA graying tied to layout index; reordering cells breaks rule without compile failure.
- **`GrayedControlHelper` direct APVTS footer writes** — uses `setProperty` on `uiMessageText`/`uiMessageSeverity` instead of `ExceptionPropagator`; spec allows thin wrapper.

## Deferred from: code review of 7-6-internal-patches-panel-wiring (2026-07-14)

- ~~**Footer ROM persiste après retour banque RAM**~~ **Resolved 2026-07-25 (v1 polish controls/footer)**: exact-string clear of ROM footer when `romPasteStoreBlocked_` goes true→false.
- ~~**Hover footer absent si curseur déjà sur bouton au blocage ROM**~~ **Resolved 2026-07-25 (v1 polish controls/footer)**: after rewire while blocked, show ROM footer if Init/Paste/Store already `isMouseOver()`.
- **Smoke manuel Standalone non documenté (AC9 partiel)** — UAT reporté à Guillaume par politique projet ; pas de preuve dans le diff de revue.

## Deferred from: code review of 7-11-internal-patches-init-sysex-device-rules (2026-07-14)

- **Footer succès INIT affiché avant envoi MIDI / si `midiManager_` null** — pattern hérité de 7.3 (INIT éditeur-only) ; pas de requirement spec sur cohérence footer ↔ livraison MIDI.
- **Pas de garde « port MIDI ouvert » avant enqueue** — identique à `sendPatch` ; erreur via `lastError` au moment de l’envoi effectif.
- **`apvtsToBuffer()` sans vérification de succès** — même pattern que PASTE/STORE.
- **`getCurrentPatch()` non clampé** — même pattern que PASTE/STORE ; M-6 borné 0–99.
- **Type device `kUnknown` → branche M-1000 / 0x0D** — résolution DeviceMemoryLimits pré-existante, hors périmètre 7-11.
- **Pas de tests unitaires `MidiManager::sendPatchToEditBuffer`** — AC5 couvre encodeur + handler ; pas exigé par la story.

## Deferred from: code review of 11-3-ci-build-time-optimizations (2026-07-11)

- **PRs de fork — l’auteur ne peut pas ajouter `ci-full` sans maintainer** — limitation GitHub Actions ; documenter dans CONTRIBUTING si friction observée.

## Deferred from: code review of 11-2-cd-release-pipeline (2026-07-11)

- **Dry-run E2E (push tag réel + assets GitHub Release)** — post-merge maintainer après configuration des secrets ; déjà documenté story L187.
- **Windows Authenticode non signé pour v1** — choix spec ; documenté CONTRIBUTING comme acceptable MIT open-source.
- **macOS arm64-only en CI (pas universal)** — décision spec Dev Notes ; universal optionnel documenté.
- **Pins pytest/PyYAML sans borne supérieure** — dette mineure reproductibilité locale/CI.

## Deferred from: code review of 7-5-bank-utility-ui-wiring (2026-07-11)

- **Matrix-6/6R Bank Utility graying UAT not exercised** — smoke item 6 blocked (no M-6 hardware); device-type simulation harness backlog (UAT grid Appendix C); code path review only in Story 7-5 review.
- **AC #9 build/test gate not re-run in code review** — Dev Agent Record claims 1830 tests + macOS targets green; no independent re-verification in review session.

## Deferred from: code review of 11-1-ci-multi-platform-build-and-tests (2026-07-11)

- **Hard-coded test_binary paths per OS in CI workflow** — fragile if CMake output layout changes; consider ctest or artifact discovery later.
- **COPY_* CACHE FORCE may retain stale copy flags on local reconfigure** — CI mitigated by fresh checkout each run.
- **CONTRIBUTING.md entire TOC uses claude.ai URLs instead of in-repo anchors** — pre-existing; fix holistically in doc hygiene pass.
- **Thread::sleep(50) in MidiManagerTests queue timing** — pre-existing potential CI flakiness under load.
- **Personal Dropbox artefact paths in CMakeUserPresets.json** — copy disabled in CI; optional ci-* presets deferred.
- **MIDI-dependent tests skip silently when no output device** — acceptable for AC3 headless CI; mock coverage deferred.

- **Panel M change leaves R list stale until processor hook** — resolved 1+2: sync reads APVTS in 6.6; 7.4 processor calls `syncHistoryUiProperties` on `kSelectedM` change for full panel UX.

## Deferred from: code review of 7-4-mutatoractionhandler (2026-06-20)

- **`actionHooks.onPatchLoaded` copied before assignment** — pre-existing Epic 6 ordering; patch load clear works via PatchManagerActionHandler until engine needs the hook directly.

## Deferred from: code review of 6-13-clear-history-on-patch-load (2026-06-20)

- **Hook contract for future synth load not documented in source** — AC #9 contract in story markdown; optional comment on `ActionExecutionHooks::onPatchLoaded` when `requestCurrentPatch` is implemented.
- **Handler hook test covers select-file load only** — Prev/Next navigation uses same `applyLoadedPatchToApvtsAndSynth`; redundant path test unless regression fear.
- **No PluginProcessor integration test for engine + hook wiring** — story AC #12 limits scope to engine/handler unit tests; composition-root test deferred to 7.4.
- **`getCurrentPatchNumberForMutator()` silent fallback to patch 0** — pre-existing APVTS default pattern; clamp/validate when 7.4 wires mutator actions.

## Deferred from: code review of 6-12-recipe-persistence-and-action-enabled-states (2026-06-20)

- **Recipe property IDs maintained in three separate lists** — add/remove toggle requires edits in `MutatorSessionPersistence.h`, `PatchMutatorPanel::isRecipeProperty`, and toggle binding table.
- **Engine `MutationHistoryStore` not cleared on session load** — story scopes engine wiring to 7.4; APVTS ephemeral reset sufficient for UI until then.
- **Four processor methods are one-line passthroughs** — mirrors clipboard init pattern; thin wrappers acceptable.
- **`recipeHydrating_` not RAII-guarded** — no exceptions in JUCE UI callbacks.
- **No amount/random clamp on session load** — engine clamps at mutation time; matches other APVTS property init patterns.
- **Combo selection change does not refresh enabled mirrors** — requires engine/handler wiring (7.4) on `kSelectedM` change.
- **Limit tests inject store directly** — spec test table allows harness store manipulation for mirror predicate matrix.

## Deferred from: code review of 6-11-mutator-export-layout (2026-06-20)

- **Partial export fail-fast leaves prior files on disk** — v1 spec explicit; no rollback required.
- **Re-export overwrite behavior undefined** — out of scope v1; document or handle in future export UX story.
- **Non-writable folder tests use nonexistent path, not read-only directory** — test hardening; CI may not cover permission-denied scenario.
- **No tests for mid-batch export failure / orphan output** — v1 fail-fast policy documented in spec; add when rollback story exists.

## Deferred from: code review of 6-10-historydefrag-on-limit (2026-06-20)

- **`HistoryDefragService::defrag()` exceeds 15-line function limit** — architecture guideline; extract compaction loop helper when touching service again.
- **`PatchMutatorEngine::defragHistory()` exceeds 15-line function limit** — same architecture guideline; optional refactor in focused cleanup pass.

## Deferred from: code review of 6-9-delete-and-clear-history (2026-06-20)

- **`deleteSelected()` / `resolveSelectionAfterDelete()` exceed ≤15-line convention** — helpers are correct and match spec algorithm; optional refactor in a focused cleanup pass.

## Deferred from: code review of 6-8-compare-mode (2026-06-20)

- **DELETE/CLEAR during compare may restore stale `compareSavedM_`/`compareSavedR_`** — story 6.9 compare exit side-effects.
- **Initial snapshot not reset on patch load / history clear without mutate** — story 6.13 clear-on-patch-load.
- **`pushResultToEditorAndSynth` return value ignored on compare enter/exit** — pre-existing 6.4–6.7 pattern; revisit if handler propagates push failures.

## Deferred from: code review of 6-7-history-selection-audition-with-debounce (2026-06-20)

- **Debouncer pending callback dropped on destruction** — `stopTimer()` only; acceptable for 6.7; 7.4 handler lifetime must outlive debouncer or add explicit cancel.
- **No mutate→audition integration test for AC #2 double-SysEx scenario** — idempotent memcmp path tested via double `auditionSelectedHistoryEntry()`; full mutate→sync→audition chain deferred.
- **No message-thread assertion on `ComboboxPatchSendDebouncer::schedule()`** — matches existing `MatrixModSysExCoalesceTimer` pattern; document at 7.4 wiring.

## Deferred from: code review of 6-6-history-m-and-r-ui-properties (2026-06-20)

- **`auditionSelectedHistoryEntry()` stub** — story 6.7 audition SysEx.
- **`syncHistoryUiProperties` after delete/clear/export** — stories 6.9+ store mutations.
- **Preset migration `patchMutatorHistory` → M/R properties** — brownfield, no migration story.
- **`resolveAuditionBuffer` non-const side-effect** — intentional AC#4 apply-before-resolve.
- **`applySelectionFromApvts` skip when `kSelectedM` absent** — preserves `setAuditionSelection()` for 6.4/6.5 tests.
- **`resolveSelectedRootIndex()` caller contract** — safe today because `retry()` calls apply first.
- **`kCompareActive` not consumed by panel** — story 6.8 COMPARE toggle.
- **No GUI panel tests** — AC#8 scoped to engine unit tests.
- **DAW reload APVTS mirrors vs empty session store** — story 6.13 clear-on-patch-load.
- **Corrupt/malformed APVTS / non-integer property coercion** — host edge cases.
- **24px combo width clipping** — minor UX.
- **Descriptor registration vs manual panel combo wiring** — existing project pattern.

## Deferred from: code review of 6-4-mutate-action (2026-06-19)

- **Constructor 7 parameters vs Clean Code 3-param limit** (`PatchMutatorEngine.h:33-39`) — Guillaume chose option 3: defer `PatchMutatorEngineDeps` struct to Story 7.4 composition root; keep seven DI args for 6.4 as spec-written.
- **`mutate()` exceeds 20-line orchestration limit** (`PatchMutatorEngine.cpp:72-137`) — style debt; extract early-return helpers in a focused cleanup pass.
- **No documented thread-safety contract on `PatchMutatorEngine`** (`PatchMutatorEngine.h`) — message-thread assumption matches other Core services; document when Story 7.4 wires processor.
- **Silent fallback to live patch when history entry missing** (`PatchMutatorEngine.cpp:241-255`) — defensive; store should stay internally consistent.
- **Negative `getCurrentPatchNumber()` not rejected before `sendPatch`** (`PatchMutatorEngine.cpp:263-268`) — `jlimit(0,255)` clamps; callback contract owned by 7.4 composition root.

## Deferred from: code review of 6-2-mutationhistorystore-two-level-mr (2026-06-19)

- **Full `Matrix-Control_Tests` target not green** — pre-existing `MidiActivityTrackerTests` failure; all 11 `MutationHistoryStore` tests pass.
- **`getInitialSnapshot()` without prior `setInitialSnapshot()`** — returns zero buffer; AC #3 provides `hasInitialSnapshot()` guard; caller contract (Story 6.8 engine).
- **`isRetrySlotsFull` / `peekNextRetryIndex` on missing root** — indistinguishable from “capacity available”; engine must call `hasRoot` first (Story 6.4+).
- **`rootCount()` O(n) and redundant sort** — acceptable v1; worst-case ~2.7 MB RAM per architecture note.

## Deferred from: code review of 6-1-mutationalgorithm-specification (2026-06-19)

- **Golden vectors lack pinned output hex** — Owner sign-off 2026-06-19 accepted deferred pinning to Story 6.4 per GV-04 note; AC #7 literal "expected output" not fully met in appendix.

## Deferred from: Internal Patches startup policy (2026-06-19)

- **Persist bank/patch in session for display-only recall** — rejected; hardware state unknown at launch. Policy: always 0/00. See `spec-internal-patch-startup-coordinates.md`.
- **Read current bank/patch from Matrix-1000 at startup** — no reliable SysEx query in v1; would supersede 0/00 policy if added later.

## Deferred from: code review of 4-6-previous-and-next-file-navigation (2026-06-19)

- **Appel direct `handleLoadSelectedPatchFile` quand id inchangé (N==1)** (`PatchManagerActionHandler.cpp:165-166`) — déviation documentée du libellé tâche « advance only » ; justifiée par le no-op JUCE `setProperty` ; conforme AC#3.
- **Pas de test GUI `syncSelectionFromApvts`** (`ComputerPatchesPanel.cpp:67-68`) — aucune infra de tests panel ; listener vérifié par inspection.
- **Tests simulent le dispatch `kSelectPatchFile` manuellement** (`PatchManagerActionHandlerTests.cpp:289-301`) — limitation harness ; chaîne PluginProcessor non intégrée.
- **`advanceComputerPatchesSelection` ~19 lignes** (`PatchManagerActionHandler.cpp:376-394`) — dette style mineure (limite 15 lignes).
- **Section BMad titres dans `CONVENTIONS.md`** — hors périmètre story 4.6 ; à committer séparément.

## Deferred from: code review of 4-5-import-name-reconciliation (2026-06-19)

- **CONVENTIONS.md hors périmètre story** — section BMad titres agents ajoutée dans le même diff que le chargement 4.5 ; à committer séparément.
- **`AlertWindow::runModalLoop` synchrone** (`PluginEditor.cpp:132`) — premier modal du projet ; migration async JUCE 8 reportée.

## Deferred from: code review of 4-4-save-with-filename-injection (2026-06-19)

- **SAVE AS outside library folder** — no combobox entry after save; spec v1 allows any folder, rescan stays on library path only.
- **Long disk filename vs 8-char injected name** — intentional FR-28 / D-025 divergence per story dev notes.
- **`ensureSyxExtension` unused** — dead API added but not wired; low impact cleanup.


- **Lacunes de tests optionnelles** — remplacement explicite AC #7, intégration `PluginProcessor::setStateInformation`, assert `kScanRevision` plus fort ; conformité AC validée par l'auditeur.
- **Chemins absolus non portables entre machines** (`PatchManagerActionHandler.cpp:241`) — v1 AD-7 / D-010 ; chemins relatifs ou bookmarks hors périmètre.
- **Scan synchrone sur `setStateInformation`** (`PluginProcessor.cpp:565`) — acceptable v1 selon story 4.1.
- **`handleOpenPatchFolder` 16 lignes** (`PatchManagerActionHandler.cpp:230`) — dette style mineure (limite 15 lignes story).
- **Chemin whitespace / relatif / XML corrompu non validé** (`PatchManagerActionHandler.cpp:249`) — entrées manuelles improbables.
- **`kScanRevision` via `getMillisecondCounterHiRes()`** — pattern hérité story 4.2 ; collision théorique même tick.

## Deferred from: code review of 4-2-combobox-sentinel-states (2026-06-19)

- **`kScanRevision` uses `getMillisecondCounterHiRes()`** (`PatchManagerActionHandler.cpp:242-245`) — theoretical duplicate if two scans complete in same tick; same project-wide APVTS trigger pattern as 5-3.
- **Lexicographic case-sensitive file sort** (`PatchFileService.cpp:80`) — inherited from Story 4.1; combobox display order follows service cache.

## Deferred from: code review of 4-1-patchfileservice-folder-scan (2026-06-19)

- **Scan synchrone bloque le thread UI** (`PatchFileService.cpp:15-47`) — acceptable v1 selon dev notes story 4.1 ; worker async si bibliothèques volumineuses.
- **Logs `decodePatchSysEx: validation failed` à chaque fichier rejeté** (`SysExDecoder.cpp`) — message pré-existant du décodeur ; bruit en scan bulk.
- **Pas de test d'intégration handler OPEN** (`PatchManagerActionHandlerTests.cpp:198`) — optionnel selon spec story 4.1.
- **`loadFileAsData` sans plafond de taille** (`PatchFileService.cpp:67-72`) — hors périmètre v1.

## Deferred from: code review of 7-3b-bank-utility-unlock-semantics-and-id-rename (2026-06-19)

- **MIDI dupliqué sur navigation prev/next** (`PatchManagerActionHandler.cpp:242-258`, `PluginProcessor.cpp:1220-1221`) — `applyPatchCoordinates` + listener `handlePatchNumberChange` enchaînent deux `syncSelection` ; dette 7.3.
- **Renommage APVTS lock sans migration** (`PluginIDs.h`, `MidiManager.cpp`) — `patchManagerBankLock` → `patchManagerBanksLocked` ; accepté pre-release par spec 7-3b.
- **`kSelectedBank` désynchronisé après navigation cross-banque** (`PatchManagerActionHandler.cpp:242-252`) — UI Bank Utility ; reporté story 7.5.
- **Flags suppress SysEx sans RAII** (`PatchManagerActionHandler.cpp:139-190`) — pattern transversal 7.1/7.2.

## Deferred from: code review of 7-2-moduleactionhandler-i-c-p-and-matrix-mod-init (2026-06-18)

- **AC #8 — suppress-hook order not asserted in unit tests** (`ModuleActionHandlerTests.cpp`) — flags recorded but never expected true-during-push / false-before-dispatch.
- **AC #8 — PATCH init not exercised through `ModuleActionHandler`** (`ModuleActionHandlerTests.cpp:96`) — harness passes `patchModuleInitService_ == nullptr`; init routing, suppress wrap, footer untested at handler level.
- **AC #8 — matrix-mod paste test incomplete vs spec matrix** (`ModuleActionHandlerTests.cpp:215`) — 10× `dispatchBus` only; no `pushBusToApvts` count or bytes 0–103 isolation.
- **AC #8 — footer propagation untested at handler level** — `PatchModuleInitServiceTests` covers service fallback; no APVTS `uiMessageText` / `uiMessageSeverity` assertion via handler.
- **AC #6 partial — `endsWith("Paste")` for module paste routing** (`ModuleActionHandler.cpp:182`) — mitigated by explicit matrix-mod branch + `patchModuleKindFromWidgetId`; replace with explicit Paste ID set when Copy/Paste split debt is addressed.
- **Suppress flags without RAII on exception path** (`ModuleActionHandler.cpp`) — same pattern as 7.1 master/matrix init; ScopeGuard transversal (see 7-1 deferred item).
- **`dispatchModule` rebuilds descriptor vectors each call** (`PatchParameterSysExDispatcher.cpp:19`) — perf micro-optimization; cache or reuse filtered module descriptors if profiling warrants.
- **`PatchModuleInitService.h` includes `ClipboardService.h` for `PatchModuleKind`** (`PatchModuleInitService.h:9`) — Init subsystem coupled to clipboard Services header; extract shared type when layering is next touched.
- **`moduleGroupIdFromPatchModuleKind` on init service used by paste handler** (`ModuleActionHandler.cpp:195`) — paste routing depends on `PatchModuleInitService` for non-init mapping; relocate with Copy/Paste split debt.

## Deferred from: code review of 7-1-actiondispatcher-and-handler-interfaces (2026-06-18)

- **Flags SysEx suppress sans RAII en cas d’exception** (`ModuleActionHandler.cpp:59-68,90-96`) — code migré depuis `PluginProcessor` ; même dette que stories 3-3/3-4 ; ScopeGuard transversal recommandé.
- **Sélection banque sans clamp aux limites device** (`PatchManagerActionHandler.cpp:70-73`) — comportement brownfield préservé ; clamp via `jlimit(limits.minBankNumber(), limits.maxBankNumber(), bankIndex)` si UI Matrix-1000 expose des boutons hors plage.
- **`ModuleActionHandler::handleAction` enchaîne trois sous-handlers** (`ModuleActionHandler.cpp:33-37`) — pattern hérité du processor ; dispatch ciblé par `propertyId` en 7.2+.
- **Garde `endsWith("Init")` trop large pour stubs patch Init** (`ModuleActionHandler.cpp:42-43`) — remplacer par liste explicite des IDs PatchEdit Init en story 7.2.
- **Chaînes brutes `uiMessageText` / `uiMessageSeverity`** (`ModuleActionHandler.cpp:148,153`) — migré tel quel ; utiliser constantes `ExceptionPropagator` lors d’un refactor footer.

## Deferred from: code review of 5-3-matrix-modulation-section-i-c-p-gui (2026-06-18)

- **`valueTreePropertyChanged` without root-tree guard** (`MatrixModulationPanel.cpp:35-41`) — same pattern as `ModuleHeader.cpp`; add `treeWhosePropertyHasChanged != state_` if child trees ever carry colliding property IDs.
- **Millisecond timestamp collision on rapid clicks** (`MatrixModulationPanel.cpp:288-319`) — project-wide APVTS trigger pattern; use counter or `Time::getHighResolutionTicks()` if automation reports dropped actions.
- **±1 px button gap at non-integer UI scales** (`MatrixModulationPanel.cpp:349-353`) — same `scaledInt` add-then-round vs round-then-add drift as `ModuleHeader::layoutInitCopyPasteButtons`; cosmetic only.

## Deferred from: code review of 5-2-module-copy-paste-enable-and-gray-rules (2026-06-18)

- **`patchModuleKindFromWidgetId` maps Paste widget IDs too** (`ClipboardPasteEnabledResolver.cpp`, `PluginProcessor.cpp`) — `endsWith("Copy")` guard prevents false copy today; Story 7.2 must split Copy vs Paste handlers before reusing this lookup.
- **`refreshDeviceLimits()` on clipboard property change** (`InternalPatchesPanel.cpp:324`) — correct button state but unnecessary ROM re-read; narrow to `updatePasteStoreEnabled` when limits unchanged.
- **Duplicated `*PasteEnabled` property ID lists** (`PluginProcessor.cpp`) — `initializeClipboardPasteEnabledProperties` and `refreshClipboardPasteEnabledProperties` maintain separate arrays; single SSOT table would reduce drift risk.
- **MASTER module `*PasteEnabled` properties** — FR-35 matrix-mod graying includes MASTER Paste buttons; no MASTER C/P widgets exist yet (InitOnly layout); wire when MASTER headers get C/P in Epic 7.

## Deferred from: Story 5.2 smoke test — Button disabled paint fix (2026-06-18)

- ~~**`BankUtilityPanel` alpha graying vs `Button` disabled colours**~~ **Resolved 2026-07-25 (v1 polish controls/footer)**: superseded prior “clickable while gray” intent — Bank Utility now uses body-style module lock (dim + `setInterceptsMouseClicks(false,false)`) on Matrix-6/6R, with Matrix-1000-only footer published/cleared on gray enter/leave (not on click).

## Deferred from: code review of 5-1-clipboardservice-compatibility-matrix (2026-06-18)

- **`pasteModule` returns true when all params skipped** — implicit UX per spec; no partial-paste feedback in v1; revisit if Story 7.2 needs paste-outcome granularity.
- **Cross-module paste uses `displayName` matching** — same pattern as Epic 3 init services; relies on PluginDisplayNames SSOT invariant.
- **No thread-safety contract on ClipboardService** — message-thread assumption aligned with other Core services until Story 7.2 wiring.
- **`ClipboardService.cpp` exceeds 200-line self-review target** — 467 lines with anonymous-namespace helpers; optional split if file grows in 7.2.

## Deferred from: code review of 3-4-master-module-init-confirmation-path (2026-06-18)

- **Flag suppress sans RAII** (`PluginProcessor.cpp:1347-1349`) — même set/restore que story 3.3 ; ScopeGuard si refactor transversal.
- **Init re-déclenché au chargement preset** — propriétés `midiInit`/`vibratoInit`/`miscInit` (timestamps) sérialisées dans `copyState` ; pattern identique Matrix Mod init 3.3.
- **Pas de tests GUI Cancel/Confirm** — FR-17 couvert manuellement (SM-1) ; pas de harness GUI automatisé v1.
- **`handleMasterModuleInitPropertyChange` > 15 lignes** — hygiène Clean Code, non bloquant AC.
- **`kMatrix1000OnlyFooterMessage` dans le même diff** — hunk story 8.5 ; découper au commit.

## Deferred from: code review of 3-3-matrix-mod-init-defaults (2026-06-17)

- **Suppress flag without RAII** (`PluginProcessor.cpp:1286-1293`) — Same set/restore pattern as `swapMatrixModBusContents`; cross-cutting hygiene if scope guard is introduced later.
- **Hardcoded if-chain `parseMatrixModBusInitIndex`** — Mirrors `parseBankButtonIndex`; safe while `kModulationBusCount == 10`; revisit if bus count becomes variant-specific.
- **`sendProgramChange` in `handlePatchNumberChange`** — Bundled from patch-manager / 8-5 work in the same `PluginProcessor` diff, not 3-3 AC scope.
- **`"deviceDetected"` string literal** — Bundled device-type reconciliation; name constant when 8-5 / device work is finalized.

## Deferred from: code review of u-11-module-panel-config-dedup (2026-06-17)

- **O(n) linear descriptor scan at panel construction** (`PluginHelpers.cpp:63-124`) — factory hash maps exist but builder uses PluginHelpers scan; negligible at 13-panel init unless resolution path (A) adopted.
- **`PluginHelpers::resolveParameterWidgetKind` exceeds Clean Code limits** (`PluginHelpers.cpp:63-124`) — ~60 lines, high cyclomatic complexity; accepted per code-review decision B (2026-06-17); optional future hygiene refactor.

## Deferred from: code review of u-2b-d-013-paint-hairlines (2026-06-17)

- **`MultiColumnPopupMenu` manual border calc** (`MultiColumnPopupMenu.cpp`) — bypasses `snappedStrokeThicknessFromDesign`; out of U-2b AC 2 scope.
- **ScrollablePopupMenu thumb inset baked at ctor** (`ScrollablePopupMenu.cpp:259-263`) — `logicalInsetPixelsFromDesign` uses `displayScale` 1.0 before component is on-screen; needs CustomScrollBar refresh API; cosmetic Retina thumb inset.
- **EnvelopeDisplay / TrackGeneratorDisplay zero-bounds mid-drag** — division-by-zero risk if host resizes during gesture; pre-existing, not introduced by hairline migration.
- **SectionHeader contentArea height vs component height at high uiScale** — pre-existing layout scaling (U-2), not paint hairline scope.
- **`systemDisplayScaleForComponent` in paint() per frame** — established pattern across compliant widgets; perf optimization deferred.

## Deferred from: code review of u-1-testcomponent-enrichment-d-064 (2026-06-16) — Group 1 Infrastructure

- **CMake `TIMESTAMP` build date frozen at configure time** (`CMakeLists.txt`) — bundled versioning change, not U-1 scope.
- **`MATRIX_CONTROL_PRERELEASE_SUFFIX` defaults to `"alpha"`** (`CMakeLists.txt`) — release-build footgun from bundled CMake changes.
- **`juce_gui_basics` added to unit test target** (`CMakeLists.txt`) — headless CI risk; unrelated to TestComponent sandbox.
- **`restoreSettingsPanelFromState` skips HW latency in standalone** (`PluginEditor.cpp`) — settings consolidation story scope.
- **`refreshAudioFromCombo` fallback without populated header** (`PluginEditor.cpp`) — header-panel story scope.
- **VST3 helper `-Wno-deprecated-declarations` removal** (`CMakeLists.txt`) — unrelated build hygiene.
- **`juce_gui_extra` linked without documented rationale** (`CMakeLists.txt`) — About/popup story scope.
- **`std::function` heap alloc on every `layoutTestContentHost` pass** (`TestComponent.cpp`) — perf optimization, not blocking UAT.

## Deferred from: code review of 7-10-about-modal (2026-06-16)

- **Unrelated `ModulePanelConfigBuilder` changes bundled in same commit range** (`CMakeLists.txt`) — separate story `u-11-module-panel-config-dedup`; split for cleaner review boundaries.
- **No automated About UI tests** (Escape, click-outside, menu wiring) — AC8 accepts manual link verification for v1.
- **Build clean claim not evidenced in diff** (AC7) — verify at merge/CI.
- **Fixed `kDesignHeight` may clip if fonts or localized strings grow** (`AboutPanel.h`) — English-only v1 per story out-of-scope.
- **`baseWidth <= 0` scale fallback skips `updateAboutWindowLayout`** (`PluginEditor.cpp`) — same pattern as Settings window.

## Deferred from: code review of u-0b-factory-dimension-registry-and-descriptor-decoupling (2026-06-09)

- **`patchMutatorEnable*` descriptor type vs registry width** — Descriptors typed `kButton`, UI uses `TSS::Toggle`; registry returns `kInit` for guard only. Proper fix: add `kToggle` descriptor type (future taxonomy story).
- **`WidgetFactory` default slider overload still reads `Design*`** (`WidgetFactory.cpp:63-64`) — AC 2.1 permits unchanged overload; slider dimension registry migration deferred.
- **Dual Design* read paths (DimensionFactory + WidgetDimensionRegistry)** — Path A architecture; registry reads `DesignAtoms` by design; runtime struct unification deferred.
- **Suffix/prefix registry rules fragile** (`WidgetDimensionRegistry.cpp`) — accepted Path A trade-off; explicit map expansion when ID taxonomy changes.
- **No `DimensionFactory` unit tests** — AC 7 requires registry guard only; dimension struct coherence tests deferred.

## Deferred from: code review of 2-10-matrix-mod-bus-reorder-sysex (2026-06-06)

- **`suppressMatrixModParameterSysEx_` bool vs nested counter** (`PluginProcessor.h:185`) — story spec recommends bool; nested multi-swap reentrancy out of scope for message-thread v1.

## Deferred from: code review of r-3-midi-port-open-error-feedback (2026-06-06)

- **`MidiPortOpenResult` types colocated in `MidiPortOpenFeedback.h`** — pulls `juce_audio_processors` into port headers; extract to `MidiPortOpenResult.h` if port layer grows.
- **No end-to-end `kOpenRejected` propagation test** — formatter branch covered; OS-level open rejection not mockable without harness.

## Deferred from: code review of r-2-hardware-latency-gui (2026-06-06)

- **Standalone header layout overlap (HW LATENCY + SCALE/SKIN vs UI ELEMENTS)** (`HeaderPanel.cpp`) — superseded by planned SETTINGS window story (Epic 7.7+); header interim layout accepted until then.
- **Redundant `setHardwareLatencyMs` + `updateHostDisplay` on editor open** (`PluginEditor.cpp:130-132`) — idempotent host refresh; no functional bug.
- **Sample-rate fallback 44100 before first `prepareToPlay`** (`PluginProcessor.cpp:512-518`) — corrected on prepare; same pattern as audio passthrough.
- **No automated APVTS round-trip test for `hardwareLatencyMs`** — mirrors existing `inputGainDb` test gap.

## Deferred from: code review of 2-11-header-panel-layout-and-widget-styling (2026-06-05)

- **Slider `-inf` display keyed on `unit_ == "dB"`** (`Slider.cpp:127`) — only header gain slider uses dB unit today; flag or per-slider opt-in when a second dB slider is added.
- **Optional `MidiManager::dispatchOutboundMessage` outbound-notify test** (`MidiManagerTests.cpp`) — AC 7.2 optional; producer + `MidiActivityTrackerTests` cover main paths.
- **Clock step-back guard in `computeLevelFromTimestamp`** (`MidiActivityTracker.cpp:11`) — inherited from Story 2.8; extremely rare on monotonic hi-res counter.

## Deferred from: code review of 2-8-activity-leds-on-queue-traffic (2026-06-05)

- **`memory_order_relaxed` on activity atomics** — matches `AudioPassthroughProcessor` peak pattern (Story 2.7); acquire/release optional hardening deferred.
- **`ActivityLed` unused `width_`/`height_`/`uiScale_`** — mirrors `PeakIndicator` prior art; cleanup deferred to Story 7.8 header polish.
- **No unit test for `MidiManager::dispatchOutboundMessage` tracker notify** — optional coverage; producer + EditorPath tests cover main paths.
- **Clock step-backward guard in `computeLevelFromTimestamp`** — extremely rare on monotonic hi-res counter; low priority.

## Deferred from: code review of 2-7-audio-passthrough-and-peak-indicator (2026-06-05)

- **Standalone audio-from combo not refreshed on device change** (`PluginEditor.cpp:73-77`) — follow-up UX; spec lists channels at editor open only.
- **Weak symbols (`__attribute__((weak))`) not portable to MSVC** (`StandaloneAudioInputRouterStubs.cpp:5-15`) — Windows build follow-up; macOS verified.
- **Standalone `audioFromSourceId` uses unstable numeric channel indices** (`StandaloneAudioInputRouterStandalone.cpp:35-36`) — device-change edge case; channel names as IDs optional hardening.
- **`getInstrumentPathEnabled` reads `apvts.state` on audio thread (standalone)** (`PluginProcessor.cpp:321-324`) — pre-existing pattern extracted from `processBlock`; atomic cache deferred to Epic 7/8.
- **Redundant `setAudioFromSourceId` / `setInputGainDb` on startup** (`PluginProcessor.cpp:386-388`, `PluginEditor.cpp:97-112`) — harmless duplication between ctor init and editor restore.

## Deferred from: code review of 2-9b-header-routing-controls-uat-slice (2026-06-05)

- **~~Combo/backend mismatch si `MidiManager::set*Port` échoue~~** (`PluginProcessor.cpp`) — ~~pattern brownfield pre-existing ; combo avance, APVTS inchangé jusqu’au prochain succès.~~ **Resolved 2026-07-25 (v1-2, review patch option 2)**: reporting sync / standalone / last deferred attempt align APVTS to open reality; soft intermediate plugin retries keep the desired id for reopen; Header combo resyncs from APVTS.
- **Double forwarding host buffer en standalone** (`PluginProcessor.cpp:210-218`) — AC #7 assume buffer hôte vide ; edge case host injectant MIDI en standalone.
- **~~MIDI FROM + KEYBOARD FROM même device~~** (`HeaderPanel` / `PluginProcessor`) — ~~deux `juce::MidiInput` sur un identifiant ; pas de garde UI dans scope UAT slice.~~ **Resolved 2026-07-25 (v1-2, review patch)**: standalone rejects same-id on interactive setters **and** on `syncMidiPortsFromStateImpl`; conflict footer clears after a successful distinct selection.
- **`setStateInformation` sans resync ports/combos** (`PluginProcessor.cpp:233-246`) — restore ports uniquement au ctor `PluginEditor` ; reload session complète OK. (Full host reload resync remains out of v1-2 scope beyond open/combo coherence.)
- **Thread safety `keyboardFromEnabled` message/audio** (`PluginProcessor.cpp:213-214`) — `juce::var` lu audio thread, écrit message thread ; pre-existing story 2.3.

## Deferred from: code review of 2-9-wire-midimanager-queue-consumer (2026-06-05)

- **RPC + consumer concurrent `sysExDelay_` / `MidiSender` access** (`MidiManager.cpp:223,275,351`) — brownfield threading; story completion notes accept timestamp-only gate; `MidiSender` not thread-safe if inquiry runs concurrent with consumer; no call-site refactor in scope.
- **AC #8c strict `millisUntilNextAllowed` assertion absent** (`MidiManagerTests.cpp:113-133`) — gate-sharing test verifies drain-without-hang only; strict timing deferred to `SysExInterMessageDelayTests` per dev notes.
- **`stopThread` during blocking `waitUntilReady`** (`MidiManager.cpp:351`) — consumer may block on inter-SysEx sleep; teardown races possible; same pattern as existing `PluginProcessor` lifecycle.
- **No test for `MidiConnectionException` → `updateErrorState` in consumer** (`MidiManagerTests.cpp`) — catch branch unverified; no-output guard prevents throw in current tests.
- **Drain tests assert queue empty, not bytes on wire** (`MidiManagerTests.cpp`) — no spy on `sendMidiMessage`/`sendSysEx`; hardware validation deferred to Story 2.9b smoke.

## Deferred from: code review of 2-6-matrix-mod-bus-parameter-sysex (2026-06-05)

- **`readPackedByte` silent zero on invalid offset** (`MatrixModBusParameterSysExDispatcher.cpp:44-47`) — same `PackedFieldCodec::safeOffset` pattern as `PatchParameterSysExDispatcher`; descriptor offsets are stable.
- **`syncIntToBuffer` / `syncChoiceToBuffer` skip when `rawValue == nullptr`** (`ApvtsPatchMapper.cpp:38-47`) — pre-existing mapper behaviour; Matrix Mod extension inherits it.
- **Unknown `parameterId` silent return** (`MatrixModBusParameterSysExDispatcher.cpp:25-27`) — matches patch dispatcher; typo in ID fails silently in release.
- **Dual routing ID sets** (`PluginProcessor.cpp:632-643`) — `patchParameterIds_`, `matrixModParameterIds_`, and dispatcher map built separately; established 2.4/2.5 branching pattern.
- **Matrix Mod mapper test one-way only** (`ApvtsPatchMapperTests.cpp:175-211`) — `apvtsToBuffer` spot-check bus 0 satisfies story task; full `bufferToApvts` round-trip optional.
- **No SysEx coalescing on rapid Matrix Mod edits** (`PluginProcessor.cpp:518-525`) — one 0x0B per property change by design (FR-14); debounce deferred to future perf work.

## Deferred from: code review of 2-5-apvts-master-parameter-to-full-master-sysex-0x03 (2026-06-05)

- **Choice unit test omits full payload byte-compare** (`MasterParameterSysExDispatcherTests.cpp:91-123`) — int test compares `msg->sysExData == expected`; choice test stops at header/frame checks. AC #5 satisfied; parity optional.
- **Full test-suite pass not evidenced in changeset** — no CI log or local `Matrix-Control_Tests` output in artifact; run before merge.

## Deferred from: code review of 2-4-apvts-patch-parameter-to-sysex-0x06 (2026-06-04)

- **`MidiManager::enqueueRemoteParameterEdit` sans tests** (`MidiManager.cpp:172`) — garde silencieuse + enqueue EditorPath ; `MidiManagerTests` reste stub TODO, hors périmètre 2.4.
- **Pas de test E2E `PluginProcessor` → `MidiManager` → queue** — câblage prod validé par revue ; test d’intégration reporté à story consommateur / harness MIDI.
- **`apvtsToBuffer()` O(n) par changement patch** (`PluginProcessor.cpp:501`) — aligné dev notes 2.4 ; batching perf = story future.

## Deferred from: code review of 2-2-sysexdelayprofile-and-inter-message-delay (2026-06-04)

- **Inquiry / RPC SysEx bypass inter-SysEx gate** (`MidiManager.cpp:204,252`) — only `sendSysExWithDelay` uses `SysExInterMessageDelay` per AC #5; global gate for all outbound SysEx deferred to Story 2.9 consumer.
- **4-char Device Inquiry version limits optimised EPROM detection** (`SysExDecoder.cpp:181-191`) — `extractDeviceVersion` fills at most 4 chars; full `TAUNTEK`/`GLIGLI`/`NORDCORE` tokens cannot match on real replies until decoder/SM-1 extends version extraction.

## Deferred from: code review of 2-1-midioutboundqueue-core (2026-06-04)

- **Unbounded queue growth** (`MidiOutboundQueue.cpp:5`) — no max depth; revisit with 2.9 consumer or flood policy.
- **SysEx via `enqueueRealtime`** (`MidiOutboundQueue.cpp:5`) — no `isSysEx()` reroute; producer discipline in 2.3.
- **`isEmpty` / `dequeue` TOCTOU** (`MidiOutboundQueue.cpp:38`) — consumer must not gate on prior `isEmpty()`.
- **Large SysEx copy + mutex hold on dequeue** (`MidiOutboundQueue.cpp:30`) — audio enqueue can block; profile before optimizing.
- **No flush/clear API** (`MidiOutboundQueue.h`) — disconnect/panic handling deferred to 2.9.
- **`enqueueRealtime` by-value copy** (`MidiOutboundQueue.cpp:5`) — optional `&&` overload if profiling warrants.
- **Empty SysEx `MemoryBlock` accepted** (`MidiOutboundQueue.cpp:11`) — validate at consumer if needed.
- **Single-consumer not enforced** (`MidiOutboundQueue.cpp:17`) — document for `MidiManager::run()` in 2.9.
- **Tests beyond AC A–D** (`MidiOutboundQueueTests.cpp:10`) — SysEx byte assert, concurrency optional later.

## Deferred from: code review of 1-5-patch-name-bytes-0-7-sync (2026-06-04)

- **`apvtsToBuffer` no thread contract, allocates** (`PatchNameSyncer.cpp:15`) — `valueTreePropertyChanged` may fire off the message thread; `getProperty().toString()` + `setName` allocate. Same pre-existing pattern as `ApvtsPatchMapper`; revisit with threading-hardening story.
- **Lossy 6-bit/7-bit charset round-trip in `PatchModel`** (`PatchModel.cpp:62`) — `setName` masks 0x7F, `getName` masks 0x3F + remaps codes < 0x20; identity only in printable 0x20-0x5F band. Pre-existing `PatchModel` behavior, out of Story 1.5 scope.
- **Model name not seeded from default at startup** (`PluginProcessor.cpp:271`) — default `"--------"` set before `addListener`, so `apvtsToBuffer` never runs at construction; model name stays zero-init (decodes `"@@@@@@@@"`) until first edit/patch load. Matches existing mapper construction pattern.

## Deferred from: code review of 1-3-apvtspatchmapper-round-trip (2026-06-04)

- **`apvtsToBuffer()` O(n) full-buffer syncs per parameter change** (`PluginProcessor.cpp:459`) — intentional for Story 1.3; batching belongs in a future performance story.
- **Silent null skip without assertion** (`ApvtsPatchMapper.cpp:36-38`) — correct guard for synthetic-descriptor test pattern; add `jassert` if stricter contract needed.
- **Thread safety — `bufferToApvts()` / `apvtsToBuffer()` unguarded** (`ApvtsPatchMapper.cpp`) — explicitly scoped to future threading hardening story per spec dev notes.
- **`memset` in Test C bypasses `PatchModel` API** (`ApvtsPatchMapperTests.cpp:190`) — acceptable direct-buffer reset in test; revisit if PatchModel gains write invariants.
- **Descriptor vectors built twice at startup** (`PluginProcessor.cpp:526-534`) — mapper ctor + `buildPatchParameterIdSet()` both call static builders; startup-only, negligible cost.
- **Hardcoded `intDescs[0]` / `[2]` indices in tests** (`ApvtsPatchMapperTests.cpp:132-134`) — confirmed correct by ECH reading `PluginDescriptorsPatchEdit.cpp`; same deferred pattern as Story 1.4.
- **Public `bufferToApvts()` feedback loop risk** (`ApvtsPatchMapper.h:22`) — not called in current code; Story 2.4 caller sites must use `MessageManager::callAsync` per spec thread note.

## Deferred from: code review of 1-4-apvtsmastermapper-round-trip (2026-06-04)

- **Hardcoded vector indices in tests** (`Tests/Unit/ApvtsMasterMapperTests.cpp:130`) — `intDescs[0]` / `intDescs[2]` rely on `buildIntDescriptors()` returning a stable order; tests pass and document expected index-to-parameter mapping; revisit with `std::find_if` if descriptor order ever changes.

## Deferred from: code review of 1-2-mastermodel-packed-buffer (2026-06-03)

- **`safeOffset` no release-mode bounds guard** (`Source/Core/Models/PackedFieldCodec.cpp:22`) — debug-only jassert mirrors the original PatchModel pattern; descriptor offsets are compile-time constants so the risk is low; revisit if runtime-provided offsets are ever introduced.
- **`choices.size() - 1` size_t underflow when choices is empty** (`Source/Core/Models/MasterModel.cpp:30`, same in `PatchModel.cpp`) — pre-existing from Story 1.1; all real descriptors have ≥ 1 choice; add `jassert(!descriptor.choices.isEmpty())` if stricter contract is desired.

## Deferred from: code review of 1-1-patchmodel-packed-buffer (2026-06-02)

- **`signBitPosition` undefined for `maxValue ≤ 0` or non-`2^n−1` ranges** (`Source/Core/Models/PatchModel.cpp:53-58`) — `jlimit` acts as a safety net so no current descriptor is affected; revisit if a signed descriptor with a non-power-of-two max is ever added.
- **`getChoiceIndex` silently clamps stale/corrupt buffer bytes** (`Source/Core/Models/PatchModel.cpp:40`) — defensive clamping via `jmax`/`jlimit` is correct for the current descriptor set; add `jassert(!descriptor.choices.isEmpty())` if a stricter contract is desired.

## Deferred from: code review of 6-5-retry-from-parent-snapshot (2026-06-20)

- **Footer messages not asserted in retry tests** (`PatchMutatorEngineTests.cpp`) — AC2 requires `{ success: false, footerMessage }`; constants exist and are returned but tests only check `success`.
- **`retry_usesParentSnapshot_notResult` does not prove algorithm input source** (`PatchMutatorEngineTests.cpp:354-356`) — asserts stored `parentSnapshot` byte, not that `working` was loaded from `parentSnapshot` vs `result`.
- **Missing-root failure path untested** (`PatchMutatorEngine.cpp:165-170`) — stale `selectedRootIndex_` with no matching root returns `kNoSelectionFooterMessage`; no test.
- **Highest-sorted-root fallback untested** (`PatchMutatorEngine.cpp:369-373`) — no retry test with multiple roots and `selectedRootIndex_ < 0`.
- **`retry_noOpRecipe_blocked` covers Amount=0 only** (`PatchMutatorEngineTests.cpp:441`) — `randomPercent == 0` guard path unverified.

## Deferred from: code review of 7-3c-bank-utility-unlock-simplify (2026-06-19)

- **Point rouge transitoire « off » lors d'une sélection de banque** (`InternalPatchesPanel.cpp:76-88`) — `kCurrentBankNumber` déclenche `refreshBankLockIndicator` avant `markBanksLockedInApvts()` ; état final correct ; risque visuel faible.
- **Nom `kBanksLocked` ne reflète plus la sémantique display-only** — dette sémantique pré-existante ; rename hors scope 7-3c.
- **Commande grep AC#9 dans la story** — chemin répertoire inexistant ; cosmétique doc.
- **`InternalPatchesPanel.cpp/.h` absents du File List story** — wiring indicateur AC#7 ; mettre à jour le File List.

## Deferred from: spec-mutator-synth-load-history-export-compare (2026-07-16)

- source_spec: `_bmad-output/implementation-artifacts/spec-mutator-synth-load-history-export-compare.md`
  summary: History-gate and collision AlertWindow modals run nested message loops from APVTS/ValueTree change paths.
  evidence: Blind Hunter; `handlePatchNumberChange` → `confirmPatchContextChangeGate` → `runModalLoop`; re-entrancy risk if timers/edits fire during modal. Partial mitigation (2026-07-16 review): gate refuses off message-thread; nested-loop redesign still open.

- source_spec: `_bmad-output/implementation-artifacts/spec-mutator-synth-load-history-export-compare.md`
  summary: ~~Hard-coded 50 ms settle / 500 ms queue-idle timeouts for device dump may be wrong for slow MIDI interfaces.~~ **Resolved 2026-07-25 (v1-2)**: `MidiRequestTiming` SSOT with profile-aware floors shared by dump + inquiry; idle timeout still fails dump (v1-1 path).
  evidence: Blind Hunter / Edge Case; silent stale-buffer risk if synth is slower than settle; needs hardware profiling or delay profile hook.

- source_spec: `_bmad-output/implementation-artifacts/spec-mutator-synth-load-history-export-compare.md`
  summary: Compare footer clear matches exact `kCompareLockedFooter` string only.
  evidence: Blind Hunter; brittle if another message overwrites the footer while Compare is active.

- source_spec: `_bmad-output/implementation-artifacts/spec-mutator-synth-load-history-export-compare.md`
  summary: End-to-end device-dump path has no mocked-MIDI unit coverage.
  evidence: Blind Hunter + Completion Notes; only idle/availability smoke tests exist; needs fake MIDI port harness.

- source_spec: `_bmad-output/implementation-artifacts/spec-mutator-synth-load-history-export-compare.md`
  summary: History-gate Export path and Export button path handle collision resolution via two different sync/async styles.
  evidence: Blind Hunter; gate captures sync resolution; button uses async callback — latent fork if modal becomes async.

- source_spec: `_bmad-output/implementation-artifacts/spec-mutator-synth-load-history-export-compare.md`
  summary: Export Keep-both collision suffixes stop at `-999` and may return an existing folder.
  evidence: Code review 6-13; `resolveKeepSessionFolder` max suffix 999; extreme-stress only — harden with explicit failure or unbounded suffix later.

## Deferred from: spec-disabled-controls-look (2026-07-17)

- source_spec: `_bmad-output/implementation-artifacts/spec-disabled-controls-look.md`
  summary: Dead `*BorderDisabled` Look/skin fields remain loaded while paint uses enabled border colours.
  evidence: Blind Hunter / Edge Case; Button, Toggle, ButtonLike tokens still wired but unused at paint time — cleanup when skin API is next trimmed.

- source_spec: `_bmad-output/implementation-artifacts/spec-disabled-controls-look.md`
  summary: Very small or near-zero slider values can omit the disabled value fill (width rounds to 0).
  evidence: Edge Case Hunter; same rounding path as enabled sliders; pre-existing, not introduced by the three-layer disabled paint.

## Resolved during code review 6-13 (2026-07-16)

- Device dump no longer blocks the message thread (`requestSinglePatchAsync` + timers).
- Dump failure / no device keeps Mutator history (clear only after successful dump).
- Async one-shot SysEx capture ignores non-patch frames and keeps listening until timeout.
- History-gate modal refused when not on the message thread (`jassert` + cancel).


## Deferred from: spec-startup-patch-name-default (2026-07-17)

- source_spec: `_bmad-output/implementation-artifacts/spec-startup-patch-name-default.md`
  summary: No PluginProcessor-level unit test asserts PatchNameSyncer model bytes after session restore.
  evidence: Blind Hunter; MutatorRecipePersistenceTests cover APVTS helpers only — model sync lives in PluginProcessor::resetEphemeralMutatorStateAfterSessionLoad.

- source_spec: `_bmad-output/implementation-artifacts/spec-startup-patch-name-default.md`
  summary: Patch-name session policy lives inside MutatorSessionPersistence helpers.
  evidence: Blind Hunter; kPatchName is Patch Edit metadata, not mutator-scoped — risk if mutator-only persistence is later refactored without noticing the name rule.

- source_spec: `_bmad-output/implementation-artifacts/spec-startup-patch-name-default.md`
  summary: Offline (no MIDI ports) session restore can keep patch PARAM values while Patch Name resets to --------.
  evidence: Blind Hunter; accepted for this bugfix per D-010 / option 1; PARAM strip policy remains SessionPersistencePolicy scope.

## Deferred from: quick-dev midi-docs-oberheim-verification (2026-07-17)

- source_spec: `_bmad-output/implementation-artifacts/spec-midi-docs-oberheim-verification.md`
  summary: Matrix-1000 Global Parameter table omits byte 33 (jumps 32→34) as in the official manual.
  evidence: Blind Hunter; PDF table has the same gap; do not invent a Not Used row without a clearer source.

- source_spec: `_bmad-output/implementation-artifacts/spec-midi-docs-oberheim-verification.md`
  summary: Matrix-1000 Bank Select SysEx reconstruction not glyph-proven from the scanned PDF OCR.
  evidence: Blind Hunter; page OCR is mangled; MD form retained pending a cleaner scan or hardware capture.

- source_spec: `_bmad-output/implementation-artifacts/spec-midi-docs-oberheim-verification.md`
  summary: Matrix-6/6R remote-edit prose references a Select Parameter operation with no opcode section.
  evidence: Blind Hunter; youngmonkey page also lacks that opcode; source gap, not introduced by this verification.


## Deferred from: code review of 8-1-devicetyperegistry-and-member-byte-fix (2026-07-17)

- source_spec: `_bmad-output/implementation-artifacts/8-1-devicetyperegistry-and-member-byte-fix.md`
  summary: Member-byte accept/reject decision tree remains duplicated across DeviceTypeRegistry, SysExDecoder::validateMatrixFamilyDevice, and SysExDelayProfile::deviceFamilyFromMemberBytes.
  evidence: Blind Hunter; constants are shared but branching is not — future UAT byte changes risk partial updates.

- source_spec: `_bmad-output/implementation-artifacts/8-1-devicetyperegistry-and-member-byte-fix.md`
  summary: SysExDelayProfileTests hardcode 0x02/0x00 and 0x01/0x00 instead of SysExConstants::DeviceInquiry named constants.
  evidence: Blind Hunter; pre-existing test style; unknown-member fallback can mask constant drift.

- source_spec: `_bmad-output/implementation-artifacts/8-1-devicetyperegistry-and-member-byte-fix.md`
  summary: Asymmetric naming kExpectedFamily (low only) vs kExpectedFamilyHigh invites incomplete family checks.
  evidence: Blind Hunter; rename would touch call sites beyond this story’s audit scope.

- source_spec: `_bmad-output/implementation-artifacts/8-1-devicetyperegistry-and-member-byte-fix.md`
  summary: Optional decoder golden coverage only rejects bad familyHigh; missing wrong manufacturer / familyLow / swapped D-080 members / happy-path familyHigh asserts.
  evidence: Blind Hunter; AC6 treats golden decoder tests as optional.

- source_spec: `_bmad-output/implementation-artifacts/8-1-devicetyperegistry-and-member-byte-fix.md`
  summary: testD080MemberConstants does not lock provisional kMatrix6MemberLow/High against named literals.
  evidence: Blind Hunter; AC2 only requires M-1000 D-080 lock.

- source_spec: `_bmad-output/implementation-artifacts/8-1-devicetyperegistry-and-member-byte-fix.md`
  summary: fromApvtsProperty remains untested in DeviceTypeRegistryTests.
  evidence: Blind Hunter; AC6 focuses on inquiry / member-byte mapping, not APVTS override path.

## Deferred from: code review of 8-2-device-inquiry-and-footer-identity (2026-07-18)

- source_spec: `_bmad-output/implementation-artifacts/8-2-device-inquiry-and-footer-identity.md`
  summary: ~~Timer::callAfterDelay / MessageManager::callAsync lambdas capture raw MidiManager `this` without SafePointer; destructor bumps async token but cannot stop a callback that already started on a destroyed object.~~ **Resolved 2026-07-25 (v1-3)**: `JUCE_DECLARE_WEAK_REFERENCEABLE(MidiManager)` + WeakReference guards on Device Inquiry and `requestSinglePatchAsync` deferred closures; `asyncRequestToken_` retained for living-object cancel.
  evidence: Blind Hunter + Edge Case Hunter; same pattern already used by requestSinglePatchAsync / armAsyncSinglePatchCapture.

- source_spec: `_bmad-output/implementation-artifacts/8-2-device-inquiry-and-footer-identity.md`
  summary: No jassert that performDeviceInquiry runs on the message thread (Timer / callAsync assumptions).
  evidence: Edge Case Hunter; today only syncMidiPortsFromStateImpl (message thread) calls refresh → inquiry.

- source_spec: `_bmad-output/implementation-artifacts/8-2-device-inquiry-and-footer-identity.md`
  summary: No unit tests for async Device Inquiry success, timeout, invalid re-arm, or cancel/restart under a fake receiver — only DeviceInquiryTrigger debounce + clear-when-no-ports.
  evidence: Blind Hunter; story T5 allows documenting manual UAT when no injectable seam exists.

## Deferred from: code review of t-1-matrix-device-inquiry-simulator (2026-07-18)

- source_spec: `_bmad-output/implementation-artifacts/t-1-matrix-device-inquiry-simulator.md`
  summary: No automated tests in Matrix-Simulator repo after extract; goldens live only in Matrix-Control SysExEncoderTests and cannot catch simulator DeviceInquiry.h drift.
  evidence: Blind Hunter; Acceptance Auditor soft drift-process note.

- source_spec: `_bmad-output/implementation-artifacts/t-1-matrix-device-inquiry-simulator.md`
  summary: Matrix-Simulator CMakeLists hard-codes machine-local JUCE absolute paths (plus env/preset fallbacks).
  evidence: Blind Hunter; same developer-machine pattern as Matrix-Control.

- source_spec: `_bmad-output/implementation-artifacts/t-1-matrix-device-inquiry-simulator.md`
  summary: Once hasPortFilter_ is true, enabled port IDs are sticky — stale identifiers accumulate and there is no UI path back to “all ports”.
  evidence: Blind Hunter + Edge Case Hunter (empty filter / persistence).

- source_spec: `_bmad-output/implementation-artifacts/t-1-matrix-device-inquiry-simulator.md`
  summary: Non-inquiry SysEx floods post unbounded MessageManager::callAsync log lines with no rate limit.
  evidence: Edge Case Hunter; AC only requires ignore/log without crash.

## Deferred from: code review of 8-3-ui-lock-without-synth (2026-07-18)

- source_spec: `_bmad-output/implementation-artifacts/8-3-ui-lock-without-synth.md`
  summary: Four CompareLockBinder instances each call syncDeviceLockFooter from apply — no single owner for left-zone device-lock guidance.
  evidence: Blind Hunter; BodyPanel, SharedPanel, PatchManagerPanel×2.

- source_spec: `_bmad-output/implementation-artifacts/8-3-ui-lock-without-synth.md`
  summary: maySendEditorSysEx / isDeviceInquirySysEx allowlist is unit-tested but MidiManager inquiry bypasses via ungated sendSysExWithDelay; gate API is deviceDetected-only.
  evidence: Blind Hunter; intentional path-based unlock today — wire or document later.

## Deferred from: quick-dev review of spec-8-4-virtual-instrument-registration-and-bus-layout (2026-07-19)

- source_spec: `_bmad-output/implementation-artifacts/spec-8-4-virtual-instrument-registration-and-bus-layout.md`
  summary: ~~Master SysEx fail-closed is not centralized in MidiManager::sendMaster — only PluginProcessor dispatch and ModuleActionHandler INIT are gated.~~ **Resolved 2026-07-25 (v1-3)**: centralized fail-closed in `MidiManager::sendMaster` (upstream gates retained).
  evidence: Blind Hunter; future callers of MasterParameterSysExDispatcher could bypass FR-46.

- source_spec: `_bmad-output/implementation-artifacts/spec-8-4-virtual-instrument-registration-and-bus-layout.md`
  summary: ~~When master edit becomes allowed after detection, stored master APVTS is not flushed to hardware until the next parameter edit.~~ **Resolved-by-doc 2026-07-25 (v1-3)**: product rule is **next edit only** — no auto-flush of MASTER APVTS on unlock (explicit V1.3 AC5; not open feature debt unless reopened).
  evidence: Edge Case Hunter; no AC requires auto-sync on unlock for MASTER.

## Deferred from: code review of v1-3-release-hygiene-and-hardening (2026-07-25)

- Hygiene pytest fixtures only synthesize Linux VST3/Standalone trees; macOS `.app`/`.component` and Windows `.exe` layouts from `discover_artefact_paths` are untested in `test_check_release_hygiene.py` (scan logic still covered; Release matrix legs exercise real layouts).
- `processOutboundQueue` still admits SysEx via `maySendEditorSysEx` only — no FR-46/`isMasterEditAllowed` re-check. A MASTER blob already on the queue (TOCTOU on device type, or a future enqueue site) can still ship on Matrix-6/6R. V1.3 AC4 scoped fail-closed at `sendMaster` only.

## Deferred from: code review of 10-3-trackgeneratordisplay-direct-apvts-editing (2026-07-20)

- source_spec: `_bmad-output/implementation-artifacts/10-3-trackgeneratordisplay-direct-apvts-editing.md`
  summary: After endChangeGesture clears the gesture gate, a callAsync lambda queued before the drag can still apply a pre-drag value to the display (gate only skips while the gesture is active). Same pattern as EnvelopeDisplayApvtsBinding; fix would need a generation/token across envelope + track bindings.
  evidence: Blind Hunter; intentional copy of 10.2 gesture-gate pattern.

## Deferred from: code review of u-9-master-edit-panel-layout-audit.md (2026-07-24)

- source_spec: `_bmad-output/implementation-artifacts/u-9-master-edit-panel-layout-audit.md`
  summary: At odd UI Scale presets, the sum of individually scaled Master Edit strips (header + modules + gaps) can leave unused bottom slack or starve later strips; story policy is fixed scaled strips with aggregate residual UAT on U-10. **U-10 accepted delta (2026-07-24).**
  evidence: Blind Hunter + Edge Case Hunter; MasterEditPanel.cpp resized.

- source_spec: `_bmad-output/implementation-artifacts/u-9-master-edit-panel-layout-audit.md`
  summary: Parent-assigned module heights (scaledInt of whole-module design heights) can disagree by a pixel with BaseModulePanel’s interior sum (scaled header + N × scaled row) at non-100% presets — possible internal clipping/slack inside Midi/Vibrato/Misc. **U-10 accepted delta (2026-07-24).**
  evidence: Blind Hunter; no module code changed in U-9; U-10 residual.

- source_spec: `_bmad-output/implementation-artifacts/u-9-master-edit-panel-layout-audit.md`
  summary: MasterEditPanelDimensions::sectionHeaderWidth is injected and used at construction, but resized() sizes the section header from the full parent strip width — latent coupling if header width ever diverges from panel width. **U-10 accepted delta (2026-07-24) — latent, not UAT-blocking.**
  evidence: Blind Hunter; pre-existing, unchanged by U-9 stack refactor.

## Deferred from: code review of spec-standalone-audio-input-passthrough (2026-07-25)

- source_spec: `_bmad-output/implementation-artifacts/spec-standalone-audio-input-passthrough.md`
  summary: Stereo source ids carry a start index (`stereo:N`) but AudioPassthroughProcessor stereo mode always reads bus channels 0/1 — selecting a second stereo pair on >2 active inputs is cosmetic only.
  evidence: Blind Hunter; pre-existing routing limit; JUCE settings typically max two active inputs so rarely reachable.

- source_spec: `_bmad-output/implementation-artifacts/spec-standalone-audio-input-passthrough.md`
  summary: Compacted `mono:N` / `stereo:N` ids are stable only for a given active-channel bitmask — enabling a different subset can make the same string refer to a different physical input.
  evidence: Blind Hunter; related to deferred Story 2.7 unstable numeric channel indices; hardware-name ids would be a larger hardening.

## Deferred from: quick-dev spec-logo-alt-click-audio-midi (2026-07-25)

- source_spec: `_bmad-output/implementation-artifacts/spec-logo-alt-click-audio-midi.md`
  summary: Shift+double-click still opens Settings then resets UI scale (pre-existing immediate Shift path).
  evidence: Blind Hunter; Alt path now uses the click/double-click timer; unifying Shift would be a separate behavior change.

- source_spec: `_bmad-output/implementation-artifacts/spec-logo-alt-click-audio-midi.md`
  summary: No automated tests for the logo gesture matrix (click, modifiers, plugin no-op).
  evidence: Blind Hunter; no existing Logo tests in Tests/; add only if gesture regressions become frequent.

- source_spec: `_bmad-output/implementation-artifacts/spec-logo-alt-click-audio-midi.md`
  summary: English user manual still absent while FR shortcut docs were extended.
  evidence: Blind Hunter; pre-existing gap (user-manual.md à venir).


## Deferred from: quick-dev spec-system-style-confirmation-modals (2026-07-25)

- source_spec: `_bmad-output/implementation-artifacts/spec-system-style-confirmation-modals.md`
  summary: Sync confirmation gates still use nested modal loops on the message thread (re-entrancy risk with APVTS/ValueTree during the dialog).
  evidence: Blind Hunter; pre-existing pattern kept by spec; native OS modals do not remove nested-loop risk.

- source_spec: `_bmad-output/implementation-artifacts/spec-system-style-confirmation-modals.md`
  summary: macOS sync native alerts use runModal and do not sheet-attach via withAssociatedComponent (sheet parenting is async-only in JUCE).
  evidence: Blind Hunter; framework limit; association still passed for Windows HWND parenting where available.


## Deferred from: quick-dev spec-rom-factory-names-export-folders (2026-07-25)

- source_spec: `_bmad-output/implementation-artifacts/spec-rom-factory-names-export-folders.md`
  summary: No integration test that a successful ROM-bank device dump injects Matrix1000FactoryPatchNames into PatchModel/APVTS (MIDI dump path is hard to unit-mock).
  evidence: Blind Hunter; lookup unit tests cover the table; inject wiring is thin and reviewed manually.

- source_spec: `_bmad-output/implementation-artifacts/spec-rom-factory-names-export-folders.md`
  summary: Dirty snapshot after ROM factory name inject may disagree with a later hardware re-dump that still has blank name bytes.
  evidence: Blind Hunter; by design for editor UX until hardware UAT proves RAM name retention.


## Deferred from: quick-dev spec-patch-name-inline-edit-and-mutator-dual-line (2026-07-26)

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-name-inline-edit-and-mutator-dual-line.md`
  summary: No automated GUI/unit tests for PatchNameDisplay caret editor, dual-line panel wiring, or interrupt-cancel property allowlist.
  evidence: Blind Hunter; only PatchNameEditRules empty-commit helper is unit-tested; rest covered by manual checklist.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-name-inline-edit-and-mutator-dual-line.md`
  summary: Loading a different patch that keeps the same displayed name may not fire tracked APVTS properties, so an in-progress rename might not auto-cancel.
  evidence: Blind Hunter / Edge Case Hunter; rare same-name reload with empty history; focus-loss still cancels on click-away.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-name-inline-edit-and-mutator-dual-line.md`
  summary: Device-type change mid-edit is not in the interrupt property allowlist.
  evidence: Edge Case Hunter; uncommon during a caret session; bank/patch/history/compare changes still cancel.

## Deferred from: quick-dev spec-confirmation-modal-button-order (2026-07-26)

- source_spec: `_bmad-output/implementation-artifacts/spec-confirmation-modal-button-order.md`
  summary: Win/Linux AlertWindow middle buttons (Keep/Discard/Internal) are mouse-only — no Tab focus or letter shortcut.
  evidence: Blind Hunter; Enter/Escape paths covered; a11y keyboard path for middle not in this story’s AC.

- source_spec: `_bmad-output/implementation-artifacts/spec-confirmation-modal-button-order.md`
  summary: Master Init overlay is still non-modal — Tab can move focus under the overlay so Escape/Enter miss the dialog.
  evidence: Edge Case Hunter; pre-existing overlay pattern; button LTR/Enter-on-dialog focus subtree is in scope, focus trap is not.

## Deferred from: quick-dev spec-bank-utility-import-export (2026-07-26)

- source_spec: `_bmad-output/implementation-artifacts/spec-bank-utility-import-export.md`
  summary: Re-export into an existing BANK N/PATCHES folder can leave orphan .syx files when a patch name changes between runs (old Pxx - OLDNAME.syx remains beside Pxx - NEWNAME.syx).
  evidence: Blind Hunter; overwrite of the same stem is intentional; clearing or slot-prefix cleanup needs a product choice later.

- source_spec: `_bmad-output/implementation-artifacts/spec-bank-utility-import-export.md`
  summary: No dedicated unit tests for the PatchManagerActionHandler bank transfer state machine (cancel/snapshot/restore/generation).
  evidence: Blind Hunter; pure helpers BankImportPlanner and bankExportFileStem are covered; MIDI hardware orchestration remains manual UAT.

- source_spec: `_bmad-output/implementation-artifacts/spec-bank-utility-import-export.md`
  summary: Mid-transfer device swap / reconnect is not re-validated beyond outbound-allowed checks already present.
  evidence: Blind Hunter; rare during a ~100-slot transfer; existing disconnect paths abort or fail restore messaging.

## Deferred from: quick-dev spec-patch-mutator-delete-confirmation (2026-07-26)

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-delete-confirmation.md`
  summary: Delete-confirm AlertWindow checkbox uses a fixed 360×24 size without UI scale awareness.
  evidence: Blind Hunter; AlertWindow custom component; acceptable for V1 — revisit if high-DPI scale clips the row.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-delete-confirmation.md`
  summary: New Delete confirm gate is not listed in the confirmation-modal button-order audit inventory.
  evidence: Blind Hunter; LTR/Enter rules applied locally via configureOrderedAlertButtons; update that inventory in a follow-up if desired.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-delete-confirmation.md`
  summary: No automated coverage for NEVER WARN bypass or Don't-ask-again policy write (editor/UI path).
  evidence: Blind Hunter; handler Cancel/Continue covered; Settings/policy persistence remains manual UAT.

## Deferred from: quick-dev spec-patch-name-display-settings (2026-07-31)

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-name-display-settings.md`
  summary: Optional second Settings control Patch Name Export (Hardware vs Musical names in Bank Utility / export stems), deferred to avoid confusing Display×Export combinations.
  evidence: Product scenario review; Display-only ships first; exports remain musical until a dedicated follow-up.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-name-display-settings.md`
  summary: Patch Mutator export folder/basename still uses the current model patch name, so Hardware Names display can yield BNK-style Mutator export stems until a dedicated musical resolve at freeze/export time.
  evidence: Blind Hunter; Bank Utility export path already forces musical resolve; Mutator freeze uses patchModel_->getName().

## Deferred from: quick-dev spec-settings-combobox-popup-display (2026-07-31)

- source_spec: `_bmad-output/implementation-artifacts/spec-settings-combobox-popup-display.md`
  summary: Settings ComboBoxes still never receive setUiScale; non-1.0 UI scale may leave popup item metrics/fonts unscaled.
  evidence: Blind Hunter; layoutRow only scales sliders; pre-existing, not introduced by popup-look wiring.

- source_spec: `_bmad-output/implementation-artifacts/spec-settings-combobox-popup-display.md`
  summary: Default PopupMenuLook remains transparent on hand-built TSS::ComboBox; forgetting setPopupMenuLook can silently recreate blank/brown popups elsewhere.
  evidence: Blind Hunter; hardening the default look would touch shared popup infrastructure (Ask First in this spec).

- source_spec: `_bmad-output/implementation-artifacts/spec-settings-combobox-popup-display.md`
  summary: Skin switch while a Settings combo popup is open leaves the open menu with stale colours until dismiss/reopen.
  evidence: Edge Case Hunter; setPopupMenuLook only updates stored look; same pattern as Header/WidgetFactory; AC covers reopen after skin change.

- source_spec: `_bmad-output/implementation-artifacts/spec-settings-combobox-popup-display.md`
  summary: No automated assertion that Settings ComboBoxes receive a non-default PopupMenuLook after ctor/setSkin.
  evidence: Blind Hunter; verification remains build + manual UAT for this UI wiring fix.

## Deferred from: quick-dev spec-bug-midi-01-residual-panic-alert (2026-08-02)

- source_spec: `_bmad-output/implementation-artifacts/spec-bug-midi-01-residual-panic-alert.md`
  summary: Header PANIC can overlap left-growing port/audio packets at high UI scale or narrow widths
  evidence: Absolute far-right placement has no reserved layout budget against PacketPlacer; reviewers flagged collision / negative panicX without clamp

- source_spec: `_bmad-output/implementation-artifacts/spec-bug-midi-01-residual-panic-alert.md`
  summary: Outbound consumer can busy-spin while pending SysEx remains and more SysEx are queued (didWork true without sleeping the gate)
  evidence: Blind Hunter; pre-existing processOutboundQueue/run pattern, not introduced by residual drain patch

## Deferred from: quick-dev spec-footer-synth-version-dot-format (2026-08-02)

- source_spec: `_bmad-output/implementation-artifacts/spec-footer-synth-version-dot-format.md`
  summary: ~~No unit tests for footer device-version display formatting (1234→12.34, 116→1.16, dotted passthrough).~~ **Resolved 2026-08-02**: `DeviceVersionDisplayFormatTests` covers the display contract for `TSS::formatDeviceVersionForDisplay`.
  evidence: Blind Hunter; display formatter needs dedicated coverage so CI locks the Vxx.yy contract.

## Deferred from: quick-dev spec-header-port-names-uppercase (2026-08-02)

- source_spec: `_bmad-output/implementation-artifacts/spec-header-port-names-uppercase.md`
  summary: Fixed-width header port comboboxes (and popups sized from that width) can clip long device/channel names harder once labels are uppercase.
  evidence: Blind Hunter; pre-existing 112px port combo + no ellipsis; uppercase may widen glyphs slightly — layout/tooltip work is out of scope for this display-only change.

## Deferred from: quick-dev spec-midi-synth-detection-retry (2026-08-02)

- source_spec: `_bmad-output/implementation-artifacts/spec-midi-synth-detection-retry.md`
  summary: Failed presence retries rewrite lastError / warning logs on every Device Inquiry timeout while the synth stays absent.
  evidence: Blind Hunter; finishAsyncDeviceInquiryFailure path pre-existed; presence timer makes the write cadence more frequent.

- source_spec: `_bmad-output/implementation-artifacts/spec-midi-synth-detection-retry.md`
  summary: USB/interface reappear under a new OS device identifier leaves persisted From/To ids stale until the user reselects ports.
  evidence: Blind Hunter; port-id persistence mismatch is pre-existing host/driver behavior, not introduced by presence retry.

- source_spec: `_bmad-output/implementation-artifacts/spec-midi-synth-detection-retry.md`
  summary: No automated coverage for presence timer interval switching, busy-skip, or OS list-change sync path.
  evidence: Blind Hunter; only DeviceInquiryTrigger force-predicate unit tests were in scope.

- source_spec: `_bmad-output/implementation-artifacts/spec-midi-synth-detection-retry.md`
  summary: Legacy blocking waitForSysExResponse paths are outside asyncSysExCaptureActive_ and could still race a presence tick.
  evidence: Edge Case Hunter; async Device Inquiry / patch paths are gated; remaining sync wait usage is pre-existing and rare on the message thread.

## Deferred from: quick-dev spec-fix-github-actions-quality-gate (2026-08-02)

- source_spec: `_bmad-output/implementation-artifacts/spec-fix-github-actions-quality-gate.md`
  summary: No regression test that Scripts/quality/ stays un-ignored (e.g. git check-ignore).
  evidence: Blind Hunter; future unanchored Quality/ rule could silently re-break tracking on ignorecase=true.

- source_spec: `_bmad-output/implementation-artifacts/spec-fix-github-actions-quality-gate.md`
  summary: requirements.txt uses a floating lizard pin (>=1.17.10,<2) that can drift CI metrics across minor releases.
  evidence: Blind Hunter; pin policy is product/ops preference outside this green-CI fix.

- source_spec: `_bmad-output/implementation-artifacts/spec-fix-github-actions-quality-gate.md`
  summary: Nesting scan strips only // comments, not block comments or brace literals — possible false positives.
  evidence: Blind Hunter; pre-existing lint_touched.py behavior, not introduced by tracking/CI wiring.

- source_spec: `_bmad-output/implementation-artifacts/spec-fix-github-actions-quality-gate.md`
  summary: Path gating uses startswith Source/Tests without normalizing backslashes for unusual Windows git output.
  evidence: Blind Hunter; pre-existing; CI runners currently use forward-slash paths.

- source_spec: `_bmad-output/implementation-artifacts/spec-fix-github-actions-quality-gate.md`
  summary: Exact needs list order equality is brittle to harmless YAML reorders; no assert quality-gate stays parallel to release-script-tests.
  evidence: Blind Hunter; acceptable contract strictness for now.

- source_spec: `_bmad-output/implementation-artifacts/spec-fix-github-actions-quality-gate.md`
  summary: Tests do not lock fetch-depth:0 on quality-gate checkout; shallow history can still starve three-dot diffs.
  evidence: Blind Hunter; workflow already sets fetch-depth:0; hardening the test lock is optional.

## Deferred from: quick-dev spec-device-unresponsive-presence-sysex-brake (2026-08-02)

- source_spec: `_bmad-output/implementation-artifacts/spec-device-unresponsive-presence-sysex-brake.md`
  summary: Non–Device-ID SysEx during inquiry re-arms capture without rescheduling the timeout, so a noisy wire can still trip unresponsive.
  evidence: Edge Case Hunter; pre-existing inquiry re-arm path; worsened only by soft-abort consequence.

- source_spec: `_bmad-output/implementation-artifacts/spec-device-unresponsive-presence-sysex-brake.md`
  summary: No MidiManager unit fixture for soft timeout while detected → flag set → clear on success.
  evidence: Blind Hunter; timer/async capture hard to unit without deeper fakes.

- source_spec: `_bmad-output/implementation-artifacts/spec-device-unresponsive-presence-sysex-brake.md`
  summary: Host automation can still mutate APVTS while UI is locked; MIDI is dropped so plugin/synth can desync.
  evidence: Blind Hunter / Edge Case Hunter; same pattern as Compare lock today.

- source_spec: `_bmad-output/implementation-artifacts/spec-device-unresponsive-presence-sysex-brake.md`
  summary: Unsupported-but-detected + timeout prefers unresponsive overload footer over unsupported copy.
  evidence: Edge Case Hunter; rare Unknown Matrix-family path.

## Deferred from: quick-dev spec-user-message-ascii-encoding (2026-08-02)

- source_spec: `_bmad-output/implementation-artifacts/spec-user-message-ascii-encoding.md`
  summary: Stale Compare footer wording with em dash remains in mutator export/compare implementation artifact.
  evidence: Blind Hunter; documentation drift, not runtime; outside this bugfix code path.

## Deferred from: quick-dev spec-dirty-unsaved-patch-ux-chantier-1 (2026-08-03)

- source_spec: `_bmad-output/implementation-artifacts/spec-dirty-unsaved-patch-ux-chantier-1.md`
  summary: `patchNotStoredInRam_` is not persisted across plugin/session reload after INIT without STORE.
  evidence: Edge Case Hunter; chantier 3 / session close is out of scope; in-memory flag is enough for live leave prompts.

- source_spec: `_bmad-output/implementation-artifacts/spec-dirty-unsaved-patch-ux-chantier-1.md`
  summary: No PluginProcessor-level test that leave-after-INIT (clean + notStored) blocks via real policy+modal choice enum.
  evidence: Blind Hunter; handler harness still mocks bool `confirmPatchContextChange`; policy unit-tested separately.

- source_spec: `_bmad-output/implementation-artifacts/spec-dirty-unsaved-patch-ux-chantier-1.md`
  summary: Persist Store while MIDI outbound blocked aborts leave with no dedicated footer beyond existing STORE silence.
  evidence: Blind/Edge Hunter; tryPersist returns false and stays; footer polish can wait unless real-world confusion appears.

- source_spec: `_bmad-output/implementation-artifacts/spec-dirty-unsaved-patch-ux-chantier-1.md`
  summary: If Persist succeeds then Mutator history Cancel aborts navigation, the patch may already be STORED/saved while the user remains on the slot.
  evidence: Edge Case Hunter; acceptable (work is safe); deferring Persist until both gates would change gate ordering semantics.

## Deferred from: quick-dev spec-mt4-bridge-presence-device-inquiry (2026-08-06)

- source_spec: `_bmad-output/implementation-artifacts/spec-mt4-bridge-presence-device-inquiry.md`
  summary: Bridge teVirtualMIDI merged bidirectional `MT4 Port N` should move to real separate Input/Output virtual endpoints (`MT4 Input N` / `MT4 Output N`) — not rename-only — to match Scarlett-style non-echo wiring for the Emagic community.
  evidence: Lab presence flap with MIDI-OX still seeing identity replies; merged create was chosen to dodge identical IN/OUT name collisions; Matrix-Control capture filter is the first fix; Bridge port-model rework is sequenced next in unitor-win64-driver.

- source_spec: `_bmad-output/implementation-artifacts/spec-mt4-bridge-presence-device-inquiry.md`
  summary: Async patch one-shot still re-arms via callAsync after non-patch SysEx (same steal race pattern as Device Inquiry had).
  evidence: MidiManagerAsyncPatch.cpp armAsyncSinglePatchCapture; out of presence-flap scope; same optional filter pattern could apply later.

## Deferred from: quick-dev spec-dirty-unsaved-patch-ux-chantier-3 (2026-08-22)

- source_spec: `_bmad-output/implementation-artifacts/spec-dirty-unsaved-patch-ux-chantier-3.md`
  summary: Plugin editor close in a DAW host has no reliable JUCE pre-destroy hook; at-risk close warning is not shown when the host tears down the editor without going through Standalone quit/close paths.
  evidence: PluginEditor destructor clears modal gates before teardown; AudioProcessorEditor has no portable closeButtonPressed equivalent in plugin mode.

- source_spec: `_bmad-output/implementation-artifacts/spec-dirty-unsaved-patch-ux-chantier-3.md`
  summary: DAW project quit, plugin unload, and `getStateInformation` cannot show a blocking FR-51-family modal (no message-thread UI during processor teardown).
  evidence: JUCE host lifecycle; `releaseResources` / destructor paths are unsuitable for AlertWindow; best-effort coverage is Standalone-only.

- source_spec: `_bmad-output/implementation-artifacts/spec-dirty-unsaved-patch-ux-chantier-3.md`
  summary: `patchNotStoredInRam_` is still not persisted in session state — reloading a DAW project after INIT-without-STORE will not re-prompt until the user edits or INIT again.
  evidence: Chantier-1 deferred-work retained; chantier 3 uses live in-memory risk only at close hooks.

## Deferred from: build spec-patch-manager-nav-debounce (2026-08-23)

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-manager-nav-debounce.md`
  summary: No settleInProgress guard if the user clicks Next/Previous again while the unsaved-edit confirm modal is open during Internal settle.
  evidence: Edge Case Hunter; confirm is typically a blocking modal, nested message-loop re-entry is rare; revisit if UAT sees nested navigations during the dialog.

## Deferred from: code review of spec-12-1-spike-undomanager-on-apvts-with-one-slider-and-midi-proof (2026-08-24)

- Gestes clavier / double-clic sans `ScopedDragNotification` sur `TSS::Slider` — story 12-4 couvre le groupement flèches clavier.

- Pas de test deux drags consécutifs ni invalidation redo — hors périmètre spike ; à couvrir quand la pile undo sera complète.

- Risque pollution undo par écritures programmatiques APVTS après branchement UndoManager — pattern `nullptr` déjà en place ; audit complet prévu story 12-5.

- Helper `sysExMatchesRemoteEdit` dupliqué entre spike tests et `PatchParameterSysExDispatcherTests` — dette test mineure, extraction partagée optionnelle.

## Deferred from: build spec-cap-3-undo-redo-keyboard-shortcuts (2026-08-24)

- source_spec: `_bmad-output/implementation-artifacts/spec-cap-3-undo-redo-keyboard-shortcuts.md`
  summary: No automated assertion that PluginEditor::tryHandleEditorialUndoRedoKey calls classifyEditorialUndoRedoShortcut — helper tests alone stay green if the editor reverts to getTextCharacter matching.
  evidence: Verification Gap Reviewer; EditorialUndoRedoShortcutTests cover the pure helper only; PluginEditor keyPressed harness still deferred from story 12-4.

- source_spec: `_bmad-output/implementation-artifacts/spec-cap-3-undo-redo-keyboard-shortcuts.md`
  summary: No automated check that JucePlugin_EditorRequiresKeyboardFocus stays 1 after configure — hosted Cmd+Z delivery can regress with a green unit-test run.
  evidence: Verification Gap Reviewer; currently verified by inspecting generated Defs.txt and manual Live UAT.

- source_spec: `_bmad-output/implementation-artifacts/spec-cap-3-undo-redo-keyboard-shortcuts.md`
  summary: Escape overlay blocking list is wider than the editorial-undo modal guard (e.g. Settings / native alerts / popups) — pre-existing asymmetry.
  evidence: Blind Hunter; undo modal guard unchanged in CAP-3; revisit only if UAT shows Cmd+Z leaking through overlays Escape already treats as blocking.

## Deferred from: build spec-vcf-vca-dco-mix-balance-ui (2026-08-27)

- source_spec: `_bmad-output/implementation-artifacts/spec-vcf-vca-dco-mix-balance-ui.md`
  summary: Verify the longer "DCO 2 | DCO 1 MIX" ParameterCell label fits without clipping at current Patch Edit label width.
  evidence: Blind Hunter; Label uses drawText without ellipsis; UAT visual check preferred over layout redesign in this oneshot.

- source_spec: `_bmad-output/implementation-artifacts/spec-vcf-vca-dco-mix-balance-ui.md`
  summary: User manual VCF/VCA bullet still says generic "balance des oscillateurs" without the new on-screen label.
  evidence: Blind Hunter; Documentation/User/manuel-utilisateur.md; docs pass optional after UAT.

- source_spec: `_bmad-output/implementation-artifacts/spec-vcf-vca-dco-mix-balance-ui.md`
  summary: ~~No automated or TestSliders harness coverage for reverseHorizontalUi fill and left/right key mapping.~~ **Superseded 2026-08-27**: reverseHorizontalUi removed after UAT; gesture stayed normal.

- source_spec: `_bmad-output/implementation-artifacts/spec-vcf-vca-dco-mix-balance-ui.md`
  summary: Parameter IDs remain kBalance / vcfVcaBalance while UI and Oberheim docs say Mix — rename would touch SysEx maps and automation IDs.
  evidence: Blind Hunter; intentional keep for this UI-only change; unreleased but still a wider chore.

## Deferred from: spec-patch-mutator-playable-calibration (2026-08-29)

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-playable-calibration.md`
  summary: Family-gesture coupled moves during jitter (envelope contour / Frequency+Resonance as a family) remain post-pass only.
  evidence: Addendum Must track #2; Blind Hunter — Frequency+Resonance rescue is post-apply, not coordinated during jitter.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-playable-calibration.md`
  summary: Kindred LFO “rates that breathe” soft-avoid (very slow + deep toward volume) not implemented.
  evidence: Addendum Must track #4 / A6.5 LFO soft guidance; no LFO Speed/Amplitude Kindred policy.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-playable-calibration.md`
  summary: Hard Amp floor when an LFO is the sole VCA opener (MM relay + dead VCA2←ENV2) not implemented.
  evidence: Addendum A6.5 LFO escalation; only ENV1/ENV3 risk-source floors exist today.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-playable-calibration.md`
  summary: Sync-specific CHOIX that widens toward Wild still uses generic MODE choice-step limits.
  evidence: Addendum A7.1 Sync freer toward Wild; Blind Hunter.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-playable-calibration.md`
  summary: MM risk Amount clamp may over-limit intentional strong positives on risk destinations.
  evidence: Addendum focuses on unfavorable (esp. negative) Amounts; current clamp is a hard window — Ask First retune.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-playable-calibration.md`
  summary: Legacy Amount/Random APVTS descriptors remain for migration while MODE/PITCH drive MUTATE.
  evidence: Spec Always allows migration leftovers; Blind Hunter noted host-automation surface — follow-on cleanup.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-playable-calibration.md`
  summary: Should items not shipped — guard indicator, dead→playable golden, Unison/Keyboard LOCK through Warp, measured Noise inject.
  evidence: Spec Ask First / Should; deferred to keep Must shippable.

## Deferred from: review of spec-patch-mutator-playability-unit-tests.md (2026-08-30)

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-playability-unit-tests.md`
  summary: ROM corpus under Tests/Fixtures/Patches/ROM/ is still untracked and must be committed before CI can stay green.
  evidence: Suite loads 16 listed paths from that tree; clean checkout without those files fails corpus_romFixturesArePresent.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-playability-unit-tests.md`
  summary: After MM opener restore, wave-select cardinality and mix-vs-silent-DCO guards are not re-armed (only filter + MM role guards).
  evidence: Edge/blind review of finishMatrixModPass; corpus currently green but a restored MM→mix path could theoretically reopen a silence class.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-playability-unit-tests.md`
  summary: Oracle skips VCA2 floor when any live MM→VCA2 exists, without checking opener amount adequacy.
  evidence: Edge hunter on checkAmplitudePath / matrixModDrivesVca2Volume; production openers usually repair this, but oracle gap remains.

- source_spec: `_bmad-output/implementation-artifacts/spec-version-0-2-0-alpha-and-ci-green.md`
  summary: User manual still declares Version-Produit 0.1.2-alpha after product SSOT moved to 0.2.0-alpha.
  evidence: Documentation/User/manuel-utilisateur.md front matter not in this story's Always; docs drift surfaced in review.

- source_spec: `_bmad-output/implementation-artifacts/spec-version-0-2-0-alpha-and-ci-green.md`
  summary: Standalone getApplicationVersion may show bare CMake version (0.2.0) while About shows PluginVersion with alpha suffix.
  evidence: Dual version surfaces pre-exist; About path was the story scope only.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-nav-focus-and-bank-marker.md`
  summary: No automated GUI checks for placeholder `-`/`--`, bank badge marker, or red focus colours (manual UAT only).
  evidence: Verification Gap / Blind Hunter — Core tests assert APVTS only; NumberBox harness has no DisplayState assertions.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-nav-focus-and-bank-marker.md`
  summary: No unit test for PluginProcessor session/startup reset of established+focus, nor for NumberBox-accepted patch establish path.
  evidence: Verification Gap — resetInternalPatchCoordinatesToDefaults and applyAcceptedPatchNumberChange lack Tests/ coverage.

## Deferred from: code review of spec-patch-nav-focus-and-bank-marker.md (2026-09-04)

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-nav-focus-and-bank-marker.md`
  summary: Matrix-6/6R gray-empty bank box and establish/OPEN-without-bank paths have no dedicated unit coverage; Verification lists manual UAT only.
  evidence: Acceptance Auditor / Blind Hunter — new handler tests are Matrix-1000-only.

## Deferred from: review of spec-patch-mutator-flat-export.md (2026-09-05)

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-mutator-flat-export.md`
  summary: Direct `exportMutatorHistory` (non-session) does not clear leftover nested `Mxx/` or old `Initial.syx` when writing into a non-empty folder.
  evidence: Edge Case Hunter — GUI export uses session Overwrite/Keep which clears or creates fresh; leftover pollution only hits the direct API / unclean folder path.

## Deferred from: one-shot NumberBox edit border and font (2026-09-05)

- source_spec: `_bmad-output/implementation-artifacts/spec-numberbox-edit-border-and-font.md`
  summary: External setValue / setDisplayState / setFocusHighlight while the TextEditor is open can desync the field or leave focus colour intent unused.
  evidence: Pre-existing editor lifecycle; Blind Hunter — not introduced by the edit border/font change.

- source_spec: `_bmad-output/implementation-artifacts/spec-numberbox-edit-border-and-font.md`
  summary: Editor seeds unpadded currentValue_ while idle display uses zero-padded cachedValueText_.
  evidence: Pre-existing showEditor text path; more noticeable now that font size matches display.

- source_spec: `_bmad-output/implementation-artifacts/spec-numberbox-edit-border-and-font.md`
  summary: TestNumberBoxes has no double-click edit, border-over-editor, or font-parity coverage.
  evidence: Blind Hunter — harness only shows static scale/focus samples.

- source_spec: `_bmad-output/implementation-artifacts/spec-numberbox-edit-border-and-font.md`
  summary: System display scale change mid-edit (e.g. window moved between monitors) does not re-layout the editor until resize/scale API.
  evidence: Blind Hunter edge case; paint snaps stroke independently of editor bounds.

- source_spec: `_bmad-output/implementation-artifacts/spec-numberbox-edit-border-and-font.md`
  summary: TextEditor caret and remaining colour IDs are unset; default caret may clash with NumberBox chrome.
  evidence: Pre-existing showEditor colour setup; transparent outlines unchanged.

## Deferred from: review of spec-master-edit-hide-matrix-6.md (2026-09-05)

- source_spec: `_bmad-output/implementation-artifacts/spec-master-edit-hide-matrix-6.md`
  summary: PRD/epic FR-46 still describes MASTER EDIT as grayed on Matrix-6/6R while the UI now hides the column content.
  evidence: Spec Ask First blocked PRD wording updates; product docs and shipped UX will drift until a dedicated copy pass.

- source_spec: `_bmad-output/implementation-artifacts/spec-master-edit-hide-matrix-6.md`
  summary: MasterEditGate hide-predicate tests live in DeviceAudioInputPreferenceTests.cpp rather than a dedicated Core test file.
  evidence: Blind Hunter — mixed suites; functional coverage exists, file split is cleanup only.

## Deferred from: code review of spec-plugin-modals-always-on-top.md (2026-09-05)

- source_spec: `_bmad-output/implementation-artifacts/spec-plugin-modals-always-on-top.md`
  summary: No automated regression check that raise-before/after runs around FileChoosers and Mutator Delete; z-order stays manual-smoke only.
  evidence: Verification Gap review — dropping a raise would still leave build/lint/unit tests green; project GUI modality is validated manually.

## Deferred from: code review of spec-gui-lock-dimming-film-holes.md (2026-09-05)

- source_spec: `_bmad-output/implementation-artifacts/spec-gui-lock-dimming-film-holes.md`
  summary: Film does not continuously trap keyboard focus; Tab may still reach body controls under the dimming film after the initial giveAwayKeyboardFocus.
  evidence: Edge/Blind review — binders steal focus only on lock transitions; Mutator lockOnCompare=false; no ongoing focus filter for film-covered regions.

- source_spec: `_bmad-output/implementation-artifacts/spec-gui-lock-dimming-film-holes.md`
  summary: COMPARE hole may drift if Mutator re-layouts the button without a MainComponent resize or lock-property APVTS change (only a one-shot async retry covers empty bounds).
  evidence: Edge/Blind review — hole refresh is driven by shell resized + lock APVTS props, not Mutator-local layout callbacks.

- source_spec: `_bmad-output/implementation-artifacts/spec-gui-lock-dimming-film-holes.md`
  summary: No automated tests for MainComponent film attach, Header/Footer/COMPARE hole assembly, LockDimmingFilm::hitTest/paint wiring, or binder click-block cutover — only LockDimmingFilmPolicyTests (pure helpers).
  evidence: Verification Gap review (2026-09-05 second pass) — omitting attach or breaking component hitTest still leaves policy unit tests green.

## Deferred from: review of spec-bank-export-dot-filename-display.md (2026-09-05)

- source_spec: `_bmad-output/implementation-artifacts/spec-bank-export-dot-filename-display.md`
  summary: Sibling done-specs / deferred notes may still document Bank Utility export as `Pxx - {Name}` after the separator changed to `Pxx. {Name}`.
  evidence: Blind Hunter — `spec-bank-utility-import-export.md` and an orphan-file deferred entry still use the hyphen form. Factory fixtures were renamed to `Pxx.` (`353488f5`; Ask First resolved keep in code review 2026-09-05).

## Deferred from: code review of spec-bank-export-dot-filename-display.md (2026-09-05)

- No IMPORT backfill unit test for new `Pxx.` stems — parse SSOT covered; Bank Import only shares the parser.
- Hand-edited empty-after-dot stems (`P76.` / `P76. `) fall back to raw stem in `stemForFilenameReconcile` — export never emits these; empty-vs-nonmatch API ambiguity.
- Manual hardware smoke (EXPORT → Finder → OPEN FILE NAMES / combo / SYSEX NAMES) — scheduled next week; not a code-review blocker.

- source_spec: `_bmad-output/implementation-artifacts/spec-version-0-3-0-alpha.md`
  summary: User manual still declares Version-Produit 0.1.2-alpha after product SSOT moved to 0.3.0-alpha.
  evidence: Documentation/User/manuel-utilisateur.md front matter out of bump scope; prior 0.2.0 deferred entry remains and this bump widens the drift.

- source_spec: `_bmad-output/implementation-artifacts/spec-version-0-3-0-alpha.md`
  summary: CONTRIBUTING.md still uses v0.2.0-alpha as the worked alpha release example.
  evidence: Blind Hunter on version bump; example-only, not required for About/PluginVersion SSOT.

- source_spec: `_bmad-output/implementation-artifacts/spec-version-0-3-0-alpha.md`
  summary: Tracked empty CHANGELOG.md has no 0.3.0-alpha / Unreleased stub after the product version bump.
  evidence: Blind Hunter; changelog hygiene not in Intent; empty file pre-existed.

## Deferred from: oneshot review of spec-patch-nav-internal-header-reload-bank-reclick.md (2026-09-07)

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-nav-internal-header-reload-bank-reclick.md`
  summary: Sibling done spec still describes Internal header reclaim as device-dump/editor reload only, omitting Set Bank + Program Change recall.
  evidence: Blind Hunter; historical done artifact left untouched in this bugfix.

## Deferred from: spec-matrix-1000-silent-patch-stuck (2026-09-07)

- source_spec: `_bmad-output/implementation-artifacts/spec-matrix-1000-silent-patch-stuck.md`
  summary: MasterModel::setChoiceIndex still writes the raw CHANNEL menu index into byte 11 only; non-mapper callers can recreate the silent-Matrix bug.
  evidence: Intentional generic accessors; live APVTS path uses MidiChannelMasterCodec via ApvtsMasterMapper; remaining risk is tests or future call sites that bypass the codec.

- source_spec: `_bmad-output/implementation-artifacts/spec-matrix-1000-silent-patch-stuck.md`
  summary: MasterParameterSysExDispatcherTests still seeds midiChannel via MasterModel::setChoiceIndex without composing Omni/Mono.
  evidence: Pre-existing test fixture pattern outside this diff; production CHANNEL edits go through the mapper.

## Deferred from: code review of spec-syx-drag-drop-load.md (2026-09-07)

- source_spec: `_bmad-output/implementation-artifacts/spec-syx-drag-drop-load.md`
  summary: Multi-file drop reject (kDropRejectedMultiFile) lives only in PluginEditor with no automated test.
  evidence: handleSyxFilesDropped gates files.size()!=1 before Core; DropLoad harness cannot see StringArray without extracting a Core-visible helper; GUI entry remains manual per Core testing policy.

- source_spec: `_bmad-output/implementation-artifacts/spec-syx-drag-drop-load.md`
  summary: Successful .syx drop may briefly show Computer Patches scan-summary footer before load-success footer.
  evidence: scanAndPublishFolder publishes scan footer before loadSelectedPatchFileImmediately; same pattern as Open; Implementation Notes already call it out.

## Deferred from: code review of spec-syx-drag-drop-load.md (2026-09-08)

- source_spec: `_bmad-output/implementation-artifacts/spec-syx-drag-drop-load.md`
  summary: Reconfirmed — multi-file drop reject remains GUI-only with no automated coverage.
  evidence: Same as 2026-09-07 entry; re-review layers agreed defer (Core testing policy).

- source_spec: `_bmad-output/implementation-artifacts/spec-syx-drag-drop-load.md`
  summary: Reconfirmed — successful drop may flash scan-summary footer before load success.
  evidence: Same as 2026-09-07 entry; Open-parity; still deferred.

## Deferred from: spec-system-init-syx-filenames.md (2026-09-08)

- source_spec: `_bmad-output/implementation-artifacts/spec-system-init-syx-filenames.md`
  summary: User manual still does not spell the full OS Application Support path to the system Init/ folder.
  evidence: Product intent hides the folder; documenting the exact path is support/docs polish deferred from this Build.

- source_spec: `_bmad-output/implementation-artifacts/spec-system-init-syx-filenames.md`
  summary: Decision-log D-042 still mentions a stale “Reset all Master to default” mockup label wording.
  evidence: Chrome locked to UTILITY/INIT; D-034/D-035 updated this Build; D-042 hygiene left for a docs pass.

- source_spec: `_bmad-output/implementation-artifacts/spec-system-init-syx-filenames.md`
  summary: openMasterGlobalInitConfirmDialog duplicates openMasterInitConfirmDialog layout/focus wiring.
  evidence: Both work; shared helper would be a follow-up DRY cleanup, not required for acceptance.

## Deferred from: spec-settings-delete-init-template.md (2026-09-08)

- source_spec: `_bmad-output/implementation-artifacts/spec-settings-delete-init-template.md`
  summary: DONE (promoted to open Patch on Settings Init resume 2026-09-09) — SAVE AS INIT silent null-deps no-op is tracked as unchecked `[Review][Patch]` on both Settings Init specs (not deferred).
  evidence: Was deferred during DELETE-only pass; combined resume keeps it as actionable patch.

- source_spec: `_bmad-output/implementation-artifacts/spec-settings-delete-init-template.md`
  summary: Live fixed Init/ no-arg Writer/service resolve path is not covered by an AppData round-trip test.
  evidence: Pre-existing SAVE AS INIT verification gap; DELETE logic is covered via injectable tempDir overloads.

- source_spec: `_bmad-output/implementation-artifacts/spec-settings-delete-init-template.md`
  summary: Master LOAD via loadMasterFromUserFile has no unit test that a failed load leaves MasterModel bytes unchanged.
  evidence: Master UTILITY LOAD from prior SAVE AS INIT story; out of DELETE scope.

## Deferred from: code review of spec-system-init-syx-filenames.md + spec-settings-delete-init-template.md (2026-09-09)

- source_spec: `_bmad-output/implementation-artifacts/spec-system-init-syx-filenames.md`
  summary: DONE (superseded) — STORE-after-INIT name-required gate shipped in `spec-store-after-init-name-required.md`. Combined Settings Init review resumed 2026-09-09 (GPC B); Review Findings Decision→Defer “resume after PATCH NAME” closed as Decision→Defer→Done on both Settings Init specs.
  evidence: Former chantier-first reminder; product STORE gate implemented 2026-09-09; resume review refreshed findings.

- source_spec: `_bmad-output/implementation-artifacts/spec-system-init-syx-filenames.md`
  summary: Reconfirmed — live fixed Init/ no-arg Writer/service resolve path still lacks an AppData round-trip unit test.
  evidence: Combined review verification-gap; tempDir overloads cover I/O; construction supplier not asserted.

- source_spec: `_bmad-output/implementation-artifacts/spec-system-init-syx-filenames.md`
  summary: Reconfirmed — `loadMasterFromUserFile` failure path has no assert that MasterModel bytes stay unchanged.
  evidence: Combined review verification-gap; success round-trip covered only.

- source_spec: `_bmad-output/implementation-artifacts/spec-system-init-syx-filenames.md`
  summary: After Internal Patches INIT, Mutator export/history basename paths can still use `* INIT *` from `patchModel_->getName()` with no sentinel gate.
  evidence: Combined review Blind Hunter; frozen AC only gates Computer Patches Save/Save As.

- source_spec: `_bmad-output/implementation-artifacts/spec-system-init-syx-filenames.md`
  summary: Master UTILITY SAVE AS file chooser starts at process CWD (`Master.syx` under empty File), not Documents/app-data.
  evidence: Combined review Blind Hunter; AC requires user `.syx` chooser, not a specific start folder.

## Deferred from: spec-patch-name-display-modes.md (2026-09-09)

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-name-display-modes.md`
  summary: DONE (superseded) — STORE-after-INIT pending-action + name-required arm/cancel/success wired in `spec-store-after-init-name-required.md`. Audition SysEx suppressed on pending-STORE name commit (STORE `sendPatch` alone). Resume Settings Init combined review next.
  evidence: Former UI-only Build follow-up; product STORE gate implemented 2026-09-09.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-name-display-modes.md`
  summary: DONE (covered by STORE gate consumer) — name-required cancel/empty outcome aborts pending STORE (no SysEx); Core unit tests lock sentinel STORE gate + hook invoke; GUI cancel path remains manual / harness.
  evidence: Former verification-gap; STORE wiring Build asserts Core gate; Escape/blur GUI outcome still best checked manually or in TestComponent.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-name-display-modes.md`
  summary: DONE — Settings Init / DELETE Review Findings refreshed on resume 2026-09-09; Decision→Defer “resume after PATCH NAME” closed as Decision→Defer→Done.
  evidence: Blind Hunter cross-spec staleness; resume review completed the refresh.

- source_spec: `_bmad-output/implementation-artifacts/spec-patch-name-display-modes.md`
  summary: DONE — STORE wiring done; Settings Init combined review resumed 2026-09-09 (no further “implement modes first” gate).
  evidence: Blind Hunter overlapping deferred-work next-steps; historical reminder closed by resume.

## Deferred from: spec-store-after-init-name-required.md (2026-09-09)

- source_spec: `_bmad-output/implementation-artifacts/spec-store-after-init-name-required.md`
  summary: DONE — paused combined Settings Init code review resumed 2026-09-09 (GPC B); Review Findings refreshed; Decision→Defer PATCH NAME/STORE closed.
  evidence: GPC B / Build AC; STORE gate shipped; resume completed.

## Deferred from: code review resume of spec-system-init-syx-filenames.md + spec-settings-delete-init-template.md (2026-09-09)

- source_spec: `_bmad-output/implementation-artifacts/spec-system-init-syx-filenames.md`
  summary: Reconfirmed on resume — live fixed Init/ no-arg Writer/service resolve path still lacks an AppData round-trip unit test.
  evidence: Resume verification-gap; tempDir overloads cover I/O; no-arg path unpinned.

- source_spec: `_bmad-output/implementation-artifacts/spec-system-init-syx-filenames.md`
  summary: Reconfirmed on resume — `loadMasterFromUserFile` failure path has no assert that MasterModel bytes stay unchanged.
  evidence: Resume verification-gap; success round-trip covered only.

- source_spec: `_bmad-output/implementation-artifacts/spec-system-init-syx-filenames.md`
  summary: Reconfirmed on resume — Mutator export/history basename can still use sentinel `* INIT *` (out of Settings Init AC).
  evidence: Out of frozen Computer Patches Save/Save As scope; unchanged.

- source_spec: `_bmad-output/implementation-artifacts/spec-system-init-syx-filenames.md`
  summary: Reconfirmed on resume — Master UTILITY SAVE AS chooser starts at process CWD.
  evidence: UX polish; AC does not require Documents start folder.

- source_spec: `_bmad-output/implementation-artifacts/spec-store-after-init-name-required.md`
  summary: Automate `commitPatchNameRename(..., suppressAuditionSysEx)` against the MIDI outbound queue (true skips audition; false still auditions) — today only the bool policy helper is unit-tested.
  evidence: Verification-gap review 2026-09-09; invert suppress leaves helper/StoreSentinel green.

- source_spec: `_bmad-output/implementation-artifacts/spec-store-after-init-name-required.md`
  summary: Automate Editor pending-STORE outcome true → `executeInternalPatchStore` (and false → no STORE) beyond Core helpers; GUI Escape/blur remains manual/harness.
  evidence: Verification-gap review 2026-09-09; remove execute call leaves Core tests green.

- source_spec: `_bmad-output/implementation-artifacts/spec-store-after-init-name-required.md`
  summary: Optional Matrix-6/6R StoreSentinel assert that deferred rename+STORE does not double-send slot writes when audition would have used 0x01.
  evidence: Blind Hunter 2026-09-09; frozen one-write policy shared; M-1000 coverage present.

## Deferred from: code review of spec-store-after-init-name-required.md (2026-09-09)

- source_spec: `_bmad-output/implementation-artifacts/spec-store-after-init-name-required.md`
  summary: Automate `PatchNameDisplayPanel` interrupt cancel when `isNameRequiredArmed()` (APVTS property change / redirect), including drag-armed sessions that may not report `isEditing()` — today only production code; no unit observer.
  evidence: Verification-gap review 2026-09-09; revert `|| isNameRequiredArmed()` leaves CI green; GUI manual/harness same as Escape/blur.

## Deferred from: review of spec-master-edit-sysex-debounce.md (2026-09-09)

- source_spec: `_bmad-output/implementation-artifacts/spec-master-edit-sysex-debounce.md`
  summary: Strengthen Master Edit SysEx debounce tests to drive real PluginProcessor flush seam (not only the mirrored harness).
  evidence: Current unit tests mirror schedule/fire/cancel locally; restoring immediate dispatch or dropping processor cancel sites would still leave harness tests green. Spec AC allows harness + sync flush; Patch Manager/Mutator use real-object flush seams.

- source_spec: `_bmad-output/implementation-artifacts/spec-master-edit-sysex-debounce.md`
  summary: Add Master-aware coverage to editorial undo/redo MIDI tests for cancelPending of Master debounce.
  evidence: performEditorialUndo/Redo cancel Master debounce beside Matrix Mod; UndoManagerEditorialUndoMidiTests has no Master enqueue assertion, so deleting those cancel calls would not fail CI.

## Deferred from: code review of spec-master-edit-sysex-debounce.md (2026-09-10)

- source_spec: `_bmad-output/implementation-artifacts/spec-master-edit-sysex-debounce.md`
  summary: RECONFIRMED — strengthen Master Edit SysEx debounce tests to drive real PluginProcessor flush seam (not only the mirrored harness). See 2026-09-09 entry above.
  evidence: Blind Hunter + Verification Gap + Acceptance Auditor on commit 297a3e76; `flushMasterEditSysExDebouncerForTests` still unused by tests.

- source_spec: `_bmad-output/implementation-artifacts/spec-master-edit-sysex-debounce.md`
  summary: RECONFIRMED — Master-aware editorial undo/redo MIDI cancel coverage for Master debounce. See 2026-09-09 entry above.
  evidence: Blind Hunter + Verification Gap + Acceptance Auditor on commit 297a3e76; production cancel sites present, UndoManagerEditorialUndoMidiTests still Matrix-Mod-only.

## Deferred from: build review of spec-computer-patches-multi-drop-virtual-list.md (2026-09-10)

- source_spec: `_bmad-output/implementation-artifacts/spec-computer-patches-multi-drop-virtual-list.md`
  summary: PATCH NAME drag overlay for multi/folder/junk selections has no automated coverage (manual Finder check only).
  evidence: Verification-gap review; repo convention and spec Verification mark GUI blink/overlay as manual; Core drop matrix is unit-tested.

## Deferred from: code review of spec-computer-patches-multi-drop-virtual-list.md (2026-09-10)

- source_spec: `_bmad-output/implementation-artifacts/spec-computer-patches-multi-drop-virtual-list.md`
  summary: Sibling `spec-syx-drag-drop-load.md` still documents multi-file reject (“one .syx at a time”) after this feature accepts multi/folder drops.
  evidence: Blind Hunter; other-spec SSOT hygiene — do not edit under this review’s patch loop.
- source_spec: `_bmad-output/implementation-artifacts/spec-computer-patches-multi-drop-virtual-list.md`
  summary: PATCH NAME drag overlay / editor accept path still has no automated coverage (reconfirmed).
  evidence: Verification Gap; GUI convention + manual Finder; prior deferred entry remains valid.
- source_spec: `_bmad-output/implementation-artifacts/spec-computer-patches-multi-drop-virtual-list.md`
  summary: Footer Loaded/Saved path-style middle truncate is untested in paint; APVTS asserts see full strings only.
  evidence: Verification Gap; GUI paint; spec Verification already lists manual Loaded/Saved readability.
- source_spec: `_bmad-output/implementation-artifacts/spec-computer-patches-multi-drop-virtual-list.md`
  summary: No DropLoad-level test for multi-folder-only drops (two+ folders, no loose `.syx`).
  evidence: Blind Hunter; coverage gap — mix and multi-file covered.
- source_spec: `_bmad-output/implementation-artifacts/spec-computer-patches-multi-drop-virtual-list.md`
  summary: No automated lock that Save As default folder prefers last real `kFolderPath` while a virtual list is active.
  evidence: Blind Hunter; `resolveDefaultSaveFolder` already prefers persisted folder — coverage only.

## Deferred from: code review (post-patches) of spec-computer-patches-multi-drop-virtual-list.md (2026-09-10)

- source_spec: `_bmad-output/implementation-artifacts/spec-computer-patches-multi-drop-virtual-list.md`
  summary: Full-path Loaded/Saved footer strings are asserted via the same `formatReadablePatchLocation` helper used in production (tautological Core check).
  evidence: Verification Gap on post-patch re-review; paint middle-truncate already deferred; optional later assert on `getFullPathName()` without the helper.
- source_spec: `_bmad-output/implementation-artifacts/spec-computer-patches-multi-drop-virtual-list.md`
  summary: Plural junk drag overlay `BAD FILES` still has no automated coverage (reconfirmed after post-patch re-review).
  evidence: Verification Gap; GUI convention + manual Finder; prior deferred entries remain valid.
