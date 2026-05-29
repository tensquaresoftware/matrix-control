---
organization: Ten Square Software
project: Matrix-Control
title: Oberheim Matrix-6 / Matrix-6R — MIDI & SysEx Implementation
author: BMad Agent
status: reference
provenance: Community/third-party transcription of the Matrix-6/6R MIDI specification (software v2.13+). Not verified against an official Oberheim PDF. Reformatted for readability.
sources:
  - oberheim-matrix-1000-midi-sysex-implementation.md
original_author: Unknown (web source, archived by Guillaume DUPONT)
original_date: 2026-05-28
created: 2026-05-28
updated: 2026-05-29
archived: 2026-05-29
---

# Oberheim Matrix-6 / Matrix-6R — MIDI & SysEx Implementation

## Document status

| Item | Detail |
|---|---|
| **Applies to** | Matrix-6 keyboard and Matrix-6R rack (abbreviated **M-6**) |
| **Software** | Version **2.13 or later** (per source document) |
| **Provenance** | Third-party / community document — treat as **strong hint**, confirm critical bytes on hardware |
| **Companion** | [Matrix-1000 reference](./oberheim-matrix-1000-midi-sysex-implementation.md) — official transcription; Universal Device Inquiry; 10 ms patch gap; opcode `0BH` Matrix Mod |

---

## 1. Introduction

This document describes the MIDI implementation of the Matrix-6 and Matrix-6R. It assumes familiarity with the instruments and the MIDI 1.0 Specification.

Parameter naming follows the Owner's Manual with page prefixes:

| Prefix | Page | Scope |
|---|---|---|
| **P** | Patch | Changes with each patch |
| **M** | Master | Global across patches and splits |
| **S** | Split | Edited with splits |

---

## 2. Notation

- **Hexadecimal:** `19H` = 25 decimal = `00011001B`
- **MIDI status byte:** `b` = any hex digit 0–F (channel 1–16)
- **Ranges:** `0 P 99` means 0 through 99 inclusive

---

## 3. MIDI modes

Channel Mode messages are received on the **Basic Channel**. An All Notes Off is performed and MIDI mode parameters **M01 OMNI MODE** and **M18 MIDI MONO** are updated.

| Message | Effect |
|---|---|
| All Notes Off | Gates off notes received on the same channel as the message |
| Other mode changes | All voice gates removed before entering new mode |
| Mono On | All six voices available for mono mode regardless of last byte |
| Omni On in Mono | Omni treated as **off** while in Mono mode |

---

## 4. Notes and voice assignment

### 4.1 MIDI Poly mode

**Omni (M01):** ON → respond to notes on channels 1–16. OFF → Basic Channel only.

#### Note On / key press

1. Find ungated voice(s); assign using **P48 KEYBOARD MODE**:
   - **ROTATE** — next ungated voice in sequence
   - **REASSIGN** — prefer voice that last played same pitch; else ROTATE
   - **REASSIGN w/ ROB** — allows robbing gated voices for new notes

2. If all voices gated:
   - **Rob ON:** pick victim (same pitch if possible, else ROTATE); update pitch; multi trigger only
   - **Rob OFF:** if **M13 SPILLOVER** ON → Note On on Basic Channel **+ 2** (wrap 15→1, 16→2); else Note On on Basic Channel

**MIDI Out for notes:** Matrix-6 keyboard notes generate MIDI Out; Matrix-6R (MIDI-only input) does **not** echo internal voice notes to MIDI Out.

#### Note Off / key release

Matching Note On determines internal voice vs spilled MIDI channel; gate removal and optional MIDI Out generation follow assignment.

### 4.2 MIDI Mono mode

Voices map to consecutive channels starting at Basic Channel (six-channel band, wrap at 16). **Mode 3 (Omni On + Mono) not supported.**

Second Note On on same monitored channel robs voice (multi trigger, old Note Off does not ungate until matching new note's Note Off).

**M13 SPILLOVER:** no effect in Mono mode.

### 4.3 Unison mode

**P48 KEYBOARD MODE = UNISON:** all assigned voices play each note (low-note priority). Spillover after **first** note if enabled. Usually used with Poly mode.

---

## 5. Controllers

Controllers hold values from hardware and/or MIDI CC. Local controllers sum with MIDI values.

| Controller | Default CC | Notes |
|---|---|---|
| Pedal 1 | 7 | Continuous 0–127; **local** |
| Pedal 2 | 64 | Switch 0/127; **local** |
| Lever 1 | Pitch Bend | 8-bit 0–255; cannot reassign |
| Lever 2 | 1 | Also controls Lever 3 on "away" half of physical lever |
| Lever 3 | 2 | Virtual — other half of Lever 2 throw |

**M02 CONTROLLERS:** when OFF, no TX/RX of controller messages.

**Local vs global:** Lever 1, Pedal 1, Pressure = **local** (per voice on RX by channel). Others = **global**.

---

## 6. Split mode

Two patches (Upper / Lower) with programmable split point. Voice configs: 6/0, 4/2, 2/4, 0/6.

- Lower responds on Basic Channel **N**; Upper on **N + 1**
- Spillover: Lower → **N + 2**, Upper → **N + 3**
- Master parameters (M01, M18, M13, etc.) affect **both** patches

Split patch SysEx: opcode **02H** (see §8.4).

---

## 7. System Exclusive — general format

All M-6 SysEx messages:

```
F0H  10H  dd  <opcode>  <data...>  F7H
```

| Field | Value |
|---|---|
| Manufacturer | `10H` Oberheim |
| Device ID `dd` | **`06H`** — Matrix-6 native format |
| | **`02H`** — Matrix-12 / Xpander compatible format (recognized on RX; M-6 TX uses `06H`) |

**Timing (critical):** M-6 waits **20 ms** after EOX before other data. **Incoming** patch dumps should be separated by **≥ 20 ms** (Matrix-1000 official spec cites **10 ms** — do not assume identical timing).

**Parameter gate:** **M04 SYSTEM EXCLUSIVE** must be ON for SysEx generate/recognize.

Invalid manufacturer, device ID (not `06H` or `02H`), or illegal opcode → message ignored.

---

## 8. System Exclusive — operations

### 8.1 Opcode summary (device ID `06H`)

| Opcode | Name | Direction |
|---|---|---|
| `00H` | Single Patch Request | RX only — legacy; also `F0 10 06 00 pp F7` (v1.xx compat) |
| `01H` | Single Patch Data | TX/RX — 134-byte patch (see §10.1) |
| `02H` | Split Patch Data | TX/RX — 18-byte split (see §10.2) |
| `03H` | Master Parameter Data | TX/RX — **236 bytes** (see §10.3) |
| `04H` | General Data Request | RX only |
| `05H` | Enter Remote Edit Mode | RX only — prefix before param edits |
| `06H` | Change Parameter | RX only — param 0–99, value non-negative |

**Matrix-1000-only opcodes** (Set Bank `0AH`, Matrix Mod bus `0BH`, etc.) are documented in the [Matrix-1000 reference](./oberheim-matrix-1000-midi-sysex-implementation.md) — **not listed** in this M-6 appendix; verify on hardware before relying on them for Matrix-6/6R.

### 8.2 Request patch dump (`04H`)

```
F0H 10H 06H 04H xx pp F7H
```

| `xx` (request type) | Transmits |
|---|---|
| 0 | All single patches, splits, and master (separate messages each) |
| 1 | Single patch |
| 2 | Split patch |
| 3 | Master parameters |

| `pp` | Patch number: 0–99 single, 0–49 split; ignored for type 0 and 3 (pad byte still required) |

**Dump all:** ~29K bytes total; end detection: user prompt or **> 500 ms** idle timeout.

### 8.3 Single / split / master transmission

Nibble packing identical to Matrix-1000: each data byte → two 7-bit nibbles (low nibble first, then high). Checksum = 7-bit sum of **packed** data bytes.

| Message | Packed size | Total TX bytes (approx.) |
|---|---|---|
| Single patch | 134 | 275 |
| Split patch | 18 | 42 |
| Master | **236** | **477** |

On RX: hardware protect and patch protect checked before store.

### 8.4 Remote editing

1. Send **`05H`** — enter Quick Patch Edit (unit must already be in Patch Edit mode)
2. Send **`06H pp vv`** — change parameter; out-of-range ignored; negative values **not supported** via this opcode (must send full patch)

**Xpander / Matrix-12 compatibility (device ID `02H`, opcode `0DH`):**

| Sub-opcode | Mode |
|---|---|
| `01H` | Single Patch mode |
| `02H` | Split ("Multi-Patch") mode |

### 8.5 Front-panel send (Master Edit)

| Parameter | Action |
|---|---|
| M10 SEND DATA | Sends current single or split patch |
| M11 SEND ALL | All 100 singles + 50 splits + master (~12 s) |

---

## 9. Miscellaneous MIDI

| Topic | Rule |
|---|---|
| **Patch Change** | If M03 ON: TX/RX program change; max patch 99 (single) / 49 (split) |
| **Patch Map** | M15 / M16 translate incoming/outgoing program numbers |
| **Echo** | M12 MIDI ECHO — retransmit MIDI In (not SysEx); spillover can duplicate channels |
| **Running Status** | Supported in and out |
| **Note Off** | Always separate Note Off with release velocity (not velocity 0 Note On) |
| **Local Control** | M05 — OFF disables keyboard→voices and spillover |
| **Active Sensing** | M14 — TX after 240 ms idle; All Notes Off after 360 ms RX idle |
| **Tune Request** | Tunes HFOs; CALIBRATE is separate |

---

## 10. Data formats

### 10.1 Single patch (134 bytes)

**Identical packed layout to Matrix-1000.** Full byte map: see [Matrix-1000 reference § Single Patch Data Format](./oberheim-matrix-1000-midi-sysex-implementation.md).

Statistics: 134 bytes packed → 268 nibbles + header + checksum + EOX = **275 bytes** transmitted.

**Parameter ranges in patch** (examples — not all 0–63):

| Encoding | Examples |
|---|---|
| 6-bit unsigned | Most envelope times, LFO speeds, DCO pulse width |
| 6-bit signed | DCO 2 Detune |
| 7-bit unsigned | VCF Frequency (param 21) |
| 7-bit signed | Mod amounts (bytes 86–103), Matrix bus amounts |
| 5-bit codes | Matrix bus source/destination, track input |

### 10.2 Split patch (18 bytes)

| Byte | Parameter | Bits | Description |
|---|---|---|---|
| 0–5 | Split name | 6 each | 6-bit ASCII characters |
| 6–7 | — | 6 each | Not used |
| 8 | Lower patch | 7 | Lower patch number |
| 9 | Upper patch | 7 | Upper patch number |
| 10 | S00 | 7 | Left zone limit |
| 11 | S01 | 6 signed | Left zone transpose |
| 12 | S02 | 1 | Left zone MIDI out enable |
| 13 | S03 | 7 | Right zone limit |
| 14 | S04 | 6 signed | Right zone transpose |
| 15 | S05 | 1 | Right zone MIDI out enable |
| 16 | S06 | 6 signed | Left/right balance (−31 left … +31 right) |
| 17 | S07 | 2 | Voice config: 0=2/4, 1=4/2, 2=6/0, 3=0/6 |

Total transmitted: **42 bytes**.

### 10.3 Master parameters (236 bytes)

| Byte | Param | Bits | Description |
|---|---|---|---|
| 0 | — | — | Not used |
| 1 | 30 | 6 | Vibrato Speed |
| 2 | 31 | 3 | Vibrato Waveform |
| 3 | 32 | 6 | Vibrato Amplitude |
| 4 | 33 | 2 | Vibrato Speed mod source |
| 5 | 34 | 6 | Vibrato Speed mod amount |
| 6 | 35 | 2 | Vibrato Amp mod source |
| 7 | 36 | 6 | Vibrato Amp mod amount |
| 8 | 55 | 6 signed | Master Tune |
| 9 | 40 | 2 | Velocity scale type |
| 10 | 41 | 6 | Velocity sensitivity |
| 11 | 00 | 4 | MIDI Basic Channel |
| 12 | 01 | 1 | MIDI Omni enable |
| 13 | 02 | 1 | MIDI Controllers enable |
| 14 | 03 | 1 | MIDI Patch Changes enable |
| 15 | 04 | 1 | MIDI SysEx enable |
| 16 | 05 | 1 | MIDI Local Control enable |
| 17–20 | 06–09 | 7 each | Pedal/Lever CC assignments |
| 21 | 42 | 1 | Pedal 2 invert |
| 22 | 43 | 1 | Levers invert |
| 23 | 53 | 5 | Display brightness |
| 24 | 56 | 1 | SQUICK enable |
| 25 | 17 | 1 | Patch map echo |
| 26 | 57 | 1 | **Stereo output enable** |
| 27 | — | — | Not used |
| 28 | 44 | 6 | Pressure standoff |
| 29 | 13 | 1 | Spillover enable |
| 30 | — | — | Not used |
| 31 | 14 | 1 | Active sensing enable |
| 32 | 12 | 1 | MIDI echo enable |
| 33 | 15 | 1 | Patch map enable |
| 34 | — | — | Not used |
| 35 | 18 | 1 | MIDI Mono enable |
| 36–135 | — | 6 each | Input patch map (100 entries) |
| 136–235 | — | 6 each | Output patch map (100 entries) |

**Matrix-1000 master = 172 bytes** — different layout and size. Matrix-Control v1 grays MASTER EDIT on Matrix-6/6R (D-006).

---

## 11. Matrix-1000 cross-reference (Matrix-Control)

| Topic | Matrix-1000 | Matrix-6/6R |
|---|---|---|
| Single patch packed size | 134 bytes | **Same** |
| Master packed size | 172 bytes | **236 bytes** |
| Min gap between patch SysEx | **10 ms** (official) | **20 ms** (this doc) |
| Split patches | Dummy 50 splits on Request All | Native 50 splits, opcode `02H` |
| SysEx device ID in `F0 10 dd …` | `06H` | `06H` (native) / `02H` (compat RX) |
| Remote Matrix Mod bus opcode `0BH` | Documented | **Not in this doc** — use M-1000 ref + hardware test |
| Universal Device Inquiry | Documented (§ Miscellaneous) | **Not in source doc** — see §12 |

### Table 1 — LFO wave codes

0 Triangle · 1 Up Saw · 2 Down Saw · 3 Square · 4 Random · 5 Noise · 6 Sampled · 7 Not used

### Table 2 — Modulation sources (0–20)

0 Unused* · 1–3 Env 1–3 · 4–5 LFO 1–2 · 6 Vibrato · 7–8 Ramp 1–2 · 9 Keyboard · 10 Portamento · 11 Track Gen · 12 Kbd Gate · 13 Velocity · 14 Rel Vel · 15 Pressure · 16–17 Pedal 1–2 · 18–20 Lever 1–3

\* Code 0 valid for mod source list only — not a track generator input (param 33 / S33).

### Table 3 — Modulation destinations (1–32)

1 DCO1 Freq · 2 DCO1 PW · 3 DCO1 Wave · 4 DCO2 Freq · 5 DCO2 PW · 6 DCO2 Wave · 7 Mix · 8 VCF FM · 9 VCF Freq · 10 VCF Res · 11 VCA1 · 12 VCA2 · 13–16 Env1 D/D/A/R · 17 Env1 Amp · 18–22 Env2 · 23–27 Env3 · 28–29 LFO1 · 30–31 LFO2 · 32 Portamento Time

---

## 12. Device Inquiry (from Matrix-1000 official spec)

The M-6 source document does **not** describe Universal Device Inquiry. The [Matrix-1000 reference](./oberheim-matrix-1000-midi-sysex-implementation.md) defines:

**Request:** `F0H 7EH 7FH 06H 01H F7H` (channel `7FH` = any device)

**Reply:** `F0H 7EH <chan> 06H 02H 10H 06H 00H <memb-lo> <memb-hi> <rev-0..3> F7H`

| Field | Matrix-1000 (official) |
|---|---|
| `<mfg>` | `10H` Oberheim |
| `<fam-lo>` | `06H` (Matrix-6/6R/1000 family) |
| `<fam-hi>` | `00H` |
| `<memb-lo>` | **`02H`** (Matrix-1000) |
| `<memb-hi>` | **`00H`** |
| `<rev-0..3>` | Four ASCII chars, version right-justified (e.g. ` 110` = v1.10) |

**Matrix-6/6R member bytes:** not documented in either archived file. Matrix-Control must accept M-6/M-6R replies with distinct `<memb-lo>` / `<memb-hi>` once confirmed on hardware (community M4L feedback confirms PATCH editing works; Tauntek firmware cited for M-6 and M-1000).

**Code note:** `SysExConstants.h` currently sets `kExpectedMemberLow = 0x00`, `kExpectedMemberHigh = 0x02` — likely **byte order mismatch** vs official spec (`memb-lo=02H`, `memb-hi=00H`). Fix during Device Inquiry hardening.

---

## 13. Appendix A — Channel voice messages (summary)

| Status | Data | Description |
|---|---|---|
| `8nH` | note, velocity | Note Off (release vel 1–127) |
| `9nH` | note, velocity | Note On (vel 0 = Note Off RX only) |
| `BnH` | cc, value | Control Change (if enabled) |
| `CnH` | program | Program Select (if enabled) |
| `EnH` | LSB, MSB | Pitch Bend |

## 14. Appendix B — Channel mode messages (summary)

| CC | Value | Function |
|---|---|---|
| 122 | 0/127 | Local Control off/on |
| 123 | 0 | All Notes Off |
| 124 | 0 | Omni Off |
| 125 | 0 | Omni On |
| 126 | 6 | Mono On |
| 127 | 0 | Mono Off |

---

## 15. MIDI volume workaround (M-6 / 6R)

Use MIDI CC 7 (Volume) via Pedal 1 routing to modulate Env2 → VCA2:

1. **Master:** M06 Pedal1 = 7  
2. **Patch:** VCA1/VEL +63, VCA2/EN2 +63, ENV2 AMP 0, ENV2/VEL 0  
3. **Matrix Mod:** PED1 → +63 → E2AMP  

Patch edits and matrix mod must be applied per program.

---

*End of reformatted Matrix-6/6R reference — archived 2026-05-29.*
