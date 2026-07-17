---
title: 'MIDI docs Oberheim verification'
type: 'chore'
created: '2026-07-17'
status: 'done'
route: 'one-shot'
---

# MIDI docs Oberheim verification

## Intent

**Problem:** Local MIDI/SysEx Markdown references for Matrix-1000 and Matrix-6/6R could diverge from their sources (official PDF vs community web page), including dropped fixed bytes such as the literal `0` after opcode `0DH`.

**Approach:** Section-by-section verification against each source; correct the Markdown only on firm evidence; list OCR/web ambiguities without inventing; report product-code contamination risks without coding.

## Suggested Review Order

**Matrix-1000 (official PDF)**

- Opcode `0DH` literal header `0` and length note moved off `0EH`
  [`oberheim-matrix-1000-midi-sysex-implementation.md:176`](../../_local/Documents/References/MD/oberheim-matrix-1000-midi-sysex-implementation.md#L176)

- Global bytes 166–168 restored (incl. Group Mode Enable)
  [`oberheim-matrix-1000-midi-sysex-implementation.md:461`](../../_local/Documents/References/MD/oberheim-matrix-1000-midi-sysex-implementation.md#L461)

- Ambiguities / implementer caveats after review
  [`oberheim-matrix-1000-midi-sysex-implementation.md:538`](../../_local/Documents/References/MD/oberheim-matrix-1000-midi-sysex-implementation.md#L538)

**Matrix-6/6R (youngmonkey, nuanced confidence)**

- Provenance + confidence banner
  [`oberheim-matrix-6-6r-midi-sysex-implementation.md:24`](../../_local/Documents/References/MD/oberheim-matrix-6-6r-midi-sysex-implementation.md#L24)

- Split Balance `-31` OCR correction
  [`oberheim-matrix-6-6r-midi-sysex-implementation.md:493`](../../_local/Documents/References/MD/oberheim-matrix-6-6r-midi-sysex-implementation.md#L493)

**Product risk (no code change this run)**

- `buildHeader` may omit literal `0` for opcode `0x0D`
  [`SysExEncoder.cpp:195`](../../Source/Core/MIDI/SysEx/SysExEncoder.cpp#L195)
