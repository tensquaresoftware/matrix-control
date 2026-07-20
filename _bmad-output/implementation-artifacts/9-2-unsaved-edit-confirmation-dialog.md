---
organization: Ten Square Software
project: Matrix-Control
title: Story 9.2 — Unsaved Edit Confirmation Dialog
author: BMad Agent
status: done
baseline_commit: ba6f84237fb333c3dc8a75fb0b4569972cd506ae
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - implementation-artifacts/epic-9-context.md
  - implementation-artifacts/spec-9-1-dirtypatchtracker.md
  - implementation-artifacts/deferred-work.md
  - implementation-artifacts/7-7-settings-page-consolidation.md
  - project-context.md
created: 2026-07-19
updated: 2026-07-20
---

# Story 9.2: Unsaved Edit Confirmation Dialog

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

## Story

As a sound designer,
I want a confirmation when navigating away with unsaved edits,
so that I do not lose work accidentally (FR-51).

## Brownfield Reality (Read First)

**Tracker exists; gates and snapshot refresh do not.** Story 9.1 shipped `DirtyPatchTracker` owned by `PluginProcessor`, with unit tests, but **no public getter**, **no production `captureSnapshot`**, and **no navigation gate**. Mutator history already uses `confirmPatchContextChange` → `confirmPatchContextChangeGate` → PluginEditor `AlertWindow`. Settings Policies today only expose name-reconciliation — unsaved-warn policy IDs are missing.

| Deliverable | Status at 9.1 close (`458c76e`) |
|-------------|----------------------------------|
| `DirtyPatchTracker` API (`capture` / `clear` / `isDirty` / `syncApvtsAndIsDirty`) | **Exists** — Core-only |
| `PluginProcessor::dirtyPatchTracker_` ownership | **Exists** — private; no getter |
| Live capture after load / SAVE / STORE | **Missing** |
| FR-51 Cancel/Continue modal | **Missing** |
| Settings always/never warn policy | **Missing** (7.7 deferred to Epic 9) |
| Dirty gate on internal + Computer navigation + OPEN | **Missing** |
| Cancel revert for combo / OPEN selection | **Missing** (known defect in deferred-work) |
| Mutator history gate (`confirmPatchContextChangeGate`) | **Exists** — history-only today |

**This story wires FR-51 end-to-end: snapshot refresh + Settings policy + modal + navigation gates. It does not reinvent dirty compare, Mutator history UX, or Epic 8 device lock.**

## Acceptance Criteria

1. **Given** Story 9.1 tracker and Epic 7 `PatchManagerActionHandler` navigation  
   **When** the current PATCH is dirty (APVTS/packed ≠ last snapshot) and the user navigates to a **different** internal patch (Prev/Next, patch NumberBox, bank button) or Computer Patches file (combobox, Prev/Next FR-52, or OPEN that would auto-load)  
   **And** Settings policy is **warn always** (default)  
   **Then** a modal warns that edits were **not stored to the synth** and **not saved as `.syx`**  
   **And** **Cancel** aborts the navigation and keeps the current edit context (selection / NumberBox / OPEN outcome must not leave a stale “new” selection with the old buffer)  
   **And** **Continue** discards unsaved edits and proceeds with the requested load.

2. **And** Settings exposes unsaved-warning policy (FR-40): **warn always** (default) | **never warn** (expert). Policy persists on `apvts.state` with other plugin prefs. When **never warn**, dirty navigation proceeds without the FR-51 modal.

3. **And** after successful internal device dump load, Computer file load, SAVE / SAVE AS, and STORE, the tracker baseline is refreshed so the patch is clean (sync APVTS→model via mapper + `PatchNameSyncer` **before** `captureSnapshot`).

4. **And** does **not** modal-block: bulk import name reconciliation (D-025), init template loads / Internal INIT (D-037), Mutator MUTATE/RETRY, or module paste. History gate behaviour for Mutator sessions remains; FR-51 is a separate Cancel/Continue dialog (no Export, no in-modal SAVE/STORE).

5. **And** when dirty Continue proceeds and Mutator history is non-empty, the existing history gate still runs afterward (order: **dirty first → history second**).

6. **And** Core ↛ GUI: no `AlertWindow` / dialog includes in Core or `DirtyPatchTracker`. Modal lives in PluginEditor via a processor gate callback (same pattern as history / Defrag / name reconciliation).

7. **And** `Matrix-Control_Tests` pass; macOS Debug build clean; English-only source; panels stay presentation-only (no tracker includes in GUI panels).

## Tasks / Subtasks

- [x] **T1 — Settings policy SSOT** (AC: #2, #7)
  - [x] Add `PluginIDs::Settings` property + enum (`kWarnAlways` default, `kNeverWarn`)
  - [x] Initialize property in `PluginProcessor` (mirror `initializeNameReconciliationPolicyProperty`)
  - [x] Add Policies combo in `SettingsPanel` + English display names
  - [x] Wire restore/onChange in `PluginEditor::restoreSettingsPanelFromState` / `wireSettingsPanel`

- [x] **T2 — Snapshot refresh (capture sites)** (AC: #3, #7)
  - [x] Expose tracker access from `PluginProcessor` (getter or handler injection — prefer injecting `DirtyPatchTracker*` into `PatchManagerActionHandler` like other services)
  - [x] After successful device dump apply, Computer `applyLoadedPatchToApvtsAndSynth`, `completeSuccessfulSave`, and STORE send path: sync APVTS→model then `captureSnapshot`
  - [x] Account for PatchModel name encode/decode asymmetry (deferred 9.1): capture **after** `bufferToApvts` / settled APVTS sync so false-dirty from hardware name round-trip is minimized (document residual risk if any)

- [x] **T3 — FR-51 modal gate (GUI + Core hook)** (AC: #1, #2, #4, #5, #6)
  - [x] Add English strings under `PluginDisplayNames` (title, body mentioning STORE + `.syx`, Cancel, Continue)
  - [x] Add modal gate setter on `PluginProcessor` (e.g. `setUnsavedEditConfirmModalGate`) returning Cancel/Continue; PluginEditor wires `AlertWindow` with two buttons (match history-gate style; project already has `JUCE_MODAL_LOOPS_PERMITTED=1`)
  - [x] Extend `confirmPatchContextChangeGate`: if dirty and policy=always → show FR-51 modal; Cancel → false; Continue → fall through to existing Mutator history gate; if policy=never or clean → history gate only
  - [x] Message-thread guard (`jassertfalse` + refuse) like history gate — do **not** migrate all modals to `showAsync` in this story

- [x] **T4 — Navigation Cancel revert + OPEN** (AC: #1)
  - [x] Computer file load Cancel: revert `kSelectPatchFile` to previous id (fix known combo/OPEN cancel defect)
  - [x] OPEN: do not leave first-file selection committed if user Cancels dirty/history gate — gate before forcing load commit, or revert selection + avoid orphan auto-load
  - [x] Patch NumberBox Cancel path already reverts `lastKnownPatchNumber_` — verify it still works with chained dirty gate
  - [x] Internal Prev/Next / bank: abort before dump request (gate already before load — verify)

- [x] **T5 — Tests + verification** (AC: #3, #7)
  - [x] Unit tests: policy skip (never warn → no modal callback / proceed); dirty+always → Cancel aborts; Continue proceeds; capture after save/load leaves clean; optional handler-level Cancel revert for selection id if extractable without GUI harness
  - [x] Build `Matrix-Control_Tests`; smoke Standalone: edit → navigate → Cancel stays; Continue loads; Settings never-warn skips dialog; SAVE then navigate stays silent when clean

### Review Findings

- [x] [Review][Patch] OPEN Cancel still leaves new-folder browser with old buffer [`PatchManagerActionHandler.cpp:370-406`] — Decision 1 (2026-07-20): fix before validating 9-2; restore prior folder+scan+selection on Cancel via `pendingBrowserRestoreOnCancel_` + `abortComputerPatchesNavigation`.
- [x] [Review][Patch] Computer Prev/Next Cancel re-triggers forced load (second FR-51/history prompt or silent reload that discards kept edits) [`PatchManagerActionHandler.cpp:194-205`] — force load only when intended `nextId == beforeId` (single-file wrap), not after Cancel revert.
- [x] [Review][Patch] Computer load `kFailed` does not revert selection id [`PatchManagerActionHandler.cpp:478-481`] — `abortComputerPatchesNavigation` on resolve failure.
- [x] [Review][Patch] STORE always `captureCleanSnapshot` after `sendPatch` with no outbound/success guard [`PatchManagerActionHandler.cpp:366-367`] — capture only when `isEditorOutboundAllowed()`.
- [x] [Review][Patch] Restore Settings unsaved policy without clamping unknown ids [`PluginEditor.cpp` restoreSettingsPanelFromState] — clamp to default warn-always.
- [x] [Review][Patch] Add unit coverage for Computer Prev/Next Cancel after selection revert [`PatchManagerActionHandlerTests.cpp`] — plus OPEN Cancel browser restore and blocked-STORE keeps dirty.
- [x] [Review][Defer] Device Continue then dump unavailable/fail leaves advanced coordinates with discarded-intent edits [`PatchManagerActionHandler.cpp` loadCurrentPatchFromDevice] — deferred, pre-existing async navigation
- [x] [Review][Defer] Edits between dirty Continue and async dump completion overwritten then marked clean — deferred, pre-existing settle window
- [x] [Review][Defer] PASTE still pushes APVTS with null PatchNameSyncer (name/dirty skew) — deferred, pre-existing; FR-51 intentionally skipped for PASTE
- [x] [Review][Defer] Name-reconciliation Cancel after dirty Continue + history Discard cannot restore Mutator history — deferred, pre-existing gate ordering side effect
- [x] [Review][Defer] `lastCommittedComputerPatchesSelectedId_` starts at 0 until first successful load — deferred, edge bootstrap
- [x] [Review][Defer] INIT captures clean even on fallback/invalid template path — deferred, intentional baseline-after-init for FR-51 smoke
- [x] [Review][Defer] Broader harness gaps (OPEN Cancel folder restore, NumberBox+dirty chain, never-warn vs real modal) — deferred, beyond minimal regression test for Prev/Next Cancel

## Dev Notes

### Epic context

Epic 9 = FR-51 unsaved protection. 9.1 = tracker only. 9.2 = modal + Settings + live wiring. Upstream: Epic 4 file library, Epic 7 ActionDispatcher / Patch Manager handlers. [Source: `epic-9-context.md`, `epics.md` Epic 9]

### Locked product / technical decisions (do not re-litigate)

| Topic | Decision | Why |
|-------|----------|-----|
| Dialog buttons | **Cancel** / **Continue** only — no SAVE/STORE in modal | Product intent in deferred-work (OPEN FR-51 note) |
| Dialog UI | `AlertWindow` via PluginEditor gate (like history gate) | Consistency; skinned `MasterInitConfirmDialog` reserved for MASTER INIT |
| Gate composition | Chain inside `confirmPatchContextChangeGate`: dirty → history | One hook already covers NumberBox + handler call sites |
| Settings policy | Ship in **this** story | FR-51 / FR-40 / epic AC require it; 7.7 deferred UI here |
| INIT / PASTE / MUTATE | **No** FR-51 modal | D-074 / D-037 / epic out-of-scope list |
| Gate order | Dirty first, then Mutator history | Distinct loss vectors; keep history Export/Discard UX |
| Async modal migration | **Out of scope** | Existing gates use `runModalLoop`; deferred-work already tracks redesign |

### Current code to extend (UPDATE, do not fork)

**DirtyPatchTracker** — lean API already correct:

```13:33:Source/Core/Services/DirtyPatchTracker.h
    class DirtyPatchTracker
    {
    public:
        // ...
        bool isDirty(const PatchModel& current) const;
        bool syncApvtsAndIsDirty(ApvtsPatchMapper& mapper,
                                 PatchNameSyncer& nameSyncer,
                                 PatchModel& model);
```

**History gate pattern** — replicate structure for unsaved edits:

- Hook: `ActionExecutionHooks::confirmPatchContextChange` → `PluginProcessor::confirmPatchContextChangeGate` (`PluginProcessor.cpp` ~862–908)
- Editor wire: `setMutatorHistoryGateModalGate` (`PluginEditor.cpp` ~164–184)
- Handler entry: `PatchManagerActionHandler::confirmPatchContextChange` before loads (~133, ~218, ~471, NumberBox via processor ~1597)

**Settings policy pattern** — copy name-reconciliation:

- IDs: `PluginIDs::Settings::kComputerPatchesNameReconciliationPolicy`
- Init: `initializeNameReconciliationPolicyProperty`
- UI: `SettingsPanel` Policies combo + `PluginEditor::restoreSettingsPanelFromState` / `wireSettingsPanel`

**OPEN gap today** — scan + force `kSelectPatchFile=1` then load; Cancel on gate does not revert (`PatchManagerActionHandler::handleOpenPatchFolder` ~362–398). Fix as part of T4.

**Capture sites (intended):**

| Success path | Function | Note |
|--------------|----------|------|
| Device dump | async callback after `loadFrom` + push to APVTS (~707–716) | Capture after settled model/APVTS |
| Computer load | `applyLoadedPatchToApvtsAndSynth` (~630–640) | After sync |
| SAVE / SAVE AS | `completeSuccessfulSave` (~749–754) | After name syncer `bufferToApvts` |
| STORE | `handleInternalPatchStore` (~339–360) | After `apvtsToBuffer` + successful send |

### Architecture compliance

- **Core ↛ GUI** — modal only in GUI; Core returns bool proceed/abort via injected gate.
- **Composition root** — processor owns tracker; handlers consult via pointer or gate.
- **Permitted modals** — FR-51 is explicitly allowed (with Defrag + MASTER init). No MIDI-path blocking spinners.
- **English-only** user-visible strings in `PluginDisplayNames`.
- **Do not** put tracker includes in Computer/Internal panels — handlers gate loads.

### Anti-patterns (will fail review)

1. Calling `isDirty(model)` without APVTS sync after UI edits → use `syncApvtsAndIsDirty` or mapper + name syncer first.
2. `captureSnapshot` on unsynced model after load/save → skewed baseline.
3. `AlertWindow` in Core / handlers.
4. Save-in-dialog or three-button Export-style FR-51 copy.
5. Blocking D-025 / INIT templates / MUTATE with dirty modal.
6. Gating inside async dump **callback** instead of before request.
7. Cancel without reverting Computer selection / OPEN first-file commit.
8. Exposing tracker to GUI panels or inventing a second lock framework (Epic 8 lock ≠ FR-51).
9. French in source / dialog strings.
10. Merging FR-51 copy into Mutator “Unsaved mutations” dialog.

### Previous story intelligence (9.1)

- Spec: `spec-9-1-dirtypatchtracker.md` (status done). Ask First left capture + gates to 9.2.
- Deferred: no getter / no live capture; sync-before-capture mandatory; name encode/decode false-dirty risk after hardware dump.
- Tests: `Tests/Unit/DirtyPatchTrackerTests.cpp` — extend or add gate/policy tests nearby; follow APVTS fixture patterns from 9.1.

### Git intelligence (recent)

- `458c76e` — Close 9.1 review; deferred name-encoding + capture wiring to 9.2
- `111f2fa` — APVTS name-edit coverage via `syncApvtsAndIsDirty`
- `984aefd` — Add DirtyPatchTracker Core service

### Latest tech notes (JUCE 8)

- Prefer async `AlertWindow::showAsync` for new designs; this repo already permits modal loops (`JUCE_MODAL_LOOPS_PERMITTED=1`) for history/Defrag/reconciliation. **Match existing Patch Manager gates** in 9.2; do not expand scope to rewrite all modals.
- Always assert message thread before `runModalLoop` (existing history gate pattern).

### Project structure notes

| Action | Path |
|--------|------|
| UPDATE | `Source/Core/PluginProcessor.{h,cpp}` |
| UPDATE | `Source/Core/Actions/PatchManagerActionHandler.{h,cpp}` |
| UPDATE | `Source/Core/Actions/ActionExecutionHooks.h` (only if documenting chained semantics; optional new hook not required) |
| UPDATE | `Source/GUI/PluginEditor.cpp` |
| UPDATE | `Source/GUI/Settings/SettingsPanel.{h,cpp}` |
| UPDATE | `Source/Shared/Definitions/PluginIDs.h` |
| UPDATE | `Source/Shared/Definitions/PluginDisplayNames.h` |
| UPDATE | `CMakeLists.txt` (only if new test `.cpp`) |
| UPDATE/NEW | `Tests/Unit/*` — policy/gate/capture coverage |
| KEEP | `Source/Core/Services/DirtyPatchTracker.*` — extend only if tiny helpers needed |
| AVOID | Mutator engine, CompareLockBinder rewrite, module clipboard handlers |

### Testing requirements

- Prefer Core/unit tests over GUI automation (no GUI harness for Cancel/Confirm in v1 — same as MASTER INIT).
- Cover: clean navigation silent; dirty+always Cancel; dirty+always Continue; never-warn proceeds; post-SAVE clean; no-snapshot still not dirty until first capture after load.
- Manual smoke: internal bank/patch nav, Computer combo + prev/next, OPEN with dirty buffer, Settings toggle.

### References

- [Source: `epics.md` — Epic 9 / Story 9.2]
- [Source: `prd.md` — FR-51, FR-40]
- [Source: `addendum.md` — D-074]
- [Source: `architecture.md` — DirtyPatchTracker, permitted modals, Core↛GUI]
- [Source: `epic-9-context.md`]
- [Source: `spec-9-1-dirtypatchtracker.md`]
- [Source: `deferred-work.md` — 9.1 capture/getter; OPEN Cancel/Continue product intent; combo Cancel defect]
- [Source: `project-context.md` — language, dependency direction, APVTS prefs]

## Dev Agent Record

### Agent Model Used

Composer (Cursor agent)

### Debug Log References

### Completion Notes List

- FR-51 end-to-end: Settings unsaved-warning policy (warn always default / never warn), PluginEditor Cancel/Continue AlertWindow, chained dirty→history gate.
- `DirtyPatchTracker*` injected into `PatchManagerActionHandler`; `captureCleanSnapshot` after device dump, Computer load, SAVE, and STORE (APVTS→model sync before capture to reduce name round-trip false-dirty).
- Navigation Cancel reverts Computer `kSelectPatchFile` via `lastCommittedComputerPatchesSelectedId_`; INIT/PASTE call gate with `includeUnsavedEditWarning=false` so FR-51 does not block them.
- Residual risk (deferred 9.1): hardware name encode/decode asymmetry may still produce rare false-dirty after dump if APVTS settling diverges; capture after settled sync minimizes this.
- Follow-up fix (smoke): no baseline after INIT/cold start meant `isDirty` stayed false — capture after INIT + startup/session restore baseline so INIT→edit→navigate shows FR-51.
- Unit tests extended; `Matrix-Control_Tests` and macOS Debug build green. Manual Standalone smoke recommended for dialog UX.

### File List

- Source/Core/Actions/ActionExecutionHooks.h
- Source/Core/Actions/PatchManagerActionHandler.cpp
- Source/Core/Actions/PatchManagerActionHandler.h
- Source/Core/PluginProcessor.cpp
- Source/Core/PluginProcessor.h
- Source/Core/Services/UnsavedEditWarningPolicy.h
- Source/GUI/PluginEditor.cpp
- Source/GUI/Settings/SettingsPanel.cpp
- Source/GUI/Settings/SettingsPanel.h
- Source/Shared/Definitions/PluginDisplayNames.h
- Source/Shared/Definitions/PluginIDs.h
- Tests/Unit/DirtyPatchTrackerTests.cpp
- Tests/Unit/PatchManagerActionHandlerTests.cpp
- _bmad-output/implementation-artifacts/9-2-unsaved-edit-confirmation-dialog.md
- _bmad-output/implementation-artifacts/sprint-status.yaml

### Change Log

- 2026-07-19: Implemented FR-51 unsaved-edit confirmation (Settings policy, snapshot capture sites, modal gate, Computer Cancel revert, unit tests); status → review.
- 2026-07-20: Code review patches — OPEN Cancel restores prior browser; Prev/Next Cancel no double-load; kFailed revert; STORE capture gated on outbound; Settings policy clamp; regression tests; status → done.

## Implementation Plan

- T1 Settings SSOT → T2 inject tracker + capture → T3 dirty→history gate + AlertWindow → T4 selection revert → T5 tests/build.
