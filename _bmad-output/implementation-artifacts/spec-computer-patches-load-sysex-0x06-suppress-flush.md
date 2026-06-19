---
title: 'Computer Patches load — suppress SysEx 0x06 burst fix'
type: 'bugfix'
created: '2026-06-19'
status: 'done'
route: 'one-shot'
---

# Computer Patches load — suppress SysEx 0x06 burst fix

## Intent

**Problem:** Loading a `.syx` file (combobox or `<`/`>` navigation) enqueued one full patch SysEx (`0x01`) then ~40 per-parameter SysEx (`0x06`/`0x0B`) because JUCE APVTS defers parameter→ValueTree sync on an internal timer (~20 ms). The suppress flags were cleared before those deferred notifications reached `PluginProcessor::valueTreePropertyChanged`.

**Approach:** Centralize bulk model→APVTS push in `pushPatchModelToApvtsWithSuppress`, force a synchronous APVTS flush via `copyState()` (public API that internally calls the private flush) while suppress is still active, then release suppress.

## Suggested Review Order

1. [PatchManagerActionHandler.cpp — bulk push helper](Source/Core/Actions/PatchManagerActionHandler.cpp) — core fix: suppress → bufferToApvts → flush → release
2. [ApvtsPatchMapperTests.cpp — flush contract test](Tests/Unit/ApvtsPatchMapperTests.cpp) — regression test for deferred-notification timing
3. [PluginProcessor.cpp — valueTreePropertyChanged suppress branch](Source/Core/PluginProcessor.cpp) — confirm 0x06 dispatch is gated by `suppressPatchParameterSysEx_`
