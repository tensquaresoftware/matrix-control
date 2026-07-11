# Sprint Change Proposal — Epic 11: CI & Release Infrastructure

**Project:** Matrix-Control  
**Date:** 2026-07-11  
**Author:** Correct Course workflow (Batch mode — PO context pre-supplied)  
**Change signal:** Post-Luthier return; CI/CD absent despite PRD NFR-1 / D-047; cross-platform compile failures discovered locally (2026-07-10)  
**Source registry:** PRD NFR-1, D-047, SM-6, README CI claim, Luthier Epic 10 pattern, recent commits `1736e18`, `222f21f`, `177546d`  
**Scope classification:** **Minor** — one new epic, two stories (11.2 deferred), PRD/epics doc amendments; no rollback

---

## 1. Issue Summary

### 1.1 Problem Statement

Matrix-Control has **no GitHub Actions workflows** (`.github/` is empty) despite README claiming *"CI pipeline are in place"*. The PRD defines an automated test pyramid (NFR-1, D-047) and success metric SM-6 (*"CI Core unit tests green on macOS"*), but **no CI story exists** in `epics.md` or `sprint-status.yaml`.

On **2026-07-10**, Guillaume discovered **two cross-platform compilation blockers** that built cleanly on macOS but failed on Windows and Linux:

| Commit | Platform | Issue type |
|--------|----------|------------|
| `1736e18` | **Linux** | Ambiguous `juce::var` cast in `ApvtsLogger` |
| `222f21f` | **Windows** | Router stub linkage after `PluginDescriptors` split |
| `177546d` | **All** | Orbitron font not embedded — runtime/rendering risk off-macOS |

**A macOS-only CI would not have caught the Linux and Windows compile failures.** Unit tests running only on macOS validate *logic* on one toolchain; they do **not** substitute for a **multi-OS build matrix** that exercises MSVC, GCC/Clang, platform headers, and linker behaviour.

Decision D-047 originally scoped *"macOS CI minimum on Core unit tests"* as a **cost-saving bootstrap**. Guillaume's experience confirms that **compile-time cross-platform drift** is already occurring mid-sprint — the minimum viable CI for this JUCE/C++ project is a **three-OS build + test matrix**, modelled on Luthier Story 10.1.

### 1.2 Trigger Type

- **Post-sprint infrastructure gap** — NFR-1 promised but never storied or implemented.
- **Evidence-driven scope upgrade** — D-047 macOS-only bootstrap superseded by observed Windows/Linux compile failures.

### 1.3 Evidence

| Source | Finding |
|--------|---------|
| `.github/` | Empty — no workflows committed |
| `README.md` L59 | Claims CI in place — **incorrect** |
| `CMakeLists.txt` | `MATRIX_BUILD_TESTS=ON` → `Matrix-Control_Tests` console runner; 25+ unit test translation units |
| `CMakeUserPresets.json` | Presets for macOS ARM, Windows (VS 2026/2022), Linux — CI must adapt paths (no Dropbox artefact dirs) |
| `sprint-status.yaml` | Epics 0–6, R done; Epic 7/8/U in progress — no CI epic |
| Luthier Epic 10 | Proven BMad pattern: `correct-course` → epic + stories → `dev-story` |
| Git log 2026-07-09–10 | Cross-platform fixes merged after manual discovery |

---

## 2. Impact Analysis

### 2.1 Checklist Summary

| Section | Status | Notes |
|---------|--------|-------|
| 1 — Trigger & context | [x] Done | CI absent; compile failures on Win/Linux |
| 2 — Epic impact | [x] Done | New **Epic 11**; existing epics unchanged |
| 3 — Artifact conflicts | [x] Done | PRD SM-6 / D-047 wording; README; epics.md |
| 4 — Path forward | [x] Done | **Direct Adjustment** — Epic 11 / Stories 11.1 (+ 11.2 deferred) |
| 5 — Proposal components | [x] Done | This document + story `11-1-ci-multi-platform-build-and-tests.md` |
| 6 — Final review | [x] Done | **Approved by Guillaume 2026-07-11** |

### 2.2 Epic Impact

| Epic | Status | Impact |
|------|--------|--------|
| Epics 0–6, R | **done** | No rollback; CI validates existing code |
| Epic 7, 8, U | **in-progress** | **Unblocked** — CI becomes merge gate for ongoing work |
| Epic 9, 10 | **backlog** | No change |
| **Epic 11 — CI & Release Infrastructure** | **new — backlog** | Story 11.1: multi-OS build + unit tests; Story 11.2: CD release (deferred) |

**Priority recommendation:** Run Epic 11 Story 11.1 **before** resuming Epic 7 backlog (7-5, 7-6, 7-8) so every subsequent story benefits from the gate.

### 2.3 PRD / MVP Impact

**MVP scope unchanged.** This change **implements** an existing NFR rather than adding features.

| Requirement | Current text | Proposed change |
|-------------|--------------|-----------------|
| **NFR-1** | Unit test pyramid; no hardware in CI | Add: CI runs on **macOS, Windows, Linux** GitHub-hosted runners |
| **SM-6** | CI Core unit tests green **on macOS** | **SM-6:** CI build + Core unit tests green on **macOS, Windows, and Linux** |
| **D-047 follow-up** | macOS CI minimum | **Superseded:** three-OS build matrix minimum; unit tests on all legs |
| **README** | "CI pipeline are in place" | Update when 11.1 done — or mark as in-progress in 11.1 AC |

No FR additions. No user-facing behaviour change.

### 2.4 Architecture Impact

| Area | Impact |
|------|--------|
| **AD-8 / tests layout** | Unchanged — CI invokes existing `Matrix-Control_Tests` target |
| **JUCE_DIR** | CI sets `JUCE_DIR` via checkout (tag **8.0.12**) or `actions/cache` — no GUI dep in Core tests |
| **CMake presets** | CI uses preset names with **overrides**: `COPY_TO_*=OFF`, no personal artefact paths; Windows leg likely **`windows-debug-vs2022`** (GHA runner default) until VS 2026 runners exist |
| **Linux deps** | apt: build-essential, ninja, ALSA, freetype, X11 libs — document in CONTRIBUTING |
| **CD / signing** | Out of scope for 11.1 — Story 11.2 deferred (AU/VST3 codesign, notarisation) |

No architecture-spine invariant changes.

### 2.5 UI/UX Impact

**[N/A]** — infrastructure-only epic.

### 2.6 Technical Impact

| Artifact | Story | Change |
|----------|-------|--------|
| `.github/workflows/build-and-test.yml` | 11.1 | **New** — matrix: `macos-latest`, `windows-latest`, `ubuntu-latest` |
| `CMakeUserPresets.json` or CI env | 11.1 | Optional `ci-*` preset variants or workflow `-D` overrides for headless CI |
| `CONTRIBUTING.md` | 11.1 | CI section: triggers, matrix, local reproduction, JUCE_DIR |
| `README.md` | 11.1 | Fix CI status claim |
| `epics.md` | CC | Epic 11 + stories 11.1, 11.2 |
| `sprint-status.yaml` | CC (on approval) | Epic 11 entries |
| `prd.md` / `.decision-log.md` | CC (proposed) | SM-6, D-047 addendum |
| `deferred-work.md` | Optional | Track 11.2 CD, pip/cmake cache until pain observed |

**Risk:** Medium — JUCE CI setup is heavier than Luthier pytest; first workflow may need iteration on Windows VS version and Linux packages.  
**Effort:** Medium — ~1–2 dev sessions (11.1).  
**Timeline:** Recommended **immediate** — protects Epic 7/8/U resumption.

---

## 3. Recommended Approach

**Selected: Option 1 — Direct Adjustment**

Add **Epic 11** with Story **11.1** (three-OS build + unit tests). Defer Story **11.2** (CD release pipeline) to pre-v1.0.0 release gate — plugin binary signing/notarisation is substantially harder than Luthier PyInstaller zip.

**Rejected:**

| Option | Reason |
|--------|--------|
| Rollback / defer CI | Compile failures already occurred; manual tri-OS builds are the current pain |
| macOS-only CI (D-047 bootstrap) | Insufficient — proven blind to Win/Linux compile errors |
| MVP Review | No scope reduction — implementing existing NFR |
| `bmad-quick-dev` without epic | PO chose Option A — traceability in sprint-status |

---

## 4. Detailed Change Proposals

### 4.1 Epic 11 (new) — `epics.md`

**Section:** Epic List (after Epic U or as infrastructure epic)

**NEW:**

```markdown
### Epic 11: CI & Release Infrastructure
Automated multi-platform build and Core unit tests on GitHub Actions; release pipeline deferred until v1 distribution strategy is fixed.

**FRs covered:** — (implements NFR-1, SM-6)
**Priority:** Immediate (2026-07-11 correct-course)
**Depends on:** Epic 0 (CMake, tests layout)
**Blocks:** confident merge gate for Epics 7, 8, U
**Note:** CD (AU/VST3/Standalone signing) = Story 11.2, deferred pre-v1.0.0.
```

---

### 4.2 Story 11.1 — Multi-Platform Build & Unit Tests (CI)

**File:** `_bmad-output/implementation-artifacts/11-1-ci-multi-platform-build-and-tests.md`

**Story:**

As a contributor,
I want every push and pull request to build Matrix-Control and run Core unit tests on macOS, Windows, and Linux,
So that cross-platform compile regressions and logic failures are caught before merge.

**Acceptance Criteria:**

**AC1 — Triggers**  
**Given** a push or pull request targeting `main`  
**When** GitHub Actions runs  
**Then** `.github/workflows/build-and-test.yml` executes

**AC2 — Three-OS matrix**  
**Given** the workflow  
**When** it runs  
**Then** jobs execute on `macos-latest`, `windows-latest`, and `ubuntu-latest`  
**And** each leg: checkout JUCE 8.0.12, configure CMake with `MATRIX_BUILD_TESTS=ON`, build **plugin target + `Matrix-Control_Tests`**, run test binary  
**And** `COPY_TO_SYSTEM_FOLDERS=OFF` and `COPY_TO_ARTEFACTS_DIR=OFF` in CI

**AC3 — Platform toolchain**  
**Given** each matrix leg  
**When** configuring  
**Then** macOS uses Ninja + `macos-debug-arm64` (or CI-equivalent)  
**And** Windows uses VS 2022 preset (or documented GHA-compatible generator)  
**And** Linux installs documented apt dependencies before configure

**AC4 — No hardware**  
**Given** CI environment  
**When** tests run  
**Then** no MIDI hardware is required; existing headless unit tests pass

**AC5 — Documentation**  
**Given** workflow is merged  
**When** a contributor reads `CONTRIBUTING.md`  
**Then** CI triggers, matrix, and local reproduction steps are documented  
**And** `README.md` CI claim is accurate

**AC6 — Would-have-caught**  
**Given** commits `1736e18` and `222f21f` reverted on a branch  
**When** CI runs on that branch  
**Then** Linux and Windows legs **fail at compile** (documented as validation during story dev — optional regression check)

---

### 4.3 Story 11.2 — CD Release Pipeline (deferred)

**Status:** backlog / deferred  
**Scope:** Tag-triggered GitHub Release with AU/VST3/Standalone artefacts per OS; codesign + macOS notarisation; semver aligned with `project()` version in CMake.  
**Model:** Luthier Story 10.2 — reuse local scripts, add `publish-ci` subcommand pattern.  
**Trigger:** Correct-course or story creation when v1.0.0 release planning starts.

---

### 4.4 PRD — `prd-matrix-control-2026-05-25/prd.md`

**Section: Success Metrics — SM-6**

**OLD:**
> **SM-6:** CI Core unit tests green on macOS for SysEx round-trip, PatchModel packing, ClipboardService, ActionDispatcher routing mocks. Validates NFR-1.

**NEW:**
> **SM-6:** CI build and Core unit tests green on **macOS, Windows, and Linux** for SysEx round-trip, PatchModel packing, ClipboardService, ActionDispatcher routing mocks. Validates NFR-1.

---

### 4.5 Decision Log — D-047 addendum

**NEW entry D-047-R (2026-07-11):**

- **Decision:** **Supersede** macOS-only CI bootstrap → **three-OS build + test matrix** on GitHub Actions.
- **Rationale:** Linux (`juce::var` ambiguity) and Windows (linkage) compile failures occurred while macOS built clean; unit tests on macOS alone would not detect platform-specific compilation errors.
- **Follow-up:** Epic 11 Story 11.1; Story 11.2 for CD deferred.

---

### 4.6 README — `README.md`

**Section: Current status**

**OLD:** `- ✅ Project structure, build system (CMake), and CI pipeline are in place`

**NEW:** `- ✅ Project structure and build system (CMake) are in place`  
**NEW:** `- 🔄 CI pipeline (GitHub Actions, Epic 11) — in progress`

*(Final ✅ when 11.1 done.)*

---

## 5. Implementation Handoff

### 5.1 Scope Classification

**Minor → Moderate boundary:** Single epic, one active story, backlog/doc updates — **Minor** for BMad routing (Developer agent implements 11.1 directly).

### 5.2 Handoff Plan

| Step | Skill / agent | Deliverable |
|------|---------------|-------------|
| 1 | **PO approves** this proposal | Approved sprint-change-proposal |
| 2 | Update `epics.md`, `sprint-status.yaml`, decision log | Backlog reflects Epic 11 |
| 3 | `/bmad-create-story 11-1` | Story file validated |
| 4 | `/bmad-dev-story 11-1` | `.github/workflows/build-and-test.yml`, docs |
| 5 | `/bmad-code-review 11-1` | Review before merge |
| 6 | Resume Epic 7/8/U with CI gate | `/bmad-sprint-status` |

### 5.3 Success Criteria

- [ ] All three matrix legs green on `main`
- [ ] PR to `main` runs CI automatically
- [ ] README and CONTRIBUTING accurate
- [ ] Guillaume no longer needs manual tri-OS compile checks for every change

---

## 6. Approval

**Approved by Guillaume:** 2026-07-11

**Artifacts updated:**
- `sprint-status.yaml` — Epic 11 added; Story 11.1 `ready-for-dev`
- `epics.md` — Epic 11 + Stories 11.1, 11.2
- `prd.md` — SM-6 three-OS wording
- `.decision-log.md` — D-047-R
- `README.md` — CI status corrected
- `11-1-ci-multi-platform-build-and-tests.md` — story file ready

**Next step:** `/bmad-dev-story 11-1` (fresh context recommended)
