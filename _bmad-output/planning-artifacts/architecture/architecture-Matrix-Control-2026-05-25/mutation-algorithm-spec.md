---
organization: Ten Square Software
project: Matrix-Control
title: MutationAlgorithm Specification
author: BMad Agent
status: approved
version: "1.0.1"
sources:
  - ../../prds/prd-Matrix-Control-2026-05-25/prd.md
  - ../../prds/prd-Matrix-Control-2026-05-25/addendum.md
  - ../../prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - architecture.md
  - ../../../implementation-artifacts/6-1-mutationalgorithm-specification.md
created: 2026-06-19
updated: 2026-06-19
---

# MutationAlgorithm Specification

Normative rules for the Patch Mutator buffer transform (`MutationAlgorithm::apply`). This document closes PRD §9 #7 and architecture AD-6 open item **MutationAlgorithm**.

**Scope:** Algorithm transform only. MUTATE/RETRY history semantics (D-083), naming (Story 6.3), SysEx enqueue (Story 6.4), and GUI wiring (Stories 6.6–6.12, 7.4) are out of scope.

**Status:** `APPROVED` — owner sign-off 2026-06-19 (Story 6.1 AC #9). Matrix Mod scope: **Option A**.

---

## 1. Definitions

| Term | Meaning |
|---|---|
| **Patch buffer** | 134-byte packed PATCH data (`SysExConstants::kPatchPackedDataSize`) |
| **Recipe** | Amount, Random, and ten module-enable booleans read from APVTS at mutation time |
| **Descriptor** | Entry from `ApvtsPatchMapper::buildIntDescriptors()` or `buildChoiceDescriptors()` |
| **Eligible descriptor** | Int or choice descriptor whose `parentGroupId` matches an enabled module toggle (§5), or a `MatrixModulationSection` descriptor with `sysExOffset` ∈ [104, 133] when §7.1 scope is active |
| **No-op recipe** | Recipe for which §3 early-exit applies |

---

## 2. Recipe inputs

The engine MUST build a `MutationRecipe` from APVTS at each MUTATE or RETRY invocation. Values MUST reflect current APVTS state, not cached session defaults.

### 2.1 Amount and Random sliders

| Property ID | Type | Range | Default | SysEx |
|---|---|---|---|---|
| `patchMutatorAmount` | int | 0–100 (percent) | 0 | `kNoSysExId` (recipe only) |
| `patchMutatorRandom` | int | 0–100 (percent) | 0 | `kNoSysExId` (recipe only) |

Descriptor source: `PluginDescriptors::PatchManagerSection::PatchMutatorModule::kIntParameters`.

### 2.2 Module enable toggles

Ten booleans, default **off** until the user enables them in `PatchMutatorPanel`. Stored as APVTS **state** properties (not parameters).

| Toggle property ID | UI label | Patch Edit `kGroupId` |
|---|---|---|
| `patchMutatorEnableDco1` | DCO1 | `patchEditSection.dco1Module` |
| `patchMutatorEnableDco2` | DCO2 | `patchEditSection.dco2Module` |
| `patchMutatorEnableVcfVca` | VCF/VCA | `patchEditSection.vcfVcaModule` |
| `patchMutatorEnableFmTrack` | FM/TRACK | `patchEditSection.fmTrackModule` |
| `patchMutatorEnableRampPortamento` | RAMP/PORT | `patchEditSection.rampPortamentoModule` |
| `patchMutatorEnableEnvelope1` | ENV1 | `patchEditSection.envelope1Module` |
| `patchMutatorEnableEnvelope2` | ENV2 | `patchEditSection.envelope2Module` |
| `patchMutatorEnableEnvelope3` | ENV3 | `patchEditSection.envelope3Module` |
| `patchMutatorEnableLfo1` | LFO1 | `patchEditSection.lfo1Module` |
| `patchMutatorEnableLfo2` | LFO2 | `patchEditSection.lfo2Module` |

Mapping is **1:1** between toggle and `PluginIDs::PatchEditSection::*Module::kGroupId`.

### 2.3 C++ recipe shape (Story 6.4)

```cpp
struct MutationRecipe
{
    int amountPercent;   // 0–100, clamped
    int randomPercent;   // 0–100, clamped
    bool enableDco1;
    bool enableDco2;
    bool enableVcfVca;
    bool enableFmTrack;
    bool enableRampPortamento;
    bool enableEnvelope1;
    bool enableEnvelope2;
    bool enableEnvelope3;
    bool enableLfo1;
    bool enableLfo2;
};
```

---

## 3. Early exit (no-op)

The algorithm MUST return `false` (no mutation applied) when:

- `amountPercent == 0` **OR** `randomPercent == 0` — output buffer MUST be byte-identical to input (all 134 bytes).

When this condition holds, the implementation MUST NOT consume RNG draws.

When `A > 0` and `R > 0`, the algorithm processes all eligible descriptors (§5 module mask + §7 Matrix Mod). After processing, it MUST return `false` if every byte 8–133 is unchanged from input; otherwise MUST return `true`. Bytes 0–7 MUST remain unchanged in all cases.

**Note:** Patch name bytes 0–7 remain unchanged in all cases (§6). Post-algorithm naming (`Mxx`, `Mxx-Ryy`) is owned by `MutationNaming` (Story 6.3).

---

## 4. Amount and Random curves

Let:

```
A = clamp(amountPercent, 0, 100) / 100.0    // float in [0, 1]
R = clamp(randomPercent, 0, 100) / 100.0    // float in [0, 1]
```

**Semantics:**

- **Amount (A)** — maximum mutation depth relative to each parameter's legal range.
- **Random (R)** — scales effective jitter span for int parameters **and** mutation probability for choice parameters.

The algorithm MUST use **uniform** random distribution in v1. Gaussian noise MUST NOT be used.

### 4.1 Int parameters

For each eligible int descriptor with `minValue`, `maxValue`, and current value `oldValue`:

```
range    = maxValue - minValue
halfSpan = (range / 2.0) * A
spread   = round(halfSpan * R)          // integer delta bound; round half-up
if spread == 0: skip this parameter
delta    = uniformRandomInt(rng, -spread, +spread)   // inclusive bounds
newValue = clamp(oldValue + delta, minValue, maxValue)
```

Write `newValue` through `PatchModel` / `PackedFieldCodec` using the descriptor — do not write raw bytes except where §7.2 fallback applies.

**SHOULD:** At `A=100, R=100`, int `spread` reaches half the parameter range — full exploratory mutation intent.

**Example:** `minValue=0, maxValue=63, oldValue=31, A=1.0, R=1.0` → `halfSpan=31.5`, `spread=32`, delta ∈ [−32, +32].

Signed int parameters (e.g. `minValue=-63, maxValue=63`) use the same formula on the full range.

### 4.2 Choice parameters

For each eligible choice descriptor with `choiceCount = choices.size()` and current index `oldIndex`:

```
if choiceCount <= 1: skip
pMutate = A * R                       // probability in [0, 1]
if rng.nextFloat() >= pMutate: skip
newIndex = uniformRandomInt(rng, 0, choiceCount - 1)
```

Write `newIndex` through descriptor API. Re-selecting the current index is allowed.

**SHOULD:** At `A=100, R=100`, `pMutate=1.0` — every eligible choice parameter is re-rolled each mutation.

### 4.3 Descriptor iteration order

The implementation MUST iterate descriptors in stable order:

1. All int descriptors from `ApvtsPatchMapper::buildIntDescriptors()` filtered by eligibility.
2. All choice descriptors from `ApvtsPatchMapper::buildChoiceDescriptors()` filtered by eligibility.

Order within each list MUST match the vector order returned by the builder (same as `PatchModuleInitService::copyModuleFromInitTemplate` filtering pattern).

---

## 5. Module toggle mask

For each enabled module `kGroupId`:

- **MUST** mutate all `IntParameterDescriptor` and `ChoiceParameterDescriptor` entries where `descriptor.parentGroupId == kGroupId`.
- **MUST NOT** mutate descriptors belonging to disabled modules — packed bytes for those parameters MUST remain byte-identical to input.

Disabled modules: every byte touched by that module's descriptors MUST match input exactly.

**VCF/VCA, FM/TRACK, RAMP/PORT** use the same mechanism as DCO/ENV/LFO toggles.

There is no Mutator toggle for MASTER parameters. The algorithm operates on the PATCH buffer only.

### 5.1 Descriptor access pattern

Reuse `ApvtsPatchMapper::buildIntDescriptors()` / `buildChoiceDescriptors()` filtered by `parentGroupId` for Patch Edit module eligibility. Matrix Mod descriptors use a separate eligibility path (§7.1) — they MUST NOT be gated by module toggles. **MUST NOT** duplicate parameter lists or maintain parallel SysEx offset tables (AD-1, NFR-4).

Reference: `PatchModuleInitService::copyModuleFromInitTemplate`.

---

## 6. Excluded fields

| Bytes / field | Rule |
|---|---|
| **0–7** (patch name) | **MUST NOT** be altered by `MutationAlgorithm`. `MutationNaming` writes `Mxx` / `Mxx-Ryy` after mutate (Story 6.3). |
| Bytes **8–133** | Subject to module mask + Matrix Mod rules when eligible. |
| Buffer size | 134 bytes fixed. Bytes outside 0–133 do not exist. |
| MASTER parameters | Out of scope — Mutator operates on PATCH buffer only. |
| Parameters with `sysExId == kNoSysExId` that are **not** packed patch-buffer fields | **MUST NOT** be mutated. Includes `patchMutatorAmount`, `patchMutatorRandom`, Patch Manager UI-only parameters, and standalone widgets. **Exception:** `MatrixModulationSection` int/choice descriptors with `sysExOffset` ∈ [104, 133] remain eligible under §7 even when `sysExId == kNoSysExId` (patch-buffer fields, not APVTS recipe inputs). |

---

## 7. Matrix Mod inclusion (bytes 104–133)

Matrix Mod occupies bytes **104–133** (30 bytes): 10 buses × 3 bytes (source, amount, destination). Same slice as `ClipboardService::kMatrixModSnapshotOffset` / `kMatrixModSnapshotSize`.

| Bus n (0–9) | Source | Amount | Destination |
|---|---|---|---|
| n | `104 + 3n` | `105 + 3n` | `106 + 3n` |

Descriptor source: `PluginDescriptors::MatrixModulationSection` — each bus has int amount (`minValue=-63, maxValue=63`) and choice source/destination parameters with `sysExOffset` in 104–133.

### 7.1 Scope gate — Option A (approved)

**Owner decision (2026-06-19):** Option **A** is normative.

When `A > 0` and `R > 0`, the algorithm **MUST** mutate bytes **104–133** using §4 rules on Matrix Mod descriptors, **independent** of module toggles.

**Eligibility:** When Matrix Mod scope is active, every int or choice descriptor from `PluginDescriptors::MatrixModulationSection` whose `sysExOffset` ∈ [104, 133] is eligible — regardless of `parentGroupId` module-toggle state and regardless of `sysExId == kNoSysExId` (see §6 exception).

| Option | Rule | Status |
|---|---|---|
| **A** | When `A > 0` and `R > 0`, always mutate bytes 104–133 | **MUST** (approved) |
| **B** | Mutate 104–133 only if at least one module toggle is true | Rejected |
| **C** | Never mutate Matrix Mod in v1 | Rejected |

Rationale: Absynth-style mutation affects whole timbre including routing; module toggles remain Patch Edit scoped.

### 7.2 Matrix Mod mutation rules (when scope active)

When Matrix Mod scope is active (`A > 0` and `R > 0` per §7.1):

1. **MUST** prefer `MatrixModulationSection` descriptors whose `sysExOffset` ∈ [104, 133].
2. Apply §4.1 to int amount fields (`minValue`, `maxValue` from descriptor).
3. Apply §4.2 to choice source and destination fields.
4. **MAY** fall back to raw byte jitter (int 0–127, clamp) only for bytes in 104–133 with no matching descriptor — MUST be documented in Story 6.4 if used.

Matrix Mod mutation MUST NOT affect bytes 0–103 except through unrelated module toggles.

---

## 8. Randomness and reproducibility

### 8.1 Production behaviour

- **MUST** use a fresh non-seeded `juce::Random` per MUTATE or RETRY click.
- **RETRY** MUST draw fresh random values from the parent snapshot (FR-56, D-083, UJ-4).
- Algorithm **MUST NOT** read hidden global or static random state.

### 8.2 Injectable RNG for tests

```cpp
struct IRandomSource
{
    virtual ~IRandomSource() = default;
    virtual float nextFloat() = 0;              // [0, 1)
    virtual int nextInt(int rangeSize) = 0;     // [0, rangeSize)
};

// uniformRandomInt(rng, lo, hi) = lo + rng.nextInt(hi - lo + 1)
```

Unit tests **MUST** inject deterministic RNG. `MutationAlgorithm::apply` MUST accept `IRandomSource&` (or equivalent) — no hard-coded `juce::Random` inside the algorithm class.

### 8.3 Golden-vector seed policy

Golden vectors in Appendix A use `juce::Random::setSeed(uint32)` before each vector. Seeds are documented per vector. Story 6.4 `MutationAlgorithmTests` MUST reproduce appendix outputs using the same seed policy and `InitDefaults` input buffer.

---

## 9. Interface contract (Story 6.4)

Target path: `Source/Core/Services/PatchMutator/MutationAlgorithm.{h,cpp}`

```cpp
class MutationAlgorithm
{
public:
    // Copies input → working buffer, mutates per this spec.
    // Returns false if no-op (§3) or no byte 8–133 changed; true if any byte 8–133 changed.
    // Bytes 0–7 always unchanged.
    bool apply(PatchModel& inOut, const MutationRecipe& recipe, IRandomSource& rng) const;
};
```

- **Stateless** — no history, naming, or SysEx side effects.
- Input may be audition buffer (MUTATE) or `parentSnapshot` (RETRY) — same transform.
- Caller owns buffer copy semantics for history store (Story 6.2).

---

## 10. Relationship to other stories

| Story | Relationship |
|---|---|
| **6.2** | Stores `result` buffers produced by this algorithm |
| **6.3** | Writes patch name bytes 0–7 **after** algorithm output |
| **6.4** | First code consumer — implements this spec |
| **6.5** | Same algorithm, input = `parentSnapshot` |
| **7.4** | Handler delegates to engine; MUST NOT embed algorithm |

MUTATE/RETRY I/O semantics remain in addendum (D-083). This spec defines **only** the buffer transform.

---

## Appendix A — Golden test vectors

Input buffer for all vectors: **`InitDefaults::patchData()`** — grounded in `Tests/Fixtures/Init/PatchInit.syx` (134 bytes). Patch name bytes 0–7 = `0x20` × 8 (space padding).

Full input hex (134 bytes):

```
20 20 20 20 20 20 20 20 01 00 1F 1F 03 02 00 1F
18 03 01 02 1F 00 00 02 00 00 37 00 00 02 00 3F
00 00 00 28 00 00 00 00 09 00 1E 00 00 00 00 09
00 00 00 00 0A 32 0A 28 00 00 00 00 00 0A 32 0A
28 00 00 00 00 00 14 00 14 28 00 00 09 00 0F 1F
2F 3F 00 00 00 00 00 00 00 00 2A 00 00 3F 3F 3F
3F 3F 3F 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00
```

RNG: `juce::Random rng; rng.setSeed(seed);` before `apply`.

Matrix Mod scope: **Option A** (approved).

---

### GV-01 — Global no-op (Amount or Random zero)

| Field | Value |
|---|---|
| **Scenario** | Early exit §3 |
| **Recipe** | `amountPercent=0`, `randomPercent=50`, any toggles |
| **Seed** | (not consumed) |
| **Expectation** | Output **identical** to input — all 134 bytes equal |
| **`apply` return** | `false` |

Alternate: `amountPercent=50`, `randomPercent=0` — same expectation.

---

### GV-02 — DCO1 module only

| Field | Value |
|---|---|
| **Scenario** | Single enabled module; full Amount/Random |
| **Recipe** | `A=100`, `R=100`, only `enableDco1=true`, all others false |
| **Seed** | `0x6D757461` |
| **Expectation** | Bytes 0–7 unchanged. At least one byte in DCO1 offsets {9, 10, 11, 13, 25, 86, 87} MUST differ from input. Bytes for disabled modules (including DCO2) MUST match input. Byte 8 (RampPortamento) MUST match input when `enableRampPortamento=false`. Under Option A, bytes 104–133 MAY also change — assert DCO1 offsets separately from Matrix Mod slice. |
| **`apply` return** | `true` if any byte 8–133 changed; `false` only if entire buffer 8–133 unchanged |

**Representative DCO1 offsets** (from `PluginDescriptorsPatchEdit.cpp`):

| Offset | Parameter | Type |
|---|---|---|
| 9 | DCO1 Frequency | int 0–63 |
| 10 | DCO1 Wave Shape | int 0–63 |
| 11 | DCO1 Pulse Width | int 0–63 |
| 13 | DCO1 Wave Select | choice (4) |
| 25 | DCO1 Sync | choice (4) |

Verification: compare output vs input; at least one byte in {9, 10, 11, 13, 25, 86, 87} MUST differ; byte 8 MUST match input (RampPortamento, not DCO1); bytes 0–7 MUST match.

---

### GV-03 — Matrix Mod only (all module toggles off)

| Field | Value |
|---|---|
| **Scenario** | Matrix Mod mutation with no Patch Edit modules enabled |
| **Recipe** | `A=50`, `R=100`, all module toggles **false** |
| **Seed** | `0x4D61746D` |
| **Scope** | Option **A** — Matrix Mod mutates even with all module toggles off |
| **Expectation** | Bytes 0–7 unchanged. Bytes 8–103 unchanged. At least one byte in 104–133 MUST differ from input (init template has zeros in 104–133). |
| **`apply` return** | `true` |

Init Matrix Mod slice (104–115): `00 00 00 00 00 00 00 00 00 00 00 00`.

---

### GV-04 — Deterministic choice re-roll

| Field | Value |
|---|---|
| **Scenario** | Single choice parameter, fixed seed |
| **Recipe** | `A=100`, `R=100`, only `enableDco1=true` |
| **Seed** | `0x43484F31` |
| **Target** | DCO1 Wave Select (`sysExOffset=13`, 4 choices, init index encoded in byte 13 = `0x02`) |
| **Expectation** | With seed `0x43484F31`, after processing prior DCO1 int/choice descriptors in iteration order, byte 13 MUST equal the deterministically computed choice index. Story 6.4 tests MUST capture the full output byte 13 value on first test run and pin it in this appendix after verification. |
| **`apply` return** | `true` |

> **Implementation note:** Pin exact expected byte values in Story 6.4 when implementing `SeededRandom` against `juce::Random`. The seed policy and iteration order in §4.3/§8.3 are normative; numeric outputs are verified by test, not hand-simulated.

---

## Revision history

| Date | Version | Change |
|---|---|---|
| 2026-06-19 | 1.0-draft | Initial normative spec — Story 6.1 |
| 2026-06-19 | 1.0 | Owner sign-off: Option A confirmed; early exit §3 updated; approved for Story 6.4 |
| 2026-06-19 | 1.0.1 | Code review fixes: §6 kNoSysExId exception, §7 eligibility path, apply() return rule, cross-refs, GV-02, Appendix B |

---

## Appendix B — Traceability

| Requirement | Spec section |
|---|---|
| FR-30–FR-34 | §2 recipe, §9 interface |
| FR-54–FR-60 | §8.1 RETRY randomness; history/naming deferred |
| PRD §9 #7 | This document |
| AD-6 | `architecture.md` cross-reference |
| D-083 | §8.1 stateless transform; MUTATE/RETRY input selection in engine |
| Story 5.1 | §7 byte layout 104–133 |
| Story 6.3 | §6 name exclusion |
| Story 6.4 | §9 implementation target |
| Story 7.4 | §10 handler must not embed algorithm |
