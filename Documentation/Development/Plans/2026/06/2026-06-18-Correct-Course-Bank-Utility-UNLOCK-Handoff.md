# Correct Course Handoff — Bank Utility UNLOCK (revert BANK LOCK toggle)

**Date:** 2026-06-18  
**Author:** Guillaume  
**Prepared for:** Fresh-context `bmad-correct-course` run  
**Status:** Ready to execute  
**Triggers:** Product decision after Story 7.3 review + Oberheim MIDI spec analysis  
**Supersedes:** D-022 rationale (BANK LOCK toggle aligned with front panel); U-IDs harmonization decision `bankUtilityLockBank` / `kLockBank`

---

## 1. Executive summary

Revert Bank Utility from a **BANK LOCK toggle** (Story 7.3 implementation) to a **simple UNLOCK button**, matching:

- Oberheim Matrix-1000 MIDI spec (`0AH` = Set Bank + lock ON; `0CH` = Unlock Bank only)
- Legacy Matrix-1000 Editor (Max for Live) UX
- Lower implementation risk (no `0AH`+`0CH` pairing, no lock toggle state machine)

**User mental model:**

| User action | MIDI | Hardware |
|-------------|------|----------|
| Click bank **0–9** | `SetBank(n)` → `0AH` | Bank changes; **lock enabled** (red dot ON) |
| Click **UNLOCK** | `UnlockBank()` → `0CH` | Lock **disabled** (red dot OFF); bank unchanged |
| Navigate `<` `>` across banks while unlocked | No `0AH` until user selects a bank or crosses bank via navigation that sends `0AH` | Per existing navigation rules (D-023) |

**Out of scope for this Correct Course:**

- Universal Bank Select SysEx spike (archived — not needed for v1)
- Toggle LOCKED/UNLOCKED UI
- Bidirectional sync `miscBankLockEnable` ↔ Bank Utility (optional follow-up)
- Master request at startup for byte 165 (optional follow-up)

---

## 2. Decision record (for Sprint Change Proposal)

### D-022-R1 — Bank Utility control is UNLOCK only

- **Decision:** Bank Utility shows **UNLOCK** (not BANK LOCK). Single action: send `0CH`, clear editor lock mirror. No toggle, no `0AH` on UNLOCK.
- **Rationale:** Oberheim asymmetry (`0AH` locks as side-effect; only `0CH` unlocks). One extra click after bank change is acceptable; red dot on Internal Patches bank NumberBox + synth display reminds user that bank select re-locks.
- **Reverts:** D-022 follow-up « BANK LOCK label »; Story 7.3 AC #3 toggle semantics; U-IDs story rename `unlockBank` → `bankUtilityLockBank`.

### D-022-R2 — Lock state is derived, not toggled by button

- `patchManagerBanksLocked` (APVTS state bool) reflects **whether banks are currently locked** (hardware mirror + navigation input).
- Set **`true`** when `0AH` is sent (bank button, navigation crossing bank, `PatchSelectionMidiSync` Set Bank path).
- Set **`false`** only when `0CH` is sent (UNLOCK button) or explicit hardware/master resync (future).
- UNLOCK button **never** sets locked `true`.

### D-022-R3 — `miscBankLockEnable` remains distinct

- Master Edit → Misc → **BANK LOCK** (`miscBankLockEnable`, master byte 165) is **not** the Bank Utility button.
- Do **not** rename master parameter IDs/display for this course.
- No requirement to bidirectionally sync in this change set (document as deferred).

---

## 3. Target behaviour (acceptance-level)

### 3.1 Bank Utility UNLOCK button

1. Label displays **UNLOCK** (English, per `PluginDisplayNames`).
2. Click → `PatchManagerActionHandler` sends **only** `MidiManager::sendUnlockBank()` (`0CH`).
3. Sets `patchManagerBanksLocked` APVTS state to `false`.
4. Does **not** flip a toggle; does **not** send `0AH`.
5. Grayed when `!DeviceMemoryLimits::hasBankConcept()` (Matrix-6/6R), same as bank buttons.

### 3.2 Bank select 0–9

1. Click bank N → update `patchManagerSelectedBank` + `internalPatchesCurrentBankNumber`.
2. Send `PatchSelectionMidiSync::syncSelection` (or equivalent): `0AH` + Program Change.
3. Set `patchManagerBanksLocked` to **`true`** (because `0AH` enables lock on hardware).

### 3.3 Navigation / patch NumberBox

- Preserve Story 7.3 MIDI sync paths.
- `patchManagerBanksLocked` drives `DeviceMemoryLimits::advancePatch(..., banksLocked)` wrap rules (D-023): locked → wrap within bank; unlocked → wrap across banks.
- When navigation changes bank and sends `0AH`, set `patchManagerBanksLocked = true`.

### 3.4 Internal Patches UI (Story 7.5 scope — document now, implement in follow-up story)

- Red dot on **bank NumberBox** when `patchManagerBanksLocked == true` (not on UNLOCK button).
- No padlock on bank Utility UNLOCK button.

### 3.5 Persistence

- `patchManagerBanksLocked` and bank coordinates persist via existing APVTS state tree (no new persistence mechanism).

---

## 4. Code rename inventory (mandatory alignment)

### 4.1 Principle

| Layer | Naming rule |
|-------|-------------|
| **Action** (button) | `Unlock` / `UNLOCK` — verb matches what the control does |
| **State** (bool) | `BanksLocked` — adjective state, not an action |
| **MIDI SysEx** | Keep `kUnlockBank`, `encodeUnlockBank`, `sendUnlockBank` (Oberheim opcode 0CH — already correct) |
| **Master param** | Keep `miscBankLockEnable`, `kBankLockEnable` (byte 165 — different domain) |

### 4.2 Rename table — Bank Utility action (button)

| Current | Target | Files (non-exhaustive — grep to verify) |
|---------|--------|----------------------------------------|
| APVTS action string `bankUtilityLockBank` | `bankUtilityUnlockBank` | `PluginIDs.h`, `ActionPropertyRegistry.cpp`, `PluginDescriptorsPatchManager.cpp`, tests |
| C++ `kLockBank` | `kUnlockBank` | `PluginIDs.h` + all references |
| Display `"BANK LOCK"` | `"UNLOCK"` | `PluginDisplayNames.h` → `BankUtilityModule::StandaloneWidgets::kUnlockBank` |
| `lockBankButton_` | `unlockBankButton_` | `BankUtilityPanel.h/.cpp` |
| `patchManagerLockBankWidth` | `patchManagerUnlockBankWidth` | `WidgetDimensions.h`, `DimensionFactory.cpp`, `BankUtilityPanel.cpp` |
| `kPatchManagerLockBank` | `kPatchManagerUnlockBank` | `DesignAtoms.h`, `WidgetDimensionRegistry.cpp`, tests |
| Comments « lock bank » | « unlock bank » | `BankUtilityPanel.cpp`, dimension registry |

### 4.3 Rename table — APVTS lock **state** (not the button)

| Current | Target | Rationale |
|---------|--------|-----------|
| `patchManagerBankLock` | `patchManagerBanksLocked` | State = « banks are locked »; avoids confusion with old lock button |
| `kBankLock` (StateProperties) | `kBanksLocked` | C++ symbol matches state semantics |

**Update all readers/writers:**

- `PatchManagerActionHandler.cpp`
- `MidiManager.cpp` (init default)
- `PatchManagerActionHandlerTests.cpp`
- `DeviceMemoryLimits` call sites (`bankLocked` local → `banksLocked` optional cleanup)
- Any `valueTreePropertyChanged` listeners (grep `patchManagerBankLock`)
- Saved sessions: **breaking APVTS property rename** — document migration or accept reset-on-upgrade for dev builds (plugin not released; prefer clean rename without migration unless Guillaume requests compat shim)

### 4.4 Do NOT rename

| Symbol | Reason |
|--------|--------|
| `miscBankLockEnable` / `kBankLockEnable` | Master Edit parameter (byte 165) |
| `PluginDisplayNames::MasterEditSection::...::kBankLockEnable` → display **BANK LOCK** | Correct for master parameter |
| `SysExConstants::Opcode::kUnlockBank` | Oberheim 0CH opcode name |
| `MidiManager::sendUnlockBank()` | Sends unlock — correct verb |
| `PatchSelectionMidiSync::sendUnlockBank()` | MIDI helper — correct |
| `encodeSetBank` / `sendSetBank` | Oberheim 0AH |

### 4.4 Naming collision note

After rename, `PluginIDs::...::kUnlockBank` (action widget) and `SysExConstants::Opcode::kUnlockBank` (opcode byte) share the English word « UnlockBank » in different namespaces — **acceptable**; use fully qualified names in Core if ambiguous.

---

## 5. Core logic changes (Story 7.3 fix)

### 5.1 `PatchManagerActionHandler` — replace toggle block

**Remove** (current Story 7.3):

```cpp
// Toggle kBankLock, send SetBank on lock-on, Unlock on lock-off
```

**Replace with:**

```cpp
void handleUnlockBank(const DeviceMemoryLimits& limits)
{
    if (!limits.hasBankConcept()) return;
    if (patchSelectionMidiSync_ != nullptr)
        patchSelectionMidiSync_->sendUnlockBank(limits);
    apvts_.state.setProperty(kBanksLocked, false, nullptr);
}
```

- Register action on `kUnlockBank` (`bankUtilityUnlockBank`).
- **No** `setProperty(kBanksLocked, true)` in unlock handler.

### 5.2 Set `banksLocked = true` when sending Set Bank

Centralize in `PatchSelectionMidiSync::syncSelection` or handler after any path that enqueues `0AH`:

- Bank button select
- Navigation changing bank
- `PluginProcessor::handlePatchNumberChange` when Set Bank sent

Avoid duplicate scattered assignments — prefer one helper `markBanksLockedInApvts()`.

### 5.3 Lock-on via bank button when already locked

No special case: `0AH` with new bank is correct (stays locked).

### 5.4 Lock-on via bank button when unlocked

Sending `0AH` re-locks per spec — set `banksLocked = true`. **No** follow-up `0CH`.

---

## 6. Files to touch (implementation checklist)

### Shared definitions

- [ ] `Source/Shared/Definitions/PluginIDs.h`
- [ ] `Source/Shared/Definitions/PluginDisplayNames.h`
- [ ] `Source/Shared/Definitions/PluginDescriptorsPatchManager.cpp`

### Core

- [ ] `Source/Core/Actions/PatchManagerActionHandler.cpp`
- [ ] `Source/Core/Actions/ActionPropertyRegistry.cpp`
- [ ] `Source/Core/MIDI/PatchSelectionMidiSync.cpp` (optional: APVTS lock state after `0AH`)
- [ ] `Source/Core/MIDI/MidiManager.cpp` (init property name)
- [ ] `Source/Core/PluginProcessor.cpp` (if property listeners reference old name)

### GUI

- [ ] `Source/GUI/Panels/.../BankUtilityPanel.h/.cpp`
- [ ] `Source/GUI/Layout/Design/DesignAtoms.h`
- [ ] `Source/GUI/Layout/WidgetDimensions.h`
- [ ] `Source/GUI/Factories/DimensionFactory.cpp`
- [ ] `Source/GUI/Layout/WidgetDimensionRegistry.cpp`

### Tests

- [ ] `Tests/Unit/PatchManagerActionHandlerTests.cpp` — rename `lockOff_sendsUnlock` → `unlockBank_sends0CH`; remove toggle expectations; assert `banksLocked` false after unlock, true after bank select
- [ ] `Tests/Unit/WidgetDimensionRegistryTests.cpp`
- [ ] `Tests/Unit/ActionDispatcherTests.cpp` (if property id referenced)
- [ ] Full `Matrix-Control_Tests` + VST3 + Standalone Debug build

### Planning artifacts (Correct Course output)

- [ ] `_bmad-output/planning-artifacts/prds/.../prd.md` §4.6 Bank Utility
- [ ] `_bmad-output/planning-artifacts/prds/.../.decision-log.md` — add D-022-R1/R2/R3, annotate D-022 superseded
- [ ] `_bmad-output/planning-artifacts/epics.md` — FR-20, Story 7.5 AC
- [ ] `_bmad-output/implementation-artifacts/7-3-*.md` — add errata or mark follow-up story
- [ ] Archive or annotate spike `2026-06-18-Matrix-1000-Bank-Lock-MIDI-Spike.md` as **not required**

---

## 7. Story breakdown after Correct Course

| Story | Title | Scope |
|-------|-------|-------|
| **7-3b** (new) | Bank Utility UNLOCK semantics + ID rename | Core handler, renames, tests, MidiManager init property |
| **7.5** (amend) | Bank Utility + Internal Patches visuals | UNLOCK label wiring (if not in 7-3b), red dot on bank NumberBox when `banksLocked`, active bank red text |

Execute **7-3b** before **7.5**.

---

## 8. PRD / FR amendments (draft text)

### FR-20 (replace)

> **FR-20:** Bank Utility provides bank buttons 0–9 (Set Bank / `0AH`, enables hardware bank lock) and an **UNLOCK** button (Unlock Bank / `0CH`). Editor state `patchManagerBanksLocked` mirrors hardware lock for navigation wrap and UI indicators. No lock toggle on Bank Utility.

### PRD §4.6 (replace BANK LOCK paragraph)

> **UNLOCK** sends Unlock Bank SysEx (`0CH`) and clears the editor lock mirror. Selecting a bank sends Set Bank (`0AH`) and locks per Oberheim spec. Internal Patches bank NumberBox shows a red lock indicator when banks are locked. Master Edit **BANK LOCK** (`miscBankLockEnable`) is a separate master parameter (byte 165).

---

## 9. Test plan (manual — post-implementation)

1. Matrix-1000, MIDI monitor on.
2. Click bank **3** → `F0 10 06 0A 03 F7` + PC; red dot ON (synth + editor when 7.5 done).
3. Click **UNLOCK** → `F0 10 06 0C F7` only; red dot OFF; bank still 3.
4. Click bank **0** → `0AH 00`; red dot ON.
5. `<` `>` while unlocked (after UNLOCK) → wraps across banks per D-023; `0AH` when bank changes.
6. Matrix-6: bank buttons + UNLOCK grayed; no SysEx `0AH`/`0CH`.

---

## 10. Risks and mitigations

| Risk | Mitigation |
|------|------------|
| APVTS property rename breaks saved sessions | Pre-release; document in change log; optional one-time migration listener |
| Stale docs reference `bankUtilityLockBank` | Grep repo + update `_bmad-output` in Correct Course |
| `kUnlockBank` symbol collision (widget vs opcode) | Namespaces; code review |
| 7.3 story marked done with wrong AC | Errata + 7-3b story references this handoff |

---

## 11. Commands for fresh-context agent

```
/bmad-correct-course
```

**Input to paste:**

> Use handoff: `Documentation/Development/Plans/2026/06/2026-06-18-Correct-Course-Bank-Utility-UNLOCK-Handoff.md`  
> Produce Sprint Change Proposal, update PRD/epics/decision-log, then recommend story 7-3b creation.

After Correct Course approval:

```
/bmad-create-story 7-3b
```

---

## 12. Grep anchors (verification after implementation)

```bash
# Should return ZERO in Source/ (except comments/history)
rg 'bankUtilityLockBank|kLockBank|patchManagerBankLock|lockBankButton_|patchManagerLockBank'

# Should exist
rg 'bankUtilityUnlockBank|kUnlockBank|patchManagerBanksLocked|unlockBankButton_|UNLOCK'
```

---

## 13. Reference

- Oberheim: `_local/Documents/References/MD/oberheim-matrix-1000-midi-sysex-implementation.md` — §0AH, §0CH, master byte 165
- Implemented (to fix): Story 7.3 `PatchManagerActionHandler` toggle at `kLockBank`
- Deferred spike: `Documentation/Development/Plans/2026/06/2026-06-18-Matrix-1000-Bank-Lock-MIDI-Spike.md`
