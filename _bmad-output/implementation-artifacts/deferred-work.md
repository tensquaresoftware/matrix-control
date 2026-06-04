# Deferred Work

## Deferred from: code review of 1-3-apvtspatchmapper-round-trip (2026-06-04)

- **`apvtsToBuffer()` O(n) full-buffer syncs per parameter change** (`PluginProcessor.cpp:459`) — intentional for Story 1.3; batching belongs in a future performance story.
- **Silent null skip without assertion** (`ApvtsPatchMapper.cpp:36-38`) — correct guard for synthetic-descriptor test pattern; add `jassert` if stricter contract needed.
- **Thread safety — `bufferToApvts()` / `apvtsToBuffer()` unguarded** (`ApvtsPatchMapper.cpp`) — explicitly scoped to future threading hardening story per spec dev notes.
- **`memset` in Test C bypasses `PatchModel` API** (`ApvtsPatchMapperTests.cpp:190`) — acceptable direct-buffer reset in test; revisit if PatchModel gains write invariants.
- **Descriptor vectors built twice at startup** (`PluginProcessor.cpp:526-534`) — mapper ctor + `buildPatchParameterIdSet()` both call static builders; startup-only, negligible cost.
- **Hardcoded `intDescs[0]` / `[2]` indices in tests** (`ApvtsPatchMapperTests.cpp:132-134`) — confirmed correct by ECH reading `PluginDescriptorsPatchEdit.cpp`; same deferred pattern as Story 1.4.
- **Public `bufferToApvts()` feedback loop risk** (`ApvtsPatchMapper.h:22`) — not called in current code; Story 2.4 caller sites must use `MessageManager::callAsync` per spec thread note.

## Deferred from: code review of 1-4-apvtsmastermapper-round-trip (2026-06-04)

- **Hardcoded vector indices in tests** (`Tests/Unit/ApvtsMasterMapperTests.cpp:130`) — `intDescs[0]` / `intDescs[2]` rely on `buildIntDescriptors()` returning a stable order; tests pass and document expected index-to-parameter mapping; revisit with `std::find_if` if descriptor order ever changes.

## Deferred from: code review of 1-2-mastermodel-packed-buffer (2026-06-03)

- **`safeOffset` no release-mode bounds guard** (`Source/Core/Models/PackedFieldCodec.cpp:22`) — debug-only jassert mirrors the original PatchModel pattern; descriptor offsets are compile-time constants so the risk is low; revisit if runtime-provided offsets are ever introduced.
- **`choices.size() - 1` size_t underflow when choices is empty** (`Source/Core/Models/MasterModel.cpp:30`, same in `PatchModel.cpp`) — pre-existing from Story 1.1; all real descriptors have ≥ 1 choice; add `jassert(!descriptor.choices.isEmpty())` if stricter contract is desired.

## Deferred from: code review of 1-1-patchmodel-packed-buffer (2026-06-02)

- **`signBitPosition` undefined for `maxValue ≤ 0` or non-`2^n−1` ranges** (`Source/Core/Models/PatchModel.cpp:53-58`) — `jlimit` acts as a safety net so no current descriptor is affected; revisit if a signed descriptor with a non-power-of-two max is ever added.
- **`getChoiceIndex` silently clamps stale/corrupt buffer bytes** (`Source/Core/Models/PatchModel.cpp:40`) — defensive clamping via `jmax`/`jlimit` is correct for the current descriptor set; add `jassert(!descriptor.choices.isEmpty())` if a stricter contract is desired.
