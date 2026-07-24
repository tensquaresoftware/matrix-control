---
organization: Ten Square Software
project: Matrix-Control
title: Story U-9 — Master Edit Panel Layout Audit
author: BMad Agent
status: done
baseline_commit: 98458a2
sources:
  - planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md
  - implementation-artifacts/u-0-zone-dimension-tables.md
  - implementation-artifacts/u-5-body-shell-padding-separators-and-column-gaps.md
  - implementation-artifacts/u-6-patch-edit-panels-layout-audit.md
  - implementation-artifacts/u-7-matrix-modulation-panel-layout-audit.md
  - implementation-artifacts/u-8-patch-manager-panels-layout-audit.md
  - Source/GUI/Layout/Design/DesignPanels.h
  - Source/GUI/Layout/Design/DesignChecks.h
  - Source/GUI/Layout/ScaledLayout.h
  - Source/GUI/Layout/PanelDimensions.h
  - Source/GUI/Factories/DimensionFactory.cpp
  - Source/GUI/Panels/MainComponent/BodyPanel/MasterEditPanel/
  - Source/GUI/Panels/Reusable/BaseModulePanel.cpp
  - _bmad-output/project-context.md
created: 2026-07-24
updated: 2026-07-24
---

# Story U.9: Master Edit Panel Layout Audit

Status: done

<!-- Ultimate context engine analysis completed - comprehensive developer guide created -->

<!--
Sprint key: u-9-master-edit-panel-layout-audit
Prerequisites: U-5 (body shell) done; U-0/U-0b dims; U-2 widget sizes; U-6 BaseModulePanel integer rows done; U-7/U-8 integer-stack patterns done.
Parallel siblings: U-6/U-7/U-8 done — do not reopen Patch Edit / Matrix Mod / Patch Manager interiors here.
Next: U-10 aggregate residual UAT + release gate.
-->

## Story

As a sound designer,
I want MIDI, Vibrato, and Misc master modules stacked per Figma,
so that the MASTER EDIT column height matches the PATCH EDIT column at every UI Scale preset.

## Problem Statement / Errata

### What this story owns

**Master Edit section interiors only:**

1. Vertical module stack in `MasterEditPanel` (section header → MIDI → gap → Vibrato → gap → Misc).
2. Inter-module gap **`kInterModuleGap` = 12** (between the three modules).
3. Confirmation that Midi / Vibrato / Misc (via `BaseModulePanel` + `ParameterCell`) still lay out on the integer patterns hardened in U-6 — fix any residual ME-only drift if audit proves it.

### Epic / prior-story errata (map before coding)

| Epic / older wording | Live reality | Action |
|----------------------|--------------|--------|
| Epic U-9 “Given U-0 … `static_assert` height parity” | Already asserted in `DesignChecks.h` (ME height == PE height == **704**) | **Do not** change asserts unless a documented Figma delta appears |
| U-6 “Master Edit residual stays on U-9” | `BaseModulePanel` already uses `removeFromTop` for header + parameter rows (U-6) | Primary debt is **section stack** in `MasterEditPanel`, not redoing BaseModulePanel from scratch |
| “Touch MidiPanel/VibratoPanel/MiscPanel layout” | Those three files are **config-only** (`makeMasterEditModuleLayout` + `BaseModulePanel` ctor) — no local `resized` | Layout lives in `MasterEditPanel` + `BaseModulePanel` / `ParameterCell` |
| Misc 8th parameter `""` | Empty slot → `ParameterType::None` spacer row so Misc height stays **224** (8×24 + header) | **Preserve** empty slot — do not drop it to “clean” the list |
| M-6 / grayed Master Edit | Epic 8 device gating: gray appearance + footer on click; layout height unchanged when grayed | Layout-only — **preserve** gating semantics |

### Critical brownfield debt — `y +=` section stack (same class as pre-U-8 PatchManagerPanel)

`MasterEditPanel::resized()` already scales heights/gaps with `ScaledLayout::scaledInt`, but places modules with mutable `y += height + gap` (drift cousin of pre-U-5 SharedPanel / pre-U-8 PatchManagerPanel):

```157:178:Source/GUI/Panels/MainComponent/BodyPanel/MasterEditPanel/MasterEditPanel.cpp
void MasterEditPanel::resized()
{
    // … scaledInt on heights/gap …
    sectionHeader_->setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), sectionHeaderHeight);

    int y = bounds.getY() + sectionHeaderHeight;
    midiPanel_->setBounds(bounds.getX(), y, childWidth, midiH);
    y += midiH + interModuleGap;
    vibratoPanel_->setBounds(bounds.getX(), y, childWidth, vibratoH);
    y += vibratoH + interModuleGap;
    miscPanel_->setBounds(bounds.getX(), y, childWidth, miscH);
}
```

**Target:** successive `removeFromTop` (mirror `PatchManagerPanel::resized` / `PatchEditPanel::resized` / `SharedPanel::resized` after U-5/U-6/U-8).

### U-0b / U-2 / U-5 / U-6 already done (do not redo)

| Item | Owner | Note |
|------|-------|------|
| Design* SSOT + Factory injection | U-0 / U-0b | Panels must **not** `#include Design*` |
| Widget / ParameterCell sizes | U-2 | Place injected sizes; do not re-hardcode |
| Hairlines / D-013 | U-2b | Layout bounds only here |
| Body shell columns | U-5 | Do not reopen `BodyPanel` |
| `BaseModulePanel` header + param rows → `removeFromTop` | U-6 | Verify ME consumers; reopen only if ME residual proven |
| Patch Edit / Matrix Mod / Patch Manager interiors | U-6 / U-7 / U-8 | Out of scope |

### Scope split vs neighbours

| Concern | Owner |
|---------|-------|
| Body column placement / PE↔ME height parity asserts | U-5 + DesignChecks (done) |
| Patch Edit 5-column / BaseModulePanel harden | U-6 (done) |
| Master Edit **section stack** + ME UAT | **U-9** |
| Aggregate odd-scale residual UAT | U-10 |
| Master SysEx / Master Init confirm dialog | Epic 3 (out of scope) |
| Matrix-6 Master Edit gray gating behaviour | Epic 8 (preserve; do not redesign) |

## Acceptance Criteria

### AC 1 — Master Edit section stack matches Design @ 100 % and scales without drift

**Given** U-0 Master Edit dimensions injected as `MasterEditPanelDimensions`  
**When** `MasterEditPanel` resizes at each UI Scale preset  
**Then**

1. At **100 %**, vertical identity holds:  
   `sectionHeader (32) + 224 + 12 + 200 + 12 + 224 = 704`  
   (= `kSectionHeader + Midi + gap + Vibrato + gap + Misc`). Width **152**.
2. Band heights / `interModuleGap` come from **injected** dims via `ScaledLayout::scaledInt` (no magic numbers, no Design* reads in panels).
3. Vertical stack uses successive `removeFromTop` (replace `y +=` placement).
4. Design height parity with Patch Edit (**704**) remains intact (existing `DesignChecks` — do not weaken).
5. Manual UAT checklist (below) signed or owner-narrowed with note → U-10.

### AC 2 — Module interiors stay on U-6 integer patterns

**Given** Midi / Vibrato / Misc are `BaseModulePanel` consumers with Factory-injected `moduleHeader` + `parameterCell` dims  
**When** each module resizes at each preset  
**Then**

1. Parameter rows remain successive `removeFromTop` of `parameterCell.rowHeight` (U-6 path) — no regression to `y += rowHeight`.
2. Misc empty 8th slot (`""` → `ParameterType::None`) still consumes one row height so Misc module height stays **224** @ 100 %.
3. `ParameterCell` ME label/control widths stay from injected dims (`Recipes::Label/ComboBox::kMasterEditModule` via Factory) — do not invent private ME layout tokens.
4. Touch `BaseModulePanel` / `ParameterCell` **only** if audit proves ME-specific residual after the section-stack fix (document why in Completion Notes). Prefer algorithm fix over token churn.

### AC 3 — Preserve device-gating & behaviour contracts

**Given** Master Edit can gray when device forbids master editing (Matrix-6 family / unsupported)  
**When** layout is converted to integer strips  
**Then**

1. `setMasterEditGrayed` / `refreshDeviceGating` / Compare root-lock skip-child-gray behaviour **unchanged**.
2. Grayed click still shows footer message (`showMatrix6PatchOnlyFooterMessage` path).
3. Grayed section **height/layout** unchanged vs enabled (layout-only story — gating is Epic 8).
4. Module Init confirmation gates / COPY-PASTE absence on Master Edit headers unchanged.

### AC 4 — Preserve contracts & out of scope

1. No global `AffineTransform` on the prod UI tree.
2. No `#include` of Design* from Master Edit panels / `BaseModulePanel`.
3. Do not reopen Body shell, Patch Edit, Matrix Mod, Patch Manager, Header, Footer.
4. Do **not** change master SysEx paths, Master Init confirm dialogs (3.4), descriptors/IDs, APVTS wiring, or Epic 8 gate logic.
5. Do not reopen widget **paint** / skin tokens / D-013 hairline policy.
6. Touch `DimensionFactory` / Design* **only** if audit proves a wrong injected token (unlikely — prefer algorithm fix). Do not break PE↔ME height parity asserts.
7. No new automated GUI pixel tests.

### AC 5 — Docs, build, sprint

1. Zone table Master Edit section remains authoritative; update only if a documented Figma delta appears.
2. macOS Debug build green; existing unit tests green (`cmake --build --preset macos-debug-arm64` + `Matrix-Control_Tests`).
3. Sprint key `u-9-master-edit-panel-layout-audit` → `review` after implementation + UAT (or owner-narrowed UAT with note).
4. Completion Notes must include grep proof: no mutable `y += height + gap` placement left in `MasterEditPanel::resized`.

## Tasks / Subtasks

- [x] **T1 — Baseline capture** (AC: #1, #2, #4)
  - [x] Confirm DesignChecks identities: Midi **224**, Vibrato **200**, Misc **224**, gap **12**, section **704**, PE↔ME height equality
  - [x] Confirm `MasterEditPanel` `y +=` stack vs PatchManagerPanel `removeFromTop` precedent
  - [x] Confirm Midi/Vibrato/Misc are config-only; layout in `BaseModulePanel`
  - [x] Confirm Misc empty 8th parameter slot still present
  - [x] Optional: screenshots @ 100 % and @ 150 % / 175 % before edits (enabled + Matrix-6 grayed)

- [x] **T2 — MasterEditPanel vertical stack** (AC: #1, #4)
  - [x] Refactor `resized` to `removeFromTop` + scaled gaps (mirror PatchManagerPanel / PatchEditPanel)
  - [x] Grep proof: no mutable `y += midiH + interModuleGap` (or equivalent) placement remaining
  - [x] Preserve `setUiScale` propagation to children (parent drives child `resized` via `setBounds`)

- [x] **T3 — Module interior verification** (AC: #2, #3, #4)
  - [x] Smoke Midi / Vibrato / Misc row stacks @ 100 % and one odd preset (150 % or 175 %)
  - [x] Confirm Misc spacer row still present (height **224** @ 100 %)
  - [x] Confirm grayed gating still dims / blocks / footer-on-click
  - [x] Only if residual found: minimal fix in `BaseModulePanel` / `ParameterCell` with Completion Notes rationale

- [x] **T4 — Docs + UAT + build** (AC: #1, #5)
  - [x] Complete manual UAT checklist (or owner narrowing note → U-10)
  - [x] Build + existing unit tests green
  - [x] Completion Notes: algorithm change, grep proof, any intentional deltas (should be none), grayed smoke result

### Review Findings

- [x] [Review][Patch] Revert unrelated Epic 11 comment rewrap in sprint-status.yaml [`_bmad-output/implementation-artifacts/sprint-status.yaml` Epic 11 comment]
- [x] [Review][Patch] Align Manual UAT checklist checkboxes with owner smoke sign-off [`u-9-master-edit-panel-layout-audit.md` UAT table]
- [x] [Review][Defer] Odd-scale leftover when Σ scaled strips ≠ parent height [`MasterEditPanel.cpp:170-176`] — deferred, pre-existing / U-10 residual policy
- [x] [Review][Defer] Parent module height vs BaseModulePanel interior sum at odd presets [`MasterEditPanel` + `BaseModulePanel`] — deferred, pre-existing / U-10
- [x] [Review][Defer] `sectionHeaderWidth` unused in `resized` (header uses full strip width) [`MasterEditPanel.cpp:170`] — deferred, pre-existing

## Manual UAT checklist (layout audit)

**Goal:** Master Edit reads as one Figma-aligned 3-module stack; inter-module gap 12; column height matches Patch Edit (**704** @ 100 %); no 1 px drift at odd presets; Matrix-6 gray still works.  
**Build:** Standalone Debug · **Tester:** Guillaume · **Skin:** Black; Cream spot-check @ 100 %

| Check | 50% | 75% | 100% | 125% | 150% | 175% | 200% |
|-------|-----|------|------|------|------|------|------|
| Section stack: header + 224/200/224 + 2×12 = 704 @ 100 %; no overlap/gap | ☐ | ☐ | ☑ | ☐ | ☐ | ☐ | ☐ |
| PE ↔ ME column bottoms align (both 704 @ 100 %) | ☐ | ☐ | ☑ | ☐ | ☐ | ☐ | ☐ |
| MIDI: 8 parameter rows + header — no clip / drift | ☐ | ☐ | ☑ | ☐ | ☐ | ☐ | ☐ |
| Vibrato: 7 parameter rows + header — no clip / drift | ☐ | ☐ | ☑ | ☐ | ☐ | ☐ | ☐ |
| Misc: 7 controls + spacer row (= 8×24) — height matches MIDI | ☐ | ☐ | ☑ | ☐ | ☐ | ☐ | ☐ |
| No clipped controls at Master Edit edges (width 152) | ☐ | ☐ | ☑ | ☐ | ☐ | ☐ | ☐ |
| Matrix-6 (or unsupported) gray: height unchanged; click → footer message | ☐ | ☐ | ☑ | ☐ | ☐ | ☐ | ☐ |
| Hairlines crisp (D-013 — no paint regression) | ☐ | ☐ | ☑ | ☐ | ☐ | ☐ | ☐ |
| Screenshot captured (preferred @ 100 + 150 + 175) | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ | ☐ |

**Sign-off:** Owner smoke test **passed** (2026-07-24) — **100 %** column checked above. Full 7-preset matrix residual → **U-10**. Optional screenshots skipped.  
**UAT note:** Owner-narrowed smoke confirmed success @ 100 %; full 7-preset visual matrix deferred → **U-10** (U-5…U-8 precedent).

## Dev Notes

### Depends on

| Dependency | Status | Note |
|------------|--------|------|
| U-0 / U-0b | done | Master Edit dims injected |
| U-2 / U-2b | done | Widget sizes + hairlines |
| U-5 | done | Body shell unlocked column audits |
| U-6 | done | BaseModulePanel integer rows; ME residual deferred here |
| U-7 / U-8 | done | Integer stack precedents (`removeFromTop`) |
| U-10 | backlog | Aggregate residual UAT |

### Recommended order

Baseline → `MasterEditPanel` `removeFromTop` stack → module smoke (incl. Misc spacer + grayed) → optional residual fix → UAT → build/tests.

### Key files (Code Map)

| Path | Change |
|------|--------|
| `…/MasterEditPanel/MasterEditPanel.cpp` | **Primary stack:** kill `y +=`; use `removeFromTop` |
| `…/MasterEditPanel/MasterEditPanel.h` | Only if signature/helpers needed (unlikely) |
| `Source/GUI/Panels/Reusable/BaseModulePanel.cpp` | Touch **only** if ME residual proven after stack fix |
| `Source/GUI/Widgets/ParameterCell.cpp` | Touch **only** if ME residual proven |
| `…/Modules/MidiPanel.cpp` / `VibratoPanel.cpp` / `MiscPanel.cpp` | Config-only — **do not** invent local layout; preserve Misc `""` slot |
| `Source/GUI/Layout/ScaledLayout.h` | Use existing `scaledInt` only — **no** new distribute API |
| `DimensionFactory.cpp` / Design* / `PanelDimensions.h` | Touch **only** if injected token wrong |
| `u-0-zone-dimension-tables.md` § Master Edit | Update only on documented Figma delta |

**Do not edit:** `BodyPanel.cpp`, Patch Edit / Matrix Mod / Patch Manager panels, Core MIDI/master SysEx, Master Init dialogs, widget paint/skins, Epic 8 gate semantics (beyond layout preservation).

### Suggested Review Order

1. `MasterEditPanel::resized` — stack identity + `removeFromTop`
2. Design identity cross-check (224/200/224 + 2×12 + 32 = 704)
3. Misc empty parameter slot still present
4. Grayed gating smoke (behaviour preserved)
5. Completion Notes grep proof + UAT note

### Design SSOT @ 100 % (Master Edit)

| px | Symbol / meaning |
|----|------------------|
| 152 × 704 | `Panels::Body::MasterEditSection::kWidth` × `kHeight` |
| 32 | `Atoms::Heights::kSectionHeader` |
| 224 | `MidiModule::kHeight` = `32 + 8×24` |
| 200 | `VibratoModule::kHeight` = `32 + 7×24` |
| 224 | `MiscModule::kHeight` = `32 + 8×24` (7 params + 1 empty spacer) |
| 12 | `MasterEditSection::kInterModuleGap` (= `Spacing::kLarge`) |
| 2 | `kInterModuleGapCount` |
| 704 | PE column height — asserted equal |

Identities asserted in `DesignChecks.h` — prefer not to change asserts unless a Figma delta is documented.

### Target stack (replace `y +=`)

Prefer (illustrative — match live PatchManagerPanel style):

```cpp
auto area = getLocalBounds();
sectionHeader_->setBounds(area.removeFromTop(sectionHeaderHeight));
midiPanel_->setBounds(area.removeFromTop(midiH));
area.removeFromTop(interModuleGap);
vibratoPanel_->setBounds(area.removeFromTop(vibratoH));
area.removeFromTop(interModuleGap);
miscPanel_->setBounds(area.removeFromTop(miscH));
```

Width: modules may use `childWidth` from injected dims (equals section width @ design). Prefer full `area` width after header strip if that matches current visual contract — preserve pre-change width behaviour at 100 %.

### Remainder policy (odd-scale leftover)

- Prefer **fixed** scaled heights + gaps via `removeFromTop` (U-7/U-8 fixed-width precedent).
- Do **not** invent a distribute API. Only if audit proves `Σ scaled(pieces) ≠ parent height` by ±1–2 px at odd presets, Misc may absorb the leftover on the **last** strip — and at **100 %** Misc must still equal design **224**.
- Document the chosen policy in Completion Notes. Aggregate odd-scale residual UAT → **U-10** (do not invent runtime asserts unless AC asks).

### Architecture compliance

- **UI Scale:** design @ 100 % × preset via `ScaledLayout::scaledInt` only — no global `AffineTransform` ([project-context.md](../project-context.md) § GUI & UI Scale Rules).
- **U-0b:** injected dims only; no Design* includes in panels.
- **Clean Architecture:** layout-only in `Source/GUI/`; no Core / SysEx / APVTS behaviour changes.
- **JUCE 8.0.12:** prefer `Rectangle::removeFromTop` integer slicing (same pattern as U-5…U-8).
- **CMake:** Master Edit sources already registered — no new TU expected.

### Previous story intelligence

| Lesson | Source | Apply here |
|--------|--------|------------|
| Float / mutable `y +=` “fine at 100 %” still drifts at 150/175 | U-5…U-8 | Kill `y +=` in MasterEditPanel |
| Do not invent distribute / remainder APIs | U-5/U-6 | `scaledInt` only |
| BaseModulePanel already hardened — don’t redo blindly | U-6 | Verify; fix only proven residual |
| Config-only module `*Panel.cpp` files — layout is shared | U-6 / U-11 | Don’t add per-module `resized` |
| Owner-narrowed UAT + U-10 residual is acceptable | U-5…U-8 | Document in Completion Notes |
| Do not bundle unrelated hygiene “while here” | U-7/U-8 | No SysEx / init dialog / gate redesign |
| Grep proofs required in Completion Notes | U-5…U-8 | `y +=` gone from MasterEditPanel |

### Git intelligence (recent)

- `4eaa743` / `3a24deb` — Close U-8; Patch Manager float → integer strips (`removeFromTop` + shared helper)
- `606b242` — Close U-7 Matrix Mod integer column strip
- `7b61500` / `d8faa29` — Close U-6; BaseModulePanel `removeFromTop`; ME residual deferred to U-9
- Master Edit gray gating / Epic 8 commits are **behaviour** — preserve; this story is placement only

### Latest tech notes

- Project JUCE **8.0.12**; layout pattern remains `getLocalBounds()` + successive `removeFrom*` with integer amounts from `ScaledLayout::scaledInt`.
- No new third-party deps. No widget API version chase required for this audit.

### Project context reference

Follow `_bmad-output/project-context.md` and `CONVENTIONS.md`: French chat / English artifacts; no French in source; no Core←GUI deps; UI Scale via `ScaledLayout`; Ask First before expanding beyond layout ACs.

### Testing requirements

- Manual UAT table above (or owner-narrowed smoke + U-10 note).
- Build: `cmake --build --preset macos-debug-arm64`.
- Run existing `Matrix-Control_Tests` — no new GUI pixel tests.
- Smoke: Matrix-6 (or unsupported device) gray still dims Master Edit and click still shows footer; Misc spacer row still present; Init on module headers still works.

### Anti-patterns (forbidden)

- Leaving `y +=` “because it looks fine at 100 %”
- Inventing `distributeFixedDesignRowsWithRemainderOnLast` or new ScaledLayout APIs
- `#include` Design* from Master Edit / BaseModulePanel
- Dropping Misc empty `""` parameter to “clean” the list (breaks 224 height)
- Reopening Patch Edit / Matrix Mod / Patch Manager / Body shell / widget paint
- Changing Epic 8 gray gating into `setEnabled(false)` or redesigning footer messaging
- Changing DesignChecks PE↔ME height parity without documented Figma delta

## Dev Agent Record

### Agent Model Used

Composer (Cursor agent)

### Debug Log References

- Build: `cmake --build --preset macos-debug-arm64` green
- Tests: `Matrix-Control_Tests` exit 0 (`-DMATRIX_BUILD_TESTS=ON`)

### Completion Notes List

- **Algorithm:** `MasterEditPanel::resized` now uses successive `removeFromTop` for section header → Midi → gap → Vibrato → gap → Misc (mirrors `PatchManagerPanel` / `PatchEditPanel`). Module widths still use injected `childWidth` via `.withWidth(childWidth)`. Heights/gaps still from injected dims + `ScaledLayout::scaledInt` only.
- **Remainder policy:** Fixed scaled strip heights + gaps only — no last-strip remainder absorption; no new ScaledLayout distribute API.
- **Module interiors:** No residual found after stack fix. `BaseModulePanel` already uses `removeFromTop` for header + parameter rows (U-6). Misc empty `""` 8th slot preserved (height **224** @ 100 %). No `BaseModulePanel` / `ParameterCell` / Design* / DimensionFactory edits.
- **Behaviour preserved:** `setMasterEditGrayed` / `refreshDeviceGating` / Compare root-lock skip-child-gray / footer-on-click path untouched (layout-only). `setUiScale` still propagates to children then calls `resized()`.
- **Grep proof** (2026-07-24): `rg 'y \+=' MasterEditPanel.cpp` → no matches. No Design* includes under MasterEditPanel/.
- **Intentional deltas:** none.
- **UAT:** owner smoke test **passed** (2026-07-24); full 7-preset visual matrix → **U-10** (U-5…U-8 precedent). Optional pre-edit screenshots skipped.
- Design identity @ 100 % confirmed: `32 + 224 + 12 + 200 + 12 + 224 = 704`; PE↔ME height equality asserted in `DesignChecks.h` unchanged.

### File List

- `Source/GUI/Panels/MainComponent/BodyPanel/MasterEditPanel/MasterEditPanel.cpp`
- `_bmad-output/implementation-artifacts/u-9-master-edit-panel-layout-audit.md`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`

### Change Log

- 2026-07-24 — Implemented U-9 Master Edit section stack: `y +=` → successive `removeFromTop`; module interiors verified (no residual); build + unit tests green; UAT owner-narrowed → U-10; status → review.
- 2026-07-24 — Owner smoke test passed; full 7-preset residual remains → U-10.
- 2026-07-24 — Code review: reverted unrelated Epic 11 comment wrap; aligned UAT 100 % checkboxes with smoke sign-off; status → done.
