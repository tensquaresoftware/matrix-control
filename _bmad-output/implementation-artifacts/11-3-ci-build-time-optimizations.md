---
organization: Ten Square Software
project: Matrix-Control
title: Story 11.3 — CI Build-Time Optimizations
author: BMad Agent
status: review
epic: 11
story: 3
story_key: 11-3-ci-build-time-optimizations
depends_on: [11-1, 11-2]
blocks: []
implementation_order: 3
correct_course_date: 2026-07-11
baseline_commit: 6f32302
baseline_workflow: .github/workflows/build-and-test.yml
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/sprint-change-proposal-2026-07-11-ci-infrastructure.md
  - implementation-artifacts/11-1-ci-multi-platform-build-and-tests.md
  - implementation-artifacts/11-2-cd-release-pipeline.md
  - implementation-artifacts/deferred-work.md
created: 2026-07-11
updated: 2026-07-11
---

# Story 11.3: CI Build-Time Optimizations

Status: review

<!-- Epic 11 — CI & Release Infrastructure. Flow ergonomics story: faster PR CI without weakening the 3-OS merge gate. Does NOT modify release.yml (Story 11.2). -->

## Story

As a contributor,
I want pull-request CI to finish faster without weakening the cross-platform merge gate,
So that the dev → PR → review → merge loop stays practical on a solo maintainer workflow.

## Context

**Observed baseline (2026-07-11):** `Build and Test` wall clock **~12 min** on a typical code PR (3 parallel matrix legs; **Windows ~12 min** sets wall clock; macOS ~5 min; Linux ~9 min). Story **11.1** runs full Debug **plugin + tests** on every PR; JUCE source is cached but **compiled objects are not**.

**Trigger:** Guillaume feedback after Story 7-5 PR — waiting on CI slows the BMad loop (`dev-story → PR → CI → code-review → merge`).

**Post-11.2 state:** `build-and-test.yml` now has a **`release-script-tests`** job (pytest on `Scripts/release/tests/`) that must run before the matrix. Branch protection on `main` requires **four** status checks (1 + 3 matrix legs) with **truncated names** (~100 chars) — fragile when matrix `test_binary` paths change.

**Story 11.2 explicitly scoped out** merging release workflow into 11.1 — optimize **Debug PR CI only**; leave `release.yml` untouched.

**Planning references:**
- `_bmad-output/planning-artifacts/epics.md` — Epic 11 (extend with Story 11.3)
- `_bmad-output/implementation-artifacts/deferred-work.md` — 11-1 review: hard-coded `test_binary` paths
- Luthier Story 10.1 pattern (matrix CI) — same repo family; no Luthier 10.3 equivalent documented

## Acceptance Criteria

### AC1 — Merge gate preserved

**Given** a PR that changes compiled sources (`Source/`, `Tests/`, `CMakeLists.txt`, `.github/workflows/`)  
**When** the PR is marked **ready for review** (or equivalent “full CI” trigger — see Dev Notes)  
**Then** macOS, Windows, and Linux Debug builds + `Matrix-Control_Tests` still run  
**And** merge to `main` remains blocked until all required checks pass (branch protection intact)

### AC2 — Measurable improvement

**Given** a typical Core/GUI code PR (not docs-only)  
**When** CI runs in the **fast path** (draft / WIP pushes — see Lever B)  
**Then** wall-clock time improves meaningfully vs ~12 min baseline — target **≤8 min** for the common WIP loop, or document measured result + rationale if free runners cannot reach target

**And** Dev Agent Record includes **before/after timings** (cold + warm) for at least one test PR

### AC3 — At least two optimization levers

**Given** story completion  
**Then** at least **two** levers below are implemented (or one lever + measurement proving sufficient gain):

| ID | Lever | Intent | Est. gain |
|----|-------|--------|-----------|
| **A** | ccache / sccache | Cache compiled objects between runs (JUCE cache alone insufficient) | −30–50% warm runs |
| **B** | Draft PR = fast tier | macOS-only (or macOS + `release-script-tests`) on draft/`synchronize`; full 3-OS when PR ready for review or label `ci-full` | ~5 min during active dev |
| **C** | Path filters | Skip workflow when diff is docs/BMad-only (`_bmad-output/`, `Documentation/`, `*.md` only — **not** `CONTRIBUTING.md` if workflow section changes) | Skip entirely |
| **D** | PR builds tests only | `--target Matrix-Control_Tests` on PR fast tier; full `Matrix-Control` + tests on full tier / `push` to `main` | −20–30% compile |
| **E** | CMake configure cache | Reuse `CMakeCache.txt` + build tree keyed by preset + lockfile hash | Moderate warm runs |
| **F** | **`ci-success` aggregate job** | Final job `needs:` all legs; single stable check name for branch protection | Ops stability (no functional speed gain) |

**Recommended minimum for v1:** **B + F** (flow win + branch protection hygiene). Add **D** on fast tier if still short of target.

### AC4 — Branch protection alignment

**Given** Lever F is implemented  
**When** `main` branch protection is updated  
**Then** required status checks use **`release-script-tests`** + **`ci-success`** (or one combined gate — document choice)  
**And** truncated matrix job names are **removed** from required checks (or documented as optional if GitHub ruleset limits prevent full migration)

### AC5 — Documentation

**Given** the workflow is merged  
**When** a contributor reads **`CONTRIBUTING.md`** § Continuous Integration  
**Then** CI **tiers** are documented: what runs on draft vs ready-for-review vs push to `main`; how to request full matrix (`ci-full` label or mark ready); expected durations  
**And** solo workflow tip: code-review may run in parallel with CI; merge only when Checks are green

### AC6 — No regression

**Given** changes merge to `main`  
**When** push CI runs  
**Then** full 3-OS matrix + `release-script-tests` pass  
**And** `release.yml` is **unchanged** by this story (AC regression guard)

### AC7 — Tests / validation

**Given** story completion  
**Then** workflow YAML structure tests extended (or new) under `Scripts/release/tests/` or dedicated `Tests/CI/` — assert tier triggers, `ci-success` job graph, path filter rules  
**And** existing `Scripts/release/tests/` and `Matrix-Control_Tests` still pass locally

## Tasks / Subtasks

- [x] Record baseline per job in Dev Agent Record (reuse PR #21 / #22 numbers; re-measure after 11-2 `release-script-tests` job)
- [x] Implement Lever **B** — draft/fast vs full matrix split in `build-and-test.yml` (AC: 1, 2, 3)
- [x] Implement Lever **F** — `ci-success` job + update `main` branch protection via `gh api` or documented manual steps (AC: 3, 4)
- [ ] (Optional) Implement Lever **D** on fast tier — tests-only target (AC: 2, 3)
- [ ] (Optional) Implement Lever **C** — path filters for docs-only PRs (AC: 3)
- [ ] (Optional) Implement Lever **A** or **E** — compile/configure cache (AC: 2, 3)
- [x] Extend workflow structure tests (AC: 7)
- [x] Update `CONTRIBUTING.md` CI section (AC: 5)
- [ ] Validate on a test PR: cold + warm run; capture timings in Dev Agent Record (AC: 2) — **Flow B: pending single push post code-review**
- [x] Resolve or defer 11-1 review item: hard-coded `test_binary` paths (consider `ctest` or `cmake --build --target run_tests` if added) — **deferred**: paths centralized in `resolve-ci-tier` JSON; `ctest` out of scope for v1

### Review Findings

- [x] [Review][Decision] AC2 — valider les timings avant de clôturer la story — **reporté au premier push PR** (Flow B) ; story reste `review` jusqu’aux mesures cold/warm dans Dev Agent Record.
- [x] [Review][Decision] AC4 — moment de la migration branch protection — **après premier run vert sur la PR 11-3** (ordre : push → CI verte → `gh api` → merge).
- [x] [Review][Decision] AC7 — profondeur des tests tier — **reporté v1** ; `gh pr view` + `jq` dans le workflow couvre re-run stale et détection label ; tests structurels suffisants pour ce sprint.
- [x] [Review][Patch] Trigger `labeled` manquant — ajouté `labeled` + `unlabeled` [`.github/workflows/build-and-test.yml:21`]
- [x] [Review][Patch] Re-run workflow avec payload stale — `resolve-ci-tier` lit l’état live via `gh pr view` [`.github/workflows/build-and-test.yml:47-71`]
- [x] [Review][Patch] Détection label `ci-full` par grep fragile — remplacé par `jq` exact match [`.github/workflows/build-and-test.yml:64`]
- [x] [Review][Defer] PRs de fork — l’auteur ne peut pas ajouter `ci-full` sans maintainer [`.github/workflows/build-and-test.yml:64`] — deferred, limitation GitHub Actions

## Dev Notes

### Current workflow (baseline — after Story 11.2)

File: `.github/workflows/build-and-test.yml`

| Job | Runner | Role |
|-----|--------|------|
| `release-script-tests` | `ubuntu-latest` | `pytest Scripts/release/tests/` (~7 s) |
| `build-and-test` | matrix ×3 | JUCE cache → configure Debug preset → build plugin + tests → run binary |

Matrix `test_binary` paths are **hard-coded** per OS (11-1 review defer). Changing CMake output layout breaks CI **and** branch protection check names.

### Baseline timings (reference)

| Runner | Duration (PR #21, pre-11.2) | Notes |
|--------|----------------------------|-------|
| macos-latest | ~5 min 17 s | Ninja ARM Debug |
| ubuntu-latest | ~9 min 01 s | includes `apt-get install` |
| windows-latest | ~12 min 23 s | VS 2026 Debug — **wall clock** |

Add ~7 s for `release-script-tests` after 11.2.

### Recommended implementation order

**Phase 1 (required):** Lever **B** + **F**
- Split `pull_request` triggers: `synchronize` / `opened` on **draft** → macOS-only matrix (still run `release-script-tests` first).
- `ready_for_review` or label `ci-full` → full 3-OS matrix (current behaviour).
- `push` to `main` → always full matrix (post-merge gate).
- Add job `ci-success` with `needs: [release-script-tests, build-and-test]` (or conditional needs pattern) — **single check** for merge.
- Update branch protection contexts (document exact `gh api` payload in CONTRIBUTING or story completion notes).

**Phase 2 (if AC2 target missed):** Lever **D** on fast tier — `--target Matrix-Control_Tests` only; plugin target on full tier + main push.

**Phase 3:** Lever **A** — `ccache` on Linux (`apt install ccache`, `CMAKE_CXX_COMPILER_LAUNCHER`); `sccache` on Windows; macOS `ccache` or Xcode derived-data cache (higher setup cost).

**Phase 4:** Lever **C** — `paths` / `paths-ignore` on `pull_request`; ensure workflow still runs when `.github/workflows/build-and-test.yml` changes.

### Lever B — GitHub draft PR mechanics

```yaml
on:
  pull_request:
    types: [opened, synchronize, reopened, ready_for_review]
```

Use `if:` on matrix jobs:
- `github.event.pull_request.draft == true` → macOS leg only
- `github.event.pull_request.draft == false` OR `contains(github.event.pull_request.labels.*.name, 'ci-full')` → full matrix

**Caveat:** marking PR ready triggers `ready_for_review` — full matrix runs once before merge. Document for Guillaume.

### Lever F — Branch protection (2026-07-11 state)

Currently required on `main` (set during Story 11-2 review cleanup):
- `release-script-tests`
- 3× truncated `build-and-test (...)` names

**Target after 11.3:**
- `release-script-tests`
- `ci-success`

### Architecture compliance

- **Infrastructure-only** — no `Source/Core/` or `Source/GUI/` changes required.
- **Do not** modify `.github/workflows/release.yml`.
- **Do not** weaken `COPY_TO_*` OFF flags or `MATRIX_BUILD_TESTS=ON`.
- Version SSOT / plugin code untouched.

### Previous story intelligence (11.1)

- Reuse JUCE `8.0.12` cache key pattern: `juce-${{ env.JUCE_VERSION }}-${{ runner.os }}`.
- Linux apt package list — copy verbatim if duplicating jobs; do not trim.
- `fail-fast: false` on full matrix.
- `shell: bash` on Windows for configure/build steps.
- `test -f` preflight before test binary execute.
- Debug CI must stay fast vs Release (`release.yml`) — do not merge workflows.

### Previous story intelligence (11.2)

- `release-script-tests` is a **merge gate** — keep it on all tiers (fast + full).
- Release workflow is tag-only — optimizations here do not affect CD.
- `Scripts/release/tests/test_prepare_release.py` validates workflow YAML — extend for tier logic instead of duplicating assertions in raw shell.

### Git intelligence (recent)

- `6f32302` / PR #22 — Story 11-2 on `feature/11-2-cd-release-pipeline` (release.yml + release-script-tests job).
- `ab0a263` — Story 11-1 post-review fixes (may ride PR #22 to `main`).
- Branch protection enabled on `main` 2026-07-11 — any workflow job rename **must** update protection contexts (Lever F motivation).

### Latest tech information

- **GitHub Actions path filters:** `paths-ignore` on `pull_request` skips entire workflow — use narrowly; compiled code paths must never be ignored.
- **ccache + CMake:** `CMAKE_CXX_COMPILER_LAUNCHER=ccache` (Ninja/Linux); Windows often uses `sccache` with MSVC cl.exe wrapper.
- **Draft PR CI:** Supported natively via `pull_request.draft` and `ready_for_review` event (GitHub Docs).
- **Required checks:** Rulesets API can require "all workflows" but granular control via named `ci-success` job is simpler for solo repos.

### Project context reference

- Chat/docs conventions: `CONVENTIONS.md` §1; BMad artifacts English.
- Builds in `Builds/` per platform — preset paths in `CMakeUserPresets.json`.
- CI is merge gate for Epic 7+ stories (7-5, 7-6, 7-8 recommended).

### Out of scope

- Self-hosted / larger paid runners
- Optimizing `release.yml` (Release + signing legs)
- Replacing JUCE `UnitTest` framework
- Epic 11 retrospective (separate workflow)

### Open decisions (resolve during dev-story)

1. **Fast tier scope:** macOS-only vs macOS + Linux (Linux cheaper than Windows for compile sanity)?
2. **Full matrix trigger:** `ready_for_review` only vs additional `ci-full` label for re-runs without toggling draft?
3. **Lever D default:** tests-only on fast tier always, or only when diff is `Source/Core/` + `Tests/` without `Source/GUI/`?
4. **Branch protection update:** automated in workflow docs vs one-time `gh api` run documented in completion notes?

## Dev Agent Record

### Agent Model Used

Composer (dev-story Flow B)

### Debug Log References

- `python3 -m pytest Scripts/release/tests/ -q` — 22 passed
- macOS `Matrix-Control_Tests` — existing binary ran green (JUCE not at `/Applications/JUCE`; configure skipped)

### Baseline timings (before 11.3, PR #21 + 11.2 overhead)

| Job | Duration | Notes |
|-----|----------|-------|
| `release-script-tests` | ~7 s | Added in 11.2 |
| macOS matrix leg | ~5 min 17 s | PR #21 |
| Linux matrix leg | ~9 min 01 s | PR #21 |
| Windows matrix leg | ~12 min 23 s | PR #21 — **wall clock** |
| **Full PR total** | **~12 min** | 3 parallel legs |

**After 11.3 (expected, not yet measured on GitHub):**

| Tier | Expected |
|------|----------|
| Draft PR (fast) | ~5 min (macOS + release-script-tests) |
| Ready for review / push `main` | ~12 min (unchanged full gate) |

After timings: capture on the single Flow B PR push (cold run ready-for-review; optional warm re-run via `ci-full` label).

### Completion Notes List

- Lever **B**: `resolve-ci-tier` job outputs dynamic matrix — macOS-only when `pull_request` + draft + not `ready_for_review` + no `ci-full` label; full 3-OS otherwise (including all `push` to `main`).
- Lever **F**: `ci-success` aggregate job; branch protection migration documented in `CONTRIBUTING.md` (`gh api` payload).
- Optional levers A/C/D/E skipped for v1 (B alone meets fast-tier target).
- `test_binary` paths consolidated in `resolve-ci-tier` shell (single SSOT within workflow); still hard-coded strings — `ctest` deferred.
- **Flow B:** no push yet; AC2 after-timings pending PR validation.
- **Code review (2026-07-11):** patches applied — `labeled`/`unlabeled` triggers, live PR state via `gh pr view`, `jq` exact `ci-full` match; AC2/AC4 deferred per review decisions.

### File List

- `.github/workflows/build-and-test.yml` (modified)
- `Scripts/release/tests/test_build_and_test_workflow.py` (added)
- `CONTRIBUTING.md` (modified)
- `_bmad-output/implementation-artifacts/11-3-ci-build-time-optimizations.md` (modified)
- `_bmad-output/implementation-artifacts/sprint-status.yaml` (modified)

### Change Log

- 2026-07-11 — Story 11.3 created: CI build-time optimizations + `ci-success` branch protection lever after 7-5 PR ~12 min observation and main protection setup.
- 2026-07-11 — Implemented Lever B (draft fast tier) + F (`ci-success`); workflow structure tests; CONTRIBUTING CI tiers (Flow B, no push).

## References

- [Source: `_bmad-output/planning-artifacts/epics.md` — Epic 11]
- [Source: `_bmad-output/planning-artifacts/sprint-change-proposal-2026-07-11-ci-infrastructure.md`]
- [Source: `_bmad-output/implementation-artifacts/11-1-ci-multi-platform-build-and-tests.md`]
- [Source: `_bmad-output/implementation-artifacts/11-2-cd-release-pipeline.md` — § CI workflow relationship]
- [Source: `.github/workflows/build-and-test.yml`]
- [Source: `_bmad-output/implementation-artifacts/deferred-work.md` — 11-1 hard-coded test_binary]
- [External: GitHub — path filters, draft PRs, required status checks]

## Change Log

- 2026-07-11 — Story 11.3 created: CI build-time optimizations + `ci-success` branch protection lever after 7-5 PR ~12 min observation and main protection setup.
