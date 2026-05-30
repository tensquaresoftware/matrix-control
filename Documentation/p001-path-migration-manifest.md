# P-001 path migration manifest

**Status:** Superseded (2026-05-30) — lowercase GitHub-style root renames (Epic E0) were reverted in favor of JUCE project generator v2 layout (PascalCase roots).

## Current root layout (generator v2)

| Path | Role |
|------|------|
| `Source/` | Application code (`Core/`, `GUI/`, `Shared/`) |
| `Assets/` | Fonts, README screenshots |
| `Tests/Unit/` | Unit tests (linked via `Tests/CMakeLists.txt`) |
| `Documentation/` | Versioned public `.md` (kebab-case filenames) |
| `Builds/` | CMake outputs — **gitignored** (`macOS/ARM`, `macOS/Intel`, …) |
| `Logs/` | Runtime logs — **gitignored** (`MIDI/`, `APVTS/`) |
| `_bmad/`, `_bmad-output/` | BMad tooling and artifacts |
| `_local/` | Personal workspace — **gitignored** |

## Internal conventions

- Under `Source/`: `Core/`, `GUI/`, `Shared/` remain PascalCase.
- CMake include root: `Source/` (includes like `"Core/MIDI/MidiManager.h"` unchanged).
- Runtime project root: `ProjectPaths` + `.matrix-control-root` marker (AD-10).

## Historical note

Epic E0 stories 0.1–0.3 briefly used `src/`, `assets/`, `tests/`, `docs/`. Branch `chore/juce-project-generator-v2-structure` restores PascalCase and adopts generator v2 CMake (`project-configuration.cmake`, `CMakeUserPresets.json`).
