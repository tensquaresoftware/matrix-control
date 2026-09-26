---
organization: Ten Square Software
project: Matrix-Control
title: M1KM Master Import
author: BMad Agent
status: done
route: dispatch
review_loop_iteration: 0
baseline_commit: 592a108850502b4344c6e1144436ffd939e4ad95
created: 2026-09-25
updated: 2026-09-25
context:
  - {project-root}/_bmad-output/project-context.md
  - {project-root}/_bmad-output/implementation-artifacts/spec-m1kp-patch-import.md
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Users of the legacy Matrix-1000 Editor (Max for Live) still have master settings as `.m1km` files. Matrix-Control Settings LOAD only accepts `.syx`, so those masters cannot be reopened.

**Approach:** Treat `.m1km` as a read-only alternate on-disk encoding of the same 172 packed master bytes already used by `MasterModel`. Decode into the existing Settings master LOAD path; never write `.m1km` (SAVE AS stays `.syx` only). Do not build a Groups / cascade editor in this chantier. On every `.m1km` entry path, ask the user whether to load Master Edit–useful settings only (reset Groups/cascade) or the full master (preserve Groups/cascade).

## Boundaries & Constraints

**Always:**
- `.m1km` on disk = exactly **172 little-endian int16** values (**344 bytes**), no magic header / version / checksum (M4L `File.writeint16` / `readint16`).
- Decode to 172 packed bytes: each int16 must be in **`[-128, 255]`** (covers signed Master Tune / Transpose and Groups masks `0…255`); packed byte = low 8 bits. Reject wrong size or out-of-range int16.
- **Groups / cascade choice (decision):** every successful `.m1km` decode that is about to commit must show a **blocking** modal before applying to the live master / sending SysEx. Same modal contract for Settings LOAD now and for future Master drag-drop (drop is out of this chantier’s implementation, but the dialog must be reusable).
  - Body (ASCII): `This .m1km Master file may include Groups and cascade data. Matrix-Control does not edit those yet, but the Matrix-1000 still uses them.\n\nChoose whether to load Master settings only (Groups/cascade reset) or the full master (Groups/cascade kept).\nCancel leaves the current Master unchanged.`
  - Buttons: `Master settings only` | `Full Master (including groups/cascade)` | `Cancel`
  - **Master settings only:** keep Master Edit–mapped param bytes from the file; set Groups (36–160) and cascade (166–168) from `InitDefaults` master buffer before commit + `0x03` dispatch.
  - **Full Master:** commit all 172 decoded bytes as-is, then dispatch.
  - **Cancel:** no model change, no SysEx, no success footer.
- After the user confirms a load option, reuse the existing Settings LOAD success path: `MasterModel.loadFrom` → `ApvtsMasterMapper::bufferToApvts` → full Master SysEx `0x03` dispatch. Source `.m1km` is never rewritten on open.
- **Surface (this chantier):** Settings → LOAD Master. File-chooser **title** stays `Load Master`. Chooser **filter** accepts `*.syx;*.m1km`. Button label stays `LOAD`. Contextual help: `SETTINGS: Loads a Master .syx or legacy .m1km file into the editor.` SAVE AS remains `.syx` only (no `.m1km` in save copy or filter).
- Unit-test decode + loader / policy gates with a fixture copied from M4L Study masters (e.g. `Master 1.m1km` and/or Groups On/Off).
- Groups bytes (36–160) and cascade fields (166–168) remain **opaque** in Master Edit UI (no new controls).

**Never:**
- Do not add SAVE AS / MasterInit writers for `.m1km`.
- Do not change `PluginDescriptors` master layout or invent a second in-RAM master model.
- Do not ship a Groups / multi-unit cascade editor or product parity with M4L master Groups UI.
- Do not implement Master drag-drop in this chantier (deferred; must call the same `.m1km` choice modal when built).
- Do not reopen `.m1kp` design except minimal shared patterns (codec twin, extension gate, confirmation-dialog style).
- Do not rename syx-branded APIs in this chantier (already deferred).
- Do not auto-pick Groups policy without the modal (no silent preserve/reset default for `.m1km`).

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Happy `.m1km` + Master settings only | Valid 344-byte file; user picks that button | Mapped params from file; Groups/cascade = InitDefaults; APVTS updated; full `0x03`; footer MASTER loaded | N/A |
| Happy `.m1km` + Full Master | Valid file; user picks Full Master | All 172 bytes from file; APVTS + `0x03`; footer MASTER loaded | N/A |
| `.m1km` + Cancel | Valid file; user cancels modal | Live master unchanged; no SysEx; no success footer | N/A |
| Happy path `.syx` | Existing valid master `.syx` | Unchanged behaviour (no Groups modal) | N/A |
| Wrong size | Length ≠ 344 | Reject before modal; live model unchanged | Settings footer MASTER file failed |
| Out of range | Any int16 outside [-128, 255] | Reject before modal; model unchanged | Same |
| Empty / unreadable | Missing or I/O failure | Reject before modal | Same |
| After edit + SAVE AS | Loaded from `.m1km`, user saves | Writes `.syx` only; `.m1km` untouched | Existing save errors |

</frozen-after-approval>

## Code Map

- `Source/Core/Services/PatchM1kpCodec.{h,cpp}` — twin pattern for `MasterM1kmCodec` (344 B / 172 int16 / range gate / `decodeToPacked`).
- `Source/Core/Init/InitTemplateLoader.{h,cpp}` — `loadMasterFromFile` / `decodeMasterIntoModel` currently SysEx-only; branch `.m1km` → codec → packed buffer; keep `kUserFile` success contract used by `InitTemplateWriter::loadMasterFromUserFile`.
- Core policy helper (new small function/type near loader or services) — apply **Master settings only** vs **Full Master** to a 172-byte scratch (copy InitDefaults into offsets 36–160 and 166–168 for the reset path); unit-test without GUI.
- `Source/Core/Init/InitTemplateWriter.cpp` — user LOAD scratch + commit-on-`kUserFile`; SAVE AS stays `encodeMasterSysEx` + `.syx`.
- `Source/Core/PluginProcessorInitTemplates.cpp` — `loadMasterFromUserFile` success path (mapper + `dispatchFull`); orchestrate decode → modal gate → policy → commit (or editor callback pattern matching other confirms).
- `Source/GUI/PluginEditorSettings.cpp` — LOAD chooser filter `*.syx;*.m1km`; title stays `Load Master`; SAVE AS `*.syx`; open reusable `.m1km` choice dialog before processor commit.
- `Source/GUI/Dialogs/` — new blocking confirm dialog (mirror `M1kpSiblingSyxOverwriteConfirm` / Master init confirm patterns): frozen title/body/buttons from Intent.
- `Source/Shared/Definitions/PluginDisplayNames.h` — dialog strings + `kMasterLoad` help update (ASCII); LOAD button label unchanged; SAVE AS copy unchanged regarding `.m1km`.
- `Source/Core/Models/MasterModel.*` / `InitDefaults` — 172 B packed SSOT; Groups defaults `0xFF` at 36–160; cascade defaults in init bytes — SSOT for reset path.
- `Source/Shared/Definitions/PluginDescriptorsMasterEdit.cpp` — ~22 SysEx-mapped params; no Groups descriptors — do not add.
- `Source/Core/MIDI/SysEx/*` — master packed 172 / opcode `0x03` for `.syx` encode/decode only.
- M4L reference (read-only): `m1k.dataManager.js`, `m1k.masterModel.js`, `m1k.masterController.js`.
- Fixtures: Study `Master 1.m1km` (+ optional Groups On/Off) → `Tests/Fixtures/…`.
- `Tests/Unit/` — codec + Groups-policy unit tests; loader reject-before-modal; extend `InitTemplateWriterTests` as needed.
- `CMakeLists.txt` / `Tests/CMakeLists.txt` — register new sources.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/Services/MasterM1kmCodec.{h,cpp}` -- decode/validate 344-byte `.m1km` → 172 packed -- format gate
- [x] `Source/Core/` (small policy helper) -- Full vs Master-settings-only (reset Groups 36–160 + cascade 166–168 from InitDefaults) -- testable without GUI
- [x] `Source/Core/Init/InitTemplateLoader.{h,cpp}` (+ Writer/Processor as needed) -- `.m1km` on user master load; `.syx` unchanged; invalid rejects before modal -- Settings LOAD I/O
- [x] `Source/GUI/Dialogs/` + `PluginEditorSettings.cpp` -- blocking `.m1km` choice modal; chooser filter `*.syx;*.m1km`; title `Load Master`; SAVE AS `.syx` -- surface
- [x] `Source/Shared/Definitions/PluginDisplayNames.h` -- frozen modal copy + LOAD help mentioning `.syx` / `.m1km` (ASCII) -- honesty
- [x] `Tests/Fixtures/` + unit tests -- Study fixture; codec + policy + reject cases -- regression
- [x] `CMakeLists.txt` / `Tests/CMakeLists.txt` -- register new sources -- build

**Acceptance Criteria:**
- Given a valid `.m1km`, when the user Loads it from Settings, then the Groups/cascade modal appears before any live master change.
- Given the user picks `Master settings only`, when load completes, then Master Edit params match the file and Groups/cascade bytes match InitDefaults, and a full `0x03` is sent.
- Given the user picks `Full Master (including groups/cascade)`, when load completes, then all 172 packed bytes match the file decode and a full `0x03` is sent.
- Given the user picks `Cancel`, when the modal closes, then the live master and synth state are unchanged.
- Given an invalid `.m1km`, when Load is attempted, then no modal appears, the live master is unchanged, and the failure footer is shown.
- Given a master loaded from `.m1km`, when the user SAVE AS, then only a `.syx` is written and the original `.m1km` is unchanged.
- Given Settings LOAD contextual help, when shown, then it mentions Master `.syx` and legacy `.m1km`.

### Review Findings

- [x] [Review][Patch] One-shot load callbacks on MasterM1kmLoadChoiceDialog (prevent double commit / SysEx) [Source/GUI/Dialogs/MasterM1kmLoadChoiceDialog.cpp:89]
- [x] [Review][Patch] Assert All Groups Off fixture Groups/cascade differ from InitDefaults before policy reset [Tests/Unit/MasterM1kmCodecTests.cpp:333]
- [x] [Review][Patch] Automate Settings-style commitMasterM1kmUserLoad success path (APVTS + 0x03 + footer) [Tests/Unit/MasterM1kmCommitPathTests.cpp]
- [x] [Review][Defer] tryDecodeMasterM1kmUserFile live-master-untouched before modal untested at processor — deferred: already tracked in deferred-work.md; Core decode-without-commit covers shared helper; Matrix-Control_Tests does not link processor init templates

**Rejected (this review):**
- `false` — Failed-decode scratch InitDefaults pollution: local scratch discarded; live master fail-closed via Writer / tryDecode.
- `false` — closeSettingsWindow leaves choice dialog up: true at 75095c63; already fixed on main in 609c50bc.
- `false` — loadMasterFromUserFile hard-rejects .m1km: intentional fail-closed when modal is bypassed.
- `false` — Oversized on-disk size gate untested: loader_invalidM1kmRejectsBeforeSuccess covers truncated (≠344) file.
- `false` — Spec triage / Verification / Code Map hygiene: would require editing the spec under review.
- `low` — All Groups Off unused fixture: rejected (All Groups On already covers policy; prior Build triage same call).
- `low` — Case-only chooser filter `*.syx;*.m1km`: matches project-wide chooser pattern; unlikely on macOS everyday use.
- `low` — decode_validStudyFixture lacks golden bytes: happy decode + full-master memcmp already pin packing.

## Implementation Notes

- Added `MasterM1kmCodec` + `MasterM1kmLoadPolicy`; Settings LOAD decodes `.m1km` before the Groups/cascade modal, then commits with the chosen policy.
- `InitTemplateWriter::loadMasterFromUserFile` requires an explicit policy for `.m1km` (no silent default). SAVE AS remains `.syx`-only via `writeMasterToFile`.
- Surface: `MasterM1kmLoadChoiceDialog` + chooser filter `*.syx;*.m1km`; help string updated.
- Fixtures: `Tests/Fixtures/Masters/Master 1.m1km`, `All Groups On.m1km`, `All Groups Off.m1km`.
- Unit coverage: `MasterM1kmCodecTests` (codec, policy, loader reject-before-modal, writer policy + SAVE AS, decode-without-commit / Cancel-equivalent, help copy mentions `.syx` / `.m1km`).
- Review patches: align modal body newline to frozen Intent; mutual exclusion for Settings/About/EPROM/bank vs `.m1km` dialog; early `.m1km` size reject; `loadPackedIntoModel` shared by Writer + processor commit with unit tests; drop unused dialog include.
- Deferred: Master drag-drop (same modal); `tryDecodeMasterM1kmUserFile` processor live-untouched (see `deferred-work.md`).

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence |
|---------|---------|----------|
| Blind: frozen body `\n` vs shipped `\n\n` before Cancel | medium | Confirmed in `PluginDisplayNames.h` `MasterM1kmLoadChoice::kBody` vs frozen Intent; shipped copy diverges from approved string. |
| Blind: `loadMasterFromUserFile` hard-fails any `.m1km` | false | Intentional fail-closed for callers that skip the modal; Settings uses `tryDecode` + `commitMasterM1kmUserLoad` instead. |
| Blind: missing-policy uses `kFileInvalid` | low | Real but developer-only; rejected (unlikely everyday; not worth new enum surface). |
| Blind: `openSettingsWindow` can bury `.m1km` overlay | medium | Confirmed: `openSettingsWindow` does not close `MasterM1kmLoadChoiceDialog` while `toFront` on Settings. |
| Blind: About / EPROM / bank openers lack mutual exclusion with `.m1km` | medium | Confirmed: `openMasterM1kmLoadChoiceDialog` closes About/init/defrag only; EPROM/bank openers do not dismiss `.m1km`. Same root as Settings bury. |
| Blind: `All Groups Off.m1km` unused in tests | low | True; `All Groups On` already covers Groups policy. Rejected. |
| Blind: study fixture decode lacks known-byte asserts | low | True; reject (happy decode + writer full-master memcmp already pin packing). |
| Blind: processor commit/decode APIs untested | medium | Same as Verification Gap #1 / #2; routed there. |
| Blind: modal “reset” ambiguous vs InitDefaults | false | Approved frozen wording; not a code defect. |
| Blind: bytes 161–165 left undocumented in partial load | false | Intentional opaque slots; policy only resets Groups + cascade as frozen. |
| Blind: unused `#include <memory>` in dialog header | low | Confirmed; no `unique_ptr` in header — trivial delete. |
| Blind: body may clip at high UI scale | maybe-false | No reproduction; design height matches other confirms; reject. |
| Blind: patch lacks recorded verification evidence | false | Process/meta; would require editing the spec artifact. |
| Blind: Code Map cites `Tests/CMakeLists.txt` | false | Spec hygiene only; reject (do not edit spec for this). |
| Edge: full-file read before 344-byte size gate | medium | Confirmed `loadSysExBytes` → `loadFileAsData` before decode; `.m1kp` already early-sizes. |
| Edge: EPROM/bank during `.m1km` choice | medium | Same mutual-exclusion root as Blind Settings/About. |
| Edge: frozen body newline mismatch | medium | Same as Blind frozen-body finding. |
| VG: Settings commit path (`commitMasterM1kmUserLoad`) untested | medium | Pre-verified; writer policy tests do not call processor commit. |
| VG: `tryDecodeMasterM1kmUserFile` live-untouched not pinned | defer | Core decode-without-commit covered; processor glue not in `Matrix-Control_Tests`. |
| VG other: loader invalid “live unchanged by construction” | low | Weak assert; reject. |
| VG other: writer `.m1km`+policy unused by Settings | false | Parallel Core API kept for tests / non-GUI callers; by design. |

## Design Notes

**Format (verified against M4L v1.0.2 + Study samples):**
- 172 × int16 LE = 344 bytes; `MASTER_TYPE = "M1KM"` is Max dialog registration only, not in-file.
- Logical layout matches Oberheim / MC packed master: UI params sparse; Groups G000–G124 at 36–160; cascade at 166–168; Unison / Volume Invert / Memory Protect at 169–171.
- Study samples (`Master 1.m1km`, Groups On/Off) are all 344 bytes.

**Frozen UI copy (Settings + modal):**
- File chooser title: `Load Master`
- Help: `SETTINGS: Loads a Master .syx or legacy .m1km file into the editor.`
- Modal body + buttons: see Boundaries (decision block).

**Future Master drag-drop:** out of scope here; must reopen the same modal for `.m1km` (append to `deferred-work.md` at implementation close if not already tracked).

**Why not a second master model:** Decode → existing `MasterModel` 172-byte buffer. Groups remain opaque bytes with an explicit user choice at import time.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- build succeeds
- Unit-test filter for MasterM1km / InitTemplate / policy -- pass
- `python3 Scripts/quality/lint_touched.py` -- clean on touched C++

**Manual checks:**
- Settings LOAD a Study `.m1km`; exercise all three modal buttons; confirm Master Edit + synth behaviour for only vs full.
- SAVE AS writes a loadable `.syx`; re-LOAD that `.syx` (no modal).
