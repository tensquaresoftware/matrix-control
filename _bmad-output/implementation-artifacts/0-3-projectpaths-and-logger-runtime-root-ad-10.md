---
story_key: 0-3-projectpaths-and-logger-runtime-root-ad-10
epic: 0
story: 3
status: done
baseline_commit: 2921c4f
---

# Story 0.3: ProjectPaths and Logger Runtime Root (AD-10)

Status: done

## Story

As a developer,
I want log paths resolved at runtime relative to the repo root,
So that moving the project on disk no longer writes logs to a stale absolute path (D-093, D-094).

## Acceptance Criteria

1. **Given** Story 0.1 complete **When** `ProjectPaths` discovers the Matrix-Control root **Then** loggers use `{root}/logs/midi/` and `{root}/logs/apvts/`.
2. **And** CMake `MATRIX_CONTROL_PROJECT_ROOT` compile define is removed.
3. **And** a rebuild after moving the repo (without reconfigure) writes logs under the new `{root}/logs/` (runtime discovery, not compile-time path).

## Tasks / Subtasks

- [x] Add `src/Shared/ProjectPaths.h/.cpp` — env override, executable/CWD walk-up, CMakeLists marker (AC: #1)
- [x] Refactor `MidiLogger` / `ApvtsLogger` default log directories (AC: #1)
- [x] Remove `MATRIX_CONTROL_PROJECT_ROOT` from `CMakeLists.txt` (AC: #2)
- [x] Fallback to user Application Data + one-time warning when repo root not found (AD-10)
- [x] Update sprint-status.yaml (AC: #3)

## Dev Notes

- Discovery order: walk-up from executable parent → walk from CWD → `MATRIX_CONTROL_PROJECT_ROOT` env (CI only) → user data fallback.
- Marker: `.matrix-control-root` or `CMakeLists.txt` containing `project(Matrix-Control`.
- Standalone dev runs resolve repo root via walk-up from `.app` bundle; VST3 in system folder uses fallback unless env is set.

## Dev Agent Record

### Completion Notes List

- `ProjectPaths` SSOT under `src/Shared/`; loggers no longer depend on CMake compile define.
- Log subfolders lowercase: `logs/midi/`, `logs/apvts/` (P-001 / D-094).

### File List

- `src/Shared/ProjectPaths.h`
- `src/Shared/ProjectPaths.cpp`
- `src/Core/Loggers/MidiLogger.h`
- `src/Core/Loggers/MidiLogger.cpp`
- `src/Core/Loggers/ApvtsLogger.h`
- `src/Core/Loggers/ApvtsLogger.cpp`
- `CMakeLists.txt`
- `_bmad-output/implementation-artifacts/0-3-projectpaths-and-logger-runtime-root-ad-10.md`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`

### Change Log

- 2026-05-30: Story 0.3 — ProjectPaths runtime root + logger path refactor (AD-10, D-093).
