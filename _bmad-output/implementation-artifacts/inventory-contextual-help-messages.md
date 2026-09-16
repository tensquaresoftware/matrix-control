---
organization: Ten Square Software
project: Matrix-Control
title: Inventory — Contextual help messages
author: BMad Agent
status: archived
sources:
  - Desktop draft Matrix-Control-aide-contextuelle-messages.md
created: 2026-09-16
updated: 2026-09-17
---

# Matrix-Control — Aide contextuelle (brouillon de messages)

**But :** valider / corriger les textes avant implémentation.
**Format affiché :** badge `HELP` + texte blanc furtif : `MODULE: message`
**Langue UI :** anglais (ASCII seulement : tiret `-`, ellipsis `...` — pas de tiret cadratin).
**Déjà en production :** table Patch Mutator (reference, ne pas réinventer sauf ajustement volontaire).
**Préfixe Header (decide) :** `SESSION`.

Comment editer : change la colonne **Message proposé** ; laisse **Contrôle** tel quel.
Quand c'est prêt, dis-moi dans le chat Build quelles tables tu as touchées (ou renvoie le fichier).

---

## Idées de préfixe pour le Header (pas HEADER)

| Option | Exemple | Ton |
|--------|---------|-----|
| **MAIN** | `MAIN: Selects the MIDI input from the synth.` | Neutre, court, aligne sur "zone principale" |
| **GLOBAL** | `GLOBAL: ...` | Réglages transverses au plugin |
| **SESSION** | `SESSION: ...` | Ports / undo / panic = session de travail |
| **STUDIO** | `STUDIO: ...` | Un peu plus "produit", moins technique |
| **CONSOLE** | `CONSOLE: ...` | Barre de commande / cockpit |
| **BRIDGE** | `BRIDGE: ...` | Pont MIDI/audio entre hôte et Matrix |
| **PLUGIN** | `PLUGIN: ...` | Clair en hote DAW, un peu froid |
| **SHELL** | `SHELL: ...` | Coque UI autour de l'editeur |

**Decide :** **SESSION**.

---

## 1. SESSION (Header)

| Contrôle | Message proposé |
|----------|-----------------|
| MIDI FROM | SESSION: Selects the MIDI input that receives SysEx and notes from the synthesizer. |
| MIDI TO | SESSION: Selects the MIDI output that sends edits and notes to the synthesizer. |
| KEYBOARD FROM | SESSION: Selects a separate MIDI keyboard input for playing (Standalone application only). |
| HOST | SESSION: Keyboard MIDI comes from the DAW host (Plugin only). |
| AUDIO FROM | SESSION: Selects the audio input used for monitoring through the plugin. |
| INPUT GAIN | SESSION: Sets monitoring level for the selected audio input. |
| UNDO | SESSION: Undoes the last Patch or master edit in this session. |
| REDO | SESSION: Redoes the last undone edit. |
| PANIC | SESSION: Sends MIDI panic to clear stuck notes and ease a backed-up send queue. |
| LOGO | SESSION: Opens the logo menu for Settings, Audio/MIDI, About, Skin, and UI Scale. |
| SETTINGS... | SESSION: Opens plugin Settings (paths, warnings, master utility, shortcuts). |
| AUDIO/MIDI... | SESSION: Opens the host Audio/MIDI device settings (Standalone application only). |
| ABOUT... | SESSION: Shows product version, links, and credits. |
| SKIN | SESSION: Chooses the visual skin for the editor. |
| UI SCALE | SESSION: Sets the user interface scale. |
| LED (KEYBOARD FROM / HOST) | SESSION: Lights when MIDI activity arrives on Keyboard From or Host. |
| LED (MIDI FROM) | SESSION: Lights when MIDI activity arrives from the synthesizer on MIDI From. |
| LED (MIDI TO) | SESSION: Lights when MIDI activity is sent to the synthesizer on MIDI To. |
| AUDIO PEAK | SESSION: Shows the peak level of the monitored audio input. |

---

## 2. DCO 1

| Contrôle | Message proposé |
|----------|-----------------|
| I | DCO 1: Resets this module to its init values. |
| C | DCO 1: Copies this module to the clipboard. |
| P | DCO 1: Pastes compatible clipboard data into this module. |
| FREQUENCY | DCO 1: Sets DCO 1 pitch in semitones. |
| FREQ < LFO 1 | DCO 1: How much LFO 1 modulates DCO 1 frequency. |
| SYNC | DCO 1: Hard-syncs DCO 1 to DCO 2 when enabled. |
| PULSE WIDTH | DCO 1: Sets pulse width for pulse / combination waves. |
| PW < LFO 2 | DCO 1: How much LFO 2 modulates pulse width. |
| WAVE SHAPE | DCO 1: Mixes wave components for the selected wave family. |
| WAVE SELECT | DCO 1: Chooses the DCO 1 waveform family. |
| LEVERS | DCO 1: Enables lever modulation for this DCO. |
| KEYBD/PORTA | DCO 1: Tracks keyboard / portamento for this DCO. |
| KEY CLICK | DCO 1: Adds a short click on key-on for this DCO. |

---

## 3. DCO 2

| Contrôle | Message proposé |
|----------|-----------------|
| I | DCO 2: Resets this module to its init values. |
| C | DCO 2: Copies this module to the clipboard. |
| P | DCO 2: Pastes compatible clipboard data into this module. |
| FREQUENCY | DCO 2: Sets DCO 2 pitch in semitones. |
| FREQ < LFO 1 | DCO 2: How much LFO 1 modulates DCO 2 frequency. |
| DETUNE | DCO 2: Fine detune of DCO 2 relative to DCO 1. |
| PULSE WIDTH | DCO 2: Sets pulse width for pulse / combination waves. |
| PW < LFO 2 | DCO 2: How much LFO 2 modulates pulse width. |
| WAVE SHAPE | DCO 2: Mixes wave components for the selected wave family. |
| WAVE SELECT | DCO 2: Chooses the DCO 2 waveform family (includes Noise). |
| LEVERS | DCO 2: Enables lever modulation for this DCO. |
| KEYBD/PORTA | DCO 2: Tracks keyboard / portamento for this DCO. |
| KEY CLICK | DCO 2: Adds a short click on key-on for this DCO. |

---

## 4. VCF/VCA

| Contrôle | Message proposé |
|----------|-----------------|
| I | VCF/VCA: Resets this module to its init values. |
| DCO 2 \| DCO 1 MIX | VCF/VCA: Balances DCO 1 and DCO 2 into the filter stage. |
| FREQUENCY | VCF/VCA: Sets the filter cutoff frequency. |
| FREQ < ENV 1 | VCF/VCA: How much ENV 1 modulates filter cutoff frequency. |
| FREQ < PRESSURE | VCF/VCA: How much pressure modulates filter cutoff frequency. |
| RESONANCE | VCF/VCA: Sets filter resonance (emphasis at cutoff frequency). |
| VCA 1 VOLUME | VCF/VCA: Sets VCA 1 overall volume. |
| VCA 1 < VELOCITY | VCF/VCA: How much velocity modulates VCA 1. |
| VCA 2 < ENV 2 | VCF/VCA: How much ENV 2 modulates VCA 2. |
| LEVERS | VCF/VCA: Enables lever modulation for filter / amplifier. |
| KEYBD/PORTA | VCF/VCA: Tracks keyboard / portamento for the filter. |

---

## 5. FM/TRACK

| Contrôle | Message proposé |
|----------|-----------------|
| I | FM/TRACK: Resets this module to its init values. |
| VCF FM AMOUNT | FM/TRACK: Sets FM amount into the VCF from DCO 2. |
| FM < ENV 3 | FM/TRACK: How much ENV 3 modulates FM amount. |
| FM < PRESSURE | FM/TRACK: How much pressure modulates FM amount. |
| TRACK POINT 1 | FM/TRACK: Sets Track Generator point 1 level. |
| TRACK POINT 2 | FM/TRACK: Sets Track Generator point 2 level. |
| TRACK POINT 3 | FM/TRACK: Sets Track Generator point 3 level. |
| TRACK POINT 4 | FM/TRACK: Sets Track Generator point 4 level. |
| TRACK POINT 5 | FM/TRACK: Sets Track Generator point 5 level. |
| TRACK INPUT | FM/TRACK: Selects the source that drives the Track Generator. |
| Track Generator (curve) | FM/TRACK: Drag the Track Generator curve points for tracking shape. |

---

## 6. RAMP/PORTAMENTO

| Contrôle | Message proposé |
|----------|-----------------|
| I | RAMP/PORTAMENTO: Resets this module to its init values. |
| RAMP 1 RATE | RAMP/PORTAMENTO: Sets Ramp 1 rise time. |
| RAMP 1 TRIGGER | RAMP/PORTAMENTO: Chooses what starts Ramp 1. |
| RAMP 2 RATE | RAMP/PORTAMENTO: Sets Ramp 2 rise time. |
| RAMP 2 TRIGGER | RAMP/PORTAMENTO: Chooses what starts Ramp 2. |
| PORTAMENTO RATE | RAMP/PORTAMENTO: Sets glide time between notes. |
| PORTA < VELOCITY | RAMP/PORTAMENTO: How much velocity scales portamento rate. |
| PORTA MODE | RAMP/PORTAMENTO: Chooses how portamento glides between notes. |
| LEGATO PORTA | RAMP/PORTAMENTO: Enables legato-only portamento (Unison keyboard modes). |
| KEYBOARD MODE | RAMP/PORTAMENTO: Chooses poly / unison / related keyboard response. |

---

## 7. ENV 1

| Contrôle | Message proposé |
|----------|-----------------|
| I | ENV 1: Resets this module to its init values. |
| C | ENV 1: Copies this module to the clipboard. |
| P | ENV 1: Pastes compatible clipboard data into this module. |
| DELAY | ENV 1: Delay before the envelope attack starts. |
| ATTACK | ENV 1: Time to rise from zero to peak. |
| DECAY | ENV 1: Time to fall from peak to sustain. |
| SUSTAIN | ENV 1: Level held while the key is down. |
| RELEASE | ENV 1: Time to fall to zero after key-up. |
| AMPLITUDE | ENV 1: Overall envelope depth. |
| AMP < VELOCITY | ENV 1: How much velocity scales envelope amplitude. |
| TRIGGER MODE | ENV 1: Chooses how the envelope is triggered. |
| ENVELOPE MODE | ENV 1: Chooses envelope contour behaviour. |
| LFO 1 TRIGGER | ENV 1: Lets LFO 1 retrigger this envelope when enabled. |
| Envelope display | ENV 1: Drag Delay, Attack, Decay, Sustain, and Release on the curve. |

---

## 8. ENV 2

| Contrôle | Message proposé |
|----------|-----------------|
| I | ENV 2: Resets this module to its init values. |
| C | ENV 2: Copies this module to the clipboard. |
| P | ENV 2: Pastes compatible clipboard data into this module. |
| DELAY | ENV 2: Delay before the envelope attack starts. |
| ATTACK | ENV 2: Time to rise from zero to peak. |
| DECAY | ENV 2: Time to fall from peak to sustain. |
| SUSTAIN | ENV 2: Level held while the key is down. |
| RELEASE | ENV 2: Time to fall to zero after key-up. |
| AMPLITUDE | ENV 2: Overall envelope depth. |
| AMP < VELOCITY | ENV 2: How much velocity scales envelope amplitude. |
| TRIGGER MODE | ENV 2: Chooses how the envelope is triggered. |
| ENVELOPE MODE | ENV 2: Chooses envelope contour behaviour. |
| LFO 1 TRIGGER | ENV 2: Lets LFO 1 retrigger this envelope when enabled. |
| Envelope display | ENV 2: Drag Delay, Attack, Decay, Sustain, and Release on the curve. |

---

## 9. ENV 3

| Contrôle | Message proposé |
|----------|-----------------|
| I | ENV 3: Resets this module to its init values. |
| C | ENV 3: Copies this module to the clipboard. |
| P | ENV 3: Pastes compatible clipboard data into this module. |
| DELAY | ENV 3: Delay before the envelope attack starts. |
| ATTACK | ENV 3: Time to rise from zero to peak. |
| DECAY | ENV 3: Time to fall from peak to sustain. |
| SUSTAIN | ENV 3: Level held while the key is down. |
| RELEASE | ENV 3: Time to fall to zero after key-up. |
| AMPLITUDE | ENV 3: Overall envelope depth. |
| AMP < VELOCITY | ENV 3: How much velocity scales envelope amplitude. |
| TRIGGER MODE | ENV 3: Chooses how the envelope is triggered. |
| ENVELOPE MODE | ENV 3: Chooses envelope contour behaviour. |
| LFO 1 TRIGGER | ENV 3: Lets LFO 1 retrigger this envelope when enabled. |
| Envelope display | ENV 3: Drag Delay, Attack, Decay, Sustain, and Release on the curve. |

---

## 10. LFO 1

| Contrôle | Message proposé |
|----------|-----------------|
| I | LFO 1: Resets this module to its init values. |
| C | LFO 1: Copies this module to the clipboard. |
| P | LFO 1: Pastes compatible clipboard data into this module. |
| SPEED | LFO 1: Sets LFO 1 rate. |
| SPEED < PRESSURE | LFO 1: How much pressure modulates LFO 1 speed. |
| RETRIGGER POINT | LFO 1: Phase point used when the LFO retriggers. |
| AMPLITUDE | LFO 1: Sets LFO 1 depth. |
| AMP < RAMP 1 | LFO 1: How much Ramp 1 scales LFO 1 amplitude. |
| WAVEFORM | LFO 1: Chooses the LFO 1 waveform. |
| TRIGGER MODE | LFO 1: Chooses how LFO 1 starts and retriggers. |
| LAG | LFO 1: Smooths LFO 1 output changes. |
| SAMPLE INPUT | LFO 1: Chooses which modulation source the LFO samples when WAVEFORM is SAMPLED. |

---

## 11. LFO 2

| Contrôle | Message proposé |
|----------|-----------------|
| I | LFO 2: Resets this module to its init values. |
| C | LFO 2: Copies this module to the clipboard. |
| P | LFO 2: Pastes compatible clipboard data into this module. |
| SPEED | LFO 2: Sets LFO 2 rate. |
| SPEED < KEYBD | LFO 2: How much keyboard tracking modulates LFO 2 speed. |
| RETRIGGER POINT | LFO 2: Phase point used when the LFO retriggers. |
| AMPLITUDE | LFO 2: Sets LFO 2 depth. |
| AMP < RAMP 2 | LFO 2: How much Ramp 2 scales LFO 2 amplitude. |
| WAVEFORM | LFO 2: Chooses the LFO 2 waveform. |
| TRIGGER MODE | LFO 2: Chooses how LFO 2 starts and retriggers. |
| LAG | LFO 2: Smooths LFO 2 output changes. |
| SAMPLE INPUT | LFO 2: Chooses which modulation source the LFO samples when WAVEFORM is SAMPLED. |

---

## 12. PATCH NAME

| Contrôle | Message proposé |
|----------|-----------------|
| Patch name (display / edit) | PATCH NAME: Shows the 8-character patch name - double-click to rename when editable. |

---

## 13. MATRIX MODULATION

| Contrôle | Message proposé |
|----------|-----------------|
| I (section) | MATRIX MODULATION: Resets all modulation buses to init. |
| C (section) | MATRIX MODULATION: Copies all modulation buses to the clipboard. |
| P (section) | MATRIX MODULATION: Pastes compatible clipboard data into all modulation buses. |
| # (bus handle) | MATRIX MODULATION: Drag to reorder this modulation bus. |
| I (per bus) | MATRIX MODULATION: Resets this bus source, amount, and destination. |
| SOURCE | MATRIX MODULATION: Selects the modulation source for this bus. |
| AMOUNT | MATRIX MODULATION: Sets how strongly this bus modulates its destination. |
| DESTINATION | MATRIX MODULATION: Selects what this bus modulates. |

---

## 14. MIDI (Master Edit)

| Contrôle | Message proposé |
|----------|-----------------|
| I | MIDI: Resets this module to its init values. |
| CHANNEL | MIDI: Sets the synthesizer MIDI channel. |
| MIDI ECHO | MIDI: Echoes incoming MIDI back out when enabled. |
| CONTROLLERS | MIDI: Enables or disables MIDI controller reception. |
| PATCH CHANGES | MIDI: Enables or disables MIDI program changes. |
| PEDAL 1 SELECT | MIDI: Assigns the function for pedal 1. |
| PEDAL 2 SELECT | MIDI: Assigns the function for pedal 2. |
| LEVER 2 SELECT | MIDI: Assigns the function for lever 2. |
| LEVER 3 SELECT | MIDI: Assigns the function for lever 3. |

---

## 15. VIBRATO (Master Edit)

| Contrôle | Message proposé |
|----------|-----------------|
| I | VIBRATO: Resets this module to its init values. |
| SPEED | VIBRATO: Sets global vibrato rate. |
| WAVEFORM | VIBRATO: Chooses the vibrato waveform. |
| AMPLITUDE | VIBRATO: Sets global vibrato depth. |
| SPEED MOD SOURCE | VIBRATO: Selects what modulates vibrato speed. |
| SPEED MOD AMOUNT | VIBRATO: How much the speed mod source affects vibrato rate. |
| AMP MOD SOURCE | VIBRATO: Selects what modulates vibrato depth. |
| AMP MOD AMOUNT | VIBRATO: How much the amp mod source affects vibrato depth. |

---

## 16. MISC (Master Edit)

| Contrôle | Message proposé |
|----------|-----------------|
| I | MISC: Resets this module to its init values. |
| MASTER TUNE | MISC: Fine-tunes overall instrument pitch. |
| MASTER TRANSPOSE | MISC: Transposes the instrument in semitones. |
| BEND RANGE (+/-) | MISC: Sets pitch-bend range in semitones up and down. |
| UNISON | MISC: Enables Master Unison (can override Patch keyboard mode). |
| VOLUME INVERT | MISC: Inverts volume pedal response when enabled. |
| BANK LOCK | MISC: Locks bank changes on the hardware. |
| MEMORY PROTECT | MISC: Protects synth memory from being overwritten. |

---

## 17. BANK UTILITY

| Contrôle | Message proposé |
|----------|-----------------|
| 0 | BANK UTILITY: Selects Matrix-1000 bank 0 (RAM) and sets it as the copy/paste/import/export target. |
| 1 | BANK UTILITY: Selects Matrix-1000 bank 1 (RAM) and sets it as the copy/paste/import/export target. |
| 2 | BANK UTILITY: Selects Matrix-1000 bank 2 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1). |
| 3 | BANK UTILITY: Selects Matrix-1000 bank 3 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1). |
| 4 | BANK UTILITY: Selects Matrix-1000 bank 4 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1). |
| 5 | BANK UTILITY: Selects Matrix-1000 bank 5 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1). |
| 6 | BANK UTILITY: Selects Matrix-1000 bank 6 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1). |
| 7 | BANK UTILITY: Selects Matrix-1000 bank 7 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1). |
| 8 | BANK UTILITY: Selects Matrix-1000 bank 8 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1). |
| 9 | BANK UTILITY: Selects Matrix-1000 bank 9 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1). |
| COPY | BANK UTILITY: Copies the selected bank from the synthesizer to the clipboard. |
| PASTE | BANK UTILITY: Pastes the clipboard bank into the selected destination bank. |
| IMPORT | BANK UTILITY: Imports patch files into the selected RAM bank. |
| EXPORT | BANK UTILITY: Exports the selected bank as SysEx files on disk. |

---

## 18. INTERNAL PATCHES

| Contrôle | Message proposé |
|----------|-----------------|
| Title (module header) | INTERNAL PATCHES: Reloads / focuses the internal patch browser for the connected synthesizer. |
| BROWSER | INTERNAL PATCHES: Shows the internal patch browser list. |
| MEMORY | INTERNAL PATCHES: Shows memory-oriented patch controls. |
| < | INTERNAL PATCHES: Loads the previous patch in the current bank. |
| > | INTERNAL PATCHES: Loads the next patch in the current bank. |
| CURRENT BANK | INTERNAL PATCHES: Selects the current synthesizer bank. |
| CURRENT PATCH | INTERNAL PATCHES: Selects the current patch number in the bank. |
| INIT | INTERNAL PATCHES: Loads the init patch template into the editor and sends it to the synthesizer. |
| COPY | INTERNAL PATCHES: Copies the current patch to the clipboard. |
| PASTE | INTERNAL PATCHES: Pastes a clipboard patch into the editor and sends it to the synthesizer. |
| STORE | INTERNAL PATCHES: Stores the current patch into the selected synthesizer memory slot. |

---

## 19. COMPUTER PATCHES

| Contrôle | Message proposé |
|----------|-----------------|
| Title (module header) | COMPUTER PATCHES: Reloads / focuses computer .syx patch browsing on disk. |
| BROWSER | COMPUTER PATCHES: Shows the computer patch browser list. |
| STORAGE | COMPUTER PATCHES: Shows save / storage controls for .syx files. |
| < | COMPUTER PATCHES: Loads the previous patch file in the folder. |
| > | COMPUTER PATCHES: Loads the next patch file in the folder. |
| SELECT A PATCH | COMPUTER PATCHES: Chooses which .syx patch file is selected. |
| OPEN | COMPUTER PATCHES: Opens a folder of .syx patch files. |
| SAVE AS | COMPUTER PATCHES: Saves the current patch as a new .syx file. |
| SAVE | COMPUTER PATCHES: Saves over the current .syx file. |

---

## 20. PATCH MUTATOR (déjà en production — reference)

| Contrôle | Message actuel |
|----------|----------------|
| MODE | PATCH MUTATOR: Sets how far mutations stray - Kindred, Drift, Warp, or Wild. |
| PITCH | PATCH MUTATOR: Controls how DCO pitch may move - Keep, Consonant, Dissonant, or Free. |
| HISTORY | PATCH MUTATOR: Recalls a mutation or retry from this session. |
| MUTATE | PATCH MUTATOR: Creates a new variation from the current recipe and sends it to the synthesizer. |
| RETRY | PATCH MUTATOR: Rolls again from the same mutation root. |
| < | PATCH MUTATOR: Steps backward through session history. |
| > | PATCH MUTATOR: Steps forward through session history. |
| C | PATCH MUTATOR: Compares with the origin patch and locks editing until you click [C] button again. |
| D | PATCH MUTATOR: Deletes the selected history entry. |
| F | PATCH MUTATOR: Flushes the whole session mutation history. |
| E | PATCH MUTATOR: Exports the session mutations as SysEx files. |
| D1 | PATCH MUTATOR: Include DCO 1 module in the mutation recipe. |
| D2 | PATCH MUTATOR: Include DCO 2 module in the mutation recipe. |
| F/A | PATCH MUTATOR: Include VCF/VCA module in the recipe. |
| F/T | PATCH MUTATOR: Include FM/TRACK module in the recipe. |
| R/P | PATCH MUTATOR: Include RAMP/PORTAMENTO module in the recipe. |
| E1 | PATCH MUTATOR: Include ENV 1 module in the recipe. |
| E2 | PATCH MUTATOR: Include ENV 2 module in the recipe. |
| E3 | PATCH MUTATOR: Include ENV 3 module in the recipe. |
| L1 | PATCH MUTATOR: Include LFO 1 module in the recipe. |
| L2 | PATCH MUTATOR: Include LFO 2 module in the recipe. |
| MM | PATCH MUTATOR: Include MATRIX MODULATION module in the recipe. |

---

## 21. SETTINGS (ajoute apres figement Bureau — decide in scope)

| Controle | Message propose |
|----------|-----------------|
| MATRIX-1000 PATCHES | SETTINGS: Chooses how Matrix-1000 patch names are displayed. |
| COMPUTER PATCHES | SETTINGS: Chooses how computer .syx patch names are displayed. |
| UNSAVED STATE | SETTINGS: Chooses when to warn about unsaved patch changes. |
| SAVE AS INIT (patch) | SETTINGS: Saves the current patch as the patch init template. |
| DELETE (patch init) | SETTINGS: Deletes the saved patch init template. |
| DELETE WARNING | SETTINGS: Chooses when Patch Mutator delete asks for confirmation. |
| DEFRAG HISTORY | SETTINGS: Coming soon - renumber mutation history when full. |
| HARDWARE LATENCY | SETTINGS: Sets inter-SysEx delay for reliable hardware transfers (plugin). |
| LOAD (master) | SETTINGS: Loads a Master settings file into the editor. |
| SAVE AS (master) | SETTINGS: Saves current Master settings as a new file. |
| INIT (master) | SETTINGS: Resets Master settings to the master init template. |
| SAVE AS INIT (master) | SETTINGS: Saves current Master settings as the master init template. |
| DELETE (master init) | SETTINGS: Deletes the saved master init template. |
| SETTINGS shortcut row | SETTINGS: Shows the keyboard shortcut that opens Settings. |
| AUDIO/MIDI shortcut row | SETTINGS: Shows the keyboard shortcut that opens Audio/MIDI. |
| UI SCALE shortcut row | SETTINGS: Shows the keyboard shortcut cycle for UI Scale. |
| SKIN shortcut row | SETTINGS: Shows the keyboard shortcut cycle for Skin. |

---

## 22. ABOUT (ajoute apres figement Bureau — decide in scope)

| Controle | Message propose |
|----------|-----------------|
| Email link | ABOUT: Opens email to Ten Square Software. |
| GitHub link | ABOUT: Opens the Matrix-Control GitHub repository. |
| LinkedIn link | ABOUT: Opens the author LinkedIn profile. |

---

## Hors perimetre (cette vague)

_(vide — LEDs / peak Header et DEVICE footer ajoutes 2026-09-16)_

---

## 23. DEVICE (footer, bande droite)

| Controle | Message propose |
|----------|-----------------|
| DEVICE (+ modele + version EPROM) | DEVICE: Shows the connected synthesizer model and EPROM version, or connection status. |

