# Windows MIDI Multi-Client Setup

Matrix-Control on Windows uses JUCE MIDI I/O, which targets the legacy **WinMM** API. WinMM MIDI ports are typically **single-client**: when one application (often your DAW) opens a hardware MIDI port, other applications cannot open the same port until it is released.

If you see a footer error such as:

> MIDI To: "Your Interface" — could not open (port may be in use by another application, e.g. your DAW…)

that usually means a **port conflict**, not a broken cable or missing driver.

## Why this happens

| Platform | Typical behaviour |
|----------|-------------------|
| **Windows (WinMM)** | One app per hardware MIDI port at a time |
| **macOS / Linux** | Multiple apps can often share the same port |

Ableton Live, Cubase, and most Windows DAWs open MIDI hardware exclusively. Matrix-Control **MIDI From** and **MIDI To** in the header need their own open handles — the same limitation applies in **standalone** and when the **hosted plugin** uses host-routed ports that map back to one physical interface.

**Future note:** Microsoft is developing **Windows MIDI Services** with native multi-client support. Until DAWs and JUCE adopt it widely, assume WinMM rules above.

## Quick checks

1. Close any app that might hold the port (DAW, utility, old standalone instance).
2. Retry **MIDI From** / **MIDI To** in the Matrix-Control header.
3. Unplug/replug the USB MIDI interface if the port vanished from the list.

## Recommended: loopMIDI (virtual ports)

**loopMIDI** creates virtual MIDI loopback ports that multiple applications can use at the same time. Matrix-Control recommends **loopMIDI 1.0.16 or newer**:

- Download: [Tobias Erichsen — loopMIDI](http://www.tobias-erichsen.de/software/loopmidi.html)

### Typical routing

```
Hardware MIDI interface
        │
        ▼
   (optional: MIDI-OX or DAW internal routing — fan-out to virtual ports)
        │
        ├── loopMIDI port "Matrix To Synth"  ──► Matrix-Control MIDI To
        ├── loopMIDI port "Matrix From Synth" ◄── Matrix-Control MIDI From (SysEx reply)
        └── loopMIDI port "DAW Out"          ──► Ableton MIDI track output
```

### Setup steps

1. Install loopMIDI **1.0.16+** and launch it (runs in the system tray).
2. Click **+** to add virtual ports with clear names (e.g. `Matrix To Synth`, `Matrix From Synth`, `DAW Matrix Out`).
3. Enable **autostart** from the tray icon if you want ports after reboot (loopMIDI must be running for ports to exist).
4. Route your **hardware MIDI output** into the virtual ports your DAW and Matrix-Control will use:
   - **Hardware → synth:** use a port wired to your Matrix-1000 MIDI IN.
   - **Hardware → Matrix-Control MIDI From:** use a port carrying synth MIDI OUT (Device Inquiry / inbound SysEx).
5. In **Matrix-Control**, set **MIDI To** and **MIDI From** to the matching loopMIDI ports (not the raw hardware names).
6. In **Ableton Live 12**, set the instrument track's MIDI output to its dedicated loopMIDI port — not the same exclusive hardware port Matrix-Control would fight over.

### loopMIDI tips

- Virtual ports disappear when loopMIDI is quit — keep it running during sessions.
- If SysEx behaves oddly after a Windows restart, reinstall loopMIDI (known issue on some setups); 1.0.16+ is the baseline version we test against.
- You may need **MIDI-OX** or your interface's control panel to duplicate one hardware input to several virtual outputs if the interface exposes only one WinMM port.

## Alternative: rtpMIDI (network MIDI)

**rtpMIDI** (Apple RTP-MIDI session for Windows) exposes network MIDI ports. Useful when:

- Machines or apps on the same LAN should share a session, or
- You already use Wi‑Fi/Ethernet MIDI and accept network latency.

rtpMIDI does **not** replace local USB/DIN routing by itself — pair it with loopMIDI or direct routing as your rig requires. See your rtpMIDI driver documentation for session setup.

## Matrix-Control port roles

| Header control | Role |
|----------------|------|
| **MIDI To** | Outbound to synth (notes, CC, SysEx editor path) |
| **MIDI From** | Inbound from synth (Device Inquiry, firmware detection) |

In **hosted plugin** mode, the DAW supplies MIDI routing; use virtual ports in the host so Matrix-Control and the DAW never contend for one WinMM hardware handle.

## Ableton Live 12 workflow (summary)

1. **Instrument track** — Matrix-Control plugin; MIDI routed via loopMIDI virtual ports.
2. **Audio track** — monitors Matrix-1000 audio return (see [hardware-latency.md](hardware-latency.md) for PDC).
3. Avoid assigning both Live and Matrix-Control standalone to the **same hardware MIDI port name** without a virtual split.

## Related

- [Hardware Latency](hardware-latency.md) — analog round-trip delay for DAW compensation (AD-11 two-track workflow).
- Footer port errors — implemented in Story R-3; this guide is linked from the Windows open-failure message.
