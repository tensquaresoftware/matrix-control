---
organization: Ten Square Software
project: Matrix-Control
title: Story 2.2 — SysExDelayProfile and Inter-Message Delay
author: BMad Agent
status: done
baseline_commit: 8b0cfb1021750fbce88af158318b57a59443822b
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - implementation-artifacts/2-1-midioutboundqueue-core.md
  - reference-docs/oberheim/index.md
created: 2026-06-04
---

# Story 2.2: SysExDelayProfile and Inter-Message Delay

Status: done

<!-- Note: Validation is optional. Run validate-create-story for quality check before dev-story. -->

## Story

As a sound designer,
I want inter-SysEx delay selected from device firmware class,
so that stock and optimised EPROMs are not overloaded (FR-7, NFR-2, PRD §9 #2 spike).

## Acceptance Criteria

1. `SysExDelayProfile` lives in `Source/Core/MIDI/Queue/SysExDelayProfile.{h,cpp}`, namespace `Core`. Zero GUI dependencies. Resolves **EPROM class** (`kStock` / `kOptimised`) and **device family** (`kMatrix1000` / `kMatrix6Or6R`) from Device Inquiry data (`DeviceIdInfo` or equivalent version string + member bytes). String matching rules for Tauntek / Gligli / Nordcore are documented in code comments and tested.
2. `getDelayMs() const noexcept` returns the active minimum inter-SysEx gap in milliseconds:
   - **Stock M-1000:** 10 ms
   - **Stock M-6 / 6R:** 20 ms
   - **Optimised (any family):** shorter than stock — use named constants `kOptimisedDelayMsMatrix1000` and `kOptimisedDelayMsMatrix6` (placeholder values allowed; document `// SM-1 hardware gate may tune` in source)
3. `SysExInterMessageDelay` lives in `Source/Core/MIDI/Queue/SysExInterMessageDelay.{h,cpp}`, namespace `Core`. Holds a `SysExDelayProfile`, tracks timestamp of last SysEx send, and exposes **testable** API (no hidden wall-clock in untestable private methods):
   - `int millisUntilNextAllowed(juce::int64 nowMs) const noexcept` — returns 0 when next SysEx may send, else remaining wait
   - `void recordSysExSent(juce::int64 nowMs) noexcept` — call after each outbound SysEx EOX is transmitted
   - `void setProfile(SysExDelayProfile profile) noexcept` — hot-swap when Device Inquiry completes
4. Delay is enforced **after EOX** (gap between consecutive SysEx sends), not between realtime MIDI and SysEx. Realtime messages from `MidiOutboundQueue` are never delayed by this gate (consumer applies gate only after sending a dequeued `kSysEx` message — wiring in Story 2.9; this story delivers the gate + profile only).
5. `MidiManager::sendSysExWithDelay` is refactored to use `SysExInterMessageDelay` + `SysExDelayProfile` instead of hard-coded `SysExConstants::kMinSysExDelayMs` sleep. `performDeviceInquiry()` updates the profile from decoded `DeviceIdInfo` on success. Default before inquiry: stock M-1000 (10 ms).
6. Unit tests in `Tests/Unit/SysExDelayProfileTests.cpp` and `Tests/Unit/SysExInterMessageDelayTests.cpp` (JUCE `UnitTest` subclasses) verify:
   - (a) stock M-1000 default → 10 ms; stock M-6/6R → 20 ms
   - (b) version strings containing `TAUNTEK`, `GLIGLI`, `NORDCORE` (case-insensitive) → optimised class with delay &lt; stock for same device family
   - (c) unknown / empty version → stock class
   - (d) gate: `recordSysExSent(0)` then `millisUntilNextAllowed(5)` returns 5 for 10 ms profile; returns 0 at T ≥ delay
   - (e) profile swap mid-session: `setProfile` changes subsequent `millisUntilNextAllowed` without resetting last-send timestamp (document behaviour in test name)
7. New `.cpp` files added to `PLUGIN_SOURCES` and `Matrix-Control_Tests` `target_sources`; test files added to test target. All existing 35 tests pass; VST3 builds cleanly.
8. `MidiOutboundQueue` is **not** modified (priority dequeue unchanged). `MidiManager::run()` queue consumer loop remains stub — full dequeue+send+delay wiring is Story 2.9.

## Tasks / Subtasks

- [x] **Define delay constants and enums** (AC: #1, #2)
  - [x] Add `Core::MatrixDeviceFamily` and `Core::EpromClass` enums in `SysExDelayProfile.h`
  - [x] Add stock/optimised delay constants (prefer `SysExConstants` namespace extension or `SysExDelayProfile` nested `constexpr` with `k` prefix per CONVENTIONS)
  - [x] Document string-match tokens and SM-1 tuning note in header comment block

- [x] **Implement `SysExDelayProfile`** (AC: #1, #2)
  - [x] `static SysExDelayProfile fromDeviceInquiry(const DeviceIdInfo& info)` — map member bytes → device family; version string → EPROM class
  - [x] `static SysExDelayProfile stockDefault()` — M-1000 stock baseline for pre-inquiry default
  - [x] `getDelayMs()`, `getEpromClass()`, `getDeviceFamily()` accessors
  - [x] Case-insensitive substring match helper for optimised firmware detection

- [x] **Implement `SysExInterMessageDelay`** (AC: #3, #4)
  - [x] Track `lastSysExSentMs_` (`juce::int64`, `-1` = never sent)
  - [x] `millisUntilNextAllowed(nowMs)` computes `max(0, lastSent + profile.getDelayMs() - nowMs)`
  - [x] `recordSysExSent(nowMs)` updates timestamp
  - [x] Optional convenience `void waitUntilReady()` for MIDI thread — may call `std::this_thread::sleep_for` using injectable `nowMs` from `juce::Time::getMillisecondCounterHiRes()`; keep logic thin (≤ 15 lines)

- [x] **Wire into `MidiManager`** (AC: #5)
  - [x] Add `Core::SysExInterMessageDelay sysExDelay_` and `Core::SysExDelayProfile activeDelayProfile_` members (or single gate owning profile)
  - [x] Constructor: initialise stock M-1000 default profile
  - [x] `performDeviceInquiry()`: on valid `DeviceIdInfo`, `sysExDelay_.setProfile(SysExDelayProfile::fromDeviceInquiry(deviceInfo))`
  - [x] `sendSysExWithDelay`: before send, `waitUntilReady()` if needed; after send, `recordSysExSent(now)`; remove direct `SysExConstants::kMinSysExDelayMs` sleep
  - [x] Keep `SysExConstants::kMinSysExDelayMs` defined (legacy alias to stock M-1000 or deprecate with comment — do not delete without updating all references)

- [x] **Write unit tests** (AC: #6)
  - [x] `SysExDelayProfileTests`: stock/optimised/device-family matrix + string matching
  - [x] `SysExInterMessageDelayTests`: timing gate with synthetic `nowMs` (no real sleeps in tests)

- [x] **Update `CMakeLists.txt`** (AC: #7)
  - [x] Add `SysExDelayProfile.cpp`, `SysExInterMessageDelay.cpp` to plugin + test targets
  - [x] Add both test `.cpp` files to test target
  - [x] Rebuild; confirm 35 + new tests pass

### Review Findings

- [x] [Review][Patch] Reset delay profile to stock on inquiry failure / disconnect [`MidiManager.cpp:208-242`]
- [x] [Review][Patch] Unit test — unknown member bytes fall back to M-1000 stock delay [`SysExDelayProfileTests.cpp`]
- [x] [Review][Patch] Unit tests — assert optimised delay constants (5/10 ms) and `"TAUN 2.0"` stays stock [`SysExDelayProfileTests.cpp`]
- [x] [Review][Defer] `performDeviceInquiry` / `requestSysExData` bypass inter-SysEx gate — Story 2.9 consumer scope [`MidiManager.cpp:204,252`] — deferred, pre-existing RPC paths unchanged by AC #5
- [x] [Review][Defer] `extractDeviceVersion` caps at 4 chars — TAUNTEK/GLIGLI/NORDCORE unlikely on real hardware until decoder extended [`SysExDecoder.cpp:181-191`] — deferred, SM-1 / Epic 8

## Dev Notes

### Architecture Mandate — AD-3 + NFR-2

From AD-3, inter-SysEx delay comes from `SysExDelayProfile` (stock vs optimised EPROM — Device Inquiry string match). The unified queue consumer (`MidiManager::run()`, Story 2.9) will:

1. Dequeue realtime → send immediately (no delay gate)
2. Dequeue SysEx → `waitUntilReady()` → send → `recordSysExSent()`

Story 2.2 builds the profile + gate and migrates the **existing** synchronous `sendSysExWithDelay` path so hardware RPC (`requestCurrentPatch`, etc.) already respects EPROM-adaptive timing before the queue consumer exists.

### Delay Profile Table (D-078)

| Profile | Detection | M-1000 delay | M-6 / 6R delay |
|---|---|---|---|
| **Stock** | Default; factory Oberheim version string | **10 ms** | **20 ms** |
| **Optimised** | Version contains `TAUNTEK`, `GLIGLI`, or `NORDCORE` (case-insensitive substring) | **&lt; 10 ms** (constant TBD, suggest **5 ms** placeholder) | **&lt; 20 ms** (constant TBD, suggest **10 ms** placeholder) |

Source: [addendum.md § SysEx delay profiles], [architecture.md:56], [oberheim-matrix-6-6r-midi-sysex-implementation.md:146]

**Important:** Optimised numeric values are **not** locked in PRD — placeholders must be named constants with SM-1 comment. Do not hard-code magic numbers inline in logic.

### Device Family from Member Bytes

`DeviceIdInfo` already exposes `memberLow`, `memberHigh`, `version` ([SysExDecoder.h:9-18]).

| Device | Member bytes (Oberheim spec) | Story 2.2 mapping |
|---|---|---|
| Matrix-1000 | `memb-lo=0x02`, `memb-hi=0x00` | `kMatrix1000` |
| Matrix-6 / 6R | TBD on hardware (Epic 8) | `kMatrix6Or6R` when member pattern known; else fall back to `kMatrix1000` delay table with comment |

**Known bug D-080:** `SysExConstants::DeviceInquiry::kExpectedMemberLow/High` may be swapped vs Oberheim spec. Do **not** fix member-byte validation in this story unless required for profile tests — Epic 8 Story 8-1 owns the fix. Profile code should use decoded `DeviceIdInfo` values as-is; add a unit test with explicit member bytes independent of `kExpected*` constants.

### Optimised EPROM String Matching

Implement a single private helper:

```cpp
bool matchesOptimisedFirmware(const juce::String& version)
{
    const auto upper = version.toUpperCase();
    return upper.contains("TAUNTEK")
        || upper.contains("GLIGLI")
        || upper.contains("NORDCORE");
}
```

Document in header: matching rules are **best-effort** until SM-1 hardware confirms Tauntek/Gligli/Nordcore reply strings. Add test cases with representative version strings (e.g. `"1.11"`, `"TAUN 2.0"`, `"GLIGLI"`, `"NORDCORE 1.0"`).

M-4L user feedback confirms Tauntek dramatically improves SysEx responsiveness — optimised profile must be **strictly less** than stock delay for the same device family.

### Class Skeleton — `SysExDelayProfile`

```cpp
// SysExDelayProfile.h
#pragma once

#include <juce_core/juce_core.h>
#include "Core/MIDI/SysEx/SysExDecoder.h"  // DeviceIdInfo — or forward-declare + include in .cpp only

namespace Core
{
    enum class MatrixDeviceFamily { kMatrix1000, kMatrix6Or6R };
    enum class EpromClass { kStock, kOptimised };

    class SysExDelayProfile
    {
    public:
        static SysExDelayProfile fromDeviceInquiry(const DeviceIdInfo& info);
        static SysExDelayProfile stockDefault() noexcept;

        int getDelayMs() const noexcept;
        EpromClass getEpromClass() const noexcept;
        MatrixDeviceFamily getDeviceFamily() const noexcept;

    private:
        SysExDelayProfile(EpromClass epromClass, MatrixDeviceFamily deviceFamily) noexcept;

        EpromClass epromClass_;
        MatrixDeviceFamily deviceFamily_;
    };
}
```

Keep class ≤ 200 lines; methods ≤ 15 lines.

### Class Skeleton — `SysExInterMessageDelay`

```cpp
// SysExInterMessageDelay.h
#pragma once

#include <juce_core/juce_core.h>
#include "Core/MIDI/Queue/SysExDelayProfile.h"

namespace Core
{
    class SysExInterMessageDelay
    {
    public:
        SysExInterMessageDelay() = default;
        explicit SysExInterMessageDelay(SysExDelayProfile profile) noexcept;

        void setProfile(SysExDelayProfile profile) noexcept;
        int getRequiredDelayMs() const noexcept;

        int millisUntilNextAllowed(juce::int64 nowMs) const noexcept;
        void recordSysExSent(juce::int64 nowMs) noexcept;

        void waitUntilReady();  // MIDI thread only — uses HiRes clock + sleep

    private:
        SysExDelayProfile profile_ { SysExDelayProfile::stockDefault() };
        juce::int64 lastSysExSentMs_ { -1 };
    };
}
```

**Gate semantics:** First SysEx after construction or reset → `millisUntilNextAllowed` returns 0 (no prior send). Delay applies between **consecutive** SysEx only.

### `MidiManager` Integration — Current State

Today ([MidiManager.cpp:177-182]):

```cpp
void MidiManager::sendSysExWithDelay(...)
{
    midiSender->sendSysEx(sysExMessage);
    ...
    std::this_thread::sleep_for(std::chrono::milliseconds(SysExConstants::kMinSysExDelayMs));
}
```

Replace post-send blind sleep with gate-aware wait **before** send + record **after** send:

```cpp
void MidiManager::sendSysExWithDelay(const juce::MemoryBlock& sysExMessage, const juce::String& description)
{
    sysExDelay_.waitUntilReady();
    midiSender->sendSysEx(sysExMessage);
    MidiLogger::getInstance().logSysExSent(sysExMessage, description);
    sysExDelay_.recordSysExSent(juce::Time::getMillisecondCounterHiRes());
}
```

`performDeviceInquiry()` ([MidiManager.cpp:198-221]): after `deviceInfo.isValid`, call `sysExDelay_.setProfile(Core::SysExDelayProfile::fromDeviceInquiry(deviceInfo))`.

`MidiManager::run()` stub ([MidiManager.cpp:296-302]) still uses `wait(kMinSysExDelayMs)` — **leave unchanged** for Story 2.9.

### Threading Contract

| Component | Thread | Notes |
|---|---|---|
| `SysExDelayProfile` | any | Immutable value object after construction |
| `SysExInterMessageDelay::millisUntilNextAllowed` / `recordSysExSent` | MIDI thread (consumer) | No mutex needed if single consumer — document assumption |
| `setProfile` | message thread (inquiry callback) | Called rarely on port connect; acceptable race with consumer if inquiry completes before queue traffic — use same thread as inquiry handler or document ordering |
| `waitUntilReady` | MIDI thread only | Blocks thread — never audio thread |

If `setProfile` from message thread while MIDI thread reads profile: profile is small POD-like — assign by value (no pointer swap needed). Last-send timestamp unchanged on profile swap (AC #6e).

### What NOT to Do

- Do **not** modify `MidiOutboundQueue` dequeue ordering — Story 2.1 scope frozen
- Do **not** implement `MidiManager::run()` queue consumer loop — Story 2.9
- Do **not** add `InstrumentMidiForwarder` / `EditorPath` — Story 2.3
- Do **not** implement DAW automation burst throttling / coalescing — PRD §9 open question; future story
- Do **not** build `DeviceTypeRegistry` — Epic 8; use minimal member-byte heuristic here
- Do **not** call `MidiOutput` from audio thread
- Do **not** add GUI for delay profile display (footer shows `deviceVersion` already)

### Deferred Findings from Story 2.1

From [deferred-work.md] and Story 2.1 review — relevant to consumer wiring (2.9), not blocking 2.2:

- Large SysEx `MemoryBlock` copy under mutex on dequeue — delay gate adds **additional** MIDI-thread blocking; acceptable for Story 2.2; profile before optimizing mutex hold time
- Consumer must loop `dequeue()` without `isEmpty()` guard (TOCTOU)
- `sendSysExWithDelay` RPC path and future queue consumer **share** one `SysExInterMessageDelay` instance on `MidiManager` so inter-message gap is global across all outbound SysEx

### Test Pattern

Mirror [Tests/Unit/MidiOutboundQueueTests.cpp] — static `juce::UnitTest` subclass, no `MinimalAudioProcessor`.

**SysExInterMessageDelayTests** must use synthetic timestamps only — no `sleep_for` in tests (deterministic CI).

Example:

```cpp
Core::SysExInterMessageDelay gate { Core::SysExDelayProfile::stockDefault() };
gate.recordSysExSent(100);
expectEquals(gate.millisUntilNextAllowed(105), 5);
expectEquals(gate.millisUntilNextAllowed(110), 0);
```

Include `SysExDecoder.h` / build `DeviceIdInfo` literals in profile tests — no hardware.

### CMakeLists.txt — Insertion Points

After `Source/Core/MIDI/Queue/MidiOutboundQueue.cpp` (~line 128):

```cmake
Source/Core/MIDI/Queue/SysExDelayProfile.cpp
Source/Core/MIDI/Queue/SysExInterMessageDelay.cpp
```

Test target — after `MidiOutboundQueueTests.cpp`:

```cmake
Tests/Unit/SysExDelayProfileTests.cpp
Tests/Unit/SysExInterMessageDelayTests.cpp
Source/Core/MIDI/Queue/SysExDelayProfile.cpp
Source/Core/MIDI/Queue/SysExInterMessageDelay.cpp
```

### Project Structure Notes

- Both new classes live in `Source/Core/MIDI/Queue/` per architecture table ([architecture.md:476,503])
- Namespace `Core` throughout
- Allman braces, 4-space indent, `k` prefix constants, private members with `_` suffix
- Include paths: `"Core/MIDI/Queue/..."` (matches existing queue files)

### References

- Story 2.2 epic AC — [epics.md:468-479]
- D-078 EPROM-adaptive delay — [.decision-log.md:578-582]
- SysEx delay profiles table — [addendum.md:259-271]
- AD-3 queue + delay — [architecture.md:130-143]
- Architecture gap #3 (matching rules) — [architecture.md:634]
- M-1000 10 ms / M-6 20 ms — [architecture.md:56], [oberheim-matrix-6-6r-midi-sysex-implementation.md:322]
- Existing delay constant — [SysExConstants.h:55]
- Device Inquiry decode — [SysExDecoder.cpp:172-200], [SysExDecoder.h:9-18]
- `sendSysExWithDelay` migration target — [MidiManager.cpp:177-182]
- `performDeviceInquiry` profile hook — [MidiManager.cpp:198-221]
- Story 2.1 queue (unchanged) — [MidiOutboundQueue.h], [2-1-midioutboundqueue-core.md]
- Deferred queue findings — [deferred-work.md]
- Test baseline count — [2-1-midioutboundqueue-core.md] (35 tests)

## Dev Agent Record

### Agent Model Used

Composer (Cursor)

### Debug Log References

### Completion Notes List

- `SysExDelayProfile` + `SysExInterMessageDelay` in `Source/Core/MIDI/Queue/`; EPROM class from TAUNTEK/GLIGLI/NORDCORE substring match; device family from member bytes (M-1000 `0x02/0x00`, provisional M-6 `0x01/0x00`).
- `MidiManager`: `sysExDelay_` stock default; inquiry updates profile; `sendSysExWithDelay` wait-before / record-after EOX. `run()` stub unchanged.
- 9 new unit tests (6 profile + 3 gate); full suite green with `MATRIX_BUILD_TESTS=ON`.
- VST3 + test target build verified (`Builds/macOS/ARM`).

### File List

- Source/Core/MIDI/Queue/SysExDelayProfile.h
- Source/Core/MIDI/Queue/SysExDelayProfile.cpp
- Source/Core/MIDI/Queue/SysExInterMessageDelay.h
- Source/Core/MIDI/Queue/SysExInterMessageDelay.cpp
- Source/Core/MIDI/MidiManager.h
- Source/Core/MIDI/MidiManager.cpp
- Source/Core/MIDI/SysEx/SysExConstants.h
- Tests/Unit/SysExDelayProfileTests.cpp
- Tests/Unit/SysExInterMessageDelayTests.cpp
- CMakeLists.txt

### Change Log

- 2026-06-04: Story 2.2 — EPROM-adaptive inter-SysEx delay profile and gate; MidiManager migration.
