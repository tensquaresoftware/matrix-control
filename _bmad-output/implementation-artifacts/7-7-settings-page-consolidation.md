---
organization: Ten Square Software
project: Matrix-Control
title: Story 7.7 — Settings Page Consolidation
author: BMad Agent
status: done
baseline_commit: 114bd7c979ca7e967ea8262cf20f37922ad5eae1
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - implementation-artifacts/r-2-hardware-latency-gui.md
  - implementation-artifacts/deferred-work.md
  - project-context.md
created: 2026-06-06
updated: 2026-06-12
---

# Story 7.7: Settings Page Consolidation

Status: done

<!-- FR-40 (partial Phase A), FR-4b relocation, FR-18/FR-59/NFR-7 Phase B. Spawned from R-2 code review — header layout overlap + Guillaume decision for dedicated Settings window with tabs. -->

## Story

As a Matrix-Control user,
I want a dedicated **Settings** window with clear sections for common, plugin-only, and standalone-only options,
so that header routing stays compact and advanced prefs (audio, latency, policies, Master ops) are easy to find (FR-40).

## Acceptance Criteria

### Phase A — Settings shell + header prefs migration (implement in this story)

#### AC A1 — Entry point and window shell

1. **SETTINGS** control in header (replace or supplement interim layout pressure): opens a modal or child `DocumentWindow` / `DialogWindow` centred on the plugin editor — follow existing JUCE + `tss::` skin patterns.
2. Window title: `Settings` (or `PluginDisplayNames` equivalent); close via **X**, **Escape**, or explicit **Close** button.
3. Window size: design dimensions divisible by 4; content scrolls if needed at high UI scale (125–200 %).
4. Settings open/close does **not** block audio/MIDI threads; all writes remain on message thread (same as header sliders today).

#### AC A2 — Tabbed layout (Common / Plugin / Standalone)

1. Three tabs (labels in `PluginDisplayNames::Settings` namespace):
   - **Common** — prefs valid in hosted plugin **and** standalone.
   - **Plugin** — hosted-only (e.g. Hardware Latency for DAW PDC).
   - **Standalone** — standalone-only (audio input routing, input gain, peak context).
2. Tab visibility: **Common** always; **Plugin** hidden in standalone; **Standalone** hidden in hosted plugin.
3. Use existing tab or segmented-control widget if present; otherwise minimal `TabbedComponent` styled via `ISkin`.

#### AC A3 — Controls migrated from header (remove from header after Settings wired)

| Control | Tab | Current location | Persistence |
|---------|-----|------------------|-------------|
| Hardware Latency (0–100 ms, step 0.1) | Plugin | `HeaderPanel` right cluster (R-2) | `hardwareLatencyMs` on `apvts.state` |
| Input Gain | Standalone | `HeaderPanel` audio packet | `inputGainDb` |
| Audio From | Standalone | `HeaderPanel` audio packet | `audioFromSourceId` |
| Peak indicator context (optional read-only level or label) | Standalone | header | display only — no new persistence |

1. Reuse **`tss::Slider`** with `SliderConfig` (min/max/default/step/unit) — no unit-specific hacks.
2. Wiring: same `PluginProcessor` setters / getters as `PluginEditor` header callbacks today; restore on open from APVTS.
3. **Remove** migrated controls (+ labels) from `HeaderPanel` layout; verify standalone header no longer overlaps `uiElementsButton_` (R-2 review defer).
4. Update `Documentation/hardware-latency.md` — control location → Settings → Plugin tab.

#### AC A4 — Common tab (minimum viable)

1. **Skin** and **UI scale** combos moved from header right cluster to **Common** tab (or keep header if UX test fails — prefer move per this story intent).
2. Persistence unchanged: `guiScaleId`, skin selection via existing APVTS/state properties.
3. If skin/scale remain in header temporarily, document in Dev Agent Record with rationale.

#### AC A5 — Regression and build

1. Hardware Latency: `setLatencySamples` + `updateHostDisplay` still fire on change (processor SSOT unchanged).
2. Standalone audio passthrough + peak indicator still functional after migration.
3. `HardwareLatencyTests` + existing unit tests pass; VST3 + Standalone build clean.

### Phase B — Full FR-40 consolidation (defer; separate tasks or follow-up story)

> Depends on Stories **3.2** (INIT templates), **4.5** (reconciliation policy), **6.10** (Defrag). Do **not** block Phase A on these.

1. Master load/save/init actions (FR-18)
2. Computer Patches reconciliation policy (FR-29)
3. Unsaved patch edit warning policy (FR-51)
4. Manual Defrag mutation history — disabled when empty (FR-59)
5. INIT template folder paths (`PatchInit.syx`, `MasterInit.syx`)
6. Logging opt-in (NFR-7)

## Tasks / Subtasks

- [x] **Phase A — Shell** (AC: A1, A2)
  - [x] Add `Source/GUI/Settings/` (or `Panels/SettingsPanel/`) with tabbed layout component
  - [x] `PluginDisplayNames::Settings` labels for tabs and section headers
  - [x] Header **SETTINGS** button → open/close; register in `PluginEditor`
  - [x] Mode-aware tab visibility (`PluginProcessor::isStandalone()`)

- [x] **Phase A — Migrate controls** (AC: A3)
  - [x] Plugin tab: Hardware Latency slider + doc link/tooltip
  - [x] Standalone tab: Audio From combo, Input Gain slider
  - [x] Remove migrated widgets from `HeaderPanel`; simplify `resized()` right cluster
  - [x] `PluginEditor`: move `onValueChange` / restore logic to Settings panel or shared helper

- [x] **Phase A — Common tab** (AC: A4)
  - [x] Skin + UI scale combos in Common tab
  - [x] Trim header right cluster to SKIN/SCALE removal if moved

- [x] **Phase A — Verify** (AC: A5)
  - [x] Manual UAT: plugin HW latency PDC; standalone audio path
  - [x] Run `Matrix-Control_Tests`; macOS-ARM-Debug VST3 + Standalone build

- [x] **Phase B — Backlog placeholders** (AC: B)
  - [x] Stub sections or disabled rows with "Coming soon" for Master / Policies / Defrag / Logging

## Dev Notes

### Problem / context

- **R-2** placed Hardware Latency in header right cluster → standalone layout overlap with UI ELEMENTS (~117 px @ 100 % scale). Interim header placement was acceptable until Settings exists.
- **Guillaume decision (2026-06-06):** dedicated Settings window; tabs for common / plugin / standalone; no further header layout hacks.
- **2-9b** explicitly deferred SETTINGS button — implement here.

### Architecture

- Settings UI: `GUI/Settings/` per architecture.md table (`Settings / Defrag` row).
- Panels stay presentation-only long-term (Epic 7.1 `ActionDispatcher`); Phase A may call `PluginProcessor` directly mirroring header pattern — refactor to handlers in 7.1 if needed.
- Persistence: continue `apvts.state` properties pattern (`inputGainDb`, `hardwareLatencyMs`, port IDs) — **not** new `AudioParameter`s unless spec changes.

### Reuse patterns

| Concern | Pattern |
|---------|---------|
| Sliders | `tss::Slider` + `SliderConfig` (step, unit, `minimumDisplayText` for `-inf` dB) |
| Latency host report | `PluginProcessor::setHardwareLatencyMs` → `applyHardwareLatencyToHost` |
| Standalone audio | `setAudioFromSourceId`, `setInputGainDb`, `refreshAudioFromCombo` |
| Skin/scale | `PluginEditor::applyUiScale`, `updateSkin`, `PluginIDs::Settings::kGuiScaleId` |

### Files likely touched

- **New:** `Source/GUI/Settings/SettingsPanel.h/.cpp` (or equivalent), `PluginDisplayNames` Settings section
- **Update:** `HeaderPanel.*`, `PluginEditor.cpp`, `PluginDisplayNames.h`, `Documentation/hardware-latency.md`, `CMakeLists.txt`
- **Preserve:** `PluginProcessor` hardware latency + audio APIs (no semantic change)

### Testing

- No new Core tests required for UI shell unless extracting wiring helpers.
- Regression: `HardwareLatencyTests`, audio passthrough tests if touched.

### References

- [epics.md Story 7.7]
- [prd.md FR-40, FR-18, FR-4b]
- [r-2-hardware-latency-gui.md] — interim header AC; relocate in A3
- [deferred-work.md] — header overlap deferred to this story

## Dev Agent Record

### Agent Model Used

Composer (Cursor)

### Completion Notes List

- Added `SettingsPanel` (480×360, tab bar COMMON / PLUGIN / STANDALONE) and `SettingsWindow` (`DocumentWindow`, Escape + native close).
- Header: **SETTINGS** + **UI Elements** only on right cluster; overlap R-2 resolved.
- Migrated Hardware Latency → Plugin tab; Audio From, Input Gain, Peak → Standalone tab; Skin + UI scale → Common tab.
- Phase B placeholders: Master Operations, Policies, Defrag, Logging ("Coming soon").
- Wiring unchanged at processor layer (`setHardwareLatencyMs`, `setAudioFromSourceId`, `setInputGainDb`, APVTS properties).
- Settings window scales with editor UI scale; recentres on scale change.
- `Documentation/hardware-latency.md` updated for Settings → Plugin tab location.
- Build: `default-macos-arm64` preset — VST3 + Standalone + `Matrix-Control_Tests` all green (exit 0).

### File List

- `Source/GUI/Settings/SettingsPanel.h` (new)
- `Source/GUI/Settings/SettingsPanel.cpp` (new)
- `Source/GUI/Settings/SettingsWindow.h` (new)
- `Source/GUI/Settings/SettingsWindow.cpp` (new)
- `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h` (modified)
- `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp` (modified)
- `Source/GUI/PluginEditor.h` (modified)
- `Source/GUI/PluginEditor.cpp` (modified)
- `Source/Shared/Definitions/PluginDisplayNames.h` (modified)
- `Documentation/hardware-latency.md` (modified)
- `CMakeLists.txt` (modified)

## Change Log

- 2026-06-06: Story created (ready-for-dev) — Phase A from R-2 review + Guillaume Settings window decision; Phase B FR-40 scope deferred.
- 2026-06-06: Phase A implemented — Settings window shell, header migration, build/tests green; status → review.
- 2026-06-12: Story closed — Phase A accepted; Phase B remains deferred to Stories 3.2, 4.5, 6.10; status → done.
