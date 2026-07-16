# Deferred Work

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

- **Duplicated `CustomScrollBar` in `HierarchicalPopupMenu`** — ~copy of `ScrollablePopupMenu` thumb/timer/wheel math; story 6-15 allowed reuse patterns without full U-13 DRY. Extract shared helper in U-13.
- **Permanent 20 Hz scrollbar timers while hierarchical popup is open** — inherited from ScrollablePopupMenu CustomScrollBar pattern (`startTimerHz(20)` + idle `repaint`); fix with shared scroll infra in U-13.

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

- **Footer ROM persiste après retour banque RAM** (`InternalPatchesPanel.cpp:316`) — pas de clear `uiMessageText`/`uiMessageSeverity` quand `romPasteStoreBlocked_` repasse à false ; polish UX mineur.
- **Hover footer absent si curseur déjà sur bouton au blocage ROM** (`InternalPatchesPanel.cpp:134`) — `mouseEnter` ne fire pas sans mouvement souris lors du passage banque→ROM.
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

- **`BankUtilityPanel` alpha graying vs `Button` disabled colours** (`BankUtilityPanel.cpp:69-107`) — Story 8.5 introduced `setBankUtilityGrayed`: buttons stay **`setEnabled(true)`** + `setAlpha(0.5f)` so clicks still reach `onClick` and show the Matrix-1000-only footer (8-5 review: « Footer unreachable on grayed bank buttons »). Story 5.2 fixed `TSS::Button` to paint skin `*Disabled` colours when `setEnabled(false)` — Paste graying now uses that path. **Follow-up:** replace alpha hack with a shared « visually disabled but clickable » API on `Button` (or apply disabled look without disabling hit-testing); align `GroupLabel` / `ModuleHeader` alpha in the same panel. **Candidate story:** bundle into `u-8-patch-manager-panels-layout-audit` or a small Epic U hygiene story after 5.2 review.

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

- **Combo/backend mismatch si `MidiManager::set*Port` échoue** (`PluginProcessor.cpp:249-268`) — pattern brownfield pre-existing ; combo avance, APVTS inchangé jusqu’au prochain succès.
- **Double forwarding host buffer en standalone** (`PluginProcessor.cpp:210-218`) — AC #7 assume buffer hôte vide ; edge case host injectant MIDI en standalone.
- **MIDI FROM + KEYBOARD FROM même device** (`HeaderPanel` / `PluginProcessor`) — deux `juce::MidiInput` sur un identifiant ; pas de garde UI dans scope UAT slice.
- **`setStateInformation` sans resync ports/combos** (`PluginProcessor.cpp:233-246`) — restore ports uniquement au ctor `PluginEditor` ; reload session complète OK.
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
  summary: Hard-coded 50 ms settle / 500 ms queue-idle timeouts for device dump may be wrong for slow MIDI interfaces.
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

## Resolved during code review 6-13 (2026-07-16)

- Device dump no longer blocks the message thread (`requestSinglePatchAsync` + timers).
- Dump failure / no device keeps Mutator history (clear only after successful dump).
- Async one-shot SysEx capture ignores non-patch frames and keeps listening until timeout.
- History-gate modal refused when not on the message thread (`jassert` + cancel).

