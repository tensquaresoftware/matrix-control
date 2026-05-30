# P-001 Path Migration Manifest

**Epic E0** — repository tree alignment (architecture AD-9).  
**Status:** in progress (Stories 0.1–0.5).

## Root renames (versioned)

| Legacy path | Target path | Story | Status |
|---|---|---|---|
| `Source/` | `src/` | 0.1 | done |
| `Assets/` | `assets/` | 0.1 | done |
| `Tests/Unit/` | `tests/unit/` | 0.2 | done |
| `Documentation/` | `docs/` | 0.2 | started (`docs/` created; legacy folder was personal → `_local/documentation/`) |
| `Builds/` | `builds/` | 0.4 | pending |
| `Logs/` | `logs/` | 0.3 | pending (runtime; gitignored) |

## Personal / gitignored (`_local/`)

| Legacy path | Target path | Story | Status |
|---|---|---|---|
| `Quality/` (non-test) | `_local/quality/` | 0.2 | done (from `_local/mockup/`) |
| `Workbench/` | `_local/workbench/` | 0.2 | done (from `_local/testlab/`) |
| `Source/Archives/` | `_local/src-archives/` | manual (Guillaume) | done |
| Legacy plans & specs | `_local/documentation/` | pre-BMad | unchanged |

## Internal code layout (unchanged names)

Under `src/`: **`Core/`**, **`GUI/`**, **`Shared/`** remain PascalCase.  
CMake include root: `src/` (includes like `"Core/MIDI/MidiManager.h"` unchanged).

## Filename convention (new public docs)

New files under `docs/`: **kebab-case** `.md` (e.g. `p001-path-migration-manifest.md`).
