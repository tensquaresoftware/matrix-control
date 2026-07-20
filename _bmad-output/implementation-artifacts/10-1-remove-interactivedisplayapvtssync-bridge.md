---
baseline_commit: c1b1859e780aa374afd76b30bf25aca0bf270e8d
---

# Story 10.1: Remove InteractiveDisplayApvtsSync Bridge

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

## Story

As a developer,
I want the legacy `InteractiveDisplayApvtsSync` bridge removed,
so that Stories 10.2 / 10.3 can wire EnvelopeDisplay and TrackGeneratorDisplay to APVTS directly (D-018 second pass; FR-11 / FR-12).

## Acceptance Criteria

1. **Given** Epic 2 APVTS sync is stable (done)  
   **When** `PatchEditDisplaysPanel` / `PatchEditPanel` are refactored  
   **Then** `InteractiveDisplayApvtsSync` (class + `.h` / `.cpp`) is fully removed from the tree, CMake `PLUGIN_SOURCES`, and all call sites  
   **And** no equivalent hand-rolled Display↔Slider mediator is introduced under a new name

2. **Given** the bridge is gone  
   **When** the user edits Env1 / Env2 / Env3 / FmTrack parameters via their `ParameterCell` sliders  
   **Then** slider → APVTS → host / processor / SysEx behaviour is unchanged (no regression in slider-driven edits)  
   **And** `PatchNameDisplayPanel` wiring from Story 1.6 is untouched

3. **Given** Story 10.1 alone (before 10.2 / 10.3)  
   **When** envelope / track **displays** are inspected after bridge removal  
   **Then** it is an **accepted interim gap** that curve/track displays no longer sync from APVTS and no longer write APVTS on drag  
   **And** that gap is listed explicitly in the manual checklist (not treated as a 10.1 bug)  
   **And** Stories 10.2 / 10.3 are **not** implemented here

4. **Given** the refactor lands  
   **When** validation is recorded  
   **Then** a GUI manual checklist (or GUI tests if added) is documented in this story’s Dev Agent Record / Completion Notes  
   **And** macOS Debug build + `Matrix-Control_Tests` remain green

## Tasks / Subtasks

- [x] Task 1 — Delete bridge sources (AC: #1)
  - [x] Delete `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/InteractiveDisplayApvtsSync.h`
  - [x] Delete `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/InteractiveDisplayApvtsSync.cpp`
  - [x] Remove the matching entry from `CMakeLists.txt` `PLUGIN_SOURCES` (currently ~line 268)

- [x] Task 2 — Strip ownership from `PatchEditDisplaysPanel` (AC: #1, #2, #3)
  - [x] Remove `#include "Modules/InteractiveDisplayApvtsSync.h"`, forward-decl, `apvtsSync_` member, construction, `syncAllFromApvts()` call
  - [x] Remove public `connectSliderFastPaths(...)` declaration and definition
  - [x] Keep constructing / laying out / skinning / scaling the three `EnvelopeDisplay`, `TrackGeneratorDisplay`, and `PatchNameDisplayPanel`
  - [x] Keep an APVTS reference available on the panel (ctor already takes `apvts`) for Stories 10.2 / 10.3 — do not force a signature churn that 10.2 must undo
  - [x] If `apvts_` becomes unused after bridge removal: either keep it intentionally for 10.2 / 10.3 (preferred) or suppress unused-member warnings without deleting the ctor parameter

- [x] Task 3 — Strip call site from `PatchEditPanel` (AC: #1, #2)
  - [x] Remove the ctor block that calls `patchEditDisplaysPanel_->connectSliderFastPaths(...)`
  - [x] Do not reintroduce cross-panel slider→display listeners

- [x] Task 4 — Preserve widgets for 10.2 / 10.3 (AC: #3)
  - [x] Do **not** delete `EnvelopeDisplay` / `TrackGeneratorDisplay` callback hooks (`onValueChanged`, `onEditGestureBegin` / `End`, `setDelay`… / `setTrackPoint*` with `notify`) — they are the intended attachment surface for 10.2 / 10.3
  - [x] Do **not** change D-017 marker look, drag geometry, or paint code in this story
  - [x] Do **not** invent a new bridge class, mapping table, or “temporary” sync helper

- [x] Task 5 — Verify & document (AC: #2, #4)
  - [x] Confirm `ParameterCell` / `SliderAttachment` still drive Env / FmTrack int params (0–63)
  - [x] Grep: zero hits for `InteractiveDisplayApvtsSync` under `Source/` and `CMakeLists.txt`
  - [x] Build `macos-debug-arm64` (or local Debug preset) + run `Matrix-Control_Tests`
  - [x] Write manual checklist into Dev Agent Record (see Testing section)

## Dev Notes

### Scope fence (critical)

| In scope | Out of scope |
|----------|--------------|
| Delete `InteractiveDisplayApvtsSync` + wiring | Direct drag → APVTS on envelopes (Story **10.2**) |
| Keep slider-driven APVTS edits working | Direct Y-drag → APVTS on track points (Story **10.3**) |
| Document interim display desync | D-017 markers / addendum geometry changes |
| CMake source list cleanup | Core / MIDI / SysEx / PatchModel |
| | Patch name display (Story 1.6 — leave alone) |
| | Epic U layout / scale audits |

**Product meaning:** 10.1 is a **delete-and-clear** gate. After it, sliders still edit the synth parameters; the interactive curves go quiet until 10.2 / 10.3 reattach them the D-018 way (display listens / writes APVTS; JUCE attachments update sliders — **no** manual `slider.setValue()` from the display).

### Target architecture reminder (D-018) — for 10.2 / 10.3, not this story

[Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md` § Target sync architecture (D-018)]

- APVTS `AudioParameter` is SSOT (Int / Choice with descriptor normalisation).
- Slider → curve: `SliderAttachment` writes APVTS; display listens and repaints with `notify = false` on setters.
- Curve → slider: display writes `setValueNotifyingHost` + `beginChangeGesture` / `endChangeGesture`; attachments update sliders — **never** display → `slider.setValue()`.
- External updates (preset, automation, MIDI echo): same parameter listeners refresh displays.
- `PluginProcessor` handles SysEx / business logic — not UI resync.

**History:** D-018’s first pass (2026-05-27, commit `7d50bfb`) **created** `InteractiveDisplayApvtsSync` to replace an older Display→Slider if-chain in `PatchEditPanel`. Epic 10 is the **second** pass: remove that mediator so each display owns APVTS I/O. Do not recreate the mediator pattern.

[Source: `.decision-log.md` D-018; `epics.md` Epic 10]

### Current state — files being modified

#### `InteractiveDisplayApvtsSync` (DELETE)

- Path: `.../PatchEditDisplaysPanel/Modules/InteractiveDisplayApvtsSync.{h,cpp}`
- Owns: APVTS `Listener` for 20 int params (5 × 3 envelopes + 5 track points); display callbacks; `slidersByParameterId_` map; `Slider::Listener` fast paths; gesture pairing.
- Writes APVTS via `setValueNotifyingHost` **and** still pushes `slider.setValue(..., dontSendNotification)` — exactly the Display→Slider detour D-018 forbids long-term.
- No timers; message-thread listener driven.

#### `PatchEditDisplaysPanel` (UPDATE)

- Owns displays + `PatchNameDisplayPanel` + `std::unique_ptr<InteractiveDisplayApvtsSync> apvtsSync_`.
- Ctor: builds sync last, calls `syncAllFromApvts()`.
- Public `connectSliderFastPaths(top, bottom)` forwards to the bridge.
- Must keep: layout (`resized`), `setSkin`, `setUiScale`, patch-name panel, display members.

#### `PatchEditPanel` (UPDATE)

- Ctor (~lines 44–47) calls `connectSliderFastPaths` after all child panels exist — remove that block only.

#### Keep read-only / untouched

- `Source/GUI/Widgets/EnvelopeDisplay.{h,cpp}`
- `Source/GUI/Widgets/TrackGeneratorDisplay.{h,cpp}`
- `Source/GUI/Widgets/ParameterCell.{h,cpp}` — already has independent `SliderAttachment` (this is what “slider-driven edits” relies on)
- `Source/GUI/Tests/TestEnvelopeDisplays.*` / `TestTrackGeneratorDisplays.*` — visual harness only; no bridge dependency
- `Source/Shared/Definitions/PluginIDs.h` — Env / FmTrack parameter ID SSOT
- Anything under `Source/Core/`

### Parameter IDs the bridge currently binds (for 10.2 / 10.3 reference)

Reuse `PluginIDs.h` — never new string literals:

- Envelope modules: `env{1,2,3}Delay|Attack|Decay|Sustain|Release` via `Envelope{1,2,3}Module::ParameterWidgets`
- Track: `fmTrackPoint1..5` via `FmTrackModule::ParameterWidgets`

### Architecture compliance

| Rule | Implication for 10.1 |
|------|----------------------|
| GUI → Core → Shared; Core ↛ GUI | Touch **GUI only** |
| Real paths use `Source/` not aspirational `src/` | Follow `.cursorrules` / `project-context.md` |
| APVTS IDs only in `PluginIDs.h` | No new ID strings |
| No GUI deps in Core | Do not “move sync logic to Core” |
| No global AffineTransform scale | Unrelated — leave alone |
| English-only source | Checklist text in story file may be English |

[Source: `_bmad-output/project-context.md`; `architecture.md` AD-1 / anti-patterns; note: architecture E10 table paths under `src/` are **stale**]

### Anti-patterns to refuse

- Replacing `InteractiveDisplayApvtsSync` with `InteractiveDisplayApvtsSync2` / `DisplaySyncHelper` / panel-local if-chain that still does `slider.setValue` from display callbacks
- Implementing full FR-11 / FR-12 bidirectional display editing inside 10.1 “while we’re here”
- Extending or resurrecting the bridge for patch-name (Story 1.6 already uses a separate `ValueTree::Listener`)
- Deleting `onValueChanged` / gesture callbacks from widgets “because unused” — breaks 10.2 / 10.3
- Touching Epic U layout / Design* / Factory work

### Project Structure Notes

- Bridge lives under panel `Modules/` next to `PatchNameDisplayPanel` — delete only the sync pair.
- After deletion, `Modules/` still holds `PatchNameDisplayPanel`.
- CMake: one `PLUGIN_SOURCES` line to remove; no new `.cpp` expected for 10.1.

### Previous / related story intelligence

- **No prior story in Epic 10** — this is the epic opener; set `epic-10` → `in-progress` in sprint status when creating this story.
- **Story 1.6** (`1-6-wire-patch-name-display.md`): explicitly forbids extending `InteractiveDisplayApvtsSync` for patch name; confirms bridge is Epic 10 deletion target; patch-name uses `ValueTree::Listener` on a **string property**, not int AudioParameters — leave that path alone.
- **Epic 9** complete — dirty-patch / unsaved dialog orthogonal; no shared files expected.
- **Epic U** notes that interactive drag geometry is Epic 10 — do not mix layout audits into this delete.

### Git intelligence

- Introducing commit: `7d50bfb` — “Centralize interactive display sync on APVTS with responsive UI paths” (created the bridge; removed older PatchEditPanel slider bridge).
- No later commits touch `InteractiveDisplayApvtsSync.cpp` — deletion surface is clean and isolated.
- Recent commits are docs / Epic 9 status — no conflicting WIP on these files.

### Latest tech notes (JUCE 8.0.12)

- Project pin: JUCE **8.0.12** (`/Applications/JUCE` on this machine).
- For **10.2 / 10.3** (not 10.1): prefer message-thread `setValueNotifyingHost` + matched `beginChangeGesture` / `endChangeGesture`; APVTS / parameter listeners may fire off the message thread — never `repaint()` directly from listener; use `MessageManager::callAsync` / async updater patterns per `project-context.md`.
- `AudioProcessorValueTreeState::SliderAttachment` remains the model for slider↔APVTS; displays need custom listen/write (no stock curve attachment).

### Testing requirements

**Automated:** No existing unit test references this bridge. Do not invent GUI unit tests unless extracting pure logic (unlikely for a pure delete). Run existing `Matrix-Control_Tests` as regression gate.

**Manual checklist (required — paste results into Completion Notes):**

1. Standalone Debug build launches; PATCH EDIT visible.
2. Move Env1 Delay / Attack / Decay / Sustain / Release sliders → values stick; no crash; (optional with hardware) SysEx / synth follows as before Epic 10.
3. Repeat sample sliders on Env2, Env3, and FmTrack Point 1–5.
4. Confirm **known interim:** envelope / track **displays** do **not** follow slider moves and drag-on-curve does **not** edit parameters (expected until 10.2 / 10.3).
5. Patch name display still updates on load / mutate / init (Story 1.6 regression).
6. UI Scale + skin switch still apply to the display band.
7. Grep clean: `InteractiveDisplayApvtsSync` absent from `Source/` and `CMakeLists.txt`.

### References

- [Source: `_bmad-output/planning-artifacts/epics.md` — Epic 10, Story 10.1–10.3]
- [Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md` — FR-11, FR-12]
- [Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md` — D-017, D-018]
- [Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md` — D-016, D-018]
- [Source: `_bmad-output/planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md` — E10 mapping (treat `src/` paths as stale)]
- [Source: `_bmad-output/project-context.md` — GUI/Core rules, testing pyramid]
- [Source: `_bmad-output/implementation-artifacts/1-6-wire-patch-name-display.md` — anti-pattern: do not extend this bridge]
- [Source: `Source/GUI/.../InteractiveDisplayApvtsSync.{h,cpp}` — deletion target]
- [Source: `Source/GUI/.../PatchEditDisplaysPanel.{h,cpp}`, `PatchEditPanel.cpp` — UPDATE]

## Dev Agent Record

### Agent Model Used

Composer (Cursor agent router)

### Debug Log References

- Grep: zero hits for `InteractiveDisplayApvtsSync` under `Source/` and `CMakeLists.txt` after deletion.
- `cmake --build --preset macos-debug-arm64` — success.
- `Matrix-Control_Tests` — exit code 0 (all unit tests green).

### Implementation Plan

1. Delete `InteractiveDisplayApvtsSync.{h,cpp}` and remove from `PLUGIN_SOURCES`.
2. Strip bridge ownership / `connectSliderFastPaths` from `PatchEditDisplaysPanel`; keep `apvts_` for 10.2 / 10.3 with `[[maybe_unused]]`.
3. Remove `PatchEditPanel` ctor call site only.
4. Leave `EnvelopeDisplay` / `TrackGeneratorDisplay` / `ParameterCell` / `PatchNameDisplayPanel` untouched.

### Completion Notes List

- Bridge fully removed; no replacement mediator introduced.
- `ParameterCell` still owns independent `SliderAttachment` for Env / FmTrack int params — slider-driven APVTS path unchanged by this delete.
- Widget callback hooks (`setOnValueChanged`, edit-gesture begin/end, notify setters) preserved for Stories 10.2 / 10.3.
- **Manual GUI checklist (AC #4)** — agent automated gates done; interactive steps for human review:

  | # | Check | Expected | Agent |
  |---|-------|----------|-------|
  | 1 | Standalone Debug launches; PATCH EDIT visible | OK | Build green; launch confirm on review |
  | 2 | Env1 Delay/Attack/Decay/Sustain/Release sliders stick; no crash | OK | SliderAttachment path untouched |
  | 3 | Sample Env2 / Env3 / FmTrack Point 1–5 sliders | OK | Same as #2 |
  | 4 | **Known interim:** envelope/track displays do **not** follow sliders; curve/track drag does **not** write APVTS | Expected until 10.2 / 10.3 | By design after bridge removal |
  | 5 | Patch name display still updates on load / mutate / init | OK | `PatchNameDisplayPanel` untouched |
  | 6 | UI Scale + skin still apply to display band | OK | `setUiScale` / `setSkin` kept |
  | 7 | Grep clean: `InteractiveDisplayApvtsSync` absent from `Source/` + `CMakeLists.txt` | OK | Verified |

### File List

- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/InteractiveDisplayApvtsSync.h` (deleted)
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/InteractiveDisplayApvtsSync.cpp` (deleted)
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/PatchEditDisplaysPanel.h` (modified)
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/PatchEditDisplaysPanel.cpp` (modified)
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditPanel.cpp` (modified)
- `CMakeLists.txt` (modified)
- `_bmad-output/implementation-artifacts/10-1-remove-interactivedisplayapvtssync-bridge.md` (modified)
- `_bmad-output/implementation-artifacts/sprint-status.yaml` (modified)

### Review Findings

- [x] [Review][Decision] Accept 10.1 before interactive GUI checklist is human-run? — Resolved: validate now (code review + automated gates); interactive checklist optional later.

## Change Log

- 2026-07-20: Removed `InteractiveDisplayApvtsSync` bridge and call sites; documented interim display desync; status → review.
- 2026-07-20: Code review — 1 decision-needed (interactive checklist vs done gate); Blind Hunter returned empty.
- 2026-07-20: Code review resolved — decision = validate now; status → done.
