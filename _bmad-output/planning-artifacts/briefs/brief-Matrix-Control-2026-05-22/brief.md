---
title: "Product Brief: Matrix-Control"
status: ready
created: 2026-05-22
updated: 2026-05-23
baseline_tag: v0.0.66-alpha-pre-bmad
language: en
target_release: "Christmas 2026 (aspirational)"
version: "0.3"
---

# Product Brief: Matrix-Control

## Executive Summary

Matrix-Control is a cross-platform JUCE **virtual instrument** and standalone application that gives Oberheim Matrix-1000 owners full, intuitive, real-time control over every synthesis parameter of their instrument — directly from any major DAW or as a standalone editor.

It combines **two roles in one plugin**: a **hardware instrument** (MIDI notes and CC in, audio from the synth out) and an **always-on bidirectional MIDI editor** (SysEx patch and master editing independent of track arming). One DAW track replaces the Live External Instrument + separate editor workflow.

Born from a well-received Max for Live editor (Matrix-1000 Editor, 2020) and a complete JUCE rewrite started in 2025, Matrix-Control addresses a persistent gap: the Matrix-1000 is one of the most capable analog polysynths of the 1980s, yet it ships without a front-panel editor. Existing hardware and software alternatives fall short on ergonomics, completeness, or reliability — especially under long programming sessions.

The product promise is twofold:

1. **For musicians:** A professional MIDI editor that makes users say *"Wow — I finally understand what this synth can do."* Complete PATCH and MASTER editing, an original PATCH MANAGER (internal memory, computer library, Patch Mutator), DAW automation with SysEx rate limiting, and a pixel-crafted UI with interactive envelope and track displays.

2. **For developers:** An exemplary open-source JUCE reference repository — Clean Code, Clean Architecture, SOLID, tests, and thorough public documentation in English.

The non-negotiable success criterion is **rock-solid SysEx reliability**: hours of uninterrupted editing without the Matrix-1000 hanging or crashing because the editor flooded it with MIDI. Everything else serves that trust.

Matrix-Control v1 will be **free (MIT)** with optional GitHub Sponsors support. Full Matrix-6/6R compatibility (including SPLIT mode) is planned for v2.

---

## The Problem

### The synth nobody could program

The Oberheim Matrix-1000 shares the synthesis engine of the legendary Matrix-6 and Matrix-6R, but unlike its siblings it has no knobs — only buttons and a small display. Owners often treat it as a preset box with 1,000 factory sounds, unaware of its deep programming potential.

Programming requires either a Matrix-6/6R as a MIDI surrogate, a dedicated hardware programmer, or third-party software. None of the options Guillaume tested deliver a complete, ergonomic, *"go-to editor"* experience:

| Alternative | Limitation |
|---|---|
| ObieEditor, Patch Base, LinuxTech, Ctrlr, MidiQuest, Matrix-6G | UI poorly designed; incomplete workflows |
| Stereoping (hardware) | Physical controller; not a full software editor |
| Matrix-6/6R front panel | Usable but cumbersome for deep editing |

### The Max for Live editor proved demand — and exposed limits

Guillaume's Matrix-1000 Editor (released December 2020) validated the need:

- Community rating **5/5** on maxforlive.com
- Featured by **Cycling '74** (Jeremy Bernstein tutorial) and **AudioFanzine**
- Repeated praise for UI clarity and user documentation

But platform constraints and maintenance debt accumulated:

- Ableton Live only; Live/Max version fragility
- Patch save-to-disk broken after Max 9; duplicate/spam SysEx messages
- Automation and MIDI controller mapping highly requested but fragile
- MIDI routing confusion (OUT vs THRU) caused support burden
- Two-track workaround (MIDI editor + audio track or External Instrument) added friction
- Owner could no longer sustain quality alone while working full-time

Users publicly asked for a **VST/AU rewrite**. The community is waiting.

### The vibe-coding trap

The JUCE rewrite (summer 2025) made rapid progress with AI-assisted development, but without structured product/engineering method:

- UI scaling produced blur, clipping, performance issues
- Code and documentation grew faster than coherence
- Existing specs became outdated or unreliable

BMad Method is now the guardrail to consolidate vision, documentation, architecture, and delivery — validated first on a separate mobile project (Sparkay).

---

## The Solution

Matrix-Control is a **native JUCE 8 virtual instrument** (AU, VST3) and **standalone app** for macOS, Windows, and Linux — a dedicated software remote and audio gateway for the Matrix-1000.

### Core experience

- **Bidirectional PATCH editing** — 10 synthesis modules (96 parameters), **fully interactive** envelope and track displays (slider ↔ curve, mouse drag on curve), PATCH NAME editor
- **MATRIX MODULATION** — 10 buses × Source / Amount / Destination
- **MASTER EDIT** — MIDI, Vibrato, Misc global settings (22 parameters)
- **PATCH MANAGER** — original section not present on the hardware:
  - *Bank Utility* — direct bank select (0–9), bank lock
  - *Internal Patches* — navigate 100/1000 patches, INIT / COPY / PASTE / STORE
  - *Computer Patches* — open folder of `.syx` files, save/save-as, filename vs internal name reconciliation
  - *Patch Mutator* — randomize/mutate patches (Absynth-inspired); final spec via dedicated brainstorming session
- **Workflow bonuses** — per-module Init / Copy / Paste (type-aware clipboard), UI scale (50–200%), Black/Cream themes, footer message bar

### Dual role: Instrument + MIDI Editor

Matrix-Control solves the two simultaneous user needs in **one DAW track**:

| Need | Role | How |
|---|---|---|
| Play the Matrix-1000 from clips or a master keyboard | **Instrument** | Host MIDI (notes, CC, pitch bend) → forwarded to **MIDI To**; obeys track arming like any virtual instrument |
| Edit patches bidirectionally all session long | **Editor** | **MIDI From** listens permanently on a dedicated thread; SysEx I/O independent of track arming |

**Header routing (Ableton-inspired labels):**

| Control | Plugin mode | Standalone mode |
|---|---|---|
| **MIDI From** | Synth return port (SysEx in only) | Synth return port (SysEx in only) |
| **MIDI To** | Port to Matrix-1000 MIDI In | Port to Matrix-1000 MIDI In |
| **Keyboard From** | Visible, grayed, displays **Host** | Master keyboard port (notes + CC, e.g. mod wheel for LEVERS) |
| **Audio From** | Physical input carrying synth audio | Same |
| **Input Gain** + level meter | Optional gain trim near Audio From | Same |

**MIDI message routing rules:**

| Path | Messages | Mechanism |
|---|---|---|
| Instrument → synth | Note On/Off, CC, Pitch Bend | `processBlock()` (plugin) or **Keyboard From** (standalone) → **MIDI To** |
| Instrument filter | Program Change, SysEx | **Stripped** from instrument path — no conflict with editor |
| Editor → synth | SysEx, Program Change (patch select) | **Dedicated MIDI thread** → **MIDI To** |
| Editor ← synth | SysEx only | **Dedicated MIDI thread** ← **MIDI From** (no inbound PC) |

All editor SysEx and outbound patch management traffic uses the **dedicated MIDI thread**, never the real-time audio thread. Instrument notes and CC may pass through `processBlock()` when hosted as a plugin.

A **unified outbound queue** merges instrument and editor traffic to **MIDI To**, with notes/CC prioritized and SysEx rate-limited.

**Activity indicators:** two header LEDs — **Instrument** (notes/CC) and **Editor** (SysEx).

**Multi-instance:** each plugin instance owns its own port triple; user manual recommends distinct MIDI From/To pairs per Matrix-1000 (e.g. interface In1/Out1 for unit #1, In2/Out2 for unit #2).

Cross-platform builds use the owner's **JUCE Project Generator** toolchain (macOS Universal/ARM/Intel, Windows VST3, Linux VST3).

### Device detection and degraded operation

At startup (and on MIDI port change), the plugin performs **Device Inquiry** to confirm a compatible Oberheim unit. If no valid device responds, **the entire UI is locked** and the footer displays actionable guidance (check cables, ports, power-cycle or reset the synth).

On partial MIDI failure, the footer shows **context-specific degraded-mode warnings** (inbound SysEx loss vs outbound send failure). No automatic retry — user re-edits when connection restores.

### Documentation as product

A complete **user manual** (English & French) and the public repo documentation are part of v1 — not an afterthought. The M4L editor's manual was repeatedly cited as a differentiator; Matrix-Control will meet or exceed that standard.

---

## What Makes This Different

| Differentiator | Honest assessment |
|---|---|
| **Instrument + editor in one track** | Replaces Live External Instrument + separate editor; SysEx automation overlays notes in one MIDI clip |
| **UI craftsmanship** | Pixel-designed Figma mockup; ÷4 grid for clean UI scaling; interactive displays |
| **PATCH MANAGER** | No competitor offers this integrated workflow (internal + computer + mutator) |
| **SysEx reliability** | Rate-limited queue, connection diagnostics — prevents "editor useless if synth hangs" |
| **Cross-platform native plugin** | Escapes M4L/Live lock-in; same feature set in standalone |
| **Open-source reference repo** | Clean Code, Clean Architecture with rich docs & tests — secondary audience: JUCE learners |
| **Free + sponsors** | Low barrier for a niche hardware community; sustainability via recognition |

What this is **not**: a technical moat or patented protocol. The advantage is **execution quality** and **trust earned in long sessions**.

---

## Who This Serves

### Primary user

A **Matrix-1000 hardware owner** who:

- Used the synth mainly as a preset player until now
- Knows (or suspects) the instrument is deeply programmable
- Has tried other editors without satisfaction
- Wants frictionless remote control to sculpt sound and discover new ideas for production
- May upgrade to a **Tauntek EPROM** for smoother SysEx (recommended, not required)

Success for this user: *"I rediscovered my synth and put it back at the center of my music."*

### Secondary audiences

| Audience | Need |
|---|---|
| Sound designers | Faster workflow than Matrix-6 panel; computer patch library |
| JUCE developers | Reference architecture, docs, tests |
| Matrix-1000 Editor alumni | Native plugin, fixed save/automation, same care in documentation |

Matrix-6/6R owners may use **PATCH-mode editing only in v1** (PATCH EDIT + MATRIX MODULATION + PATCH MANAGER). **MASTER EDIT is grayed out** on Matrix-6/6R because the Master SysEx data format differs fundamentally from the Matrix-1000 (236 vs 172 bytes, different parameter layout). **Full v2 compatibility** includes MASTER EDIT, SPLIT mode, and correct patch-count semantics.

---

## Success Criteria

### User success (non-negotiable)

| Criterion | Measure |
|---|---|
| **Session reliability** | 4 hours continuous editing with hardware Matrix-1000; no hang attributable to plugin SysEx flooding |
| **Single-track DAW workflow** | One armed track plays notes/CC and routes audio; editor SysEx works while track is disarmed |
| **Complete editing (Matrix-1000)** | All PATCH + MASTER parameters read/write bidirectionally |
| **Partial editing (Matrix-6/6R, v1)** | PATCH + MATRIX MODULATION + PATCH MANAGER only; MASTER EDIT disabled |
| **Computer patch workflow** | Save/load `.syx` reliably (M4L pain point fixed) |
| **Community trust** | Positive feedback from beta testers and forum; no regression vs M4L editor trust |

### Product / project success

| Criterion | Measure |
|---|---|
| GitHub Sponsors | Active program; community contributions |
| Press / credibility | Coverage in specialist media (AudioFanzine precedent; Sound On Sound, etc.) |
| Personal use | Owner uses Matrix-Control in own productions |
| Reference repo | Documentation and code quality suitable as JUCE case study |

### Technical acceptance tests

- **Manual:** Owner-led 4 h hardware session (play + edit simultaneously)
- **Automated (CI complement):** SysEx queue and rate-limiting scenarios during owner work hours

Stock Oberheim firmware must remain supported; automation overload on stock EPROM is documented as user responsibility.

---

## Scope

### In scope — v1.0

| Area | Details |
|---|---|
| **Plugin category** | Virtual instrument (MIDI in + audio out) — not MIDI effect only |
| **Platforms** | macOS, Windows, Linux |
| **Formats** | AU (macOS), VST3 (all), Standalone (all) |
| **Dual role** | Instrument path (notes/CC) + Editor path (SysEx); see § Dual role |
| **PATCH EDIT** | 10 modules, 96 params, interactive envelope/track/patch name displays |
| **MATRIX MODULATION** | 10 buses |
| **MASTER EDIT** | 3 modules (MIDI, VIBRATO, MISC), 22 params |
| **PATCH MANAGER** | Bank Utility, Internal Patches (PASTE/STORE on RAM banks 0–1 only), Computer Patches (`.syx` only), Patch Mutator `[spec: brainstorming TBD]` |
| **Bonuses** | Module I/C/P, type-aware clipboard, INIT.syx or hardcoded defaults |
| **Header UI** | MIDI From, MIDI To, Keyboard From (Host when plugin), Audio From, Input Gain + meter, Instrument/Editor LEDs, UI scale 50–200%, skin |
| **Body / Footer** | Four edit sections; messages; firmware version |
| **MIDI architecture** | Unified outbound queue; dedicated editor thread; SysEx rate limiting (PRD defines timings) |
| **Automation** | DAW automation of PATCH + MASTER parameters with throttling |
| **Diagnostics** | Device Inquiry; UI lock if synth not detected; degraded-mode messaging |
| **GUI business rules** | Gray only where required (LEGATO PORTA, ROM PASTE/STORE, device gating, Keyboard From in plugin); matrix mod stays active |
| **Documentation** | User manual (English & French) + maintained public technical docs |

### Official test matrix (v1)

- Ableton Live 12
- Reason Studios 12
- GarageBand (macOS Tahoe 26)
- JUCE AudioPluginHost (macOS, Windows, Linux)
- Stretch (via community): Bitwig, Logic, Cubase, Reaper, FL Studio, Studio One Pro

### Beta program

- Jeremy Bernstein (Cycling '74, Matrix-1000 owner)
- Selected M4L community contacts (email / maxforlive.com)

### Out of scope — v1.0

| Item | Target |
|---|---|
| Matrix-6/6R **MASTER EDIT** | v2 |
| Matrix-6/6R SPLIT mode and full device support | v2 |
| **GROUPS** multi-unit Master mode | Not planned |
| **Request All Bank** bulk dump | Not planned |
| **Patch tags / ratings / PATCH LIBRARIAN** | v2 |
| OpenGL rendering pipeline | Abandoned |
| Paid edition / extended features | Post-v1 |
| Oberheim branding in UI | Never — own logo |
| iPad / Mira remote control | Out of scope |
| Settings menu detail (`Settings.json`, paths) | PRD |

### Aspirational release

Christmas 2026 — echo of the M4L editor launch (2020-12-21). No hard deadline; quality over speed.

---

## Vision

If Matrix-Control succeeds:

**Year 1:** Matrix-1000 owners worldwide treat it as the definitive editor. The open-source repo becomes a cited JUCE learning resource. Sponsors and press validate the craft.

**Year 2–3:** Matrix-6/6R full compatibility (v2). Possible extended or companion products. Guillaume's portfolio opens doors to paid audio tools — but Matrix-Control v1 remains the gift that rebuilt community trust.

The long arc: users who never owned a Matrix-1000 hear the plugin demo and go hunting for the hardware — because the editor finally revealed what the machine always was.

---

*Brief v0.3 — finalized 2026-05-23. Guillaume DUPONT / Ten Square Software. Patch Mutator spec and INIT hardcoded values pending separate workflows.*
