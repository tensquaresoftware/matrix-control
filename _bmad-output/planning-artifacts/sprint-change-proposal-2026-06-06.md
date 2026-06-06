# Sprint Change Proposal — Audio/MIDI Architecture Revision

**Project:** Matrix-Control  
**Date:** 2026-06-06  
**Author:** Correct Course workflow (BMad)  
**Trigger:** External architecture review — `matrix-control-audio-midi-architecture.md` (Claude design session, June 2026)  
**Status:** Approved (2026-06-06, Guillaume)

---

## Section 1: Issue Summary

### Problem Statement

During Epic 2 implementation, Matrix-Control adopted **Decision D-055** and Story **2.7**, declaring an active stereo audio input bus (`"Audio From"`) in hosted plugin mode. This was intended to replicate Ableton Live's External Instrument single-track workflow (MIDI + synth audio return on one instrument track).

An independent architecture review (June 2026) demonstrates that this goal is **structurally impossible** for third-party VST3/AU plugins:

- Plugins cannot enumerate or open physical audio hardware; they only receive host-allocated audio buses in `processBlock()`.
- Routing hardware input to a plugin instrument sidechain is **not uniform across DAWs** — Ableton lists *tracks*, not hardware inputs, on instrument sidechains.
- The implemented approach (Option A) still requires **two DAW tracks** in Ableton, but with **more complex inter-track routing** and **counter-intuitive UX** (synth audio exiting the instrument track).

The correct cross-DAW pattern is **Option D**: instrument plugin on a MIDI/instrument track (MIDI only) + separate audio track monitoring the synth hardware input — managed natively by the DAW.

### Discovery Context

- **When:** 2026-06-06, design session with Claude reviewing BMad artifacts and current codebase.
- **Triggering stories:** **2.7** (Audio Passthrough and Peak Indicator), **2.9b** (Header Routing Controls), **2.11** (Header Panel Layout) — all built on D-055 assumptions.
- **Evidence:**
  - VST3/AU API constraints (no hardware audio access in plugin mode).
  - Ableton Live does not support third-party MIDI effect plugins (Option C also ruled out).
  - Current `PluginProcessor` declares `.withInput("Audio From", stereo, true)` in all build targets.
  - PRD user journey (Alex) and FR-4/FR-8 describe hosted plugin audio passthrough as primary workflow.

### Secondary Finding (No Immediate Change)

Notes/CC forwarded via `MidiOutboundQueue` (AD-3) lose sample-accurate timestamps. Jitter (~1–5 ms) is acceptable for patch editing on Matrix-1000 via MIDI DIN. **Recommendation: retain AD-3 (R-D)** unless real-time keyboard play testing proves unsatisfactory.

---

## Section 2: Impact Analysis

### Epic Impact

| Epic | Status | Impact |
|------|--------|--------|
| **E0** | done | None |
| **E1** | done | None |
| **E2** | in-progress | **Moderate revision.** Stories 2.7, 2.9b, 2.11 ACs reference hosted audio passthrough. New stories R-1–R-5 required. Story 2.10 unaffected. |
| **E3–E7** | backlog | Low direct impact. R-4 (MPSC audit) should complete before E3. |
| **E8** | backlog | **Story 8.4 scope changes.** Mono/stereo input bus per device type applies to **standalone only** in hosted mode; plugin has output-only bus. Partial overlap with R-1 — merge or sequence. |
| **E9–E10** | backlog | None |

### Story Impact

| Story | Status | Required Action |
|-------|--------|-----------------|
| 2.7 | done | **Revise AC retroactively** — scope narrows to standalone passthrough; plugin mode = silent output only |
| 2.9b | done | **Revise AC** — Audio From / Input Gain / peak visible **standalone only** |
| 2.11 | done | **Revise AC** — header layout: AUDIO FROM packet hidden in plugin mode |
| 2.10 | backlog | No change — proceed after R-1 |
| 8.4 | backlog | **Revise AC** — bus layout: output stereo always; input bus conditional on standalone + device type |

### Artifact Conflicts

| Artifact | Section | Conflict | Action |
|----------|---------|----------|--------|
| **PRD** | Vision (L35), UJ Alex (L63), FR-4 (L143–147), FR-8 (L177), Header (L87) | "One DAW track replaces External Instrument"; hosted audio input bus | Update to Option D two-track model |
| **Decision log** | D-055 | Plugin = host audio input bus for single-track workflow | **Supersede** with D-055-R (standalone-only hardware input) |
| **Architecture** | AD-3 | Intact — queue for instrument path retained | Add footnote on optional future R-C path |
| **Architecture** | §4.2, E8 mapping | `AudioPassthroughProcessor` in plugin mode | **Add AD-11** — plugin: no input bus; standalone: input + passthrough |
| **Epics** | E2 description, FR-8 mapping | Audio passthrough as cross-mode feature | Reframe FR-8: standalone passthrough; plugin silent output |
| **Epics** | E8 / 8.4 | Mono/stereo input bus in hosted plugin | Restrict to standalone |
| **UX** | UX-DR3 (header routing) | Audio From in header always visible | Conditional visibility per build context |
| **Tests** | `AudioPassthroughProcessorTests` | May assume plugin input bus | Update for dual-mode behaviour |
| **deferred-work.md** | 2.7 items | MSVC weak symbols, standalone combo refresh | Retain; R-1 may resolve some items |

### Technical Impact (Code)

| Component | Change |
|-----------|--------|
| `PluginProcessor` | Conditional `BusesProperties()` — no input bus when hosted |
| `AudioPassthroughProcessor` | `process()` no-op / silence in plugin mode |
| `HeaderPanel` / `PluginEditor` | Hide Audio From, Input Gain, PeakIndicator in plugin mode |
| `StandaloneAudioInputRouter*` | Unchanged — remains standalone path |
| `InstrumentMidiForwarder` | Unchanged (R-D — keep queue) |
| `MidiOutboundQueue` | R-4 audit before E3 |

**Effort estimate:** R-1 = **Low–Medium** (isolated components, tests to update). Full Epic R = **Medium**.

**Risk:** Low technical risk; moderate documentation/UX risk if DAW setup guides are delayed.

---

## Section 3: Recommended Approach

### Selected Path: **Hybrid — Direct Adjustment + New Epic (Option 1 + partial Option 2)**

Neither full rollback nor MVP reduction is required. The MIDI architecture (AD-3, thread model, Editor/Instrument split) is sound and should be preserved.

| Option | Viable? | Rationale |
|--------|---------|-----------|
| **1 — Direct Adjustment** | ✅ Primary | Add Epic R stories; revise completed story ACs; update PRD/Architecture |
| **2 — Rollback** | ❌ Not viable | Reverting 2.7/2.9b/2.11 wholesale wastes good standalone work; surgical R-1 is cheaper |
| **3 — MVP Review** | ❌ Not viable | MVP goals unchanged — only the *hosted audio routing model* is corrected |

### Decision Summary

| Decision | Choice | ID |
|----------|--------|-----|
| Hosted plugin audio input bus | **Remove** (Option R-A) | AD-11 |
| Standalone audio input | **Keep** | AD-11 |
| Notes/CC send path | **Keep queue** (Option R-D) | AD-3 unchanged |
| Hardware Latency GUI | **Add** (new story R-2) | New FR or FR-8 extension |
| MIDI port open errors | **Add GUI feedback** (R-3) | New story |
| Queue MPSC audit | **Before E3** (R-4) | Technical debt closure |

### Timeline Impact

- **+1 epic slice** (Epic R: 5 stories) inserted between E2 completion and E3 start.
- E2.10 can proceed in parallel with R-1 if file conflicts avoided (R-1 touches `PluginProcessor`, `HeaderPanel` — coordinate sequencing).
- **Recommended order:** R-1 → R-4 → 2.10 → R-2 → R-3 → R-5 → E3.

---

## Section 4: Detailed Change Proposals

### 4.1 PRD Changes

#### PRD — Vision (§1)

**OLD:**
> One DAW track replaces the Ableton Live External Instrument + separate editor workflow…

**NEW:**
> One **instrument track** hosts Matrix-Control for MIDI editing and performance (notes/CC when armed, SysEx always-on). A **separate audio track** monitors the synth hardware return — the standard cross-DAW pattern for external instruments. Standalone mode provides integrated audio input selection via `AudioDeviceManager`.

**Rationale:** Honest UX aligned with VST3/AU constraints.

---

#### PRD — User Journey Alex (§2)

**OLD:**
> …route the synth audio output into the plugin input bus, and arm the track. … audio returns through the plugin.

**NEW:**
> …create an instrument track with Matrix-Control, configure **MIDI From/To**, and arm the track. They add an **audio track** monitoring their interface input connected to the Matrix-1000 output. Device Inquiry succeeds; UI unlocks. They play a clip — notes reach the synth; audio is heard on the audio track. They disarm the instrument track and tweak envelope segments — SysEx still flows.

**Rationale:** Reflects Option D workflow; matches Ableton/Logic/Cubase conventions.

---

#### PRD — FR-4: Virtual Instrument Plugin Category

**OLD:**
> …with MIDI input, MIDI output, audio input (Audio From), and stereo audio output.
> **Audio input bus layout:** after Device Inquiry, mono for M-1000 / stereo for M-6/6R…

**NEW:**
> …with MIDI input, MIDI output, and stereo audio output.
> **Hosted plugin:** no audio input bus. Synth audio return is managed on a **separate DAW audio track**.
> **Standalone:** audio input via `AudioDeviceManager`; after Device Inquiry, mono channel selection for M-1000, stereo for M-6/6R (FR-46 device-type rules apply to channel layout guidance).

**Rationale:** AD-11; removes impossible hosted hardware routing promise.

---

#### PRD — FR-8: Audio Passthrough

**OLD:**
> Synth audio routed to the plugin input bus (hosted) or selected physical input (standalone) passes to the plugin output…

**NEW:**
> **Standalone:** synth audio from the selected physical input passes to the plugin output with user-adjustable Input Gain and peak level indicator.
> **Hosted plugin:** audio output bus is silent (or optional future MIDI-activity monitor only — not synth audio). Input Gain and peak indicator are **standalone-only** UI controls.

**Rationale:** Eliminates confusion; preserves standalone monitoring value.

---

#### PRD — Header Controls (§3)

**OLD:**
> **Audio From** — Standalone: physical interface input via device manager. Plugin: host-routed input bus…

**NEW:**
> **Audio From** — **Standalone only:** physical interface input via `AudioDeviceManager` and channel ComboBox. **Hidden in hosted plugin mode.**

**Rationale:** Matches conditional UI in R-1.

---

#### PRD — New Requirement: FR-4b Hardware Latency (proposed)

> User can set **Hardware Latency** (ms, 0–100, 0.1 ms precision) representing analog round-trip delay (DAW → MIDI → synth → ADC → audio track). Value reported to host via `AudioProcessor::reportLatency()` and persisted in `apvts.state`.

**Rationale:** External Instrument parity for recording alignment; §4.4 of architecture report.

---

### 4.2 Architecture Changes

#### New AD-11 — Audio Bus Layout by Host Context

```markdown
### AD-11 — Audio bus layout by host context

| Context | Input bus | Output bus | AudioPassthroughProcessor |
|---------|-----------|------------|---------------------------|
| Hosted plugin (VST3/AU) | **None** | Stereo (silent) | No-op; output cleared |
| Standalone | Stereo (optional enable) | Stereo | Passthrough + gain + peak |

Detection: `juce::JUCEApplicationBase::isStandaloneApp()` or equivalent build flag.

Mono vs stereo **input channel selection** (M-1000 vs M-6/6R) applies to **standalone** only (see DeviceTypeRegistry, Epic 8).

**Supersedes:** D-055 hosted-plugin input bus rationale. D-055 retained for standalone path only.
```

---

#### AD-3 Footnote Addition

**ADD after "Never call `MidiOutput` from audio thread":**

> **Future consideration (R-C):** Direct `sendMessageNow()` from `processBlock()` for notes/CC may improve sample-accurate timing. Requires explicit AD revision and hardware play-testing. Current v1 retains queue path (R-D).

---

### 4.3 Epic Changes

#### New Epic R: Architectural Revision (Audio/MIDI Host Model)

Insert after Epic 2, before Epic 3.

**Goal:** Align hosted plugin audio/MIDI model with VST3/AU constraints; add missing robustness features.

| Story | Title | Priority | Depends On |
|-------|-------|----------|------------|
| **R-1** | Remove plugin audio input bus (Option R-A) | 🔴 High | 2.11 done |
| **R-2** | Hardware Latency GUI + `reportLatency()` | 🔴 High | R-1 (optional parallel) |
| **R-3** | MIDI port open error → footer/GUI feedback | 🟡 Medium | — |
| **R-4** | MidiOutboundQueue MPSC audit + stress test | 🟡 Medium | 2.9 done |
| **R-5** | Windows MIDI multi-client documentation | 🟢 Low | R-3 |

---

#### Story R-1 — Full Specification

**As a** Ableton/Logic/Cubase user,  
**I want** Matrix-Control to behave as a standard MIDI instrument on an instrument track without routing synth audio through the plugin,  
**So that** my DAW routing is natural and unambiguous.

**Acceptance Criteria:**

1. `BusesProperties` declares **no input bus** in hosted plugin builds.
2. Standalone retains input bus + `StandaloneAudioInputRouter` functionality.
3. `AudioPassthroughProcessor::process()` in plugin mode: silent stereo output; no input bus read.
4. `HeaderPanel`: Audio From combo, Input Gain slider, PeakIndicator **hidden** in plugin mode; visible in standalone.
5. Existing Activity LEDs (Story 2.8) remain primary MIDI feedback in plugin mode.
6. All unit tests updated; VST3 + Standalone builds clean.
7. Story 2.7, 2.9b, 2.11 ACs annotated as revised per this story.

---

#### Story R-2 — Full Specification

**As a** user recording Matrix-1000 audio in my DAW,  
**I want** to set Hardware Latency (ms) in Matrix-Control,  
**So that** my DAW can compensate for analog round-trip delay.

**Acceptance Criteria:**

1. Numeric control in Settings or footer zone: 0–100 ms, 0.1 ms step.
2. Value converted to samples and exposed via `setLatencySamples()` / `reportLatency()`.
3. Persisted in `apvts.state`.
4. User docs: typical values table (USB interface + Matrix-1000 DAC/ADC).

---

#### Story R-3 — Full Specification

**As a** Windows user sharing MIDI ports with my DAW,  
**I want** clear GUI feedback when port open fails,  
**So that** I can diagnose exclusive-access conflicts.

**Acceptance Criteria:**

1. `MidiInputPort` / `MidiOutputPort` open failures propagate to footer `uiMessageText`.
2. Message names the port and suggests likely cause (Windows exclusive access).
3. `MidiLogger` records error code.
4. Unit test: mock port failure → footer message asserted.

---

#### Story R-4 — Full Specification

**As a** developer,  
**I want** documented MPSC contract on `MidiOutboundQueue`,  
**So that** concurrent producers (audio thread + message thread) are safe.

**Acceptance Criteria:**

1. Code comment in `MidiOutboundQueue.h` documents producer/consumer contract.
2. Verify or fix: two producers + one consumer without data races.
3. Basic stress test: dual producer threads + consumer.

---

#### Story R-5 — Full Specification

**As a** Windows user,  
**I want** setup documentation for MIDI port conflicts,  
**So that** I can use loopMIDI or rtpMIDI when winMM blocks shared access.

**Acceptance Criteria:**

1. `Documentation/` guide: Windows MIDI multi-client limitations.
2. Recommends loopMIDI 1.0.16+ (or verified alternative).
3. Links from in-app error message (R-3).

---

#### Revised Story 8.4 AC

**OLD:**
> …mono input for M-1000 and stereo for M-6/6R; bus updates on type change

**NEW:**
> **Hosted plugin:** VST3 Instrument / AU Music Device with **stereo output only** (no input bus).
> **Standalone:** after Device Inquiry, input channel layout guidance follows device type (mono M-1000, stereo M-6/6R). M-6/6R grays MASTER EDIT (FR-46).

---

### 4.4 Decision Log Entry

#### D-055-R — Supersedes D-055 (partial)

**Decision:** REVISE — Audio From dual-path  
**OLD D-055:** Plugin = host audio input bus for single-track External Instrument parity.  
**NEW D-055-R:**
- **Standalone:** `AudioDeviceManager` + channel ComboBox + passthrough (unchanged).
- **Hosted plugin:** **No audio input bus.** User creates separate DAW audio track for synth return.
- **Rationale:** VST3/AU APIs prohibit hardware audio access; Ableton sidechain routing still requires two tracks with worse UX than Option D.

---

## Section 5: Implementation Handoff

### Scope Classification: **Moderate**

Requires backlog reorganization (new Epic R), PRD/Architecture updates, and targeted code revision — not a full replan.

### Handoff Recipients

| Role | Agent / Skill | Responsibility |
|------|---------------|----------------|
| **Architect** | `bmad-create-architecture` | Integrate AD-11, AD-3 footnote; update component diagram §4.2 |
| **PM** | `bmad-prd` (update) | Apply PRD edits §4.1; add FR-4b |
| **PO** | `bmad-create-epics-and-stories` | Add Epic R; revise 2.7/2.9b/2.11/8.4 ACs; update FR coverage table |
| **Scrum** | Manual / `bmad-sprint-planning` | Update `sprint-status.yaml` with Epic R entries |
| **Dev** | `bmad-create-story` → `bmad-dev-story` | Implement R-1 first |
| **Tech Writer** | `bmad-agent-tech-writer` | DAW setup guides + Windows MIDI guide (R-5); FAQ §7 of architecture report |
| **UX** | `bmad-ux` (optional) | Header layout spec: conditional Audio From packet |

### Success Criteria

1. Hosted VST3/AU build declares **zero input channels**.
2. Ableton Live 12: instrument track + separate audio track workflow documented and smoke-tested.
3. Standalone audio passthrough unchanged from current behaviour.
4. PRD, Architecture, Epics, and sprint-status aligned with AD-11.
5. No regression in MIDI Editor/Instrument path separation (AD-3).

### Sequencing (Post-Approval)

```
1. bmad-create-architecture  → AD-11
2. bmad-prd (update)         → FR-4, FR-8, vision, FR-4b
3. bmad-create-epics-and-stories → Epic R + AC revisions
4. sprint-status.yaml update
5. bmad-create-story R-1
6. bmad-dev-story R-1
7. bmad-create-story R-4 (parallel eligible after R-1 merge)
8. bmad-dev-story R-2, R-3, R-5
9. Resume E2.10 → E3
```

---

## Checklist Completion Summary

| Section | Status |
|---------|--------|
| §1 Trigger & Context | [x] Done — Stories 2.7/2.9b; D-055 misunderstanding |
| §2 Epic Impact | [x] Done — E2 revision + Epic R; E8.4 scope change |
| §3 Artifact Conflicts | [x] Done — PRD, Architecture, Epics, UX |
| §4 Path Forward | [x] Done — Hybrid Option 1 selected |
| §5 Proposal Components | [x] Done — this document |
| §6 Final Review | [x] Done — approved 2026-06-06 |
| §6.4 sprint-status | [x] Done — Epic R added 2026-06-06 |

---

## Approval

**User decision required:**

- [x] **Approve** — proceed with handoff sequence above (2026-06-06)
- [ ] **Revise** — specify adjustments
- [ ] **Reject** — retain current D-055 / Option A model (not recommended)

---

*Generated by bmad-correct-course workflow — Matrix-Control — 2026-06-06*
