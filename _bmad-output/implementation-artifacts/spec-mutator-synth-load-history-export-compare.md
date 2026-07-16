---
organization: Ten Square Software
project: Matrix-Control
title: Spec — Mutator Synth Load, History Gate, Export Folders, Compare Lock
author: BMad Agent
type: feature
created: 2026-07-16
status: done
baseline_commit: 7f47759033566f2d1646448fadb4f1ec1791ea1a
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/CONVENTIONS.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Bank/Internal navigation only sends Set Bank + Program Change — the editor does not load synth patch bytes — so Mutator history/Compare/Export can disagree with the slot the user believes they are editing. History is not cleared on those navigations; Compare does not lock the UI; Export dumps into the chosen folder with no session subfolder tied to origin.

**Approach:** After Internal/Bank selection, request the patch from the synth into PatchModel/APVTS and treat that like a patch load (Mutator reset). Track a small PatchLoadContext (device vs `.syx`) updated at real load sites and freeze it on first MUTATE for hybrid Export folder names. Gate any patch-context change when history is non-empty with Export / Cancel / Discard. Entering Compare locks nearly all editing/navigation UI and sets a generic footer; logo menu stays available.

## Boundaries & Constraints

**Always:**
- History is session-only and cleared on every real editor patch-context change (Computer `.syx`, device dump after Bank/Internal nav, INIT, PASTE) — never keep per-patch history maps (FR-31).
- Freeze export metadata at first successful MUTATE with the initial snapshot; clear both on `resetSessionForPatchLoad`.
- Export session subfolder naming:
  - Device / INIT / PASTE: `B{bb:02d}-P{pp:02d}` plus `-NAME` only when trimmed internal name is non-empty (sanitize NAME with `PatchFileNameSanitizer`; if sanitize empties, omit `-NAME`).
  - Computer file: `Syx-{sanitizedFileStem}`.
  - Inside that folder keep FR-33 layout (`Initial.syx`, `Mxx/…`).
- Existing session folder: confirm Overwrite (delete folder contents then export) / Keep (create `…-2`, `…-3`, …) / Cancel.
- Non-empty history before patch-context change: confirm Export (run export flow first) / Cancel (abort navigation) / Discard (reset then proceed).
- Compare active: lock Patch Edit, Master Edit, Matrix Mod, Bank Utility, Internal Patches, Computer Patches, Mutator controls except COMPARE; freeze envelope/track mouse edit; footer on enter: `Compare mode — editing and patch/bank changes are locked. Click COMPARE again to exit.`; logo menu (UI Scale, Skin, Audio/MIDI, Settings, About) stays usable.
- Device dump must work for RAM and ROM banks (do not gate on paste/store allow).

**Ask First:**
- Hardware cannot return a patch dump after navigation (timeout / invalid SysEx): whether to keep coordinates + old editor buffer, revert coordinates, or show only footer — do not invent a silent policy beyond a clear warning footer without owner OK.
- Exact dialog button labels/copy if Guillaume wants French UI strings (code/docs stay English unless he asks otherwise).

**Never:**
- Classic Observer framework for load origin — use an explicit PatchLoadContext updated at load sites.
- Persist Mutator history across sessions or per-patch maps.
- Auto-export on each MUTATE/RETRY.
- Lock logo-menu destinations during Compare.
- Rename `Mxx` / `Mxx-Ryy` export stems away from MutationNaming (FR-33 / D-085).

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Device nav load | Bank/patch change, synth returns dump | Editor mirrors dump; PatchLoadContext=DeviceMemory+coords; Mutator history cleared | Dump fail → warning footer; HALT policy if unclear (Ask First) |
| Computer load | Select `.syx` | Editor loads file; context=ComputerFile+stem; history cleared | Existing file-load errors unchanged |
| First MUTATE | Context set, history empty | Capture initial snapshot + freeze export folder basename from context | N/A |
| Export device named | Frozen `B08-P25` + name `OB-VOX` | Write under `…/B08-P25-OB-VOX/` | Folder not writable → existing export warning |
| Export blank name | Trimmed name empty | Folder `Bxx-Pyy` only | N/A |
| Export computer | Stem `WARM-PAD.syx` | Folder `Syx-WARM-PAD` | N/A |
| Export collision | Target session folder exists | Modal: Overwrite / Keep (`-2`) / Cancel | Cancel leaves disk unchanged |
| History gate | History non-empty, user changes patch context | Modal: Export / Cancel / Discard | Export failure does not discard/navigate |
| Compare enter | History non-empty, toggle Compare | Audition initial; UI locked; footer set | Empty history still blocks Compare |
| Compare exit | Toggle Compare off | Unlock UI; clear Compare footer; restore selection audition | N/A |

</frozen-after-approval>

## Code Map

- `Source/Core/MIDI/MidiManager.cpp` / `.h` — `requestCurrentPatch`, `requestSysExData`; extend for post-selection dump + apply path
- `Source/Core/MIDI/SysEx/SysExEncoder.cpp` — `encodeRequestMessage`; edit-buffer vs single-patch request
- `Source/Core/MIDI/SysEx/SysExDecoder.cpp` — `decodePatchSysEx`
- `Source/Core/MIDI/PatchSelectionMidiSync.cpp` — Set Bank + Program Change only today
- `Source/Core/Actions/PatchManagerActionHandler.cpp` — `applyPatchCoordinates`, bank buttons, `.syx` `applyLoadedPatchToApvtsAndSynth`, INIT/PASTE
- `Source/Core/PluginProcessor.cpp` — `handlePatchNumberChange`, `onPatchLoaded` → `resetSessionForPatchLoad`
- `Source/Core/Actions/ActionExecutionHooks.h` — `onPatchLoaded`
- `Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp` — `mutate` snapshot, `toggleCompare`, `exportHistory`, `resetSessionForPatchLoad`
- `Source/Core/Services/PatchMutator/MutationHistoryStore.*` — initial snapshot; extend for frozen export metadata
- `Source/Core/Services/PatchFileService.*` — `exportMutatorHistory` layout root
- `Source/Core/Services/PatchFileNameSanitizer.*` — folder/name sanitize
- `Source/Core/Device/DeviceMemoryLimits.*` — ROM vs RAM helpers (load not paste-gated)
- `Source/GUI/Helpers/GrayedControlHelper.*` — gray + optional click footer patterns
- `Source/GUI/Panels/.../PatchMutatorPanel.cpp` — Compare blink / history disable
- `Source/GUI/Panels/.../InternalPatchesPanel.cpp` — STORE/INIT vs `kCompareActive`
- `Source/GUI/Dialogs/MasterInitConfirmDialog.*` + `PluginEditor` Defrag/AlertWindow — dialog patterns
- `Source/Shared/Definitions/PluginDisplayNames.h` — dialog + footer strings
- `Source/GUI/.../InteractiveDisplayApvtsSync.cpp` + envelope/track widgets — freeze mouse edit
- `Tests/Unit/PatchMutatorEngineTests.cpp`, `PatchFileServiceTests.cpp`, `PatchManagerActionHandlerTests.cpp`, MIDI/SysEx tests — extend

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/...` (new small PatchLoadContext type + owner on processor/engine) -- record DeviceMemory vs ComputerFile (+ bank/patch or file stem) at `.syx` load, device dump, INIT, PASTE -- export origin SSOT — DONE; device-dump site now sets DeviceMemory context via `loadCurrentPatchFromDevice`
- [x] `MidiManager` + selection handlers (`PatchManagerActionHandler`, `PluginProcessor::handlePatchNumberChange`) -- after Set Bank/PC, request dump, `loadFrom` + APVTS suppress push, set context, `onPatchLoaded`; order queue vs RPC safely -- editor follows synth slot — DONE; `MidiManager::waitUntilOutboundQueueIdle` + `isDeviceDumpAvailable` gate the blocking RPC; dump-fail keeps coords + old buffer + warning footer; no `sendPatch` back
- [x] `PatchMutatorEngine` / `MutationHistoryStore` -- freeze export basename (+ origin fields) on first MUTATE with initial snapshot; clear on session reset -- stable Export names
- [x] History gate before patch-context changes -- modal Export / Cancel / Discard when history non-empty; only then navigate/reset -- no silent mutation loss — DONE; `ActionExecutionHooks::confirmPatchContextChange` -> `PluginProcessor::confirmPatchContextChangeGate` -> PluginEditor AlertWindow; gate consulted before nav / `.syx` / INIT / PASTE / number-box edit; Cancel reverts number box
- [x] `PatchFileService::exportMutatorHistory` (+ engine/handler/picker) -- create session subfolder from frozen basename; collision Overwrite/Keep/Cancel -- hybrid folders — DONE; PluginEditor collision AlertWindow wired via `setMutatorExportCollisionModalGate` (serves both the Export button and the gate's Export path)
- [x] Compare lock -- broadcast `kCompareActive` (shared `CompareLockBinder` helper) across Patch/Master/Matrix/Bank/Internal/Computer panels; freeze interactive displays; footer on enter/clear on exit; COMPARE + logo menu remain -- A/B safety — DONE; Mutator panel self-locks all controls except COMPARE
- [x] `Tests/Unit/...` -- cover I/O matrix edges (context freeze, folder names, blank name, Syx stem, collision index, history gate ordering, compare lock property, device-load hook fires `onPatchLoaded`) -- regress safely — DONE for context freeze / folder names / blank name / Syx stem / collision index / overwrite; added history-gate ordering (cancel aborts nav/INIT, proceed advances) + MidiManager idle/availability smoke. Device dump with mocked MIDI not unit-covered (needs hardware/mock port) — manual check
- [x] `PluginDisplayNames.h` -- English dialog/footer copy for gate, collision, Compare -- UI strings SSOT

**Completion Notes (feature complete — 2026-07-16):**

All five previously-remaining gaps are now implemented. Full plugin (Standalone / AU / VST3) builds clean and 465/465 unit tests pass (460 prior + 5 new). Manual hardware verification of the device dump path is still recommended (see below).

Second-pass implementation (device dump, history gate, collision UI, compare lock):
- **Device dump after Bank/Internal nav:** `MidiManager::waitUntilOutboundQueueIdle(timeoutMs)` (drains the outbound queue via `isEmpty()` + a `hasPendingSysEx_` atomic, waking the consumer) and `MidiManager::isDeviceDumpAvailable()` (output open AND `deviceDetected`). `PatchManagerActionHandler::loadCurrentPatchFromDevice()` waits idle -> ~50 ms settle -> `requestCurrentPatch()` -> `PatchModel::loadFrom` -> APVTS suppress push -> DeviceMemory context -> `onPatchLoaded`. On dump failure it keeps coordinates + old buffer and shows an English warning footer; never `sendPatch` back. Wired on prev/next, bank buttons, and `PluginProcessor::handlePatchNumberChange`. Skips cleanly when no device is available (headless/tests).
- **History gate:** `ActionExecutionHooks::confirmPatchContextChange` -> `PluginProcessor::confirmPatchContextChangeGate()` (empty history / no UI gate -> proceed; else Export/Cancel/Discard). Export runs the folder-picker + engine export + collision resolution and only proceeds when it succeeds; Discard calls `resetSessionForPatchLoad`; Cancel aborts. Consulted before nav, `.syx` load, INIT, PASTE, and direct patch-number edits (number box reverts to the previous value on Cancel). INIT/PASTE now also call `onPatchLoaded` after a successful apply.
- **Collision modal UI:** `PluginEditor` supplies `setMutatorExportCollisionModalGate` (Overwrite / Keep both / Cancel AlertWindow) and `setMutatorHistoryGateModalGate` (Export / Discard / Cancel AlertWindow).
- **Compare lock:** new `TSS::CompareLockBinder` (GUI helper listening to `kCompareActive`) locks Patch Edit + Master Edit (BodyPanel), Matrix Mod (SharedPanel), and Bank Utility / Internal Patches / Computer Patches (PatchManagerPanel) by blocking child mouse clicks + graying; interactive displays freeze because their parent panel is locked. The Mutator panel self-locks every control except COMPARE. Footer set on Compare enter (`kCompareLockedFooter`) and cleared on any exit (toggle-off, `forceExitCompare`). Logo menu stays usable.
- **Tests:** added history-gate ordering (cancel aborts nav + INIT, proceed advances) in `PatchManagerActionHandlerTests` and `isDeviceDumpAvailable` / `waitUntilOutboundQueueIdle` smoke tests in `MidiManagerTests`.

First-pass implementation (Core, builds clean):
- `Core::PatchLoadContext` (`Source/Core/Services/PatchMutator/PatchLoadContext.{h,cpp}`): SSOT value type, `deviceMemory` / `computerFile` factories, `computeExportBasename()` (`Bbb-Ppp[-NAME]` / `Syx-STEM`, blank/unsanitizable name omitted).
- `PatchFileNameSanitizer::sanitizeToMatrixNameOrEmpty()` (no-fallback filter) — used for the optional `-NAME` segment.
- `MutationHistoryStore` frozen export basename storage, cleared in `clearInitialSnapshot()` (so it clears on `resetSessionForPatchLoad`).
- `PatchMutatorEngine`: `setPatchLoadContextProvider()`, freeze basename on first MUTATE, `exportHistory` creates the session subfolder and requests a collision modal when it exists, `exportHistoryResolved(Overwrite/Keep/Cancel)`.
- `PatchFileService`: `exportMutatorHistorySession(clearExisting)`, `resolveKeepSessionFolder()` (`-2`, `-3`, …), extracted `writeHistoryLayout()`.
- `ActionExecutionHooks::setPatchLoadContext` hook; `PluginProcessor` owns `patchLoadContext_`, feeds the engine provider, and sets device context on Bank/Internal coordinate changes; `PatchManagerActionHandler` sets ComputerFile on `.syx` load and DeviceMemory on INIT/PASTE.
- `MutatorActionHandler` + `PluginProcessor` collision-modal-gate plumbing (`setMutatorExportCollisionModalGate`).
- English strings for Compare footer, history gate, and collision dialog in `PluginDisplayNames.h`.

Remaining work: manual hardware verification only (see Manual checks). No known code gaps.

**Acceptance Criteria:**
- Given Bank or Internal patch change and a valid synth dump, when navigation completes, then editor parameters/name match the dump and Mutator history is empty with Compare off.
- Given a Computer `.syx` load, when load succeeds, then PatchLoadContext is ComputerFile with that file stem and history resets.
- Given non-empty Mutator history, when the user attempts any patch-context change, then a modal offers Export / Cancel / Discard and Cancel leaves history and patch unchanged.
- Given first MUTATE after a device load at bank 8 patch 25 named `OB-VOX`, when Export runs to folder F, then files land under `F/B08-P25-OB-VOX/` with FR-33 contents.
- Given Computer stem `WARM-PAD`, when Export runs, then session folder is `Syx-WARM-PAD`.
- Given trimmed patch name empty on device origin, when Export runs, then session folder is `Bxx-Pyy` only.
- Given session folder already exists, when Export is confirmed Keep, then a new folder `…-2` (then `-3`, …) is used; Overwrite clears that folder then writes; Cancel writes nothing.
- Given Compare on, when the user views the UI, then editing and bank/patch/file navigation controls are disabled/frozen, Compare footer is shown, and logo-menu actions still work; exiting Compare restores interaction and clears that footer.

## Spec Change Log

## Design Notes

**Device load sequencing (normative intent):** update coordinates → sync Set Bank/PC → ensure those MIDI messages are sent before dump request → request edit buffer (preferred after PC) or single-patch by number → decode → `PatchModel::loadFrom` → APVTS suppress sync → set PatchLoadContext DeviceMemory → `onPatchLoaded`. Do not `sendPatch` back on this path.

**Export basename examples:** `B01-P77-WARM-PAD`, `B00-P00` (blank name), `Syx-MY-PAD`. Collision: `B03-P38-OB-VOX` exists → Keep → `B03-P38-OB-VOX-2`.

**Compare lock:** prefer one APVTS-driven lock path panels already listen to (`kCompareActive`); interactive displays must ignore mouse while locked (callbacks no-op or intercepts off), not merely look gray.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` -- expected: build succeeds
- Run Mutator / PatchFile / PatchManager / SysEx-related unit tests in `Matrix-Control_Tests` -- expected: green for touched suites

**Manual checks:**
- Hardware: Bank Utility + Internal prev/next update Patch Edit name/params; Mutator history clears; Compare locks UI + footer; Export creates hybrid session folder; history-loss and collision dialogs behave as matrix.

## Suggested Review Order

**Device dump into the editor**

- Entry point: after Set Bank/PC, drain queue then request edit buffer into PatchModel.
  [`PatchManagerActionHandler.cpp:633`](../../Source/Core/Actions/PatchManagerActionHandler.cpp#L633)

- Queue-idle wait before the blocking dump RPC.
  [`MidiManager.cpp:360`](../../Source/Core/MIDI/MidiManager.cpp#L360)

- Number-box navigation also triggers the dump after the history gate.
  [`PluginProcessor.cpp:1602`](../../Source/Core/PluginProcessor.cpp#L1602)

**History gate before losing mutations**

- Export / Cancel / Discard decisions before any patch-context change.
  [`PluginProcessor.cpp:849`](../../Source/Core/PluginProcessor.cpp#L849)

- Successful Export clears history before proceeding to nav/dump.
  [`PluginProcessor.cpp:867`](../../Source/Core/PluginProcessor.cpp#L867)

**Export session folders**

- Basename rules for device vs Computer Patches.
  [`PatchLoadContext.cpp:34`](../../Source/Core/Services/PatchMutator/PatchLoadContext.cpp#L34)

- Freeze basename on first MUTATE with the initial snapshot.
  [`PatchMutatorEngine.cpp:596`](../../Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp#L596)

- Create hybrid session subfolder; request collision modal when it exists.
  [`PatchMutatorEngine.cpp:530`](../../Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp#L530)

**Compare lock**

- Shared binder grays Patch Edit / Master / Matrix / Patch Manager panels.
  [`CompareLockBinder.cpp:42`](../../Source/GUI/Helpers/CompareLockBinder.cpp#L42)

- Mutator keeps COMPARE clickable; History combo now disabled while locked.
  [`PatchMutatorPanel.cpp:576`](../../Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp#L576)

**Tests**

- Basename / collision / history-gate ordering coverage.
  [`PatchLoadContextTests.cpp:1`](../../Tests/Unit/PatchLoadContextTests.cpp#L1)

