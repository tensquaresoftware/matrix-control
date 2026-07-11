---
epic: 11
story: 1
story_key: 11-1-ci-multi-platform-build-and-tests
depends_on: [0-4, 0-5]
blocks: [7-5, 7-6, 7-8]
implementation_order: 1
correct_course_date: 2026-07-11
baseline_commit: 177546d3ed457ca441c501b25e70e7847119eca1
baseline_workflow: none
---

# Story 11.1: CI Multi-Platform (build + unit tests matrix)

Status: done

<!-- Epic 11 — CI & Release Infrastructure. Sprint Change Proposal 2026-07-11. -->

## Story

As a contributor,
I want every push and pull request to build Matrix-Control and run Core unit tests on macOS, Windows, and Linux,
So that cross-platform compile regressions and logic failures are caught before merge.

## Context

**Correct Course 2026-07-11 (approved pending):** Matrix-Control has no GitHub Actions workflows despite PRD NFR-1. README incorrectly claims CI exists. On 2026-07-10, Linux and Windows compile failures were found while macOS built clean (`1736e18`, `222f21f`).

**Key insight:** A macOS-only CI running unit tests would **not** have caught those failures — they are **compile-time** platform differences (MSVC vs Clang, linkage, casts). CI must include a **three-OS build matrix**, not macOS-only tests.

**Luthier reference:** Story 10.1 (`pytest.yml` matrix) — same pattern, different stack (CMake + JUCE 8.0.12 + `Matrix-Control_Tests`).

**Planning references:**
- `_bmad-output/planning-artifacts/sprint-change-proposal-2026-07-11-ci-infrastructure.md`
- PRD NFR-1, SM-6, D-047-R
- `CMakeUserPresets.json` — adapt for CI (no personal artefact paths)
- Luthier `.github/workflows/pytest.yml` — matrix structure reference

### CI constraints

| Concern | CI approach |
|---------|-------------|
| JUCE | Checkout tag `8.0.12` → `JUCE_DIR` env |
| Artefact copy | `-DUSER_COPY_TO_SYSTEM_FOLDERS=OFF -DUSER_COPY_TO_ARTEFACTS_DIR=OFF` (propagates to `COPY_TO_*` via CMake) |
| Windows VS | GitHub `windows-latest` → `windows-debug` preset (VS 2026; `windows-debug-vs2022` retained for local legacy) |
| macOS | `macos-latest` = Apple Silicon → `macos-debug-arm64` |
| Linux | apt deps for JUCE (document in workflow + CONTRIBUTING) |
| Tests | Build + run `Matrix-Control_Tests` — no `--unit-tests` plugin flag |
| Hardware | None — existing unit tests are headless |

## Acceptance Criteria

### AC1 — Triggers

**Given** a push or pull request targeting **`main`**  
**When** GitHub Actions runs  
**Then** `.github/workflows/build-and-test.yml` executes

### AC2 — Three-OS matrix

**Given** the workflow  
**When** it runs  
**Then** jobs execute on **`ubuntu-latest`**, **`windows-latest`**, and **`macos-latest`** via `strategy.matrix`  
**And** each leg checks out JUCE, configures with `MATRIX_BUILD_TESTS=ON`, builds plugin + test targets, runs `Matrix-Control_Tests`

### AC3 — Headless / no side effects

**Given** any matrix leg  
**When** build completes  
**Then** no plugin copy to system folders or Dropbox artefact dirs  
**And** no MIDI hardware required

### AC4 — Platform toolchain

**Given** each matrix leg  
**When** configuring CMake  
**Then** platform-appropriate generator and preset are used (document choices in workflow comments)  
**And** Linux installs required apt packages before configure

### AC5 — Documentation

**Given** the workflow is merged to `main`  
**When** a contributor reads **`CONTRIBUTING.md`**  
**Then** CI triggers, matrix legs, and local reproduction commands are documented  
**And** **`README.md`** current-status CI line is accurate

### AC6 — Fail-fast policy

**Given** the matrix strategy  
**When** one leg fails  
**Then** `fail-fast: false` so all OS results are visible in one run (match Luthier 10.1)

## Tasks / Subtasks

- [x] Create `.github/workflows/build-and-test.yml` with 3-OS matrix
- [x] JUCE 8.0.12 checkout + cache strategy
- [x] Per-OS configure/build/test steps
- [x] Verify green on all three legs on `main`
- [x] Update `CONTRIBUTING.md` CI section
- [x] Fix `README.md` CI status line
- [x] Optional: add CI preset overrides or documented `-D` flags

### Review Findings

- [x] [Review][Decision] **Preset Windows : VS 2026 vs VS 2022** — Résolu : garder `windows-debug` (VS 2026) ; mettre à jour spec/constraints pour refléter le choix documenté dans le workflow.
- [x] [Review][Decision] **Seams de test dans le code Core de production** — Résolu : garder tel quel (seams commentés, simplicité acceptable).
- [x] [Review][Decision] **Tests debounce : flush synchrone vs attente timer** — Résolu : accepter le compromis CI (flush sync, pas de restauration wall-clock).

- [x] [Review][Patch] **Build step Windows sans shell explicite** [`.github/workflows/build-and-test.yml:100`]
- [x] [Review][Patch] **Pas de vérification d'existence du binaire de test** [`.github/workflows/build-and-test.yml:106`]
- [x] [Review][Patch] **Pas de timeout sur le job CI** [`.github/workflows/build-and-test.yml:26`]
- [x] [Review][Patch] **File List story incomplète** [`11-1-ci-multi-platform-build-and-tests.md:133`]
- [x] [Review][Patch] **Spec constraints : preset Windows + noms flags COPY** [`11-1-ci-multi-platform-build-and-tests.md:512`]
- [x] [Review][Patch] **Completion notes : preuve CI PR #20 absente** [`11-1-ci-multi-platform-build-and-tests.md:131`]
- [x] [Review][Patch] **Lien TOC CI pointe vers claude.ai** [`CONTRIBUTING.md:13`]

- [x] [Review][Defer] **Chemins test_binary hard-codés par OS** [`.github/workflows/build-and-test.yml:35-43`] — deferred, pre-existing fragility acceptable while paths stable
- [x] [Review][Defer] **COPY_* CACHE FORCE vs cache stale local** [`CMakeLists.txt:132-133`] — deferred, CI fresh checkout mitigates
- [x] [Review][Defer] **TOC CONTRIBUTING entier en URLs claude.ai** [`CONTRIBUTING.md:9-17`] — deferred, pre-existing
- [x] [Review][Defer] **Thread::sleep(50) dans MidiManagerTests** [`Tests/Unit/MidiManagerTests.cpp:205`] — deferred, pre-existing
- [x] [Review][Defer] **Chemins Dropbox dans CMakeUserPresets** [`CMakeUserPresets.json`] — deferred, copy OFF en CI
- [x] [Review][Defer] **Tests MIDI skip silencieux sans device** [`Tests/Unit/MidiManagerTests.cpp:362`] — deferred, conforme AC3 headless

## Dev Notes

- **Recent fixes to protect:** `ApvtsLogger` Linux cast, router stub linkage, embedded Orbitron font.
- **`deferred-work.md`:** `MidiActivityTrackerTests` pre-existing failure — resolve or skip in CI scope if still red.
- **Story 11.2 (CD):** explicitly out of scope — no release workflow in this story.

## Dev Agent Record

### Implementation Plan

1. Add `.github/workflows/build-and-test.yml` — Luthier 10.1 matrix pattern (`fail-fast: false`), JUCE 8.0.12 checkout with `actions/cache`, per-OS presets documented in workflow header.
2. CI configure flags: `-DMATRIX_BUILD_TESTS=ON -DUSER_COPY_TO_SYSTEM_FOLDERS=OFF -DUSER_COPY_TO_ARTEFACTS_DIR=OFF`.
3. Fix `CMakeLists.txt` so `-DUSER_COPY_*` overrides are honoured (CACHE defaults instead of unconditional `set()`).
4. Fix test target link gap: add `PluginDescriptorsMasterEdit.cpp` to `Matrix-Control_Tests` sources.
5. Document CI in `CONTRIBUTING.md`; update `README.md` status line to reflect CI in place.

### Debug Log

- Local macOS configure with `-DUSER_COPY_*=OFF` was ignored until USER vars changed to CACHE defaults — COPY still ON in cache.
- Test binary path is `Matrix-Control_Tests` (underscore), not `Matrix-Control Tests` (PRODUCT_NAME display string).
- `MidiActivityTrackerTests` passes locally (exit 0); deferred-work note appears stale.

### Completion Notes

- ✅ `.github/workflows/build-and-test.yml` — 3-OS matrix, JUCE cache, Linux apt deps, documented preset choices.
- ✅ `CMakeLists.txt` — USER copy flags respect `-D` overrides; `PluginDescriptorsMasterEdit.cpp` linked into test target.
- ✅ `CONTRIBUTING.md` — Continuous Integration section with matrix table and local reproduction commands.
- ✅ `README.md` — CI status updated to ✅ (multi-platform build + tests on push/PR to `main`).
- ✅ macOS leg verified locally: plugin + `Matrix-Control_Tests` build and run (exit 0).
- ✅ PR #20 CI (2026-07-11): all three matrix legs green on `ubuntu-latest`, `windows-latest`, and `macos-latest` before merge to `main`.

## File List

- `.github/workflows/build-and-test.yml` (added)
- `CMakeLists.txt` (modified)
- `CONTRIBUTING.md` (modified)
- `README.md` (modified)
- `Source/Core/Actions/MutatorActionHandler.h` (modified — test seam)
- `Source/Core/Util/ComboboxPatchSendDebouncer.cpp` (modified — test seam)
- `Source/Core/Util/ComboboxPatchSendDebouncer.h` (modified — test seam)
- `Tests/TestMain.cpp` (modified — GUI initialiser for headless CI)
- `Tests/Unit/ComboboxPatchSendDebouncerTests.cpp` (modified — sync flush)
- `Tests/Unit/MidiManagerTests.cpp` (modified — skip without MIDI device)
- `Tests/Unit/MutatorActionHandlerTests.cpp` (modified — sync flush)
- `_bmad-output/implementation-artifacts/sprint-status.yaml` (modified)
- `_bmad-output/planning-artifacts/epics.md` (modified)
- `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md` (modified)
- `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md` (modified)
- `_bmad-output/planning-artifacts/sprint-change-proposal-2026-07-11-ci-infrastructure.md` (added)

## Change Log

- 2026-07-11 — Story 11.1 implemented: GitHub Actions 3-OS build+test matrix, CI copy-flag fix, test target link fix, CONTRIBUTING/README CI docs.
- 2026-07-11 — Code review: workflow hardening (timeout, bash shell, test binary preflight), story traceability/doc fixes, CONTRIBUTING CI TOC anchor.

## References

- Luthier: `_bmad-output/implementation-artifacts/10-1-ci-multi-platform-pytest-matrix.md`
- Matrix-Control tests: `CMakeLists.txt` L328+
- Presets: `CMakeUserPresets.json`
