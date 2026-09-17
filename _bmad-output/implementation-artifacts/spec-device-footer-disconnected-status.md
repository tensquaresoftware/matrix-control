---
title: 'DEVICE footer shows NOT CONNECTED + red badge when synth unresponsive'
type: 'bugfix'
created: '2026-09-17'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
baseline_commit: 'ccf9a8245f00be56e6e04792926475bd02cb12d0'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-device-unresponsive-presence-sysex-brake.md'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After a Matrix synth becomes unreachable while MIDI From/To stay selected (DIN unplug, power loss, wrong reachable device), the footer DEVICE band keeps showing the last model/firmware (e.g. `MATRIX-1000 (V1.20)`) on the INFO badge colour, so the right band looks still connected.

**Approach:** Keep Core soft-unresponsive behaviour (`deviceDetected` stays true; `deviceMidiUnresponsive` set). Teach the footer DEVICE band to treat unresponsive like disconnected: detail text stays `NOT CONNECTED` (existing `kDeviceNotConnectedDetail`, current grey detail colour) and DEVICE badge fill uses the same ERROR red path already used when `deviceDetected` is false. Port-loss hard clear and never-detected inquiry failure remain unchanged.

</frozen-after-approval>

## Implementation Notes

- Root cause: presence heartbeat soft-abort keeps `deviceDetected` + type/version; FooterPanel never reads `deviceMidiUnresponsive`, so DEVICE band stays green/INFO identity.
- Change surface: `FooterPanel` (+ header if needed) — mirror `Core::kDeviceMidiUnresponsiveProperty` in VT sync; `buildDeviceDetailText` / `paintDeviceStatus` treat unresponsive as not-OK for detail + badge.
- Do not change MidiManager soft-abort, left-band overload copy, or `kDeviceNotConnectedDetail` string.
- Prefer oneshot GUI fix; no new product copy.
- Implemented 2026-09-17: FooterPanel listens to `deviceMidiUnresponsive`; detail → `NOT CONNECTED`; badge uses ERROR fill when unresponsive (same path as undetected). Shared `isDeviceIdentityOk()` for badge fill vs identity detail predicates.
- Review: extracted shared OK helper; deferred historical unresponsive-spec / inventory doc drift.

## Review Triage Log

- Left vs right band contradiction (overload copy vs NOT CONNECTED) — **false**: locked Intent + Guillaume chose `NOT CONNECTED` for DEVICE detail while Core soft-unresponsive left message stays.
- Soft-unresponsive prior framing vs disconnect string reuse — **false**: same intentional product choice for DEVICE band.
- Spec still in-progress / no AC matrix — **false**: oneshot route; status set to done at finalize.
- Recovery not written in contract — **false**: VT listener on `deviceMidiUnresponsive` restores identity when flag clears; verified in FooterPanel sync path.
- Wrong reachable device only via unresponsive — **false**: previously detected + presence timeout uses soft-unresponsive; port change / never-detected paths already hard-clear.
- Duplicated unresponsive predicate in paint vs detail — **low** → **patch**: added `isDeviceIdentityOk()`.
- EditorOutboundGate include only for property name — **low** rejected: Core constant is SSOT; sibling string IDs pre-exist.
- Unresponsive presence spec still left-band-only in docs — **defer** (deferred-work.md).
- Inventories / DEVICE HELP omit new right-band state — **defer** (deferred-work.md).
