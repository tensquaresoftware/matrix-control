---
story_key: 0-1-gitignore-and-root-directory-renames
epic: 0
story: 1
status: done
baseline_commit: f4c263d
---

# Story 0.1: Gitignore and Root Directory Renames

Status: done

## Story

As a developer,
I want the repository root folders renamed and gitignore updated,
So that the tree matches P-001 targets before any new Core files are added.

## Acceptance Criteria

1. **Given** the brownfield repo on branch `main` **When** Story 0.1 merges **Then** `Source/` is `src/` and `Assets/` is `assets/` (git history preserved via `git mv`).
2. **Given** the updated `.gitignore` **When** runtime logs or build trees are created **Then** `logs/` and `builds/` are ignored **And** legacy `Logs/` and `Builds/` entries remain during transition (D-094).
3. **Given** renamed folders **When** CMake is configured **Then** `CMakeLists.txt` references `src/` and `assets/` only (include root + source file list + font binary paths) **And** the project still configures (Build output dir `Builds/` unchanged until Story 0.4).
4. **Given** this story scope **When** reviewed **Then** no `ProjectPaths`, no docs/tests migration, no feature code — those belong to Stories 0.2–0.5.

## Tasks / Subtasks

- [x] Update `.gitignore` for `logs/` + `src/Archives/` (AC: #2)
- [x] `git mv Source src` and `git mv Assets assets` (AC: #1)
- [x] Update `CMakeLists.txt` paths `Source/` → `src/`, `Assets/` → `assets/` (AC: #3)
- [x] Update versioned references in `README.md` (screenshot URL, OFL path) (AC: #3)
- [x] Verify `cmake -B Builds/macOS` + build Standalone target succeeds (AC: #3)
- [x] Update sprint-status.yaml story + epic status (AC: #4)

## Dev Notes

### Architecture Compliance (AD-9, P-001)

- Internal folders stay **PascalCase**: `src/Core/`, `src/GUI/`, `src/Shared/`.
- Includes like `"Core/MIDI/MidiManager.h"` unchanged — only CMake include root becomes `src/`.
- Do **not** rename `Builds/` → `builds/` on disk in this story (Story 0.4); `.gitignore` already lists both.

### Files to Touch

| File | Action |
|---|---|
| `.gitignore` | Add `logs/`; change `Source/Archives/` → `src/Archives/` |
| `CMakeLists.txt` | All `Source/` → `src/`, `Assets/` → `assets/` |
| `README.md` | GitHub raw URL + OFL path |
| `src/` | Renamed from `Source/` |
| `assets/` | Renamed from `Assets/` |

### Out of Scope (later E0 stories)

- Story 0.2: `Documentation/` → `docs/`, tests layout
- Story 0.3: `ProjectPaths`, logger fix (AD-10)
- Story 0.4: `builds/macos/` CMake output, `configure-platform.py`, VS Code tasks, `.cursorrules`
- Story 0.5: validators + P-001 sign-off

### Testing

- macOS: `cmake -B Builds/macOS -G Ninja -DCMAKE_BUILD_TYPE=Debug && cmake --build Builds/macOS --target Matrix-Control_Standalone`
- Boot: run Standalone once if build succeeds (optional smoke)

### References

- [Source: _bmad-output/planning-artifacts/epics.md — Story 0.1]
- [Source: architecture.md § P-001 rename map, AD-9]
- [Source: .decision-log.md D-092, D-094]

## Dev Agent Record

### Agent Model Used

Composer

### Debug Log References

- macOS case-insensitive FS: `Assets` → `assets` required two-step `git mv` via `assets-tmp-p001`.

### Completion Notes List

- Renamed `Source/` → `src/`, `Assets/` → `assets/` with history preserved.
- `.gitignore`: added `logs/`, kept `Logs/`; `src/Archives/` replaces `Source/Archives/`.
- `CMakeLists.txt` + `README.md` paths updated; configure + Standalone build green on `Builds/macOS`.

### File List

- `.gitignore`
- `CMakeLists.txt`
- `README.md`
- `src/**` (renamed from `Source/`)
- `assets/**` (renamed from `Assets/`)
- `_bmad-output/implementation-artifacts/0-1-gitignore-and-root-directory-renames.md`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`

### Change Log

- 2026-05-30: Story 0.1 implemented — P-001 root renames + gitignore + CMake path sync.
