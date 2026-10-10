---
organization: Ten Square Software
project: Matrix-Control
title: Standalone Quit and Window Placement
author: BMad Agent
type: bugfix
created: '2026-10-09'
status: done
route: dispatch
review_loop_iteration: 0
baseline_commit: '589de352219f1346cd9f18edadd719ce900c3737'
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-dirty-unsaved-patch-ux-chantier-3.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** On Windows Standalone, Alt+F4 does not quit the app, and a restored window can start with its native title bar off-screen so the user cannot drag or click Close (taskbar Close is the workaround). Ctrl+Q (JUCE Quit) is also unwired.

**Approach:** Keep the existing unified quit path (`closeButtonPressed` → `systemRequestedQuit` → session close gate). Explicitly handle Alt+F4 under the native title bar, wire Ctrl+Q via `ApplicationCommandManager` without adding a Windows menu, and after real content sizing clamp or recentre so the title-bar drag strip intersects a display user area.

## Boundaries & Constraints

**Always:**
- Quit from Alt+F4, Ctrl+Q, title-bar Close, and taskbar Close must reach `MatrixControlStandaloneApp::systemRequestedQuit` so `confirmSessionCloseGateIfNeeded` still runs (Cancel keeps the app open).
- Keep OS-native title bar and `setConstrainer(nullptr)` so multi-monitor drag stays possible.
- Validate on-screen placement after `fitWindowToContent` / real size, not only in the JUCE base ctor.
- Clamp or recentre only when the title-bar drag strip does not intersect any display `userBounds`; no-op when already valid (do not fight intentional multi-monitor placement).

**Never:**
- Lot 3: double-click title bar to recentre (already deferred; other branch / all OS).
- Recolour or redesign the native title bar; change plugin-host chrome; refactor outside Standalone window / quit path.
- Bypass or weaken the session close confirmation gate.
- Re-enable `decoratorConstrainer` / `setBoundsConstrained` as the multi-monitor drag path.
- Invent a Windows application menu solely to host Quit.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Alt+F4 clean | Standalone focused, not at-risk | App quits via `systemRequestedQuit` (save state as today) | N/A |
| Alt+F4 at-risk | Warn-always + dirty / not-STORED | Session close modal; Cancel stays open; Discard/Persist proceeds then quits | Persist fail / picker cancel → stay open (existing gate) |
| Ctrl+Q | Standalone focused (editor or chrome) | Same path as Alt+F4 / Close | Same gate rules |
| Bad saved Y | `windowX`/`windowY` leave title bar outside all `userBounds` after fit | Window moved so title-bar strip intersects a display user area (prefer clamp into nearest display; recentre on that display if clamp cannot) | Empty displays list → leave bounds unchanged (existing createWindow guard) |
| Valid multi-monitor | Window fully usable on secondary display | Position unchanged after fit | N/A |
| First launch | No saved `windowX`/`windowY` | JUCE primary centre, then post-fit ensure still on-screen | N/A |

</frozen-after-approval>

## Code Map

- `Source/Standalone/StandaloneWindowPlacement.h` — title-bar strip, clamp/ensure, nearest-display picker (unit-tested).
- `Source/Standalone/StandaloneQuitCommands.h` — Alt+F4 / Ctrl+Q matchers, `bindStandaloneQuitCommands` (templated for JUCE `Component` macro), quit keypress probe.
- `Source/Standalone/MatrixControlStandaloneFilterWindow.h` — native title bar + null constrainer; Windows Alt+F4 → `closeButtonPressed`; post-fit ensure via helpers + nearest display.
- `Source/Standalone/MatrixControlStandaloneApp.cpp` — `bindStandaloneQuitCommands`; `systemRequestedQuit` gate unchanged.
- `Tests/Unit/StandaloneWindowPlacementTests.cpp` — placement + quit bind coverage.
- `Source/Core/PluginProcessorGates.cpp` — `confirmSessionCloseGateIfNeeded` — **reuse only**, do not change contract.
- JUCE `juce_StandaloneFilterWindow.h` (~675–709, 749–754, 782–787) — restore/save `windowX`/`windowY`; stock close skips `systemRequestedQuit` (already overridden).
- JUCE `juce_DocumentWindow.cpp` (~365–401) — Alt+F4 shortcut is attached only when **not** using native title bar → root cause of Windows Alt+F4 gap.
- JUCE `juce_Application.cpp` (~66–91) — Quit command + `defaultKeypresses` Cmd/Ctrl+Q; needs manager + key mappings listener.
- JUCE `juce_Windowing_windows.cpp` (~3974–3977, 4087–4134) — if `doKeyDown` consumes Alt+F4, DefWindowProc never emits `SC_CLOSE`; `WM_CLOSE` → `userTriedToCloseWindow` → `closeButtonPressed`.
- No project `ensureOnScreen` helper today; nearest display lookup pattern: `ScaledDrawing.h` / JUCE `Displays::Display::userBounds`.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Standalone/MatrixControlStandaloneFilterWindow.h` — On Windows, handle Alt+F4 in `keyPressed` by calling `closeButtonPressed()` (unified quit). After successful content size in `fitWindowToContent`, if the title-bar drag strip does not intersect any display `userBounds`, clamp into the nearest display user area (JUCE jlimit pattern); if still impossible, centre on that display. Keep constrainer null and native title bar.
- [x] `Source/Standalone/MatrixControlStandaloneApp.cpp` — Own an `ApplicationCommandManager`; after window create, register Quit for this app, set first command target, attach `getKeyMappings()` as key listener on `mainWindow`. Do not add a menu. Leave `systemRequestedQuit` gate logic unchanged.
- [ ] Manual UAT (Windows Standalone Debug) — Alt+F4 / Ctrl+Q clean + at-risk Cancel; relaunch with forced bad `windowY` then with valid secondary-monitor position. (Human — pending walkthrough)
- [x] `Tests/Unit/StandaloneWindowPlacementTests.cpp` + `CMakeLists.txt` — unit coverage for placement matrix + quit key matchers; CloseGate tests cover at-risk quit gate.
- [x] `python Scripts/quality/lint_touched.py` — clean on touched C++ under `Source/`.

**Acceptance Criteria:**
- Given Windows Standalone with keyboard focus in the editor, when the user presses Alt+F4 and the session is not at-risk, then the app exits through `systemRequestedQuit`.
- Given Windows Standalone at-risk with warn-always, when the user presses Alt+F4 or Ctrl+Q, then the existing session close confirmation appears; Cancel leaves the app running.
- Given Windows Standalone with focus in the UI, when the user presses Ctrl+Q and the session is not at-risk, then the app exits through the same quit path (no new menu required).
- Given a saved position that places the title bar outside every display user area, when Standalone finishes launch sizing, then the title-bar drag strip is inside a display user area.
- Given a valid position on a secondary monitor, when Standalone launches and fits content, then the window is not forced back to the primary display.
- Given multi-monitor drag after launch, when the user moves the window between displays, then drag still works (constrainer remains null).

### Review Findings

- [x] [Review][Patch] Launch + one async placement ensure (not every resized) — Decision 2026-10-10 option 3: stop continuous ensure inside every `fitWindowToContent`/`resized`; run ensure after initial show/fit and once more via `callAsync` (covers unknown native frame top without fighting multi-monitor drag) [`Source/Standalone/MatrixControlStandaloneFilterWindow.h` / `MatrixControlStandaloneApp.cpp`]
- [x] [Review][Patch] Strengthen Ctrl+Q bind test beyond keypress registration [`Tests/Unit/StandaloneWindowPlacementTests.cpp`]
- [x] [Review][Patch] Unit-cover `findNearestDisplayForBounds` hit + distance fallback [`Tests/Unit/StandaloneWindowPlacementTests.cpp` / `StandaloneWindowPlacement.h`]
- [x] [Review][Defer] No Displays::Listener re-ensure on topology change [`Source/Standalone/MatrixControlStandaloneFilterWindow.h`] — deferred: out of launch-only matrix; monitor disconnect recovery is a later lot
- [x] [Review][Defer] No FilterWindow harness for Alt+F4→closeButtonPressed or fit→ensure call-site [`Tests/`] — deferred: already recorded in deferred-work for this spec; Manual UAT covers E2E

#### Rejected

- false — Centre fallback can return unreachable title bar: `preferredLimits` is always a display `userBounds` from the call site; prior triage already closed this.
- false — Diff mojibake of deferred/spec Unicode: PowerShell redirect artifact; on-disk UTF-8 is fine.
- false — `getKeyMappings()` null in `bindStandaloneQuitCommands`: JUCE `ApplicationCommandManager` always owns a mapping set after construction.
- false — Alt+F4 when `JUCEApplicationBase::getInstance()` is null consumes the key: unreachable for a live Standalone window; `closeButtonPressed` already null-checks.
- false — Linux Alt+F4 unhandled as a defect of this lot: frozen intent is Windows-only; already deferred.
- false — `fitWindowToContent` with null editor permanently skips ensure at launch: App `initialise` fits after the window exists with an editor; ensure runs when content is sized.
- low reject — Weak concrete X/Y assertions in placement tests: everyday risk low; intersect checks match matrix intent (prior triage).
- low reject — Alt+Shift+F4 not matched: rare chord; not everyday quit path.
- reject — Spec `status: done` while Manual UAT checkbox open: human-owned UAT task, not a code defect; fixing would only edit the spec.
- reject — Duplicate Verification Gap deferrals for Alt+F4 / fit call-site harness: already in deferred-work for this spec.

## Implementation Notes

- Helpers: `StandaloneWindowPlacement.h` + `StandaloneQuitCommands.h` (`bindStandaloneQuitCommands` templated — `juce_IncludeModuleHeaders.h` `#define Component juce::Component` breaks `juce::Component` in Standalone TUs).
- Review patches: skip ensure when native frame top unknown; nearest-display distance fallback; Quit bind helper + `quitCommandHasStandardKeypress` test.
- Build: `windows-debug` Standalone OK; tests `StandaloneWindowPlacement` + `PluginProcessorCloseGate` → 0 failures; `lint_touched.py` OK.
- Manual UAT still pending for end-to-end Alt+F4 / Ctrl+Q / multi-monitor drag.
- Code review 2026-10-10 patches applied: `ensureLaunchTitleBarOnScreen` after show/fit + async retry (removed from every `fitWindowToContent`); Quit bind test asserts first target + `invokeDirectly`; `findNearestDisplayForBounds` unit coverage.

## Spec Change Log

## Review Triage Log

- false — Alt+F4 only via window `keyPressed` vs KeyListener: JUCE walks parents from the focused component and invokes `MatrixControlStandaloneFilterWindow::keyPressed`; editor focus still reaches the handler unless a child consumes Alt+F4 (none do today).
- medium → patch — Unknown native frame top used 1 px client strip and could false-accept: now `ensureClientBoundsTitleBarOnScreen` no-ops when `frame.getTop() <= 0` until peer reports chrome.
- false — Recentre branch “unreachable”: defensive fallback when clamp strip still misses; left in place.
- medium → patch — Ctrl+Q coverage was a dead key helper: `bindStandaloneQuitCommands` + `quitCommandHasStandardKeypress` now used by App and tests.
- reject — Spec Verification commands incomplete: fix is edit of this build's spec (commands section updated separately as non-frozen hygiene).
- false — Encoding corruption in reviewed diff: PowerShell redirect artifact; on-disk deferred/spec UTF-8 readable (aside from a pre-existing em-dash glitch in an older deferred heading).
- reject — Change existing deferred `source_spec: none` entry: workflow forbids editing prior deferred rows.
- medium → patch — Fully off-screen `getDisplayForRect` not nearest: added `findNearestDisplayForBounds` distance fallback.
- false — Centre still misses all userAreas at call site: preferredLimits always comes from a member of userAreas.
- defer — Linux Alt+F4 not handled: frozen intent is Windows Standalone for this lot.
- defer — No harness asserts Alt+F4 invokes `closeButtonPressed` / `fitWindowToContent` calls ensure: Manual UAT + helper-boundary tests; no Standalone window test harness in repo.
- low reject — Incomplete concrete X/Y assertions in placement tests: everyday risk low; helper intersect checks cover matrix intent.

## Design Notes

**Why Alt+F4 breaks with native title bar:** JUCE attaches Alt+F4 to the custom close button only when `!isUsingNativeTitleBar()`. Matrix-Control forces the native bar, so that shortcut never exists. Focused content may also mark the SYSKEYDOWN as used, so DefWindowProc never emits `SC_CLOSE`. Explicit window-level Alt+F4 → `closeButtonPressed` restores the DocumentWindow contract without bringing back the custom title bar.

**Ctrl+Q:** `JUCEApplication` already implements Quit command info/perform; Matrix never created an `ApplicationCommandManager` or key-mapping listener. Wire that only — no File menu.

**Placement:** JUCE clamps in the base ctor using pre-fit size, then Matrix clears the constrainer and resizes via `fitWindowToContent`, which can push Y off-screen. Post-fit validity check (title-bar strip ∩ any `userBounds`) fixes bad saves without reintroducing continuous constrainer clamping.

**Agent decisions (no Open Questions):** Prefer clamp-to-nearest-display then centre-on-that-display if needed; attach command key mappings to `mainWindow`; Windows-only Alt+F4 handler (macOS keeps Cmd+Q / system terminate → `systemRequestedQuit`).

## Verification

**Commands:**
- `cmake --build --preset windows-debug --target Matrix-Control_Standalone` — expected: build succeeds.
- `cmake --preset windows-debug -DMATRIX_BUILD_TESTS=ON` then `cmake --build --preset windows-debug --target Matrix-Control_Tests` — expected: build succeeds.
- `Builds/Windows/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests.exe StandaloneWindowPlacement PluginProcessorCloseGate` — expected: 0 failures.
- `python Scripts/quality/lint_touched.py` — expected: clean on touched C++.

**Manual checks:**
- Alt+F4 and Ctrl+Q quit when clean; at-risk shows gate; Cancel aborts quit.
- Corrupt saved `windowY` (or move settings) → relaunch → title bar reachable.
- Place on second monitor, quit, relaunch → still on second monitor if still valid.
- Drag across monitors still works.

## Suggested Review Order

1. Alt+F4 → `closeButtonPressed` / native-title-bar gap
2. Ctrl+Q command manager wiring (no menu)
3. Post-fit title-bar on-screen helper + multi-monitor no-op
4. Confirm `systemRequestedQuit` gate untouched
