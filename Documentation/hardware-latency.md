# Hardware Latency

Matrix-Control reports **Hardware Latency** to your DAW so plugin delay compensation (PDC) can align MIDI-triggered synth audio on a separate audio track with the rest of your mix.

## What it measures

Hardware Latency represents the **analog round-trip delay**:

**DAW → MIDI out → Matrix-1000 → audio out → interface ADC → DAW audio track**

This is **not** the plugin’s internal buffer latency. The hosted plugin has no audio input bus (see AD-11); synth audio is monitored on a **separate DAW audio track**. The control tells the host how far ahead to shift the instrument track so recorded audio lines up.

## Where to set it

Open **Settings** from the header (**SETTINGS** button) and adjust **HARDWARE LATENCY :** (0.0–100.0 ms, 0.1 ms steps). The value is saved in the plugin session state and restored with your project.

## Typical starting values

These are ballpark figures — measure and tune for your rig (interface buffer size, cable length, synth path).

| Setup | Typical range (ms) | Notes |
|-------|-------------------|-------|
| USB audio interface + Matrix-1000 (direct analog out → interface in) | 3–8 | Low-latency USB drivers, short cable run |
| Class-compliant USB interface, moderate buffer (256–512 samples) | 8–15 | Add interface I/O latency from driver panel |
| Thunderbolt / PCIe interface, low buffer | 2–6 | Often tighter than budget USB |
| Matrix-1000 via mixer + extra AD/DA stage | 10–25 | Each analog stage adds delay |
| High buffer / heavy PDC project | 15–40 | Use as offset on top of measured round-trip |

## Workflow tips

1. Create an **instrument track** with Matrix-Control (MIDI only).
2. Create an **audio track** monitoring the interface input wired to the Matrix-1000 output.
3. Set Hardware Latency on the instrument track plugin instance.
4. Enable PDC in your DAW and record a sharp transient (click or short note); nudge Hardware Latency until the audio track aligns with the MIDI grid.

## Standalone

The same control is available in standalone mode. If your host supports latency reporting for standalone apps, the value is applied the same way. Primary use case remains DAW recording with a separate synth return track.
