---
story_key: 1-2-mastermodel-packed-buffer
epic: 1
story: 2
status: done
baseline_commit: d66508a
---

# Story 1.2: MasterModel Packed Buffer

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want a 172-byte packed master buffer in Core,
So that master (Global Parameters) SysEx serialization is accurate (FR-49).

## Acceptance Criteria

1. **Given** the master descriptor `sysExOffset` values in `PluginDescriptors::MasterEditSection` **When** `MasterModel` reads/writes its 172-byte buffer **Then** every master parameter is stored at the byte index declared by its descriptor — **no offset table is duplicated** anywhere outside `PluginDescriptors`.
2. **And** the buffer size is taken from `SysExConstants::kMasterPackedDataSize` (172) — not a new literal.
3. **And** the four signed master fields round-trip correctly through the **same two's-complement codec proven in Story 1.1** (8-bit two's complement, sign-bit width derived from `maxValue`): Master Tune (±31, byte 8), Master Transpose (−24..24, byte 34), Vibrato Speed Mod Amount (±63, byte 3), Vibrato Amp Mod Amount (±63, byte 7). On-wire byte encoding is asserted (e.g. −1 → `0xFF`, −31 → `0xE1`).
4. **And** the field codec (`safeOffset` / `signBitPosition` / `decodeField` / `encodeField`) is **extracted into a shared Core helper** consumed by **both** `PatchModel` and `MasterModel` — `PatchModel` is refactored onto it (zero duplication, hard rule). All existing `PatchModelTests` cases still pass unchanged after the refactor.
5. **And** a JUCE `UnitTest` round-trips the reference master dump: `Master 1.syx` decode → `MasterModel::loadFrom` → re-encode **byte-identical** to the original 351-byte message (`encodeMasterSysEx(version, data())` with `version` = original byte 4 = `0x03`). This exercises the checksum path end-to-end (re-encoded checksum must match), satisfying the epic's "verify checksum-relevant fields".
6. **And** the test fixture lives in the **versioned** path `Tests/Fixtures/Masters/` (committed); the original currently sits only in gitignored `_local/` (see Dev Notes). `MasterModel` is registered in the `Matrix-Control_Tests` CMake target.
7. **And** `MasterModel` lives in `Source/Core/Models/MasterModel.{h,cpp}`, has **NO name field** (master has no patch name), **zero GUI dependencies**, and respects Clean Code limits (class ≤ 200 lines, methods ≤ 15 lines, ≤ 3 params, no magic numbers).

## Tasks / Subtasks

- [x] **Source the test fixture** (AC: #5, #6) — *do this first, it gates the test*
  - [x] Create `Tests/Fixtures/Masters/` (PascalCase — house convention for new folders, mirrors `Tests/Fixtures/Patches/`)
  - [x] Copy `Master 1.syx` from `_local/TestLab/SysEx Data/Original/Master/` into `Tests/Fixtures/Masters/`
  - [x] Verify it is a full 351-byte master message (`F0 10 06 03 03 …`, opcode `0x03`, version `0x03`) — skip the `(Truncated)` / `(Sent)` variants
  - [x] Confirm `.gitignore` already versions `Tests/` (the `!/Tests/` re-include landed in Story 1.1 — no change expected)
- [x] **Extract the shared field codec** (AC: #3, #4) — *DRY refactor, the second consumer makes the duplication real*
  - [x] Create `Source/Core/Models/PackedFieldCodec.h` (`namespace Core::PackedFieldCodec`) with free functions: `size_t safeOffset(int sysExOffset, size_t bufferSize)`, `int decodeField(juce::uint8 raw, int minValue, int maxValue)`, `juce::uint8 encodeField(int value, int minValue, int maxValue)` — lifted from `PatchModel.cpp` (`signBitPosition` kept as a file-local helper in the `.cpp`; logic now parameterized by `bufferSize`)
  - [x] Refactor `PatchModel` to delegate to `PackedFieldCodec` for these helpers; keep the **name codec** (`decodeNameChar`, charset constants) in `PatchModel` — it is patch-only
  - [x] Add `PackedFieldCodec.cpp` to the `Matrix-Control_Tests` target source list; rebuild and run — **all existing `PatchModelTests` pass unchanged** (no behaviour change, pure extraction)
- [x] **Implement `MasterModel`** (AC: #1, #2, #3, #7)
  - [x] `Source/Core/Models/MasterModel.h` — `#pragma once`, 172-byte `std::array<juce::uint8, SysExConstants::kMasterPackedDataSize>` buffer (`kBufferSize` constant); rule-of-zero copyable, matching `PatchModel` so the two models are symmetric for later snapshot/clipboard use
  - [x] Raw buffer access: `const juce::uint8* data() const`, `juce::uint8* data()`, `loadFrom(const juce::uint8*)` + `kBufferSize` → for `SysExEncoder::encodeMasterSysEx` / `SysExDecoder::decodeMasterSysEx` interop
  - [x] Descriptor-driven read/write delegating to `PackedFieldCodec`: `getValue/setValue(IntParameterDescriptor)` + `getChoiceIndex/setChoiceIndex(ChoiceParameterDescriptor)` using `sysExOffset` — **no name accessors**
  - [x] Register `MasterModel.cpp` + `PackedFieldCodec.cpp` in `CMakeLists.txt` test target source list
- [x] **Write `Tests/Unit/MasterModelTests.cpp`** (AC: #3, #5)
  - [x] `class MasterModelTests : public juce::UnitTest` (matches `PatchModelTests` style; registers a static instance)
  - [x] Reference round-trip: `SysExDecoder::decodeMasterSysEx(Master 1.syx)` → 172-byte buffer → `MasterModel::loadFrom` → assert `data()` byte-identical to decoded; re-encode with `encodeMasterSysEx(original[4], model.data())` and assert it equals the original `MemoryBlock`
  - [x] Signed-field codec: synthetic `IntParameterDescriptor`s for the signed master fields — assert value round-trip **and** on-wire byte (−1→`0xFF`, −31→`0xE1`, −63→`0xC1`, +max) mirroring `PatchModelTests::runSignedFieldCodec`
  - [x] Choice-field store/clamp on a representative master choice descriptor (On/Off)
- [x] **Self-review against Clean Code limits** before marking done (AC: #4, #7) — zero duplication between the two models (both delegate to `PackedFieldCodec`), both classes ≤ 200 lines, codec extraction left `PatchModel` green

### Review Findings

- [x] [Review][Patch] Missing signed-field tests: Master Transpose (±24, byte 34) and Vibrato Amp Mod Amount (±63, byte 7) [Tests/Unit/MasterModelTests.cpp:runSignedFieldCodec]
- [x] [Review][Defer] `safeOffset` has no release-mode bounds guard (jassert+ignoreUnused only) [Source/Core/Models/PackedFieldCodec.cpp:22] — deferred, pre-existing pattern from PatchModel; descriptor offsets are static constants
- [x] [Review][Defer] `choices.size() - 1` size_t underflow when choices is empty [Source/Core/Models/MasterModel.cpp:30] — deferred, pre-existing in PatchModel (Story 1.1), all real descriptors have ≥ 1 choice

## Dev Notes

### What MasterModel IS — and what it must NOT duplicate

`MasterModel` is the **in-memory representation of the 172-byte packed master ("Global Parameters") buffer** — exactly what `SysExEncoder::encodeMasterSysEx(version, packedData)` consumes and `SysExDecoder::decodeMasterSysEx(sysEx, output)` produces. It reads/writes **individual master parameters by descriptor byte-offset**. It is the master twin of `PatchModel`, minus the name field.

It must **NOT** re-implement nibble packing/unpacking, checksum, or SysEx framing — those stay in `SysExEncoder` / `SysExDecoder`, identical to the patch path. [Source: Source/Core/MIDI/SysEx/SysExEncoder.h:16, SysExDecoder.h:27]

### Reuse Story 1.1, do not reinvent — and kill the duplication now

`PatchModel` already solved the descriptor-offset indexing and the **signed two's-complement codec** (validated against real hardware: Patch 71 byte 90 = `212` = −44). [Source: Source/Core/Models/PatchModel.cpp:72-100; 1-1-patchmodel-packed-buffer.md Completion Notes]

Story 1.1 deliberately left those helpers as private statics "as the obvious thing to generalize" for this story. With `MasterModel` as the second consumer, copy-pasting them would violate the **zero-duplication hard rule**. **Extract them into `Source/Core/Models/PackedFieldCodec.h` and refactor `PatchModel` onto it** (AC #4). The only `PatchModel`-specific code that stays put is the **name codec** (6-bit charset, bytes 0-7) — master has no name.

> The shared codec is the clean boundary: both models hold their own packed buffer and index it by descriptor offset; the codec turns a raw byte ↔ a signed/unsigned parameter value. Keep both models thin. [Source: CONVENTIONS.md §3.1 "Code duplication: zero tolerance"; project-context.md#Clean-Code-Limits]

Prefer a **free-function namespace** (`Core::PackedFieldCodec`) over a shared base class — project rule favors delegation/composition over inheritance. [Source: CONVENTIONS.md §6.11]

### Master byte layout (authoritative)

172-byte "Global Parameters Data Format" table — "Byte" / "# Bits" / description — in [Source: _bmad-output/reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md#Global-Parameters-Data-Format (line ~410)]. Descriptor offsets already match it. Facts the dev must respect:

- **22 master parameters total** (11 `IntParameterDescriptor` + 11 `ChoiceParameterDescriptor`) across MidiModule / VibratoModule / MiscModule; consume the combined `MasterEditSection::kIntParameters` / `kChoiceParameters` vectors. [Source: Source/Shared/Definitions/PluginDescriptorsMasterEdit.cpp:30-378]
- **Signed fields (4):** byte 3 Vibrato Speed Mod Amount (±63), byte 7 Vibrato Amp Mod Amount (±63), byte 8 Master Tune (±31), byte 34 Master Transpose (−24..24). `signBitPosition` derives the field width from `maxValue` (±63→7-bit, ±31→6-bit, ±24→6-bit). Same encoding as patch (8-bit two's complement). [Source: oberheim ref lines 421-441; PluginDescriptorsMasterEdit.cpp:174-203, 264-294]
- **Bytes 36-161 Group Enables** (one bit per patch, LSB first, 126 bytes) are **not** descriptor-mapped. The model stores them verbatim — round-trip preserves them. In `Master 1.syx` this whole region decodes to `0xFF` (all groups enabled), a useful visual anchor in the fixture.
- **Trust the descriptor `minValue`/`maxValue`, not the doc "# Bits" column** for master. The doc's bit counts are unreliable for master (e.g. it lists byte 164 Bend Range as "1 bit" though the range is 0-24). The model only needs `[min,max]` clamping for unsigned fields and `maxValue` for signed width — bit-width from the doc is irrelevant.

### Known limitation: the composite MIDI Channel field (document, do NOT "fix")

The `kChannel` choice descriptor carries `.sysExOffset = 11` with the comment *"Combination of offsets 11, 12, 35"*. Its 27-entry list (Omni, Ch 1-16, Mono G1-9) actually spans **three** bytes on the synth: byte 11 (basic channel, 4-bit), byte 12 (Omni enable), byte 35 (Mono enable). [Source: PluginDescriptorsMasterEdit.cpp:107; oberheim ref lines 429-442]

For Story 1.2 the single-offset `getChoiceIndex/setChoiceIndex` touches **byte 11 only** — it is **not** a faithful semantic mapping of the channel. That is acceptable and **intended** here: the 172-byte buffer round-trip preserves bytes 11/12/35 verbatim regardless, so AC #5 holds. The full Omni/Mono composition belongs to **Story 1.4 (ApvtsMasterMapper)**. Do not special-case the channel inside `MasterModel`; just leave a short code comment pointing to Story 1.4.

### Reference-sample caveat (be honest in the tests)

Only **one** full master dump exists (`Master 1.syx`) — unlike the four patch fixtures. It is essentially a default master: Master Tune = 0, Master Transpose = 0, all group enables on. **The signed master fields are all 0 in the reference**, so — unlike Patch 71 for the patch path — the reference cannot empirically prove negative signed encoding. Therefore:

- AC #5 round-trip (byte-identity + re-encode equality incl. checksum) is the reference-grounded guarantee.
- AC #3 signed encoding is validated with **synthetic** descriptors and explicit on-wire byte assertions, leaning on the codec already proven against real hardware in Story 1.1 (do not re-derive the encoding — reuse `PackedFieldCodec`).

### File placement & boundaries

- New files: `Source/Core/Models/MasterModel.{h,cpp}` and `Source/Core/Models/PackedFieldCodec.{h,cpp}`. [Source: architecture.md:281 "Source/Core/Models/ — PatchModel, MasterModel, mappers"]
- `Core ↛ GUI` is **strict** — only `juce_core` + descriptor structs; no APVTS coupling (APVTS↔model sync is Story 1.4). [Source: project-context.md#Architecture-Invariants]
- `Source/` is the include root → `#include "Core/Models/MasterModel.h"`, `#include "Core/MIDI/SysEx/SysExConstants.h"` (never `../`).
- The project does **not** glob — add every new source to the `Matrix-Control_Tests` `target_sources` list. [Source: CMakeLists.txt:219-232]

### Test harness (already exists — just extend it)

`Matrix-Control_Tests` console target was stood up in Story 1.1 (`-DMATRIX_BUILD_TESTS=ON`, `juce::UnitTestRunner`, fixtures dir via `MATRIX_TEST_FIXTURES_DIR`). Add `MasterModelTests.cpp`, `MasterModel.cpp`, `PackedFieldCodec.cpp` to its source list. Fixtures resolve as `juce::File(MATRIX_TEST_FIXTURES_DIR).getChildFile("Masters")`. [Source: CMakeLists.txt:216-251; Tests/Unit/PatchModelTests.cpp:30-33]

### Cross-story context (Epic 1)

- **1.4 ApvtsMasterMapper** consumes `MasterModel` get/set to sync APVTS ↔ buffer for all 22 master params, and owns the composite-channel logic. Keep the value accessors clean and descriptor-driven so the mapper just iterates `MasterEditSection::kIntParameters` / `kChoiceParameters`. [Source: epics.md#Story-1.4 lines 423-434]
- The `PackedFieldCodec` extracted here will also serve any future packed-buffer model.

### Style reminders (project-context / CONVENTIONS)

Allman braces, 4-space indent, `#pragma once`, includes ordered system → JUCE → project, English-only, `nullptr`, `enum class`, named `constexpr` (no magic numbers), `= default`/`= delete` explicit, `noexcept` on the codec helpers (as in `PatchModel`).

### Project Structure Notes

- New folder `Tests/Fixtures/Masters/` — PascalCase, house convention, mirrors `Tests/Fixtures/Patches/`.
- Test **source** files under `Tests/Unit/` alongside the existing tests.
- `PackedFieldCodec` refactor touches the already-`done` Story 1.1 files (`PatchModel.{h,cpp}`) — this is the legitimate DRY generalization Story 1.1 anticipated; list them in the File List as modified and keep `PatchModelTests` green.

### References

- [Source: _bmad-output/planning-artifacts/epics.md#Story-1.2 (lines 397-408)]
- [Source: _bmad-output/reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md#Global-Parameters-Data-Format (lines 410-451), #03H-Master-Parameter-Data (lines 84-92)]
- [Source: Source/Shared/Definitions/PluginDescriptorsMasterEdit.cpp (full)]
- [Source: Source/Core/MIDI/SysEx/SysExConstants.h:40, 45]
- [Source: Source/Core/MIDI/SysEx/SysExEncoder.h:16; SysExDecoder.h:27]
- [Source: Source/Core/Models/PatchModel.{h,cpp}] — pattern to mirror and codec to extract
- [Source: Tests/Unit/PatchModelTests.cpp] — test style to mirror
- [Source: CMakeLists.txt:216-251] — test target wiring
- [Source: 1-1-patchmodel-packed-buffer.md] — prior story learnings (signed codec, name caveat, harness, `.gitignore` fix)
- [Source: _bmad-output/project-context.md#Architecture-Invariants, #Clean-Code-Limits]

## Dev Agent Record

### Agent Model Used

claude-opus-4-8 (Claude Code, dev-story workflow)

### Debug Log References

- Build: `JUCE_DIR=/Applications/JUCE cmake -S . -B Builds/macOS/ARM -DMATRIX_BUILD_TESTS=ON` then `cmake --build Builds/macOS/ARM --target Matrix-Control_Tests` — compiled and linked clean, no warnings.
- Run: `Builds/macOS/ARM/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests` — exit 0, zero failures across all suites (PatchModel, MasterModel, SysExEncoder, SysExParser, MidiManager).

### Completion Notes List

- **All MasterModel ACs satisfied.** `MasterModel` stores the 172-byte master buffer verbatim, so the reference round-trip (AC #5) is exact: `Master 1.syx` decode → `loadFrom` → re-encode (`encodeMasterSysEx(version=0x03, data())`) is byte-identical to the original 351-byte message, which exercises the checksum path end-to-end.
- **DRY codec extraction (AC #4).** The proven Story-1.1 field codec (`safeOffset`, `signBitPosition`, `decodeField`, `encodeField`) is now `Core::PackedFieldCodec` free functions in `Source/Core/Models/PackedFieldCodec.{h,cpp}`. `safeOffset` gained a `bufferSize` parameter (134 vs 172). `PatchModel` was refactored to delegate to it; its **patch-only name codec** (`decodeNameChar`, charset constants) stayed in place. All 5 `PatchModelTests` groups pass unchanged — pure extraction, no behaviour change. Zero duplication between the two models.
- **Signed master fields validated by synthetic descriptors (AC #3).** Only one master dump exists (`Master 1.syx`) and it carries default/zero signed values (Master Tune = 0, Master Transpose = 0, all group enables `0xFF`), so — unlike Patch 71 for the patch path — the reference cannot prove negative signed encoding empirically. `MasterModelTests::runSignedFieldCodec` therefore asserts the on-wire bytes directly (−31 → `0xE1` for the 6-bit Master Tune, −1 → `0xFF`, −63 → `0xC1` for the 7-bit Vibrato Speed Mod Amount), reusing the codec already validated against real hardware in Story 1.1.
- **Composite MIDI Channel left to Story 1.4 (documented, not special-cased).** The `kChannel` choice descriptor's single `sysExOffset = 11` semantically spans bytes 11/12/35 (channel / Omni / Mono). `MasterModel::getChoiceIndex/setChoiceIndex` touch byte 11 only; a header comment points to Story 1.4 (`ApvtsMasterMapper`) for the full Omni/Mono composition. The 172-byte buffer round-trip preserves all three bytes regardless, so AC #5 holds.
- **No name field** — master has no patch name, so `MasterModel` is simpler than `PatchModel` (the only `Models/` divergence). `MasterModel` is rule-of-zero copyable, matching `PatchModel`.
- **Clean Code self-review:** `MasterModel` ~45 h + ~45 cpp lines; `PackedFieldCodec` ~35 h + ~50 cpp; every method ≤ ~10 lines, ≤ 3 params, named bit-mask logic (no magic numbers), `Core ↛ GUI` respected (only `juce_core` + descriptor structs).

### File List

- `Source/Core/Models/PackedFieldCodec.h` (new — shared field codec)
- `Source/Core/Models/PackedFieldCodec.cpp` (new)
- `Source/Core/Models/MasterModel.h` (new)
- `Source/Core/Models/MasterModel.cpp` (new)
- `Source/Core/Models/PatchModel.h` (modified — codec helpers moved to PackedFieldCodec)
- `Source/Core/Models/PatchModel.cpp` (modified — delegates to PackedFieldCodec; name codec retained)
- `Tests/Unit/MasterModelTests.cpp` (new)
- `Tests/Fixtures/Masters/Master 1.syx` (new — reference master dump)
- `CMakeLists.txt` (modified — added MasterModel, PackedFieldCodec, MasterModelTests to test target)

### Change Log

- 2026-06-03 — Implement Core::MasterModel (172-byte packed master/Global Parameters buffer), descriptor-driven value/choice access, no name field.
- 2026-06-03 — Extract shared `Core::PackedFieldCodec` (offset/sign-extend/encode helpers) from PatchModel; refactor PatchModel to delegate (DRY, zero duplication); PatchModel tests stay green.
- 2026-06-03 — Add `Tests/Unit/MasterModelTests.cpp` (reference round-trip incl. checksum, signed-field on-wire bytes, choice clamp) + commit `Tests/Fixtures/Masters/Master 1.syx`.
