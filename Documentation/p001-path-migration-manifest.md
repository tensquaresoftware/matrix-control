# P-001 path migration manifest

**Status:** **P-001 DONE** (2026-05-31) — JUCE project generator v2 layout adopted; Epic 0 Story 0.5 sign-off.

## Sign-off summary

| Check | Result |
|-------|--------|
| Generator v2 PascalCase roots (`Source/`, `Assets/`, `Tests/`, `Documentation/`, `Builds/`, `Logs/`) | ✅ |
| `ProjectPaths` runtime root (AD-10) | ✅ |
| macOS ARM Debug build + Standalone boot | ✅ |
| `auval` AU (`aufx` Mcpi Tssf) | ✅ PASS |
| DAW load (Ableton AU + VST3) | ✅ (manual, 2026-05-31) |
| System Plug-Ins fast path (no host scan hang) | ✅ |

Epic E0 stories 0.1, 0.2, 0.4 were **cancelled** (superseded by branch `chore/juce-project-generator-v2-structure`). Story 0.3 (ProjectPaths) and Story 0.5 (this sign-off) close P-001.

## Current root layout (generator v2)

| Path | Role |
|------|------|
| `Source/` | Application code (`Core/`, `GUI/`, `Shared/`) |
| `Assets/` | Fonts, README screenshots |
| `Tests/Unit/` | Unit tests (future dedicated runner — not linked into plugin targets) |
| `Documentation/` | Versioned public `.md` (kebab-case filenames) |
| `Builds/` | CMake outputs — **gitignored** (`macOS/ARM`, `macOS/Intel`, …) |
| `Logs/` | Runtime logs — **gitignored** (`MIDI/`, `APVTS/`) |
| `_bmad/`, `_bmad-output/` | BMad tooling and artifacts |
| `_local/` | Personal workspace — **gitignored** |

## Internal conventions

- Under `Source/`: `Core/`, `GUI/`, `Shared/` remain PascalCase.
- CMake include root: `Source/` (includes like `"Core/MIDI/MidiManager.h"` unchanged).
- Runtime project root: `ProjectPaths` + `.matrix-control-root` marker (AD-10).
- Plugins installed under `/Library/Audio/Plug-Ins/` skip executable walk-up; use env `MATRIX_CONTROL_PROJECT_ROOT` for dev DAW debug sessions.

## Historical note

Epic E0 stories 0.1–0.3 briefly used `src/`, `assets/`, `tests/`, `docs/`. Branch `chore/juce-project-generator-v2-structure` restores PascalCase and adopts generator v2 CMake (`project-configuration.cmake`, `CMakeUserPresets.json`).
