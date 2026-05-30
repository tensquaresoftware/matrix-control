---
story_key: 0-4-cmake-and-tooling-path-updates
epic: 0
story: 4
status: done
baseline_commit: 9aa4c26
---

# Story 0.4: CMake and Tooling Path Updates

Status: done

## Story

As a developer,
I want CMake and IDE tooling to reference `src/`, `builds/`, and `tests/`,
So that builds and CI use the target layout.

## Acceptance Criteria

1. **Given** Stories 0.1–0.3 complete **When** `CMakeLists.txt` is updated **Then** include root is `src/`, output directory is `builds/macos/` (windows/linux documented), `tests/` subdirectory is wired.
2. **And** `.cursorrules`, `project-context.md`, and README paths are updated; `.clangd` / compile_commands paths fixed.

## Tasks / Subtasks

- [x] Add `tests/CMakeLists.txt` and wire unit sources via `add_subdirectory(tests)` (AC: #1)
- [x] Migrate tooling paths `Builds/{macOS,Windows,Linux}` → `builds/{macos,windows,linux}` (AC: #1)
- [x] Update `CMakeUserPresets.json`, `.vscode/tasks.json`, `.vscode/launch.json`, `.vscode/settings.json` (AC: #2)
- [x] Update `configure-platform.py` + `.clangd` CompilationDatabase (AC: #2)
- [x] Update `.cursorrules`, `project-context.md`, `README.md`, P-001 manifest (AC: #2)
- [x] Verify `cmake -B builds/macos` + Standalone build (AC: #1)
- [x] Update sprint-status.yaml (AC: #2)

## Dev Notes

- Legacy `Builds/` remains in `.gitignore` during transition; Clean task removes both.
- On-disk `Builds/` tree is not renamed by git (gitignored) — reconfigure to `builds/macos/` locally.
- Unit tests stay linked into plugin target (JUCE UnitTest static registration); `tests/` CMake wires sources only.

## Dev Agent Record

### Completion Notes List

- CMake binary dirs: `builds/macos`, `builds/windows`, `builds/linux` (lowercase platform folders per AD-9).
- `.clangd` points to `builds/macos` CompilationDatabase; `configure-platform.py` syncs per platform.
- `.clangd` skips indexing under `builds/` (JUCE BinaryData sources hang clangd).
- Root `compile_commands.json` is a local filtered copy (gitignored), updated via `configure-platform.py --update-compile-commands`.

### File List

- `CMakeLists.txt`
- `tests/CMakeLists.txt`
- `CMakeUserPresets.json`
- `configure-platform.py`
- `.clangd`
- `.gitignore`
- `.cursorrules`
- `.vscode/settings.json`
- `.vscode/tasks.json`
- `.vscode/launch.json`
- `README.md`
- `docs/p001-path-migration-manifest.md`
- `_bmad-output/project-context.md`
- `_bmad-output/implementation-artifacts/0-4-cmake-and-tooling-path-updates.md`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`

### Change Log

- 2026-05-30: Story 0.4 — builds/ layout + tests CMake subdirectory + IDE tooling.
