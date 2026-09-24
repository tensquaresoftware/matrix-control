---
organization: Ten Square Software
project: Matrix-Control
title: M1KP Patch Import
author: BMad Agent
status: done
route: dispatch
review_loop_iteration: 0
baseline_commit: 6e10d07ec74df6e4efb03e2c733502f7f81b2aac
created: 2026-09-25
updated: 2026-09-25
context:
  - {project-root}/_bmad-output/project-context.md
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Users of the legacy Matrix-1000 Editor (Max for Live) still have patch libraries as `.m1kp` files. Matrix-Control only opens `.syx`, so those libraries cannot be reopened.

**Approach:** Treat `.m1kp` as a read-only alternate on-disk encoding of the same 134 packed patch bytes already used by `PatchModel`. Decode into the existing Computer Patches load pipeline; never write `.m1kp` (SAVE / SAVE AS stay `.syx` only).

## Boundaries & Constraints

**Always:**
- `.m1kp` on disk = exactly **134 little-endian int16** values (268 bytes), no magic header / version / checksum (M4L `File.writeint16` / `readint16`).
- Decode to 134 packed bytes by taking each int16 as an **int8-range** value (`[-128, 127]`); reject the file if size ≠ 268 or any int16 is outside that range.
- After a successful decode, reuse the same path as a valid `.syx` load: `PatchModel` → name reconciliation (D-025) → APVTS → edit-buffer audition. Source file is never rewritten on open.
- **Surfaces (decision):** full Computer Patches parity — folder OPEN scan, combobox, prev/next, drag-drop — **and** Bank IMPORT accept `.m1kp` the same way as `.syx`.
- **Computer Patches combobox display (decision):** when `.syx` and `.m1kp` share a stem (or anytime a `.m1kp` is listed), show **both** files. Combobox label for `.m1kp` only is `Stem (m1kp)` (example: `NicePad (m1kp)`). `.syx` entries keep today’s stem-only label (no extension badge).
- **PATCH NAME module (decision):** after load from either format, PATCH NAME follows the existing D-025 reconciliation / DISPLAY policy only — never append `(m1kp)`. Example: combobox `NicePad (m1kp)` → PATCH NAME still `NicePad` (or whatever the reconciler selects from internal bytes vs filename stem).
- Unit-test decode + scan/assess gates with a fixture derived from `P-Test.m1kp` (name bytes `BNK2: 02`).

**Never:**
- Do not implement `.m1km` master import (deferred).
- Do not add SAVE / SAVE AS / Mutator EXPORT / PatchInit writers for `.m1kp`.
- Do not change `PluginDescriptors` packed layout or invent a second in-RAM patch model.
- Do not treat truncated / oversized / out-of-range int16 files as valid (fail closed; count as invalid in scans).
- Do not append `(m1kp)` (or any format badge) to `.syx` combobox labels, nor to PATCH NAME / packed name bytes.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Happy path | Valid 268-byte `.m1kp` (134 int16 in [-128,127]) | 134 packed bytes; load UX same as `.syx`; combobox label `Stem (m1kp)`; PATCH NAME without format badge | N/A |
| Dual stem | Folder has `NicePad.syx` + `NicePad.m1kp` (both valid) | Combobox: `NicePad` and `NicePad (m1kp)`; loading either leaves PATCH NAME badge-free (D-025) | N/A |
| Wrong size | Length ≠ 268 | Reject as invalid patch file | Footer / scan invalid count (existing style) |
| Out of range | Any int16 outside [-128,127] | Reject as invalid | Same as above |
| Empty / unreadable | Missing or I/O failure | Reject | Existing load error path |
| After edit + SAVE | Loaded from `.m1kp`, user saves | Writes `.syx` only; `.m1kp` untouched | Existing save errors |

</frozen-after-approval>

## Code Map

- `Source/Core/Services/PatchFileService.{h,cpp}` — `kSyxExtension`, `hasSyxExtension`, `scanFolder` / `findSyxFiles`, `validateFileContents`, `loadPatchSysExFile`, `assessSinglePatchSyxFile`; primary extension + decode dispatch.
- `Source/Core/Services/PatchFileServiceSelection.cpp` — drop merge path filters `.syx` only today.
- `Source/Core/Actions/PatchManagerActionHandlerComputerLoadSave.cpp` — `decodeAndReconcilePatchFile` / `applyLoadedPatchToApvtsAndSynth` after packed bytes exist.
- `Source/Core/Actions/PatchManagerActionHandlerComputerDrop.cpp` + `Source/GUI/PluginEditorFileDragDrop.cpp` — drag heuristic `pathLooksLikeSyx`; accept overlay / reject messages.
- `Source/Core/Services/BankImportPlanner.{h,cpp}` — consumes `PatchFolderScanResult`; follows whatever scan accepts.
- `Source/Core/Models/PatchModel.h` — `kBufferSize == 134` packed RAM layout (unchanged).
- `Source/Core/MIDI/SysEx/SysExDecoder.cpp` — `.syx` → packed; keep as `.syx` path; add sibling decode for `.m1kp` (do not force through SysEx nibbles).
- `Source/Shared/Definitions/PluginDisplayNames.h` — Computer Patches / drop / import copy still says `.syx`; update only strings that would mislead once `.m1kp` is accepted.
- `Tests/Unit/PatchFileServiceTests.cpp`, `PatchFileServiceAssessTests.cpp`, load/drop handler tests — extend for `.m1kp`.
- `Tests/Fixtures/` — add `P-Test.m1kp` (or trimmed fixture) from Desktop sample.
- External format SSOT (read-only): `/Volumes/Guillaume/Dev/Projects/Max for Live/Matrix-1000 Editor/Device/Max/code/m1k.dataManager.js` (`load`/`save` via `readint16`/`writeint16`); `m1k.sysExManager.js` `PATCH_BYTES_SIZE = 134`.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/Services/` (new small helper or `PatchFileService` methods) -- decode/validate `.m1kp` → 134 packed bytes; reject wrong size / out-of-range int16 -- format gate
- [x] `Source/Core/Services/PatchFileService.{h,cpp}` + `PatchFileServiceSelection.cpp` -- accept `.m1kp` in scan / load / assess / merge / Bank IMPORT path; keep SAVE writers `.syx`-only -- Computer Patches I/O
- [x] `Source/GUI/PluginEditorFileDragDrop.cpp` + `PatchManagerActionHandlerComputerDrop.cpp` -- drag accept + drop shape counts include `.m1kp` -- drop UX
- [x] Scan/list display builder -- Computer Patches combobox only: `.m1kp` as `Stem (m1kp)`; `.syx` stems unchanged; both kept when stems collide; PATCH NAME never gets the badge -- list clarity
- [x] `Source/Shared/Definitions/PluginDisplayNames.h` -- adjust misleading `.syx`-only user-facing strings for surfaces that now accept `.m1kp` (ASCII only) -- copy honesty
- [x] `Tests/Fixtures/` + `Tests/Unit/PatchFileService*.cpp` (+ load/drop tests as needed) -- fixture + I/O matrix cases incl. dual-stem labels; register in `Tests/CMakeLists.txt` -- regression

**Acceptance Criteria:**
- Given a valid `.m1kp` (268 bytes, int16 in range), when the user loads it via Computer Patches or Bank IMPORT, then the editor shows the packed patch (name bytes + params) and auditions via the existing edit-buffer path.
- Given an invalid `.m1kp` (wrong size or out-of-range int16), when scanned or opened, then it is rejected and counted/reported like other invalid patch files.
- Given a patch loaded from `.m1kp`, when the user SAVE / SAVE AS, then only a `.syx` is written and the original `.m1kp` is unchanged.
- Given a folder with both `NicePad.syx` and `NicePad.m1kp`, when scanned, then the Computer Patches combobox shows `NicePad` and `NicePad (m1kp)` as two selectable entries, and loading either does not put `(m1kp)` into PATCH NAME.

## Implementation Notes

- Added `PatchM1kpCodec` + wired scan/load/assess/drop through `PatchFileService::hasSupportedPatchExtension` / `decodePackedFromFile`.
- Combobox labels via `formatOpenListDisplayName` (`Stem (m1kp)`); SAVE remains `.syx`-only.
- Fixture: `Tests/Fixtures/Patches/UnitTests/P-Test.m1kp`. Removed accidental `Tests/Fixtures/Matrix-1000 Editor/` copy (included deferred `.m1km`).
- Matrix coverage: `PatchFileServiceM1kpTests` (+ assess/drop). Orchestrator added `loadPatchSysExFile_missingM1kpFails` for empty/unreadable row.
- Review patches: atomic decode, early size reject, public `hasSupportedPatchExtension` for editor drag gate + test, negative/oversized cases, softened SAVE leave-modal copy for `.m1kp` origin.
## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence / route |
|---------|---------|------------------|
| Blind: no Bank IMPORT dedicated .m1kp test | low | Bank IMPORT uses `scanFolder` + `loadPatchSysExFile` already covered for `.m1kp`; no separate extension filter. Rejected (everyday risk low; shared path proven). |
| Blind: no PATCH NAME badge-free assert after load | false | Combobox badge is only `formatOpenListDisplayName`; PATCH NAME comes from packed bytes / D-025 via `getFileNameWithoutExtension` stem — no code path appends `(m1kp)` to PATCH NAME. |
| Blind: SAVE / leave-modal copy claims overwrite `.syx` after `.m1kp` load | medium | `knownSyxFullPath_` can be a `.m1kp`; SAVE rewrites extension to `.syx` sibling — copy that says overwrite “its .syx” is wrong on first save. → patch |
| Blind: dual `kM1kpExtension` / codec `kExtension` | low | Service delegates to codec for validation; drift unlikely in everyday use. Rejected. |
| Blind: no static_assert on 268 vs 134*2 | low | Cosmetic; size gate still enforced at runtime. Rejected. |
| Blind: no negative int16 round-trip test; Design Notes high-byte claim | low | Codec already accepts signed range; missing test is cheap. → patch (test + note) |
| Blind: no oversized (>268) reject test | medium | Same root as size-before-read edge; matrix says length ≠ 268. → patch (with size gate) |
| Blind: `fileNameWithoutSyxExtension` stale name | low | Cosmetic rename only. Rejected. |
| Blind: syx-branded API names (`loadPatchSysExFile`, `kNotSyx`, …) | defer | Pre-existing naming; rename footprint beyond this story. |
| Blind: stale `.syx`-only comments (BankImportPlanner, etc.) | low | Comment hygiene; unlikely everyday harm. Rejected. |
| Blind: empty Spec Change Log / Verification target name | false | Finding asks to edit this build’s spec artifact; rejected by triage rules. |
| Edge: `decodeToPacked` leaves dirty `packedOut` on mid-loop fail | medium | Confirmed: writes then returns false. Callers usually ignore on fail, but buffer contract is unsafe. → patch |
| Edge: huge `.m1kp` fully loaded before size check | medium | Confirmed: `loadFileAsData` then decode size check. → patch |
| VG: editor `selectionLooksAcceptable` / `pathLooksLikePatchFile` untested | medium | Code accepts `.m1kp` today; DropLoad bypasses editor gate — regression can green-pass. → patch (public SSOT + unit test; editor calls it) |
| VG: panel wiring of formatter untested | defer | GUI panel outside Core unit pyramid; formatter SSOT already tested. |
## Design Notes

**Format (verified against M4L v1.0.2 + `P-Test.m1kp`):**
- 268 bytes = 134 × int16 LE; high byte is always `0x00` for in-range values; ASCII name looks like UTF-16LE but is not a text file format.
- Payload equals Matrix-1000 **decoded** patch parameter bytes (same 134 bytes SysEx 0x0D/0x01 would carry after nibble decode) — not a SysEx wrapper.
- Example: `P-Test.m1kp` → name `BNK2: 02`; all 134 values in `[0, 78]` for this sample.

**Signed amounts:** packed RAM already uses 8-bit two’s complement (`SysExEncoder` comment). Storing JS numbers via Max `writeint16` preserves negative amounts in the low byte when int16 ∈ [-128, 127].

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-ControlTests` -- unit tests green (or project’s usual test target name)
- `python3 Scripts/quality/lint_touched.py` -- clean on touched C++

**Manual checks:**
- Open Desktop `P-Test.m1kp` via in-scope surface; PATCH NAME / modules match the M4L editor; SAVE AS writes a loadable `.syx`.
