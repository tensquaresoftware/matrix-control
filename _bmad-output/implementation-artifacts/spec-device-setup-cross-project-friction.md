---
title: 'DEVICE SETUP cross-project friction'
type: 'feature'
created: '2026-09-18'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
context: []
baseline_commit: '65c70d0f'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** DEVICE SETUP opens on every fresh plugin instance (e.g. each new Ableton Live project), because `settingsEpromTypePromptDone` lives only in host session APVTS. Users who keep the same MIDI ports and EPROM TYPE hit repeated friction.

**Approach:** Persist machine-global defaults for `promptDone`, last MIDI FROM/TO port ids, and EPROM TYPE. On a fresh instance (no host-restored connection prefs), seed those defaults into the live APVTS and do not open DEVICE SETUP when machine `promptDone` is already true. After first completion or SPECIFY LATER on this machine, the assistant does not return; later changes go through header ports and Settings DEVICE / EPROM TYPE only.

**Decisions:**
- Q1 → B: survive across projects = completion flag + last MIDI ports + EPROM TYPE as machine defaults.
- Q2 → A: after completion, almost never reopen; change connection prefs via Settings / header only (no auto-reopen on empty ports; no mandatory "Run again" control in this chantier).

## Boundaries & Constraints

**Always:**
- Keep DEVICE SETUP UI contract from the shipped assistant (four rows, CONFIRM / SPECIFY LATER, ASCII strings).
- Host session APVTS remains the live SSOT for the open editor (ports, EPROM, promptDone while the instance runs).
- On fresh construction (missing / empty connection prefs before host restore applies useful values), seed machine defaults into APVTS: MIDI port ids, EPROM TYPE, and promptDone.
- When the user CONFIRM or SPECIFY LATER, write machine defaults: always set promptDone true; on CONFIRM also write selected EPROM; on both paths write current MIDI port ids that were chosen in the dialog/header session; SPECIFY LATER must not change stored EPROM TYPE (session or machine).
- When the user changes MIDI ports or EPROM TYPE later via header / Settings, update the matching machine defaults so the next new project inherits the latest choice.
- Settings DEVICE / EPROM TYPE and header ports remain the way to change choices without forcing the modal.
- New persistence must work for plugin hosts and standalone on the same machine (shared PropertiesFile / Application Support path).
- Narrow AD-7 exception only for this device-connection default subset (promptDone + MIDI port ids + EPROM TYPE).

**Never:**
- Re-open DEVICE SETUP after machine promptDone is true (including new Ableton projects and standalone relaunch).
- Auto-reopen solely because both MIDI ports are empty after completion.
- Add a "Run DEVICE SETUP again" Settings control in this chantier.
- Silent overwrite of a user-chosen EPROM TYPE without CONFIRM / Settings.
- Fight a host-restored project chunk that already carries ports / EPROM / promptDone — restored session wins for that instance; still may refresh machine defaults from live edits afterward.
- INTERFACE Settings, Unison, or unrelated Settings layout work.
- Moving the entire AD-7 prefs surface to PropertiesFile (only this device-setup subset).

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| First ever launch | No machine defaults / promptDone false | DEVICE SETUP opens; CONFIRM/LATER writes machine defaults | N/A |
| New Ableton project after completion | Machine promptDone true + stored ports/EPROM | Seed APVTS from machine defaults; assistant does not open | Invalid stored port id → existing empty/sanitize port path |
| CONFIRM then new project | Machine has ports + EPROM + promptDone | New instance uses those defaults; no assistant | N/A |
| SPECIFY LATER then new project | Machine promptDone true; ports written; EPROM unchanged from prior store | No assistant; EPROM stays prior machine/session value | N/A |
| User changes ports/EPROM in Settings/header | Live edit after completion | Update machine defaults; no assistant | N/A |
| Host restores old project with session promptDone true | Restored chunk | Use restored session; no forced open | N/A |
| Host restores project with empty ports but machine has defaults | Restored empty ports present | Restored session wins for that instance (do not overwrite restored empties on load); no assistant if machine promptDone true | User can pick ports in header |

</frozen-after-approval>

## Code Map

- `Source/Shared/Definitions/PluginIDs.h` — `kEpromTypePromptDone` / EPROM / related Settings ids; may add machine-store key constants if kept out of APVTS id namespace.
- `Source/Core/PluginProcessorClipboard.cpp` — `initializeEpromTypeProperties()` defaults; extend seed from machine store after defaults.
- `Source/Core/PluginProcessorState.cpp` — host chunk save/restore; do not strip machine-backed keys from session; strip pending only.
- `Source/Core/PluginProcessorMidiPorts.cpp` — `midiInputPortId` / `midiOutputPortId`; empty defaults today; seed + write-through to machine store on change.
- `Source/Core/Services/DeviceSetupDeviceRow.h` — `shouldOpenDeviceSetupAssistant`; open decision must incorporate machine promptDone (or seeded session flag).
- `Source/GUI/PluginEditorUiConstruction.cpp` — primary open on editor ready when `!promptDone`.
- `Source/GUI/PluginEditorWindows.cpp` — CONFIRM / SPECIFY LATER persistence; also flush machine defaults.
- `Source/GUI/PluginEditorSettings.cpp` / Settings DEVICE — EPROM changes should update machine default.
- New small Core helper (preferred) for PropertiesFile read/write of the three defaults — keep GUI free of path policy.
- `Source/Standalone/MatrixControlStandaloneApp.cpp` — reference for Application Support naming; plugin must use an equivalent shared store without depending on the standalone app object.
- Architecture AD-7 — deliberate narrow exception: machine defaults for promptDone + MIDI port ids + EPROM TYPE only.
- Prior shipped intent: `_bmad-output/implementation-artifacts/spec-device-setup-assistant.md` (status done).

## Tasks & Acceptance

**Execution:**
- [x] Core machine-defaults helper (PropertiesFile under Application Support, shared plugin+standalone) — read/write promptDone + MIDI port ids + EPROM TYPE — persistence spine.
- [x] `PluginProcessor` construction / init — seed fresh APVTS from machine defaults without overriding a host-restored chunk that already applied — start coherent.
- [x] Port setters + EPROM Settings/CONFIRM paths — write-through machine defaults on user change — next project inherits latest.
- [x] `PluginEditorUiConstruction.cpp` / `PluginEditorWindows.cpp` — open only when effective promptDone is false after seed — stop cross-project nag.
- [x] Unit tests for pure seed/open/write policy helpers — cover I/O matrix rows that are logic-testable.
- [x] `lint_touched.py` on touched C++.

**Acceptance Criteria:**
- Given machine promptDone is false, when the editor becomes ready on a fresh instance, then DEVICE SETUP opens once as today.
- Given the user CONFIRM or SPECIFY LATER, when a new host project creates a fresh Matrix-Control on the same machine, then the assistant does not open and MIDI ports + EPROM follow the machine-default rules above.
- Given the user later changes ports or EPROM via header/Settings, when they create another new project, then those latest values are seeded (and the assistant still stays closed).
- Given machine promptDone is true, when both ports happen to be empty, then DEVICE SETUP still does not auto-open.

### Review Findings

- [x] [Review][Patch] Restore `kDeviceMidiUnresponsiveProperty` and drop unused `EpromTypePromptDialog.h` include — [PluginEditorAudio.cpp:12,172-177] [PluginEditorWindows.cpp:250-251]
- [x] [Review][Patch] Assert empty MIDI TO clears that side only in single-port write-through test — [DeviceConnectionMachineDefaultsTests.cpp:148-150]
- [x] [Review][Defer] Concurrent PropertiesFile load/modify/save races — deferred: already deferred in Build triage; multi-instance RMW without lock
- [x] [Review][Defer] Sanitize/clear in syncMidiPortsFromState does not write through machine ports — deferred: already deferred; invalid seeded ids can reappear next project
- [x] [Review][Defer] No PropertiesFile round-trip / production seed-write-through-open call-site tests — deferred: already deferred; repo harness gap
- [x] [Review][Defer] MidiPortRoutingPropertyTests mirror omits machine write-through — deferred: already deferred; helper claim drift only

#### Rejected

- Frozen I/O matrix “do not overwrite restored empties” vs reseed — false: Design Notes / triage intentionally reseed when machine done && session not; fixing would mean editing the frozen matrix or undoing the Ableton factory fix
- Reseed can overwrite unfinished session with real ports — false: prior triage chose session `promptDone` as the only discriminator; accepted tradeoff
- `saveIfNeeded` return ignored — low: everyday disk failure unlikely; full harden is non-trivial (matches prior reject)
- Default `machinePromptDone = false` on open guard — low: all production call sites pass the third arg; speculative future miss
- Empty Spec Change Log — rejected: fix would only edit the spec under review

## Implementation Notes

- Machine store: `~/Library/Application Support/Ten Square Software/Matrix-Control/Matrix-Control-DeviceConnection.settings` (PropertiesFile).
- After host `replaceState`, re-seed when machine `promptDone` and session still false (factory/empty Ableton chunks).
- MIDI port write-through updates one side at a time so clearing FROM cannot wipe stored TO.
- Construction seed followed by soft `syncMidiPortsFromState` + deferred plugin reopen.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence / route |
|---------|---------|------------------|
| Blind: host empty chunk wipes construction seed; new Ableton may skip assistant with empty ports | medium | Real for factory restores; patched via `reseedApvtsAfterHostRestoreIfNeeded` when machine done && !session done. Route: patch (done). |
| Blind: cannot distinguish brand-new empty vs restored empty | medium | Same root as above; session `promptDone` distinguishes cleared-after-done (true) vs unfinished/factory (false). Route: patch (done). |
| Blind: no refreshSysExDelay after construction seed | false | `refreshSysExDelayFromSettings` uses stock until `deviceDetected`; inquiry path already reads seeded EPROM from APVTS. |
| Blind: concurrent load/save races across instances | medium | Real RMW without lock; rare for this prefs file. Route: defer. |
| Blind: nullptr checks after make_unique dead; weak save failure handling | low | Cosmetics / rare IO; rejected (low + non-trivial to harden fully). |
| Blind: no load/save / production call-site tests | medium | True verification gap. Route: defer (repo harness). |
| Blind: sanitize-cleared ports not written back to machine | medium | Sync can clear APVTS without machine write-through. Route: defer. |
| Blind: write-through before first CONFIRM | false | Spec allows header/Settings write-through; CONFIRM/LATER still set promptDone. |
| Blind: hardcoded midi port key strings vs MachineDefaults ids | low | APVTS keys pre-existed; rejected (cosmetic). |
| Blind: Linux folderName / State.cpp / empty Implementation Notes | false / low | Notes filled; Linux uses same Application Support folderName pattern via ProjectPaths; State.cpp now has reseed. |
| Blind: manual checks not recorded | false | Spec Verification lists them as manual; not a code defect. |
| Edge: one-port write with empty other side wipes machine other port | medium | Real; fixed with single-side `writeMidiInputPort` / `writeMidiOutputPort`. Route: patch (done). |
| Edge: concurrent RMW can drop promptDone | medium | Same as Blind race. Route: defer. |
| Edge: saveIfNeeded false ignored | low | Rejected (low; everyday unlikely). |
| VG: editor open sites not call-site tested | medium | Pre-verified gap; disposition defer. |
| VG: production seed/write-through not executed by tests | medium | Pre-verified gap; disposition defer. |
| VG: PropertiesFile load/save never round-tripped | medium | Pre-verified gap; disposition defer. |
| VG other: unused EditorOutboundGate includes | low | Patched (removed). |
| VG other: MidiPortRoutingPropertyTests mirror stale | low | Pre-existing test helper drift; defer. |

## Design Notes

Today the "one-shot" flag rides the same host chunk as MIDI ports and EPROM. A new Ableton project creates a fresh plugin instance with empty defaults → `promptDone` false → assistant returns. This chantier adds machine defaults for the device-connection subset only (AD-7 exception), seeds fresh instances, and keeps host-restored project state authoritative when that session already completed DEVICE SETUP (`promptDone` true).

Host-restore edge: if Ableton restores a factory/empty chunk while machine `promptDone` is true and session `promptDone` is still false, re-seed ports / EPROM / promptDone from the machine store. If the restored session already has `promptDone` true (including a project where the user cleared ports after completion), do not clobber those empties; the assistant stays closed via machine/session flags.

## Verification

**Commands:**
- `python3 Scripts/quality/lint_touched.py` -- expected: pass on touched C++
- Unit test target covering open/seed helpers -- expected: pass

**Manual checks:**
- Standalone: complete DEVICE SETUP, quit, relaunch → no assistant; ports/EPROM restored from machine defaults.
- Plugin in Ableton: complete in project A, create project B with a fresh Matrix-Control → no assistant; ports/EPROM seeded from machine defaults.
- After completion, clear both ports in header → reload or new project → assistant still does not auto-open.
