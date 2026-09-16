---
organization: Ten Square Software
project: Matrix-Control
title: Inventory — Sticky footer messages
author: BMad Agent
status: archived
sources:
  - Desktop draft Matrix-Control-sticky-footer-messages.md
  - _bmad-output/implementation-artifacts/spec-sticky-footer-messages-inventory.md
created: 2026-09-16
updated: 2026-09-17
---

# Matrix-Control — Sticky footer messages inventory

**Date:** 2026-09-16  
**Scope:** Sticky left-band status messages only (`uiMessageText` / `uiMessageSeverity` in APVTS).  
**Out of scope:** Furtive HELP overlay (`ContextualHelp*`), bank progress dialog strings, DEVICE badge chrome, MIDI queue centre-band alerts.

**How sticky works:** Writers set APVTS `uiMessageText` (+ severity). `FooterPanel` paints the left band until the message is cleared or replaced. Hover HELP temporarily covers sticky text without changing APVTS.

**Casing convention (product):** Module / panel names in GUI and HELP are UPPERCASE (`PATCH MUTATOR`, `DCO 1`, `MATRIX MODULATION`, …).  
**Known sticky exception:** Patch Mutator sticky prefix is Title Case `Patch Mutator: ` (HELP uses `PATCH MUTATOR: `). Clipboard uses `Matrix Modulation` (Title Case) vs module `kName` = `MATRIX MODULATION`.

**Column guide**

| Column | Meaning |
|--------|---------|
| Symbol | Constant or formatter name |
| Display text / pattern | Exact English copy or template |
| When | Usage trigger |
| Casing note | Module-name casing to review |

---

## 1. Device / MIDI lock and ports

| Symbol | Display text / pattern | When | Casing note |
|--------|------------------------|------|-------------|
| `FooterPanel::kDeviceLockGuidance` | No synth detected - check MIDI cables, MIDI FROM / MIDI TO options, and power-cycle your Matrix synth. | Device lock / grayed UI while no supported synth | Control names UPPERCASE OK |
| `FooterPanel::kDeviceUnresponsiveGuidance` | Synth not responding (often after heavy SysEx edits) - stop editing, power-cycle the Matrix, then click PANIC if a note is still stuck. | Presence timeout | OK |
| `FooterPanel::kUnsupportedMatrixDeviceFooter` | Connected Matrix device is not supported - editing is locked. Use MIDI FROM / MIDI TO to change ports. | Unsupported Matrix member | Control names UPPERCASE OK |
| `FooterPanel::kMidiFromKeyboardFromConflictFooter` | MIDI FROM and KEYBOARD FROM must use different MIDI input devices. | Standalone port conflict | Control names UPPERCASE OK |
| `MidiPortOpenFeedback::formatFooterMessage` (`kNotFound`) | `MIDI FROM` / `MIDI TO`: "`{name}`" - port not found (device disconnected or unavailable). | Port missing | Labels UPPERCASE OK |
| `MidiPortOpenFeedback::formatFooterMessage` (`kOpenRejected`, non-Windows) | `MIDI FROM` / `MIDI TO`: "`{name}`" - could not open (port may be in use by another application. Try closing other applications using this port). | Port open rejected | Labels UPPERCASE OK |
| `MidiPortOpenFeedback::formatFooterMessage` (`kOpenRejected`, Windows) | Same as above, plus Windows-specific “e.g. your DAW / loopMIDI” wording, then optional `See Documentation/windows-midi-multi-client.md for setup help.` (+ absolute path if file exists). | Port open rejected on Windows | Labels UPPERCASE OK |

---

## 2. Settings / init templates / Master files

| Symbol | Display text / pattern | When | Casing note |
|--------|------------------------|------|-------------|
| `Settings::FooterMessages::kPatchInitTemplateSaved` | Patch init template saved (PatchInit.syx) | Save patch as init | Sentence case; no module prefix |
| `Settings::FooterMessages::kMasterInitTemplateSaved` | Master init template saved (MasterInit.syx) | Save master as init | "Master" Title Case (product section often MASTER) |
| `Settings::FooterMessages::kInitTemplateWriteFailed` | Could not write init template | Write fail | OK |
| `Settings::FooterMessages::kPatchInitTemplateDeleted` | Patch init template deleted (PatchInit.syx) | Delete patch init | OK |
| `Settings::FooterMessages::kMasterInitTemplateDeleted` | Master init template deleted (MasterInit.syx) | Delete master init | "Master" Title Case |
| `Settings::FooterMessages::kInitTemplateDeleteFailed` | Could not delete init template | Delete fail | OK |
| `Settings::FooterMessages::kMasterLoaded` | Master loaded | Load Master file | "Master" Title Case |
| `Settings::FooterMessages::kMasterSaved` | Master saved | Save Master file | "Master" Title Case |
| `Settings::FooterMessages::kMasterFileFailed` | Master file failed | Master file error | "Master" Title Case |
| `Settings::FooterMessages::kRenameBeforeSave` | Rename patch (replace * INIT *) before SAVE / SAVE AS | Sentinel name blocks save | Buttons UPPERCASE OK |
| `InitTemplateLoader::formatFallbackMessage` | `PatchInit.syx` / `MasterInit.syx` not found - using built-in defaults. | Missing template file | Filename as-is |
| `InitTemplateLoader::formatFallbackMessage` | `PatchInit.syx` / `MasterInit.syx` is invalid - using built-in defaults. | Invalid template file | Filename as-is |

---

## 3. Clipboard copy / paste

Module names come from `*Module::kName` (UPPERCASE: `DCO 1`, `ENV 1`, …) except Matrix Modulation sticky name.

| Symbol | Display text / pattern | When | Casing note |
|--------|------------------------|------|-------------|
| `ClipboardMessages::formatModuleCopied` | `{MODULE}` copied to clipboard. | Module copy | Module = UPPERCASE `kName` |
| `ClipboardMessages::formatEnvelopeShapeCopied` | `{ENV n}` envelope shape copied to clipboard (Delay, Attack, Decay, Sustain, Release). | Shape-only copy | Module UPPERCASE; envelope stage words Title Case |
| `ClipboardMessages::formatMatrixModulationCopied` | Matrix Modulation copied to clipboard. | MM copy | **Title Case** — vs `MATRIX MODULATION` |
| `ClipboardMessages::formatPatchCopied` | `{BANK n / PATCH n}` or `{PATCH n}` copied to clipboard. | Internal patch copy | BANK/PATCH UPPERCASE OK |
| `ClipboardMessages::formatModulePasted` | `{source}` pasted successfully to `{target}`. | Module paste OK | Module names UPPERCASE |
| `ClipboardMessages::formatEnvelopeShapePasted` | `{source}` envelope shape pasted successfully to `{target}` (Delay, Attack, Decay, Sustain, Release). | Shape paste OK | Same as shape copy |
| `ClipboardMessages::formatMatrixModulationPasted` | Matrix Modulation pasted successfully. | MM paste OK | **Title Case** |
| `ClipboardMessages::formatPatchPasted` | `{source}` pasted successfully to `{target}`. | Patch paste OK | BANK/PATCH UPPERCASE |
| `ClipboardMessages::formatPasteFailed` | Paste from `{source}` [to `{target}`] failed. / Paste failed. | Paste fail | Labels vary |
| `ClipboardMessages::formatIncompatiblePaste` | Clipboard contents from `{source}` are not compatible with `{target}` [ / this module]. | Type mismatch | Labels vary |
| `ShortLabels::kNothingToPasteFooter` | Nothing to paste. | Empty clipboard paste | OK |
| `ShortLabels::kIncompatiblePasteFooter` | Clipboard contents are not compatible with this module. | Defined only — **no live call sites** (formatters used) | Dead constant |
| `ShortLabels::kPasteFailedFooter` | Paste failed. | Defined only — **no live call sites** (formatters used) | Dead constant |
| `MatrixModulationSection::Header::kIncompatiblePasteFooter` | Clipboard contents are not compatible with Matrix Modulation paste. | Defined only — **no live call sites** | Dead constant; contains Title Case “Matrix Modulation” |

Constant used by formatters: `ClipboardMessages::kMatrixModulationName` = `Matrix Modulation` (**Title Case candidate**).

---

## 4. Patch-edit gates (grayed control click)

| Symbol | Display text / pattern | When | Casing note |
|--------|------------------------|------|-------------|
| `RampPortamentoModule::kLegatoPortaUnisonBlockedFooter` | LEGATO PORTA is only available when KEYBOARD MODE is UNISON. | LEGATO grayed click | Controls UPPERCASE OK |
| `RampPortamentoModule::kMasterUnisonOverrideFooter` | Master Unison is overriding Keyboard Mode. | Master unison override | **"Master Unison" / "Keyboard Mode" Title Case** — vs MASTER / KEYBOARD MODE elsewhere |
| `RampPortamentoModule::kStrigRequiresUnisonFooter` | STRIG is only available when KEYBOARD MODE is UNISON. | STRIG grayed click | Controls UPPERCASE OK |

---

## 5. Patch name

| Symbol | Display text / pattern | When | Casing note |
|--------|------------------------|------|-------------|
| `PatchNameModule::Messages::kInvalidCharacterFooter` | Invalid character for patch name | Illegal rename character | No module prefix |

---

## 6. Computer patches (disk)

| Symbol | Display text / pattern | When | Casing note |
|--------|------------------------|------|-------------|
| `FooterMessages::formatScanSummary` | `{N} valid, {M} invalid` | Folder scan summary | OK |
| `FooterMessages::kEmptyFolder` | 0 files in folder | Empty folder | OK |
| `FooterMessages::kFolderNotFound` | Folder not found | Missing folder | OK |
| `FooterMessages::formatLoadSuccess` | Loaded `{path}` | Load OK | Path as-is; FooterPanel may middle-truncate |
| `FooterMessages::formatSaveSuccess` | Saved `{path}` | Save OK | Path as-is |
| `FooterMessages::formatFirstLoadAfterScan` / `formatFirstLoadAfterScanMessage` | Patch files: `{N} valid` [/ `{M} invalid`] - Loaded `{path}` [optional `(filename used)` / `(internal name used)` when wrapped around reconciliation] | First load after OPEN / drop scan | OK |
| `FooterMessages::formatReconciliationNotice` | Loaded `{path}` (filename used) / (internal name used) | Name reconcile | OK |
| `FooterMessages::kLoadSelectionStale` | Selection out of date | Stale selection | OK |
| `FooterMessages::kPatchFileNotFound` | Patch file not found | Missing file | OK |
| `FooterMessages::kInvalidSaveStem` | Invalid patch file name (use A-Z, 0-9, space, -, _; max 8) | Bad save stem | OK |
| `FooterMessages::kDropRejectedNoValid` | Drop rejected: no valid Matrix patch .syx | Drag-drop reject | OK |
| `FooterMessages::kDropRejectedNotSyx` | Drop rejected: not a Matrix patch .syx | Drag-drop reject | OK |
| `FooterMessages::kDropRejectedBankOrMulti` | Drop rejected: bank or multi-message dump | Drag-drop reject | OK |
| `FooterMessages::kDropRejectedInvalid` | Drop rejected: invalid patch file | Drag-drop reject | OK |

---

## 7. Bank utility

Namespace: `PluginDisplayNames::PatchManagerSection::BankUtilityModule` (gates) and `…::FooterMessages` (formatters).

| Symbol | Display text / pattern | When | Casing note |
|--------|------------------------|------|-------------|
| `kImportRomBlockedFooterMessage` | Import is only available on Matrix-1000 RAM banks 0 and 1. | Import on ROM | No BANK UTILITY prefix |
| `kPasteRomBlockedFooterMessage` | Paste is only available on Matrix-1000 RAM banks 0 and 1. | Paste on ROM | Same |
| `kPasteClipboardFailedFooterMessage` | Bank paste failed - clipboard contents could not be read. | Clipboard unreadable | "Bank" sentence case |
| `kBankTransferBusyFooterMessage` | Bank transfer in progress - wait for it to finish. | Transfer busy | Same |
| `kDeviceUnavailableFooterMessage` | Bank transfer requires a connected, supported Matrix device. | No device | Same |
| `kSnapshotFailedFooterMessage` | Could not read the current bank from the device - import aborted, nothing was written. | Snapshot fail (import) | Same |
| `kPasteSnapshotFailedFooterMessage` | Could not read the current bank from the device - paste aborted, nothing was written. | Snapshot fail (paste) | Same |
| `kExportCancelledFooterMessage` | Export cancelled - no files kept for this run. | Export cancel | OK |
| `kImportCancelledFooterMessage` | Import cancelled - device restored to its prior state. | Import cancel | OK |
| `kCopyCancelledFooterMessage` | Bank copy cancelled - clipboard unchanged. | Copy cancel | Same |
| `kCopyFailedFooterMessage` | Bank copy failed - clipboard unchanged. Check the connection and try again. | Copy fail | Same |
| `kPasteCancelledFooterMessage` | Paste cancelled - device restored to its prior state. | Paste cancel | OK |
| `kFolderNotWritableFooterMessage` | Could not create or write to the export folder. | Folder not writable | OK |
| `kImportRestoreFailedFooterMessage` | Import cancelled - the device could not be fully restored. Check the connection and try again. | Restore fail | OK |
| `kPasteRestoreFailedFooterMessage` | Paste cancelled - the device could not be fully restored. Check the connection and try again. | Restore fail | OK |
| `FooterMessages::formatExportSuccess` | Bank `{n}` exported successfully : 100 patches saved to `{path}.` / Patches exported successfully : … | Export OK | "Bank" sentence case |
| `FooterMessages::formatImportSuccess` | Import complete - found `{f}`, valid `{v}`, imported `{i}` | Import OK | OK |
| `FooterMessages::formatImportNoValidFiles` | No valid .syx files to import (found `{n}`) | No valid files | OK |
| `FooterMessages::formatCopySuccess` | Bank `{n}` copied to clipboard. | Copy OK | "Bank" sentence case |
| `FooterMessages::formatPasteSuccess` | Bank `{source}` pasted to bank `{target}`. | Paste OK | "Bank" / "bank" mixed case in sentence |

**Not sticky (dialog progress — excluded):** `kExportingMessage`, `kImportingReadingMessage`, `kImportingWritingMessage`, `kImportingRestoringMessage`, `kPastingWritingMessage`.

---

## 8. Patch Mutator

All sticky strings below use prefix **`Patch Mutator: `** (`Core::…::kStickyModulePrefix` in `PatchMutatorEngineInternal.h`, or embedded literals / `PatchMutatorModule::Messages::*` in `PluginDisplayNames.h`). HELP overlay uses **`PATCH MUTATOR: `** instead.

| Symbol | Display text / pattern | When | Casing note |
|--------|------------------------|------|-------------|
| `kNoModuleScopeFooterMessage` | Patch Mutator: Enable at least one module to mutate. | Mutate with no modules | **Prefix Title Case** |
| `kNoMutationChangeFooterMessage` | Patch Mutator: No changes. Try a wider MODE or more modules. | No-op mutate | Prefix Title Case; MODE UPPERCASE OK |
| `kRetryTooSimilarFooterMessage` | Patch Mutator: RETRY too similar. Try a wider MODE or more modules. | Retry diversity fail | Prefix Title Case; RETRY/MODE UPPERCASE OK |
| `kHistoryLimitFooterMessage` | Patch Mutator: Mutation history is full. Defrag to continue. | History full | Prefix Title Case |
| `kEmptyHistoryFooterMessage` | Patch Mutator: Mutation history is empty. | Empty history ops | Prefix Title Case |
| `kNoSelectionFooterMessage` | Patch Mutator: No valid mutation history entry selected. | Bad history selection | Prefix Title Case |
| `kNoInitialSnapshotFooterMessage` | Patch Mutator: No initial patch snapshot available for compare. | Compare without snapshot | Prefix Title Case |
| `kFlushSuccessFooterMessage` | Patch Mutator: Mutation history flushed. | Flush OK | Prefix Title Case |
| `kDefragCompleteFooterMessage` | Patch Mutator: Mutation history renumbered. | Defrag OK | Prefix Title Case |
| `kExportFolderNotWritableFooterMessage` | Patch Mutator: Export folder is not writable. | Export folder | Prefix Title Case |
| `kExportFailedFooterMessage` | Patch Mutator: Mutation export failed. | Export fail | Prefix Title Case |
| `formatRetryDeleteFooterMessage` | Patch Mutator: Deleted `{Mxx-Ryy}.` | Delete retry | Prefix Title Case |
| `formatRootDeleteCascadeFooterMessage` | Patch Mutator: Deleted `{Mxx} and all retries.` | Delete root cascade | Prefix Title Case |
| `formatExportCompleteFooterMessage` / `kExportCompleteFooterStem` | Patch Mutator: Exported `{N}` mutation file(s) to `{path}.` | Export OK | Prefix Title Case |
| `Messages::kCompareLockedFooter` | Patch Mutator: Compare mode - editing and patch/bank changes are locked. Click C again to exit. | Compare on | Prefix Title Case |
| `Messages::kExportCancelledFooter` | Patch Mutator: Export cancelled. | Export cancel | Prefix Title Case |
| `Messages::kDeviceDumpFailedFooter` | Patch Mutator: Could not read the patch from the synth. Keeping the current editor buffer. Check that MIDI FROM is the synth MIDI OUT. | Device load fail | Prefix Title Case; MIDI FROM UPPERCASE OK |
| `Messages::kDeviceDumpAbortedEditedFooter` | Patch Mutator: Synth patch load cancelled because the editor changed while waiting. Bank and patch numbers were restored; your edits were kept. | Load aborted | Prefix Title Case |
| Export error passthrough (`kStickyModulePrefix` + `PatchFileExportResult::errorMessage`) | `Patch Mutator: History empty` / `Patch Mutator: Folder not writable` / `Patch Mutator: Write failed` / `Patch Mutator: Encode failed` / `Patch Mutator: Validation failed` / `Patch Mutator: Invalid patch data` / other save `errorMessage` values | Mutator export failure when service returns `errorMessage` | Prefix Title Case; wording can differ from dedicated constants (`kEmptyHistoryFooterMessage`, `kExportFolderNotWritableFooterMessage`) |

Module display name constant: `PatchMutatorModule::kName` = `PATCH MUTATOR` (UPPERCASE — used for UI title, not sticky prefix).

---

## 9. PatchFileService bare `errorMessage` (also sticky)

These short English strings are **not** in `PluginDisplayNames`. They reach the sticky footer when callers publish `result.errorMessage` / `saveResult.errorMessage` directly (computer load/save failure, bank export failure, init-template write fallback, and Mutator export passthrough with the `Patch Mutator: ` prefix).

| Symbol / origin | Display text | Typical sticky path | Casing note |
|-----------------|--------------|---------------------|-------------|
| `PatchFileService` load | Invalid patch buffer | Computer load failure | No module name |
| `PatchFileService` load | File not found | Computer load failure | Near-duplicate of `kPatchFileNotFound` (“Patch file not found”) |
| `PatchFileService` load | Read failed | Computer load failure | No module name |
| `PatchFileService` load | Invalid patch file | Computer load failure | No module name |
| `PatchFileService` load | Decode failed | Computer load failure | No module name |
| `PatchFileService` save | Validation failed | Save / export / init write | No module name |
| `PatchFileService` save | Write failed | Save / export / init write | No module name |
| `PatchFileService` save | Invalid patch data | Save / export | No module name |
| `PatchFileService` save | Folder not writable | Save / export / Mutator export | Near-duplicate of dedicated Mutator / Bank folder messages |
| `PatchFileService` save | Encode failed | Save / export | No module name |
| Mutator export service | History empty | Prefixed as `Patch Mutator: History empty` | Near-duplicate of `kEmptyHistoryFooterMessage` |

## 10. Dev / widget factory (also sticky via ExceptionPropagator)

| Symbol | Display text / pattern | When | Casing note |
|--------|------------------------|------|-------------|
| `ParameterNotFoundException` | Parameter not found: `{id}` | Factory validation | Dev-facing |
| `InvalidParameterException` | Invalid parameter '{id}': `{reason}` | Factory validation | Dev-facing |
| `WidgetNotFoundException` | Widget not found: `{id}` | Factory validation | Dev-facing |
| `InvalidWidgetTypeException` | Widget '{id}' has invalid type. Expected: {expected}, Got: {actual} | Factory validation | Dev-facing |

---

## 11. Explicitly excluded (not sticky APVTS)

| Kind | Examples | Why excluded |
|------|----------|--------------|
| Furtive HELP | `ContextualHelp::*` (e.g. `PATCH MUTATOR: …`, `BANK UTILITY: …`) | Overlay only; never writes `uiMessageText` |
| Bank progress dialogs | `kExportingMessage`, `kImporting*`, `kPastingWritingMessage` | Modal progress, not sticky footer |
| DEVICE badge chrome | Device model / version paint | Separate footer band |
| MIDI queue pressure | Centre-band alert | Not `uiMessageText` |

---

## Casing hotspots (quick review list)

| Hotspot | Sticky today | Likely target convention |
|---------|--------------|--------------------------|
| Patch Mutator sticky prefix | `Patch Mutator: ` | `PATCH MUTATOR: ` |
| Matrix Modulation clipboard name | `Matrix Modulation` | `MATRIX MODULATION` |
| Master Unison override footer | `Master Unison` / `Keyboard Mode` | Possibly `MASTER UNISON` / `KEYBOARD MODE` |
| Settings Master file footers | `Master loaded` / `Master saved` / … | Decide: keep sentence case vs `MASTER` |

---

## Primary sources

- `Source/Shared/Definitions/PluginDisplayNames.h`
- `Source/Core/Services/PatchMutator/PatchMutatorEngineInternal.h` (`kStickyModulePrefix`)
- Writers: `ExceptionPropagator`, `GrayedControlHelper::setFooter*Message`, Mutator / Patch Manager / MIDI / clipboard / init paths
- Painter: `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.{h,cpp}`

---

*Inventory only — no code changes. Edit desired strings here, then implement in a follow-up Build.*
