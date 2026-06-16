---
organization: Ten Square Software
project: Matrix-Control
title: Story 7.10 — About Modal (Logo Popup)
author: BMad Agent
status: ready-for-dev
baseline_commit: HEAD
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - implementation-artifacts/7-7-settings-page-consolidation.md
  - implementation-artifacts/manual-uat-epic2-midi-audio-hardware.md
  - project-context.md
created: 2026-06-16
updated: 2026-06-16
---

# Story 7.10: About Modal (Logo Popup)

Status: ready-for-dev

<!-- Extends FR-41 logo popup (D-014a). Complements Story 7.7 Settings window — About is read-only product info. -->

## Story

As a Matrix-Control user,
I want an **About** dialog from the logo popup menu,
so that I can see product version, authorship, and links without hunting in the build system (UAT journal gap, FR-41 partial).

## Acceptance Criteria

1. **Logo popup menu** (`HeaderLogoPopupMenu`) — rename existing entry:
   - `PluginDisplayNames::HeaderPanel::kSettingsButton` → **`"SETTINGS..."`** (ellipsis literal)
2. **New menu entry** directly **below** SETTINGS (same column, next row after horizontal rule or spacer pattern):
   - Label **`"ABOUT..."`** — new constant `kAboutButton` in `PluginDisplayNames::HeaderPanel`
   - `ItemKind::About` (or reuse action item pattern) + `onAboutRequested` callback wired like Settings
3. **Column widths** — both logo popup columns widened so **SETTINGS...** and **ABOUT...** are not truncated:
   - Update `TSS::Design::Panels::Header::kLogoPopupColumnWidth` (currently **56** in `DesignPanels.h`)
   - New value **divisible by 4** (recommend **80** design px — validate at 100 % and 200 % UI scale)
   - Popup total width recalculates from `getColumnWidth(0) + separator + getColumnWidth(1)` (unchanged formula)
4. **About modal** — opens on ABOUT... click; reuse shell patterns from `SettingsWindow` / `SettingsPanel`:
   - Centred modal over plugin editor / standalone window
   - Close via **X**, **Escape**, click outside (match Settings behaviour)
   - `tss::ISkin` colours and fonts ; no new visual language
   - **Read-only** — no editable fields
5. **About content** (exact copy — English UI per product strings convention):

   ```
   MATRIX-CONTROL

   Modern SysEx MIDI editor for the Oberheim Matrix-1000/6/6R synthesizers.

   • Organization : Ten Square Software
   • Author : Guillaume DUPONT
   • Email : tensquaresoftware@gmail.com
   • GitHub : github.com/tensquaresoftware/Matrix-Control
   • LinkedIn : in/guillaumedupontmontpellier/
   • Version : <version string>
   • Release date : <YYYY-MM-DD>
   ```

   - **Email / GitHub / LinkedIn** : clickable `juce::URL` links (`mailto:`, `https://github.com/...`, `https://www.linkedin.com/in/...`)
   - Title **MATRIX-CONTROL** : prominent (Orbitron or existing logo font if available via skin)
6. **Version string policy** (reserve **1.0.0** for official public release):

   | Phase | Display example | Source |
   |---|---|---|
   | **Now (alpha, solo testing)** | `0.0.18-alpha` | `PROJECT_VERSION` from CMake + `-alpha` suffix |
   | Future beta | `0.1.0-beta` | bump CMake minor + `-beta` |
   | Official release | `1.0.0` | CMake `1.0.0`, no suffix |

   - Implement `Shared/Definitions/PluginVersion.h` (or `.cpp`) exposing:
     - `getVersionString()` → `"0.0.18-alpha"` (suffix from build flag `MATRIX_CONTROL_PRERELEASE_SUFFIX="alpha"` in CMake)
     - `getReleaseDateString()` → ISO date at **configure time** via CMake `string(TIMESTAMP MATRIX_CONTROL_BUILD_DATE "%Y-%m-%d")` passed as compile definition
   - **Do not** hardcode version in GUI — single SSOT from CMake `project(Matrix-Control VERSION 0.0.18)`
   - Document in `Documentation/` or Dev Agent Record: bump `project(VERSION …)` when cutting test builds ; switch suffix `alpha` → `beta` → none at milestones
7. **Regression** : logo popup Skin + UI Scale selection unchanged ; Settings still opens from SETTINGS... ; builds VST3 + Standalone clean.
8. **Tests** (lightweight):
   - Unit or compile-time test: `getVersionString()` contains `PROJECT_VERSION` major.minor.patch and `-alpha` while prerelease flag set
   - Manual: open About → version matches CMake ; links open default browser

## Tasks / Subtasks

- [ ] **Display names + design** (AC: #1–3)
  - [ ] `kSettingsButton` → `"SETTINGS..."` ; add `kAboutButton` = `"ABOUT..."`
  - [ ] Bump `kLogoPopupColumnWidth` to 80 (or measured minimum ÷4)
  - [ ] Adjust `buildItems()` row indices for ABOUT below SETTINGS

- [ ] **HeaderLogoPopupMenu wiring** (AC: #2)
  - [ ] `ItemKind::About`, `onAboutRequested_` callback
  - [ ] `HeaderLogoPopupMenu::show(...)` + `Logo` / `HeaderPanel` pass-through
  - [ ] `PluginEditor` or `HeaderPanel` opens About modal

- [ ] **PluginVersion SSOT** (AC: #6)
  - [ ] CMake: `MATRIX_CONTROL_PRERELEASE_SUFFIX`, `MATRIX_CONTROL_BUILD_DATE`
  - [ ] `PluginVersion.h` — `getVersionString()`, `getReleaseDateString()`

- [ ] **AboutWindow + AboutPanel** (AC: #4–5)
  - [ ] New `Source/GUI/About/AboutWindow.{h,cpp}` + `AboutPanel.{h,cpp}` (mirror Settings structure)
  - [ ] Static text constants in `PluginDisplayNames::About` namespace
  - [ ] Clickable URLs

- [ ] **CMake + tests** (AC: #7–8)
  - [ ] Add new `.cpp` to `PLUGIN_SOURCES` + test target
  - [ ] `PluginVersionTests.cpp` optional

## Dev Notes

### Brownfield anchors

| Component | Path | Today |
|---|---|---|
| Logo popup | `Source/GUI/Widgets/HeaderLogoPopupMenu.{h,cpp}` | SETTINGS at col 1 row 5 ; column width 56 |
| Settings modal | `Source/GUI/Settings/SettingsWindow.{h,cpp}` | Pattern to mirror |
| Settings label | `PluginDisplayNames.h` | `kSettingsButton = "SETTINGS"` |
| CMake version | `CMakeLists.txt:48` | `project(Matrix-Control VERSION 0.0.18)` |

### Version recommendation for Guillaume

**Display now:** `0.0.18-alpha`

- Aligns with current CMake `0.0.18`
- `-alpha` signals pre-release (solo hardware UAT) without consuming `1.0.0`
- **Release date:** date of CMake configure (or `git commit` date if preferred — document choice in Completion Notes)

**Roadmap:**

```
0.0.x-alpha  →  solo testing (now)
0.x.0-beta   →  public beta testers
1.0.0        →  official plugin release (store / website)
```

### Menu layout after change

```
Column 0 (SCALE)     | Column 1 (SKIN)
50% … 200%           | BLACK / CREAM
                     | [spacer]
                     | ─────────
                     | SETTINGS...
                     | ABOUT...
```

### Out of scope

- Relocate Skin/Scale out of popup (Story 7.8)
- Footer version / Device Inquiry (Epic 8.2, FR-53)
- Auto-update checker
- Localized About strings (English only v1)

### References

- FR-41 partial — [prd.md §4.11]
- D-014a logo popup — [prd/.decision-log.md]
- UAT version gap — [manual-uat-epic2-midi-audio-hardware.md §10]
- Settings shell — [7-7-settings-page-consolidation.md]

## Dev Agent Record

### Agent Model Used

<!-- filled on implementation -->

### Completion Notes

<!-- filled on implementation -->

### File List

<!-- filled on implementation -->
