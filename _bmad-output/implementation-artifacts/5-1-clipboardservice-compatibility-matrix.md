---
organization: Ten Square Software
project: Matrix-Control
title: Story 5.1 — ClipboardService Compatibility Matrix
author: BMad Agent
status: done
baseline_commit: 20afdb136e63c57ddc0bb18d0ca44c6e9d8cb5d1
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - planning-artifacts/sprint-change-proposal-2026-06-18.md
  - implementation-artifacts/3-3-matrix-mod-init-defaults.md
  - implementation-artifacts/3-4-master-module-init-confirmation-path.md
  - project-context.md
created: 2026-06-18
updated: 2026-06-18
---

# Story 5.1: ClipboardService Compatibility Matrix

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want copy/paste to respect module type compatibility,
so that invalid pastes are prevented (FR-35, addendum § Clipboard).

## Acceptance Criteria

1. **Given** Epic 3 complete **When** `Core::ClipboardService` stores a module, full patch, or matrix-modulation snapshot **Then** three mutually exclusive clipboard modes exist: `Module`, `FullPatch`, `MatrixModulation`; the last successful copy replaces the previous mode and snapshot (last Copy wins — D-033).
2. **And** module compatibility matches the addendum matrix:
   - **ENV 1 ↔ ENV 2 ↔ ENV 3** — full parameter interchange (all int + choice descriptors for the source module copied to target).
   - **DCO 1 ↔ DCO 2** — partial interchange per D-060 (detailed rules below).
   - **LFO 1 ↔ LFO 2** — partial interchange per D-060 (detailed rules below).
   - Incompatible module pairs (e.g. DCO → ENV) → `canPasteModule(target)` returns `false`; `pasteModule` is a no-op returning `false`.
3. **And** full-patch snapshot captures the entire 134-byte `PatchModel` buffer (`SysExConstants::kPatchPackedDataSize`); matrix-modulation snapshot captures **only** bytes **104–133** (30 bytes = 10 buses × 3 bytes source/amount/destination).
4. **And** `pasteMatrixModulation` writes snapshot bytes into the target model's bytes 104–133 only — patch name (bytes 0–7) and all other PATCH parameters remain unchanged.
5. **And** `ClipboardService` lives in `Source/Core/Services/ClipboardService.{h,cpp}`, namespace `Core`, **zero GUI dependencies** (`Core ↛ GUI`). Descriptor-driven parameter access only — no duplicated SysEx offset tables.
6. **And** query API exposes mode + compatibility for Story 5.2: `getMode()`, `getSourceModuleKind()` (when mode is `Module`), `canPasteModule(PatchModuleKind)`, `canPasteFullPatch()`, `canPasteMatrixModulation()`, `hasContent()`.
7. **And** `Tests/Unit/ClipboardServiceTests.cpp` covers: ENV full interchange (at least one cross-pair round-trip), DCO partial rules (both directions — skip SYNC/DETUNE, WAVE SELECT NOISE guard, KEYBD/PORTA OFF guard), LFO partial rules (SPEED mod source mapping + AMP mod source mapping), matrix-mod snapshot isolation + round-trip, full-patch snapshot size, mode replacement (module copy after full-patch copy switches mode), incompatible pair rejection. Register `.cpp` in plugin + `Matrix-Control_Tests` CMake targets; full test suite passes.

## Tasks / Subtasks

- [x] **Define types and service API** (AC: #1, #5, #6)
  - [x] `ClipboardService.h` — `enum class ClipboardMode { Empty, Module, FullPatch, MatrixModulation }`
  - [x] `enum class PatchModuleKind { Dco1, Dco2, Env1, Env2, Env3, Lfo1, Lfo2 }` — seven PATCH EDIT I/C/P modules only (no MASTER modules in this story)
  - [x] Public API: `copyModule`, `copyFullPatch`, `copyMatrixModulation`, paste variants, compatibility queries, `hasContent`, getters
  - [x] Internal storage: module snapshot buffer (per-module byte subset or descriptor values), `std::array<uint8, 134>` for full patch, `std::array<uint8, 30>` for matrix mod (bytes 104–133)

- [x] **Implement compatibility matrix + partial paste** (AC: #2, #4)
  - [x] `canPasteModule` — static compatibility table (ENV triangle, DCO pair, LFO pair; else false)
  - [x] `copyModule` / `pasteModule` — descriptor-driven via `PluginDescriptors::PatchEditSection::*Module::kIntParameters` / `kChoiceParameters`
  - [x] DCO partial paste helper — skip rules per parameter ID (see Dev Notes table)
  - [x] LFO partial paste helper — map `kSpeedModByPressure` ↔ `kSpeedModByKeyboard`, `kAmplitudeModByRamp1` ↔ `kAmplitudeModByRamp2`; copy remaining params when names/roles align
  - [x] ENV paste — copy all descriptors from source module tables onto target module tables (values read/written through `PatchModel` descriptor API)
  - [x] `copyMatrixModulation` / `pasteMatrixModulation` — raw byte slice 104–133 via `PatchModel::data()` + `PackedFieldCodec::safeOffset`

- [x] **Implement full-patch snapshot** (AC: #3)
  - [x] `copyFullPatch` — `std::memcpy` entire 134-byte buffer from source model
  - [x] `pasteFullPatch` — replace entire target buffer (compatibility query only; ROM gating is Story 7.3)

- [x] **Unit tests + CMake** (AC: #7)
  - [x] `Tests/Unit/ClipboardServiceTests.cpp` — AAA structure, no hardware, no APVTS required for pure model tests
  - [x] Matrix mod test: set distinct values on bytes 104–133 + sentinel bytes outside range; paste; assert outside bytes unchanged
  - [x] Register `Source/Core/Services/ClipboardService.cpp` in plugin + test `CMakeLists.txt`

- [x] **Self-review** (AC: #5) — class ≤ 200 lines per file where practical; methods ≤ 15 lines; no GUI includes; no SysEx enqueue in this story (Story 7.2 wires paste → APVTS + dispatcher)

### Review Findings

- [x] [Review][Patch] Missing DCO2→DCO1 SYNC preservation test — AC7 requires DCO rules in both directions; code skips DCO1 `kSync` on cross-paste (`shouldSkipDcoChoiceParam`) but only DCO1→DCO2 DETUNE and DCO2→DCO1 NOISE/OFF are tested [`Tests/Unit/ClipboardServiceTests.cpp`]
- [x] [Review][Patch] Missing LFO2→LFO1 reverse cross-map test — reverse mappings exist in `resolveLfoSourceIntParamId` but only LFO1→LFO2 is exercised [`Tests/Unit/ClipboardServiceTests.cpp:221-241`]
- [x] [Review][Patch] `getSourceModuleKind()` unsafe when mode ≠ Module in release builds — fixed: returns `std::nullopt` when mode is not `Module`; mode-replacement test asserts contract [`Source/Core/Services/ClipboardService.h:42`]
- [x] [Review][Defer] `pasteModule` returns `true` even when all parameters are skipped (DCO guards) — deferred, implicit UX per spec; no partial-paste feedback in v1 [`Source/Core/Services/ClipboardService.cpp:235-249`]
- [x] [Review][Defer] Cross-module paste relies on `displayName` matching — deferred, same descriptor-driven pattern as Epic 3 init services; SSOT invariant on display names [`Source/Core/Services/ClipboardService.cpp:322-355`]
- [x] [Review][Defer] No thread-safety contract documented — deferred, aligned with other Core services (message-thread assumption until 7.2 wiring) [`Source/Core/Services/ClipboardService.h`]
- [x] [Review][Defer] `ClipboardService.cpp` exceeds 200-line self-review target (467 lines) — deferred, helpers in anonymous namespace; split optional if file grows in 7.2 [`Source/Core/Services/ClipboardService.cpp`]

## Dev Notes

### What ClipboardService IS — and what it is NOT

`ClipboardService` is the **Core in-memory clipboard** for PATCH EDIT module copy/paste, Internal Patches full-patch copy/paste, and Matrix Modulation section copy/paste. It owns snapshot storage and compatibility logic.

It must **NOT** in this story:
- Push `pasteEnabled` APVTS mirror properties or gray GUI buttons (Story **5.2**)
- Add Matrix Modulation section I/C/P GUI widgets or descriptors (Story **5.3**)
- Wire `PluginProcessor::valueTreePropertyChanged` or `ActionDispatcher` / `ModuleActionHandler` (Story **7.2**)
- Enqueue SysEx or touch `ApvtsPatchMapper` / `MatrixModBusParameterSysExDispatcher` (Story **7.2** applies paste to live edit path with suppress/coalesce pattern from Epic 3.3)
- Implement MASTER module clipboard (MASTER headers have I/C/P widgets in GUI but FR-35 module matrix covers PATCH EDIT modules only; matrix-mod mode grays MASTER Paste in 5.2)
- Implement ENV shape-only copy (D-061 deferred)
- Show footer messages for partial paste subtleties (implicit UX — user manual)

[Source: D-033, D-060 in `.decision-log.md`; FR-35 in `prd.md`; addendum § Clipboard]

### Epic 5 cross-story map

| Story | Delivers | Relation to 5.1 |
|---|---|---|
| **5.1 (this)** | `ClipboardService` Core + unit tests | Snapshot storage + compatibility matrix |
| **5.2** | `pasteEnabled` APVTS properties + GUI gray rules | Consumes query API from this story |
| **5.3** | Matrix Mod section C/P GUI + descriptors | Stamps APVTS timestamp properties only |
| **7.2** | `ModuleActionHandler` I/C/P routing | Calls `ClipboardService` + syncs APVTS + SysEx |

### Clipboard modes (authoritative — addendum + D-095)

| Mode | Trigger (future 7.2) | Snapshot scope | Paste targets (compat query) |
|---|---|---|---|
| **Module** | PATCH module Copy (DCO/ENV/LFO) | All descriptor values for source module | Compatible PATCH modules only |
| **FullPatch** | Internal Patches Copy | Full 134-byte `PatchModel` | Internal Patches Paste only (5.2 grays PATCH module Paste) |
| **MatrixModulation** | Matrix Mod section Copy | Bytes **104–133** only | Matrix Mod section Paste only (5.2 grays PATCH + MASTER module Paste) |

Last Copy replaces mode until next Copy (mutually exclusive — D-033).

### Module compatibility matrix

| Source ↓ / Target → | DCO1 | DCO2 | ENV1 | ENV2 | ENV3 | LFO1 | LFO2 |
|---|---|---|---|---|---|---|---|
| **DCO1** | ✓ partial | ✓ partial | ✗ | ✗ | ✗ | ✗ | ✗ |
| **DCO2** | ✓ partial | ✓ partial | ✗ | ✗ | ✗ | ✗ | ✗ |
| **ENV1/2/3** | ✗ | ✗ | ✓ full | ✓ full | ✓ full | ✗ | ✗ |
| **LFO1** | ✗ | ✗ | ✗ | ✗ | ✗ | ✓ partial | ✓ partial |
| **LFO2** | ✗ | ✗ | ✗ | ✗ | ✗ | ✓ partial | ✓ partial |

✓ = `canPasteModule` true; ✗ = false.

### ENV full interchange

When source and target are any ENV pair (1↔2, 1↔3, 2↔3, or same module), copy **every** int and choice descriptor from source module tables to target module tables using `PatchModel::getValue` / `setValue` / `getChoiceIndex` / `setChoiceIndex`.

Descriptor SSOT:
- `PluginDescriptors::PatchEditSection::Envelope1Module::{kIntParameters,kChoiceParameters}`
- `Envelope2Module`, `Envelope3Module` — identical parameter sets (different byte offsets).

### DCO partial paste rules (D-060)

Copy compatible parameters by **matching parameter role** across DCO1/DCO2 descriptor tables. Apply these **skip / guard** rules on cross-module paste (DCO1↔DCO2):

| Rule | Parameter(s) | Behavior |
|---|---|---|
| Skip SYNC/DETUNE cross-mapping | DCO1 `kSync` (choice, offset 25); DCO2 `kDetune` (int, offset 19) | Never copy between modules — leave target unchanged |
| WAVE SELECT subset | DCO1/DCO2 `kWaveSelect` | DCO2 has 5 choices (includes **NOISE**); DCO1 has 4. When source index is NOISE (DCO2 index 4) and target is DCO1 → **skip** (do not write). When copying to DCO2, indices 0–3 map directly; DCO1→DCO2 NOISE never occurs from source |
| KEYBD/PORTA guard | DCO1/DCO2 `kKeyboardPortamento` | DCO1 choices: KEYBOARD, PORTAMENTO (2). DCO2 choices: OFF, PORTAMENTO, KEYBOARD (3). Copy PORTAMENTO↔PORTAMENTO and KEYBOARD↔KEYBOARD. When source is DCO2 **OFF** and target is DCO1 → **skip** (do not write OFF onto DCO1) |
| All other paired params | Frequency, PW, wave shape, lever mods, levers, key click, etc. | Copy when descriptor exists on both sides with same semantic role |

Int params to always copy cross DCO: `kFrequency`, `kFrequencyModByLfo1`, `kPulseWidth`, `kPulseWidthModByLfo2`, `kWaveShape`.  
Choice params (except rules above): `kLevers`, `kKeyClick`, `kWaveSelect` (with NOISE guard), `kKeyboardPortamento` (with OFF guard).

Parameter IDs: `PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::*` / `Dco2Module::*`.

[Source: D-060; addendum § Clipboard; `PluginDescriptorsPatchEdit.cpp` DCO1/DCO2 sections]

### LFO partial paste rules (D-060)

| Source param | Target param | Notes |
|---|---|---|
| LFO1 `kSpeedModByPressure` (offset 102) | LFO2 `kSpeedModByKeyboard` (offset 103) | Cross-map on LFO1↔LFO2 paste |
| LFO2 `kSpeedModByKeyboard` | LFO1 `kSpeedModByPressure` | Reverse mapping |
| LFO1 `kAmplitudeModByRamp1` (offset 97) | LFO2 `kAmplitudeModByRamp2` (offset 98) | Cross-map |
| LFO2 `kAmplitudeModByRamp2` | LFO1 `kAmplitudeModByRamp1` | Reverse mapping |
| All other int/choice params with matching semantic role | Direct copy | Speed, retrigger, amplitude, waveform, trigger mode, lag, sample input |

Same-module paste (LFO1→LFO1): all params copy normally.

### Matrix Modulation snapshot (bytes 104–133)

Layout (from Story 3.3 / 2.6):

| Bus n (0–9) | Source byte | Amount byte | Destination byte |
|---|---|---|---|
| n | `104 + 3n` | `105 + 3n` | `106 + 3n` |

- `copyMatrixModulation`: copy 30 bytes starting at offset 104 from source `PatchModel::data()`.
- `pasteMatrixModulation`: write 30 bytes into target at offset 104 only.
- `canPasteMatrixModulation()`: true only when mode is `MatrixModulation` and snapshot present.
- No cross-compatibility with module or full-patch modes.

Use `Core::PackedFieldCodec::safeOffset` for bounds safety (same as `MatrixModInitServiceTests`).

[Source: `3-3-matrix-mod-init-defaults.md`; sprint-change-proposal-2026-06-18.md; D-095]

### Recommended API shape

```cpp
// Source/Core/Services/ClipboardService.h
namespace Core
{
    enum class ClipboardMode { Empty, Module, FullPatch, MatrixModulation };
    enum class PatchModuleKind { Dco1, Dco2, Env1, Env2, Env3, Lfo1, Lfo2 };

    class ClipboardService
    {
    public:
        bool hasContent() const noexcept;
        ClipboardMode getMode() const noexcept;
        PatchModuleKind getSourceModuleKind() const; // valid when mode == Module

        void copyModule(PatchModuleKind source, const PatchModel& model);
        void copyFullPatch(const PatchModel& model);
        void copyMatrixModulation(const PatchModel& model);

        bool canPasteModule(PatchModuleKind target) const noexcept;
        bool canPasteFullPatch() const noexcept;
        bool canPasteMatrixModulation() const noexcept;

        bool pasteModule(PatchModuleKind target, PatchModel& model);
        bool pasteFullPatch(PatchModel& model);
        bool pasteMatrixModulation(PatchModel& model);

    private:
        // snapshot storage + partial-paste helpers
    };
}
```

Keep paste methods **pure model operations** — return `false` on incompatible target without mutating `model`.

### Descriptor access pattern (reuse — do not reinvent)

Follow `MatrixModInitService::resetBusInModel` and `MasterModuleInitService::copyModuleFromInitTemplate`:

1. Resolve source/target module descriptor vectors via `PatchModuleKind` → namespace switch.
2. Read values from snapshot (stored at copy time) or live model.
3. Write through `PatchModel::setValue` / `setChoiceIndex` with target descriptors.

Do **not** add parallel offset tables — `sysExOffset` lives only in `PluginDescriptors`.

### Project structure notes

| Artifact | Path |
|---|---|
| Service header | `Source/Core/Services/ClipboardService.h` |
| Service implementation | `Source/Core/Services/ClipboardService.cpp` |
| Unit tests | `Tests/Unit/ClipboardServiceTests.cpp` |
| CMake registration | `CMakeLists.txt` — plugin sources ~line 126 (near `DeviceTypeRegistry.cpp`); test sources ~line 305 |

Architecture target path confirmed: `architecture.md` E5 → `Source/Core/Services/ClipboardService.*`.

### Testing requirements

Per NFR-1 and architecture AD-8:

- **AAA** structure in every test method
- **No hardware**, no MIDI ports, no APVTS required for core matrix tests (construct `PatchModel`, set values via descriptors, exercise service)
- High-priority unit coverage per D-060: one test per module pair category minimum
- Suggested test cases:
  1. `env1ToEnv3_fullCopy` — all ENV params match after paste
  2. `dco1ToDco2_skipsSyncAndDetune` — DCO1 SYNC unchanged on DCO2; DCO2 DETUNE unchanged when pasting from DCO1
  3. `dco2ToDco1_skipsNoiseAndOff` — WAVE SELECT NOISE not applied; KEYBD/PORTA OFF not applied
  4. `lfo1ToLfo2_mapsPressureToKeyboard` — speed mod + amp mod cross-mapping
  5. `matrixMod_snapshotIsolation` — bytes 0–103 and 134+ (if any) unchanged; 104–133 match snapshot
  6. `fullPatch_captures134Bytes`
  7. `modeReplacement_moduleAfterFullPatch`
  8. `dco1ToEnv1_rejected`

Register test class: `static ClipboardServiceTests clipboardServiceTests;`

### Architecture compliance

| Invariant | Application |
|---|---|
| `Core ↛ GUI` | No JUCE GUI modules; `juce_core` + existing Core/Shared headers only |
| Descriptor SSOT | All offsets/parameter IDs from `PluginDescriptors` / `PluginIDs` |
| No SysEx in service | Paste to model only; 7.2 adds mapper + dispatcher |
| Clean Code limits | Methods ≤ 15 lines, ≤ 3 params; extract private helpers for DCO/LFO partial logic |
| Brownfield | Do not move `MatrixModInitService` to `Services/` — Init services stay in `Core/Init/` |

### Previous story intelligence (Epic 3)

| Story | Relevant learning |
|---|---|
| **3.3 (done)** | Matrix Mod bytes 104–133 layout; descriptor triplet pattern; test harness with `PatchModel` + byte assertions at `sysExOffset` |
| **3.4 (done)** | Service pattern: Core-only, message-thread assumption, CMake dual registration, no GUI in service story |

Copy/paste SysEx burst after matrix-mod paste will reuse Epic 3.3 suppress pattern in 7.2 (`suppressMatrixModParameterSysEx_` → push all buses → dispatch all buses).

### Git intelligence

Recent commits confirm Epic 3 closure and Core service patterns:
- `60ec966` — Epic 3 done in sprint status
- `e92a0fb` — `MasterModuleInitService` + confirmation path (service + unit tests + processor wiring split across stories — **5.1 should NOT add processor wiring**)
- `dcf721d` — `DeviceTypeRegistry` / `DeviceMemoryLimits` in `Source/Core/Services/` (placement precedent for `ClipboardService`)

### Latest tech information

- **JUCE 8.0.12** — no clipboard-specific API needed; in-memory snapshots only (not OS clipboard).
- **C++17** — use `std::array`, `enum class`, `std::memcpy` for byte snapshots.
- No external library additions.

### Project context reference

- `_bmad-output/project-context.md` — dependency direction, descriptor SSOT, Clean Code limits, test pyramid
- `CONVENTIONS.md` §8.5 — AAA / F.I.R.S.T. tests
- `Documentation/Development/software-development-quality-principles.md` — design principles

### References

- [Source: `_bmad-output/planning-artifacts/epics.md` § Epic 5, Story 5.1]
- [Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md` § Clipboard compatibility matrix]
- [Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md` D-033, D-060, D-095]
- [Source: `_bmad-output/planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md` § ClipboardService, AD-8, E5 mapping]
- [Source: `Source/Shared/Definitions/PluginDescriptorsPatchEdit.cpp` — DCO/ENV/LFO module descriptor tables]
- [Source: `Source/Core/Init/MatrixModInitService.cpp` — descriptor-driven model mutation pattern]
- [Source: `Tests/Unit/MatrixModInitServiceTests.cpp` — PatchModel byte assertion harness]

## Dev Agent Record

### Agent Model Used

claude-4.6-sonnet-medium-thinking

### Debug Log References

- LFO cross-paste: int params with different parameterIds but same displayName required displayName fallback after explicit cross-map IDs.

### Completion Notes List

- Implemented `Core::ClipboardService` with three mutually exclusive clipboard modes (Module, FullPatch, MatrixModulation) and last-copy-wins semantics.
- Module compatibility: ENV full interchange, DCO partial (SYNC/DETUNE skip, NOISE guard, KEYBD/PORTA OFF guard), LFO partial (speed/amp mod cross-mapping + displayName alignment).
- Matrix-mod snapshot: bytes 104–133 only; patch name and other params preserved on paste.
- Full-patch snapshot: 134-byte `PatchModel` buffer memcpy.
- 8 unit tests in `ClipboardServiceTests.cpp`; full `Matrix-Control_Tests` suite green.
- Code review (2026-06-18): added DCO2→DCO1 SYNC test, LFO2→LFO1 reverse cross-map test; `getSourceModuleKind()` returns `std::nullopt` when mode ≠ Module (10 tests total).

### File List

- `Source/Core/Services/ClipboardService.h` (added)
- `Source/Core/Services/ClipboardService.cpp` (added)
- `Tests/Unit/ClipboardServiceTests.cpp` (added)
- `CMakeLists.txt` (modified — plugin + test target registration)

### Change Log

- 2026-06-18: Story 5.1 implementation — ClipboardService Core service + unit tests (AC #1–#7).
- 2026-06-18: Code review patches — 2 DCO/LFO direction tests, `getSourceModuleKind()` optional guard.
