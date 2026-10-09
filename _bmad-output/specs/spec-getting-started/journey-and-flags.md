# Journey, buttons, flags, and format applicability

Companion to `SPEC-getting-started`. Product SSOT absorbed from the 2026-10-08 decisions plan.

## Step matrix

| ID | Title band | Body controls | Applicability |
|----|------------|---------------|---------------|
| 0 | `GETTING STARTED` | Intro copy only — no controls | All formats — first contact / Run Setup Again |
| 1 | `GETTING STARTED | STEP 1: USER INTERFACE` | UI Scale, Skin | All |
| 2 | `GETTING STARTED | STEP 2: SYNTH COMMUNICATION` | Synth From, Synth To, DEVICE, EPROM (+ firmware suggestion suffix when relevant) | All |
| 3 | `GETTING STARTED | STEP 3: MIDI KEYBOARD` | See below | All (content differs) |
| 4 | `GETTING STARTED | STEP 4: AUDIO` | Same controls as Settings → AUDIO (see below) | **Standalone only** |

Body help copy: `ui-copy.md`.

### STEP 3 — MIDI KEYBOARD

| Format | Content | Buttons |
|--------|---------|---------|
| **Standalone** | Keyboard From combo + Skip + Standalone copy | PREVIOUS · SKIP · NEXT |
| **Plugin** | Informative only — no Keyboard From combo; notes come from host/DAW; point to user manual | PREVIOUS · NEXT or FINISH (no Skip) |

`KEYBOARD FROM` is Standalone-only. Plugin treats host MIDI as the keyboard path.

### STEP 4 — AUDIO (Standalone)

Show the **same controls as Settings → AUDIO**, in the same order, including blank spacer rows.
The wizard dialog may grow taller than the Settings modal for this step.

Order (matches Settings → AUDIO):

1. Driver / type
2. Input device
3. Output device
4. Sample rate
5. Buffer size
6. *(blank spacer)*
7. Input channels
8. SYNTH FROM (+ peak indicator)
9. *(blank spacer)*
10. Output channels
11. PLAY TEST TONE

## Buttons

Forbidden: `QUIT`, `SPECIFY LATER`, dedicated `CONFIRM` on STEP 2, second “full setup” button.

| Step | Buttons | Notes |
|------|---------|-------|
| 0 Intro | `CONFIGURE LATER` · `CONTINUE` | See Configure later policy |
| 1 User Interface | `PREVIOUS` · `NEXT` | Previous → intro |
| 2 Synth Communication | `PREVIOUS` · `NEXT` | Combos **live** (like Settings); Next marks step done |
| 3 Keyboard Standalone | `PREVIOUS` · `SKIP` · `NEXT` | Skip = step complete without requiring a port |
| 3 Keyboard plugin | `PREVIOUS` · `NEXT` or `FINISH` | No Skip; Next/Finish marks Keyboard done |
| 4 Audio | `PREVIOUS` · `FINISH` | Last Standalone step |

Last applicable step: plugin = STEP 3 → `FINISH`; Standalone = STEP 4 → `FINISH`.

## Per-step flags

Track durable completion separately for:

1. User Interface
2. Synth Communication
3. MIDI Keyboard
4. Audio

Rules:

- Skip Keyboard (Standalone) = Keyboard **done** for auto-open.
- Plugin STEP 3 Next/Finish = Keyboard **done**.
- Audio flag is only meaningful when Audio is applicable (Standalone).
- **Never** use one shared “setup done” boolean for both plugin and Standalone.
- **Never** merge these flags with `sceneAudioSafetyDefaultsApplied` (audio-safety first-run Input None).

### Auto-open

When Settings combo = `SHOW WHEN INCOMPLETE` and at least one **applicable** step is incomplete:

- Open wizard at the **first** applicable incomplete step.
- Do **not** replay intro unless first contact / user never Continued (intro is for first contact and `RUN SETUP AGAIN`).
- Plugin with steps 1–3 done and Audio N/A → no auto-open loop.
- Later first Standalone use with Audio applicable + incomplete → targeted open at STEP 4 (resume copy in `ui-copy.md`).
- All applicable done → no auto-open.

When combo = `NEVER SHOW AT LAUNCH`: never auto-open; entry only via Settings / `RUN SETUP AGAIN`.

### `RUN SETUP AGAIN`

- Reset flags for steps **applicable to the current format**.
- Open wizard at **step 0**.
- Auto-open path remains resume-only (no reset).

### Configure later (step 0)

1. Close modal.
2. Do **not** mark unvisited steps done.
3. Allow **one** auto reminder on next launch (**same format only**) while applicable incomplete work remains. While that reminder is armed, the **other** format does **not** auto-open.
4. Consuming the reminder (auto-open on that format) or a second Configure later → silence until `RUN SETUP AGAIN` or combo set back to `SHOW WHEN INCOMPLETE`.
5. **Exception:** a newly applicable incomplete step (e.g. first Standalone, only Audio left) **rearms** one targeted open.

## Settings — tabs and controls

**GETTING STARTED** (first tab) — SETUP WIZARD row: combo then `RUN SETUP AGAIN` below (no second label beside the button).

**USER INTERFACE** order:

1. UI SCALE  
2. SKIN  
3. INFO MESSAGE  
4. CONTEXTUAL HELP  

**MIDI & DEVICE** — SYNTH FROM / SYNTH TO, DEVICE (read-only), EPROM TYPE, HARDWARE LATENCY (plugin only).

Logo menu keeps Scale/Skin shortcuts.

## Distinct first-run: audio safety

Standalone cold-start may still force Audio Input = None via `sceneAudioSafetyDefaultsApplied`. That gate is **orthogonal** to wizard flags: completing or skipping GETTING STARTED Audio must not set or clear the audio-safety flag; audio-safety must not satisfy wizard Audio completion.
