# Parking — Aide contextuelle footer (post Patch Mutator playable)

**Status:** idea parked — not in current playable-calibration implementation scope  
**Origin:** brainstorm `brainstorm-patch-mutator-playable-calibration-2026-08-26` (août 2026)  
**Product intent:** Ableton Live–style Info line (not floating tooltips)

## Footer anatomy (current code / layout)

The footer is three bands aligned with the three body columns (`FooterPanel`: `leftBand` / `centreBand` / `rightBand`):

| Band (code) | Under column | What it shows today | Proposed product name (FR / EN) |
|-------------|--------------|---------------------|----------------------------------|
| **Left** | Patch Edit width | INFO / WARNING / ERROR (+ message text) via `uiMessageText` / `uiMessageSeverity` | **Bande des messages** / **Status message band** |
| **Centre** | Shared (Matrix Mod + Patch Manager) | MIDI queue pressure alert only (when active) | **Bande système** / **System alert band** (today); candidate home for **Aide contextuelle** later if redesigned |
| **Right** | Master Edit width | DEVICE + type / firmware or NOT CONNECTED | **Bande appareil** / **Device status band** |

**Note:** User-facing brainstorm sometimes called the message area “centre”; implementation and user manual place **status messages on the left**, device on the right. Naming must follow the real layout.

## Proposed interaction (agreed direction)

- **Bandeau d’état** = sticky layer (action results, warnings, errors) — remains stored.  
- **Aide contextuelle** = ephemeral display while pointer **or keyboard focus** is on a control; on leave/blur, restore sticky status.  
- Help must **not** erase sticky state in APVTS — display overlay only.  
- Priority: system/errors > sticky action messages > hover/focus help.  
- First slice: Patch Mutator C / D / F / E full names; later optional extension to all Patch Edit params (beginners).

## Out of scope for now

- Implementation story / create-story  
- Renaming identifiers in code (do when a dedicated UX story lands)  
- Changing which band paints what (design choice still open: help overlays **left** vs moves to **centre**)

## Related

- `_bmad-output/brainstorming/brainstorm-patch-mutator-playable-calibration-2026-08-26/intention-patch-mutator-jouable.md`  
- `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp`  
- `Documentation/Users/manuel-utilisateur.md` § vue d’ensemble « En bas »
