---
title: 'FR user manual — module paste subtleties, Live Auto-Scale, Standalone ASIO/JACK'
type: 'chore'
created: '2026-11-24'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
baseline_commit: '21e4268ffe2118ab4af3ac7609f971247879ffb8'
context:
  - '{project-root}/_bmad/custom/manuel-utilisateur-redaction-fr.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The FR user manual still omits three user-visible facts: cross-module paste (LFO↔LFO and DCO) can leave some destination controls unchanged without a footer cue; Ableton Live on Windows can blur freely scalable VST3 UIs via Auto-Scale Plugin Window; Standalone Settings → AUDIO can list ASIO (Windows) and JACK (Linux) among driver options.

**Approach:** Update only `Documentation/User/Manuals/Matrix-Control-Manuel-Utilisateur.md` in plain user French: expand §8 `I`/`C`/`P` with expected paste behaviour (compatible `P` active vs grey incompatible; partial silent leave-unchanged for cross-LFO/DCO), replace « balance des oscillateurs » with on-screen `DCO 2 | DCO 1 MIX`, add Windows Live Auto-Scale guidance under §6 Ableton (and a short §15/`UI SCALE` cross-note if useful), and mention ASIO/JACK as possible Standalone audio interfaces by OS without promising every backend everywhere. FR only; no product code.

**Decisions:**
- FR only; EN later.
- Target file: `Documentation/User/Manuals/Matrix-Control-Manuel-Utilisateur.md`.
- Paste UX is often implicit (no footer for every subtlety): the manual must explain what to expect.
- Auto-Scale: Ableton guidance — disable Auto-Scale Plugin Window for freely resizable VST3s, then reload the device; Standalone is not affected the same way (host Windows Live issue, not Matrix-Control UI Scale).
- Prefer beta-guide FR wording for Live Auto-Scale; update ASIO/JACK to current product (available when the build/OS offers them — factual, no CI jargon).
- Out of scope: EN; PATCH NAME / skins / BANK UTILITY / m1kp except fixing a direct contradiction; product code; commit/PR unless asked.

</frozen-after-approval>

## Implementation Notes

- Placement: Auto-Scale primary under §6 Ableton Live (beta-guide Device title-bar steps); short `UI SCALE` cross-note in §15; troubleshooting row in §17.
- Paste: expanded §8 `I`/`C`/`P` — grey `P` = total incompatibility vs active `P` with silent leave-unchanged for cross-LFO/DCO; same-module paste copies all.
- Replaced « balance des oscillateurs » with on-screen `DCO 2 | DCO 1 MIX` (+ short mix gloss).
- Standalone AUDIO: `DRIVER TYPE` + ASIO (Windows) / JACK (Linux) as possible options by OS/build; §16 + troubleshooting cross-links.
- Blind Hunter follow-ups: fixed absolute « no remap » vs `KEYBD/PORTA`; firm never-footer / same-module all; UI labels; WAVE SELECT+NOISE; Device-view cue; « notamment »; ASIO/JACK/DRIVER TYPE lexicon; DCO SYNC/DETUNE bullet; English incompatible-paste quote.
- No product code; commit deferred per Intent (unless user asks).

## Review Triage Log

- Blind: absolute « sans traduire » vs DCO `KEYBD/PORTA` remap — medium — **patched**.
- Blind: `KEYBD/PORTA` behaviour missing — medium — **patched** (short adapt note).
- Blind: « souvent pas de message » — medium — **patched** (never dedicated footer).
- Blind: « en principe tous » — low — **patched** (firm all).
- Blind: French common-param words vs UI labels — medium — **patched**.
- Blind: `NOISE` without `WAVE SELECT` — medium — **patched**.
- Blind: Auto-Scale only under Ableton MIDI steps — low — **rejected** for relocate (Intent homes §6 Ableton + §15/§17); **patched** with cross-refs.
- Blind: « uniquement » Live Windows — low — **patched** (« notamment »).
- Blind: missing « en bas, vue Device » — medium — **patched**.
- Blind: lexicon ASIO/JACK/DRIVER TYPE/Auto-Scale — medium — **patched** ASIO/JACK/DRIVER TYPE; Auto-Scale lexicon entry **rejected** (explained in place; Ableton host term).
- Blind: §16 omits ASIO/JACK names — low — **patched**.
- Blind: DCO bullet sync for both vs SYNC on DCO 1 — medium — **patched**.
- Blind: grey `P` without English UI quote — medium — **patched**.
