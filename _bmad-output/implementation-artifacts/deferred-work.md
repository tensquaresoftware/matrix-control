# Deferred Work

## Deferred from: code review of 1-1-patchmodel-packed-buffer (2026-06-02)

- **`signBitPosition` undefined for `maxValue ≤ 0` or non-`2^n−1` ranges** (`Source/Core/Models/PatchModel.cpp:53-58`) — `jlimit` acts as a safety net so no current descriptor is affected; revisit if a signed descriptor with a non-power-of-two max is ever added.
- **`getChoiceIndex` silently clamps stale/corrupt buffer bytes** (`Source/Core/Models/PatchModel.cpp:40`) — defensive clamping via `jmax`/`jlimit` is correct for the current descriptor set; add `jassert(!descriptor.choices.isEmpty())` if a stricter contract is desired.
