---
baseline_commit: 236c5fb24e219538662ef611a1c0cca1b5e249a8
---

# Story 10.3: TrackGeneratorDisplay Direct APVTS Editing

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

## Story

As a sound designer,
I want to drag Track Generator points on the Y axis,
so that track editing is interactive and writes APVTS directly (FR-12, UX-DR2, D-018 second pass — track half).

## Acceptance Criteria

1. **Given** Story 10.1 is done (bridge removed) and Story 10.2 is done (envelope displays wired)  
   **When** `PatchEditDisplaysPanel` wires the single `TrackGeneratorDisplay` to APVTS  
   **Then** dragging any of the five Y-only control points writes the matching `AudioParameterInt` (`fmTrackPoint1`…`fmTrackPoint5`) via `setValueNotifyingHost`  
   **And** matched `beginChangeGesture` / `endChangeGesture` are paired for the active drag  
   **And** no code path calls `slider.setValue(...)` (or otherwise reaches into `ParameterCell` / `FmTrackPanel`) from the display side

2. **Given** FmTrack Point 1–5 sliders (existing `SliderAttachment` on `FmTrackPanel`)  
   **When** the user moves any of those sliders  
   **Then** the Track Generator display updates to match (via APVTS parameter listener → `setTrackPointN(..., notify=false)`)  
   **And** the reverse path (point drag → APVTS → attachment) updates the slider without a Display→Slider bridge

3. **Given** external APVTS updates (preset load, automation, MIDI echo, init, mutator)  
   **When** any of the five track-point int parameters change  
   **Then** the display refreshes without feedback loops  
   **And** `PluginProcessor` / SysEx outbound behaviour remains unchanged (UI does not “resync” Core)

4. **Given** addendum § TrackGeneratorDisplay, D-017, and D-019  
   **When** the Track Generator display is used  
   **Then** drag geometry and visual markers remain as already implemented (five Y-only points; even X spacing; small filled squares)  
   **And** this story does **not** rewrite paint / hit-test / geometry code unless a proven regression requires a minimal fix

5. **Given** the wiring lands  
   **When** validation is recorded  
   **Then** a GUI manual checklist is documented in Dev Agent Record / Completion Notes  
   **And** macOS Debug build + `Matrix-Control_Tests` remain green  
   **And** Env1/2/3 display↔APVTS bindings from Story 10.2 remain intact (no regression)

## Tasks / Subtasks

- [x] Task 1 — Add Track Generator Display↔APVTS binding (AC: #1, #2, #3)
  - [x] Introduce a small helper under `PatchEditDisplaysPanel/Modules/` (recommended name: `TrackGeneratorDisplayApvtsBinding`) that binds **one** `TSS::TrackGeneratorDisplay&` + five `PluginIDs` strings to one APVTS
  - [x] Own **one** instance from `PatchEditDisplaysPanel` — do **not** fold envelopes into this helper (already done in 10.2)
  - [x] Prefer **copy/adapt** `EnvelopeDisplayApvtsBinding` (read/write/gesture/`callAsync`/gesture-gate) rather than inventing a third pattern
  - [x] Register `apvts.addParameterListener` for the five track-point IDs; remove listeners in destructor
  - [x] On `parameterChanged`: read int via `convertFrom0to1(getValue())`, clamp `[0,63]`, apply with `setTrackPoint1`…`setTrackPoint5(value, false)`
  - [x] **Threading:** `TrackGeneratorDisplay` setters call `repaint()` — never call them directly from an audio-thread listener; marshal with `juce::MessageManager::callAsync`. Guard with `SafePointer<TrackGeneratorDisplay>`
  - [x] **Gesture echo (mandatory from 10.2 review):** skip `parameterChanged` / queued `callAsync` when the active gesture gate matches that parameter (same `shared_ptr<atomic<AudioProcessorParameter*>>` pattern as envelope binding)
  - [x] Wire `setOnValueChanged` → `writeIntParameter` (`setValueNotifyingHost(convertTo0to1(...))`); `pointIndex` 0..4 maps to Point 1..5
  - [x] Wire `setOnEditGestureBegin` / `setOnEditGestureEnd` → `beginChangeGesture` / `endChangeGesture` on the active parameter
  - [x] Call initial sync on construction (seed all five values with `notify=false`)
  - [x] Add new `.cpp` to `CMakeLists.txt` `PLUGIN_SOURCES` next to `EnvelopeDisplayApvtsBinding.cpp`

- [x] Task 2 — Hook from `PatchEditDisplaysPanel` (AC: #1, #2, #5)
  - [x] Update the “Kept for Story 10.3” comment on `apvts_` once track binding uses it (pointer already required by envelope bindings)
  - [x] Construct `trackGeneratorBinding_` after `trackGeneratorDisplay_` exists; declare the unique_ptr **after** the display member (destruction order: binding cleared before display)
  - [x] Anonymous (or file-local) parameter ID table: `FmTrackModule::ParameterWidgets::kTrackPoint1` … `kTrackPoint5`
  - [x] Do **not** reintroduce `connectSliderFastPaths`, cross-panel slider listeners, or any `FmTrackPanel` includes
  - [x] Leave envelope bindings, `PatchNameDisplayPanel`, layout / skin / scale unchanged

- [x] Task 3 — Preserve widget geometry / D-017 / D-019 (AC: #4)
  - [x] Do **not** change `TrackGeneratorDisplay` paint, hit-test, or drag math unless fixing a documented wiring bug
  - [x] Do **not** change marker shape (filled squares via `fillRect`)
  - [x] Do **not** add X-axis drag or time/delay parameters (widget is Y-levels only)
  - [x] Do **not** delete or rename `onValueChanged` / gesture callback API

- [x] Task 4 — Verify & document (AC: #2, #3, #5)
  - [x] Grep: zero hits for `InteractiveDisplayApvtsSync`; no new `slider.setValue` from display binding code; binding must not include `FmTrackPanel` / `ParameterCell` / `Slider`
  - [x] Build `macos-debug-arm64` + run `Matrix-Control_Tests`
  - [x] Fill manual checklist in Completion Notes (see Testing)
  - [x] Spot-check Env1 slider↔curve still works (10.2 regression)

## Dev Notes

### Scope fence (critical)

| In scope | Out of scope |
|----------|--------------|
| Direct Track Generator Display ↔ APVTS (5 int params: Point 1–5) | EnvelopeDisplay wiring (Story **10.2** — already done) |
| Gesture pairing on Y-point drag | Rewriting TrackGeneratorDisplay geometry / D-017 markers |
| Message-thread-safe listener → display updates | Core / MIDI / SysEx / PatchModel / DirtyPatchTracker |
| CMake entry for new binding `.cpp` | FmAmount / Env3 / Pressure / Track Input / Init / Mutator enable |
| Manual GUI checklist | Epic U layout / Design* / Factory audits |
| | Resurrecting Display→Slider mediation |
| | Zero-bounds mid-drag guard (`deferred-work.md`) |

**Product meaning:** after 10.1 the curves went quiet; 10.2 restored the three envelopes. 10.3 restores the Track Generator the same **D-018** way: APVTS is SSOT; Point 1–5 sliders stay on stock `SliderAttachment`; the display listens and writes parameters — never touches sliders. Completing 10.3 finishes Epic 10’s interactive-display second pass.

### Target architecture (D-018) — must follow

[Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md` § Target sync architecture (D-018)]

- APVTS `AudioParameter` is SSOT (here: `AudioParameterInt` 0–63 for track points).
- **Slider → curve:** `SliderAttachment` writes APVTS; display listens and applies setters with `notify = false`.
- **Curve → slider:** display callbacks write `setValueNotifyingHost` + gestures; attachments update sliders — **never** `slider.setValue()` from display code.
- **External updates:** same parameter listeners refresh displays.
- `PluginProcessor` handles SysEx / business logic — not UI resync.

**History:** D-018 first pass (`7d50bfb`) created `InteractiveDisplayApvtsSync` (still pushed `slider.setValue`). Story 10.1 (`a65683e`) deleted that bridge. Story 10.2 wired envelopes. This story is the **track half** of the second pass. Do not recreate the mediator pattern.

### Recommended design (prevent reinventing the wheel)

Mirror `EnvelopeDisplayApvtsBinding` almost line-for-line; only change display type, setter names, and parameter IDs:

```text
PatchEditDisplaysPanel
  ├─ envelope1Display_ + EnvelopeDisplayApvtsBinding   ← leave alone (10.2)
  ├─ envelope2Display_ + EnvelopeDisplayApvtsBinding
  ├─ envelope3Display_ + EnvelopeDisplayApvtsBinding
  ├─ trackGeneratorDisplay_ + TrackGeneratorDisplayApvtsBinding  ← THIS STORY
  └─ patchNameDisplayPanel_                                      ← leave alone
```

| Aspect | Envelope (10.2) | Track (10.3) |
|--------|-----------------|--------------|
| Helper | `EnvelopeDisplayApvtsBinding` | `TrackGeneratorDisplayApvtsBinding` |
| Display | `EnvelopeDisplay` | `TrackGeneratorDisplay` |
| Setters | `setDelay`…`setRelease(..., false)` | `setTrackPoint1`…`5(..., false)` |
| Callback index | `paramIndex` 0..4 = Delay…Release | `pointIndex` 0..4 = Point 1…5 |
| Instance count | ×3 | ×1 |
| Semantics | Delay + ADSR geometry | **Y-only levels**, no delay/time |
| Param namespace | `Envelope{N}Module::…` | `FmTrackModule::ParameterWidgets::kTrackPoint{1..5}` |

Salvage track callback mapping from deleted bridge if useful (ignore slider fast paths):

```bash
git show a65683e^:Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/InteractiveDisplayApvtsSync.cpp
```

Focus on: `connectTrackGeneratorDisplay`, `applyTrackPointDisplayValue`, `kTrackPointIds`. Ignore: `connectSliderFastPaths`, `connectTrackPointSlider`, `SliderToDisplayListener`, `updateSliderUi`, `FmTrackPanel` includes.

**Primary template to copy:** live `EnvelopeDisplayApvtsBinding.{h,cpp}` (already includes gesture-gate / stale-async fix from 10.2 code review).

### Current state — files being modified

#### `TrackGeneratorDisplay` (READ / preserve — UPDATE only if wiring bug forces it)

- Path: `Source/GUI/Widgets/TrackGeneratorDisplay.{h,cpp}`
- Value model: five `int` levels clamped `[0,63]` — matches `fmTrackPoint1`…`5` `AudioParameterInt`
- `pointIndex` in callbacks: `0` = Point 1 … `4` = Point 5
- Two-arg setters with `notify=false` suppress `onValueChanged_` (required for APVTS→display)
- Drag path calls `onValueChanged_` / gesture begin-end directly (binding attaches here)
- **Y-only** drag; X positions fixed by geometry (even spacing)
- Markers: `g.fillRect` small squares (D-017 already done)
- No delay / time API on this widget

#### `PatchEditDisplaysPanel` (UPDATE)

- Path: `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/PatchEditDisplaysPanel.{h,cpp}`
- Owns `trackGeneratorDisplay_` value member + three envelope bindings already live
- `apvts_` already used by envelope bindings; comment still says “Kept for Story 10.3”
- After 10.2: track display still silent (no listeners, no callbacks, no initial sync)

#### New module (NEW)

- Suggested: `.../PatchEditDisplaysPanel/Modules/TrackGeneratorDisplayApvtsBinding.{h,cpp}`
- Sibling of `EnvelopeDisplayApvtsBinding` under `Modules/`
- Must **not** include `FmTrackPanel`, `ParameterCell`, or `Slider`

#### Keep untouched

- `ParameterCell` / `SliderAttachment` — already keep Point 1–5 sliders in sync with APVTS
- `FmTrackPanel` — no includes from the binding
- `EnvelopeDisplayApvtsBinding` ×3 — regression surface only
- `PatchNameDisplayPanel` — string property listener, not int params
- `Source/Core/**`
- `TestTrackGeneratorDisplays.*` — visual harness only; no APVTS required by AC

### Parameter IDs (SSOT — never new literals)

From `Source/Shared/Definitions/PluginIDs.h` → `PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets`:

| `pointIndex` | Constant | Literal | Type | Range | Default |
|--------------|----------|---------|------|-------|---------|
| 0 | `kTrackPoint1` | `"fmTrackPoint1"` | int | 0..63 | 0 |
| 1 | `kTrackPoint2` | `"fmTrackPoint2"` | int | 0..63 | 15 |
| 2 | `kTrackPoint3` | `"fmTrackPoint3"` | int | 0..63 | 31 |
| 3 | `kTrackPoint4` | `"fmTrackPoint4"` | int | 0..63 | 47 |
| 4 | `kTrackPoint5` | `"fmTrackPoint5"` | int | 0..63 | 63 |

Binding table order **must** match `pointIndex` 0..4.

**Do not wire in this story:**

| ID | Literal | Why |
|----|---------|-----|
| `kFmAmount` | `fmTrackFmAmount` | Not a curve point |
| `kFmModByEnv3` | `fmTrackFmModByEnv3` | Bipolar (−63..63) — envelope binding’s 0..63 clamp is wrong for these |
| `kFmModByPressure` | `fmTrackFmModByPressure` | Bipolar |
| `kTrackInput` | `fmTrackInput` | Choice / combo |
| `StandaloneWidgets::kInit` | `fmTrackInit` | Button |
| Mutator `kEnableFmTrack` | `patchMutatorEnableFmTrack` | Different module |

### Int ↔ normalised conversion (same as envelope binding)

```cpp
// read
auto* p = apvts.getParameter(id);
int value = juce::jlimit(0, 63, juce::roundToInt(p->convertFrom0to1(p->getValue())));

// write
p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(juce::jlimit(0, 63, value))));
```

No per-descriptor remapping needed for these five unipolar params.

### Addendum geometry (preserve — acceptance evidence, not rewrite target)

[Source: addendum.md § Interactive display geometry — TrackGeneratorDisplay]

- Same outer padding as envelope displays
- Five points: **vertical drag only** (Y maps 0–63 to ~100% panel height)
- Points spaced evenly on X; each segment ~25% panel width
- Markers: small filled squares (D-017)

Already implemented in `TrackGeneratorDisplay` — verify in manual checklist, do not “improve”.

### Architecture compliance

| Rule | Implication for 10.3 |
|------|----------------------|
| GUI → Core → Shared; Core ↛ GUI | Touch **GUI only** (+ CMake) |
| Real paths use `Source/` | Architecture doc `src/` paths are **stale** |
| APVTS IDs only in `PluginIDs.h` | No new ID strings |
| APVTS listeners may be off message thread | `callAsync` before `setTrackPoint*` / `repaint` |
| No GUI unit-test mandate | Manual checklist + existing unit tests |
| English-only source | Checklist English in story file OK |

[Source: `_bmad-output/project-context.md`; architecture E10 mapping]

### Anti-patterns to refuse

- Recreating `InteractiveDisplayApvtsSync` (or `*Sync2`) that still maps displays to sliders
- Listening to `juce::Slider` to refresh the curve (use APVTS listeners only)
- Calling `slider.setValue` / `ParameterCell::getSlider()` from display callbacks
- Including `FmTrackPanel` from the binding “to find sliders”
- Wiring bipolar FM mod params or Track Input “while we’re here”
- Changing Y-only drag rules or marker shape “for polish”
- Putting French text in source
- Moving sync logic into `Source/Core/`
- Skipping `callAsync` or the gesture-gate echo skip (both proven required by 10.2)
- Merging track + envelope into one mega-binding class

### Project Structure Notes

- New files live under panel `Modules/` next to `EnvelopeDisplayApvtsBinding`
- After 10.3, `Modules/` holds: `PatchNameDisplayPanel`, `EnvelopeDisplayApvtsBinding`, `TrackGeneratorDisplayApvtsBinding`
- CMake: add one `PLUGIN_SOURCES` line for the new `.cpp` (near line ~268, next to envelope binding)
- Naming: prefer `*Binding` over `*Sync` to avoid implying the deleted bridge pattern

### Previous story intelligence (10.1 + 10.2)

**10.1 (`a65683e`):**
- Bridge fully removed; widget callback hooks intentionally preserved for 10.2 / 10.3
- Do not treat prior track silence as a bug once 10.3 lands — silence was the accepted interim

**10.2 (`bc120fd` + review + `236c5fb` close):**
- `EnvelopeDisplayApvtsBinding` is the **canonical** D-018 display binding pattern — copy it
- Review patch: ignore APVTS→display echo while drag gesture is active (shared gesture gate + async re-check) — **must ship in 10.3 from day one**
- Human Standalone checklist was required before done; same expectation for 10.3
- Do not regress Env1/2/3 when adding track binding

[Source: `_bmad-output/implementation-artifacts/10-1-remove-interactivedisplayapvtssync-bridge.md`]  
[Source: `_bmad-output/implementation-artifacts/10-2-envelopedisplay-direct-apvts-editing.md`]

### Git intelligence

- `236c5fb` — Close story 10-2 after code review and smoke test (**baseline** for this story)
- `bc120fd` — Wire EnvelopeDisplay Env1–3 directly to APVTS (pattern to mirror)
- `a65683e` — Remove InteractiveDisplayApvtsSync bridge (salvage track helpers from `a65683e^`)
- `7d50bfb` — original bridge introduction (historical only)

### Latest tech notes (JUCE 8.0.12)

- Project pin: JUCE **8.0.12** (`/Applications/JUCE`)
- Prefer `AudioProcessorParameter::beginChangeGesture` / `endChangeGesture` + `setValueNotifyingHost` on the message thread (display mouse callbacks already are)
- `AudioProcessorValueTreeState::Listener::parameterChanged` is **not** guaranteed on the message thread — hop before GUI mutation
- No stock “curve attachment”; custom binding is expected
- `SafePointer<Component>` + shared atomic gesture gate recommended when posting `callAsync` from listeners
- Docs: https://docs.juce.com — `AudioProcessorValueTreeState`, `AudioProcessorParameter`

### Testing requirements

**Automated:** No requirement to invent GUI unit tests. Run `Matrix-Control_Tests` as regression gate. Do not put tests in `paint()` / audio thread. (`Tests/` compiles Core only — no GUI.)

**Manual checklist (required — paste results into Completion Notes):**

1. Standalone Debug launches; PATCH EDIT visible; Track Generator curve visible (right of envelopes).
2. Move FmTrack Point 1 / 2 / 3 / 4 / 5 sliders → **curve points follow** in real time; no crash.
3. Drag each of the five display points (Y only) → **matching sliders follow**; values stick after mouse up.
4. Confirm markers remain small filled squares (not circles).
5. Confirm points do **not** move horizontally when dragging (Y-only).
6. Load / init / mutate a patch (or change track points externally) → display refreshes.
7. **Regression:** Env1 Delay (or one ADSR) slider ↔ curve still bidirectional (Story 10.2).
8. Patch name display still updates (Story 1.6 regression).
9. UI Scale + skin switch still apply to the display band.
10. Grep clean: no `InteractiveDisplayApvtsSync`; track binding code has no `setValue(` on sliders; no `FmTrackPanel` include in binding files.

### References

- [Source: `_bmad-output/planning-artifacts/epics.md` — Epic 10, Story 10.3; UX-DR2]
- [Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md` — FR-12]
- [Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md` — § TrackGeneratorDisplay, D-017, D-018]
- [Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md` — D-016, D-017, D-018, D-019]
- [Source: `_bmad-output/planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md` — E10 (treat `src/` as stale)]
- [Source: `_bmad-output/project-context.md` — threading, testing pyramid, anti-patterns]
- [Source: `_bmad-output/implementation-artifacts/10-1-remove-interactivedisplayapvtssync-bridge.md` — predecessor]
- [Source: `_bmad-output/implementation-artifacts/10-2-envelopedisplay-direct-apvts-editing.md` — envelope pattern + review patch]
- [Source: `Source/GUI/Widgets/TrackGeneratorDisplay.{h,cpp}` — preserve]
- [Source: `Source/GUI/.../PatchEditDisplaysPanel.{h,cpp}` — UPDATE]
- [Source: `Source/GUI/.../Modules/EnvelopeDisplayApvtsBinding.{h,cpp}` — copy template]
- [Source: `Source/Shared/Definitions/PluginIDs.h` — FmTrack parameter ID SSOT]
- [Source: `Source/GUI/.../FmTrackPanel.{h,cpp}` — leave alone; attachments already exist]
- [Source: git `a65683e^:.../InteractiveDisplayApvtsSync.*` — salvage track helpers only]

## Dev Agent Record

### Agent Model Used

Composer (Cursor agent)

### Debug Log References

- Mirrored `EnvelopeDisplayApvtsBinding` (gesture gate + `callAsync` + `SafePointer`) for track points Point 1–5.
- Build: `cmake --build --preset macos-debug-arm64` green.
- `Matrix-Control_Tests`: exit 0 on re-run; one intermittent MidiManager outbound-activity failure on first run (unrelated to GUI binding).

### Completion Notes List

- Added `TrackGeneratorDisplayApvtsBinding`: one display + five `PluginIDs` (`fmTrackPoint1`…`5`); owned ×1 by `PatchEditDisplaysPanel`.
- Bidirectional sync via APVTS only: point drag → `setValueNotifyingHost` + gesture pair; slider / external → parameter listener → `callAsync` + `setTrackPointN(..., false)`.
- No `FmTrackPanel` / `ParameterCell` / `Slider` includes; no `slider.setValue` from binding.
- `TrackGeneratorDisplay` paint / hit-test / Y-only drag / square markers untouched.
- Envelope bindings and `PatchNameDisplayPanel` left unchanged (10.2 / 1.6 regression surface preserved in code).
- **Manual GUI checklist (AC #5):**

  | # | Check | Expected | Agent |
  |---|-------|----------|-------|
  | 1 | Standalone Debug launches; PATCH EDIT; Track Generator curve visible | OK | Human smoke test 2026-07-20 |
  | 2 | Move FmTrack Point 1–5 sliders → curve points follow; no crash | OK | Human smoke test 2026-07-20 |
  | 3 | Drag each of five display points (Y only) → matching sliders follow; values stick | OK | Human smoke test 2026-07-20 |
  | 4 | Markers remain small filled squares (not circles) | OK | Human smoke test 2026-07-20 |
  | 5 | Points do not move horizontally when dragging (Y-only) | OK | Human smoke test 2026-07-20 |
  | 6 | Load / init / mutate → display refreshes | OK | Human smoke test 2026-07-20 |
  | 7 | Env1 Delay (or ADSR) slider ↔ curve still bidirectional | OK | Human smoke test 2026-07-20 |
  | 8 | Patch name display still updates | OK | Human smoke test 2026-07-20 |
  | 9 | UI Scale + skin switch still apply to display band | OK | Human smoke test 2026-07-20 |
  | 10 | Grep: no `InteractiveDisplayApvtsSync`; binding has no slider `setValue` / no `FmTrackPanel` include | OK | Verified |

### File List

- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/TrackGeneratorDisplayApvtsBinding.h` (new)
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/TrackGeneratorDisplayApvtsBinding.cpp` (new)
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/PatchEditDisplaysPanel.h` (modified)
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/PatchEditDisplaysPanel.cpp` (modified)
- `CMakeLists.txt` (modified)
- `_bmad-output/implementation-artifacts/10-3-trackgeneratordisplay-direct-apvts-editing.md` (modified)
- `_bmad-output/implementation-artifacts/sprint-status.yaml` (modified)

### Review Findings

- [x] [Review][Defer] Stale `callAsync` can overwrite display after gesture ends [`TrackGeneratorDisplayApvtsBinding.cpp:parameterChanged`] — deferred, pre-existing (same gesture-gate pattern as `EnvelopeDisplayApvtsBinding`; gate only skips while active — a pre-drag queued lambda can still apply after `endChangeGesture`; hardening would need a generation token across envelope + track bindings)

## Change Log

- 2026-07-20: Story context created (ready-for-dev) — TrackGeneratorDisplay direct APVTS editing guide.
- 2026-07-20: Implemented `TrackGeneratorDisplayApvtsBinding`; wired Track Generator display to APVTS Point 1–5; build + tests green; status → review.
- 2026-07-20: Human smoke test OK; story → done.
- 2026-07-20: Code review — 0 decision / 0 patch / 1 defer (stale async after gesture, shared with 10.2 pattern); status remains done.
