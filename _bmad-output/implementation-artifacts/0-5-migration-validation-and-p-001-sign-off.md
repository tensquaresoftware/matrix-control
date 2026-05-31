---
story_key: 0-5-migration-validation-and-p-001-sign-off
epic: 0
story: 5
status: done
baseline_commit: a335db4
---

# Story 0.5: Migration Validation and P-001 Sign-Off

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a developer,
I want a full macOS build, boot validators, and host-load checks green after the P-001 / generator-v2 migration,
So that Epic 1 Core work can start safely on the current tree layout.

## Acceptance Criteria

1. **Given** Epic 0 foundation work is effectively complete (Story 0.3 **done**, chore `juce-project-generator-v2-structure` **done**; Stories 0.1, 0.2, 0.4 **cancelled** and superseded by generator v2 — see Dev Notes) **When** macOS **Debug** build succeeds on preset `default-macos-arm64` **Then** Standalone launches and the main editor UI opens without crash.
2. **And** `ApvtsValidator::validatePluginDescriptors()` returns valid (already invoked at processor startup via `validatePluginDescriptorsAtStartup()` — no DBG validation errors in Debug Standalone boot).
3. **And** opening the Standalone editor exercises `WidgetFactoryValidator` paths without exceptions (footer / console clean — factory creates widgets from descriptors successfully).
4. **And** installed AU + VST3 load in a DAW without hang: `auval -v aufx Mcpi Tssf` completes with **PASS** in under ~5 s; Ableton Live loads AU and VST3 instantly (manual sign-off — Guillaume confirmed 2026-05-31).
5. **And** `ProjectPaths` does not block host plugin scan: no walk-up from `/Library/Audio/Plug-Ins/`; `ensureDevelopmentLoggingStarted()` is **not** called from `PluginProcessor` constructor (logging remains on `prepareToPlay` for Standalone / dev DAW sessions only).
6. **And** uncommitted DAW-loading fix (`Source/Shared/ProjectPaths.cpp`, `Source/Core/PluginProcessor.cpp`) is committed with a clear message.
7. **And** `Documentation/p001-path-migration-manifest.md` records **P-001 DONE** (generator v2 layout, not lowercase renames).
8. **And** `_bmad-output/project-context.md` `updated` date and version bumped (minor: e.g. `1.0` → `1.1`) reflecting final root layout and AD-10 behaviour.
9. **And** `sprint-status.yaml` updated: `0-5-migration-validation-and-p-001-sign-off` → `done`; `epic-0` → `done` (sign-off closes the enabler epic despite cancelled sub-stories).

## Tasks / Subtasks

- [x] Finalize and commit DAW host-load fix (AC: #4, #5, #6)
  - [x] Keep `ProjectPaths` system-plugin fast path + `kMaxWalkUpDepth`
  - [x] Remove `ensureDevelopmentLoggingStarted()` from processor constructor
  - [x] Rebuild ARM Debug; confirm `auval` PASS
- [x] Run boot validators (AC: #2, #3)
  - [x] Debug Standalone: confirm no ApvtsValidator DBG errors at launch
  - [x] Open full UI; smoke-test widget creation (no factory validator throws)
- [x] Optional smoke: Universal preset build + quick `auval` (AC: #1) — not required for daily dev preset
- [x] Documentation and BMad sign-off (AC: #7, #8, #9)
  - [x] Update `p001-path-migration-manifest.md` status to **P-001 DONE**
  - [x] Bump `project-context.md` version / `updated`
  - [x] Update `sprint-status.yaml` and this story file → `done`
- [x] Do **not** add files under `Source/Core/Models/` in this story (Epic 1 gate)

## Dev Notes

### Migration reality (supersedes original Epic 0.1–0.4 AC)

Branch `chore/juce-project-generator-v2-structure` (merged PR #19) **reverted** lowercase root renames. Current SSOT layout:

| Path | Role |
|------|------|
| `Source/` | Code (`Core/`, `GUI/`, `Shared/`) |
| `Assets/` | Fonts, screenshots |
| `Tests/Unit/` | Future unit-test target (not linked into plugin binaries) |
| `Documentation/` | Public `.md` |
| `Builds/` | CMake outputs (gitignored) |
| `Logs/` | Runtime logs via `ProjectPaths` (`MIDI/`, `APVTS/`) |

Epic AC originally referenced `src/Core/Models/` — use **`Source/Core/Models/`** for Epic 1.

### DAW hang root cause (2026-05-31 — must stay fixed)

Commit `a335db4` added `ensureDevelopmentLoggingStarted()` in the processor **constructor**, which called `ProjectPaths::isUsingFallbackRoot()` during AU/VST load. From `~/Library/Audio/Plug-Ins/`, walk-up read `CMakeLists.txt` at every ancestor → multi-minute freeze (Ableton force-quit, `auval` hang).

**Fix pattern:** skip walk-up when executable path contains `/Library/Audio/Plug-Ins/`; cap walk depth; defer logging init out of constructor.

Reference: `sample` stack trace captured during hung `auval` (2026-05-31).

### Validators — how to verify today

| Validator | Trigger | Pass criterion |
|-----------|---------|----------------|
| `ApvtsValidator` | `PluginProcessor` ctor → `ApvtsFactory::validatePluginDescriptors()` | `validationResult.isValid == true`; no DBG errors |
| `WidgetFactoryValidator` | `WidgetFactory` methods when editor builds UI | No throws; widgets render in Standalone |

There is **no** standalone console test runner yet (`Tests/CMakeLists.txt` is a stub — static `juce::UnitTest` registrars must not link into AU/VST3). Boot + editor smoke is the acceptance path until Epic CI adds `Matrix-Control_UnitTests`.

### Build presets (from `.cursorrules` / chore notes)

- **Daily dev:** `default-macos-arm64` → `Builds/macOS/ARM`
- **Release / cross-arch smoke:** `default-macos-universal` → `Builds/macOS/Universal`
- Do not use `Matrix-Control_All` for deploy; use default `all` target for plugin copy.

### Architecture compliance

- AD-10: `ProjectPaths` SSOT; no `MATRIX_CONTROL_PROJECT_ROOT` compile define
- AD-9 / P-001: PascalCase generator v2 roots (not lowercase epic draft)
- D-003: validators at boot — Apvts debug-only today; do not add blocking modal UX in this story

### Files likely touched

- `Source/Shared/ProjectPaths.cpp` (DAW fix — may be uncommitted)
- `Source/Core/PluginProcessor.cpp` (remove ctor logging call)
- `Documentation/p001-path-migration-manifest.md`
- `_bmad-output/project-context.md`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`
- `_bmad-output/implementation-artifacts/0-5-migration-validation-and-p-001-sign-off.md`

### Previous story intelligence (0.3 + chore)

- Story 0.3: runtime log paths, fallback to Application Support
- Chore v2: `Tests/` must not link into plugin targets; `.vscode` launch sets `MATRIX_CONTROL_PROJECT_ROOT` for **debug** sessions only
- `Tests/Unit/` reserved for future dedicated runner

### Testing checklist (copy for dev-story completion)

```text
cmake --preset default-macos-arm64
cmake --build Builds/macOS/ARM --target all
open Builds/macOS/ARM/Matrix-Control_artefacts/Debug/Standalone/Matrix-Control.app
auval -v aufx Mcpi Tssf   # expect PASS ~1–2 s
# Manual: Ableton — load AU + VST3 Matrix-Control
```

### References

- [Source: _bmad-output/planning-artifacts/epics.md — Story 0.5]
- [Source: _bmad-output/implementation-artifacts/chore-juce-project-generator-v2-structure.md]
- [Source: Documentation/p001-path-migration-manifest.md]
- [Source: _bmad-output/planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md — AD-9, AD-10, boot validators]
- [Source: _bmad-output/planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md — D-003]

## Dev Agent Record

### Agent Model Used

Composer (dev-story 0.5)

### Debug Log References

- Hung `auval` sample 2026-05-31: constructor → `ProjectPaths` walk-up from system Plug-Ins folder
- Post-fix `auval` PASS ~0.4–1 s (ARM build, 2026-05-31)
- Standalone smoke: MidiManager + APVTS init, no validator errors (2026-05-31)

### Completion Notes List

- DAW host-load fix: system Plug-Ins fast path, walk depth cap, logging deferred out of processor ctor
- ARM Debug build + plugin install OK; `auval` PASS
- P-001 manifest and `project-context` v1.1 updated; Epic 0 closed in sprint-status
- Ableton AU/VST manual sign-off by Guillaume (2026-05-31, prior to this commit)

### File List

- `Source/Shared/ProjectPaths.cpp`
- `Source/Core/PluginProcessor.cpp`
- `Documentation/p001-path-migration-manifest.md`
- `_bmad-output/project-context.md`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`
- `_bmad-output/implementation-artifacts/0-5-migration-validation-and-p-001-sign-off.md`

### Change Log

- 2026-05-31: Story 0.5 — P-001 sign-off, DAW plugin load fix, Epic 0 done
