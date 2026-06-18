---
organization: Ten Square Software
project: Matrix-Control
title: Product Requirements Document
author: BMad Agent
status: final
version: "1.1"
sources:
  - ../../briefs/brief-Matrix-Control-2026-05-22/brief.md
  - .decision-log.md
  - addendum.md
  - ../../reference-docs/oberheim/index.md
created: 2026-05-25
updated: 2026-06-06
---

# Product Requirements Document

## 0. Document Purpose

This PRD defines **what** Matrix-Control v1 must do for musicians and contributors. It is written for the product owner, future collaborators, and downstream BMad workflows (UX, Architecture, Epics).

**Sources:** Product brief v0.3, Phase 0 brownfield decision log (D-001–D-089), Oberheim reference docs (`_bmad-output/reference-docs/oberheim/`), Patch Mutator brainstorming (2026-05-29, D-082–D-087), and `addendum.md` for technical detail that does not belong in the FR narrative.

**Structure:** Glossary-anchored vocabulary; features grouped with globally numbered FRs; assumptions tagged inline and indexed in §9.

**Status:** **Final v1.1** — revised 2026-06-06 (Sprint Change Proposal: AD-11 / D-055-R — hosted plugin audio bus removed; standalone passthrough unchanged). Prior finalize 2026-05-29 (`review-rubric.md`). Open questions §9 deferred to Architecture or owner input.

---

## 1. Vision

Matrix-Control is a cross-platform JUCE **virtual instrument** and standalone application that gives Oberheim Matrix-1000 owners full, intuitive, real-time control over every synthesis parameter — from any major DAW or as a standalone editor.

It combines **two roles in one product**: play the hardware (MIDI notes/CC when the instrument track is armed) and **always-on bidirectional patch editing** (SysEx independent of track arming). One **instrument track** hosts Matrix-Control for MIDI editing and performance; a **separate audio track** monitors the synth hardware return — the standard cross-DAW pattern for external instruments. Standalone mode provides integrated audio input selection via `AudioDeviceManager`. This replaces the Ableton Live External Instrument + separate Max for Live editor workflow that frustrated users of the original editor.

The product promise is professional craftsmanship and **trust**: users must edit for hours without the Matrix-1000 hanging because the editor flooded it with MIDI. Everything else — pixel UI, Patch Manager, Mutator, open-source quality — serves that reliability.

Matrix-Control v1 ships **free (MIT)** with aspirational release Christmas 2026. Full Matrix-6/6R compatibility beyond PATCH mode is planned for v2.

---

## 2. Target User

### 2.1 Jobs To Be Done

- **Program deeply** — Understand and sculpt the Matrix-1000 without a front panel or a frustrating third-party editor.
- **Play and edit on one instrument track** — Sequence notes/CC and automate patch parameters on the Matrix-Control instrument track; synth audio monitored on a separate DAW audio track.
- **Manage patches** — Navigate internal banks, store to RAM, maintain a computer `.syx` library, explore variations via Patch Mutator.
- **Trust the tool** — Long sessions without synth hangs; clear feedback when MIDI fails.
- **Learn from the repo** *(secondary)* — JUCE developers study Clean Architecture, descriptors, tests, and docs.

### 2.2 Non-Users (v1)

- Matrix-6/6R owners needing **MASTER EDIT** or **SPLIT** — v2.
- Users without a connected Matrix-1000 expecting offline patch generation — plugin is a MIDI remote, not a synth emulator.
- iPad / remote-only workflows — out of scope.

### 2.3 Key User Journeys

**UJ-1. Alex sets up Ableton Live with one Matrix-1000 track.**

Alex owns a Matrix-1000 and Live 12 Suite. They create an instrument track with Matrix-Control, configure **MIDI From** / **MIDI To** to their interface ports, and arm the track. They add an **audio track** monitoring their interface input connected to the Matrix-1000 output. Device Inquiry succeeds; the UI unlocks. They play a clip — notes reach the synth; audio is heard on the audio track. They disarm the instrument track and tweak envelope segments — SysEx still flows; the editor LED activity confirms editing continues.

**UJ-2. Alex completes a four-hour programming session.**

Alex edits PATCH and MASTER parameters, uses Internal Patches STORE on bank 0, loads a `.syx` from their computer folder, and automates filter cutoff from the DAW. No synth hang attributable to plugin MIDI flooding. Footer messages appear only for actionable issues (ROM store attempt, filename mismatch policy applied silently per Settings).

**UJ-3. Alex builds a computer patch library.**

Alex opens a folder of factory `.syx` files via Computer Patches OPEN. Invalid files are counted and reported in the footer; valid files populate the combobox. They Save As with a sanitized 8-character filename injected into the patch SysEx name. On reload, reconciliation policy from Settings resolves filename vs internal name without modal friction.

**UJ-4. Alex explores Patch Mutator variations.**

Alex loads a patch from the synth. The **History M** combobox shows `<EMPTY>`; **History R** is disabled. Alex enables DCO and ENV toggles, sets Amount and Random, and presses **MUTATE** — **M00** appears in History M (sorted numerically), patch name shows `M00`, and full patch SysEx reaches the synth. Alex adjusts Random and presses **RETRY** on the same root — **R00** appears under History R; the sound restarts from the pre-M00 state with a new random draw. Alex selects **M02** in History M and **R01** in History R, then **MUTATE** again — **M03** is appended (other roots unchanged). **COMPARE** toggles the initial pre-mutation snapshot (blinking label, History combos grayed). **EXPORT** writes `Initial.syx` plus folders `M00/` … each containing `Mxx.syx` and `Mxx-Ryy.syx` files. When slot numbering is exhausted, a modal offers **Defrag** or **Cancel**; manual **Defrag** is also available in Settings.

---

## 3. Glossary

- **APVTS** — `AudioProcessorValueTreeState`; single Processor↔Editor channel for automatable parameters and ValueTree properties.
- **Editor path** — SysEx and Program Change patch selection; always active via dedicated MIDI thread; independent of DAW track arming.
- **Instrument path** — Note On/Off, Control Change, Pitch Bend from host or Keyboard From; obeys track arming; never sends SysEx or Program Change.
- **MIDI From** — Input port receiving SysEx from the synth return.
- **MIDI To** — Output port to connected Oberheim synth MIDI In (Matrix-1000, Matrix-6, or Matrix-6R).
- **Keyboard From** — Standalone: master keyboard port. Plugin: grayed, displays **Host**.
- **Audio From** — **Standalone only:** physical interface input via `AudioDeviceManager` and channel ComboBox. **Hidden in hosted plugin mode** (see FR-4, AD-11).
- **PATCH EDIT** — Ten synthesis modules (96 parameters) plus interactive displays and patch name.
- **MASTER EDIT** — Three modules (MIDI, Vibrato, Misc; 22 parameters).
- **PATCH MANAGER** — Bank Utility, Internal Patches, Computer Patches, Patch Mutator.
- **Mutation root (Mi)** — Root mutation entry in Patch Mutator history; displayed and named **M00–M99** (Matrix-style 0-based numbering).
- **Mutation retry (Mi-Rj)** — Retry entry under root **Mi**; named **Mxx-Ryy** (max 7 characters); **R00–R99** per root.
- **Initial snapshot** — Packed patch captured at load (before any MUTATE); auditioned via COMPARE; exported as `Initial.syx`; not a History combobox entry.
- **Property (APVTS)** — Non-automatable ValueTree state (Patch Manager buttons, toggles, enabled flags, skin, scale, ports).
- **StandaloneWidget** — UI control backed by APVTS property, not `AudioParameter`.
- **Descriptor** — Entry in `PluginDescriptors` defining parameters, widgets, SysEx offsets.
- **Packed patch buffer** — 134-byte Matrix-1000 patch payload; bytes 0–7 = patch name (6-bit ASCII chars).
- **Packed master buffer** — 172-byte Matrix-1000 master payload.
- **Unified outbound queue** — Single MIDI To consumer merging instrument and editor traffic; **notes, Control Change, and Pitch Bend dequeue before SysEx** (playing the synth takes priority over parameter edits).

---

## 4. Features

### 4.1 Device Connection & Startup

**Description:** On launch and on MIDI port change, the product confirms a compatible Oberheim unit (**Matrix-1000, Matrix-6, or Matrix-6R**). Without a valid device, the UI is locked and the footer guides recovery. Startup persistence restores user prefs but not patch parameters when the synth is connected (synth is source of truth). Realizes UJ-1.

#### FR-1: Device Inquiry at connection

The system performs Universal Device Inquiry when MIDI ports are configured, parses the Device ID reply, and reports detection via APVTS properties (`deviceDetected`, `deviceType`, `deviceVersion`).

**Consequences (testable):**
- Valid Oberheim Matrix family response sets `deviceDetected=true`, sets `deviceType` (Matrix-1000 / Matrix-6 / Matrix-6R) from Device ID member bytes, and displays firmware version in the **footer** right zone (FR-53).
- Matrix-1000, Matrix-6, and Matrix-6R are accepted for v1 PATCH workflows; Matrix-6/6R trigger MASTER EDIT graying (FR-46).
- Timeout or incompatible device sets `deviceDetected=false` and locks all editing actions.
- Member-byte parsing follows Oberheim reference docs (D-079); fix known `SysExConstants` member-byte order mismatch during implementation (D-080).

#### FR-2: UI lock without synth

When `deviceDetected=false`, PATCH EDIT, MASTER EDIT, MATRIX MODULATION, PATCH MANAGER actions, and module I/C/P are disabled; footer shows actionable guidance (cables, ports, power-cycle).

**Consequences (testable):**
- No SysEx or Program Change is sent while locked.
- Port reconfiguration re-triggers Device Inquiry.

#### FR-3: Startup persistence policy

The system persists plugin preferences (MIDI port IDs, skin, UI scale, Computer Patches folder path, Mutator recipe toggles/sliders, INIT template paths, unsaved-edit warning policy) across sessions.

**Consequences (testable):**
- Mutator HISTORY is **not** restored after session end.
- When device connected at load, patch parameter values are **not** restored from saved plugin XML — user must sync from synth or load file.

**Out of Scope:** Automatic patch recall from DAW preset when synth disconnected.

---

### 4.2 Dual-Role MIDI & Audio

**Description:** Virtual instrument replacing prior MIDI-effect registration. Instrument traffic and editor traffic merge through one outbound queue. Realizes UJ-1, UJ-2.

#### FR-4: Virtual instrument plugin category

The product registers as a virtual instrument (AU Music Device, VST3 Instrument, `IS_SYNTH TRUE`) with MIDI input, MIDI output, and stereo audio output.

**Hosted plugin:** no audio input bus. Synth audio return is managed on a **separate DAW audio track**.

**Standalone:** audio input via `AudioDeviceManager`; after Device Inquiry, **mono** channel selection for Matrix-1000 and **stereo** for Matrix-6 or Matrix-6R (FR-46 device-type rules apply to channel layout guidance).

**Consequences (testable):**
- Instantiates as instrument in Ableton Live 12, Reason 12, GarageBand 10 without manual MIDI-effect workarounds.
- Standalone input channel layout guidance updates when `deviceType` changes after port reconfiguration.

#### FR-4b: Hardware Latency

User can set **Hardware Latency** (0–100 ms, 0.1 ms step) representing analog round-trip delay: DAW → MIDI → synth → DAC/ADC → separate audio track (hosted) or physical monitor path (standalone).

**Consequences (testable):**
- Value converted to samples and reported to the host via `AudioProcessor::reportLatency()` (plugin delay compensation when recording external synth audio).
- Value persisted in `apvts.state` across sessions.
- User manual documents typical values (interface + Matrix-1000 analog path).

**Notes:** Primary use case is **hosted plugin** two-track workflow (FR-4, UJ-1). Control exposed in UI per FR-40 (Settings Plugin section when consolidated; interim header placement acceptable).

#### FR-5: Instrument path forwarding

When the DAW track is armed (plugin) or Keyboard From is active (standalone), Note On/Off, CC, and Pitch Bend are enqueued to MIDI To. Program Change and SysEx from the instrument path are stripped.

**Consequences (testable):**
- Disarmed track: no notes/CC forwarded; editor SysEx still operates (FR-6).

#### FR-6: Editor path always-on

SysEx and Program Change for patch editing and Patch Manager use the dedicated MIDI thread and unified outbound queue, independent of track arming.

**Consequences (testable):**
- User edits patch while track is muted/disarmed; changes reach synth via SysEx.

#### FR-7: Unified outbound queue

All MIDI To traffic passes through a single queue where **realtime messages (Note On/Off, Control Change, Pitch Bend) are dequeued before SysEx**, with minimum inter-SysEx delay enforced (see addendum). SysEx may wait briefly so live playing is never blocked by parameter floods.

**Consequences (testable):**
- Automated unit tests verify ordering and rate limit under synthetic load.
- Audio thread never blocks on MIDI I/O.

#### FR-8: Audio passthrough

**Standalone:** synth audio from the selected physical input passes to the plugin output with user-adjustable Input Gain and a **peak level indicator** (not a continuous level meter).

**Hosted plugin:** audio output bus is silent. Input Gain and peak indicator are **standalone-only** UI controls (AD-11).

**Consequences (testable):**
- Peak indicator uses a **solid fill colour** matching the envelope curve colour from the active skin/Look (same RGB as EnvelopeDisplay curves).
- Gain adjustment affects output level without blocking audio thread (standalone only).

#### FR-9: Activity LEDs

Header displays **Instrument** and **Editor** activity indicators driven by enqueue/dequeue on the unified queue.

**Consequences (testable):**
- LED state reflects recent traffic on each path (exact UX pattern defined in UX spec).

---

### 4.3 PATCH EDIT

**Description:** Ten modules, 96 automatable parameters (Int and Choice), interactive envelope and track displays, patch name. **APVTS is the single source of truth** for all synth parameter widgets — each value range and normalisation comes from `PluginDescriptors` (not uniformly 0–63). Realizes UJ-2.

#### FR-10: Parameter editing

Each PATCH Int or Choice parameter is exposed via APVTS and editable via custom widgets (Slider, ComboBox, NumberBox); changes trigger appropriate outbound SysEx per descriptor type (Remote Parameter Edit per addendum). Value ranges follow Oberheim packed patch encoding (6-bit, 7-bit signed/unsigned, choice enums — see addendum § Parameter value ranges).

**Consequences (testable):**
- DAW automation of PATCH parameters works with throttling consistent with FR-7.

#### FR-11: Interactive envelope displays

EnvelopeDisplay supports bidirectional editing: slider updates curve; mouse drag on curve writes APVTS directly (no Display→Slider bridge). Three envelope modules behave consistently.

**Consequences (testable):**
- Geometry and drag rules match addendum § EnvelopeDisplay.
- Visual markers are small filled squares (D-017).

#### FR-12: Interactive track generator display

TrackGeneratorDisplay supports bidirectional editing with five Y-only draggable points; geometry per addendum.

#### FR-13: Patch name display

Patch name shown in PATCH EDIT reflects bytes 0–7 of the current packed patch buffer / APVTS sync; editable per Matrix 8-character name rules.

---

### 4.4 MATRIX MODULATION

**Description:** Ten modulation buses with Source, Amount, Destination parameters plus section-level and per-bus init actions.

#### FR-14: Modulation bus editing

Each bus exposes three APVTS parameters; bus parameter change sends SysEx updating SOURCE, AMOUNT, and DESTINATION simultaneously for that bus.

#### FR-15: Matrix Mod init defaults

Section init and per-bus init reset buses to hardcoded defaults: SOURCE=NONE, AMOUNT=0%, DESTINATION=NONE per bus (no user INIT file for Matrix Mod v1).

#### FR-50: Matrix Mod bus reorder

User may **drag a bus number label** to reorder modulation bus **contents** visually: bus indices **0–9 remain fixed in the UI**; dragging permutes Source, Amount, and Destination values between the source row and drop target row (swap or move — UX detail in UX spec). Each completed reorder sends SysEx to update affected buses on the synth.

**Consequences (testable):**
- Reorder does not renumber buses — only reassigns parameter values between rows.
- Inactive buses (NONE/0%/NONE) can be grouped visually by the user without changing Matrix hardware bus numbering.

---

### 4.5 MASTER EDIT

**Description:** Three modules (MIDI, Vibrato, Misc), 22 parameters. Matrix-6/6R: entire MASTER EDIT section grayed (v1).

#### FR-16: Master parameter editing

Each MASTER parameter change sends a **complete** master SysEx message (opcode 0x03).

#### FR-17: Master module init confirmation

Init on a MASTER module (MIDI, Vibrato, Misc) requires user confirmation dialog before reset. PATCH module Init has no confirmation.

**Consequences (testable):**
- Confirmed init sends full master SysEx reflecting reset module parameters.

#### FR-18: Master file operations via Settings

Load Master from file, Save Master as, Save as default init (`MasterInit.syx`), Init all Master modules, and master library actions live on the **Settings** page — not in MASTER EDIT panel footer.

---

### 4.6 PATCH MANAGER — Bank Utility

**Description:** Banks 0–9 selection and BANK LOCK aligned with Matrix-1000 front-panel semantics (replaces legacy UNLOCK). Realizes UJ-2.

#### FR-19: Bank selection

User selects active bank via buttons 0–9; Core pushes `selectedBank` to APVTS; selected bank button shows red text.

#### FR-20: Bank lock

**BANK LOCK** toggles lock on the selected bank; locked bank shows red padlock icon on the button. Lock semantics match native synth behavior (not legacy M4L UNLOCK inversion).

#### FR-21: Bank selection exclusivity

Internal Patches does **not** provide a separate bank NumberBox — bank selection is only via Bank Utility.

---

### 4.7 PATCH MANAGER — Internal Patches

**Description:** Patch navigation, INIT/COPY/PASTE/STORE on internal memory. ROM banks 2–9 restrict write operations.

#### FR-22: Patch navigation

User navigates patch number via `<` / `>` and editable patch NumberBox; navigation wraps across banks when no bank is locked (patch 99 → patch 00 next bank).

#### FR-23: ROM gating

PASTE and STORE are disabled (grayed) on ROM banks 2–9; footer warning on attempted use or hover.

#### FR-24: Internal patch operations

INIT, COPY, PASTE, STORE invoke Core via ActionDispatcher; COPY snapshots current APVTS patch buffer; STORE sends patch to synth memory per Matrix-1000 rules for RAM banks 0–1.

#### FR-51: Unsaved patch edit warning

When the user has **unsaved edits** to the current patch (APVTS differs from last loaded/s synced snapshot) and attempts to load a different patch from **internal memory** or a **Computer Patches** file, the system shows a **confirmation dialog** warning that edits were not stored to the synth or saved as `.syx`.

**Consequences (testable):**
- User may cancel (stay on current edit) or confirm (discard and load).
- **Settings** policy (FR-40): warn always (default) or never warn (expert mode).
- Does not block silent operations covered elsewhere (e.g. bulk import reconciliation without modal per D-025).

---

### 4.8 PATCH MANAGER — Computer Patches

**Description:** Folder-based `.syx` library with scan, combobox navigation, **Previous/Next file** buttons, Save/Save As. Realizes UJ-3.

#### FR-25: Folder scan

OPEN selects a folder; system scans `.syx` files, validates structure, counts valid vs invalid files in footer.

#### FR-26: Combobox states

Combobox displays sentinel states `<EMPTY>` (grayed, no folder/empty/invalid path) and `<SELECT>` (folder populated, no file chosen yet) — not real list entries.

#### FR-27: Folder persistence

Last folder path persists; on relaunch folder is rescanned without auto-loading a patch or sending SysEx.

#### FR-28: Save injects validated filename

Save/Save As writes `.syx` with sanitized 8-character filename injected into patch name bytes.

#### FR-29: Name reconciliation on import

On load, if internal patch name and filename match after sanitization, display as-is. If mismatch, apply user Settings policy (default: prefer internal name) without mandatory modal; optional footer notice.

**Notes:** `[NOTE FOR PM]` Reconciliation policy options documented in Settings FR-40.

#### FR-52: Computer Patches file navigation

When a valid folder is loaded and the combobox lists `.syx` files, **Previous Patch File** (`<`) and **Next Patch File** (`>`) buttons cycle circularly through the sorted file list and load the selected file (same validation and SysEx behaviour as combobox selection).

**Consequences (testable):**
- Buttons disabled in `<EMPTY>` / `<SELECT>` combobox states.
- Wraps from last file to first and reverse.

---

### 4.9 PATCH MANAGER — Patch Mutator

**Description:** Absynth-inspired mutation workflow: Amount/Random, ten module toggles, **MUTATE**, **RETRY**, two-level session **History** (M + R comboboxes), **COMPARE**, **DELETE**, **CLEAR**, **EXPORT**, and **Defrag** (Settings + limit modal). Realizes UJ-4. Algorithm and UX per brainstorming 2026-05-29 (D-082–D-087; addendum § Patch Mutator).

#### FR-30: Full patch SysEx on mutation actions

Each successful **MUTATE**, **RETRY**, and History selection audition sends a **complete** patch SysEx (opcode 0x01), not per-parameter Remote Parameter Edit.

#### FR-31: Session-only history

Mutator history (all Mi and Mi-Rj entries plus internal parent snapshots) exists only for the current session. History is **cleared** on new patch load from synth or `.syx` file; not persisted in plugin state. Amount/Random/toggles persist separately (FR-34).

#### FR-32: Compare mode

**COMPARE** toggles audition of the **initial snapshot** (patch as loaded, before any MUTATE). Active compare: label blinks (UX spec), **History M** and **History R** grayed, no synth bank STORE. Re-toggle exits compare and restores the previously selected History entry; PATCH NAME follows.

**Consequences (testable):**
- COMPARE disabled when History is empty (no mutations yet).

#### FR-33: Manual export layout

**EXPORT** prompts for a destination folder and writes:

- **`Initial.syx`** at export root (initial snapshot).
- One subfolder per root **`Mxx/`** containing **`Mxx.syx`** (root mutation) and **`Mxx-Ryy.syx`** for each retry (self-describing filenames if moved outside folder structure).

Patch name bytes in each file match display names (`M05`, `M05-R02`, etc.). No auto-export on each mutation (D-027).

#### FR-34: Recipe persistence

Amount, Random sliders and module enable toggles persist across sessions via APVTS properties, independent of mutation History.

#### FR-54: Two-level History comboboxes

**History M** lists root mutations **M00–M99** sorted **numerically ascending** (not creation-order insertion). Sentinel **`<EMPTY>`** when no roots exist.

**History R** lists retries for the selected root: sentinel **`—`** (root only, audition **Mi**), then **R00–R99** for that **Mi**. Disabled or empty when History M is `<EMPTY>`.

Changing **M** updates **R** for that root. No `<` `>` navigation buttons (D-026).

#### FR-55: MUTATE semantics

**MUTATE** mutates from the **currently auditioned** patch: selected **Mi** alone, or **Mi-Rj** if an R entry is selected.

Creates a **new root** assigned the next index **`max(existing Mi) + 1`** in **00–99**, preserving **gaps** from prior DELETE operations. Display re-sorts History M numerically. Other existing roots are **never** removed or reordered by MUTATE.

**Consequences (testable):**
- First mutation names **M00** (not M01).
- PATCH NAME and packed buffer name bytes updated to **`Mxx`**.
- Disabled when all **100** root slots **M00–M99** are allocated.

#### FR-56: RETRY semantics

**RETRY** applies to the selected History entry (root **Mi** or **Mi-Rj**). Generates a new retry **Mi-R(j+1)** under the **same root Mi**, assigned **`max(existing R for Mi) + 1`**, preserving gaps.

Mutation input = **`parentSnapshot`** stored for the selected entry (parameter state immediately **before** that entry was created) — **not** cumulative from the selected result. Existing retries are **never** deleted.

**Consequences (testable):**
- First retry of **M05** names **M05-R00**.
- PATCH NAME shows **`Mxx-Ryy`** (max 7 characters).
- Disabled when History empty; disabled when **100** retries **R00–R99** exist for the current **Mi**.

#### FR-57: History selection audition

Selecting an entry in **History M** or **History R** sends full patch SysEx after a **debounce** (same pattern as Computer Patches combobox — FR-52) to avoid SysEx spam on rapid scrolling.

#### FR-58: DELETE and CLEAR

**DELETE** removes the selected entry. Deleting root **Mi** **cascades** all **Mi-R*** for that root (footer notice). Deleting **Mi-Rj** removes only that retry; number gaps remain.

After DELETE: selection moves to the previous list entry, or History becomes empty (COMPARE disabled; audition initial snapshot).

**CLEAR** purges all History; History M = `<EMPTY>`; COMPARE disabled.

#### FR-59: Defrag (renumber history)

When **MUTATE** or **RETRY** cannot allocate the next index (**M00–M99** or **R00–R99** full due to gap exhaustion), the system shows a **modal**: explains numbering limit, offers **Defrag** or **Cancel**.

**Defrag** renumbers existing roots contiguously **M00…** and retries per root **R00…**, updates patch names, preserves logical entry content; **number gaps are lost**. Footer confirms completion.

**Manual Defrag:** Settings page exposes **Defrag mutation history** (same engine, confirmation required). Disabled when History empty. Not shown on main Patch Mutator panel (space + safety — D-087).

#### FR-60: Mutator enabled states

**RETRY** enabled when History is non-empty. **MUTATE** enabled when a root **Mi** is selected (with or without **R**). **EXPORT** enabled when History is non-empty. **DELETE** enabled when an entry is selected. Limit and Defrag cases produce footer messages when actions are blocked.

---

### 4.10 Module Init / Copy / Paste

**Description:** Per-module I/C/P in PATCH and MASTER headers; type-aware clipboard; Init templates. Realizes UJ-2.

#### FR-35: Module copy/paste

Module Copy always enabled; Paste enabled only on targets compatible with the last Copy source. Last Copy (module, full patch, or matrix modulation) defines clipboard mode.

**Consequences (testable):**
- Full-patch Copy grays all PATCH EDIT Paste buttons; Internal Patches Paste remains active.
- **Matrix Modulation Copy** grays all PATCH EDIT and MASTER module Paste buttons; only Matrix Modulation Paste remains active.
- Compatibility matrix per addendum § Clipboard (ENV full interchange; DCO/LFO partial rules; **Matrix Modulation full-matrix interchange**).
- Matrix Modulation clipboard snapshot: all 10 buses (source + amount + destination per bus); excludes patch name and all other PATCH parameters.

#### FR-36: Patch init sources

PATCH module Init and Internal Patches INIT load user `PatchInit.syx` from Settings folder if present, else hardcoded defaults. MASTER module Init uses `MasterInit.syx` or hardcoded master defaults.

#### FR-37: Init template separation

`PatchInit.syx` / `MasterInit.syx` are loaded via InitTemplateLoader — not through Computer Patches combobox; no filename reconciliation dialog for init templates.

#### FR-38: Master init confirmation

See FR-17; PATCH Init requires no confirmation.

**Notes:** `[DEFER]` ENV shape-only copy via header `[<n]` buttons — candidate feature after Figma (D-061); not in v1 module clipboard scope.

---

### 4.11 Header, Settings & Persistence

#### FR-39: Header routing controls

Header exposes MIDI From, MIDI To, Keyboard From, and Instrument/Editor activity LEDs in all build contexts. **Audio From**, **Input Gain**, and **peak level indicator** are exposed in **standalone only** (hidden in hosted plugin mode — AD-11). Firmware and device identity are **not** in the header (see FR-53).

#### FR-40: Settings page

Settings consolidates skin (if not moved to logo popup), UI scale (if not moved), **Hardware Latency** (FR-4b), Computer Patches reconciliation policy, **unsaved patch edit warning policy**, **Defrag mutation history** (FR-59), Master file/library/init actions, INIT template paths, optional logging opt-in (NFR), and future power-user options.

#### FR-41: Logo popup layout *(if Figma approved — D-014a)*

Clickable MATRIX-CONTROL logo opens popup for Skin + UI Scale, freeing header space for routing controls.

#### FR-42: Skin persistence

Black and Cream skins available v1; user skin choice persists via APVTS property.

#### FR-43: UI scale

Presets 50%, 75%, 100%, 125%, 150%, 175%, 200% with layout recalculated per scale (no global AffineTransform blur).

---

### 4.12 GUI Shell & Messaging

#### FR-44: Footer messaging

Non-blocking footer displays errors, warnings, and informational messages (`uiMessageText` / severity via ExceptionPropagator). Grayed-button clicks produce footer feedback, not modals.

#### FR-53: Footer device identity

Footer **right zone** displays detected **device type** (Matrix-1000 / Matrix-6 / Matrix-6R) and **firmware version** string from Device Inquiry (FR-1). Hidden or placeholder when `deviceDetected=false`.

#### FR-45: Graying rules

Controls grayed only where required: LEGATO PORTA constraints, ROM PASTE/STORE, device gating, Keyboard From in plugin, Mutator/compare states, Paste incompatibility.

#### FR-46: Matrix-6 PATCH-only mode

Matrix-6/6R detected or selected: MASTER EDIT grayed; PATCH EDIT, MATRIX MOD, PATCH MANAGER active; same PATCH SysEx as Matrix-1000.

---

### 4.13 Action Dispatch & Parameter Sync

#### FR-47: ActionDispatcher hub

StandaloneWidget actions (timestamps) route through PluginProcessor ValueTree listener → ActionDispatcher → specialized handlers (Module, PatchManager, Mutator). No business logic in GUI panels beyond `setProperty`.

#### FR-48: APVTS parameter → SysEx

PluginProcessor (or delegated services) listens to APVTS parameter changes and enqueues appropriate SysEx without resyncing sliders/displays manually.

#### FR-49: PatchModel / MasterModel

Core maintains packed buffers synced with APVTS via descriptor-driven mappers; no parallel offset tables.

---

## 5. Non-Goals (Explicit)

- Matrix-6/6R MASTER EDIT and SPLIT — v2.
- GROUPS multi-unit Master mode — not planned.
- Request All Bank bulk dump — not planned.
- OpenGL GUI pipeline — abandoned.
- Paid tier / Oberheim branding in UI — never for v1.
- Offline patch editing without hardware — not v1.
- Auto-export on every Mutator mutation — rejected v1.
- Split-patch SysEx (opcode 0x02) — discarded v1.

---

## 6. v1 Scope

### 6.1 In Scope

Full ambitious v1 per brief § Scope: virtual instrument dual-role, PATCH + MASTER + MATRIX MOD (incl. bus reorder FR-50) + full PATCH MANAGER incl. **Patch Mutator** (FR-30–FR-34, FR-54–FR-60), module I/C/P, type-aware clipboard, automation with throttling, device diagnostics (multi-model Device Inquiry), Black/Cream skins, UI scale 50–200%, user manual EN/FR, MIT open source, official DAW test matrix.

### 6.2 Out of Scope for v1

| Item | Reason / target |
|---|---|
| UI scale presets >200% | v2 candidate (D-013, D-065) |
| User-custom skins (JSON/XML) | v2 |
| Patch Browser (tags, search, favourites, drag-and-drop collections) | v2 — Serum-style patch library UX (D-077); replaces legacy "librarian" wording |
| ENV shape-only copy buttons | Deferred pending Figma (D-061) |
| TestComponent in release builds | Debug only (D-063) |

---

## 7. Success Metrics

**Primary**

- **SM-1:** 4-hour continuous hardware editing session without synth hang attributable to plugin SysEx flooding. Validates FR-7, FR-10, FR-48.
- **SM-2:** Instrument-track Live workflow — play + edit with instrument track disarmed for SysEx; synth audio on separate audio track. Validates FR-5, FR-6, UJ-1.
- **SM-3:** Complete Matrix-1000 PATCH + MASTER bidirectional editing. Validates FR-10, FR-16.

**Secondary**

- **SM-4:** Computer patch Save/Load reliability (M4L pain point closed). Validates FR-25–FR-29, FR-52, UJ-3.
- **SM-5:** Beta tester positive feedback (Jeremy Bernstein + M4L community). Validates overall UX.
- **SM-6:** CI Core unit tests green on macOS for SysEx round-trip, PatchModel packing, ClipboardService, ActionDispatcher routing mocks. Validates NFR-1.
- **SM-7:** Patch Mutator exploratory session — MUTATE/RETRY chains, COMPARE, EXPORT folder layout, Defrag at limit. Validates FR-30–FR-34, FR-54–FR-60, UJ-4.

**Counter-metrics (do not optimize)**

- **SM-C1:** Feature count / lines of code — do not ship half-working modules to inflate scope.
- **SM-C2:** SysEx messages per second — minimizing count matters; maximizing throughput does not.

---

## 8. Cross-Cutting Non-Functional Requirements

#### NFR-1: Automated test pyramid

Unit tests for SysEx round-trip, checksum, `.syx` validation, PatchModel/MasterModel 134/172 packing, ClipboardService compatibility, validators, ActionDispatcher routing (mocked), InitDefaults, queue rate logic, prefs persistence (temp files). GUI pixel tests manual. No hardware in CI. See D-047.

#### NFR-2: SysEx reliability timings

Minimum delay between outbound SysEx messages enforced on unified queue; delay profile selected from **Device Inquiry firmware / EPROM class** (stock Oberheim vs Tauntek/Gligli/Nordcore optimised — see addendum § SysEx delay profiles). DAW automation throttling prevents EPROM overload. Stock-firmware overload under heavy automation documented as user responsibility in manual.

#### NFR-3: Thread safety

No SysEx or blocking MIDI on audio thread; APVTS GUI updates via message thread; inbound MIDI decode never directly repaints.

#### NFR-4: Descriptor-driven architecture

Single source of truth in PluginDescriptors; WidgetFactory + ApvtsFactory; boot validators; no duplicated SysEx offset tables.

#### NFR-5: Code quality limits

Functions ≤15 lines, classes ≤200 lines, explicit names, no magic numbers — per project `.cursorrules`.

#### NFR-6: Documentation

User manual English + French; public technical docs maintained; descriptor stack cheat sheet per Architecture.

#### NFR-7: Logging (release)

ApvtsLogger / MidiLogger enableable in release via Settings opt-in; levels and privacy framed in Architecture (D-041).

#### NFR-8: Platforms & formats

macOS (AU + VST3 + Standalone), Windows (VST3 + Standalone), Linux (VST3 + Standalone); primary validation DAW: Ableton Live 12 Suite.

---

## 9. Open Questions

1. **Header logo popup** — Figma approval for D-014a vs inline Skin/Scale combos.
2. **SysEx automation throttle** — Exact ms curves under heavy DAW automation per EPROM profile (Architecture + hardware test).
3. **Matrix bus SysEx opcode** — Confirm 0x0B vs block 0x06 at implementation on Matrix-1000; verify Matrix-6/6R support (D-044).
4. **INIT hardcoded defaults** — Owner to supply reference values for `InitDefaults` (non-blocking).
5. **ENV shape copy** — v1 vs v1.x after Figma (D-061).
6. **Matrix-6/6R Device ID member bytes** — Confirm `<memb-lo>` / `<memb-hi>` on hardware; not in community M-6 doc (D-079).
7. **Patch Mutator algorithm** — Amount/Random curve and per-module mutation rules (Architecture + `PatchMutatorEngine`; UX/naming/history resolved D-082–D-087).

---

## 10. Assumptions Index

- `[ASSUMPTION]` Christmas 2026 release is aspirational, not a fixed deadline.
- `[ASSUMPTION]` Tauntek EPROM recommended but stock firmware supported with documented automation limits.
- `[ASSUMPTION]` Primary owner test platform: Ableton Live 12 Suite on macOS Tahoe.
- `[ASSUMPTION]` SysEx inter-message baseline **10 ms** for Matrix-1000 (Oberheim official); Matrix-6/6R community spec cites **20 ms** — Architecture validates per `deviceType`.
- `[ASSUMPTION]` Patch Mutator **history UX and naming** specified in FR-54–FR-60; mutation **algorithm** (Amount/Random application) defined in Architecture / Core epic 6.

---

*End of PRD v1.1 — status `final`. Next: `bmad-ux` (header conditional layout), `bmad-create-story` Epic R, or `bmad-dev-story` R-1.*
