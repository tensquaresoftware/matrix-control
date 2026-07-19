# Epic 8 Context: Synth Connection & Instrument Role

<!-- Compiled from planning artifacts. Edit freely. Regenerate with compile-epic-context if planning docs change. -->

## Goal

Establish reliable synth detection and device-aware product behaviour so users always know which Matrix unit is connected, cannot edit blindly when nothing is detected, and can host Matrix-Control as a standard virtual instrument in major DAWs. This epic wires Universal Device Inquiry to APVTS state and the footer, gates all editing when disconnected, registers correct plugin bus layouts per host context, and adapts the UI and patch-memory semantics for Matrix-6/6R PATCH-only hardware.

## Stories

- Story 8.1: DeviceTypeRegistry and Member Byte Fix
- Story 8.2: Device Inquiry and Footer Identity
- Story 8.3: UI Lock Without Synth
- Story 8.4: Virtual Instrument Registration and Bus Layout
- Story 8.5: Matrix-6/6R Patch Memory Limits

## Requirements & Constraints

- On MIDI port configure (and reconfigure), run Universal Device Inquiry; parse Device ID reply for Matrix-1000, Matrix-6, or Matrix-6R.
- Publish detection state via APVTS: `deviceDetected`, `deviceType`, `deviceVersion`. Inquiry timeout or incompatible reply sets `deviceDetected=false`.
- Footer right zone shows device type and firmware when detected; placeholder or hidden when not.
- When `deviceDetected=false`, disable PATCH EDIT, MASTER EDIT, MATRIX MODULATION, PATCH MANAGER, and all module I/C/P actions. Footer shows actionable connection guidance. Send no SysEx and no Program Change while locked.
- Register as a virtual instrument (AU Music Device, VST3 Instrument) with MIDI in/out and stereo audio out. Must instantiate in Live, Reason, and GarageBand without MIDI-effect workarounds.
- **Hosted plugin:** stereo output only — no audio input bus. Synth audio is monitored on a separate DAW audio track.
- **Standalone:** physical audio input via `AudioDeviceManager`; after detection, mono input guidance for Matrix-1000, stereo for Matrix-6/6R.
- Matrix-6/6R: gray entire MASTER EDIT section; PATCH, Matrix Mod, and Patch Manager remain active with Matrix-1000-compatible PATCH SysEx.
- Matrix-6/6R patch memory: 100 patches (00–99), cyclic navigation only, no bank utility, no Set Bank SysEx; ROM gating applies to Matrix-1000 only. Matrix-1000 bank/patch behaviour must stay unchanged.
- Device Inquiry is sync RPC on the MIDI thread only — never block the audio thread for hardware wait.
- M-6 Device ID member bytes may still need hardware verification; record findings if bytes remain uncertain.

## Technical Decisions

- **`DeviceTypeRegistry`** (Core): maps Inquiry member bytes → `deviceType`; also drives standalone input-channel layout (mono M-1000, stereo M-6/6R). Fix known member-byte order mismatch in `SysExConstants` during implementation.
- **`MidiManager`** owns port configure and Inquiry orchestration; results land in APVTS properties consumed by GUI and services.
- **AD-11 — audio bus layout by host context:** detect standalone vs hosted (`JUCEApplicationBase::isStandaloneApp()` or equivalent). Hosted: `BusesProperties` declares output only; `AudioPassthroughProcessor` clears silent stereo output. Standalone: stereo input (optional enable) + passthrough, gain, and peak — unchanged path via `StandaloneAudioInputRouter`.
- **D-055 superseded for hosted mode:** prior single-track plugin audio-input rationale is standalone-only after sprint change 2026-06-06.
- **Matrix-6/6R limits:** add `Matrix6Or6RLimits` (or equivalent) and resolve bounds through `DeviceMemoryLimits` / registry helper vs existing `Matrix1000Limits`.
- **UI lock enforcement:** gate at action dispatch and/or outbound MIDI producers so locked state cannot emit editor traffic.
- **Story R-1 is a prerequisite for 8.4:** plugin input bus removal must land before final instrument bus registration is considered complete.

## UX & Interaction Patterns

- Footer right zone = device identity (type + firmware); footer left/center = guidance and errors when disconnected.
- Locked UI: grayed controls with footer hints (cables, ports, power-cycle) — no modals for lock state.
- Matrix-6/6R: MASTER EDIT visibly disabled; BANK UTILITY grayed; Internal Patches navigates 00–99 with wrap only.
- Hosted DAW workflow: one instrument track for Matrix-Control (MIDI + always-on SysEx editing); separate audio track for synth return — do not route hardware audio through the plugin in hosted mode.
- Standalone: Audio From channel selector updates when device type changes after port reconfigure.

## Cross-Story Dependencies

- **Within epic:** 8.1 → 8.2 → 8.3 (detection chain). 8.1 + **Story R-1** → 8.4 (bus layout). 8.1–8.2 → 8.5 (device-type-aware memory limits).
- **Upstream:** Epic 2 MIDI stack (`MidiManager`, queue, header routing). Story R-1 (remove hosted plugin input bus).
- **Downstream:** Epics relying on `deviceDetected` / `deviceType` (patch manager bank semantics, ROM gating, SysEx delay profile selection, graying rules) assume this epic's APVTS contract is stable.
