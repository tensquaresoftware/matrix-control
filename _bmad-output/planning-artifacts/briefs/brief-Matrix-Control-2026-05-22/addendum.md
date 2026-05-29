---
organization: Ten Square Software
project: Matrix-Control
title: Product Brief Addendum
author: BMad Agent
version: "0.3"
sources:
  - brief.md
  - vision-input-fr.md
created: 2026-05-22
updated: 2026-05-23
---

# Product Brief Addendum

**Purpose:** Depth that supports PRD/architecture but does not belong in the 1–2 page brief.

---

## Source: Vision.md (Guillaume, 2026-05-22)

Full French input archived at `vision-input-fr.md`. Key product decisions extracted for brief drafting:

### Vision statement (user voice)

- Users rediscover Matrix-1000 as a **programmable synth**, not a preset box.
- Cross-platform plugin + standalone; macOS, Windows, **Linux in v1**.
- Differentiators: intelligent clipboard, module init, interactive envelope/track displays, computer patch library, **Patch Mutator**.

### Origin story (condensed)

- 2020: Max for Live editor born from borrowed Matrix-1000 + community demand.
- Summer 2025: carpal tunnel leave → pivot to JUCE rewrite after Max 9 / JS debt.
- Vibe-coding with Cursor produced progress but UI scaling pain, doc sprawl, quality drift.
- BMad validated on Sparkay (mobile app); now applied to Matrix-Control.

### Primary user (#1)

Hardware-synth owner who used Matrix-1000 as preset box; tried other editors without "Wow"; wants frictionless remote control and sound design exploration.

### v1 scope (ambitious — owner-defined)

| Area | In v1 |
|---|---|
| PATCH EDIT | 10 modules, 96 params, envelope + track displays, PATCH NAME |
| MATRIX MODULATION | 10 buses × 3 params |
| PATCH MANAGER | BANK UTILITY, INTERNAL PATCHES, COMPUTER PATCHES, PATCH MUTATOR |
| MASTER EDIT | MIDI, VIBRATO, MISC (22 params) |
| Bonus | Module I/C/P, type-aware clipboard |
| UI | Header (MIDI I/O, UI scale, skin), Body (4 sections), Footer (messages) |
| Also desired v1 | DAW automation (with SysEx rate limiting), MIDI connection status, firmware version display, Audio From routing (TBD), user manual/help |

### Open design items (parked)

- PATCH MUTATOR final behavior — see `Documentation/Development/GUI/Patch-Mutator-Brainstorming.md` and Figma mockup (M00–M99, COMPARE, RETRY semantics).
- PATCH NAME edit: double-click vs EDIT button.
- BANK LOCK vs UNLOCK button label (prefer LOCK to match hardware).
- Audio/MIDI single-track vs dual-track DAW workflow — unresolved.
- `.syx` filename vs internal 8-char patch name reconciliation rules.

---

## Source: Max for Live community feedback

Archived HTML: `m4l-user-feedback.html`. Rating: **5/5** (4 votes).

### What users love (keep in Matrix-Control)

| Theme | Evidence |
|---|---|
| Best / go-to Matrix editor UI | brobinson69, sensationfc, mouldyminge |
| Clear, cared-for documentation | LorenzoDeSilva, elliotEtag, papoi |
| Works on Matrix-6 with Tauntek firmware | sensationfc |
| Automation = "game changer" | lenfantdo, FourCandles |
| Computer patch save = preserve factory banks | diosolip |

### Pain points (Matrix-Control must fix)

| Pain | Implication for v1 |
|---|---|
| Live/Max version fragility | Native plugin removes M4L dependency |
| MIDI routing confusion (OUT vs THRU) | Clear setup docs + connection diagnostics |
| SysEx receive failures | Bidirectional sync must be robust; good error messages |
| Save-to-computer broken (Max 9) | **Critical** — COMPUTER PATCHES must work reliably |
| Duplicate/spam SysEx (Max 9) | Rate limiting + queue in Core |
| Original EPROM 1.11 slow | Document Tauntek recommendation; handle gracefully |
| Push / MIDI controller mapping | DAW automation + parameter exposure |
| Random/mutation requested | Patch Mutator validates demand |
| Some params not automatable (VCF Env bug) | Full APVTS coverage + QA |

### Community expectation (Oct 2025 – Jan 2026)

Guillaume publicly announced VST/AU rewrite. Users (RobS, others) are **waiting**. First public release sets trust bar.

---

## Source: Email archive (mbox, 39 messages)

**Readable:** yes — parsed successfully via Python `mailbox`.

Sample threads confirm same themes as public comments:

- **Joe Lowery** — save patches to computer fails (Aug–Nov 2024).
- **Boris Ender** — bank change works, patch navigation issue (Apr 2025); resolved after factory reset.
- Support tone: helpful but limited time; documentation referenced as primary support channel.

Recommendation: keep mbox as private reference; no need to copy into repo unless Guillaume wants a support FAQ derived from it.

---

## Source: Figma mockup (2026-02-22)

Pixel-perfect GUI reference. Layout matches code structure:

- PATCH EDIT (left) — 10 modules + envelope displays + PATCH NAME
- MATRIX MODULATION + PATCH MANAGER (center-right)
- MASTER EDIT (far right, orange header)
- PATCH MUTATOR with module toggles (D1–L2), Amount/Random, MUTATE, HISTORY

Design constraint confirmed: **all dimensions divisible by 4** for UI Scale factors (50%–200%).

---

## Competitive / landscape (to validate with Guillaume)

Not yet documented. Brief should name alternatives users compare against:

- Hardware programmers (e.g. original Oberheim, third-party)
- Legacy software editors (Sound Diver, etc.)
- Other Matrix-1000 editors (community / commercial)
- Matrix-6/6R front panel as "competitor" for editing UX

**Action:** ask Guillaume for his personal comparison list and why each fell short.

### Owner-provided competitor list (all fall short on UX and/or completeness)

| Editor | Type |
|---|---|
| ObieEditor | Software |
| Patch Base | iPad app |
| LinuxTech | Software |
| Ctrlr | Software (panel-based) |
| MidiQuest | Software |
| Mr1234 — Matrix-6G | Software |
| Stereoping | Hardware controller |

**Owner verdict:** None offer ergonomic UI + complete "Wow!" experience for Matrix-1000 programming.

---

## Business model & community

### Distribution (v1)

- **Free**, MIT license (consistent with current repo).
- **GitHub Sponsors / donations** — not paid plugin v1; possible paid extended edition later.
- Niche market: estimated few thousand Matrix-1000 units worldwide still operational.
- Sponsorship setup in progress; first willing sponsor (Mike Ford, Apr 2026) blocked by missing Sponsor button on profile.

### Success metrics (owner-defined)

| Signal | Notes |
|---|---|
| Reliable long-session editing | Non-negotiable — see decision log |
| GitHub sponsors | Validates sustainability |
| Community forum feedback | M4L community carries over |
| Personal production use | Dogfooding |
| Press coverage | AudioFanzine (prior), Sound On Sound, Ableton site, Tom Oberheim (contacted Dec 2020, no reply) |

### Historical press & credibility

- [AudioFanzine — Matrix-1000 Editor](https://fr.audiofanzine.com/editeur-midi/tensquare/matrix-1000-editor/news/a.play,n.50627.html)
- [Cycling '74 tutorial by Jeremy Bernstein](https://cycling74.com/tutorials/building-a-synthesizer-editor-with-javascript-part-1)
- Email to Tom Oberheim (2020-12-05) — presented M4L editor, offered test copy; no response logged.

---

## Firmware & hardware assumptions

| EPROM | v1 behavior |
|---|---|
| **Tauntek (recommended)** | Smoother SysEx; document as recommended upgrade |
| **Stock Oberheim (v1.11+)** | Must work; document automation throttle limits; user responsibility if pushing too hard |

---

## INIT.syx strategy

- No canonical file yet from owner.
- User defines personal INIT patch → saves as `INIT.syx` in configurable folder.
- Plugin falls back to **hardcoded defaults** (owner to supply reference values).
- M4L manual already documents Tauntek vs stock firmware behavior.

---

## Reference assets (M4L era — input for user manual v1)

| Asset | Path |
|---|---|
| User Manual EN (2025-10-12) | `.../Matrix-1000 Editor - User Manual (Revision 2025-10-12).pdf` |
| User Manual FR (2025-10-12) | `.../Matrix-1000 Editor - Manuel Utilisateur (Révision 2025-10-12).pdf` |
| Factory patch names (banks 2–9) | `.../Oberheim Matrix-1000 - Factory Patch Names.pdf` |

Tone and setup workflows from EN manual are proven with community (frequently praised).

---

## Release planning

| Item | Value |
|---|---|
| Aspirational date | Christmas 2026 |
| Historical anchor | M4L editor released 2020-12-21 |
| Time budget | Evenings + weekends + August holidays |
| Pressure | None — quality over speed |

---

## Open v1 decisions (still need owner input)

1. ~~**Audio From**~~ — **Resolved:** ComboBox in HeaderPanel; plugin-managed MIDI ports; standalone parity.
2. **Patch Mutator** — deferred to dedicated BMad brainstorming session; Vision.md (M00–M99, simpler history) is directionally preferred over brainstorming doc (M1–M9, ORIG/A/B).
3. **INIT hardcoded defaults** — owner to program and supply values (non-blocking).

---

## MIDI architecture (v1 — dual role Instrument + Editor)

Documented in brief v0.3. Summary for PRD/architecture handoff:

```
                    ┌── Instrument path (notes, CC, bend)
Host / Keyboard ────┤   filter: strip PC + SysEx
                    └──► Unified queue ──► MIDI To ──► Matrix-1000 IN
                              ▲
Editor thread ◄── MIDI From (SysEx in only)
     │          └── SysEx + PC (patch select) out
     └──────────────────────────┘

Matrix-1000 OUT ──► Audio From ──► plugin audio output
                  (Input Gain + meter)
```

| Control | Plugin | Standalone |
|---|---|---|
| MIDI From | Synth return (SysEx) | Synth return (SysEx) |
| MIDI To | To synth IN | To synth IN |
| Keyboard From | Grayed: **Host** | Master keyboard (notes + CC) |
| Audio From | Interface input | Same |

**Multi-instance:** distinct port pairs per unit (e.g. MT4 In1/Out1 vs In2/Out2).

**Supersedes** addendum § MIDI architecture (single-path wording).

---

## Open v1 decisions (still need owner input)

1. **Patch Mutator** — brainstorming session TBD
2. **INIT hardcoded defaults** — owner to supply values

---

## Reliability acceptance (v1)

| Test | Method |
|---|---|
| 4 h continuous editing without synth hang attributable to plugin | Manual session with owner's Matrix-1000 |
| SysEx queue / rate limiting under automation load | Automated CI scenario (complement) |

---

## Deferred workflows

| Topic | Next step |
|---|---|
| Patch Mutator final spec | BMad specialized brainstorming session (before implementation epic) |
| INIT.syx reference values | Owner supplies hardcoded defaults when ready |

---

## Doc archaeology — topics for brief/PRD discussion (2026-05-22)

Items found in pre-BMad `Documentation/` not fully captured in brief v0.1. **Not authoritative** — to validate with owner.

### High priority (scope / contradiction)

| # | Topic | Source | Suggestion |
|---|---|---|---|
| 1 | **UI scale 50–200% (brief) vs 50–400% (FS §2.5)** | Functional-Specification.md | Discuss: keep 200% max or restore 300/400% presets? |
| 2 | **Matrix-6/6R in v1** — brief says "basic editing"; UC-11 rejects non-M1000 | Use-Cases.md | Align: Matrix-1000 only at startup, or allow M6? |
| 3 | **Interactive displays** — brief promises interactive; envelope mouse edit deferred in plans | EnvelopeDisplay plan | v1 = reactive display + sliders only, or mouse drag required? |
| 4 | **Linux** — brief yes; FS §6.1 lists macOS+Windows only | Functional-Specification.md | Confirm Linux v1 non-negotiable (owner said yes) → update FS later |

### Medium priority (add to brief Out of scope or Success Criteria)

| # | Topic | Source |
|---|---|---|
| 5 | **GROUPS / multi-unit Master** — FS §6.2 = v1.1+ | Functional-Specification.md |
| 6 | **PASTE/STORE only on RAM banks 0–1** — grisé sur ROM 2–9 | FS §2.3, Use-Cases |
| 7 | **Synth auto-detection gates UI** — no edit without validated device (UC-11) | Use-Cases.md |
| 8 | **Local editing on MIDI disconnect** — no auto-retry (UC-07) | Use-Cases.md |
| 9 | **Request All Bank** bulk dump — explicitly out of FS §3.3 v1 | Functional-Specification.md |
| 10 | **Settings.json + Settings menu** beyond header (paths, persistence) | FS §2.5 |

### Lower priority (PRD detail)

| # | Topic | Source |
|---|---|---|
| 11 | LEGATO PORTA / KEYBOARD MODE = UNISON constraint | FS §2.1, UC-09 |
| 12 | SysEx timing: 10 ms inter-message, 2 s timeout | FS §3.2 |
| 13 | OpenGL plan vs integer scaling strategy — architectural tension | Plans + GUI-Scaling-Strategy.md |
| 14 | JSON/XML computer patch format mention — likely obsolete | Matrix-1000-Parameters-GUI-Logic.md |
| 15 | Patch tags/ratings — FS §6.2 future | Functional-Specification.md |
| 16 | Virtual MIDI channel OMNI/1–16/GROUPS | Matrix-1000-Parameters-GUI-Logic.md |
| 17 | Matrix mod: gray DESTINATION when SOURCE = NONE | Matrix-1000-Parameters-GUI-Logic.md |
