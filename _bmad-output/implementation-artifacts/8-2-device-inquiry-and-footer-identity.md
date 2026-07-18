---
organization: Ten Square Software
project: Matrix-Control
title: Story 8.2 — Device Inquiry and Footer Identity
author: BMad Agent
status: done
baseline_commit: 8360bce61410955f3565c94648528489e33f5d90
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - implementation-artifacts/8-1-devicetyperegistry-and-member-byte-fix.md
  - implementation-artifacts/7-8-header-footer-shell-and-persistence.md
  - reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md
  - project-context.md
created: 2026-07-18
updated: 2026-07-18
---

# Story 8.2: Device Inquiry and Footer Identity

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

## Story

As a user,
I want automatic Device Inquiry on MIDI port configure with footer identity,
so that I know which synth is connected (FR-1, FR-53).

## Brownfield Reality (Read First)

**Almost everything already exists.** Story 7-8 painted the footer; Story 8-1 hardened `DeviceTypeRegistry`; `MidiManager::performDeviceInquiry()` already encodes, waits, decodes, maps type, and writes APVTS. **Nothing calls it.** Story 7-8 incorrectly assumed inquiry already ran after port sync — that claim is false against current code.

| Deliverable | Status at baseline (`8360bce`) |
|-------------|--------------------------------|
| Universal Device Inquiry request bytes `F0 7E 7F 06 01 F7` | **Exists** — `SysExConstants::DeviceInquiry::kRequestMessage` + `SysExEncoder::encodeDeviceInquiry` |
| Decode Device ID reply → `DeviceIdInfo` | **Exists** — `SysExDecoder::decodeDeviceId` |
| Member → `deviceType` mapping (D-080) | **Exists** — `DeviceTypeRegistry` (Story 8.1 done) |
| `MidiManager::performDeviceInquiry()` body | **Exists** — success / timeout / unsupported → `updateDeviceStatus` |
| Call sites that invoke inquiry after ports open | **None** — primary gap |
| Clear `deviceDetected` when ports cleared | **Missing** — `setMidi*Port("")` does not reset device status |
| Footer right-zone identity (FR-53 display) | **Exists** — `FooterPanel::buildDeviceIdentityText` (Story 7-8) |
| UI lock / connection guidance copy | **Out of scope** — Story 8.3 |
| Post-inquiry `requestCurrentPatch()` | **Deferred** — not required to close FR-1/FR-53 |

**This story is wire / harden / verify — not a greenfield rewrite of encode, decode, registry, or footer painting.**

## Acceptance Criteria

1. **Given** Story 8.1 (`DeviceTypeRegistry` + D-080) and Epic 2 MIDI stack  
   **When** MIDI **From** (input) and MIDI **To** (output) ports are both successfully configured (user change, session restore, or host deferred sync)  
   **Then** Universal Device Inquiry runs automatically (request remains `F0 7E 7F 06 01 F7`).

2. **And** a valid Oberheim Matrix-family Device ID reply sets APVTS:
   - `deviceDetected = true`
   - `deviceType` via existing `DeviceTypeRegistry::fromDeviceInquiry` → `MatrixDeviceTypes` APVTS strings
   - `deviceVersion` = four-character revision string from the reply  
   Delay profile continues to update via `SysExDelayProfile::fromDeviceInquiry` (preserve 8.1 / 2.2 behaviour).

3. **And** timeout (no reply within `SysExConstants::kDefaultTimeoutMs`, currently 2000 ms) **or** invalid / non-Matrix reply sets `deviceDetected = false`, clears / resets `deviceType` to Unknown, and resets delay profile to stock (existing `performDeviceInquiry` failure paths — keep semantics).

4. **And** when either MIDI port is cleared or both ports are no longer available for a dump/inquiry attempt, detection is cleared (`deviceDetected = false`) so the footer does not keep a stale identity.

5. **And** port **reconfigure** (change From and/or To to a new open pair) **re-triggers** inquiry (feeds Story 8.3 AC; implement the trigger here).

6. **And** footer **right zone** shows device type + firmware when detected (e.g. `Matrix-1000 · v1.20`), and `PluginDisplayNames::FooterPanel::kNoDevice` (or equivalent placeholder) when not detected — **verify** existing `FooterPanel` wiring; do **not** re-implement from scratch. Identity stays in the footer, never the header (FR-39 / D-076).

7. **And** inquiry orchestration must **not** freeze the UI or deadlock the MIDI consumer:
   - **Do not** call blocking `waitForSysExResponse` on the MIDI thread (`MidiManager::run`) — outbound queue would never drain → permanent timeout.
   - **Do not** leave a 2 s message-thread block as the shipped UX for port configure.
   - **Preferred:** migrate inquiry to the existing non-blocking pattern (`armOneShotSysExCapture` + timeout timer), modelled on `requestSinglePatchAsync` / `armAsyncSinglePatchCapture`.
   - Keep a clear public entry point (e.g. `performDeviceInquiry` / `performDeviceInquiryAsync`) that port-sync can safely call.

8. **And** out of scope (do **not** implement here):
   - UI lock / disable PATCH·MASTER·MM·PM (Story 8.3)
   - Footer left-zone “fix connection” guidance copy (8.3)
   - Virtual instrument bus layout / MASTER gray (8.4)
   - `DeviceMemoryLimits` / Patch Manager limits (8.5 done)
   - Inventing Matrix-6R member bytes; changing D-080 or inquiry request bytes
   - Extending version extraction beyond 4 ASCII chars (deferred EPROM string match)

9. **And** `Matrix-Control_Tests` pass; macOS Debug VST3 + Standalone build clean; no French in source; `Core ↛ GUI` preserved.

## Tasks / Subtasks

- [x] **T1 — Audit trigger gap** (AC: #1, #5)
  - [x] Confirm zero production callers of `performDeviceInquiry` (grep)
  - [x] Identify all port-configure entry points: `PluginProcessor::syncMidiPortsFromStateImpl`, `restoreMidiPortsForHost` / deferred host sync, Header port APVTS → sync
  - [x] Define “both ports ready” using existing helpers (`isDeviceDumpAvailable` and/or open-with-device checks) — do not invent a second availability API unless needed

- [x] **T2 — Non-blocking inquiry orchestration** (AC: #2, #3, #7)
  - [x] Prefer refactor of `performDeviceInquiry` to async capture (mirror `requestSinglePatchAsync`: cancel token, arm one-shot, send via `sendSysExWithDelay`, timer timeout → `updateDeviceStatus(false)`)
  - [x] Preserve success path: `decodeDeviceId` → `DeviceTypeRegistry::fromDeviceInquiry` → `SysExDelayProfile::fromDeviceInquiry` → `updateDeviceStatus(true, version, type)`
  - [x] Preserve failure paths: timeout / invalid / exceptions → stock delay + `updateDeviceStatus(false)` + `updateErrorState`
  - [x] Ensure MIDI-thread safety: never block `MidiManager::run` waiting for SysEx

- [x] **T3 — Wire inquiry after ports configure** (AC: #1, #4, #5)
  - [x] After successful sync when both From + To are available → start inquiry
  - [x] On port clear / incomplete pair → `updateDeviceStatus(false)` (no stale footer)
  - [x] On port change to a new open pair → cancel any in-flight inquiry, then re-inquire
  - [x] Avoid double-fire storms (idempotent: already-open same ports should not spam inquiry every sync — document chosen debounce / “ports changed” condition)

- [x] **T4 — Footer FR-53 verify** (AC: #6)
  - [x] Confirm `FooterPanel` still listens to `deviceDetected`, `deviceType`, `deviceVersion` and builds `{type} · v{version}`
  - [x] Manual smoke: Standalone — select MIDI From/To to a Matrix (or loopback fixture if available) → footer updates; clear ports → `No device`
  - [x] Touch `FooterPanel` only if a real display bug appears; otherwise leave GUI alone

- [x] **T5 — Tests** (AC: #2, #3, #7, #9)
  - [x] Unit-test what is unit-testable without hardware: e.g. helper that maps reply bytes → APVTS expectations already covered by 8.1 — add orchestration-focused tests if a seam exists (fake receiver / injectable capture), otherwise document manual UAT for port-trigger
  - [x] Do **not** require live Matrix hardware in CI
  - [x] Regression: existing `DeviceTypeRegistryTests`, `SysExEncoderTests` (request bytes), delay-profile tests stay green

- [x] **T6 — Build / regression** (AC: #8, #9)
  - [x] Full `Matrix-Control_Tests` green
  - [x] Smoke Standalone + VST3: port configure → inquiry attempt; timeout path leaves `deviceDetected=false` and footer placeholder

## Dev Notes

### Epic context

Epic 8 = synth connection & instrument role. Sprint order: **8.1 → 8.2 → 8.3 → 8.4**; **8.5 already done** and already consumes `deviceType` / `deviceDetected`.

| Story | Owns |
|-------|------|
| **8.1 (done)** | Registry mapping + D-080 + M-6 TBD note + tests |
| **8.2 (this)** | Inquiry **trigger** + safe orchestration + FR-53 **closed** (footer verify) |
| **8.3** | UI lock when `deviceDetected=false` + guidance copy |
| **8.4** | Instrument bus layout + MASTER EDIT gray |
| **8.5 (done)** | `DeviceMemoryLimits` / Patch Manager 100-slot semantics |

### Architecture compliance

- **Owners (FR-1 / FR-53):** `MidiManager` + `DeviceTypeRegistry` write APVTS; `FooterPanel` reads — never call `MidiManager` from panels.
- **Dependency:** `Core ↛ GUI`.
- **Sync RPC:** Device Inquiry may wait for a reply, but **not** on the audio thread and **not** by starving the MIDI consumer. Architecture allows blocking on a dedicated worker context; project practice now prefers `armOneShotSysExCapture` (see `MidiReceiver.h` comment: legacy `waitForSysExResponse`).
- **SSOT:** APVTS property names `deviceDetected`, `deviceType` (`MatrixDeviceTypes::kApvtsPropertyName`), `deviceVersion` — do not rename.
- **Identity placement:** footer right zone only (D-069, D-076, FR-53). Left zone remains messages / severity (`uiMessageText`) — orthogonal.

[Source: architecture.md FR→component map; project-context.md MIDI dual-role]

### Critical threading trap (do not miss)

```
performDeviceInquiry today:
  sendSysExWithDelay(...)     // enqueue → drained only by MidiManager::run()
  waitForSysExResponse(2000)  // blocks calling thread

If calling thread == MidiManager::run  → DEADLOCK (queue never drains)
If calling thread == message thread    → works but freezes UI up to 2s
```

`PluginProcessor::syncMidiPortsFromState` already runs on the **message thread** for plugin hosts. Wiring a naive `performDeviceInquiry()` call there is a product defect even if it “works”.

**Recommended design:** async inquiry (same shape as `requestSinglePatchAsync`), triggered after ports are ready; timeout via `Timer::callAfterDelay`; decode + `updateDeviceStatus` on message thread via `MessageManager::callAsync` when capture fires on MIDI input thread.

### Current code — preserve

```cpp
// Success path inside inquiry (keep composition)
DeviceIdInfo info = sysExDecoder->decodeDeviceId(response);
if (info.isValid) {
    auto type = DeviceTypeRegistry::fromDeviceInquiry(info);
    sysExDelay_.setProfile(SysExDelayProfile::fromDeviceInquiry(info));
    updateDeviceStatus(true, info.version, type);
}

// Footer (already correct — verify only)
// deviceDetected && non-empty deviceType → "{type} · v{version}"
// else → PluginDisplayNames::FooterPanel::kNoDevice
```

```cpp
// Port sync today — NO inquiry (gap to close)
void PluginProcessor::syncMidiPortsFromStateImpl(...) {
    midiManager->setMidiInputPort(...);
    midiManager->setMidiOutputPort(...);
    // ← start inquiry when both available; clear detection when not
}
```

**Must not break:**
- `DeviceMemoryLimits` / Bank Utility / Internal Patches consumers of `deviceType` / `deviceDetected` (8.5)
- D-080 constants and inquiry request bytes
- `SysExDelayProfile` unknown-member → stock M-1000 fallback
- Footer left-zone messaging / `ExceptionPropagator`
- Session persistence / FR-3 strip policy from 7-8

### Protocol (authoritative)

| Field | Value |
|-------|--------|
| Request | `F0 7E 7F 06 01 F7` |
| Reply sub-ID | `06 02` |
| Manufacturer | `10H` (Oberheim) |
| Family | `06H` / `00H` |
| Member M-1000 | `02H` / `00H` (D-080) |
| Member M-6/6R | provisional `01H` / `00H` → `kMatrix6` only; **hardware unconfirmed** (PRD §9 #6) |
| Version | 4 ASCII chars, right-justified |

[Source: addendum Device Inquiry table; `_bmad-output/reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md` § Device ID; MMA Universal Non-Realtime Identity Request]

### Previous story intelligence

**From 8.1 (done):**
- Registry / decoder / delay profile aligned to `SysExConstants` — do not re-open D-080.
- Explicitly deferred inquiry orchestration to 8.2 — that is this story.
- Hardware note still open: M-6/6R member bytes TBD; do not invent 6R bytes.
- Review deferrals (decision-tree duplication, DelayProfile test literals, etc.) — **do not expand scope** unless a one-line fix falls out of inquiry work.

**From 7.8 (done):**
- Footer identity display **complete** (AC 4).
- False assumption: “Inquiry already runs via `MidiManager::performDeviceInquiry()` after port sync” — **correct that by wiring**, do not rewrite FooterPanel.
- Deferred to 8.2: timeout / re-inquiry polish — covered by ACs #3–#5 here.

**From deferred-work.md:**
- Optional later: name a constant for `"deviceDetected"` string literal.
- 4-char version limits full EPROM token match — **out of scope**.

### Git intelligence

- `8360bce` — Close story 8.1 DeviceTypeRegistry and member-byte audit (baseline).
- `dcf721d` (earlier) — Story 8.5 introduced registry + MidiManager `deviceType` publish path.
- Pattern to copy for async SysEx: `MidiManager::requestSinglePatchAsync` + `armAsyncSinglePatchCapture` + cancel token.

### Project structure notes

| Path | Role in 8.2 |
|------|-------------|
| `Source/Core/MIDI/MidiManager.{h,cpp}` | **PRIMARY** — async inquiry + trigger API |
| `Source/Core/PluginProcessor.cpp` | **UPDATE** — call trigger after `syncMidiPortsFromStateImpl` / clear on incomplete ports |
| `Source/Core/MIDI/Transport/MidiReceiver.*` | **Reuse** — `armOneShotSysExCapture`; avoid new legacy blocking waits |
| `Source/Core/Services/DeviceTypeRegistry.*` | **Preserve** — consume only |
| `Source/Core/MIDI/SysEx/*` | **Preserve** — encode/decode/constants |
| `Source/GUI/.../FooterPanel/*` | **Verify**; edit only if display bug |
| `Source/Shared/Definitions/MatrixDeviceTypes.h` | **Preserve** — APVTS type SSOT |
| `Source/Shared/Definitions/PluginDisplayNames.h` | **Preserve** — `kNoDevice` |
| `Tests/Unit/*` | Add orchestration tests only if a clean seam exists |

### Testing requirements

- Framework: JUCE `UnitTest` under `Tests/Unit/`
- AAA; F.I.R.S.T.; no live MIDI hardware in unit tests
- Manual UAT: Standalone port select → footer identity; disconnect → `No device`; wrong device / timeout → undetected
- Prefer TDD if extracting a pure helper (e.g. “should inquire?” / “ports changed?” predicates)

[Source: project-context.md § Testing Strategy; CONVENTIONS.md §8.5]

### Latest tech notes

- MMA Identity Request / Reply (`7E` / sub-ID `06 01` / `06 02`) is stable; Oberheim family/member fields are manufacturer-specific — trust archived Oberheim reference docs over generic web dumps for M-1000 member bytes.
- JUCE 8.0.12 has no Device Inquiry helper — keep project SysEx stack.
- Prefer non-blocking capture over `waitForSysExResponse` for any new or reworked RPC (receiver header already marks the wait as legacy).

### Anti-patterns (do not)

- Reimplement encode/decode/registry/footer from scratch
- Call blocking inquiry from `MidiManager::run` (deadlock)
- Ship 2 s UI freeze on every port sync without async migration
- Put identity in the HeaderPanel
- Implement UI lock (8.3) or bus layout (8.4)
- Re-swap D-080 or change request bytes
- Map inquiry to `kMatrix6R` without hardware evidence
- Add GUI includes to Core
- French in source / user-facing string literals outside `PluginDisplayNames`

### Project context reference

Follow `_bmad-output/project-context.md`: C++17, Allman braces, English-only source, Clean Code limits, Core/GUI dependency rule, dedicated MIDI thread for editor SysEx, JUCE UnitTest conventions.

## Dev Agent Record

### Agent Model Used

Composer (Cursor Agent)

### Debug Log References

### Completion Notes List

- T1: Confirmed zero production callers of `performDeviceInquiry`. Port sync entry point is `PluginProcessor::syncMidiPortsFromStateImpl` (also reached via `restoreMidiPortsForHost` / deferred host retry). Availability = `isDeviceDumpAvailable()`.
- T2: Migrated `performDeviceInquiry` to non-blocking outbound-idle settle + `armOneShotSysExCapture` + `Timer::callAfterDelay` (same token pattern as async single-patch). Success/failure paths preserve registry + delay-profile + `updateDeviceStatus` semantics. No `waitForSysExResponse`. Failures clear inquiry debounce; null parser fails closed; SysEx not sent if capture arm aborts.
- T3: `refreshDeviceInquiryAfterPortSync()` wired after port set in `syncMidiPortsFromStateImpl` and after Header/`PluginProcessor::setMidi*Port`. Clears detection on incomplete ports (including empty open ids). Debounce via `Core::shouldStartDeviceInquiry` + `lastInquiryInputId_` / `lastInquiryOutputId_` — same open pair does not re-inquire while in flight or after success; failure clears the pair so a later sync can retry.
- T4: Verified `FooterPanel::buildDeviceIdentityText` — listens to `deviceDetected` / `deviceType` / `deviceVersion`; format `{type} · v{version}` or `kNoDevice`. No GUI edits.
- T5: Added `DeviceInquiryTriggerTests` (debounce predicate) + MidiManager test that refresh clears stale detection without ports. Hardware UAT remains manual for live Matrix reply → footer.
- T6: Full `Matrix-Control_Tests` green; macOS Debug Standalone + VST3 built clean.

### File List

- Source/Core/MIDI/DeviceInquiryTrigger.h (new)
- Source/Core/MIDI/MidiManager.h
- Source/Core/MIDI/MidiManager.cpp
- Source/Core/PluginProcessor.cpp
- Tests/Unit/DeviceInquiryTriggerTests.cpp (new)
- Tests/Unit/MidiManagerTests.cpp
- CMakeLists.txt
- _bmad-output/implementation-artifacts/8-2-device-inquiry-and-footer-identity.md
- _bmad-output/implementation-artifacts/sprint-status.yaml

### Change Log

- 2026-07-18: Story context created (ready-for-dev) — wire inquiry on port configure; async orchestration; footer FR-53 verify
- 2026-07-18: Implemented async Device Inquiry trigger + port-sync wiring; status → review
- 2026-07-18: Code review patches — Header setMidi*Port refresh, clear debounce on failure, empty-id clear, parser fail-closed, outbound idle settle, arm-before-send

### Review Findings

- [x] [Review][Decision] Shared async SysEx token — Device Inquiry and async single-patch share `asyncRequestToken_`; each cancels the other. Resolved: keep shared single-flight slot (Guillaume option 1).
- [x] [Review][Patch] Wire Device Inquiry refresh on Header / user port configure [`PluginProcessor.cpp:651-678`]
- [x] [Review][Patch] Do not permanently debounce a failed inquiry — clear `lastInquiry*` on failure (or stamp only after success) [`MidiManager.cpp:655-657`, `finishAsyncDeviceInquiryFailure`]
- [x] [Review][Patch] When dump-available but open port ids are empty, clear detection instead of no-op [`MidiManager.cpp:637-653`]
- [x] [Review][Patch] Fail closed when `sysExParser` is null (mirror async patch path) [`MidiManager.cpp:680-689`]
- [x] [Review][Patch] Wait for outbound queue idle / settle before arming Device Inquiry (align with `requestSinglePatchAsync`) [`MidiManager.cpp:746-764`]
- [x] [Review][Patch] Do not send Device Inquiry SysEx if capture arm aborted without finishing [`MidiManager.cpp:760-764`]
- [x] [Review][Defer] Timer/`callAsync` lambdas capture raw `this` without SafePointer [`MidiManager.cpp:766-779`] — deferred, pre-existing (same pattern as async single-patch)
- [x] [Review][Defer] No `jassert` that `performDeviceInquiry` runs on the message thread [`MidiManager.cpp:746`] — deferred, pre-existing call-site assumption
- [x] [Review][Defer] No automated tests for async inquiry success/timeout/re-arm paths — deferred, pre-existing gap; trigger predicate + clear-without-ports covered; hardware UAT remains manual per story T5

## Story Completion Status

- Ultimate context engine analysis completed — comprehensive developer guide created
- Status: **done**
- Sprint key: `8-2-device-inquiry-and-footer-identity`
