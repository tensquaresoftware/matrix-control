---
title: 'Skin Editor Hold mapping modes and chrome'
type: 'bugfix'
created: '2026-10-24'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '4313c2358fe1f8e37a19cec627f38ba6f94c503c'
context:
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-editor-ui-panels.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** With **Hold colour** on, Skin Editor keeps both an armed mapping row and a selected pastille, so a “return” click (palette after mapping several rows, or a row after trying several colours) rewrites the previous mapping instead of starting a clean new gesture. The logo bar also mixes connection / last-push / version with mapping hints, so the armed state is easy to miss.

**Approach:** Model three explicit mapping modes (Idle, held colour source, armed row target), Esc to Idle, Hold-batch only in the gesture direction, and split chrome: logo bar right = mapping witness when active (empty when Idle); new bottom status bar = connection | last push / status | version. Inter for status/badge copy; Orbitron only for the app title.

## Boundaries & Constraints

**Always:**
- Start mapping only by click (palette pastille or Mappings/Unbound row) — no shortcut to enter a mode.
- Esc → Idle: clear pending arm, clear Hold sticky selection side-effects that cause collision, hide mapping witness; keep undo/redo Cmd+Z path.
- Modes: **Idle** | **HeldColour** (pastille → many rows) | **ArmedRow** (row → many pastilles). One Hold toggle serves both; UI + rules must make the current mode obvious.
- Hold on: batch only in the current gesture direction. A click that changes side / changes anchor must switch mode, not rewrite the previous mapping.
- Hold off: one successful bind returns Idle (clear pastille + pending).
- Logo bar (32px): title left (Orbitron); right = mapping witness only when not Idle (rectangle badge `MAPPING` + detail + optional colour swatch / target hint); empty when Idle — no greyed permanent badge.
- Bottom status bar: same fill and height as logo bar; zones = connection | last push / status message | version right.
- Badge / status / mapping detail fonts = Skin Editor Inter helpers only (not plugin FooterPanel / Montserrat brand chrome).
- ASCII-only UI strings (hyphen, not em dash). Unbound uses the same mode rules as Mappings.
- Stay in `Tools/SkinEditor/`; pass `lint_touched.py` on touched Skin Editor C++.

**Never:**
- Plugin Look / FooterPanel redesign; skin data model / JSON format changes; new start shortcuts (Space, etc.); blinking / red alert for mapping; permanent Idle mapping badge; expanding scope outside Skin Editor.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Colour then rows (Hold on) | Idle → click pastille → click several mapping rows | Enter HeldColour; each row maps to that pastille; logo shows MAPPING witness | Bind failure: alert; stay in mode |
| Row then colours (Hold on) | Idle → click row → click several pastilles | Enter ArmedRow; each pastille remaps that row; witness shows target | Bind failure: alert; stay in mode |
| Return / change side (Hold on) | HeldColour after rows → click a (new) pastille | Switch HeldColour anchor; do **not** remap last row via stale ArmedRow | N/A |
| Return / change side (Hold on) | ArmedRow after colours → click a (new) row | Switch ArmedRow target; do **not** bind via leftover selectedPastille sticky path | N/A |
| Hold off one-shot | Select pastille + click row (or reverse) | One bind; return Idle; witness empty | Same alerts |
| Esc | Any non-Idle mapping mode | Idle; witness empty; no further collision | N/A |
| Esc focus | Focus inside table / viewport / combo | Main component still receives Esc and clears mode | N/A |
| Idle chrome | Idle | Logo right empty; bottom bar shows connection / status / version | N/A |

</frozen-after-approval>

## Code Map

- `Tools/SkinEditor/SkinEditorMainComponent.h` — replace sticky `hasPendingMapping_` / `pendingMappingKey_` with an explicit mapping-mode enum + held pastille / armed key; Esc; status-bar members; mapping-witness widgets.
- `Tools/SkinEditor/SkinEditorMainComponentActions.cpp` — `onPastilleSelected`, `armPendingMapping`, `applyMapping`, `clearPastilleSelectionAfterOneShot`: mode transitions; Hold keeps only the active side; change-of-side switches without rewrite.
- `Tools/SkinEditor/SkinEditorMainComponentSession.cpp` — `keyPressed` Esc → Idle; `refreshStatus` wires bottom bar only (stop putting mapping hints solely into last-push).
- `Tools/SkinEditor/SkinEditorMainComponent.cpp` — `setupTitleBarChrome` / `layoutTitleBar` / `resized` / `paint`: logo right = witness; reserve bottom status bar (`kStatusBarHeight` = `kTitleBarHeight`).
- `Tools/SkinEditor/SkinEditorMappingsPanel.cpp` / `.h` (+ `SkinEditorUnboundPanel.*`) — `tryBindRow`: when HeldColour, bind with held pastille; when ArmedRow, clicking a row re-arms that key (no immediate bind from leftover selection); when Idle + pastille selected + Hold off/on enter HeldColour bind path appropriately. Prefer callbacks that expose intent (bind vs arm) over boolean soup in the panel.
- `Tools/SkinEditor/SkinEditorChromeTokens.h` — `kStatusBarHeight`; optional mapping-badge size tokens.
- `Tools/SkinEditor/SkinEditorChromeFonts.h` / `SkinEditorChromeTable.*` — Inter-only MAPPING rectangle badge helper (do not reuse plugin FooterPanel badge APIs; count-badge capsule is a different pattern — text rectangle OK).
- Do **not** change: `DevSkin` layer / JSON, bridge push semantics, session autosave payload shape, plugin `FooterPanel*`.

## Tasks & Acceptance

**Execution:**
- [x] `Tools/SkinEditor/SkinEditorMainComponent.h` + `Actions.cpp` + `Session.cpp` — introduce Idle / HeldColour / ArmedRow; Esc clears to Idle; rewrite `onPastilleSelected` / `applyMapping` / arm paths so Hold never leaves both sides sticky; update mapping witness + status messages.
- [x] `Tools/SkinEditor/SkinEditorMappingsPanel.*` + `SkinEditorUnboundPanel.*` — align `tryBindRow` with mode (HeldColour binds; ArmedRow re-arms; Idle arms or binds per selection) without collision.
- [x] `Tools/SkinEditor/SkinEditorChromeTokens.h` + `SkinEditorMainComponent.cpp` — bottom status bar (connection | last push/status | version); logo bar right witness only when mapping active.
- [x] `Tools/SkinEditor/SkinEditorChromeFonts.h` and/or `SkinEditorChromeTable.*` — Inter MAPPING badge + detail layout; Orbitron remains title-only.
- [x] Esc focus — ensure main component (or trusted parent) receives Escape while lists have focus (`setWantsKeyboardFocus` / key forwarding as needed).
- [x] Run `python3 Scripts/quality/lint_touched.py` on touched Skin Editor files; build Skin Editor target.

**Acceptance Criteria:**
- Given Hold on and a held pastille mapped to several rows, when the operator clicks another pastille, then the previous row is not remapped and mode stays / becomes HeldColour for the new pastille.
- Given Hold on and an armed row remapped with several pastilles, when the operator clicks another row, then that click arms the new row (or maps only if still in HeldColour with a held colour) without wrongly applying leftover sticky state from the prior gesture.
- Given any active mapping mode, when Esc is pressed (even with table focus), then mode is Idle and the logo-right witness is gone.
- Given Idle, when the window is shown, then logo-right is empty and connection / last push / version live on the bottom bar with Inter fonts.
- Given Hold off, when a mapping completes, then mode returns Idle and the witness clears.

## Implementation Notes

- Explicit `MappingMode` (`kIdle` / `kHeldColour` / `kArmedRow`) in main; logic in `SkinEditorMainComponentMapping.cpp`.
- Pure intents in `SkinEditorMappingModePolicy.h`; covered by `Tests/Unit/SkinEditorMappingModePolicyTests.cpp`.
- Panels expose `onRowActivated` + `onEscapeRequested`; main decides bind vs re-arm (no dual sticky pastille+pending).
- Esc → `clearMappingModeToIdle`; logo-right Inter `MAPPING` witness; bottom status bar for connection / last push / version.
- Verified: `lint_touched.py` clean; Skin Editor build OK; policy unit tests 0 failures. Manual Hold/Esc chrome checks still needed in the live app.

## Spec Change Log

## Review Triage Log

- false — Esc ListBox miss (panel keyPressed never fires): JUCE KeyListener on ListBox + Content::keyPressed + Main grabKeyboardFocus; patched with EscapeKeyForwarder on Palette/Unbound.
- false — HeldColour on every pastille removes Idle colour-edit: frozen Design Notes / matrix require Idle→pastille enters HeldColour; intentional.
- medium/patch — Title witness can overlap Orbitron title: clamped via layoutMappingWitness leftLimit — patched.
- medium/patch — Status connection uncapped overlaps version: connW clamped vs version — patched.
- false — Instructional status strings removed: witness + bottom last-push replace old arming hints per chrome split.
- false/low-reject — Hold colour label vs bidirectional Hold: rename would add UI copy churn; tooltip already direction-agnostic.
- false/low-reject — Dual MappingMode enums: conversion localized; shared converter not required for this bugfix.
- medium/patch — afterEscape unused by production: clearMappingModeToIdle now jasserts afterEscape() Idle SSOT — patched.
- false — Same-mapping Hold-off clears selection: matches Hold-off one-shot Idle after write path in frozen matrix.
- medium/patch — Compare/read-only leaves mapping mode active: refreshAll clears when !editable — patched.
- false/low-reject — Armed row not highlighted after refresh: witness is the mode SSOT; row highlight out of scope.
- false/low-reject — MAPPING string duplicated in badge helpers: cosmetic only.
- defer — Policy tests do not pin MainComponent intent→applyMapping switches: deferred-work entry appended.
- false — Idle row always arms even with pastille selected: selecting pastille enters HeldColour first, so Idle+pastille+row is unreachable.
- medium/patch — Empty swatch when pastille missing: show swatch only after ARGB lookup succeeds — patched.
- medium/patch — Esc unit test vs production Escape path: production clear now tied to afterEscape SSOT — patched.

## Design Notes

**Mode rules (agent-owned detail):**
- `HeldColour`: remember held pastille name; row click → `setMapping`/`bindUnbound` with that pastille; keep mode; pastille click while HeldColour → change held pastille only (no write using old armed key).
- `ArmedRow`: remember `SkinColourKey`; pastille click → map that key; keep mode; row click while ArmedRow → change armed key only (do not bind using selectedPastille unless entering HeldColour intentionally — prefer: row click in ArmedRow re-arms).
- Enter HeldColour: Idle + click pastille (selection). Enter ArmedRow: Idle + click row with no held colour (or after Esc). Hold off: after one successful write → Idle + clear selection helpers as today’s one-shot clear.

**Witness copy (English, ASCII):** badge text `MAPPING`; HeldColour detail like `Colour: {name}` + swatch; ArmedRow like `Target: {object/property}`; Idle → hide badge and detail entirely.

**Chrome:** bottom bar same `surfaceApp` / 32px as title; version right-aligned; connection left-of-centre or left zone; status/message fills middle — match existing muted Inter styling.

## Verification

**Commands:**
- `python3 Scripts/quality/lint_touched.py` -- expected: 0 findings on touched `Tools/SkinEditor/` files
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control-SkinEditor` (or project Skin Editor target name) -- expected: build succeeds

**Manual checks:**
- Hold colour→rows then click a new pastille: no rewrite of last row; witness updates.
- Hold row→colours then click a new row: arms new target; Esc clears witness.
- Logo Idle empty right; bottom bar shows connection / last push / version.
