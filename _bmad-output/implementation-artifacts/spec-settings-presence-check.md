---
title: 'Settings PRESENCE CHECK (device re-proof interval)'
type: 'feature'
created: '2026-10-29'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'a3e06fd888f5a755e4485ff5acd7b3893a85fa4f'
context:
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Matrix-Control always re-proves a detected synth on a fixed ~5 s heartbeat. For Matrix-6/6R that may mean a full Master dump after a mute Inquiry, which is reliable but heavy on the MIDI bus. Settings has no control for this, yet Getting Started already tells users to change it under Settings > MIDI & DEVICE > PRESENCE CHECK.

**Approach:** Add a Settings > MIDI & DEVICE combo **PRESENCE CHECK** (under DEVICE, before EPROM TYPE) with ONCE ON CONNECT / EVERY 5 S / EVERY 15 S / EVERY 30 S, default EVERY 15 S for all models. Wire the detected-state presence timer to that preference; keep initial connect discovery and Master-fallback presence policy unchanged.

## Boundaries & Constraints

**Always:**
- Tab MIDI & DEVICE; row order DEVICE → PRESENCE CHECK → EPROM TYPE → HARDWARE LATENCY (plugin only).
- Combo values (ASCII): `ONCE ON CONNECT`, `EVERY 5 S`, `EVERY 15 S`, `EVERY 30 S`.
- Default `EVERY 15 S` for Matrix-1000 and Matrix-6/6R (no AUTO, no per-model default).
- ONCE ON CONNECT: keep connect-time discovery / first proof (Inquiry, then Matrix-6/6R Master fallback when needed); do not run periodic re-proof while detected and responsive.
- Exception to ONCE: while soft-abort / `deviceMidiUnresponsive` (or equivalent) is set, resume periodic re-proof so a brief silence can recover without replugging; when responsive again, return to ONCE (no detected heartbeat).
- Undetected retry stays at 1 s while both ports are open (late power-on / recovery), including when preference is ONCE ON CONNECT.
- Changing the combo while ports are open must reschedule (or stop) the presence timer immediately.
- Persist via APVTS Settings property **and** DeviceConnectionMachineDefaults (machine-wide, same family as MIDI ports / EPROM); seed APVTS from machine on fresh construction / reseed path; write-through on Settings change.
- Default when neither store has a value: EVERY 15 S.
- ASCII-only UI copy in `PluginDisplayNames` (label, combo items, footer contextual help).
- Wizard: no new control; keep existing Synth communication body that points at this Settings option.

**Never:**
- AUTO or different defaults by device family.
- Free-form / custom millisecond intervals.
- Replace Master-fallback presence protocol with a lighter ping.
- Broad MidiManager refactor; soft-abort / Master-fallback family logic changes beyond interval branching and the ONCE+unresponsive resume rule above.
- New Getting Started Wizard control for this preference.
- Session-only persistence that leaves different presence intervals scattered across host projects for the same machine.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Fresh install | No preference key | Default EVERY 15 S; detected heartbeat every 15000 ms | N/A |
| EVERY 5 / 15 / 30 S + detected | Both ports open, deviceDetected | Timer interval 5000 / 15000 / 30000 ms; tick still runs Inquiry (+ M6 Master fallback policy unchanged) | Skip tick while async SysEx capture busy |
| ONCE ON CONNECT + detected + responsive | deviceDetected true, not unresponsive | No periodic detected heartbeat | N/A |
| ONCE ON CONNECT + undetected | Ports open, not detected | Keep 1 s undetected retry Inquiry | N/A |
| ONCE ON CONNECT + soft-abort / unresponsive | deviceDetected true, unresponsive set | Resume periodic re-proof (use EVERY 15 S interval unless a future explicit override exists; for this version use EVERY 15 S as recovery cadence) until responsive again | Existing soft-abort paths |
| Preference change live | User switches EVERY 5 S ↔ ONCE while connected | `updateDevicePresenceMonitoring` applies new interval or stops detected timer; write APVTS + machine defaults | Clamp invalid stored ids to default |
| Fresh machine / reseed | Machine store empty or seeded | EVERY 15 S in APVTS after seed | N/A |
| Port pair incomplete | Missing From or To | Timer stopped (existing gate) | N/A |
| Soft-abort / send fail / port change | Existing paths | Soft-abort family unchanged; ONCE resumes heartbeat only while unresponsive; port change still forces Inquiry | Existing failure paths |

**Decisions locked:**
- Soft-abort under ONCE → resume periodic re-proof while unresponsive, then return to ONCE when responsive.
- Persistence → APVTS + machine-wide DeviceConnectionMachineDefaults (expand AD-7 for this key).
- Spec size → keep full spec (~2200 tokens accepted).

</frozen-after-approval>

## Code Map

- `Source/GUI/Settings/SettingsPanelSetup.cpp` / `SettingsPanel.cpp` / `SettingsPanel.h` — insert PRESENCE CHECK label+combo after DEVICE, before EPROM; visibility/looks/populate; bump `midiAndDevicePageRowCount` (+1).
- `Source/GUI/Settings/SettingsShellMetrics.h` — row-count helper; tallest page still AUDIO (11) so dialog height unchanged.
- `Source/Shared/Definitions/PluginIDs.h` — `kPresenceCheck` + nested choice ints + `kDefault = EVERY 15 S`.
- `Source/Shared/Definitions/PluginDisplayNames.h` — label, four combo strings, footer contextual help (ASCII).
- `Source/Core/PluginProcessor*.{h,cpp}` — initialize APVTS property when missing; seed from machine defaults with ports/EPROM.
- `Source/Core/Services/DeviceConnectionMachineDefaults.{h,cpp}` (+ tests) — extend Values / load / save / seed / write-through for presence-check id (AD-7 expansion).
- `Source/GUI/PluginEditorSettings.cpp` — restore/wire combo; onChange → APVTS + machine write + reschedule presence monitoring.
- `Source/Core/MIDI/MidiManagerDeviceInquiry.cpp` — `updateDevicePresenceMonitoring` reads preference; ONCE disables detected heartbeat unless unresponsive (then recovery cadence); keep undetected 1 s.
- `Source/Core/MIDI/Queue/MidiRequestTiming.h` — keep undetected 1 s constant; detected 5 s becomes one mapped value (EVERY 5 S), not the sole detected SSOT.
- New small pure helper (prefer under `Source/Core/MIDI/` or Settings policy header) — preference id → detected interval ms (0 = no detected heartbeat); separate predicate or branch for ONCE+unresponsive recovery; unit-tested.
- Contextual help — Settings currently has no `ContextualHelpBinder`; add binder for PRESENCE CHECK (label/combo) resolving main footer, matching other Settings help intent.
- `Tests/Unit/SettingsTabsShellTests.cpp` — row counts (plugin/standalone become 6) + label/copy contract as needed.
- New unit tests — mapping preference → interval / disable; ONCE+unresponsive → recovery interval; invalid id → default; machine-defaults round-trip for new key.
- Reuse: EPROM write-through / seed pattern; `updateDevicePresenceMonitoring` as single schedule hook.
- Do not change: `Matrix6MasterFallbackPolicy` core rules, Device read-only strip, EpromTypePolicy, Wizard controls.
- Related continuity (done): `spec-m6r-presence-heartbeat-footer.md`, `spec-device-unresponsive-presence-sysex-brake.md`.

## Tasks & Acceptance

**Execution:**
- [x] `PluginIDs.h` / `PluginDisplayNames.h` — PRESENCE CHECK ids, default EVERY 15 S, ASCII label/items/help.
- [x] `SettingsPanel*` + `SettingsShellMetrics.h` + `PluginEditorSettings.cpp` + processor init + `DeviceConnectionMachineDefaults*` — UI row, APVTS + machine persist, wire, live reschedule.
- [x] Presence timer branch + pure mapping helper — detected interval / ONCE disable / ONCE+unresponsive recovery; keep undetected 1 s.
- [x] Settings contextual help bind for PRESENCE CHECK.
- [x] Unit tests — mapping + Settings row-count/copy; register in `Tests/CMakeLists.txt` if new file.
- [x] Keep Wizard body pointing at Settings > MIDI & DEVICE > PRESENCE CHECK (already present in working tree).

**Acceptance Criteria:**
- Given Settings > MIDI & DEVICE, when the page is shown, then PRESENCE CHECK appears under DEVICE and before EPROM TYPE with the four ASCII values and default EVERY 15 S.
- Given EVERY N S and a detected device with both ports open, when the presence timer runs, then its interval is N seconds.
- Given ONCE ON CONNECT and a detected responsive device, when monitoring updates, then no periodic detected heartbeat runs; if unresponsive, periodic re-proof resumes until responsive again; undetected retry remains 1 s if not yet detected.
- Given the user sets PRESENCE CHECK on one host/standalone instance, when another fresh instance on the same machine seeds from machine defaults, then the same preference value is restored.
- Given the user changes PRESENCE CHECK while connected, when the combo commits, then the timer reschedules or stops without requiring a reconnect.
- Given hover/focus on PRESENCE CHECK with contextual help SHOW, when the footer updates, then the new ASCII help sentence appears.
- Given Getting Started Synth communication copy, when read, then it still references Settings > MIDI & DEVICE > PRESENCE CHECK and that control exists.

## Implementation Notes

- Pure helper: `Source/Core/MIDI/PresenceCheckPolicy.h` (`normalize`, `presenceCheckDetectedIntervalMs`, `detectedPresenceTimerIntervalMs`, `monitoringIntervalMs`).
- ONCE + unresponsive recovery cadence: EVERY 15 S (`kDevicePresenceHeartbeatEvery15sMs`), composed in `detectedPresenceTimerIntervalMs`; `setDeviceMidiUnresponsive` reschedules via `updateDevicePresenceMonitoring`.
- Settings contextual help binder resolves the main footer through `PluginEditor` (Settings is not under `MainComponent`).
- Persistence: APVTS `settingsPresenceCheck` + machine defaults key of the same name; seed/write-through mirrors EPROM.
- Review patch: restore/wire moved to `PluginEditorSettingsDevice.cpp` with EPROM; restore coerces invalid ids back to APVTS; `monitoringIntervalMs` is the schedule SSOT used by MidiManager.

## Spec Change Log

## Review Triage Log

- medium / patch — MidiManager presence schedule untested vs Settings preference — evidence: only pure mapping tested; fixed via `monitoringIntervalMs` + unit cases (ports/undetected/EVERY 15/ONCE/ONCE+unresponsive) wired into `updateDevicePresenceMonitoring`.
- medium / patch — ONCE + unresponsive reschedule untested on MidiManager — evidence: `setDeviceMidiUnresponsive` calls update but no timer assert; same pure schedule SSOT covers recovery cadence; property flip still relies on existing Soft-abort paths calling that hook.
- medium / patch — restore under Appearance without APVTS coerce write-back — evidence: EPROM/policy restore write coerced ids; fixed by `restoreSettingsPresenceCheckFromState` + wire in `PluginEditorSettingsDevice.cpp`.
- low / patch — `applyConfirm` comment omitted presenceCheckId preservation — evidence: comment updated.
- low / patch — `maxBodyDesignHeightBelowSettings` comment still said non-Audio only — evidence: comment updated for Synth Communication allow-list.
- low / reject — PresenceCheckPolicy under Core/MIDI vs EpromTypePolicy under Services — evidence: preference drives MIDI timer; MIDI placement is coherent; move would be churn without user harm.
- low / reject — initializeEpromTypeProperties also seeds presence without rename — evidence: works; rename-only cleanup adds surface for no user-facing gain.
- low / reject — refreshDevicePresenceMonitoringFromSettings one-line alias — evidence: keeps Settings from calling private `updateDevicePresenceMonitoring`; intentional thin public hook.
- low / reject — updatePageVisibility toggles latency before EPROM — evidence: setVisible order does not affect layout or behavior.
- false — only PRESENCE CHECK has Settings help binder — evidence: story scope is this control; neighboring rows were never promised help.
- false — default heartbeat 5 s → 15 s needs migration note in code — evidence: frozen intent locks EVERY 15 S default for all models.
- false — Edge Case Hunter empty list — evidence: no unhandled path filed.
- reject — spec Code Map / Verification / status tracking stale — evidence: fixing would only edit this build's spec (forbidden triage route).
- defer — sibling AD-7 / device-setup docs still say machine-defaults = promptDone + ports + EPROM only — evidence: intent expanded AD-7 for PRESENCE CHECK; docs not updated in this diff.
- defer — ContextualHelpOverlay Wave 3 static_assert still keys on `registerContextualHelp` name while Settings added `registerPresenceCheckContextualHelp` — evidence: assert no longer tracks the real binder entry point; rename/assert update is follow-up hygiene.
- defer — no PropertiesFile load/save round-trip test for writePresenceCheck — evidence: seed/applyPresenceCheck covered; file I/O twin of EPROM not added here.

## Design Notes

- Prefer a pure `presenceCheckDetectedIntervalMs(int preferenceId) -> int` (0 = disabled) over scattering switch logic in MidiManager; compose with unresponsive flag in `updateDevicePresenceMonitoring`.
- ONCE ON CONNECT must not disable initial Inquiry / Master-fallback discovery on connect or port-pair change.
- ONCE + unresponsive recovery cadence: EVERY 15 S for this version (same as default preference), not EVERY 5 S — avoids slamming the bus during failure storms; document in Implementation Notes if adjusted.
- Undetected 1 s retry stays under ONCE — first detection, not periodic re-proof.
- Machine-wide persistence matches ports/EPROM: presence interval is a device-connection habit, not a project creative setting.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-ControlTests` -- expected: build succeeds
- `ctest --preset macos-debug-arm64 -R 'PresenceCheck|SettingsTabsShell|MidiRequestTiming|Matrix6MasterFallback'` -- expected: mapped tests pass (adjust filter to actual names)
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++ under Source/ and Tests/

**Manual checks (if no CLI):**
- Settings > MIDI & DEVICE: row order and combo values; change EVERY 5 S → ONCE while connected and confirm MIDI bus quiets for periodic Inquiry/Master.
- Matrix-6/6R with EVERY 15 S: heartbeat still Inquiry-then-Master policy; soft-abort footer still coherent.
