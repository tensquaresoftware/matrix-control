---
organization: Ten Square Software
project: Matrix-Control
title: Oberheim Reference Documentation Index
author: BMad Agent
status: reference
created: 2026-05-29
updated: 2026-05-29
---

# Oberheim Reference Documentation Index

Canonical Oberheim MIDI/SysEx references archived for Matrix-Control BMad workflows (PRD, Architecture, Core implementation).

| Document | Role | Provenance |
|---|---|---|
| [oberheim-matrix-1000-midi-sysex-implementation.md](./oberheim-matrix-1000-midi-sysex-implementation.md) | **Primary** — Matrix-1000 official spec transcription; Universal Device Inquiry; patch/master layouts; opcode `0BH` Matrix Mod | Guillaume DUPONT — from official Oberheim PDF |
| [oberheim-matrix-6-6r-midi-sysex-implementation.md](./oberheim-matrix-6-6r-midi-sysex-implementation.md) | **Complement** — Matrix-6/6R behaviour (Split, Mono/Poly, 236-byte master, 20 ms SysEx gap); PATCH format cross-ref M-1000 | Community/third-party spec (v2.13+); reformatted 2026-05-29 |

**Working copies (owner):** `_local/documentation/references/md/` — same filenames.

**Matrix-Control code today:** `Source/Core/MIDI/SysEx/SysExConstants.h` — validate `kExpectedMemberLow/High` against M-1000 Device ID table (`memb-lo=02H`, `memb-hi=00H`).
