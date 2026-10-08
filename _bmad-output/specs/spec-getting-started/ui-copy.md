# Frozen English UI copy (GETTING STARTED)

Absorbed from decisions plan §6. Do not invent alternate wording in Build without updating this companion and the product SSOT together.

Button / title labels (ASCII):

- Titles: `GETTING STARTED`; `GETTING STARTED — STEP 1 : USER INTERFACE`; `GETTING STARTED — STEP 2 : SYNTH COMMUNICATION`; `GETTING STARTED — STEP 3 : MIDI KEYBOARD`; `GETTING STARTED — STEP 4 : AUDIO`
- Buttons: `CONFIGURE LATER`, `CONTINUE`, `PREVIOUS`, `NEXT`, `SKIP`, `FINISH`
- Settings: `GETTING STARTED`, `SHOW WHEN INCOMPLETE`, `NEVER AT LAUNCH`, `RUN SETUP AGAIN`
- Settings order labels: `UI SCALE`, `SKIN`, `INFO MESSAGE`, `CONTEXTUAL HELP` (existing)

## Étape 0 — GETTING STARTED

> Welcome to Matrix-Control, a modern SysEx editor for the Oberheim Matrix-1000, 6, and 6R synthesizers.  
> We'll set appearance, MIDI connection, optional keyboard input, and audio monitoring (standalone application only) so you can edit, play, and hear your synth. Continue, or choose Configure later and finish in Settings.

## STEP 1 — USER INTERFACE

> Start with UI scale and skin so the next steps stay readable on your screen. You can change these anytime from the logo menu or Settings.

## STEP 2 — SYNTH COMMUNICATION

> Select the MIDI ports wired to your synth and the EPROM type installed in it. Wait until the device is recognized when possible — this unlocks reliable editing and timing.

Firmware suggestion suffix (when relevant):

> A suggestion is preselected from the reported firmware version when possible.

## STEP 3 — MIDI KEYBOARD

**Standalone:**

> If you use a separate MIDI keyboard, choose it here. Matrix-6 owners who play the built-in keys can skip this step.

**Plugin:**

> When Matrix-Control runs as a plugin, MIDI notes come from the host. Route your master keyboard on a DAW track (or MIDI input) to Matrix-Control — not in this window. See the user manual for host examples.

## STEP 4 — AUDIO

**First Standalone pass:**

> Choose the audio interface and input so you can hear your synth in Matrix-Control. Pick the SYNTH FROM channel(s) that carry the synth output.

**Resume (e.g. after plugin path already done):**

> MIDI is already set. One more step: route audio so the standalone application can monitor your synth.
