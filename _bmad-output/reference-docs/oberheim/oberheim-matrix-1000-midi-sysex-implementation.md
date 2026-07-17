---
organization: Ten Square Software
project: Matrix-Control
title: Oberheim Matrix-1000
author: Guillaume DUPONT
created: 2025-10-25
updated: 2026-07-17
notes: |
  2026-07-17: Re-verified MIDI Summary / SysEx pages against official Matrix-1000 Owner's Manual PDF
  (scanned pages ~41–56). Firm fixes: Omni Mode On data byte 00H; Active Sensing status FEH;
  restore literal header byte 0 after opcode 0DH; remove length note wrongly attached under 0EH;
  restore Global Parameter bytes 166–168 (incl. Group Mode Enable); Device ID rev-3 ASCII 30H ('0');
  Table 3 unused destination code 0; minor typos (order / received); 0BH wording aligned to manual
  (path / val / dest). Ambiguities retained as printed — see ## Verification notes.
---

# Oberheim Matrix-1000

## MIDI & SysEx Implementation

---

## Channel Voice Messages

| Status    | Data Bytes             | Description                                 |
| --------- | ---------------------- | ------------------------------------------- |
| 1000 xxxx | 0nnn nnnn<br>0vvv vvvv | Note Off<br>(Release Velocity = 1–127)      |
| 1001 xxxx | 0nnn nnnn<br>0vvv vvvv | Note On<br>(Velocity = 1–127, 0 = Note Off) |
| 1011 xxxx | 0ccc cccc<br>0nnn nnnn | Controller Change<br>(0–121, 7-bit only)    |
| 1100 xxxx | 0nnn nnnn              | Program Select                              |
| 1110 xxxx | 0000 000n<br>0nnn nnnn | Pitch Bend (LSB)<br>Pitch Bend (MSB)        |

## Channel Mode Messages

| Status    | Data Bytes | Description                                   |
| --------- | ---------- | --------------------------------------------- |
| 1011 xxxx | 79H<br>00H | Reset All Controllers                         |
| 1011 xxxx | 7BH<br>00H | All Notes Off                                 |
| 1011 xxxx | 7CH<br>00H | Omni Mode Off                                 |
| 1011 xxxx | 7DH<br>00H | Omni Mode On<br>(Omni assumed Off in Mono On) |
| 1011 xxxx | 7EH<br>06H | Mono Mode On                                  |
| 1011 xxxx | 7FH<br>00H | Mono Mode Off                                 |

## System Common Messages

| Status    | Data Bytes    | Description                    |
| --------- | ------------- | ------------------------------ |
| 1111 0000 | F0H           | Start of System Exclusive      |
|           | 10H           | System Exclusive – Oberheim ID |
|           | 06H           | Matrix-6/6R/1000 Device ID     |
|           | \<opcode>     |                                |
|           | \<data bytes> |                                |
| 1111 0111 | F7H           | End of system exclusive        |

## System Real Time Messages

| Status              | Data Bytes | Description    |
| ------------------- | ---------- | -------------- |
| 1111 1110 (`FEH`)   | *(none)*   | Active Sensing |

## SysEx Data Format

All "data" in patch and parameter dumps is unpacked for transmission.  
The general algorithm for transmission is:

1. Transmit header and messages code(s)
2. Clear checksum
3. Fetch next data byte to be transmitted
4. Add byte to checksum
5. Transmit (data & 0FH)
6. Transmit (data/16 & 0FH)
7. Repeat 3-6 for all data bytes
8. Transmit (checksum & 07FH)
9. Transmit F7H

### 01H - Single Patch Data

`F0H 10H 06H 01H <number> <data> <checksum> F7H`

`<number>` = destination patch number (0-99)

`<data>` = patch data unpacked to two nibbles per byte (see patch format listing)

`<checksum>` = sum of packed (not transmitted) `<data>`

Note: On receipt, this data will be stored into the specified patch location in the current bank. If this patch is currently selected in the Matrix-1000, the newly stored sound will be recalled into the edit buffer.

Note: A gap of at least 10 ms should be allowed between patches when sending multiple patches to the Matrix-1000.

### 03H - Master Parameter Data

`F0H 10H 06H 03H <version> <data> <checksum> F7H`

`<version>` = 03H for Matrix-1000

`<data>` = master data unpacked to two nibbles per byte (see master parameter format listing)

`<checksum>` = checksum of packed `<data>`

### 04H - Request Data

`F0H 10H 06H 04H <type> <number> F7H`

`<type>`:  
= 0 to request all patches in current bank and master parameters  
= 1 to request a single patch from the current bank  
= 3 to request master parameters  
= 4 to request edit buffer

`<number>`:  
= 0 when `<type>` = 0 or 3  
= number of patch requested when `<type>` = 1

Note: When all patches are requested, 100 patches from the current bank are transmitted in ascending order using individual Single Patch messages (code 01 above). This is followed by 50 dummy "split" patches for compatibility with the "Request All" function of the Matrix-6. Each of these splits has the form F0H 10H 06H 02H <36 bytes of data> F7H. For further information on this format, see the Matrix-6 MIDI Specification. All patches are transmitted with ten msec between patches.

> **Implementer note:** These Matrix-1000 dummy `02H` frames are **not** the full Matrix-6/6R Split Patch message (`02H <number> <nybble data> <checksum> F7H`). Treat them as M-6-compat padding in a Request-All stream, not as parseable Split dumps.

### 06H - Remote Parameter Edit

`F0H 10H 06H 06H <parameter> <value> F7H`

`<parameter>` = Matrix-1000 parameter number (see patch format) 0-99

`<value>` = the value to which that parameter should be set

Note: All values are sign extended from bit 6 into bit 7 except for parameter 121 (VCF Frequency). Range checking should be done on the value of each parameter before it is sent.

> **Implementer note (as printed contradiction):** The same manual lists remote-edit parameters as `0–99`, maps VCF Initial Frequency to front-panel parameter **21** (packed byte 26), and uses packed byte **121** for MM Bus 5 destination. Treat “parameter 121 (VCF Frequency)” as a likely typesetting error for **21**; do not use 121 as the remote-edit ID for VCF frequency.

### 07H - Set Group Mode

`F0H 10H 06H 07H <number> <ID> F7H`

`<number>` = total number of units in group (2-6)

`<ID>` :  
= 0 to set the first unit to be the master  
= 1-5 to set the unit to be a slave

Note: The value received for <ID> is used as the unit ID for certain SysEx messages. It is also accessible from the front panel as extended function 5.

Note: On receipt of this message, the unit enters group mode with its unit number set to <ID>. It then re-transmits this message with the ID incremented.

### 0AH - Set Bank

`F0H 10H 06H 0AH <bank> F7H`

`<bank>` = bank number to select (0-9)

Note: On receipt of this message, the unit will change banks and enable the bank lock.

### 0BH - Remote Parameter Edit (Matrix Modulation)

`F0H 10H 06H 0BH <path> <source> <val> <dest> F7H`

`<path>` = Matrix Modulation path number (0-9)

`<source>`:  
= 0 to delete this path  
= 1-20 to specify modulation source (see *Table 2*)

`<val>` = Modulation Amount

`<dest>`:  
= 0 to delete this path  
= 1-32 to specify modulation destination (see *Table 3*)

Note: The value is sign extended from bit 6 into bit 7. Range checking should be done on all data before it is sent.

### 0CH - Unlock Bank

`F0H 10H 06H 0CH F7H`

On receipt of this message, Bank Lock will be disabled.

### 0DH - Single Patch Data to Edit Buffer

`F0H 10H 06H 0DH 0 <data> <checksum> F7H`

The literal `0` after `0DH` is a fixed header byte (not part of packed patch `<data>`).  
With that byte, the header is 5 bytes — same total message size as `01H` Single Patch Data  
(268 nibbles + 5-byte header + checksum + F7 = 275 bytes).

`<data>` = patch data unpacked to two nibbles per byte (see patch format listing)

`<checksum>` = sum of packed (NOT transmitted) `<data>`

Note: On receipt, this data will be stored into the edit buffer.

Note: Wait at least ten msec after sending a patch to the Matrix-1000.

### 0EH - Store Edit Buffer

`F0H 10H 06H 0EH <number> <bank> <ID> F7H`

`<number>` = patch number within bank (0-99)

`<bank>` = bank number (0-9)

`<ID>`:  
= 0 if Group Mode is Off  
= Unit ID for target Matrix-1000 in Group Mode  
= 7FH for any unit in Group Mode

## Single Patch Data Format

* Statistics: 134 Bytes/Single Patch
* 268 nibbles transmitted + 5 bytes Header + 1 byte Checksum + 1 byte EOX
* Total transmitted: 275 bytes/Single Patch

| Byte | Parameter | # Bits     | Description                                                                                          |
| ---- | --------- | ---------- | ---------------------------------------------------------------------------------------------------- |
| 0-7  |           | 6 each     | Patch Name<br>Each character is represented<br>by the lower 6 bits of it's<br>ASCII representation   |
| 8    | 48        | 2          | Keyboard Mode<br>0 = Reassign<br>1 = Rotate<br>2 = Unison<br>3 = Reassign w/Rob                      |
| 9    | 0         | 6          | DCO 1 Initial Frequency<br>LSB = 1 Semitone                                                          |
| 10   | 5         | 6          | DCO 1 Initial Waveshape<br>0 = Sawtooth<br>31 = Triangle                                             |
| 11   | 3         | 6          | DCO 1 Initial Pulse Width                                                                            |
| 12   | 7         | 2          | DCO 1 Fixed Modulations<br>Bit 0 = Lever 1<br>Bit 1 = Vibrato                                        |
| 13   | 6         | 2          | DCO 1 Waveform Enable<br>Bit 0 = Pulse<br>Bit 1 = Wave                                               |
| 14   | 10        | 6          | DCO 2 Initial Frequency<br>LSB = 1 Semitone                                                          |
| 15   | 15        | 6          | DCO 2 Initial Waveshape<br>0 = Sawtooth<br>31 = Triangle                                             |
| 16   | 13        | 6          | DCO 2 Initial Pulse Width                                                                            |
| 17   | 17        | 2          | DCO 2 Fixed Modulations<br>Bit 0 = Lever 1<br>Bit 1 = Vibrato                                        |
| 18   | 16        | 3          | DCO 1 Waveform Enable `(as printed — likely DCO 2; Noise bit suggests DCO 2)`<br>Bit 0 = Pulse<br>Bit 1 = Wave<br>Bit 2 = Noise |
| 19   | 12        | 6 (signed) | DCO 2 Detune                                                                                         |
| 20   | 20        | 6          | Mix                                                                                                  |
| 21   | 8         | 2          | DCO 1 Fixed Modulations<br>Bit 0 = Portamento<br>Bit 1 = Not used                                    |
| 22   | 9         | 1          | DCO 1 Click                                                                                          |
| 23   | 18        | 2          | DCO 2 Fixed Modulations<br>Bit 0 = Portamento<br>Bit 1 = Keyboard Tracking enable                    |
| 24   | 19        | 1          | DCO 2 Click                                                                                          |
| 25   | 2         | 2          | DCO Sync Mode                                                                                        |
| 26   | 21        | 7          | VCF Initial Frequency<br>LSB = 1 Semitone                                                            |
| 27   | 24        | 6          | VCF Initial Resonance                                                                                |
| 28   | 25        | 2          | VCF Fixed Modulations<br>Bit 0 = Lever 1<br>Bit 1 = Vibrato                                          |
| 29   | 26        | 2          | VCF Keyboard Modulation<br>Bit 0 = Portamento<br>Bit 1 = Keyboard                                    |
| 30   | 30        | 6          | VCF FM Initial Amount                                                                                |
| 31   | 27        | 6          | VCA 1 (exponential) Initial Amount                                                                   |
| 32   | 44        | 6          | Portamento Initial Rate                                                                              |
| 33   | 46        | 2          | Lag Mode<br>0 = Constant Speed<br>1 = Constant Time<br>2 = Exponential<br>3 = Exponential `(as printed — duplicate label)` |
| 34   | 47        | 1          | Legato Portamento Enable                                                                             |
| 35   | 80        | 6          | LFO 1 Initial Speed                                                                                  |
| 36   | 86        | 2          | LFO 1 Trigger<br>0 = No Trigger<br>1 = Single Trigger<br>2 = Multi Trigger<br>3 = External Trigger   |
| 37   | 87        | 1          | LFO 1 Lag Enable                                                                                     |
| 38   | 82        | 3          | LFO 1 Waveshape<br>(See Table 1)                                                                     |
| 39   | 83        | 5          | LFO 1 Retrigger Point                                                                                |
| 40   | 88        | 5          | LFO 1 Sampled Source Number                                                                          |
| 41   | 84        | 6          | LFO 1 Initial Amplitude                                                                              |
| 42   | 90        | 6          | LFO 2 Initial Speed                                                                                  |
| 43   | 96        | 2          | LFO 2 Trigger<br>0 = No Trigger<br>1 = Single Trigger<br>2 = Multi Trigger<br>3 = External Trigger   |
| 44   | 97        | 1          | LFO 2 Lag Enable                                                                                     |
| 45   | 92        | 3          | LFO 2 Waveshape<br>(See Table 1)                                                                     |
| 46   | 93        | 5          | LFO 2 Retrigger Point                                                                                |
| 47   | 98        | 5          | LFO 2 Sampled Source Number                                                                          |
| 48   | 94        | 6          | LFO 2 Initial Amplitude                                                                              |
| 49   | 57        | 3          | Env 1 Trigger Mode<br>Bit 0 = Reset<br>Bit 1 = Multi Trigger<br>Bit 2 = External Trigger             |
| 50   | 50        | 6          | Env 1 Initial Delay Time                                                                             |
| 51   | 51        | 6          | Env 1 Initial Attack Time                                                                            |
| 52   | 52        | 6          | Env 1 Initial Decay Time                                                                             |
| 53   | 53        | 6          | Env 1 Sustain Level                                                                                  |
| 54   | 54        | 6          | Env 1 Initial Release Time                                                                           |
| 55   | 55        | 6          | Env 1 Initial Amplitude                                                                              |
| 56   | 59        | 2          | Env 1 LFO Trigger Mode<br>Bit 0 = Gated<br>Bit 1 = LFO Trigger                                       |
| 57   | 58        | 2          | Env 1 Mode<br>Bit 0 = DADR Mode<br>Bit 1 = Freerun                                                   |
| 58   | 67        | 3          | Env 2 Trigger Mode<br>Bit 0 = Reset<br>Bit 1 = Multi Trigger<br>Bit 2 = External Trigger             |
| 59   | 60        | 6          | Env 2 Initial Delay Time                                                                             |
| 60   | 61        | 6          | Env 2 Initial Attack Time                                                                            |
| 61   | 62        | 6          | Env 2 Initial Decay Time                                                                             |
| 62   | 63        | 6          | Env 2 Sustain Level                                                                                  |
| 63   | 64        | 6          | Env 2 Initial Release Time                                                                           |
| 64   | 65        | 6          | Env 2 Initial Amplitude                                                                              |
| 65   | 69        | 2          | Env 2 LFO Trigger Mode<br>Bit 0 = Gated<br>Bit 1 = LFO Trigger                                       |
| 66   | 68        | 2          | Env 2 Mode<br>Bit 0 = DADR Mode<br>Bit 1 = Freerun                                                   |
| 67   | 77        | 3          | Env 3 Trigger Mode<br>Bit 0 = Reset<br>Bit 1 = Multi Trigger<br>Bit 2 = External Trigger             |
| 68   | 70        | 6          | Env 3 Initial Delay Time                                                                             |
| 69   | 71        | 6          | Env 3 Initial Attack Time                                                                            |
| 70   | 72        | 6          | Env 3 Initial Decay Time                                                                             |
| 71   | 73        | 6          | Env 3 Sustain Level                                                                                  |
| 72   | 74        | 6          | Env 3 Initial Release Time                                                                           |
| 73   | 75        | 6          | Env 3 Initial Amplitude                                                                              |
| 74   | 79        | 2          | Env 3 LFO Trigger Mode<br>Bit 0 = Gated<br>Bit 1 = LFO Trigger                                       |
| 75   | 78        | 2          | Env 3 Mode<br>Bit 0 = DADR Mode<br>Bit 1 = Freerun                                                   |
| 76   | 33        | 5          | Tracking Generator Input Source Code<br>(See Table 2)                                                |
| 77   | 34        | 6          | Tracking Point 1                                                                                     |
| 78   | 35        | 6          | Tracking Point 2                                                                                     |
| 79   | 36        | 6          | Tracking Point 3                                                                                     |
| 80   | 37        | 6          | Tracking Point 4                                                                                     |
| 81   | 38        | 6          | Tracking Point 5                                                                                     |
| 82   | 40        | 6          | Ramp 1 Rate                                                                                          |
| 83   | 41        | 2          | Ramp 1 Mode<br>0 = Single Trigger<br>1 = Multi Trigger<br>2 = External Trigger<br>3 = External Gated |
| 84   | 42        | 6          | Ramp 2 Rate                                                                                          |
| 85   | 43        | 2          | Ramp 2 Mode<br>0 = Single Trigger<br>1 = Multi Trigger<br>2 = External Trigger<br>3 = External Gated |
| 86   | 1         | 7 (signed) | DCO 1 Frequency by LFO 1 Amount                                                                      |
| 87   | 4         | 7 (signed) | DCO 1 PW by LFO 2 Amount                                                                             |
| 88   | 11        | 7 (signed) | DCO 2 Frequency by LFO 1 Amount                                                                      |
| 89   | 14        | 7 (signed) | DCO 2 PW by LFO 2 Amount                                                                             |
| 90   | 22        | 7 (signed) | VCF Frequency by Env 1 Amount                                                                        |
| 91   | 23        | 7 (signed) | VCF Frequency by Pressure Amount                                                                     |
| 92   | 28        | 7 (signed) | VCA 1 by Velocity Amount                                                                             |
| 93   | 29        | 7 (signed) | VCA 2 by Env 2 Amount                                                                                |
| 94   | 56        | 7 (signed) | Env 1 Amplitude by Velocity Amount                                                                   |
| 95   | 66        | 7 (signed) | Env 2 Amplitude by Velocity Amount                                                                   |
| 96   | 76        | 7 (signed) | Env 3 Amplitude by Velocity Amount                                                                   |
| 97   | 85        | 7 (signed) | LFO 1 Amplitude by Ramp 1 Amount                                                                     |
| 98   | 95        | 7 (signed) | LFO 2 Amplitude by Ramp 2 Amount                                                                     |
| 99   | 45        | 7 (signed) | Portamento Rate by Velocity Amount                                                                   |
| 100  | 31        | 7 (signed) | VCF FM Amount by Env 3 Amount                                                                        |
| 101  | 32        | 7 (signed) | VCF FM Amount by Pressure Amount                                                                     |
| 102  | 81        | 7 (signed) | LFO 1 Speed by Pressure Amount                                                                       |
| 103  | 91        | 7 (signed) | LFO 2 Speed by Keyboard Amount                                                                       |
| 104  |           | 5          | Matrix Modulation Bus 0 Source Code<br>(See Table 2)                                                 |
| 105  |           | 7 (signed) | Matrix Modulation Bus 0 Amount                                                                       |
| 106  |           | 5          | Matrix Modulation Bus 0 Destination Code<br>(See Table 3)                                            |
| 107  |           | 5          | Matrix Modulation Bus 1 Source Code<br>(See Table 2)                                                 |
| 108  |           | 7 (signed) | Matrix Modulation Bus 1 Amount                                                                       |
| 109  |           | 5          | Matrix Modulation Bus 1 Destination Code<br>(See Table 3)                                            |
| 110  |           | 5          | Matrix Modulation Bus 2 Source Code<br>(See Table 2)                                                 |
| 111  |           | 7 (signed) | Matrix Modulation Bus 2 Amount                                                                       |
| 112  |           | 5          | Matrix Modulation Bus 2 Destination Code<br>(See Table 3)                                            |
| 113  |           | 5          | Matrix Modulation Bus 3 Source Code<br>(See Table 2)                                                 |
| 114  |           | 7 (signed) | Matrix Modulation Bus 3 Amount                                                                       |
| 115  |           | 5          | Matrix Modulation Bus 3 Destination Code<br>(See Table 3)                                            |
| 116  |           | 5          | Matrix Modulation Bus 4 Source Code<br>(See Table 2)                                                 |
| 117  |           | 7 (signed) | Matrix Modulation Bus 4 Amount                                                                       |
| 118  |           | 5          | Matrix Modulation Bus 4 Destination Code<br>(See Table 3)                                            |
| 119  |           | 5          | Matrix Modulation Bus 5 Source Code<br>(See Table 2)                                                 |
| 120  |           | 7 (signed) | Matrix Modulation Bus 5 Amount                                                                       |
| 121  |           | 5          | Matrix Modulation Bus 5 Destination Code<br>(See Table 3)                                            |
| 122  |           | 5          | Matrix Modulation Bus 6 Source Code<br>(See Table 2)                                                 |
| 123  |           | 7 (signed) | Matrix Modulation Bus 6 Amount                                                                       |
| 124  |           | 5          | Matrix Modulation Bus 6 Destination Code<br>(See Table 3)                                            |
| 125  |           | 5          | Matrix Modulation Bus 7 Source Code<br>(See Table 2)                                                 |
| 126  |           | 7 (signed) | Matrix Modulation Bus 7 Amount                                                                       |
| 127  |           | 5          | Matrix Modulation Bus 7 Destination Code<br>(See Table 3)                                            |
| 128  |           | 5          | Matrix Modulation Bus 8 Source Code<br>(See Table 2)                                                 |
| 129  |           | 7 (signed) | Matrix Modulation Bus 8 Amount                                                                       |
| 130  |           | 5          | Matrix Modulation Bus 8 Destination Code<br>(See Table 3)                                            |
| 131  |           | 5          | Matrix Modulation Bus 9 Source Code<br>(See Table 2)                                                 |
| 132  |           | 7 (signed) | Matrix Modulation Bus 9 Amount                                                                       |
| 133  |           | 5          | Matrix Modulation Bus 9 Destination Code<br>(See Table 3)                                            |

### Table 1

**LFO Wave Codes**

0 = Triangle  
1 = Up Sawtooth  
2 = Down Sawtooth  
3 = Square  
4 = Random  
5 = Noise  
6 = Sampled Modulation  
7 = Not Used

### Table 2

**Modulation Source Codes / Track Generator Inputs**

Unused modulations must have their `Sources` and `Destinations` set to 0.

0 = Unused Modulation  
1 = Env 1     
2 = Env 2     
3 = Env 3  
4 = LFO 1  
5 = LFO 2     
6 = Vibrato  
7 = Ramp 1  
8 = Ramp 2  
9 = Keyboard  
10 = Portamento  
11 = Tracking Generator  
12 = Keyboard Gate  
13 = Velocity  
14 = Release Velocity  
15 = Pressure  
16 = Pedal 1  
17 = Pedal 2  
18 = Lever 1  
19 = Lever 2  
20 = Lever 3

Note: The `0 = Unused Modulation` parameter in this table is found in the `Modulation Source` list only. This parameter is not a `Tracking Generator` input and thus will not be displayed in parameter `33 - Track Input`.

### Table 3

**Modulation Destination Codes**

Unused Modulations must have their `Sources` and `Destinations` set to 0.

0 = Unused Modulation  
1 = DCO 1 Frequency  
2 = DCO 1 Pulse Width  
3 = DCO 1 Waveshape  
4 = DCO 2 Frequency  
5 = DCO 2 Pulse Width  
6 = DCO 2 Waveshape  
7 = Mix Level  
8 = VCF FM Amount  
9 = VCF Frequency  
10 = VCF Resonance  
11 = VCA 1 Level  
12 = VCA 2 Level  
13 = Envelope 1 Delay  
14 = Envelope 1 Attack  
15 = Envelope 1 Decay  
16 = Envelope 1 Release  
17 = Envelope 1 Amplitude  
18 = Envelope 2 Delay  
19 = Envelope 2 Attack  
20 = Envelope 2 Decay  
21 = Envelope 2 Release  
22 = Envelope 2 Amplitude  
23 = Envelope 3 Delay  
24 = Envelope 3 Attack  
25 = Envelope 3 Decay  
26 = Envelope 3 Release  
27 = Envelope 3 Amplitude  
28 = LFO 1 Speed  
29 = LFO 1 Amplitude  
30 = LFO 2 Speed  
31 = LFO 2 Amplitude  
32 = Portamento Time

## Global Parameters Data Format

* Statistics: 172 bytes/Global Parameters
* 344 nibbles transmitted + 5 bytes Header + 1 byte Checksum + 1 byte EOX
* Total transmitted = 351 bytes/Global parameters

| Byte    | # Bits     | Description                                     |
| ------- | ---------- | ----------------------------------------------- |
| 0       |            | *Not Used*                                      |
| 1       | 6          | Vibrato Speed                                   |
| 2       | 2          | Vibrato Speed Mod Source Code                   |
| 3       | 6          | Vibrato Speed Modulation Amount                 |
| 4       | 3          | Vibrato Waveform                                |
| 5       | 6          | Vibrato Amplitude                               |
| 6       | 2          | Vibrato Amp Mod Source Code                     |
| 7       | 6          | Vibrato Amp Modulation Amount                   |
| 8       | 6 (signed) | Master Tune                                     |
| 9       |            | *Not Used*                                      |
| 10      |            | *Not Used*                                      |
| 11      | 4          | MIDI Basic Channel                              |
| 12      | 1          | MIDI Omni Mode Enable                           |
| 13      | 1          | MIDI Controllers Enable                         |
| 14      | 1          | MIDI Patch Changes Enable                       |
| 15      |            | *Not Used*                                      |
| 16      |            | *Not Used*                                      |
| 17      | 7          | MIDI Pedal 1 Controller                         |
| 18      | 7          | MIDI Pedal 2 Controller                         |
| 19      | 7          | MIDI Lever 2 Controller                         |
| 20      | 7          | MIDI Lever 3 Controller                         |
| 21-31   |            | *Not Used*                                      |
| 32      | 1          | MIDI Echo Enable                                |
| 34      | 6 (signed) | Master Transpose                                |
| 35      | 1          | MIDI Mono Mode Enable                           |
| 36-161  | 1 each     | Group Enables<br>(One bit per patch, LSB first) |
| 162-163 |            | *Not Used*                                      |
| 164     | 1          | Bend Range                                      |
| 165     | 1          | Bank Lock Enable<br>(In MSB only)               |
| 166     | 1          | Number of Units<br>(Group Mode) `(as printed bit-width; values 2–6 via 07H — treat width as uncertain)` |
| 167     | 1          | Current Unit Number<br>(Group Mode) `(as printed bit-width; IDs 0–5 via 07H — treat width as uncertain)` |
| 168     | 1          | Group Mode Enable<br>(In MSB only)              |
| 169     | 1          | Unison Enable                                   |
| 170     | 1          | Volume Invert Enable                            |
| 171     | 1          | Memory Protect Enable                           |

## Miscellaneous Commands

### Bank Select

`F0H 7FH <ID> 02H 01H <chan> <disc-lo> <disc-hi> <dir-lo> <dir-hi> <bank-lo> <bank-hi> F7H`

`<ID>`:  
= `7FH` for all devices (including this one)  
= (0-5) must match unit's ID

`<disc-lo>` = don't care  
`<disc-hi>` = don't care  
`<dir-lo>` = don't care  
`<dir-hi>` = don't care  
`<bank-lo>` = bank to select (0-9)  
`<bank-hi>` = 0

### Device Inquiry

`F0H 7EH <chan> 06H 01H F7H`

`<chan>`:  
= `7FH` for any device (including this one)  
= (0-15) must match unit's basic channel

### Device ID

`F0H 7EH <chan> 06H 02H <mfg> <fam-lo> <fam-hi> <memb-lo> <memb-hi> <rev-0> <rev-1> <rev-2> <rev-3> F7H`

`<chan>` = Transmitting device's Basic Channel  
`<mfg>` = 10H (Oberheim)  
`<fam-lo>` = 6H (Matrix-6/6R/1000)  
`<fam-hi>` = 0H  
`<memb-lo>` = 2H (Matrix-1000)  
`<memb-hi>` = 0H  
`<rev-0>` thru `<rev-3>` = four ASCII characters of the version number, right justified

Example: version 1.10 is represented by:

* `<rev-0>` = 20H (` `)  
* `<rev-1>` = 31H (`1`)  
* `<rev-2>` = 31H (`1`)  
* `<rev-3>` = 30H (`0`)

### Active Controllers

1 = Vibrato Amount (Lever 2)  
2 = Breath Control (Lever 3)  
4 = Foot Pedal     (Pedal 1)  
6 = Data Entry  
7 = Volume  
31 = Bank Change Enable  
64 = Sustain (Pedal 2)  
96 = Increment  
97 = Decrement  
98 = Non-Reg Parm LSB  
99 = Non-Reg Parm MSB  
100 = Reg Parm LSB  
101 = Reg Parm MSB  

### Registered Parameters

0 = Pitch Bend Range (in semitones)  
1 = Fine Tune (in 128ths of a semitone)  
2 = Transpose (in semitones)

Non-registered parameters may be used to control any parameter by using the front panel parameter number. Matrix modulations must be controlled by use of the System Exclusive message. When using registered or non-registered parameter editing, all received data entry controller values are offset by 40H, except when the currently selected parameter is the registered pitch bend range. Thus, a data entry value of 40H will update the selected parameter to be 0, while 3EH is interpreted as -2. This allows for transmission of negative numbers.

---

## Verification notes

Re-checked 2026-07-17 against the official Matrix-1000 Owner's Manual PDF (MIDI Summary through miscellaneous commands; scanned pages with footer ~41–56).

**Firm transcription fixes applied in this file** (see YAML `notes`).

**Left as printed / ambiguous (do not invent):**

- Global Parameter byte **33** is omitted in the manual table (jumps 32 → 34); unused rows elsewhere are explicit.
- Bank Select SysEx line is poorly OCR’d in the scan; the reconstructed form here matches a prior clean transcription and was not re-proven from a crisp glyph-level read.
- Remote-edit note “parameter 121 (VCF Frequency)” conflicts with param range `0–99` and param **21** for VCF frequency — see implementer note under `06H`.
- Global bytes 166–167 list `# Bits: 1` while Group Mode uses multi-value IDs — see table caveats.
- Byte 18 “DCO 1 … Noise” and Lag Mode duplicate “Exponential” labels — marked in the patch table.
