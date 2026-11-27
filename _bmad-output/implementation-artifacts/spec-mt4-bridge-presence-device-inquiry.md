---
organization: Ten Square Software
project: Matrix-Control
title: Spec — MT4 Bridge presence Device Inquiry capture
author: BMad Agent
type: bugfix
created: '2026-08-06'
status: done
baseline_commit: e2126ad
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-device-unresponsive-presence-sysex-brake.md'
  - '{project-root}/Documentation/Devs/windows-midi-multi-client.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** On the Windows MT4/Bridge path (`MT4 Port 1` teVirtualMIDI merged IN+OUT), Matrix-Control often flips to `deviceMidiUnresponsive` / footer ERROR after a quiet presence heartbeat (~5 s + ~2 s timeout), while MIDI-OX Input Monitor on the same virtual IN keeps seeing valid Matrix-1000 Device Inquiry identity replies. The same Matrix-Control + Matrix-1000 stay stable via Scarlett 6i6 (distinct IN/OUT endpoints). Bridge USB timeout tweaks did not stop the flap.

**Approach:** Isolate whether Matrix-Control’s Device Inquiry one-shot capture / re-arm drops replies that already reach the wire (especially when a bidirectional teVirtualMIDI endpoint echoes the outbound inquiry back on the same port’s input). Apply a minimal Matrix-Control fix if proven; otherwise document mitigation and a clear Bridge/VirtualMIDI follow-up for the Emagic community without implementing that neighbor repo here unless agreed. Renaming display strings alone is not a fix.

## Boundaries & Constraints

**Always:**
- Keep soft-unresponsive semantics: presence timeout while already detected must not clear `deviceDetected` or switch to cable-check copy.
- Prefer fixing capture / pairing / listen readiness over changing global presence cadence or default SysEx timeout unless evidence shows those are the root cause.
- Confirm the lab ERROR is Unresponsive (not OutboundBusy quiet deferral).
- Document a minimal repro (Bridge + `MT4 Port 1` From/To + Ableton closed; MIDI-OX IN-only OK; no MidiView).
- If root cause is multi-client / merged VirtualMIDI fan-out outside Matrix-Control, write an explicit deferred note for `unitor-win64-driver` and stop expanding Matrix-Control.

**Ask First:**
- Raising `kDefaultTimeoutMs` / presence heartbeat intervals as the primary “fix”.
- Broad MidiReceiver redesign beyond Device Inquiry one-shot behavior.
- Implementing Bridge or teVirtualMIDI port-model changes in the neighbor driver repo.
- Blocking this Matrix-Control fix until Bridge renames ports to `MT4 Input N` / `MT4 Output N` (default: do **not** block — fix capture here first; Bridge port-model follow-up separately).

**Never:**
- Treating Bridge WinUSB session timeouts as the primary Matrix-Control fix after lab evidence that identity replies already land on VirtualMIDI IN.
- Treating display-name renames alone (`MT4 Input 1` / `MT4 Output 1` strings) as the presence fix without separate virtual port objects and/or a capture fix.
- Expanding into unrelated SysEx / patch / UI work.
- Opening MidiView on the lab machine (known BSOD).
- Committing unless Guillaume asks.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Merged-port presence heartbeat | Detected; From/To = same virtual name; inquiry echo and/or identity reply arrive | Capture accepts Device ID; unresponsive stays/clears false; no ERROR flap at quiet ~5 s cadence | Non-ID SysEx must not permanently steal the armed capture before timeout |
| Identity reply on wire | `F0 7E 00 06 02 10 06 00 02 00 20 31 32 30 F7` reaches MC MidiIn while inquiry armed | Success path; footer Normal | Parser already accepts this frame if delivered |
| Distinct IN/OUT (Scarlett) | Same presence cadence | Remains stable (no regression) | Unchanged |
| True silence after send | No Device ID within timeout while detected | Soft unresponsive + ERROR guidance (honest) | Presence retries |
| Outbound queue busy before send | Idle poll times out | Soft OutboundBusy abort; no Unresponsive ERROR | Presence retries |

</frozen-after-approval>

## Code Map

- `Source/Core/MIDI/MidiManagerDeviceInquiry.cpp` — presence tick, arm-before-send, async handle/re-arm, soft Unresponsive vs OutboundBusy, timeout
- `Source/Core/MIDI/Transport/MidiReceiver.cpp` — optional one-shot filter; reject keeps capture armed
- `Source/Core/MIDI/DeviceInquiryCaptureFilter.h` — Device ID reply predicate (rejects `06 01` echo)
- `Tests/Unit/DeviceInquiryCaptureFilterTests.cpp` — echo-then-identity + unfiltered first-wins

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/MIDI/Transport/MidiReceiver.cpp` (+ header if needed) -- Make Device Inquiry capture ignore non–Device-ID frames without clearing the armed slot (or equivalent filter so inquiry echo `06 01` / unrelated SysEx cannot steal the one-shot before a Device ID arrives or timeout/cancel) -- closes merged-port loopback race
- [x] `Source/Core/MIDI/MidiManagerDeviceInquiry.cpp` -- Align arm/send/timeout with the filtered capture; keep Unresponsive vs OutboundBusy distinct; add brief diagnostic logs for echo-vs-reply when useful -- proves path and avoids quiet false ERROR
- [x] `Tests/Unit/` (new or extended) -- Cover “armed + non-ID SysEx then Device ID succeeds” and “timeout still soft-aborts when detected” without requiring hardware -- locks the race
- [x] Lab note in this spec Verification / Design Notes -- Minimal Bridge repro + success criterion (quiet 2–5 min + light edit, patch +/− OK, no recurring presence ERROR) -- handoff for Guillaume’s Boot Camp PC
- [x] If Matrix-Control fix insufficient -- Append deferred-work entry pointing at Bridge/VirtualMIDI multi-client or merged-port model for `unitor-win64-driver` -- community Emagic follow-up without scope creep here (also queued as planned Bridge follow-up regardless of lab outcome)

**Acceptance Criteria:**
- Given detected presence on merged virtual From/To, when a non–Device-ID SysEx (including inquiry echo) arrives while capture is armed, then the next valid Device ID still completes success within the same inquiry attempt (no false Unresponsive from that steal).
- Given a valid M-1000 identity frame delivered to MidiReceiver while inquiry is armed, when parse runs, then `deviceMidiUnresponsive` clears/stays false and footer leaves ERROR presence guidance owned by that path.
- Given true silence after an armed send while already detected, when 2 s elapses, then soft Unresponsive still occurs (honest failure preserved).
- Given Scarlett-style distinct IN/OUT usage, when presence heartbeats run, then behavior remains stable (no new false Unresponsive).

## Spec Change Log

## Design Notes

**Not a display-name bug.** Matrix-Control already opens MIDI From and MIDI To by separate OS identifiers. Matching UI labels (`MT4 Port 1` / `MT4 Port 1`) do not merge those handles. Renaming labels only (e.g. `MT4 Input 1` vs `MT4 Output 1`) while keeping one bidirectional teVirtualMIDI endpoint would not remove the echo path.

**Scarlett vs MT4 (plain model).** Scarlett exposes distinct hardware endpoints: what leaves the PC on OUT does not reappear on that card’s IN. The Bridge’s current teVirtualMIDI “merged” port is one bidirectional virtual endpoint: traffic sent to its OUT can also appear on its IN (echo of our own Device Inquiry request), in addition to real synth replies. That is port architecture / teVirtualMIDI mode, not string naming. Creating truly separate virtual devices (dedicated input port object vs dedicated output port object, wired by the Bridge to DIN) would resemble Scarlett and is a useful Emagic-community Bridge option — still separate from the Matrix-Control capture bug.

**Highest-probability Matrix-Control root cause:** `deliverOneShotCapture` moves/clears the callback on the **first** complete SysEx. Non–Device-ID handling re-arms only after `MessageManager::callAsync` on the message thread. On a merged bidirectional virtual port, the outbound Universal Device Inquiry can echo as SysEx; that echo consumes the one-shot; the real identity reply can arrive before re-arm and be dropped — MIDI-OX still shows the reply. Parser would accept the lab 15-byte identity frame if it reached `handleAsyncDeviceInquiryResponse`. The same fragility can bite whenever any non–Device-ID SysEx arrives first (not only echo).

**Preferred sequencing:** Fix Matrix-Control Device Inquiry capture in this Quick Dev first; rework Bridge virtual port model (`MT4 Input N` / `MT4 Output N` as real separate endpoints, not rename-only) afterward in `unitor-win64-driver`. Do not wait on Bridge to harden presence — the one-shot steal is a Matrix-Control defect on any noisy/echoing wire.

**Neighbor signal (do not implement here by default):** (1) Bridge should prefer teVirtualMIDI’s non-loopback / private-side model (distinct public IN vs OUT endpoints, or one public face + private Bridge face) over merged bidirectional ports chosen mainly to dodge identical-name collisions — renaming alone is insufficient; (2) if after the Matrix-Control capture fix the flap remains only when a second WinMM client shares the merged IN, document multi-client limits; (3) vendor/support risk for teVirtualMIDI (proprietary SDK, author responsiveness) is a Bridge-program contingency topic, not a reason to abandon this Matrix-Control fix.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- build OK
- Run focused unit tests for the new/extended capture cases -- all pass
- `python3 Scripts/quality/lint_touched.py` -- clean on touched C++

**Manual checks (Windows Boot Camp lab):**
- Bridge `--start-session --dev-zadig`; Matrix-Control MIDI From/To = `MT4 Port 1`; Ableton closed; optional MIDI-OX IN-only monitor
- Quiet 2–5 minutes then light edit + patch +/− — no recurring presence ERROR; identity replies that MIDI-OX sees must also clear Unresponsive in Matrix-Control when the fix applies

## Suggested Review Order

**Identity filter**

- Matrix-family Device ID reply only; rejects inquiry echo and incomplete frames.
  [`DeviceInquiryCaptureFilter.h:11`](../../Source/Core/MIDI/DeviceInquiryCaptureFilter.h#L11)

- Wire the filter into Device Inquiry one-shot arm.
  [`MidiManagerDeviceInquiry.cpp:167`](../../Source/Core/MIDI/MidiManagerDeviceInquiry.cpp#L167)

**One-shot keep-armed**

- Evaluate filter outside the mutex; reject leaves capture armed.
  [`MidiReceiver.cpp:193`](../../Source/Core/MIDI/Transport/MidiReceiver.cpp#L193)

- Optional filter API; unfiltered path unchanged for patch dumps.
  [`MidiReceiver.h:37`](../../Source/Core/MIDI/Transport/MidiReceiver.h#L37)

**Tests**

- Echo then identity; missing F7; foreign manufacturer; first-wins without filter.
  [`DeviceInquiryCaptureFilterTests.cpp:1`](../../Tests/Unit/DeviceInquiryCaptureFilterTests.cpp#L1)
