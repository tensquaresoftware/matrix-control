---
organization: Ten Square Software
project: Matrix-Control
title: Passation — Matrix Device Inquiry Simulator (IAC)
author: BMad Agent
status: ready-for-create-story
sources:
  - implementation-artifacts/8-1-devicetyperegistry-and-member-byte-fix.md
  - implementation-artifacts/8-2-device-inquiry-and-footer-identity.md
  - implementation-artifacts/8-3-ui-lock-without-synth.md
  - Source/Core/MIDI/SysEx/SysExConstants.h
  - Source/Core/Services/DeviceTypeRegistry.h
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
created: 2026-07-18
updated: 2026-07-18
---

# Passation — Create Story: Matrix Device Inquiry Simulator

**Audience:** fresh `bmad-create-story` (or equivalent) session.  
**Owner ask (Guillaume, 2026-07-18):** Build a **tier-1** travel/dev tool that impersonates Oberheim Matrix devices over **macOS virtual MIDI (IAC)** so Device Inquiry / FR-2 lock unlock / footer identity / memory-limit gating can be tested without hardware. Guillaume owns a Matrix-1000 only; Matrix-6/6R hardware is unavailable.

**How to launch (fresh chat):**

```text
/bmad-create-story
```

Attach or paste this file. Then confirm story placement with Guillaume (see § Placement).

---

## Executive summary

| Item | Decision |
|------|----------|
| **Product need** | Portable stand-in for Matrix-1000 and Matrix-6 Device Inquiry replies |
| **Transport** | macOS IAC Driver virtual MIDI ports |
| **Scope (approved)** | **Tier 1 only** — Universal Device Inquiry **responder** (listen + reply). No full synth emulator. |
| **Out of scope (explicit)** | Editor SysEx stubs (tier 2), patch memory, note/CC synth, inventing distinct Matrix-6R member bytes |
| **Primary consumers** | Stories 8.2 / 8.3 / 8.5 UAT without hardware; travel MacBook workflow |

---

## User story (draft for create-story to refine)

**As a** developer travelling without Matrix hardware,  
**I want** a small app that answers Universal Device Inquiry as a Matrix-1000 or Matrix-6 on IAC ports,  
**so that** Matrix-Control can detect a device, unlock the UI, and show the correct footer identity / device-type gating.

---

## Placement (ask Guillaume)

Pick one when creating the story:

1. **Preferred:** New **dev-tooling / utility** story (not Epic U layout). Suggested key shape: `T-1-matrix-device-inquiry-simulator` or similar Tools track — keep out of product epic burn-down if possible.  
2. **Alternative:** Epic 8 companion tooling story after 8.3 (e.g. `8-x-…`) if Guillaume wants it on the Synth Connection board.  
3. **Do not** fold into 8.4 (virtual instrument bus) or invent Matrix-6R inquiry bytes (blocked by 8.1 / PRD §9 #6).

Update `sprint-status.yaml` only after story file + key are agreed.

---

## Acceptance criteria (draft)

1. **Given** macOS IAC Bus enabled and simulator listening on the configured input port  
   **When** Matrix-Control sends Universal Device Inquiry (`F0 7E 7F 06 01 F7`) to the simulator’s input  
   **Then** the simulator replies on its output with a valid Device ID response that Matrix-Control accepts.

2. **And** the operator can select at least two profiles before/while running:
   - **Matrix-1000** — member `0x02 0x00` (see `SysExConstants::DeviceInquiry`)
   - **Matrix-6** — provisional member `0x01 0x00` → `DeviceTypeRegistry` → `kMatrix6`

3. **And** firmware / version bytes in the reply are configurable or fixed documented defaults (enough for footer `deviceVersion` display).

4. **And** a short README (or in-app help) documents the Audio MIDI Setup + Matrix-Control port mapping:
   - Matrix-Control **MIDI To** → simulator **In** (IAC)
   - Matrix-Control **MIDI From** ← simulator **Out** (IAC)

5. **And** Matrix-Control Standalone with From/To pointed at those ports: inquiry success → `deviceDetected=true`, footer identity shows type/version, UI unlocks (smoke with current 8.2/8.3 behaviour).

6. **And** out of scope for this story:
   - Full patch/master dump emulation
   - Remote Parameter Edit / Program Change behaviour beyond ignore-or-log
   - Distinct **Matrix-6R** inquiry member pattern (unknown on hardware; do **not** invent — 8.1)
   - Windows/Linux virtual MIDI (macOS IAC first; other OS later optional)

7. **And** tool builds as a **separate** CMake target (console or minimal Standalone) — must **not** ship inside the VST3/AU plugin binary; Core SysEx constants may be shared/read-only reused.

---

## Technical anchors (do not re-derive blindly)

### Inquiry request (Matrix-Control → simulator)

```text
F0 7E 7F 06 01 F7
```

Source: `SysExConstants::DeviceInquiry::kRequestMessage`

### Expected reply shape (simulator → Matrix-Control)

Documented in `SysExConstants.h` / decoder path:

```text
F0 7E <chan> 06 02 10 06 00 <memb-lo> <memb-hi> <rev0> <rev1> <rev2> <rev3> F7
```

| Field | M-1000 | M-6 (provisional) |
|-------|--------|-------------------|
| Manufacturer | `0x10` | `0x10` |
| Family | `0x06 0x00` | `0x06 0x00` |
| Member | `0x02 0x00` | `0x01 0x00` |

Registry: `DeviceTypeRegistry::fromDeviceInquiry` — wrong manuf/family → unknown; M-6R **not** distinguishable from inquiry today.

### Integration with product behaviour

- Unlock path: `MidiManager::performDeviceInquiry` / `refreshDeviceInquiryAfterPortSync` (8.2)
- UI lock: FR-2 / story 8.3 — needs successful detection
- Bank Utility gray when detected M-6: story 8.5 — needs `deviceType` from inquiry

---

## Suggested implementation shape (guidance only — create-story may refine)

| Piece | Suggestion |
|-------|------------|
| Target | e.g. `Matrix-Device-Simulator` JUCE console or tiny Standalone under `Tools/` or `Apps/` |
| MIDI | JUCE `MidiInput` / `MidiOutput` on selected devices; reply on message thread or dedicated MIDI thread (KISS) |
| UI | Minimal: device profile combo, MIDI In/Out selectors, status log (“Inquiry received → replied as M-1000”) |
| SSOT | Prefer including / linking the same `SysExConstants` (or a tiny shared header copy with comment “keep in sync”) — avoid drifting member bytes |
| Tests | Unit-test reply encoder bytes for both profiles; no live IAC required in CI |

---

## Manual UAT checklist (for story Dev Notes)

1. Enable IAC Driver; create/use one bus (or two ports if split In/Out naming).  
2. Launch simulator → profile Matrix-1000 → open ports.  
3. Launch Matrix-Control Standalone → MIDI To = sim In, MIDI From = sim Out.  
4. Expect: unlock + footer identity Matrix-1000 + version.  
5. Switch simulator to Matrix-6 → re-trigger inquiry (reselect port or restart inquiry path).  
6. Expect: `deviceType` Matrix-6 behaviour (e.g. Bank Utility gray per 8.5).  
7. Quit simulator / clear ports → Matrix-Control returns to locked / `No device`.

---

## Risks / open points for create-story

| Risk | Note |
|------|------|
| IAC loopback / same-port confusion | Document distinct From vs To; warn against selecting the same endpoint for both if the OS merges them oddly |
| Provisional M-6 bytes | Label UI “Matrix-6 (provisional inquiry)” until hardware capture |
| 6R request creep | Guillaume may ask for a third profile — push back unless hardware bytes known; optional APVTS-only override is product-side, not simulator invention |
| Scope creep to tier 2 | Keep AC hard on inquiry-only |

---

## Prior conversation decisions (binding)

- Guillaume approved **tier 1** (inquiry responder), not tier 2/3.  
- Motivation: travel without Matrix-1000; no Matrix-6/6R on desk.  
- Virtual MIDI via Mac ports is the intended wiring.  
- Story 8.3 UI lock UAT on real M-1000 already OK; this tool is **follow-on tooling**, not a blocker for 8.3 review close.

---

## Done when (create-story output)

- Story markdown under `_bmad-output/implementation-artifacts/` with full AC, tasks, Dev Notes, Code Map  
- Status `ready-for-dev`  
- Sprint key registered in `sprint-status.yaml` (placement agreed)  
- No implementation in the create-story session unless Guillaume asks
