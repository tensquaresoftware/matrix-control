---
title: 'Internal Patches — startup coordinates 0/00 policy'
type: 'bugfix'
created: '2026-06-19'
status: 'done'
route: 'one-shot'
---

# Internal Patches — startup coordinates 0/00 policy

## Intent

**Problem:** The plugin cannot read the Matrix-1000's active bank/patch at launch. Restoring session coordinates and assuming hardware alignment caused silent desync when the user changed the synth front panel between sessions. `lastSyncedBank_` tracked "what we believe the synth has" incorrectly.

**Approach:** On every plugin instance start and after APVTS state restore (standalone session or DAW project), reset Internal Patches UI to bank **0** / patch **00** with **no MIDI**. Clear `PatchSelectionMidiSync` state (`hasLastSyncedBank_ = false`). The first explicit navigation (Prev/Next) or bank select sends **Set Bank (0x0A) + Program Change** to establish sync; subsequent same-bank navigation sends PC only.

## Code Map

- `Source/Core/PluginProcessor.cpp` — `resetInternalPatchCoordinatesToDefaults()`; called from ctor end and `setStateInformation`
- `Source/Core/MIDI/PatchSelectionMidiSync.{h,cpp}` — `clearSyncedBankState()` vs `resetLastSyncedBank()`
- `Tests/Unit/PatchManagerActionHandlerTests.cpp` — `coldStart_firstNavigation_sendsSetBank`

## Change Log

- 2026-06-19: Policy adopted after smoke test — supersedes session-restore `resetLastSyncedBankFromApvts` interim fix (2026-06-19).
