---
organization: Ten Square Software
project: Matrix-Control
title: Story 7.5 — Bank Utility UI Wiring
author: BMad Agent
status: review
baseline_commit: ab0a263
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/sprint-change-proposal-2026-06-19-bank-unlock-simplify.md
  - implementation-artifacts/7-3c-bank-utility-unlock-simplify.md
  - implementation-artifacts/7-4-mutatoractionhandler.md
  - implementation-artifacts/investigations/next-patch-after-unlock-missing-set-bank-investigation.md
  - project-context.md
created: 2026-07-11
updated: 2026-07-11
---

# Story 7.5: Bank Utility UI Wiring

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want bank buttons 0–9 and UNLOCK wired to Core state,
so that bank selection matches synth semantics (FR-19, FR-20, FR-21).

## Acceptance Criteria

1. **Given** Story 7-3c complete (Core bank/unlock semantics + display-only lock indicator writes) **When** user selects bank **0–9** **Then** the matching Bank Utility button shows **red label text**; all other bank buttons show default (non-red) text; highlight reads `patchManagerSelectedBank` (`kSelectedBank`) — **not** `internalPatchesCurrentBankNumber`.

2. **And** when `patchManagerSelectedBank` changes (bank button, session restore, or any Core write) **Then** Bank Utility highlight updates without requiring panel rebuild; listener registered in `BankUtilityPanel` on APVTS state tree.

3. **And** Internal Patches **Current Bank NumberBox** shows **red dot** when `patchManagerBanksLocked` (`kBanksLocked`) is `true` (D-023a-R3); dot hidden when `false` or on Matrix-6/6R (no bank concept). **Verify** existing `InternalPatchesPanel::refreshBankLockIndicator` — fix only if broken; do not remove dot logic.

4. **And** UNLOCK button displays label **UNLOCK** only — **no padlock icon** overlay or secondary glyph (regression guard from D-022-R1; already text-only after 7-3b).

5. **And** optional UNLOCK info footer (D-022-R4): on UNLOCK click (Matrix-1000/1000R, not grayed), set `uiMessageText` + `uiMessageSeverity` (`"info"`) with English string from `PluginDisplayNames` — same pattern as `kMatrix1000OnlyFooterMessage` in `BankUtilityPanel`. **Do not** add footer from Core handler unless GUI wiring is impossible (prefer panel-local for consistency with grayed-click footer).

6. **And** `patchManagerSelectedBank` stays consistent with Core bank coordinate: after bank select, `kSelectedBank` == `internalPatchesCurrentBankNumber`; on cold start / `resetInternalPatchCoordinatesToDefaults`, both default to `0`. If audit finds `kCurrentBankNumber` updated without `kSelectedBank`, add minimal Core mirror in that path only — **no** navigation coupling.

7. **And** Matrix-6/6R device gating unchanged (Story 8.5): entire Bank Utility module grayed at 50% alpha; grayed clicks show existing `kMatrix1000OnlyFooterMessage`; red highlight and UNLOCK footer **not** shown when grayed.

8. **And** **no** changes to `PatchManagerActionHandler` bank/unlock MIDI semantics, `DeviceMemoryLimits::advancePatch`, or navigation wrap rules (owned by 7-3c).

9. **And** manual smoke + builds green:
   - `cmake --build Builds/macOS --target Matrix-Control_Tests Matrix-Control_VST3 Matrix-Control_Standalone` (or platform preset)
   - `Matrix-Control_Tests` full suite
   - Standalone smoke checklist in Dev Notes

## Tasks / Subtasks

- [x] **Audit brownfield state** (AC: #3, #4, #6)
  - [x] Confirm `InternalPatchesPanel::refreshBankLockIndicator` wired to `kBanksLocked`
  - [x] Confirm UNLOCK is plain text button (no padlock asset)
  - [x] Grep `kSelectedBank` writers vs `kCurrentBankNumber` writers — list any desync paths

- [x] **Active bank red text** (AC: #1, #2, #7)
  - [x] Add `refreshSelectedBankHighlight()` to `BankUtilityPanel`
  - [x] Listen to `kSelectedBank` in `valueTreePropertyChanged` (+ `valueTreeRedirected` if needed)
  - [x] Map bank index 0–9 → `selectBankNButton_`; apply red text to selected, reset others
  - [x] Call once in ctor after button setup (initial session state)
  - [x] Preserve grayed alpha styling — red text only when `!bankUtilityGrayed_`

- [x] **UNLOCK optional footer** (AC: #5)
  - [x] Add `kUnlockBankFooterMessage` to `PluginDisplayNames.h` (English, D-022-R4 wording)
  - [x] In `unlockBankButton_->onClick`, after timestamp `setProperty`, call footer helper when `!bankUtilityGrayed_`

- [x] **kSelectedBank sync gap fix** (AC: #6 — only if audit finds gap)
  - [x] If `resetInternalPatchCoordinatesToDefaults` (or similar) sets `kCurrentBankNumber` without `kSelectedBank`, mirror in same function
  - [x] **Do not** sync from navigation-only paths (patch Prev/Next never changes bank per 7-3c)

- [ ] **Optional: transient dot flash** (AC: #3 — defer if non-trivial)
  - [ ] 7-3c review noted brief dot-off before `markBanksLockedInApvts` on bank select — reorder listener refresh or batch property writes only if visibly reproducible

- [x] **Self-review & smoke** (AC: #8, #9)
  - [x] No Core navigation/MIDI diffs unless AC #6 gap fix required
  - [x] Grep `BankUtilityPanel` — no SysEx, no handler calls
  - [x] Run tests + Standalone smoke (Dev Notes checklist)
  - [x] Clean Code limits on new panel methods

### Review Findings

- [x] [Review][Patch] `reconcilePatchManagerCoordinatesForDeviceType` ne synchronise pas `kSelectedBank` — décision Guillaume : miroir dans cette fonction à chaque écriture de `kCurrentBankNumber` (AC #6). [PluginProcessor.cpp:1457-1468]

- [x] [Review][Patch] `valueTreeRedirected` est un no-op — après restauration de session DAW (`replaceState`), le surlignage peut rester périmé si `kSelectedBank` ne change pas de valeur et aucun `valueTreePropertyChanged` ne se déclenche. Aligner sur `InternalPatchesPanel` : appeler `refreshDeviceGating()` + `refreshSelectedBankHighlight()`. [BankUtilityPanel.h:40]

- [x] [Review][Patch] Le rouge disparaît au survol du bouton banque sélectionné — seul `textOff` est surchargé ; `Button::getTextColour` priorise `textHover` / `textClicked`. Surcharger aussi ces deux champs dans l'accent look. [BankUtilityPanel.cpp:138-139]

- [x] [Review][Defer] Matrix-6/6R graying non validé manuellement (AC #7) — smoke item 6 non exercé ; dette harness documentée Appendix C UAT grid ; logique code conforme.

- [x] [Review][Defer] AC #9 builds/tests non re-exécutés dans cette revue — preuve dans Dev Agent Record (1830 tests green) ; pas de régression visible dans le diff handler/InternalPatches.

## Dev Notes

### What Story 7.5 IS — and what it is NOT

| In scope (7.5) | Out of scope |
|---|---|
| Bank Utility **selected bank red text** (FR-19) | Internal Patches Prev/Next / I/C/P wiring (**7.6**) |
| Verify/fix **lock dot** on bank NumberBox (D-023a-R3) | Renaming `kBanksLocked` → `kBankLockIndicator` (deferred debt) |
| Optional **UNLOCK footer** (D-022-R4) | `miscBankLockEnable` ↔ Bank Utility sync |
| `kSelectedBank` ↔ UI highlight + minor Core mirror if desync | Cross-bank navigation (removed in **7-3c**) |
| Matrix-6 graying regression guard | Header/footer shell (**7.8**) |
| Manual GUI smoke | Automated GUI unit tests (project policy: manual) |

### Hard prerequisite — Story 7-3c

**7-3c is `done`.** Core semantics are frozen:

- UNLOCK → `0CH` only; `kBanksLocked = false`; no coordinate change.
- Bank select → Set Bank + PC; `kBanksLocked = true`.
- Navigation → intra-bank wrap only; navigation **must not read** `kBanksLocked`.
- `kBanksLocked` is **display-only** lock indicator (D-023a-R3).

**Do not** reintroduce lock-driven navigation or `clearSyncedBankState()` on UNLOCK.

### Current brownfield state (read before editing)

#### BankUtilityPanel — action wiring OK, visual state missing

`BankUtilityPanel.cpp` today:

- Buttons fire timestamp `setProperty` on action IDs (`kSelectBank0`…`kSelectBank9`, `kUnlockBank`) — **correct** (AD-5).
- Listens only to `deviceType` / `deviceDetected` for Matrix-6 graying (Story 8.5).
- **Does not** listen to `kSelectedBank` — no active-bank red text.
- **Does not** show UNLOCK footer on click.
- Grayed module: `setBankUtilityGrayed` + `showMatrix1000OnlyFooterMessage` — **preserve**.

```cpp
// BankUtilityPanel.cpp — makeBankAction pattern (keep)
apvts_.state.setProperty(propertyId, juce::Time::getCurrentTime().toMilliseconds(), nullptr);
```

#### InternalPatchesPanel — lock dot already wired (verify)

`refreshBankLockIndicator()` (lines 239–248) sets `currentBankNumber->setShowDot(bankNumberVisible_ && banksLocked)`.

Listener on `kBanksLocked` at line 88–89. **7.5 task = verify + smoke, not rewrite.**

Known deferred flash (7-3c review): `kCurrentBankNumber` change may refresh dot before handler sets `kBanksLocked` true — final state correct; fix only if user-visible.

#### Core — kSelectedBank writers

| Path | Writes `kSelectedBank`? | Writes `kCurrentBankNumber`? |
|---|---|---|
| Bank button select (`PatchManagerActionHandler` ~205–206) | ✅ | ✅ |
| Prev/Next / patch NumberBox (`applyPatchCoordinates`) | ❌ | ✅ (bank unchanged post-7-3c) |
| `MidiManager` init | ✅ default 0 | — |
| `resetInternalPatchCoordinatesToDefaults` | ❌ **audit** | ✅ default 0 |
| UNLOCK | ❌ (correct) | ❌ |

**FR-19 UI** must read `kSelectedBank`. If `resetInternalPatchCoordinatesToDefaults` leaves `kSelectedBank` stale after device-type reset, mirror `defaultBank` to `kSelectedBank` in that function (minimal Core fix, AC #6).

### Active bank red text — implementation guidance

**Requirement:** Selected bank button label in **`ColourChart::kRed`** (`0xFFFF0000`); others use default `ButtonLook.textOff`.

**Recommended approach (panel-local, no `TSS::Button` API change):**

1. Store `TSS::ButtonLook normalBankLook_` from `buttonLookFromSkin(*skin_)` in panel.
2. `refreshSelectedBankHighlight()`:
   - `selected = static_cast<int>(apvts_.state.getProperty(kSelectedBank, 0))`
   - For each `selectBankNButton_` (N = 0…9):
     - If `bankUtilityGrayed_` → reset all to `normalBankLook_` (gray alpha already applied).
     - Else if `N == selected` → `accentLook = normalBankLook_; accentLook.textOff = juce::Colour(ColourChart::kRed); button->setLook(accentLook);`
     - Else → `button->setLook(normalBankLook_);`
3. Register listener:

```cpp
if (propertyName == PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties::kSelectedBank)
    refreshSelectedBankHighlight();
```

4. Call `refreshSelectedBankHighlight()` at end of ctor and in `setSkin()` after look rebuild.

**Anti-patterns:**

- Do **not** use `setToggleState(true)` — `textOn` skin colour is **not** red (`Skin.cpp` maps `kButtonTextOn` → same as `kText`).
- Do **not** read `kCurrentBankNumber` for Bank Utility highlight — PRD FR-19 specifies `selectedBank` APVTS property.
- Do **not** add global `AffineTransform` scaling.

**Alternative (if Look copy per button feels heavy):** add `setAccentLabel(bool)` to `TSS::Button` overriding text colour to red in `getTextColour()` — acceptable if cleaner; keep scope minimal.

### UNLOCK footer — D-022-R4

Add to `PluginDisplayNames::PatchManagerSection::BankUtilityModule`:

```cpp
constexpr const char* kUnlockBankFooterMessage =
    "Synth bank lock released — use front panel for 3-digit entry; plugin bank/patch unchanged.";
```

Invoke in `unlockBankButton_->onClick` lambda **after** timestamp property set (handler runs via `ActionDispatcher` asynchronously — footer is informational, order with action dispatch is acceptable).

Reuse local `setFooterInfoMessage` helper already in `BankUtilityPanel.cpp`.

### APVTS property reference

| Property ID | C++ constant | Kind | Consumer in 7.5 |
|---|---|---|---|
| `patchManagerSelectedBank` | `BankUtilityModule::StateProperties::kSelectedBank` | state | Bank Utility red text |
| `patchManagerBanksLocked` | `BankUtilityModule::StateProperties::kBanksLocked` | state (display-only) | Internal Patches dot (verify) |
| `internalPatchesCurrentBankNumber` | `InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber` | state | NumberBox display only — **not** Bank Utility highlight |
| `bankUtilityUnlockBank` | `BankUtilityModule::StandaloneWidgets::kUnlockBank` | action (timestamp) | UNLOCK click + optional footer |

**Do not rename** IDs (7-3b done). **Do not** conflate `miscBankLockEnable` (master byte 165).

### Architecture compliance

- GUI → Core dependency OK for `DeviceMemoryLimits` / `DeviceTypeRegistry` (existing 8.5 pattern in panel).
- **No** SysEx or `PatchManagerActionHandler` calls from GUI.
- APVTS identifiers from `PluginIDs.h` only; display strings from `PluginDisplayNames.h`.
- No global `AffineTransform` UI scaling; use existing `setUiScale` / `resized()` layout.
- English only in source.

### File structure

```
Source/GUI/Panels/.../PatchManagerPanel/Modules/
├── BankUtilityPanel.h          (UPDATE — refreshSelectedBankHighlight, listener)
├── BankUtilityPanel.cpp        (UPDATE — red text, UNLOCK footer, listener)
└── InternalPatchesPanel.cpp    (VERIFY ONLY — dot; optional flash fix)

Source/Shared/Definitions/
└── PluginDisplayNames.h        (UPDATE — kUnlockBankFooterMessage)

Source/Core/
└── PluginProcessor.cpp         (UPDATE only if AC #6 desync on reset — optional)

Tests/ — no new GUI tests; existing PatchManagerActionHandlerTests remain green
```

No `CMakeLists.txt` change expected (no new files).

### Manual smoke checklist (Standalone, Matrix-1000 device type)

1. Fresh instance → bank **0** button red text; others default.
2. Click bank **3** → button 3 red; `0AH` + PC on MIDI monitor; dot **on** on bank NumberBox.
3. Click **UNLOCK** → `0CH` only; dot **off**; bank/patch digits unchanged; info footer visible.
4. Click bank **7** → button 7 red; dot **on** again.
5. Prev/Next at patch 99 → wraps to 0 **same bank**; bank button highlight **unchanged** (still 7).
6. Matrix-6 device type → Bank Utility grayed; click shows Matrix-1000-only footer; no red text.
7. UI Scale 50% / 150% → red text and dot still visible (no blur transform).

### Previous story intelligence (7-3c)

- Lock indicator is **display-only** — GUI must not use `kBanksLocked` for navigation UI logic in Bank Utility.
- Dot rendering landed early in investigation branch — 7.5 **verifies** `InternalPatchesPanel::refreshBankLockIndicator`.
- Transient dot-off on bank select deferred — optional polish only.
- ID renames complete — grep-clean; do not rename again.

### Previous story intelligence (7-4)

- Handler/panel separation: panels only `setProperty`; business logic stays Core.
- Footer pattern: `uiMessageText` + `uiMessageSeverity` via APVTS state properties.
- Story marked `done` with full test suite green — preserve CI gate (Story 11.1).

### Previous story intelligence (7-3 / 7-3b)

- Bank buttons already route through `ActionPropertyRegistry` → `PatchManagerActionHandler`.
- `unlockBankButton_` naming and UNLOCK label done in 7-3b.
- Deferred: `kSelectedBank` UI desync after cross-bank nav — **obsolete** after 7-3c; highlight now tracks bank-select only.

### Git intelligence (recent commits)

| Commit | Relevance |
|---|---|
| `ab0a263` | Story 11-1 CI fixes — run full test gate before marking done |
| `ca06ff3` | CI merge — multi-platform builds available |
| `cdc4c98` | Debounce/headless test stability — no Bank Utility impact |

No recent commits touch `BankUtilityPanel` — brownfield UI state from 8.5 graying + 7-3b ID renames.

### Latest tech / framework notes

- **JUCE 8.0.12** — `ValueTree::Listener` on `apvts_.state`; register/remove in ctor/dtor (pattern in both panels).
- **C++17** — no new dependencies.
- Red colour SSOT: `TSS::ColourChart::kRed` (used by NumberBox dot via `look_.dot` and `ModulationBusCell` reorder highlight).
- `TSS::Button::getTextColour` uses `look_.textOff` when not hovered — override `textOff` in accent look copy.

### Project context reference

- `project-context.md` — GUI/Core boundary, no French in source, builds under `Builds/`.
- `CONVENTIONS.md` — Clean Code limits (15-line functions); extract helper if highlight loop exceeds limit.
- D-022-R5 — Bank Utility layout frozen (76 px module height, 4 px gaps) — **no layout changes**.

### References

- [epics.md — Story 7.5, FR-19–FR-21]
- [prd.md — §4.6 FR-19, FR-20, FR-21]
- [.decision-log.md — D-022-R4, D-022-R5, D-023a-R3, D-022-R6]
- [sprint-change-proposal-2026-06-19-bank-unlock-simplify.md — amended 7.5 AC]
- [7-3c-bank-utility-unlock-simplify.md — Core prerequisite, dot verify]
- [investigation next-patch-after-unlock — partial 7.5 slice landed in InternalPatchesPanel]

## Dev Agent Record

### Agent Model Used

Composer 2.5

### Debug Log References

- Audit: `InternalPatchesPanel::refreshBankLockIndicator` already listens to `kBanksLocked` — no change needed.
- Audit: UNLOCK button is plain text via `PluginDisplayNames::kUnlockBank` — no padlock asset.
- Audit desync: `resetInternalPatchCoordinatesToDefaults` wrote `kCurrentBankNumber` but not `kSelectedBank` — fixed in `PluginProcessor.cpp`.

### Completion Notes List

- Added `refreshSelectedBankHighlight()` to `BankUtilityPanel`: listens to `patchManagerSelectedBank`, applies `ColourChart::kRed` to selected bank button label via accent `ButtonLook`; resets all when Matrix-6 grayed.
- UNLOCK click now sets info footer via `kUnlockBankFooterMessage` (D-022-R4) when module not grayed.
- Core mirror: `resetInternalPatchCoordinatesToDefaults` now also sets `kSelectedBank` to `defaultBank`.
- Transient dot flash deferred (optional task — not visibly reproducible in headless audit).
- Builds green: `Matrix-Control_Tests`, `Matrix-Control_VST3`, `Matrix-Control_Standalone` (macOS Debug ARM64).
- Full test suite passed (1830 tests, exit 0).
- **Manual UAT (2026-07-11, Guillaume):** Matrix-1000 + Tauntek EPROM v1.20 — smoke checklist **PASS** (items 1–5, 7). Item 6 (Matrix-6 graying) **not exercised** — no Matrix-6/6R hardware available.
- **Testing debt:** Need a first-class **device-type simulation harness** (Matrix-1000 / Matrix-6 / Matrix-6R) for manual UAT and future stories — see `Documentation/Development/Plans/2026/06/2026-06-19-Matrix-6-6R-User-Acceptance-Test-Grid.md` Appendix C. Today: partial APVTS override (Appendix A) + CI handler tests only.

### File List

- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/BankUtilityPanel.h`
- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/BankUtilityPanel.cpp`
- `Source/Shared/Definitions/PluginDisplayNames.h`
- `Source/Core/PluginProcessor.cpp`

## Change Log

- 2026-07-11: Story 7.5 created — Bank Utility UI wiring guide (red text, dot verify, UNLOCK footer, kSelectedBank sync).
- 2026-07-11: Story 7.5 implemented — Bank Utility red highlight, UNLOCK footer, kSelectedBank reset sync; status → review.
- 2026-07-11: Manual UAT PASS on Matrix-1000 (Tauntek v1.20); Matrix-6/6R graying untested — device simulation harness tracked in UAT grid Appendix C.
- 2026-07-11: Code review — 3 patches applied (`valueTreeRedirected`, hover red accent, `reconcilePatchManagerCoordinatesForDeviceType` kSelectedBank mirror); status → done.
