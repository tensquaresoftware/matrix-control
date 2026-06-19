# Matrix-1000 Bank Lock — MIDI Hardware Spike (Handoff)

**Date:** 2026-06-18  
**Author:** Guillaume / BMad agent  
**Status:** Not required for v1 — superseded by D-022-R1 (UNLOCK-only Bank Utility, 2026-06-19 Correct Course)  
**Blocks:** ~~Bank Utility lock UX redesign (Epic 7)~~ — resolved via Correct Course  
**Related:** Story 7.3 (implemented), Story 7-3b (corrective), Story 7.5 (backlog), `miscBankLockEnable` (master byte 165)

---

## Context

Oberheim Matrix-1000 bank lock semantics are **asymmetric** in the official spec:

| Message | Opcode | Documented effect |
|---------|--------|-------------------|
| Set Bank | `0AH` | Select bank 0–9 **and enable bank lock** |
| Unlock Bank | `0CH` | **Disable** bank lock |
| *(none)* | — | No dedicated « Lock Bank only » SysEx |

Current Matrix-Control (Story 7.3) sends **Set Bank `0AH`** on bank select, navigation, and patch NumberBox sync — which **re-locks** the synth on every coordinate change. That explains the persistent red dot on the M-1000 display.

**Hypothesis to validate:** Universal **Bank Select** SysEx may change the active bank **without** toggling bank lock, enabling decoupled « change bank » vs « lock/unlock » UX (toggle UNLOCKED/LOCKED in Bank Utility).

**Secondary hypothesis:** Sending `0AH` with the **already active** bank number may act as « lock only » without audible bank change.

---

## Prerequisites

- Matrix-1000 connected MIDI Out ← plugin/editor, MIDI monitor on that port
- Known starting state: note whether the **red lock dot** on the synth display is on or off
- Optional: request master dump (`04H` type 3) before/after each step to inspect byte **165 — Bank Lock Enable**

---

## Reference messages

### Set Bank (Oberheim manufacturer SysEx)

```
F0 10 06 0A <bank> F7
```

Example bank 3: `F0 10 06 0A 03 F7`

### Unlock Bank

```
F0 10 06 0C F7
```

### Universal Bank Select (per Oberheim spec — verify `<ID>` on your unit)

```
F0 7F <ID> 02 01 <chan> <disc-lo> <disc-hi> <dir-lo> <dir-hi> <bank-lo> <bank-hi> F7
```

**Suggested test payload** (all « don't care » fields zero, all devices, channel 0):

```
F0 7F 7F 02 01 00 00 00 00 00 <bank-lo> 00 F7
```

Example bank 5: `F0 7F 7F 02 01 00 00 00 00 00 05 00 F7`

Try also `<ID> = 00` if `7F` is ignored by your firmware.

### Program Change (baseline — patch within current bank only)

```
C0 <patch 0–7F>
```

---

## Test matrix

Record for **each row**: red dot (on/off), audible bank change (Y/N), current patch/bank on display, notes.

| Step | Action | Red dot after? | Bank changed? | Pass criteria |
|------|--------|--------------|---------------|---------------|
| A0 | Power-on baseline | record | — | — |
| A1 | Send `0CH` | **off** | N | Unlock works |
| A2 | Send `0AH` bank **current** | **on** | N? | Lock-only without bank change? |
| A3 | Send `0CH` | off | N | Reset |
| A4 | Send `0AH` bank **different** | on | Y | Spec baseline |
| B1 | From unlocked: Universal Bank Select → bank **2** | ? | Y? | **Key test:** dot stays **off** |
| B2 | From locked (after A4): Universal Bank Select → bank **7** | ? | Y? | Dot stays **on** or turns **off**? |
| B3 | From unlocked: PC to patch 42 only | unchanged | N (patch only) | PC does not affect lock |
| C1 | Front panel Bank Lock toggle off → on | on | N | Reference for UI target |
| C2 | Front panel Bank Lock toggle on → off | off | N | Reference for UI target |

---

## Decision tree (fill after tests)

```
B1 dot stays OFF after Universal Bank Select?
├── YES → Use Universal Bank Select for bank buttons when UNLOCKED
│         Use 0AH only when user explicitly locks (or 0AH same-bank if A2 passes)
│         Use 0CH when user unlocks
└── NO  → Universal Bank Select not sufficient
          ├── A2 passes (0AH same-bank = lock only)?
          │   └── Use 0AH(currentBank) for lock; never 0AH on mere bank change while unlocked
          └── Need master byte 165 path or accept lock coupled to 0AH bank changes
```

---

## Observations to capture

1. Does Universal Bank Select change the **three-digit patch display** bank digit (hundreds)?
2. After B1, does Program Change 0–99 address patches in the **new** bank while unlocked?
3. Does master byte **165** in a dumped master block match red dot state after each step?
4. Any firmware version string from Device Inquiry (for doc archival)?

---

## Out of scope for this spike

- Matrix-6/6R (no bank concept)
- GUI toggle styling (Story 7.5)
- Code changes in Matrix-Control
- `miscBankLockEnable` ↔ Bank Utility sync (separate work item — see Correct Course)

---

## Suggested follow-up artifacts

| Outcome | Next step |
|---------|-----------|
| Spike results documented | **Correct Course** — revise FR-19/FR-20 MIDI strategy + Bank Utility UX |
| Universal Bank Select works | Amend Story **7.5** (+ optional 7.3 patch for `PatchSelectionMidiSync`) |
| `0AH` same-bank locks only | Simpler Core fix; still run Correct Course for UX toggle spec |
| Neither works | Escalate: master dump bit 165 toggle path; PRD review |

---

## Copy-paste SysEx (hex strings)

```
Unlock:           F0 10 06 0C F7
Set bank 0:       F0 10 06 0A 00 F7
Set bank 3:       F0 10 06 0A 03 F7
Universal bank 5: F0 7F 7F 02 01 00 00 00 00 00 05 00 F7
PC patch 10:      C0 0A
```

---

## Reporter

Fill results below and attach MIDI monitor screenshots if useful.

| Step | Red dot | Bank/patch display | Notes |
|------|---------|-------------------|-------|
| A1 | | | |
| A2 | | | |
| A4 | | | |
| B1 | | | |
| B2 | | | |
| C1 | | | |
| C2 | | | |

**Conclusion:**

**Recommended MIDI strategy for Matrix-Control:**

**Date completed:**
