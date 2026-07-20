---
baseline_commit: a65683ecacffe5e61a2b462d91cd518508b123e0
---

# Story 10.2: EnvelopeDisplay Direct APVTS Editing

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

## Story

As a sound designer,
I want to drag envelope curves to edit parameters,
so that editing matches addendum geometry and writes APVTS directly (FR-11, UX-DR1, D-018 second pass).

## Acceptance Criteria

1. **Given** Story 10.1 is done (bridge removed; displays currently silent)  
   **When** `PatchEditDisplaysPanel` wires the three `EnvelopeDisplay` instances to APVTS  
   **Then** dragging a curve point / sustain segment on Env1, Env2, or Env3 writes the matching `AudioParameterInt` via `setValueNotifyingHost`  
   **And** matched `beginChangeGesture` / `endChangeGesture` are paired for the active drag  
   **And** no code path calls `slider.setValue(...)` (or otherwise reaches into `ParameterCell` / `Env*Panel`) from the display side

2. **Given** Env1 / Env2 / Env3 Delay, Attack, Decay, Sustain, Release sliders (existing `SliderAttachment`)  
   **When** the user moves any of those sliders  
   **Then** the corresponding envelope display updates to match (via APVTS parameter listener → `setXxx(..., notify=false)`)  
   **And** the reverse path (curve drag → APVTS → attachment) updates the slider without a Display→Slider bridge

3. **Given** external APVTS updates (preset load, automation, MIDI echo, init)  
   **When** any of the 15 envelope ADSR int parameters change  
   **Then** the matching display refreshes without feedback loops  
   **And** `PluginProcessor` / SysEx outbound behaviour remains unchanged (UI does not “resync” Core)

4. **Given** addendum § EnvelopeDisplay and D-017  
   **When** the three envelope modules are used  
   **Then** drag geometry and visual markers remain as already implemented (small filled squares; Delay/Attack/Decay/Sustain/Release rules)  
   **And** this story does **not** rewrite paint / hit-test / geometry code unless a proven regression requires a minimal fix

5. **Given** the wiring lands  
   **When** validation is recorded  
   **Then** a GUI manual checklist is documented in Dev Agent Record / Completion Notes  
   **And** macOS Debug build + `Matrix-Control_Tests` remain green  
   **And** Track Generator stays the accepted interim gap until Story 10.3 (display still silent / no APVTS write)

## Tasks / Subtasks

- [x] Task 1 — Add envelope-only Display↔APVTS binding (AC: #1, #2, #3)
  - [x] Introduce a small helper under `PatchEditDisplaysPanel/Modules/` (recommended name: `EnvelopeDisplayApvtsBinding`) that binds **one** `TSS::EnvelopeDisplay&` + five `PluginIDs` strings to one APVTS
  - [x] Own three instances from `PatchEditDisplaysPanel` (Env1 / Env2 / Env3) — do **not** fold Track Generator into this helper (Story 10.3)
  - [x] Register `apvts.addParameterListener` for the five IDs; remove listeners in destructor
  - [x] On `parameterChanged`: read int via `convertFrom0to1(getValue())`, clamp `[0,63]`, apply with `setDelay/Attack/Decay/Sustain/Release(value, false)`
  - [x] **Threading:** `EnvelopeDisplay` setters call `repaint()` — never call them directly from an audio-thread listener; marshal with `juce::MessageManager::callAsync` (or equivalent message-thread hop). Guard against destroyed owner (SafePointer / weak flag)
  - [x] Wire `setOnValueChanged` → `writeIntParameter` (`setValueNotifyingHost(convertTo0to1(...))`)
  - [x] Wire `setOnEditGestureBegin` / `setOnEditGestureEnd` → `beginChangeGesture` / `endChangeGesture` on the active parameter (single active gesture, matching widget + deleted bridge)
  - [x] Call initial sync on construction (seed all five values with `notify=false`)
  - [x] Add new `.cpp` to `CMakeLists.txt` `PLUGIN_SOURCES`

- [x] Task 2 — Hook from `PatchEditDisplaysPanel` (AC: #1, #2, #5)
  - [x] Remove `[[maybe_unused]]` from `apvts_` once binding uses it
  - [x] Construct the three bindings after displays exist (ctor body or member-init after displays)
  - [x] Do **not** reintroduce `connectSliderFastPaths`, cross-panel slider listeners, or any Env panel includes
  - [x] Leave `TrackGeneratorDisplay`, `PatchNameDisplayPanel`, layout / skin / scale unchanged

- [x] Task 3 — Preserve widget geometry / D-017 (AC: #4)
  - [x] Do **not** change `EnvelopeDisplay` paint, hit-test, or drag math unless fixing a documented wiring bug
  - [x] Do **not** change marker shape (filled squares via `fillRect`)
  - [x] Do **not** delete or rename `onValueChanged` / gesture callback API

- [x] Task 4 — Verify & document (AC: #2, #3, #5)
  - [x] Grep: zero hits for `InteractiveDisplayApvtsSync`; no new `slider.setValue` from display binding code
  - [x] Build `macos-debug-arm64` + run `Matrix-Control_Tests`
  - [x] Fill manual checklist in Completion Notes (see Testing)

## Dev Notes

### Scope fence (critical)

| In scope | Out of scope |
|----------|--------------|
| Direct Env1/2/3 Display ↔ APVTS (15 int params) | Track Generator Display ↔ APVTS (Story **10.3**) |
| Gesture pairing on curve drag | Rewriting EnvelopeDisplay geometry / D-017 markers |
| Message-thread-safe listener → display updates | Core / MIDI / SysEx / PatchModel / DirtyPatchTracker |
| CMake entry for new binding `.cpp` | Patch name display (Story 1.6) |
| Manual GUI checklist | Epic U layout / Design* / Factory audits |
| | Resurrecting Display→Slider mediation |

**Product meaning:** after 10.1 the curves went quiet. 10.2 turns the three envelope displays back on the **D-018** way: APVTS is SSOT; sliders stay on stock `SliderAttachment`; displays listen and write parameters — never touch sliders.

### Target architecture (D-018) — must follow

[Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md` § Target sync architecture (D-018)]

- APVTS `AudioParameter` is SSOT (here: `AudioParameterInt` 0–63).
- **Slider → curve:** `SliderAttachment` writes APVTS; display listens and applies setters with `notify = false`.
- **Curve → slider:** display callbacks write `setValueNotifyingHost` + gestures; attachments update sliders — **never** `slider.setValue()` from display code.
- **External updates:** same parameter listeners refresh displays.
- `PluginProcessor` handles SysEx / business logic — not UI resync.

**History:** D-018 first pass (2026-05-27, `7d50bfb`) created `InteractiveDisplayApvtsSync` (still pushed `slider.setValue`). Story 10.1 (`a65683e`) deleted that bridge. This story is the **envelope half** of the second pass. Do not recreate the mediator pattern (no slider map, no `connectSliderFastPaths`, no Env panel includes).

### Recommended design (prevent reinventing the wheel)

Prefer a **per-envelope** binding helper (one display + five IDs), owned ×3 by the panel:

```text
PatchEditDisplaysPanel
  ├─ envelope1Display_ + EnvelopeDisplayApvtsBinding (env1Delay…Release)
  ├─ envelope2Display_ + EnvelopeDisplayApvtsBinding (env2…)
  ├─ envelope3Display_ + EnvelopeDisplayApvtsBinding (env3…)
  ├─ trackGeneratorDisplay_          ← still unwired (10.3)
  └─ patchNameDisplayPanel_          ← leave alone
```

Reuse the deleted bridge’s **good** bits (int read/write, binding tables, gesture pairing, `paramIndex` 0..4 mapping) and drop the **bad** bits (slider listeners, `updateSliderUi`, track points, cross-panel includes).

Recover reference implementation from git if useful:

```bash
git show a65683e^:Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/InteractiveDisplayApvtsSync.cpp
git show a65683e^:Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/InteractiveDisplayApvtsSync.h
```

Focus on: `connectEnvelopeDisplay`, `writeIntParameter` / `readIntParameter`, `beginParameterGesture` / `endParameterGesture`, `applyEnvelopeDisplayValue`, `kEnvelope{1,2,3}Bindings`. Ignore: `connectSliderFastPaths`, `SliderToDisplayListener`, `updateSliderUi`, track helpers.

### Current state — files being modified

#### `EnvelopeDisplay` (READ / preserve — UPDATE only if wiring bug forces it)

- Path: `Source/GUI/Widgets/EnvelopeDisplay.{h,cpp}`
- Value model: `int` clamped `[0,63]` — matches Env Delay…Release `AudioParameterInt`
- `paramIndex` in callbacks: `0=Delay, 1=Attack, 2=Decay, 3=Sustain, 4=Release`
- Two-arg setters with `notify=false` suppress `onValueChanged_` (required for APVTS→display)
- Drag path calls `onValueChanged_` / gesture begin-end directly (binding attaches here)
- Markers: `g.fillRect` small squares (D-017 already done)
- Geometry: already matches addendum § EnvelopeDisplay (do not re-open)

#### `PatchEditDisplaysPanel` (UPDATE)

- Path: `.../PatchEditDisplaysPanel/PatchEditDisplaysPanel.{h,cpp}`
- Owns three `EnvelopeDisplay` value members + `apvts_*` pointer kept for this story (`[[maybe_unused]]` today)
- Ctor signature already takes `AudioProcessorValueTreeState&` — no parent signature churn
- After 10.1: no listeners, no display callbacks, no initial sync

#### New module (NEW)

- Suggested: `.../PatchEditDisplaysPanel/Modules/EnvelopeDisplayApvtsBinding.{h,cpp}`
- Sibling of `PatchNameDisplayPanel` under `Modules/`
- Must **not** include Env panels, `ParameterCell`, or `Slider`

#### Keep untouched

- `ParameterCell` / `SliderAttachment` — already keep sliders in sync with APVTS
- `Env1Panel` / `Env2Panel` / `Env3Panel` — no includes from the binding
- `TrackGeneratorDisplay` + any track wiring — Story 10.3
- `PatchNameDisplayPanel` — string property listener, not int params
- `Source/Core/**`
- `TestEnvelopeDisplays.*` — visual harness only; optional demo callbacks not required by AC

### Parameter IDs (SSOT — never new literals)

From `Source/Shared/Definitions/PluginIDs.h`:

| Module | IDs |
|--------|-----|
| Env1 | `Envelope1Module::ParameterWidgets::kDelay` … `kRelease` → `env1Delay` … `env1Release` |
| Env2 | `env2Delay` … `env2Release` |
| Env3 | `env3Delay` … `env3Release` |

Binding table order **must** match `paramIndex` 0..4 (Delay, Attack, Decay, Sustain, Release).

### Int ↔ normalised conversion (copy from deleted bridge)

```cpp
// read
auto* p = apvts.getParameter(id);
int value = juce::jlimit(0, 63, juce::roundToInt(p->convertFrom0to1(p->getValue())));

// write
p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(juce::jlimit(0, 63, value))));
```

No per-descriptor remapping needed for these five params (unlike bipolar AmplitudeModByVelocity).

### Addendum geometry (preserve — acceptance evidence, not rewrite target)

[Source: addendum.md § Interactive display geometry — EnvelopeDisplay]

| Parameter | Drag | Notes |
|-----------|------|-------|
| Delay | Horizontal duration at bottom | Max ~20% width |
| Attack | Horizontal at top | Value 0 not fully vertical |
| Decay | Horizontal; Y follows Sustain | **No direct vertical drag on Decay** |
| Sustain | Vertical via point/segment | Fixed horizontal length |
| Release | Horizontal at bottom | Value 0 not fully vertical |

Markers: small filled squares (D-017). Already implemented — verify in manual checklist, do not “improve”.

### Architecture compliance

| Rule | Implication for 10.2 |
|------|----------------------|
| GUI → Core → Shared; Core ↛ GUI | Touch **GUI only** (+ CMake) |
| Real paths use `Source/` | Architecture doc `src/` paths are **stale** |
| APVTS IDs only in `PluginIDs.h` | No new ID strings |
| APVTS listeners may be off message thread | `callAsync` before `setXxx` / `repaint` |
| No GUI unit-test mandate | Manual checklist + existing unit tests |
| English-only source | Checklist English in story file OK |

[Source: `_bmad-output/project-context.md`; architecture E10 mapping]

### Anti-patterns to refuse

- Recreating `InteractiveDisplayApvtsSync` (or `*Sync2`) that still maps displays to sliders
- Listening to `juce::Slider` to refresh the curve (use APVTS listeners only)
- Calling `slider.setValue` / `ParameterCell::getSlider()` from display callbacks
- Wiring Track Generator “while we’re here” (breaks epic split; Story 10.3)
- Changing Decay vertical-drag rules or marker shape “for polish”
- Putting French text in source
- Moving sync logic into `Source/Core/`
- Skipping `callAsync` because `RampPortamentoPanel` currently refreshes inline — that panel only toggles enablement; envelope setters **repaint** and must be message-thread safe per project-context

### Project Structure Notes

- New files live under panel `Modules/` next to `PatchNameDisplayPanel`
- After 10.2, `Modules/` holds: `PatchNameDisplayPanel`, `EnvelopeDisplayApvtsBinding` (name may vary if justified in Completion Notes)
- CMake: add one `PLUGIN_SOURCES` line for the new `.cpp`
- Naming: prefer `*Binding` / `*Attachment` over `*Sync` to avoid implying the deleted bridge pattern

### Previous story intelligence (10.1)

- Bridge fully removed; interim gap documented: envelope/track displays silent until 10.2 / 10.3
- Widget callback hooks intentionally preserved for this story
- `apvts_` kept on panel with `[[maybe_unused]]` — remove the attribute when used
- Do not treat Track silence as a 10.2 bug
- Review decision on 10.1: automated gates + code review sufficient; interactive checklist optional later — for **10.2**, interactive bidirectional checks are **in scope** (AC #5) because behaviour is restored

[Source: `_bmad-output/implementation-artifacts/10-1-remove-interactivedisplayapvtssync-bridge.md`]

### Git intelligence

- `a65683e` — Remove InteractiveDisplayApvtsSync bridge for Epic 10 (baseline for this story)
- `7d50bfb` — original bridge introduction (reference for read/write/gesture patterns to salvage)
- Recent work is Epic 9 + docs — no conflicting WIP expected on envelope widgets

### Latest tech notes (JUCE 8.0.12)

- Project pin: JUCE **8.0.12** (`/Applications/JUCE`)
- Prefer `AudioProcessorParameter::beginChangeGesture` / `endChangeGesture` + `setValueNotifyingHost` on the message thread (display mouse callbacks already are)
- `AudioProcessorValueTreeState::Listener::parameterChanged` is **not** guaranteed on the message thread — hop before GUI mutation
- No stock “curve attachment”; custom binding is expected
- `SafePointer<Component>` or an atomic “alive” flag recommended when posting `callAsync` from listeners

### Testing requirements

**Automated:** No requirement to invent GUI unit tests. Run `Matrix-Control_Tests` as regression gate. Do not put tests in `paint()` / audio thread.

**Manual checklist (required — paste results into Completion Notes):**

1. Standalone Debug launches; PATCH EDIT visible; three envelope curves visible.
2. Move Env1 Delay / Attack / Decay / Sustain / Release sliders → **curves follow** in real time; no crash.
3. Drag Env1 Delay / Attack / Decay / Sustain (point or segment) / Release on the display → **sliders follow**; values stick after mouse up.
4. Repeat sample slider + drag checks on Env2 and Env3 (at least one param each direction).
5. Confirm markers remain small filled squares (not circles).
6. Confirm Decay point does not offer independent vertical drag (Sustain owns vertical).
7. Load / init / mutate a patch (or change params externally) → displays refresh.
8. **Known remaining interim:** Track Generator display still does **not** follow Point 1–5 sliders and drag does **not** write APVTS (Story 10.3).
9. Patch name display still updates (Story 1.6 regression).
10. UI Scale + skin switch still apply to the display band.
11. Grep clean: no `InteractiveDisplayApvtsSync`; binding code has no `setValue(` on sliders.

### References

- [Source: `_bmad-output/planning-artifacts/epics.md` — Epic 10, Story 10.2; UX-DR1]
- [Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md` — FR-11]
- [Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md` — § EnvelopeDisplay, D-017, D-018]
- [Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md` — D-016, D-017, D-018, D-019]
- [Source: `_bmad-output/planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md` — E10 (treat `src/` as stale)]
- [Source: `_bmad-output/project-context.md` — threading, testing pyramid, anti-patterns]
- [Source: `_bmad-output/implementation-artifacts/10-1-remove-interactivedisplayapvtssync-bridge.md` — predecessor]
- [Source: `Source/GUI/Widgets/EnvelopeDisplay.{h,cpp}` — preserve]
- [Source: `Source/GUI/.../PatchEditDisplaysPanel.{h,cpp}` — UPDATE]
- [Source: `Source/Shared/Definitions/PluginIDs.h` — Env parameter ID SSOT]
- [Source: git `a65683e^:.../InteractiveDisplayApvtsSync.*` — salvage patterns only]

## Dev Agent Record

### Agent Model Used

Composer (Cursor agent)

### Debug Log References

- Salvaged read/write/gesture patterns from `a65683e^:.../InteractiveDisplayApvtsSync.*` (envelope-only; dropped slider map / fast paths).
- Build: `cmake --preset macos-debug-arm64` + build green; `MATRIX_BUILD_TESTS=ON` → `Matrix-Control_Tests` exit 0.

### Completion Notes List

- Added `EnvelopeDisplayApvtsBinding`: one display + five `PluginIDs` (Delay…Release); owned ×3 by `PatchEditDisplaysPanel`.
- Bidirectional sync via APVTS only: display drag → `setValueNotifyingHost` + gesture pair; slider / external → parameter listener → `callAsync` + `setXxx(..., false)`.
- No Env panel / `ParameterCell` / `slider.setValue` paths; Track Generator left silent (Story 10.3).
- `EnvelopeDisplay` paint / hit-test / markers untouched.
- **Manual GUI checklist (AC #5):**

  | # | Check | Expected | Agent |
  |---|-------|----------|-------|
  | 1 | Standalone Debug launches; PATCH EDIT; three envelope curves visible | OK | Human smoke test 2026-07-20 |
  | 2 | Move Env1 Delay/Attack/Decay/Sustain/Release sliders → curves follow; no crash | OK | Human smoke test 2026-07-20 |
  | 3 | Drag Env1 Delay/Attack/Decay/Sustain/Release on display → sliders follow; values stick | OK | Human smoke test 2026-07-20 |
  | 4 | Sample slider + drag on Env2 and Env3 (one param each direction) | OK | Human smoke test 2026-07-20 |
  | 5 | Markers remain small filled squares (not circles) | OK | `EnvelopeDisplay` paint untouched |
  | 6 | Decay has no independent vertical drag (Sustain owns vertical) | OK | Geometry untouched |
  | 7 | Load / init / mutate → displays refresh | OK | Human smoke test 2026-07-20 |
  | 8 | **Known interim:** Track Generator display still silent (no slider follow / no APVTS write) | Expected until 10.3 | By design |
  | 9 | Patch name display still updates | OK | `PatchNameDisplayPanel` untouched |
  | 10 | UI Scale + skin switch still apply to display band | OK | `setUiScale` / `setSkin` unchanged |
  | 11 | Grep: no `InteractiveDisplayApvtsSync` in `Source/`; binding has no slider `setValue` | OK | Verified |

### File List

- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/EnvelopeDisplayApvtsBinding.h` (new)
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/EnvelopeDisplayApvtsBinding.cpp` (new)
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/PatchEditDisplaysPanel.h` (modified)
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/PatchEditDisplaysPanel.cpp` (modified)
- `CMakeLists.txt` (modified)
- `_bmad-output/implementation-artifacts/10-2-envelopedisplay-direct-apvts-editing.md` (modified)
- `_bmad-output/implementation-artifacts/sprint-status.yaml` (modified)

### Review Findings

- [x] [Review][Decision] Validate story without a human Standalone GUI pass? — resolved: human Standalone checklist required before done (option 1).
- [x] [Review][Patch] Ignore APVTS→display echo while curve drag gesture is active (and avoid stale async apply) [`EnvelopeDisplayApvtsBinding.cpp:143`] — skip `parameterChanged` / queued `callAsync` when the active gesture gate matches that parameter.

## Change Log

- 2026-07-20: Story context created (ready-for-dev) — EnvelopeDisplay direct APVTS editing guide.
- 2026-07-20: Implemented `EnvelopeDisplayApvtsBinding` ×3; wired Env1–3 displays to APVTS; build + tests green; status → review.
- 2026-07-20: Code review — 1 decision-needed, 1 patch; ~10 findings dismissed (noise / by design / pre-existing pattern).
- 2026-07-20: Review patch — ignore APVTS→display updates while curve drag gesture is active (shared gesture gate + async re-check).
- 2026-07-20: Human smoke test OK; story → done.
