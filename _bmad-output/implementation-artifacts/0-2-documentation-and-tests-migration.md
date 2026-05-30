---
story_key: 0-2-documentation-and-tests-migration
epic: 0
story: 2
status: done
baseline_commit: 484a19c
---

# Story 0.2: Documentation and Tests Migration

Status: done

## Story

As a developer,
I want public docs and unit tests moved to `docs/` and `tests/`,
So that versioned artifacts follow pro JUCE layout.

## Acceptance Criteria

1. **Given** Story 0.1 complete **When** documentation and test folders are migrated **Then** public `.md` files live under `docs/` (kebab-case manifest recorded).
2. **And** `Tests/Unit/` becomes `tests/unit/` in git and CMake.
3. **And** personal `Quality/` / `Workbench/` equivalents live under `_local/quality/` and `_local/workbench/`.
4. **And** `_local/` remains gitignored.

## Tasks / Subtasks

- [x] Git-track `tests/unit/` (from `Tests/Unit/`) (AC: #2)
- [x] Update `CMakeLists.txt` test source paths (AC: #2)
- [x] Create `docs/` with README + P-001 manifest (AC: #1)
- [x] Align `_local/mockup` → `_local/quality`, `_local/testlab` → `_local/workbench` (AC: #3)
- [x] Update `.gitignore` (remove `src/Archives/`, add legacy `Tests/`) (AC: #4)
- [x] Update sprint-status.yaml (AC: #4)

## Dev Notes

- Public `Documentation/` was previously moved out of git to `_local/documentation/` (commit a310cb8). Story 0.2 establishes versioned `docs/` for public material going forward; BMad artifacts stay in `_bmad-output/`.
- Guillaume had already created `tests/unit/` on disk; git index still pointed at `Tests/Unit/` — fixed with per-file `git mv`.
- `Builds/` → `builds/` remains **Story 0.4**.

## Dev Agent Record

### Completion Notes List

- `tests/unit/` tracked; CMake paths updated; configure/build unchanged for plugin target.
- `docs/README.md` + `docs/p001-path-migration-manifest.md` added.
- `_local/quality/` and `_local/workbench/` aligned to P-001 naming.

### File List

- `tests/unit/*.cpp` (renamed from `Tests/Unit/`)
- `CMakeLists.txt`
- `.gitignore`
- `docs/README.md`
- `docs/p001-path-migration-manifest.md`
- `_bmad-output/implementation-artifacts/0-2-documentation-and-tests-migration.md`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`

### Change Log

- 2026-05-30: Story 0.2 — docs scaffold + tests/unit migration + _local layout.
