---
organization: Ten Square Software
project: Matrix-Control
title: PRD Addendum
author: BMad Agent
status: draft
version: "0.2"
sources:
  - prd.md
  - .decision-log.md
  - ../../reference-docs/oberheim/index.md
created: 2026-05-25
updated: 2026-05-29
---

# PRD Addendum

Technical detail that supports the PRD but does not belong in the main FR narrative.

---

## Interactive display geometry — EnvelopeDisplay

Source: Guillaume, Phase 0 zone 3 workshop (2026-05-27). Values 0–63 match Matrix-1000 SysEx (6-bit positive parameters).

### Layout

- Light padding around the panel so control points do not touch edges.
- Start point: fixed bottom-left (Note On start).

### Segments and drag behaviour

| Parameter | Horizontal | Vertical | Max X span (value 63) | Max Y span (value 63) | Notes |
|---|---|---|---|---|---|
| Delay | Draggable (duration) | Fixed at bottom | ~20% panel width | — | Segment always horizontal at bottom |
| Attack | Draggable (duration) | Fixed at top | ~20% panel width | — | At value 0, segment intentionally not fully vertical |
| Decay | Draggable (duration) | Follows Sustain level | ~20% panel width | ~100% panel height | At value 0, segment intentionally not fully vertical; **no direct vertical drag on Decay point** (avoids simultaneous Decay + Sustain SysEx) |
| Sustain | Fixed segment length | Draggable via sustain point/segment | ~20% panel width (fixed) | ~100% panel height | Horizontal segment translates vertically |
| Release | Draggable (duration) | Fixed at bottom | ~20% panel width | — | At value 0, segment intentionally not fully vertical |

### Visual markers (D-017)

- Replace filled/hollow circles with **small filled squares** at junctions (80's rectangular GUI aesthetic).

---

## Interactive display geometry — TrackGeneratorDisplay

- Same outer padding as envelope displays.
- Five points: **vertical drag only** (Y maps 0–63 to ~100% panel height).
- Points spaced evenly on X; each segment ~25% panel width.
- Markers: small filled squares (same as envelope displays).

---

## Target sync architecture (D-018)

**Single source of truth:** APVTS `AudioParameter` values — **Int** and **Choice** types with per-descriptor normalisation (not uniformly 0–63; see § Parameter value ranges).

**Slider → curve:** existing `SliderParameterAttachment` updates APVTS; display listens to parameter change and repaints (`notify = false` on display setters to avoid feedback loops).

**Curve → slider:** display writes `parameter.setValueNotifyingHost(normalised)` (with `beginChangeGesture` / `endChangeGesture` during mouse drag); JUCE attachments update sliders automatically — **no** manual `slider.setValue()` from the display.

**External updates** (preset load, DAW automation, MIDI echo from synth): same parameter listeners refresh both sliders (via attachments) and displays.

**PluginProcessor:** reacts to parameter changes for SysEx / business logic — not for UI resync.

**Refactor scope:** remove Display→Slider bridge in `PatchEditPanel`; replace `PatchEditDisplaysPanel` paramId if-chain with descriptor-driven mapping table.

---

## Outbound SysEx strategy (D-044)

| Trigger | Message type | Opcode / pattern |
|---|---|---|
| PATCH slider / combo change | Remote Parameter Edit (single param) | 0x06 |
| MASTER any param change | Full master message | 0x03 |
| Matrix Mod bus param change | Bus SOURCE + AMOUNT + DESTINATION together | Per Oberheim MD + M4L ref (verify 0x0B vs 0x06 block at impl) |
| Full patch replace (STORE, COPY load, Mutator, file load, etc.) | Single patch data | 0x01 |
| Split patch | — | **Not supported v1** (D-046) |

Reference: `_bmad-output/reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md`

---

## PatchModel / MasterModel (D-057)

- **PatchModel:** 134-byte packed buffer (Matrix-1000 / Matrix-6 / Matrix-6R PATCH); bytes **0–7** = patch name (6-bit ASCII per char); accessors `getPatchName` / `setPatchName` on buffer.
- **MasterModel:** 172-byte packed buffer (Matrix-1000 only in v1). Matrix-6/6R master is **236 bytes** — v2 when MASTER EDIT ships (D-006).
- **ApvtsPatchMapper / ApvtsMasterMapper:** read/write APVTS ↔ buffer using `sysExId` and `sysExOffset` from `PluginDescriptors` only — **no parallel offset table**.
- APVTS remains UI value source of truth; buffer is SysEx serialization view.

---

## MIDI architecture (D-048 – D-052)

### Public MidiManager API

Imperative methods: `sendPatch`, `sendMaster`, `requestCurrentPatch`, `requestMaster` (rename from `requestMasterData`). Internally all outbound calls **enqueue**; only MIDI thread dequeues and writes hardware.

### Thread model (hybrid)

- **Sync RPC:** Device Inquiry, `requestCurrentPatch`, `requestMaster` — blocking wait acceptable for startup and explicit user operations.
- **Async:** `MidiManager::run()` consumes outbound queue; inbound callback notifies MIDI thread; APVTS updates via `MessageManager::callAsync()`.

### Unified outbound queue (D-067)

- Producers: Instrument path (notes/CC/bend, PC/SysEx stripped), Editor path (SysEx, Program Change).
- Consumer: single MIDI thread → `MidiSender`.
- **Priority:** Note On/Off, Control Change, and Pitch Bend **dequeue before** SysEx — live playing must not wait on parameter floods (FR-7, glossary).
- **Inter-SysEx delay:** minimum gap enforced after each SysEx EOX; baseline and profile per § SysEx delay profiles (D-078).
- Never call `MidiOutput` from audio thread.

### Transport classes

Keep SRP split: `MidiInputPort`, `MidiOutputPort`, `MidiSender`, `MidiReceiver`, plus **BUILD NEW** `MidiOutboundQueue`. Rewire `MidiReceiver` → `MidiManager::handleIncomingSysEx` (currently disconnected).

---

## PluginProcessor / dual-role (D-053 – D-056)

- CMake: `IS_SYNTH TRUE`, AU Music Device, VST3 Instrument.
- `processBlock()` delegates to `InstrumentMidiForwarder` + `AudioPassthroughProcessor` (< 15 lines).
- **Standalone Audio From:** `AudioDeviceManager` physical input selection.
- **Plugin Audio From:** host stereo input bus (user routes synth out → plugin in).
- Instrument obeys track arming; Editor ignores arming.

---

## Clipboard compatibility matrix (D-033, D-060)

### Modes

- **Module clipboard:** last module Copy wins; Paste on compatible modules only.
- **Patch clipboard:** Internal Patches Copy grays all PATCH EDIT Paste buttons.

### Module pairs

| Pair | Rule |
|---|---|
| ENV 1 ↔ 2 ↔ 3 | Full parameter copy |
| DCO 1 ↔ 2 | Partial: skip SYNC/DETUNE cross-mapping; WAVE SELECT only OFF/PULSE/WAVE/BOTH; KEYBD/PORTA only KEYBD and PORTA (not OFF from DCO2→DCO1) |
| LFO 1 ↔ 2 | Partial: SPEED<PRESSURE ↔ SPEED<KEYBD; AMP<RAMP1 ↔ AMP<RAMP2; remaining compatible params copy normally |

No footer explanation — document in user manual EN/FR.

### Deferred: ENV shape-only copy (D-061)

Copy envelope **shape** (not full module) via header `[<n]` buttons (Figma); not v1 module I/C/P scope.

---

## Computer Patches combobox states (D-024)

| State | When | Combobox | Footer |
|---|---|---|---|
| `<EMPTY>` | No folder / invalid path / empty folder | Grayed | “0 files…” or “Folder not found” |
| `<SELECT>` | Folder has valid files, none chosen | Active | Prompts user to select |
| File entry | User selected `.syx` | Active | Load result / reconciliation notice |

Sentinels are UI-only — not list items. Chevron `<…>` = 80's special-state convention (8 Matrix-compatible chars).

### Previous / Next file navigation (D-075, FR-52)

- **`[<]` / `[>]`** buttons cycle circularly through the **sorted valid file list** (same order as combobox entries).
- Disabled when combobox is in `<EMPTY>` or `<SELECT>` state.
- Load behaviour identical to combobox selection (validation, SysEx send, reconciliation policy D-025).
- **Distinct from** Patch Mutator navigation — D-026 excludes `<` `>` only for Mutator (combobox sufficient there).

---

## Filename ↔ internal name reconciliation (D-025)

**Default Settings policy:** prefer internal name (sanitized).

**Alternatives:** prefer filename if valid; ask once per load (opt-in).

**Save/Save As:** inject validated filename into patch name bytes.

**No default modal** on bulk folder import.

---

## INIT policy (D-034, D-034a, D-040)

| Target | User file | Fallback |
|---|---|---|
| PATCH module I, Internal INIT | `PatchInit.syx` in Settings folder | Hardcoded `InitDefaults` |
| MASTER module I | `MasterInit.syx` | Hardcoded master defaults |
| Matrix Mod section / bus | — | Hardcoded per-bus NONE/0%/NONE |

Init templates loaded via **InitTemplateLoader** — not Computer Patches combobox; no D-025 reconciliation dialog.

---

## ActionDispatcher property taxonomy (D-031)

| Class | Persisted in session XML | Examples |
|---|---|---|
| **event** | No | Button timestamps |
| **state** | Yes | Mutator toggles, bank/patch numbers, skin, scale |
| **uiMirror** | Optional | Paste enabled flags pushed Core→GUI |

---

## v1 implementation order (D-058)

1. PatchModel + MasterModel (+ mappers)
2. APVTS → SysEx wiring
3. InitDefaults + InitTemplateLoader
4. PatchFileService
5. ClipboardService
6. PatchMutatorEngine **last** (brainstorming before detailed FR finalize)

---

## Dev sandbox (D-062 – D-065)

- Enrich TestComponent to cover all custom widget types × UI scale 50–200%.
- Remove from **release** binary after prod UI scale audit complete.
- Keep under `#if JUCE_DEBUG` for personal regression testing post-release.
- Study >200% (up to 400%) in debug only — not v1 prod scale range.

---

## Automated testing priorities (D-047)

**High (CI unit):** SysEx round-trip, checksum, `.syx` validation, PatchModel/MasterModel packing, ClipboardService, ApvtsValidator, WidgetFactoryValidator, ActionDispatcher routing mock, InitDefaults, queue rate logic, prefs temp files.

**Low (manual):** Skin rendering, UI scale visual audit, widget paint.

**Acceptance gate:** 4 h owner hardware session — manual, not CI.

---

## Device Inquiry & deviceType (D-069, D-079, D-080)

**Request:** Universal Device Inquiry `F0 7E <chan> 06 01 F7` (channel `7F` = any device).

**Reply:** Device ID `F0 7E <chan> 06 02 <mfg> <fam-lo> <fam-hi> <memb-lo> <memb-hi> <rev-0..3> F7`

| Field | Matrix-1000 (documented) | Matrix-6 / 6R |
|---|---|---|
| `<mfg>` | `10H` Oberheim | same |
| `<fam-lo>` / `<fam-hi>` | `06H` / `00H` | same |
| `<memb-lo>` / `<memb-hi>` | **`02H` / `00H`** | **TBD — hardware confirm** (open question PRD §9) |
| `<rev-0..3>` | ASCII firmware, right-justified | same pattern |

**APVTS properties:** `deviceDetected`, `deviceType` (enum: Matrix-1000 / Matrix-6 / Matrix-6R), `deviceVersion` (four-char string).

**UI placement:** device type + firmware → **footer right zone** (FR-53), not header.

**Implementation fix (D-080):** `SysExConstants.h` likely has member bytes reversed vs spec — expect `kExpectedMemberLow = 0x02`, `kExpectedMemberHigh = 0x00` for M-1000.

**Reference:** `_bmad-output/reference-docs/oberheim/index.md`

---

## SysEx delay profiles (D-078, NFR-2)

Minimum gap **after EOX** before the next outbound SysEx (or next patch dump). Profile selected from Device Inquiry firmware string / EPROM class.

| Profile | When | M-1000 baseline | M-6 / 6R baseline |
|---|---|---|---|
| **Stock** | Factory Oberheim firmware | **10 ms** (official) | **20 ms** (M-6 ref) |
| **Optimised** | Tauntek / Gligli / Nordcore (string match TBD) | Shorter than stock (Architecture + hardware test) | Shorter than stock (TBD) |

**Notes:**
- DAW automation may coalesce/throttle Remote Parameter Edit bursts — exact curves TBD Architecture (PRD open question §9).
- Stock EPROM overload under heavy automation is a **documented user responsibility** in manual — not a plugin bug if limits are respected.
- Matrix Mod bus reorder (FR-50) sends multiple bus updates — must respect active profile.

---

## Parameter value ranges (D-072)

APVTS exposes **Int** and **Choice** parameters. Each descriptor defines:

- **SysEx encoding** at `sysExOffset` in packed patch buffer (6-bit, 7-bit signed/unsigned, choice index).
- **Display range** and **normalisation** for `AudioParameterInt` / `AudioParameterChoice`.

**Examples (Matrix-1000 patch table — not exhaustive):**

| Category | Typical range | APVTS type |
|---|---|---|
| Envelope times, LFO speed, most levels | 0–63 (6-bit) | Int |
| Signed modulation amounts | −63…+63 (7-bit signed) | Int |
| Wave select, key modes | enum list | Choice |
| Matrix Mod source/destination | enum list | Choice |

**Rule:** no parallel offset table — `PluginDescriptors` only. Complete audit against Oberheim M-1000 parameter table before Architecture sign-off.

---

## Peak level indicator (D-071, FR-8)

- **Not** a continuous VU/level meter — a **peak hold / clip-style indicator** in header.
- **Fill colour:** solid RGB matching **EnvelopeDisplay curve colour** from active Skin/Look (same token or LookBuilder constant).
- Input Gain remains user-adjustable; indicator reflects post-gain peak on audio input bus.

---

## Audio input bus layout (D-070, FR-4)

| `deviceType` | Plugin input bus | Rationale |
|---|---|---|
| Matrix-1000 | **Mono** | Mono audio out on hardware |
| Matrix-6 / Matrix-6R | **Stereo** | Master byte 57 Stereo Output (M-6 ref) |

- Bus layout changes when `deviceType` updates after port reconfiguration / Device Inquiry.
- Standalone: physical input selection via `AudioDeviceManager` follows same mono/stereo expectation per detected device.
- Host routing (e.g. Live External Instrument mono/stereo channel) is user/DAW responsibility.

---

## Unsaved patch dirty snapshot (D-074, FR-51)

**Dirty when:** current PATCH APVTS / packed buffer differs from **last loaded or synced snapshot** (internal patch load, Computer file load, `requestCurrentPatch` success, post-STORE sync).

**Warn on navigation away to:** different internal patch number/bank, different Computer Patches file, combobox selection change, FR-52 prev/next.

**Settings policy (FR-40):** warn always (default) | never warn (expert).

**Out of scope for modal:** bulk import reconciliation (D-025), init template loads (D-037), Mutator MUTATE (in-session by design).

---

## Matrix Mod bus reorder (D-073, FR-50)

- User drags **bus number label** (0–9 fixed positions) to reorder **row contents**.
- **Labels 0–9 do not move** — only Source, Amount, Destination **values** permute between rows (swap or move — UX spec).
- Each completed reorder sends SysEx updating affected buses (SOURCE + AMOUNT + DESTINATION together per FR-14).
- Enables grouping inactive buses (NONE/0%/NONE) visually without renumbering hardware buses.

---

*Addendum v0.2 — supports PRD v0.2 (PO review 2026-05-29); Architecture may extend without duplicating FR narrative.*
