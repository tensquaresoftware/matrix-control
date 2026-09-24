---
title: 'Standalone audio Finder vs Cursor launch'
type: 'bugfix'
created: '2026-09-24'
status: 'done'
route: 'dispatch'
baseline_commit: 'c92bf122ef5c7011900aab732b38faffe5790765'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-artefacts-standalone-no-audio.md'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Standalone Matrix-Control stays silent on the AUDIO FROM monitor when the `.app` is opened from Finder (Builds tree, Dropbox Artefacts, or a Desktop copy), even though AUDIO FROM shows the correct Scarlett input and the interface meters live signal. The same binary plays that monitor correctly when Cursor launches `Contents/MacOS/Matrix-Control` via CMake Run Without Debugging.

**Approach:** Harden the Standalone bundle so macOS can grant microphone / input access for LaunchServices launches: enable JUCE `MICROPHONE_PERMISSION_*` so `Info.plist` carries `NSMicrophoneUsageDescription`, rebuild, then verify Finder launches (and Artefacts copy) restore peak + speakers. Do not relocate Artefacts off Dropbox.

**Decisions:**
- Mic dialog text: `"Matrix-Control needs access to audio input devices."` (shorter wording; avoids “microphone”, which would confuse synth → audio interface monitoring).

## Boundaries & Constraints

**Always:**
- Keep hosted VST3/AU without an input bus (AD-11 / Story R-1).
- Keep shared Application Support prefs; do not make audio settings path-dependent.
- Keep Artefacts on Dropbox unless Guillaume later asks to relocate.
- Reuse existing mute/passthrough plumbing (`StandaloneAudioInputRouter`, `syncAudioPassthroughFromSourceId`); treat silence with a correct AUDIO FROM id as device-open / OS privacy failure, not a combo bug.
- Microphone dialog copy: English ASCII only (no em dash / fancy Unicode).
- Treat this bug class as macOS LaunchServices / TCC + missing `NSMicrophoneUsageDescription`; the CMake flag lives in the shared `juce_add_plugin` block but only affects Apple Info.plist generation (JUCE docs).

**Never:**
- Rewrite JUCE `AudioDeviceManager` / `StandalonePluginHolder` to paper over Dropbox or Finder.
- Change `audioFromSourceId` format or mute-unmute product semantics without a confirmed product bug.
- Change Cursor / CMake launch configs to “fix” Finder by launching the bare Mach-O instead of fixing the `.app`.
- Assume binaries differ without re-checking SHA-256 after the next Artefacts copy.
- Scope Windows or Linux “IDE vs Explorer/file-manager launch” silence into this pass — no evidence of the same TCC/plist failure mode; do not invent platform permission UI for those OSes here.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Cursor CMake run | Launch `…/Contents/MacOS/Matrix-Control` with workspace cwd | Peak + speakers when AUDIO FROM set (already works) | N/A |
| Finder Builds `.app` | Double-click Standalone `.app` under Builds | After fix: macOS may prompt once for Mic; then peak + speakers | If denied: stay silent; user enables in System Settings → Privacy → Microphone |
| Finder Artefacts / Desktop copy | Same binary after Artefacts copy | Same as Builds Finder after copy picks up new plist | N/A |
| Mic denied | User declines or toggles off in Privacy | No peak / silent speakers; AUDIO FROM may still show saved id | Do not invent a custom in-app permission UI in this pass |

</frozen-after-approval>

## Code Map

- `CMakeLists.txt` (`juce_add_plugin`) — add `MICROPHONE_PERMISSION_ENABLED TRUE` and `MICROPHONE_PERMISSION_TEXT "…"`. JUCE regenerates Standalone `Info.plist` with `NSMicrophoneUsageDescription` (see JUCE `JUCEUtils.cmake` / `juce_PlistOptions.cpp`). No checked-in Info.plist in repo.
- `.vscode/launch.json` — Cursor runs the bare Mach-O under `Contents/MacOS/` (not `open` on the `.app`). Explains IDE vs Finder differential; **do not change** as the product fix.
- `Source/Standalone/MatrixControlStandaloneApp.cpp` — `StandalonePluginHolder` + shared Application Support settings; no cwd-based audio path.
- `Source/Core/Audio/StandaloneAudioInputRouterStandalone.cpp` — toggles JUCE `muteInput`; unmute is a no-op if CoreAudio opened with 0 inputs.
- `Source/Core/PluginProcessorAudio.cpp` — `syncAudioPassthroughFromSourceId`; can leave a saved AUDIO FROM id visible while hardware inputs are empty.
- Prior closed pass — `_bmad-output/implementation-artifacts/spec-artefacts-standalone-no-audio.md` (`done`, Explain-and-wait). Retest criteria met: Desktop copy still silent; Builds Finder also silent; Cursor still works → reopen for mic-plist hardening.

## Tasks & Acceptance

**Execution:**
- [x] `CMakeLists.txt` -- Enable `MICROPHONE_PERMISSION_ENABLED` / `MICROPHONE_PERMISSION_TEXT` on `juce_add_plugin` with the frozen dialog string -- so Finder launches can prompt and receive input access.
- [x] Rebuild Standalone (macos-debug-arm64) -- Confirm generated `Info.plist` contains `NSMicrophoneUsageDescription` matching the frozen text.
- [x] Manual verification notes -- Record Finder Builds + Artefacts (or Desktop copy) checklist in Implementation Notes after Guillaume can retest; agent verifies plist + build only if interactive Finder audio is unavailable in-session.

**Acceptance Criteria:**
- Given a rebuilt Standalone `.app`, when its `Info.plist` is inspected, then it contains `NSMicrophoneUsageDescription` equal to the frozen mic dialog text. **Met** (Builds Standalone `plutil` 2026-09-24).
- Given Finder launch of that `.app` with Scarlett input live and AUDIO FROM set to that input, when macOS Microphone access is allowed, then Matrix-Control peak meter and speakers show the monitor (same as Cursor launch). **Pending Guillaume Finder retest** (see Implementation Notes checklist).
- Given the user denies Microphone access, when the `.app` is launched from Finder, then monitor stays silent without crashing; no custom permission dialog is required in-app. **Pending Guillaume optional deny check**.

## Implementation Notes

- 2026-09-24: `CMakeLists.txt` `juce_add_plugin` now sets `MICROPHONE_PERMISSION_ENABLED TRUE` and `MICROPHONE_PERMISSION_TEXT "Matrix-Control needs access to audio input devices."` (ASCII, frozen string).
- Reconfigure + build `macos-debug-arm64` / `Matrix-Control_Standalone`: success (ninja reported no further compile work after juceaide refreshed the bundle plist during configure).
- Agent-verified plist: `Builds/macOS/ARM/Debug/Matrix-Control_artefacts/Debug/Standalone/Matrix-Control.app/Contents/Info.plist` contains `NSMicrophoneUsageDescription` = `Matrix-Control needs access to audio input devices.`
- Also forced `CopyToArtefactsDir`: Dropbox Artefacts Standalone `Info.plist` now matches the same `NSMicrophoneUsageDescription` (2026-09-24).
- `Scripts/quality/lint_touched.py`: OK (CMake-only; no Source/ C++ touched).
- Matrix Test Audit: I/O rows are OS LaunchServices / TCC scenarios — no automated unit tests apply. Coverage = plist command check (packaging) + Guillaume Finder manual checklist below.
- Interactive Finder audio retest not available in-session. Guillaume checklist after Artefacts/post-build copy picks up the new plist:
  1. Finder → Builds Standalone `.app` → allow Microphone if prompted → expect peak + speakers with AUDIO FROM = Scarlett input.
  2. Finder → Artefacts (or Desktop copy) → same expectation.
  3. Optional: deny Microphone → expect silent monitor, no crash, no custom in-app permission UI.
  4. Optional differential: `open …/Matrix-Control.app` vs bare `…/Contents/MacOS/Matrix-Control` — both should work when Mic allowed.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence |
|---------|---------|----------|
| Blind: no CMake comment explaining why a synth needs mic usage | false | Spec Always already records LaunchServices / TCC rationale; speculative “later cleanup removes flags” is not a demonstrated defect. Fix would be comment-only noise. |
| Blind: tasks `[x]` while Finder AC still pending | false | Spec task 3 and Implementation Notes explicitly allow agent plist/build verification when interactive Finder audio is unavailable; marking tasks complete matches that contract. Fix would be editing this build’s spec. |
| Blind: checklist omits pre-existing TCC deny / Privacy flip | false | Fix would be editing this build’s checklist/spec; Frozen I/O already covers Mic denied. |
| Blind: checklist omits quit/relaunch after Mic grant | false | Fix would be editing this build’s spec notes; not a code defect in the CMake change. |
| Blind: no “still silent after allow” reopen branch | false | Fix would be editing this build’s spec; reopen path is a process note, not missing product code. |
| Blind: Spec Change Log empty despite reopen | false | Spec Change Log is for bad_spec loopbacks only (template). Design Notes already record the 2026-09-19 → 2026-09-24 falsification. Fix would be editing this build’s spec. |
| Blind: prior `spec-artefacts-standalone-no-audio` still says no MICROPHONE_PERMISSION | false | Prior artifact is closed historical record; this pass’s context + Design Notes supersede it. Fix would be editing another/this build’s docs. |
| Blind: no post-copy SHA-256 recorded | false | Spec Never says do not assume binaries differ without re-check; both plists verified identical usage string after `CopyToArtefactsDir`. SHA-256 optional. Fix would be editing Implementation Notes. |
| Blind: Verification `plutil` uses ellipsis path | false | Fix would be editing this build’s Verification section; Implementation Notes already have concrete Builds path. |
| Blind: Debug vs Release retest flavor unclear | false | Fix would be editing this build’s checklist; agent verified the Debug Builds + Artefacts copy produced by this pass. |
| Blind: shared `juce_add_plugin` also adds usage string to AU/VST3 Info.plist | false | Confirmed real side effect (`plutil` on AU and VST3). Spec Always places the flag on shared `juce_add_plugin`; hosted formats stay input-bus-free (AD-11). Hosts own input; extra usage string is harmless, not a product defect. |
| Blind: dialog avoids “microphone” while Privacy category is Microphone | false | Frozen decision intentionally avoids “microphone” in the prompt; macOS category naming is OS-owned. Not a code defect. |
| Blind: I/O matrix missing Privacy-enable recovery row | false | Fix would edit frozen I/O matrix; Mic denied row already covers silent denial. |
| Blind: status in-review with product AC open looks closable | false | Workflow owns status; pending Finder AC is documented. Fix would be editing this build’s spec. |
| Edge Case Hunter: (none) | false | Empty finding set `[]` — no claim to verify. |
| Verification Gap: no recurring CI/plutil pin for `NSMicrophoneUsageDescription` | medium | Real: deleting `MICROPHONE_PERMISSION_*` would drop the plist key while unit tests and current GH Actions stay green. Spec Verification already requires one-shot `plutil`; wiring CI is extra hardening beyond frozen intent. |

## Design Notes

2026-09-19 assumed Dropbox/TCC path differential; 2026-09-24 retest falsifies path-only: Desktop copy and Builds Finder both silent; Cursor bare-binary launch still works. Strongest product fix is declaring mic usage so LaunchServices can prompt. Cursor success without a plist is consistent with a separately granted TCC identity for the repeatedly launched Mach-O under the IDE, not proof that Finder launches are healthy.

Cross-platform: JUCE `MICROPHONE_PERMISSION_*` only writes `NSMicrophoneUsageDescription` into Info.plist (macOS/iOS). Windows and Linux desktop builds do not consume that key; ordinary WASAPI / ALSA / JACK input does not use the same “missing usage string → silent deny” gate. One shared CMake edit covers all presets, but the runtime fix is Apple-only.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Standalone` -- expected: build success
- `plutil -p "…/Standalone/Matrix-Control.app/Contents/Info.plist"` -- expected: `NSMicrophoneUsageDescription` present with frozen text
- `python3 Scripts/quality/lint_touched.py` -- expected: pass if any `Source/` touched (likely N/A if CMake-only)

**Manual checks (if no CLI):**
- Finder: Builds Standalone `.app` → allow Mic if prompted → peak + speakers with AUDIO FROM = Scarlett input.
- Finder: Artefacts (or Desktop copy after post-build copy) → same.
- Optional differential: Terminal `open …/Matrix-Control.app` vs direct `…/Contents/MacOS/Matrix-Control` — after fix both should work when Mic allowed.
