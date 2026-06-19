---
organization: Ten Square Software
project: Matrix-Control
title: Story 6.1 — MutationAlgorithm Specification
author: BMad Agent
status: done
baseline_commit: 3b9d4c5
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - planning-artifacts/briefs/brief-Matrix-Control-2026-05-22/vision-input-fr.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - implementation-artifacts/7-4-mutatoractionhandler.md
  - project-context.md
created: 2026-06-19
updated: 2026-06-19
---

# Story 6.1: MutationAlgorithm Specification

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a product owner,
I want Amount/Random and module-toggle mutation rules documented,
so that MUTATE/RETRY implementation has a testable spec (PRD §9 #7, AD-6).

## Acceptance Criteria

1. **Given** FR-30–FR-34 and FR-54–FR-60 **When** the spec is written **Then** a standalone document exists at `_bmad-output/planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/mutation-algorithm-spec.md` with normative rules (MUST/SHOULD/MAY) — not prose-only intent.

2. **And** the spec defines **recipe inputs** read from APVTS at mutation time:
   - `patchMutatorAmount` — int 0–100 (%), default 0
   - `patchMutatorRandom` — int 0–100 (%), default 0
   - Ten module enable booleans (`patchMutatorEnableDco1` … `patchMutatorEnableLfo2`) — default **off** until user enables

3. **And** the spec defines **Amount/Random curves** with explicit formulas for:
   - Continuous (int) parameters — jitter span as a function of `[minValue, maxValue]`, Amount, Random
   - Discrete (choice) parameters — mutation probability and index selection rules
   - Edge cases: Amount=0 or Random=0 → **no parameter changes** (buffer identical except post-algorithm naming in Story 6.3)

4. **And** the spec defines a **module toggle mask** mapping each enable toggle 1:1 to `PluginIDs::PatchEditSection` module `kGroupId`s (10 modules). Disabled modules: all int + choice descriptors under that `parentGroupId` are **byte-identical** copies from input.

5. **And** the spec defines **Matrix Mod inclusion** for packed bytes **104–133** (10 buses × 3 bytes per `ClipboardService` / addendum). Rule must be explicit (see Dev Notes — owner decision on scope gate).

6. **And** the spec lists **excluded fields**: patch name bytes **0–7** (owned by `MutationNaming`, Story 6.3); any parameter with `sysExId == kNoSysExId` outside Mutator recipe scope.

7. **And** the spec includes **≥3 golden test vectors** (input buffer hex or descriptor values + recipe + expected output) suitable for `MutationAlgorithmTests` in Story 6.4 — including at least one case with Matrix Mod bytes changing and one with all toggles off.

8. **And** `architecture.md` AD-6 open item **MutationAlgorithm** is resolved: add cross-reference to `mutation-algorithm-spec.md`; mark item resolved in § Open Architecture Items (strikethrough + link acceptable — item need not be deleted).

9. **And** **Guillaume approves** the spec (sign-off checkbox in this story's Dev Agent Record) **before** Story 6.4 starts — AC from epics is a hard gate.

10. **And** this story delivers **documentation only** — no `Source/Core/Services/PatchMutator/` implementation, no GUI changes, no unit test code (tests belong to Story 6.4+).

## Tasks / Subtasks

- [x] **Draft normative spec document** (AC: #1–#7)
  - [x] Create `mutation-algorithm-spec.md` using section outline in Dev Notes
  - [x] Copy module toggle table and formulas verbatim into spec (adjust only after owner review)
  - [x] Add golden test vectors with reproducible RNG seed policy
  - [x] Document injectable RNG requirement for unit tests (`IRandomSource` or seeded `juce::Random`)

- [x] **Resolve Matrix Mod scope gate** (AC: #5) — owner decision
  - [x] Present Option A (recommended) vs Option B in spec § Matrix Mod
  - [x] Mark chosen option as MUST after Guillaume sign-off — **Option A**

- [x] **Update architecture traceability** (AC: #8)
  - [x] Edit `architecture.md` AD-6 bullet for `MutationAlgorithm` → link to spec file
  - [x] Resolve open item #1 in § Open Architecture Items (strikethrough + link)

- [x] **Owner approval gate** (AC: #9)
  - [x] Walk through spec with Guillaume (SM-7 exploratory session optional)
  - [x] Record approval date + any deltas in Dev Agent Record — Option A, spec OK, 2026-06-19
  - [x] Do **not** mark Story 6.4 `ready-for-dev` until sign-off recorded here — sign-off recorded; 6.4 unblocked for create-story/dev when ready

- [x] **Self-review** (AC: #10)
  - [x] No C++ source changes except optional comment-only stub forbidden
  - [x] Spec uses English; defers MUTATE/RETRY history semantics to addendum (already defined D-083)
  - [x] Spec does not duplicate FR-55/FR-56 I/O — only algorithm transform

### Review Findings

- [x] [Review][Decision] Open Architecture Item — remove vs strikethrough — Resolved 2026-06-19: keep strikethrough; AC #8 and task wording amended to allow resolved-but-visible items.

- [x] [Review][Patch] §6 kNoSysExId contradicts Matrix Mod mutation — Fixed in spec v1.0.1 §6 exception for MatrixModulationSection patch-buffer fields.

- [x] [Review][Patch] Matrix Mod eligibility vs §5.1 parentGroupId filter — Fixed in spec v1.0.1 §5.1 + §7.1 eligibility path.

- [x] [Review][Patch] `apply()` return when active recipe changes nothing — Fixed in spec v1.0.1 §3 + §9.

- [x] [Review][Patch] Wrong section cross-references — Fixed §1, §4.1 in spec v1.0.1.

- [x] [Review][Patch] Appendix B empty; traceability table malformed — Fixed in spec v1.0.1 Appendix B.

- [x] [Review][Patch] GV-02 byte 8 attribution wrong — Fixed in spec v1.0.1 GV-02.

- [x] [Review][Patch] GV-02 test isolation contradicts Option A — Fixed in spec v1.0.1 GV-02 expectations.

- [x] [Review][Patch] Phantom GV-05 in Completion Notes — Fixed to GV-01–GV-04.

- [x] [Review][Patch] Stale DRAFT Dev Notes contradict approved spec — Replaced with approved-spec pointer section.

- [x] [Review][Patch] API name drift mutateBuffer vs apply — Fixed Epic 6 context table.

- [x] [Review][Patch] §3 early-exit intro wording — Fixed in spec v1.0.1 §3.

- [x] [Review][Patch] Change log stale pending sign-off entry — Updated change log + added code review entry.

- [x] [Review][Defer] Golden vectors lack pinned output hex — Owner sign-off 2026-06-19 accepted deferred pinning to Story 6.4 per GV-04 note; AC #7 literal "expected output" not fully met in appendix. — deferred, pre-existing accepted trade-off

## Dev Notes

### What Story 6.1 IS — and what it is NOT

| In scope (6.1) | Out of scope |
|---|---|
| Normative `MutationAlgorithm` specification document | `MutationAlgorithm.cpp` implementation (**6.4** uses spec) |
| Amount/Random curves + module mask + Matrix Mod rules | `MutationHistoryStore`, M/R naming, Defrag (**6.2–6.3, 6.10**) |
| Golden test vectors (documented, not coded) | `PatchMutatorEngine`, SysEx enqueue (**6.4**, Epic 2) |
| Architecture AD-6 cross-reference update | `MutatorActionHandler` wiring (**7.4**) |
| Owner sign-off gate for Story 6.4 | History M/R combobox GUI (**6.6**) |
| | Recipe persistence wiring (**6.12** — IDs exist today) |

**Hard gate:** Epics explicitly block Story **6.4 MUTATE** until this spec is **approved** [Source: `epics.md` Story 6.1 AC; architecture validation table].

### Epic 6 context (cross-story)

Epic 6 delivers Absynth-style mutation session (D-082–D-087). Algorithm spec is the **only** missing PRD §9 #7 item; history UX, naming, EXPORT, Defrag are fully specified in PRD/addendum.

| Story | Relationship to 6.1 |
|---|---|
| **6.2** | Store holds `result` buffers produced by algorithm |
| **6.3** | Sets patch name bytes 0–7 **after** algorithm output |
| **6.4** | **First code consumer** — implements `MutationAlgorithm::apply()` per this spec |
| **6.5** | Same algorithm, different input snapshot (`parentSnapshot`) |
| **7.4** | Handler delegates to engine; must not embed algorithm |

### Recipe inputs — current brownfield state

**Sliders** (`PluginDescriptorsPatchManager.cpp`):

| Property ID | Range | Default | SysEx |
|---|---|---|---|
| `patchMutatorAmount` | 0–100 | 0 | `kNoSysExId` (recipe only) |
| `patchMutatorRandom` | 0–100 | 0 | `kNoSysExId` (recipe only) |

**Module toggles** (`PatchMutatorPanel.cpp` → APVTS state bool via `setProperty`):

| Toggle property ID | UI label | Patch Edit module `kGroupId` |
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

**Pattern to reuse:** `PatchModuleInitService::copyModuleFromInitTemplate` iterates descriptors filtered by `parentGroupId` — same filter for mutation eligibility [Source: `PatchModuleInitService.cpp`]. **Do not** duplicate parameter lists; algorithm MUST use `ApvtsPatchMapper::buildIntDescriptors()` / `buildChoiceDescriptors()` or equivalent descriptor iteration.

**Matrix Mod:** No Mutator toggle in UI. Bytes **104–133** = matrix modulation slice [Source: `ClipboardService` / addendum § Patch Mutator / Story 5.1 AC #3].

### Patch buffer model

- `PatchModel` — 134-byte packed buffer (`SysExConstants::kPatchPackedDataSize`)
- Parameter access — descriptor-driven via `PackedFieldCodec` [Source: `PatchModel.h`, `PackedFieldCodec.h`]
- Int parameters: signed/unsigned per descriptor `minValue`/`maxValue`
- Choice parameters: index 0…`choiceCount-1` mapped to packed byte via descriptor

**Algorithm API shape (for spec § interface — implementation in 6.4):**

```cpp
struct MutationRecipe
{
    int amountPercent;   // 0–100
    int randomPercent;   // 0–100
    // ten booleans aligned with table above
};

class MutationAlgorithm
{
public:
    // Copies input → output, mutates in place per spec; returns false if recipe is no-op
    bool apply(PatchModel& inOut, const MutationRecipe& recipe, IRandomSource& rng) const;
};
```

Engine passes auditioned buffer (MUTATE) or `parentSnapshot` (RETRY) — algorithm is **stateless** [Source: D-083, addendum § MUTATE/RETRY].

### Normative rules — approved spec (v1.0.1)

The authoritative normative document is [`mutation-algorithm-spec.md`](../planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/mutation-algorithm-spec.md) (status: **approved**, Option A, owner sign-off 2026-06-19). Story 6.4 MUST implement that file — not the historical draft notes below this section if any remain from story creation.

Key decisions (for quick reference only):

- Early exit when `amountPercent == 0` OR `randomPercent == 0`; otherwise return `false` if no byte 8–133 changed
- Module toggles gate Patch Edit descriptors only; Matrix Mod bytes 104–133 mutate when `A>0,R>0` (Option A)
- Patch name bytes 0–7 excluded; `MutationNaming` in Story 6.3
- Golden vectors GV-01–GV-04 in spec Appendix A; exact output hex pinned in Story 6.4

### Architecture compliance

- Spec lives under planning artifacts English path [Source: `project-context.md`]
- Resolves architecture open item #1 [Source: `architecture.md` § Open Architecture Items]
- AD-6 component diagram unchanged — `MutationAlgorithm` remains sibling of `MutationHistoryStore`
- Descriptor-only access — **no** parallel SysEx offset tables [Source: AD-1, anti-patterns]
- Core service target path: `Source/Core/Services/PatchMutator/MutationAlgorithm.{h,cpp}` (**Story 6.4**, not 6.1)

### File structure (this story)

```
_bmad-output/planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/
├── architecture.md                    (UPDATE — AD-6 link, resolve open item)
└── mutation-algorithm-spec.md         (NEW — primary deliverable)

_bmad-output/implementation-artifacts/
└── 6-1-mutationalgorithm-specification.md   (this file — story + gate)
```

**No** `Source/` or `Tests/` changes in Story 6.1.

### Testing requirements (documentation only)

Story 6.1 does not add tests. Spec **must** enable Story 6.4 tests:

- `Tests/Unit/MutationAlgorithmTests.cpp` — golden vectors from spec appendix
- Mock `PatchModel` + injectable RNG
- Edge cases: all toggles off, Amount/Random boundaries, Matrix Mod byte isolation

Follow AAA pattern from `ClipboardServiceTests` / `DeviceMemoryLimitsTests`.

### Previous story intelligence

No prior Epic 6 story file exists — Epic 6 starts here.

**Epic 5 / Epic 7 context:**

- `ClipboardService` established module boundaries and bytes 104–133 matrix slice — reuse for Matrix Mod scope [Source: `5-1-clipboardservice-compatibility-matrix.md`]
- `PatchModuleInitService` established `parentGroupId` filtering pattern for module-scoped buffer ops
- Story **7.4** documents engine API; explicitly lists 6.1 spec as prerequisite — keep spec aligned with engine `MutationRecipe` shape

### Git intelligence

| Commit | Relevance |
|---|---|
| `3b9d4c5` | Latest — Epic 7 bank work; no Mutator code yet |
| `55e220c` | Story 7.4 artifact — dependency map for 6.1–6.13 |
| Epic 2/4/5 done | PatchModel, SysEx queue, file services ready for downstream 6.4 |

### Latest tech / framework notes

- **JUCE 8.0.12**, **C++17** — no new dependencies for spec
- `juce::Random::nextInt(range)` for uniform draws; document inclusive bounds
- Parameter ranges already in `PluginDescriptors` — spec references descriptors as SSOT, not duplicated Oberheim tables

### Project context reference

- `project-context.md` — English artifacts, French chat with Guillaume
- `CONVENTIONS.md` — no French in spec body
- PRD §9 #7 — this story closes the Mutator algorithm open question
- D-028, D-059 superseded by D-082–D-088 for UX; algorithm remained open until this story

### References

- [epics.md — Story 6.1, Epic 6 overview]
- [prd.md — FR-30–FR-34, FR-54–FR-60, §9 #7]
- [addendum.md — § Patch Mutator]
- [.decision-log.md — D-028, D-082–D-088]
- [architecture.md — AD-6, open item #1]
- [vision-input-fr.md — §3.4 Patch Mutator original intent]
- [7-4-mutatoractionhandler.md — engine prerequisite map]
- [PatchMutatorPanel.cpp — recipe UI wiring]
- [PluginDescriptorsPatchManager.cpp — Amount/Random ranges]
- [PatchModuleInitService.cpp — module group filtering pattern]

## Dev Agent Record

### Agent Model Used

Claude (Auto / Cursor Agent)

### Debug Log References

- Story 6.1 — documentation-only deliverable; no test suite run (AC #10).

### Completion Notes List

- Created normative spec at `_bmad-output/planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/mutation-algorithm-spec.md` with MUST/SHOULD/MAY rules for recipe inputs, Amount/Random curves, module toggle mask, excluded fields, Matrix Mod bytes 104–133, RNG policy, and Story 6.4 interface contract.
- Four golden test vectors (GV-01–GV-04) in spec Appendix A; input buffer grounded in `InitDefaults` / `PatchInit.syx`.
- Updated `architecture.md`: AD-6 cross-reference, open item #1 struck through, gap analysis updated.
- Owner sign-off 2026-06-19: **Option A** (Matrix Mod always mutates when A>0,R>0); golden vectors accepted; spec approved. Story 6.4 gate cleared.
- Code review 2026-06-19: spec v1.0.1 — §6 kNoSysExId exception, Matrix Mod eligibility path, apply() return rule, GV-02 fixes, Appendix B traceability; story Dev Notes reconciled with approved spec.

### Owner Sign-off (required before Story 6.4)

- [x] Guillaume reviewed `mutation-algorithm-spec.md`
- [x] Matrix Mod option (A / B / C) confirmed — **A**
- [x] Golden test vectors accepted or revised — accepted
- [x] Sign-off date: 2026-06-19

### File List

- `_bmad-output/planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/mutation-algorithm-spec.md` (NEW)
- `_bmad-output/planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md` (UPDATED)
- `_bmad-output/implementation-artifacts/6-1-mutationalgorithm-specification.md` (UPDATED)
- `_bmad-output/implementation-artifacts/sprint-status.yaml` (UPDATED)

## Change Log

- 2026-06-19: Story 6.1 created — MutationAlgorithm specification guide with draft normative rules, module mask table, Matrix Mod decision gate, and Story 6.4 approval gate.
- 2026-06-19: Draft normative spec + architecture traceability delivered; owner sign-off session for Matrix Mod option and AC #9 gate.
- 2026-06-19: Owner sign-off — Option A, spec approved; spec v1.0; story ready for review.
- 2026-06-19: Code review — spec v1.0.1; all patch findings resolved; story done.
