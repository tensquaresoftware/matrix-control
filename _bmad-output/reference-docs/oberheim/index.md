---
organization: Ten Square Software
project: Matrix-Control
title: Oberheim Reference Documentation Index
author: BMad Agent
status: reference
created: 2026-05-29
updated: 2026-07-17
---

# Oberheim Reference Documentation Index

Canonical Oberheim MIDI/SysEx references archived for Matrix-Control BMad workflows (PRD, Architecture, Core implementation).

**Single copy:** these files under `_bmad-output/reference-docs/oberheim/` are the project SSOT (no parallel `_local/` copies to maintain).

| Document | Role | Provenance |
|---|---|---|
| [oberheim-matrix-1000-midi-sysex-implementation.md](./oberheim-matrix-1000-midi-sysex-implementation.md) | **Primary** — Matrix-1000 MIDI/SysEx (opcodes, patch/master layouts, Device Inquiry) | Official Owner's Manual PDF; re-verified 2026-07-17 |
| [oberheim-matrix-6-6r-midi-sysex-implementation.md](./oberheim-matrix-6-6r-midi-sysex-implementation.md) | **Complement** — Matrix-6/6R behaviour (Split, Mono/Poly, 236-byte master, 20 ms SysEx gap); patch format cross-ref M-1000 | Community transcription (youngmonkey, v2.13+); re-checked 2026-07-17 — confidence nuanced |
| [oberheim-matrix-6-6r-midi-sysex-guide-lecture.md](./oberheim-matrix-6-6r-midi-sysex-guide-lecture.md) | Reading guide for the Matrix-6/6R SysEx doc | Owner notes |
| [oberheim-matrix-1000-sysex-messages-examples.md](./oberheim-matrix-1000-sysex-messages-examples.md) | Worked SysEx message examples (Matrix-1000) | Owner notes |
| [oberheim-matrix-1000-factory-patch-names.md](./oberheim-matrix-1000-factory-patch-names.md) | Factory patch name list | Owner notes |
| [oberheim-matrix-1000-user-manual-personal-notes.md](./oberheim-matrix-1000-user-manual-personal-notes.md) | Personal notes on the Matrix-1000 manual | Owner notes |
| [oberheim-matrix-6r-basic-patch.md](./oberheim-matrix-6r-basic-patch.md) | Basic Matrix-6R patch notes | Owner notes |

**Related verification artifact:** `_bmad-output/implementation-artifacts/spec-midi-docs-oberheim-verification.md` (2026-07-17 quick-dev).

**Matrix-Control code today:** `Source/Core/MIDI/SysEx/SysExConstants.h` — validate `kExpectedMemberLow/High` against M-1000 Device ID table (`memb-lo=02H`, `memb-hi=00H`). Note: opcode `0x0D` requires a literal header byte `0` after the opcode (see Matrix-1000 MD); encoder fix tracked separately.
