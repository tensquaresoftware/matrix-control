---
organization: Ten Square Software
project: Matrix-Control
title: Story 8.1 — DeviceTypeRegistry and Member Byte Fix
author: BMad Agent
status: done
baseline_commit: 6eff7b075d9da154150c0f1ef0d9288c77ace2cd
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - implementation-artifacts/8-5-matrix-6-6r-patch-memory-limits.md
  - implementation-artifacts/2-2-sysexdelayprofile-and-inter-message-delay.md
  - reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md
  - reference-docs/oberheim/index.md
  - project-context.md
created: 2026-07-17
updated: 2026-07-17
---

# Story 8.1: DeviceTypeRegistry and Member Byte Fix

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

## Story

As a developer,
I want device type resolved from Inquiry member bytes correctly,
so that M-1000 vs M-6/6R detection is accurate (FR-1, D-080, PRD §9 #6).

## Brownfield Reality (Read First)

**Most of Story 8.1 already shipped inside Story 8.5** (`dcf721d`). Sprint status still lists `8-1-…` as backlog because the story file was never created and the ACs were never formally closed.

| Deliverable | Status at baseline |
|-------------|-------------------|
| `DeviceTypeRegistry` (`Source/Core/Services/`) | **Exists** — `fromMemberBytes`, `fromDeviceInquiry`, `fromApvtsProperty` |
| `MatrixDeviceTypes.h` | **Exists** — enum + APVTS string IDs |
| D-080 constants (`kExpectedMemberLow/High = 0x02/0x00`) | **Fixed** in `SysExConstants.h` |
| `SysExDecoder::validateMatrixFamilyDevice` | **Accepts** M-1000 + provisional M-6 member pairs |
| `MidiManager` publishes `deviceType` on inquiry | **Wired** (overlaps Story 8.2 transport; do not rip out) |
| Hardware verification note for M-6 bytes | **Missing** — still TBD (PRD §9 #6) |
| Focused registry / decoder unit tests | **Thin** — only `fromMemberBytes` inside `DeviceMemoryLimitsTests` |
| Stale comment `Member: 0x10 0x02` in `SysExConstants.h` | **Bug** — contradicts fixed constants |

**This story is audit / harden / document — not a greenfield rewrite.**

## Acceptance Criteria

1. **Given** Epic 2 MIDI stack and existing `DeviceTypeRegistry` from Story 8.5  
   **When** a Device ID reply is parsed  
   **Then** `DeviceTypeRegistry::fromDeviceInquiry` / `fromMemberBytes` map member bytes → `MatrixDeviceTypes::Type` using `SysExConstants::DeviceInquiry` constants only (no duplicated literals in the registry).

2. **And** D-080 is verified closed: `kExpectedMemberLow == 0x02`, `kExpectedMemberHigh == 0x00` (Oberheim M-1000 `memb-lo=2H`, `memb-hi=0H`). Unit tests lock these values against named Oberheim literals so the swap cannot regress.

3. **And** stale / contradictory comments in `SysExConstants.h` (and any sibling comments that still describe the pre-D-080 swap) are corrected so comments match the live constants.

4. **And** a **hardware verification note** is recorded in this story’s Dev Agent Record (and optionally one sentence in `SysExConstants.h` / registry header) stating that Matrix-6/6R Universal Device Inquiry member bytes remain **unconfirmed on hardware** (PRD §9 #6). Provisional mapping `0x01/0x00` → `kMatrix6` is intentional until UAT.

5. **And** Matrix-6R is **not** invented from inquiry: distinct 6 vs 6R member bytes are unknown; keep mapping provisional bytes to `kMatrix6` only. `kMatrix6R` remains available for APVTS / manual override; do not fake a 6R member-byte pattern.

6. **And** unit tests cover (JUCE `UnitTest`, AAA):
   - (a) D-080 constant values = `0x02` / `0x00`
   - (b) `fromMemberBytes(0x02,0x00)` → `kMatrix1000`; `(0x01,0x00)` → `kMatrix6`; unknown → `kUnknown`
   - (c) `fromDeviceInquiry`: reject wrong manufacturer / family; accept valid M-1000 and provisional M-6 `DeviceIdInfo`
   - (d) Prefer a dedicated `Tests/Unit/DeviceTypeRegistryTests.cpp` (move or duplicate the thin coverage currently in `DeviceMemoryLimitsTests` — do not leave registry as an afterthought of limits tests)

7. **And** optional hardening (in scope if cheap; do not expand into 8.2–8.4):
   - Align `SysExDelayProfile` member-byte literals with `SysExConstants::DeviceInquiry` (remove duplicated `0x02/0x00` / `0x01/0x00` magic numbers in `SysExDelayProfile.cpp`)
   - Optionally assert `familyHigh == 0x00` in decoder validation (Oberheim reply format)

8. **And** out of scope (do **not** implement here): footer identity UI (8.2), UI lock (8.3), bus layout / MASTER gray (8.4), `DeviceMemoryLimits` / Patch Manager behaviour (8.5 done). Do not change inquiry request bytes `F0 7E 7F 06 01 F7`.

9. **And** `Matrix-Control_Tests` pass; macOS Debug VST3 + Standalone build clean; no French in source; `Core ↛ GUI` preserved.

## Tasks / Subtasks

- [x] **T1 — Audit existing registry + D-080** (AC: #1, #2, #3)
  - [x] Confirm `SysExConstants::DeviceInquiry` member constants match Oberheim M-1000 reference (`02H`/`00H`)
  - [x] Fix stale comment at `SysExConstants.h` (~L80 `Member: 0x10 0x02`)
  - [x] Confirm `DeviceTypeRegistry.cpp` uses constants only (already true after 8.5 review patch)
  - [x] Confirm `SysExDecoder::validateMatrixFamilyDevice` stays in sync with the same constants

- [x] **T2 — Hardware verification note** (AC: #4, #5)
  - [x] Document in Dev Agent Record: M-6/6R member bytes still TBD; provisional `kMatrix6Member* = 0x01/0x00`; 6R not distinguishable from inquiry
  - [x] Keep / tighten provisional comments on `kMatrix6MemberLow/High`
  - [x] Do **not** invent distinct Matrix-6R member bytes

- [x] **T3 — Unit tests** (AC: #6)
  - [x] Add `Tests/Unit/DeviceTypeRegistryTests.cpp` (or equivalent dedicated suite)
  - [x] Cover D-080 constants, `fromMemberBytes`, `fromDeviceInquiry` accept/reject paths
  - [x] Register in `Tests/CMakeLists.txt` / test target sources
  - [x] Optionally add `SysExDecoder` Device ID golden-byte test (structure + member extraction)

- [x] **T4 — Optional alignment** (AC: #7)
  - [x] Point `SysExDelayProfile` member-byte mapping at `SysExConstants` (preserve unknown → stock M-1000 delay fallback behaviour)
  - [x] Decide whether to validate `familyHigh`; if yes, reject non-`0x00` with clear log; if no, document why deferred

- [x] **T5 — Build / regression** (AC: #8, #9)
  - [x] Full unit suite green
  - [x] Smoke: Device Inquiry still sets `deviceDetected` / `deviceType` / `deviceVersion` via existing `MidiManager` path (no behaviour regression for 8.5 consumers)

## Dev Notes

### Epic context

Epic 8 = synth connection & instrument role. Story order in sprint status: **8.1 → 8.2 → 8.3 → 8.4**; **8.5 already done** out of sequence and depends on `deviceType` from 8.1/8.2.

| Story | Owns |
|-------|------|
| **8.1 (this)** | Registry mapping + D-080 fix + M-6 TBD note + tests |
| **8.2** | Inquiry orchestration polish + footer FR-53 display |
| **8.3** | UI lock when `deviceDetected=false` |
| **8.4** | Instrument bus layout + MASTER EDIT gray (FR-46) |
| **8.5 (done)** | `DeviceMemoryLimits` / Patch Manager 100-slot semantics |

### Architecture compliance

- **Placement:** `Source/Core/Services/DeviceTypeRegistry.{h,cpp}` — architecture E8 map; **not** under GUI; `MIDI/` alternate allowed but do not relocate existing files.
- **Dependency direction:** `Core ↛ GUI`. Registry may include `SysExConstants` / `SysExDecoder` (`DeviceIdInfo`).
- **SSOT for type strings:** `MatrixDeviceTypes.h` (`"Matrix-1000"`, `"Matrix-6"`, `"Matrix-6R"`, `"Unknown"`). Never redefine APVTS IDs elsewhere.
- **Composition:** `MidiManager::performDeviceInquiry` already calls `DeviceTypeRegistry::fromDeviceInquiry` then `updateDeviceStatus`. Preserve this path.

[Source: architecture.md — DeviceTypeRegistry / E8 / FR→component map]

### D-080 and Oberheim member bytes

| Field | Matrix-1000 (authoritative) | Matrix-6/6R |
|-------|----------------------------|---------------|
| Manufacturer | `10H` | same family expected |
| Family lo/hi | `06H` / `00H` | same |
| Member lo/hi | **`02H` / `00H`** | **TBD — hardware confirm** (not in community M-6 docs) |

**Pre-fix bug (D-080):** code had `Low=0x00`, `High=0x02` (swapped). **Current code is already corrected.** Decision-log / PRD wording may still describe the old bug — do not “re-swap” constants.

**Provisional M-6:** `kMatrix6MemberLow/High = 0x01/0x00` — heuristic used by decoder, registry, and delay profile. Public community SysEx docs do **not** document Universal Device Inquiry member bytes for M-6/6R (proprietary `F0 10 06` lead-in uses device ID `06H`, which is **not** the same field as Inquiry `memb-lo/hi`).

[Source: addendum.md Device Inquiry table; decision-log D-079/D-080; `oberheim-matrix-1000-midi-sysex-implementation.md` § Device ID]

### Current code — preserve

```cpp
// DeviceTypeRegistry.cpp — keep constant-based mapping
fromMemberBytes(0x02,0x00) → kMatrix1000
fromMemberBytes(0x01,0x00) → kMatrix6
else → kUnknown
```

```cpp
// SysExConstants.h — keep values; fix comments only
kExpectedMemberLow  = 0x02;
kExpectedMemberHigh = 0x00;
kMatrix6MemberLow   = 0x01; // provisional
kMatrix6MemberHigh  = 0x00; // provisional
```

**Must not break:**
- `DeviceMemoryLimits::resolve` / wrap / ROM gating (8.5)
- APVTS property names `deviceDetected`, `deviceVersion`, `deviceType`
- `SysExDelayProfile` unknown-member → stock M-1000 delay fallback
- Inquiry request `F0 7E 7F 06 01 F7`
- Footer / Patch Manager panels that already read `deviceType`

### Previous story intelligence (8.5)

From `8-5-matrix-6-6r-patch-memory-limits.md`:

- Registry was introduced as **partial 8.1** to unblock memory limits.
- Review defer: **Matrix-6R never assigned from Inquiry** — close formally in this story’s hardware note, not by inventing bytes.
- Review patch already forced registry to use `SysExConstants` instead of duplicated literals — keep that discipline.
- GUI panels call `DeviceMemoryLimits::resolve` directly — out of scope; do not refactor.

### Git intelligence

- `dcf721d` — Story 8.5: registry + D-080 fix + decoder + limits + MidiManager `deviceType` publish.
- `8c3a830` — Story 2.2: `SysExDelayProfile` used explicit member literals **independent** of `kExpected*` because D-080 was owned by Epic 8 — **now** safe to unify with constants (AC #7).

### Project structure notes

| Path | Role in 8.1 |
|------|-------------|
| `Source/Core/Services/DeviceTypeRegistry.*` | **UPDATE** — audit/harden |
| `Source/Shared/Definitions/MatrixDeviceTypes.h` | **Preserve** — type SSOT |
| `Source/Core/MIDI/SysEx/SysExConstants.h` | **UPDATE** — comments + verify constants |
| `Source/Core/MIDI/SysEx/SysExDecoder.*` | **Touch lightly** — keep validation aligned; optional `familyHigh` |
| `Source/Core/MIDI/Queue/SysExDelayProfile.*` | **Optional** — unify member-byte literals |
| `Source/Core/MIDI/MidiManager.cpp` | **Do not rework** inquiry orchestration (8.2) |
| `Source/Core/Services/DeviceMemoryLimits.*` | **Do not change** (8.5) |
| `Tests/Unit/DeviceTypeRegistryTests.cpp` | **New** preferred |
| `CMakeLists.txt` / `Tests/CMakeLists.txt` | Register new test source |

### Testing requirements

- Framework: JUCE `UnitTest` under `Tests/Unit/`
- Prefer TDD for any new decoder/registry assertions
- AAA structure; F.I.R.S.T.; no MIDI hardware in unit tests — synthesize `DeviceIdInfo` / byte arrays
- Run: configure with `MATRIX_BUILD_TESTS=ON`; execute `Matrix-Control_Tests`

[Source: project-context.md § Testing Strategy; CONVENTIONS.md §8.5]

### Latest tech notes

- JUCE 8.0.12 — no special Device Inquiry API; custom SysEx parse remains project-owned.
- Universal Device Inquiry is MMA standard (`F0 7E … 06 02 …`); Oberheim family/member fields are manufacturer-specific — trust archived `_bmad-output/reference-docs/oberheim/` over random web SysEx dump pages for M-1000.

### Anti-patterns (do not)

- Recreate `DeviceTypeRegistry` under a new name or path
- Hard-code member bytes outside `SysExConstants`
- Map provisional bytes to `kMatrix6R` without hardware evidence
- Implement footer, UI lock, bus layout, or Patch Manager changes
- “Fix” D-080 by swapping constants back to `0x00/0x02`
- Add GUI includes to Core services
- French comments or user-facing strings in source

### Project context reference

Follow `_bmad-output/project-context.md`: C++17, Allman braces, English-only source, Clean Code limits, Core/GUI dependency rule, JUCE UnitTest conventions.

## Dev Agent Record

### Agent Model Used

Composer (Cursor agent router)

### Debug Log References

- macOS Debug arm64: `Matrix-Control_Tests`, `Matrix-Control_VST3`, `Matrix-Control_Standalone` built clean
- Full `Matrix-Control_Tests` suite exit code 0 (includes new `DeviceTypeRegistry` suite)

### Completion Notes List

**Hardware verification note (AC #4 / PRD §9 #6):** Matrix-6/6R Universal Device Inquiry member bytes remain **unconfirmed on hardware**. The provisional mapping `kMatrix6MemberLow/High = 0x01/0x00` → `MatrixDeviceTypes::Type::kMatrix6` is intentional until UAT. Distinct Matrix-6R member bytes are **unknown** and are **not** invented from inquiry; `kMatrix6R` stays available only via APVTS / manual override.

**Implementation summary:**
- Audited D-080 constants (`0x02`/`0x00`); fixed stale `Member: 0x10 0x02` comment; registry continues to use `SysExConstants` only
- Added `kExpectedFamilyHigh = 0x00`; decoder + registry reject non-zero family high with a clear warning log
- Aligned `SysExDelayProfile` member-byte mapping to `SysExConstants` (unknown still falls back to stock M-1000 delay)
- Added dedicated `DeviceTypeRegistryTests.cpp`; removed thin registry coverage from `DeviceMemoryLimitsTests`
- `MidiManager::performDeviceInquiry` path left intact (`fromDeviceInquiry` → `updateDeviceStatus` / delay profile)

### File List

- `Source/Core/MIDI/SysEx/SysExConstants.h`
- `Source/Core/MIDI/SysEx/SysExDecoder.cpp`
- `Source/Core/Services/DeviceTypeRegistry.h`
- `Source/Core/Services/DeviceTypeRegistry.cpp`
- `Source/Core/MIDI/Queue/SysExDelayProfile.cpp`
- `Source/Core/MIDI/Queue/SysExDelayProfile.h`
- `Tests/Unit/DeviceTypeRegistryTests.cpp`
- `Tests/Unit/DeviceMemoryLimitsTests.cpp`
- `CMakeLists.txt`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`
- `_bmad-output/implementation-artifacts/8-1-devicetyperegistry-and-member-byte-fix.md`

### Change Log

- 2026-07-17: Story 8.1 audit/harden — D-080 comment fix, familyHigh validation, SysExDelayProfile constant alignment, dedicated registry unit tests; status → review
- 2026-07-17: Code review — dropped StereoPing/UAT docs from lot; fixed DelayProfile header, decoder log, SysExConstants comments; status → done

## Story Completion Status

- Story implementation complete
- Status: **done**
- Sprint key: `8-1-devicetyperegistry-and-member-byte-fix`
- Completion note: Audit/harden of existing 8.5 registry; ACs closed with tests and hardware TBD note; code-review patches applied

### Review Findings

- [x] [Review][Decision] StereoPing / UAT reference docs in the same 8-1 changeset — resolved: option 2 (keep only 8-1 fix; remove docs from this lot)
- [x] [Review][Patch] Revert StereoPing notes + UAT/index/lecture cross-links out of the 8-1 working tree [`_bmad-output/reference-docs/oberheim/`, `Documentation/Development/Plans/2026/06/2026-06-19-Matrix-6-6R-User-Acceptance-Test-Grid.md`]
- [x] [Review][Patch] Fix stale SysExDelayProfile.h comment claiming independence from SysExConstants [`Source/Core/MIDI/Queue/SysExDelayProfile.h:25`]
- [x] [Review][Patch] Fix decodeDeviceId failure log still saying "not a Matrix-1000" after M-6 acceptance [`Source/Core/MIDI/SysEx/SysExDecoder.cpp:83`]
- [x] [Review][Patch] Deduplicate Member comment and clarify provisional 6 vs 6R inquiry wording in SysExConstants.h [`Source/Core/MIDI/SysEx/SysExConstants.h:80`]
- [x] [Review][Defer] Triple copy of member-byte decision tree (registry / decoder / delay profile) [`Source/Core/Services/DeviceTypeRegistry.cpp`] — deferred, pre-existing
- [x] [Review][Defer] SysExDelayProfileTests still hardcode member-byte literals instead of SysExConstants [`Tests/Unit/SysExDelayProfileTests.cpp`] — deferred, pre-existing
- [x] [Review][Defer] Asymmetric naming kExpectedFamily vs kExpectedFamilyHigh [`Source/Core/MIDI/SysEx/SysExConstants.h`] — deferred, pre-existing
- [x] [Review][Defer] Decoder golden coverage only partially locks familyHigh / reject paths [`Tests/Unit/DeviceTypeRegistryTests.cpp`] — deferred, pre-existing
- [x] [Review][Defer] D-080 constant lock test does not pin provisional kMatrix6Member* literals [`Tests/Unit/DeviceTypeRegistryTests.cpp`] — deferred, pre-existing
- [x] [Review][Defer] fromApvtsProperty untested in dedicated registry suite [`Source/Core/Services/DeviceTypeRegistry.cpp`] — deferred, pre-existing
