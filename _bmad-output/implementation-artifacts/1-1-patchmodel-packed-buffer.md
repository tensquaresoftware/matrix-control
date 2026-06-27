---
story_key: 1-1-patchmodel-packed-buffer
epic: 1
story: 1
status: done
baseline_commit: 3813b56
---

# Story 1.1: PatchModel Packed Buffer

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want a 134-byte packed patch buffer in Core,
So that patch data matches the Oberheim Matrix-1000 layout for SysEx and file I/O (FR-49).

## Acceptance Criteria

1. **Given** descriptor offsets in `PluginDescriptors` (`sysExOffset` on each Patch-Edit `IntParameterDescriptor` / `ChoiceParameterDescriptor`) **When** `PatchModel` reads/writes its 134-byte buffer **Then** every parameter is stored at the byte index declared by its descriptor — **no offset table is duplicated** anywhere outside `PluginDescriptors`.
2. **And** the buffer size is taken from `SysExConstants::kPatchPackedDataSize` (134) — not a new literal.
3. **And** patch name bytes 0–7 use the Matrix 6-bit-ASCII rule (character = lower 6 bits of ASCII); `getName()` / `setName(juce::String)` round-trip through those 8 bytes, enforcing **max 8 chars** (longer input truncated, shorter input space/`0x20`-padded — confirm padding byte against reference samples).
4. **And** signed fields (e.g. byte 19 DCO 2 Detune = 6-bit signed, byte 86 = 7-bit signed) round-trip correctly: the on-wire byte encoding is validated against real reference patches, not assumed.
5. **And** a JUCE `UnitTest` round-trips **at least three** reference patches (`Patch 5`, `Patch 66`, `Patch 808`) decode→`PatchModel`→re-encode and asserts the **packed 134-byte buffer is byte-identical** to the decoded input.
6. **And** the test fixtures live in the **versioned** path `Tests/Fixtures/Patches/` (committed to the repo); the originals currently sit only in gitignored `_local/` (see Dev Notes).
7. **And** `PatchModel` lives in `Source/Core/Models/PatchModel.{h,cpp}`, has **zero GUI dependencies**, and respects Clean Code limits (class ≤ 200 lines, methods ≤ 15 lines, ≤ 3 params, no magic numbers).

## Tasks / Subtasks

- [x] **Source the test fixtures** (AC: #5, #6) — *do this first, it gates the test*
  - [x] Create `Tests/Fixtures/Patches/` (PascalCase — house convention for new folders; `Tests/Fixtures/Masters/` will follow in Story 1.2)
  - [x] Copy `Patch 5.syx`, `Patch 66.syx`, `Patch 808.syx` from `_local/TestLab/SysEx Data/Original/Patch/` into `Tests/Fixtures/Patches/`
  - [x] Verify each is a full 275-byte single-patch message (`F0 10 06 01 <num> …`) — skip the `(Truncated)` / `(Copy)` variants
  - [x] Remove stale `tests/` entry from `.gitignore` (collided with versioned `Tests/` on case-insensitive macOS, would have blocked committing fixtures)
- [x] **Establish the unit-test runner target** (AC: #5) — *in scope for this story; Epic-1 enabler, see Dev Notes*
  - [x] Add a `juce_add_console_app` test runner that compiles `Tests/Unit/*.cpp` + the Core sources under test, runs `UnitTestRunner`, returns non-zero on failure (gated by `-DMATRIX_BUILD_TESTS=ON`)
  - [x] Keep it **out** of the AU/VST3/Standalone targets (static `juce::UnitTest` registrars crash hosts)
  - [x] Confirm existing `SysExParserTests` / `SysExEncoderTests` / `MidiManagerTests` build and run under the new target (they build and run; see Dev Agent Record about one pre-existing SysExParser assertion failure, flagged separately — out of scope)
- [x] **Implement `PatchModel`** (AC: #1, #2, #3, #4, #7)
  - [x] `Source/Core/Models/PatchModel.h` — `#pragma once`, 134-byte `std::array<juce::uint8, SysExConstants::kPatchPackedDataSize>` buffer (`kBufferSize` constant), accessors
  - [x] Raw buffer access: `const juce::uint8* data() const`, `juce::uint8* data()`, `loadFrom()` + `kBufferSize` → for `SysExEncoder::encodePatchSysEx` / `SysExDecoder::decodePatchSysEx` interop
  - [x] Descriptor-driven read/write: `getValue/setValue(IntParameterDescriptor)` + `getChoiceIndex/setChoiceIndex(ChoiceParameterDescriptor)` using `sysExOffset`; shared `decodeField`/`encodeField` helpers (zero duplication)
  - [x] Name accessors: `getName()` / `setName()` over bytes 0–7 (Matrix charset helper `decodeNameChar`)
  - [x] Signed-field handling: `isSignedField` / `signBitPosition` derive two's-complement width from descriptor min/max (validated by AC #4)
  - [x] Register both files in `CMakeLists.txt` (test target source list)
- [x] **Write `Tests/Unit/PatchModelTests.cpp`** (AC: #3, #4, #5)
  - [x] `class PatchModelTests : public juce::UnitTest` (matches existing test style)
  - [x] Load each fixture, `SysExDecoder::decodePatchSysEx` → 134-byte buffer → `PatchModel::loadFrom` → assert `data()` byte-identical; plus re-encode equals original message
  - [x] Name round-trip test (8-char, > 8-char truncation, < 8-char padding) + reference-name decode (`BNK4: 05/66/40`)
  - [x] Signed-field round-trip (7-bit ±63 at offset 86, 6-bit ±31 at offset 19, unsigned 0–127) with on-wire byte assertions; choice index store/clamp
- [x] **Self-review against Clean Code limits** before marking done (AC: #7)

## Dev Notes

### What PatchModel IS — and what it must NOT duplicate

`PatchModel` is the **in-memory representation of the 134-byte packed patch buffer** — the exact thing `SysExEncoder::encodePatchSysEx(patchNumber, packedData)` consumes and `SysExDecoder::decodePatchSysEx(sysEx, output)` produces. Its job is to read/write **individual parameter values by descriptor byte-offset** and to handle the name field.

It must **NOT** re-implement:
- **Nibble packing/unpacking** — owned by `SysExEncoder::unpackBytesToNibbles` / `SysExDecoder::packNibbles`.
- **Checksum** — owned by `SysExEncoder::calculateChecksum`.
- **SysEx framing** (`F0 10 06 01 …`) — owned by encoder/decoder.

> The packed buffer is the clean boundary: decoder gives you 134 bytes, you index them by descriptor offset, encoder turns 134 bytes back into a wire message. Keep `PatchModel` on the byte side of that line only. [Source: Source/Core/MIDI/SysEx/SysExEncoder.h, SysExDecoder.h]

### Single source of truth — offsets come from descriptors

Each `IntParameterDescriptor` / `ChoiceParameterDescriptor` already carries `sysExOffset` and `sysExId`. [Source: Source/Shared/Definitions/PluginDescriptors.h:54-75]
- `sysExOffset` = **byte index into the 134-byte patch buffer** (the "Byte" column of the Oberheim Single Patch Data Format table).
- `sysExId` = the Matrix-1000 **parameter number** for the `0x06` Remote Parameter Edit message (the "Parameter" column) — **not** the buffer offset. Used later in Epic 2, not in this story.

Patch-Edit descriptors live in the `PluginDescriptors::PatchEditSection::*Module` namespaces (Dco1, Dco2, VcfVca, FmTrack, RampPortamento, Envelope1-3, Lfo1-2). [Source: Source/Shared/Definitions/PluginDescriptors.h:134-205, PluginDescriptorsPatchEdit.cpp]

Constants `kPatchPackedDataSize = 134`, `kMasterPackedDataSize = 172` already exist — **reuse them**. [Source: Source/Core/MIDI/SysEx/SysExConstants.h:44-45]

### Byte layout reference (authoritative)

Full 134-byte table — "Byte" / "Parameter #" / "# Bits" / description — is in [Source: _bmad-output/reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md#Single-Patch-Data-Format (line ~192)]. Key facts the dev must respect:
- **Bytes 0–7**: patch name, 6 bits each (lower 6 bits of ASCII).
- Most other bytes hold one parameter each, valid range bounded by the descriptor `minValue`/`maxValue`.
- **Signed fields exist**: byte 19 (DCO 2 Detune, 6-bit signed), byte 86 (DCO 1 Freq by LFO 1, 7-bit signed), and others marked "(signed)". The spec note for `0x06` says values are *"sign extended from bit 6 into bit 7 except for parameter 121 (VCF Frequency)"* — confirm the **packed-buffer** representation against the reference samples (AC #4); do not guess two's-complement vs sign-magnitude.

### Test harness must be stood up here (decided: in scope for 1.1)

This is the **first story that requires a passing unit test**, but the repo has **no test runner target** yet:
- `Tests/Unit/` contains `SysExParserTests.cpp`, `SysExEncoderTests.cpp`, `MidiManagerTests.cpp` using **`juce::UnitTest`** (not Catch2 — project-context's "Catch2 or Google Test" line is superseded by the actual code). [Source: Tests/Unit/SysExParserTests.cpp]
- `Tests/CMakeLists.txt` is only a reserved comment; the root `CMakeLists.txt:205` explicitly warns **not** to link `Tests/` into the plugin targets.

**Decision (Guillaume, 2026-06-02):** the `juce_add_console_app` runner is built **inside Story 1.1** — it is the enabler for all of Epic 1. Wire it so the existing three test files run alongside the new `PatchModelTests`.

### File placement & boundaries

- New file path: `Source/Core/Models/PatchModel.{h,cpp}` — create the `Models/` folder. [Source: architecture.md:281 "Source/Core/Models/ — PatchModel, MasterModel, mappers"]
- `Core ↛ GUI` is **strict** — no JUCE GUI headers, no APVTS coupling in this story (APVTS↔model sync is Story 1.3). [Source: project-context.md#Architecture-Invariants]
- One primary class per `.h/.cpp`; no util junk drawers. [Source: architecture.md:287]

### Cross-story context (Epic 1)

- **1.2 MasterModel** will mirror this exact pattern for the 172-byte master buffer → design `PatchModel` so the offset-indexing helper is the obvious thing to generalize, but **do not** prematurely abstract a shared base in this story.
- **1.3 ApvtsPatchMapper** consumes `PatchModel` get/set to sync APVTS ↔ buffer — keep the value accessors clean and descriptor-driven so the mapper just iterates descriptors.
- **1.5 Patch Name sync** builds on `getName`/`setName` here (6-bit ASCII, max 8). Get the name codec right now. [Source: epics.md#Epic-1 lines 384-447]

### Style reminders (project-context / CONVENTIONS)

Allman braces, 4-space indent, `#pragma once`, includes ordered system → JUCE → project, English-only, `nullptr`, `enum class`, named `constexpr` (no magic numbers), `= default`/`= delete` explicit, `JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR` consistent with `SysExEncoder`/`SysExDecoder`.

### Project Structure Notes

- `Source/` is the include root → `#include "Core/Models/PatchModel.h"`, `#include "Core/MIDI/SysEx/SysExConstants.h"` (never `../`).
- Test **source** files live under `Tests/Unit/` (PascalCase, house convention) alongside the existing SysEx/MidiManager tests.
- Test **fixtures** go in new `Tests/Fixtures/Patches/` — **PascalCase**, the house convention for newly created folders (`Tests/Fixtures/Masters/` follows in Story 1.2).
- Add new sources to the root `CMakeLists.txt` source list (the project does not glob).

### References

- [Source: _bmad-output/planning-artifacts/epics.md#Story-1.1 (lines 384-395)]
- [Source: _bmad-output/reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md#Single-Patch-Data-Format]
- [Source: Source/Shared/Definitions/PluginDescriptors.h:54-205]
- [Source: Source/Core/MIDI/SysEx/SysExConstants.h:39-45]
- [Source: Source/Core/MIDI/SysEx/SysExEncoder.h, SysExDecoder.h]
- [Source: _bmad-output/planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md:281, 308-312]
- [Source: _bmad-output/project-context.md#Architecture-Invariants, #Single-sources-of-truth]
- Existing test style: [Source: Tests/Unit/SysExParserTests.cpp]

## Dev Agent Record

### Agent Model Used

claude-opus-4-8 (Claude Code, dev-story workflow)

### Debug Log References

- Build: `cmake -S . -B Builds/macOS/ARM -DMATRIX_BUILD_TESTS=ON` then `cmake --build Builds/macOS/ARM --target Matrix-Control_Tests` (JUCE_DIR=/Applications/JUCE) — compiled and linked clean.
- Run: `Builds/macOS/ARM/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests`.

### Completion Notes List

- **All PatchModel ACs satisfied.** PatchModel stores the 134-byte buffer verbatim, so the buffer-level round-trip (AC #5) is exact: all **four** reference patches (5, 66, 71, 808) decode → load → re-encode byte-identical to the original SysEx message. Name decode matches `BNK4: 05 / 66 / 40` and `BNK2: 71`.
- **Name encoding finding (grounded in reference samples, per AC #4/#5):** the reference `.syx` store the patch name as full display ASCII in bytes 0–7 (e.g. byte 0 = `0x42` 'B', > 0x3F), not as raw 6-bit codes. `decodeNameChar` masks the low 6 bits and remaps codes < 0x20 into the `0x40` letter block, so it reads both display-ASCII and raw-6-bit correctly; `setName` writes display ASCII to match the reference representation. The "6-bit ASCII" spec note describes the synth's character set, not the stored byte form.
- **Signed fields — corrected against authoritative Oberheim data.** The Matrix-1000 stores signed params as **8-bit two's complement** (the spec's "sign extended from bit 6 into bit 7"), confirmed by the Max-for-Live capture: reference Patch 71 byte 90 (VCF Frequency by Env 1, param 22) = `212` = `0xD4` = **−44**. `decodeField` masks to the field width (derived from `maxValue` via `signBitPosition`: ±63→7-bit, ±31→6-bit) then sign-extends; `encodeField` is a plain `uint8` cast of the clamped value, which reproduces the synth's canonical byte (−44→`0xD4`, −1→`0xFF`, −63→`0xC1`, −31→`0xE1`). `Tests/Unit/PatchModelTests.cpp` now asserts this directly against Patch 71 (decode `212`→−44 and re-encode −44→`212`), closing the earlier "asserted by construction only" gap.
- **Test harness stood up (Epic-1 enabler):** new `Matrix-Control_Tests` console target (option `MATRIX_BUILD_TESTS`, default OFF) with `Tests/TestMain.cpp` running `juce::UnitTestRunner`; compiles only the Core sources under test (no GUI, not linked into AU/VST3/Standalone). Fixtures path injected via `MATRIX_TEST_FIXTURES_DIR`.
- **`.gitignore` fix:** the stale lowercase `tests/` entry matched the canonical versioned `Tests/` directory on case-insensitive macOS, which would have blocked committing the fixtures (AC #6). Kept the legacy entry but added an anchored re-include `!/Tests/` so the root `Tests/` is versioned while nested lowercase `tests/` folders (tooling) stay ignored.
- ⚠️ **Pre-existing parser bug surfaced by the new runner — fixed by a separate task, NOT part of this story.** `SysExParserTests` → "Oberheim manufacturer ID validation" failed deterministically at first: `validateStructure` required `kMinSysExLength` (7) bytes, so the 6-byte wrong-manufacturer message was rejected with the misleading "missing F0 or F7" before the manufacturer check ran. This was flagged via a spawned task; that task (run separately) fixed it by adding `kMinSysExEnvelopeLength = 2` and relaxing `validateStructure` to the envelope check. Those edits to `Source/Core/MIDI/SysEx/SysExConstants.h` and `SysExParser.cpp` landed in the **same working tree** and show as modified — **they belong to the parser task, not Story 1.1**, and should be committed/reviewed separately. With them compiled in, the manufacturer test passes for the right reason ("Invalid Manufacturer ID or Device ID") and the full suite exits 0; PatchModel's 8 cases pass deterministically.
- Clean Code self-review: `PatchModel` class ~50 h + ~110 cpp lines; every method ≤ ~12 lines, ≤ 3 params, named bit-mask constants (no magic numbers), `Core ↛ GUI` respected (only `juce_core` + descriptor structs). Type stays copyable (rule of zero) for later snapshot/clipboard use.

### Review Findings

- [x] [Review][Patch] **setName must fold to uppercase before storing** — Matrix charset is uppercase-only (6-bit, no lowercase). `setName` currently silently mangles lowercase input (`"abc"` → `getName()` = `"!\"#"`). Fix: call `name.toUpperCase()` before the loop. [Source/Core/Models/PatchModel.cpp:60-67]
- [x] [Review][Patch] **`loadFrom(nullptr)` is UB despite `noexcept`** — added `jassert(packedData != nullptr)` before `std::memcpy`. [Source/Core/Models/PatchModel.cpp:21]
- [x] [Review][Patch] **`sysExOffset` not bounds-checked → OOB on bad/negative descriptors** — extracted `safeOffset(int)` private helper with `jassert`; used in `getValue`, `setValue`, `getChoiceIndex`, `setChoiceIndex`. [Source/Core/Models/PatchModel.cpp]
- [x] [Review][Patch] **Positional aggregate init in tests is brittle** — replaced with C++20 designated initializers (`.minValue`, `.maxValue`, `.sysExOffset`, `.choices`) in all inline descriptor constructions. [Tests/Unit/PatchModelTests.cpp]
- [x] [Review][Patch] **`decodeField`/`encodeField` not marked `noexcept`** — added `noexcept` to both definitions and declarations. [Source/Core/Models/PatchModel.h, PatchModel.cpp]
- [x] [Review][Patch] **`runNameCodec` test is uppercase-only** — added `"synthbas"→"SYNTHBAS"` and `"Hi There!"→"HI THERE"` sub-cases. [Tests/Unit/PatchModelTests.cpp]
- [x] [Review][Defer] **`signBitPosition` undefined for `maxValue ≤ 0` or non-`2^n−1` ranges** [Source/Core/Models/PatchModel.cpp:53-58] — deferred, latent; `jlimit` safety net covers it, no current descriptor triggers it
- [x] [Review][Defer] **`getChoiceIndex` silently clamps stale/corrupt buffer bytes** — no current descriptor with empty `choices`; defensive clamping acceptable [Source/Core/Models/PatchModel.cpp:40]

### Change Log

- 2026-06-02 — Implement Core::PatchModel (134-byte packed single-patch buffer), descriptor-driven value/name access, signed-field two's-complement codec.
- 2026-06-02 — Add `Matrix-Control_Tests` console test runner (`MATRIX_BUILD_TESTS` option) + `Tests/TestMain.cpp`; wire existing SysEx tests; add `Tests/Unit/PatchModelTests.cpp`.
- 2026-06-02 — Commit reference fixtures `Tests/Fixtures/Patches/{Patch 5,Patch 66,Patch 71,Patch 808}.syx`; fix `.gitignore` so `Tests/` is versioned on case-insensitive filesystems (`!/Tests/`).
- 2026-06-02 — Correct signed-field encoding to 8-bit two's complement after cross-checking the Oberheim Max-for-Live capture (Patch 71 byte 90 = −44); add Patch 71 fixture + reference signed-value test.

### File List

- `Source/Core/Models/PatchModel.h` (new)
- `Source/Core/Models/PatchModel.cpp` (new)
- `Tests/TestMain.cpp` (new)
- `Tests/Unit/PatchModelTests.cpp` (new)
- `Tests/Fixtures/Patches/Patch 5.syx` (new)
- `Tests/Fixtures/Patches/Patch 66.syx` (new)
- `Tests/Fixtures/Patches/Patch 71.syx` (new)
- `Tests/Fixtures/Patches/Patch 808.syx` (new)
- `CMakeLists.txt` (modified — added `Matrix-Control_Tests` target)
- `.gitignore` (modified — anchored `!/Tests/` re-include so the versioned `Tests/` survives the legacy `tests/` rule)
