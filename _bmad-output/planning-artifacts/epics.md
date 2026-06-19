---
stepsCompleted: [1, 2, 3, 4]
status: complete
completedAt: 2026-05-29
inputDocuments:
  - prds/prd-Matrix-Control-2026-05-25/prd.md
  - prds/prd-Matrix-Control-2026-05-25/addendum.md
  - prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - project-context.md
---

# Matrix-Control - Epic Breakdown

## Overview

This document provides the complete epic and story breakdown for Matrix-Control, decomposing the requirements from the PRD, UX Design if it exists, and Architecture requirements into implementable stories.

## Requirements Inventory

### Functional Requirements

FR-1: Device Inquiry at connection — Universal Device Inquiry on port configure; parse Device ID; set `deviceDetected`, `deviceType`, `deviceVersion`; footer firmware display.
FR-2: UI lock without synth — When `deviceDetected=false`, disable editing actions; footer guidance; no outbound MIDI; re-inquiry on port change.
FR-3: Startup persistence policy — Persist plugin prefs (ports, skin, scale, folders, Mutator recipe, INIT paths, warning policy); do not restore patch params when device connected; do not restore Mutator history.
FR-4: Virtual instrument plugin category — AU/VST3/Standalone instrument with MIDI in/out and stereo audio output. **Hosted plugin:** no audio input bus; synth return on separate DAW audio track (AD-11). **Standalone:** physical input via AudioDeviceManager; mono channel layout for M-1000, stereo for M-6/6R after inquiry.
FR-4b: Hardware Latency — User sets analog round-trip delay (ms); value reported via `reportLatency()` and persisted in apvts.state (Epic R).
FR-5: Instrument path forwarding — When armed/active keyboard, enqueue Note/CC/PB to MIDI To; strip Program Change and SysEx from instrument path.
FR-6: Editor path always-on — SysEx and Program Change for editing via dedicated thread and unified queue regardless of track arming.
FR-7: Unified outbound queue — Single queue; realtime messages before SysEx; inter-SysEx delay enforced; audio thread never blocks on MIDI I/O.
FR-8: Audio passthrough — **Standalone:** physical input to output with Input Gain and peak indicator (solid fill matching envelope curve colour). **Hosted plugin:** silent output; Input Gain and peak standalone-only (AD-11).
FR-9: Activity LEDs — Header Instrument and Editor indicators driven by unified queue traffic.
FR-10: Parameter editing — PATCH Int/Choice via APVTS and custom widgets; descriptor-driven ranges; Remote Parameter Edit SysEx; DAW automation with FR-7 throttling.
FR-11: Interactive envelope displays — Bidirectional EnvelopeDisplay; direct APVTS writes from curve drag; no Display→Slider bridge; three modules consistent.
FR-12: Interactive track generator display — TrackGeneratorDisplay with five Y-only draggable points per addendum geometry.
FR-13: Patch name display — Reflect bytes 0–7 of packed patch buffer; editable per 8-character Matrix rules.
FR-14: Modulation bus editing — Three APVTS params per bus; change sends SysEx updating SOURCE, AMOUNT, DESTINATION together.
FR-15: Matrix Mod init defaults — Section and per-bus init reset to SOURCE=NONE, AMOUNT=0%, DESTINATION=NONE (hardcoded v1).
FR-50: Matrix Mod bus reorder — Drag bus label to permute Source/Amount/Destination values between rows; indices 0–9 fixed; SysEx on completed reorder.
FR-16: Master parameter editing — Each MASTER change sends complete master SysEx (opcode 0x03).
FR-17: Master module init confirmation — MASTER module Init requires confirmation dialog; PATCH Init does not.
FR-18: Master file operations via Settings — Load/Save Master, Save as default init, Init all, library actions on Settings page only.
FR-19: Bank selection — Buttons 0–9 set active bank; red text on selected bank button.
FR-20: Bank lock — UNLOCK sends `0CH`; bank select sends `0AH` (locks); `patchManagerBanksLocked` mirrors hardware; red dot on Internal Patches bank NumberBox when locked.
FR-21: Bank selection exclusivity — No separate bank NumberBox in Internal Patches; bank via Bank Utility only.
FR-22: Patch navigation — `<` / `>` and NumberBox with wrap across banks when unlocked.
FR-23: ROM gating — PASTE/STORE disabled on ROM banks 2–9 with footer warning.
FR-24: Internal patch operations — INIT/COPY/PASTE/STORE via ActionDispatcher; COPY snapshots APVTS patch buffer; STORE to synth RAM rules.
FR-51: Unsaved patch edit warning — Confirm dialog when navigating away with unsaved edits (internal or Computer Patches); Settings policy always/never warn.
FR-25: Folder scan — OPEN folder; scan `.syx`; validate; footer valid/invalid counts.
FR-26: Combobox states — `<EMPTY>` and `<SELECT>` sentinel states for Computer Patches combobox.
FR-27: Folder persistence — Last folder path persists; rescan on relaunch without auto-load or SysEx.
FR-28: Save injects validated filename — Save/Save As writes `.syx` with sanitized 8-char name in patch bytes.
FR-29: Name reconciliation on import — Match/mismatch policy per Settings; default prefer internal name; optional footer notice.
FR-52: Computer Patches file navigation — Previous/Next file buttons cycle sorted list circularly; same load behaviour as combobox.
FR-30: Full patch SysEx on mutation actions — MUTATE, RETRY, History audition send complete patch SysEx (0x01).
FR-31: Session-only history — Mutator history RAM-only; cleared on patch load; recipe sliders/toggles persist separately (FR-34).
FR-32: Compare mode — COMPARE toggles initial snapshot audition; blinking label; History grayed; re-toggle restores selection.
FR-33: Manual export layout — EXPORT writes `Initial.syx` at root plus `Mxx/` folders with `Mxx.syx` and `Mxx-Ryy.syx`.
FR-34: Recipe persistence — Amount, Random, module toggles persist via APVTS across sessions.
FR-54: Two-level History comboboxes — History M: M00–M99 numeric sort; History R: `—` or R00–R99 per selected Mi; no `<` `>` nav.
FR-55: MUTATE semantics — New root max(Mi)+1 with gaps preserved; first root M00; PATCH NAME updated; disabled at 100 roots.
FR-56: RETRY semantics — New retry max(R)+1 under same Mi from parentSnapshot; first retry R00; disabled at 100 retries per Mi.
FR-57: History selection audition — Full patch SysEx after debounce on M or R selection.
FR-58: DELETE and CLEAR — DELETE cascades Mi-R* when deleting root; gaps remain; CLEAR purges all history.
FR-59: Defrag (renumber history) — Modal on limit exhaustion; manual Defrag in Settings; contiguous renumber M/R.
FR-60: Mutator enabled states — Enable/disable rules for RETRY, MUTATE, EXPORT, DELETE; footer when blocked.
FR-35: Module copy/paste — Type-aware clipboard; Paste enabled only on compatible modules; full-patch Copy grays PATCH Paste.
FR-36: Patch init sources — PATCH Init from `PatchInit.syx` or hardcoded; MASTER from `MasterInit.syx` or hardcoded.
FR-37: Init template separation — Init templates via InitTemplateLoader; not via Computer Patches combobox.
FR-38: Master init confirmation — See FR-17.
FR-39: Header routing controls — MIDI From/To, Keyboard From, activity LEDs always; Audio From, Input Gain, peak **standalone only** (AD-11); not device identity.
FR-40: Settings page — Skin, scale, reconciliation policy, unsaved warning, Defrag, Master file actions, INIT paths, logging opt-in.
FR-41: Logo popup layout — Clickable logo opens Skin + UI Scale popup if Figma approved (D-014a).
FR-42: Skin persistence — Black and Cream skins v1; choice persists via APVTS.
FR-43: UI scale — Presets 50–200%; ScaledLayout recalculation; no global AffineTransform.
FR-44: Footer messaging — Non-blocking `uiMessageText` via ExceptionPropagator; grayed-click footer feedback.
FR-53: Footer device identity — Footer right zone shows device type and firmware from FR-1; hidden when not detected.
FR-45: Graying rules — LEGATO PORTA, ROM, device gating, Keyboard From plugin, Mutator/compare, Paste incompatibility.
FR-46: Matrix-6 PATCH-only mode — M-6/6R: MASTER EDIT grayed; PATCH/Matrix Mod/PM active; BANK UTILITY disabled; internal navigation 00–99 only (no bank concept).
FR-47: ActionDispatcher hub — StandaloneWidget timestamps → ActionDispatcher → Module/PatchManager/Mutator handlers; panels setProperty only.
FR-48: APVTS parameter → SysEx — Processor/services listen and enqueue SysEx without manual widget resync.
FR-49: PatchModel / MasterModel — Packed 134/172 B buffers synced via descriptor mappers; no parallel offset tables.

### NonFunctional Requirements

NFR-1: Automated test pyramid — Unit tests for SysEx round-trip, checksum, `.syx` validation, PatchModel/MasterModel packing, ClipboardService, validators, ActionDispatcher mocks, InitDefaults, queue rate, prefs persistence; no hardware in CI.
NFR-2: SysEx reliability timings — Inter-SysEx delay on unified queue; profile from Device Inquiry EPROM class; DAW automation throttling.
NFR-3: Thread safety — No SysEx/blocking MIDI on audio thread; APVTS GUI updates on message thread; inbound MIDI never direct repaint.
NFR-4: Descriptor-driven architecture — Single SSOT in PluginDescriptors; WidgetFactory + ApvtsFactory; boot validators; no duplicate offset tables.
NFR-5: Code quality limits — Functions ≤15 lines, classes ≤200 lines, explicit names, no magic numbers per `.cursorrules`.
NFR-6: Documentation — User manual EN+FR; public technical docs; descriptor stack cheat sheet.
NFR-7: Logging (release) — ApvtsLogger/MidiLogger enableable in release via Settings opt-in; levels and privacy per D-041.
NFR-8: Platforms & formats — macOS AU+VST3+Standalone; Windows/Linux VST3+Standalone; primary DAW validation Ableton Live 12 Suite.

### Additional Requirements

- **Brownfield baseline (AD-1):** Extend tag `v0.0.66-alpha-pre-bmad`; no greenfield starter; preserve descriptor stack, validators, SysEx parser/encoder, existing GUI panels.
- **Composition root (AD-2):** PluginProcessor owns ~12 Core services via unique_ptr; construction order per D-058.
- **Epic E0 first (AD-9, P-001):** Execute tree migration before E1 new Core files — `Source→src`, `Assets→assets`, `Logs→logs`, `Documentation→docs`, `Quality/Tests→tests`, `Builds→builds`, personal → `_local/`; internal Core/GUI/Shared PascalCase preserved.
- **Runtime project paths (AD-10, D-093):** Replace CMake-baked `MATRIX_CONTROL_PROJECT_ROOT` with `ProjectPaths` runtime discovery; logs under `{root}/logs/midi/` and `logs/apvts/`.
- **Logs not versioned (D-094):** Runtime log files gitignored; never commit `logs/`.
- **MIDI queue (AD-3):** InstrumentMidiForwarder + EditorPath producers; MidiManager consumer; realtime before SysEx; SysExDelayProfile from inquiry.
- **Audio bus by host context (AD-11):** Hosted plugin = no input bus, silent output; standalone = passthrough + gain + peak. Epic R implements revision (Sprint Change Proposal 2026-06-06).
- **APVTS→SysEx routing (AD-4):** PATCH 0x06; MASTER 0x03; Matrix Mod block (opcode TBD §9); full buffer 0x01 for STORE/load/Mutator.
- **ActionDispatcher handlers (AD-5):** ModuleActionHandler, PatchManagerActionHandler, MutatorActionHandler; debounced History selection.
- **PatchMutatorEngine (AD-6):** MutationHistoryStore two-level M/R; MutationNaming; HistoryDefragService; MutationAlgorithm spec deferred to E6 story (PRD §9 #7).
- **Persistence boundaries (AD-7):** Prefs in session XML; patch params not restored when device connected; Mutator history session RAM only.
- **Testing strategy (AD-8):** Mirror Core structure under `tests/`; Catch2 or existing framework; SM-1 hardware gate manual.
- **Implementation patterns (Step 5):** Core folder layout, APVTS event/state/uiMirror taxonomy, anti-patterns (no GUI SysEx, no duplicate offsets, no global AffineTransform).
- **Critical path (D-058):** E1 PatchModel → E2 SysEx/queue → E3 Init → E4 PatchFileService → E5 Clipboard → E6 Mutator → E7 ActionDispatcher → E8 DeviceTypeRegistry → E9 DirtyPatchTracker → E10 Display sync refactor.
- **Open spikes routed to epics:** SysEx automation throttle (E2); Matrix Mod opcode (E2 hardware); INIT hardcoded defaults (E3, owner input); M-6 member bytes (E8 hardware); MutationAlgorithm (E6 spec story).
- **Refactors in scope:** MidiManager queue wiring; PluginProcessor SRP split; remove InteractiveDisplayApvtsSync bridge (E10); fix SysExConstants member-byte order (D-080).

### UX Design Requirements

No dedicated UX Design Specification document found under `{planning_artifacts}`. UX requirements are embedded in PRD FRs, PRD addendum (EnvelopeDisplay, TrackGenerator, Patch Mutator, Clipboard matrix), and PRD §9 open item #1 (logo popup / Figma). Optional follow-up: `bmad-ux` workflow before E7 panel rewire.

UX-DR1: EnvelopeDisplay bidirectional geometry and drag rules per addendum (implements FR-11).
UX-DR2: TrackGeneratorDisplay five-point Y-only drag geometry per addendum (implements FR-12).
UX-DR3: Activity LED UX pattern for Instrument/Editor queue traffic (implements FR-9).
UX-DR4: COMPARE mode blinking label and History graying states (implements FR-32).
UX-DR5: Matrix Mod bus reorder drag interaction — swap vs move detail (implements FR-50).
UX-DR6: Patch Mutator panel layout — Amount/Random, toggles, History M/R, action buttons (implements FR-54–FR-60).
UX-DR7: Logo popup vs inline Skin/Scale — pending Figma approval (implements FR-41, PRD §9 #1).
UX-DR8: ScaledLayout per UI scale preset without global AffineTransform blur (implements FR-43).

### FR Coverage Map

FR-1: Epic 8 — Device Inquiry at connection
FR-2: Epic 8 — UI lock without synth
FR-3: Epic 7 — Startup persistence policy (prefs; wired with Settings/header stories)
FR-4: Epic 8 + Epic R — Virtual instrument plugin category & audio bus layout (AD-11)
FR-4b: Epic R — Hardware Latency GUI + reportLatency()
FR-5: Epic 2 — Instrument path forwarding
FR-6: Epic 2 — Editor path always-on
FR-7: Epic 2 — Unified outbound queue
FR-8: Epic 2 + Epic R — Audio passthrough & peak indicator (standalone; AD-11 revision via R-1)
FR-9: Epic 2 — Activity LEDs on unified queue
FR-10: Epic 2 — PATCH parameter editing → SysEx (requires Epic 1 model)
FR-11: Epic 10 — Interactive envelope displays (Display↔APVTS direct)
FR-12: Epic 10 — Interactive track generator display
FR-13: Epic 1 — Patch name bytes 0–7 in packed buffer / APVTS sync
FR-14: Epic 2 — Matrix Mod bus parameter SysEx
FR-15: Epic 3 — Matrix Mod init defaults
FR-16: Epic 2 — Master parameter editing → full master SysEx
FR-17: Epic 3 — Master module init confirmation
FR-18: Epic 7 — Master file operations via Settings
FR-19: Epic 7 — Bank selection
FR-20: Epic 7 — Bank lock
FR-21: Epic 7 — Bank selection exclusivity
FR-22: Epic 7 — Internal patch navigation
FR-23: Epic 7 — ROM gating
FR-24: Epic 7 — Internal patch INIT/COPY/PASTE/STORE
FR-25: Epic 4 — Computer Patches folder scan
FR-26: Epic 4 — Combobox sentinel states
FR-27: Epic 4 — Folder path persistence
FR-28: Epic 4 — Save injects validated filename
FR-29: Epic 4 — Name reconciliation on import
FR-30: Epic 6 — Full patch SysEx on MUTATE/RETRY/audition
FR-31: Epic 6 — Session-only Mutator history
FR-32: Epic 6 — COMPARE mode
FR-33: Epic 6 — Mutator EXPORT layout (via PatchFileService)
FR-34: Epic 6 — Mutator recipe persistence
FR-35: Epic 5 — Module copy/paste compatibility
FR-36: Epic 3 — Patch/Master init sources
FR-37: Epic 3 — Init template separation from Computer Patches
FR-38: Epic 3 — Master init confirmation (see FR-17)
FR-39: Epic 7 — Header routing controls (brownfield completion/wiring)
FR-40: Epic 7 — Settings page consolidation
FR-41: Epic 7 — Logo popup layout (conditional Figma — PRD §9 #1)
FR-42: Epic 7 — Skin persistence
FR-43: Epic 7 — UI scale presets
FR-44: Epic 7 — Footer messaging (ExceptionPropagator)
FR-45: Epic 7 — Graying rules across panels
FR-46: Epic 8 — Matrix-6/6R PATCH-only mode
FR-47: Epic 7 — ActionDispatcher hub
FR-48: Epic 2 — APVTS parameter → SysEx listener path
FR-49: Epic 1 — PatchModel / MasterModel + mappers
FR-50: Epic 2 + 7 — Matrix Mod bus reorder (SysEx E2; drag UX + handler E7)
FR-51: Epic 9 — Unsaved patch edit warning
FR-52: Epic 4 — Computer Patches Previous/Next file navigation
FR-53: Epic 8 — Footer device identity display
FR-54: Epic 6 — Two-level History M/R comboboxes
FR-55: Epic 6 — MUTATE semantics
FR-56: Epic 6 — RETRY semantics
FR-57: Epic 6 — History selection audition with debounce
FR-58: Epic 6 — DELETE and CLEAR
FR-59: Epic 6 + 7 — Defrag modal + Settings manual Defrag
FR-60: Epic 6 — Mutator enabled/disabled states

**NFR coverage (by primary epic):** NFR-1 all epics with unit tests · NFR-2 Epic 2 · NFR-3 Epic 2/8 · NFR-4 Epic 1/2 · NFR-5 all · NFR-6 ongoing · NFR-7 Epic 0/7 · NFR-8 Epic 0

## Epic List

### Epic 0: Development Foundation (P-001)

Migrate the repository to the target tree (`src/`, `assets/`, `logs/`, `docs/`, `tests/`, `builds/`, `_local/`), fix runtime project paths (`ProjectPaths`, AD-10), and align CMake, gitignore, and agent docs. **Enabler epic** — no direct end-user FR; unblocks all Core work on correct paths.

**FRs covered:** _(infrastructure — see Additional Requirements AD-9, AD-10, D-094)_

**Depends on:** none · **Unblocks:** Epics 1–10

---

### Epic 1: Patch & Master Memory Model

Users' patch and master edits are backed by descriptor-driven packed buffers (134/172 bytes) synced with APVTS — the single source of truth for serialization, patch name, and future SysEx.

**FRs covered:** FR-49, FR-13 (foundation)

**Depends on:** Epic 0 · **Unblocks:** Epic 2+

---

### Epic 2: Live MIDI Editing Pipeline

Users edit PATCH/MASTER/Matrix Mod parameters and hear changes on the synth reliably: unified outbound queue (realtime before SysEx), APVTS→SysEx routing, instrument forwarding, audio passthrough, activity LEDs, and DAW-safe throttling.

**FRs covered:** FR-5, FR-6, FR-7, FR-8, FR-9, FR-10, FR-14, FR-16, FR-48, FR-50 (SysEx side)

**Depends on:** Epic 1 · **Unblocks:** Epic R, Epics 3–7 (editing paths)

---

### Epic R: Architectural Revision (Audio/MIDI Host Model)

Align hosted plugin audio/MIDI model with VST3/AU constraints (Option D); add Hardware Latency, MIDI port error feedback, and queue MPSC validation. **Sprint Change Proposal 2026-06-06 — approved.**

**FRs covered:** FR-4 (revision), FR-4b, FR-8 (revision), FR-39 (conditional header), NFR-3 (R-4)

**Depends on:** Epic 2 (stories 2.7–2.11 done) · **Unblocks:** Epic 3 (R-4 before E3 recommended)

**Story order:** R-1 → R-4 → R-2 → R-3 → R-5 (2.10 may proceed after R-1)

---

### Epic 3: Init Templates & Module Defaults

Users reset modules and patches from INIT templates or hardcoded defaults; Matrix Mod and Master inits behave per PRD (Master confirmation, separated template loader).

**FRs covered:** FR-15, FR-17, FR-36, FR-37, FR-38

**Depends on:** Epic 2 · **Unblocks:** Epic 5, 7 (INIT actions)

---

### Epic 4: Computer Patches Library

Users browse, load, save, and navigate `.syx` files from a folder with validation, sentinel combobox states, filename injection, and reconciliation policies.

**FRs covered:** FR-25, FR-26, FR-27, FR-28, FR-29, FR-52

**Depends on:** Epic 2 · **Unblocks:** Epic 9 (navigation + dirty state)

---

### Epic 5: Type-Aware Module Clipboard

Users copy and paste compatible modules (and full patches) with correct enable/gray rules across PATCH EDIT and Patch Manager.

**FRs covered:** FR-35

**Depends on:** Epic 3 · **Unblocks:** Epic 7 (module I/C/P handlers)

---

### Epic 6: Patch Mutator Workflow

Users explore variations via MUTATE/RETRY, two-level History, COMPARE, EXPORT, DELETE/CLEAR, and Defrag — Absynth-style session workflow with recipe persistence.

**FRs covered:** FR-30, FR-31, FR-32, FR-33, FR-34, FR-54, FR-55, FR-56, FR-57, FR-58, FR-59, FR-60

**Depends on:** Epic 2, Epic 4 (export service) · **Includes:** MutationAlgorithm spec story (PRD §9 #7) before MUTATE implementation

---

### Epic 7: Patch Manager & Action Dispatch

Users operate Bank Utility, Internal Patches, Settings, header/footer shell, and all StandaloneWidget actions through the ActionDispatcher hub — panels stay presentation-only.

**FRs covered:** FR-3, FR-18, FR-19, FR-20, FR-21, FR-22, FR-23, FR-24, FR-39, FR-40, FR-41, FR-42, FR-43, FR-44, FR-45, FR-47, FR-50 (UX/handler side), FR-59 (Settings Defrag entry)

**Depends on:** Epics 2–6 (handlers call services) · **Unblocks:** Epic 9

---

### Epic 8: Synth Connection & Instrument Role

Users connect a Matrix-1000/6/6R, see device identity in the footer, get UI lock when disconnected, and run as a virtual instrument with correct audio bus layout and M-6 PATCH-only mode.

**FRs covered:** FR-1, FR-2, FR-4, FR-46, FR-53

**Depends on:** Epic 2 (MIDI stack) · Can parallelize late in sprint plan per D-058

---

### Epic 9: Unsaved Edit Protection

Users are warned before losing unsaved patch edits when navigating internal patches or Computer Patches files, with Settings policy control.

**FRs covered:** FR-51

**Depends on:** Epic 4, Epic 7 · **Uses:** DirtyPatchTracker

---

### Epic 10: Direct Interactive Display Editing

Users drag envelope curves and track generator points that write APVTS directly — removing the legacy Display→Slider bridge.

**FRs covered:** FR-11, FR-12

**Depends on:** Epic 2 (APVTS sync stable) · **UX-DR:** UX-DR1, UX-DR2

---

### Epic U: UI Scale Audit & Pixel-Perfect Layout

Users see a Figma-faithful interface at every UI Scale preset (50–200%) — crisp layout via `ScaledLayout` / `ScaledDrawing` / compile-time `Design*` plan (Factory-injected `*Dimensions`) on all prod panels and widgets, with harmonized widget IDs, debug `TestComponent` enrichment, and a release gate when audit is complete.

**FRs covered:** FR-43 (geometry & validation only), UX-DR8

**Decisions:** D-013, D-062, D-063, D-064 (D-065 debug-only note)

**Depends on:** Epic 2 Story 2.11 (header left cluster frozen) · **Parallel with:** Epics 3–6 Core · **Blocks:** v1 UI craftsmanship release gate · **Does not duplicate:** Story 7.8 (shell/persistence), Epic 2 header stories done

**Sequence (Epic U):** U-IDs → U-0 → U-0b → U-2…U-9 → U-10 · U-1 parallel with U-0/U-0b

**Full spec:** `planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md` (v1.1) · **ID inventory:** `implementation-artifacts/u-ids-widget-id-harmonization-inventory.md`

---

**Implementation sequence (D-058):** E0 → E1 → E2 → E3 → E4 → E5 → E6 → E7 → E8 → E9 → E10 · **Parallel track:** Epic U (GUI scale audit)

**Total:** 12 epics (incl. Epic U) · **66 stories** · Epic U = layout audit overlay (FR-43 geometry) · Epic U stories: U-IDs, U-0, U-0b, U-1…U-10

---

## Epic 0: Development Foundation (P-001)

Migrate repository layout and runtime paths so all subsequent Core work lands in target locations with correct log resolution.

### Story 0.1: Gitignore and Root Directory Renames

As a developer,
I want the repository root folders renamed and gitignore updated,
So that the tree matches P-001 targets before any new Core files are added.

**Acceptance Criteria:**

**Given** the brownfield repo on branch `main`
**When** Epic 0 Story 0.1 is merged
**Then** `Source/` is renamed to `src/`, `Assets/` to `assets/` (if tracked), `.gitignore` lists `builds/` and `logs/` (legacy `Builds/` and `Logs/` retained during transition)
**And** no new feature code is mixed into this commit

### Story 0.2: Documentation and Tests Migration

As a developer,
I want public docs and unit tests moved to `docs/` and `tests/`,
So that versioned artifacts follow pro JUCE layout.

**Acceptance Criteria:**

**Given** Story 0.1 complete
**When** documentation and test folders are migrated
**Then** public `.md` files live under `docs/` (kebab-case manifest recorded), `Quality/Tests/` becomes `tests/`, personal `Quality/` moves to `_local/quality/`, `Workbench/` to `_local/workbench/`
**And** `_local/` remains gitignored

### Story 0.3: ProjectPaths and Logger Runtime Root (AD-10)

As a developer,
I want log paths resolved at runtime relative to the repo root,
So that moving the project on disk no longer writes logs to a stale absolute path (D-093, D-094).

**Acceptance Criteria:**

**Given** Story 0.1 complete
**When** `ProjectPaths` discovers the Matrix-Control root (executable walk-up and/or marker) and loggers use `{root}/logs/midi/` and `{root}/logs/apvts/`
**Then** CMake `MATRIX_CONTROL_PROJECT_ROOT` compile define is removed
**And** a rebuild after moving the repo (without reconfigure) writes logs under the new `{root}/logs/` without creating files at the old path

### Story 0.4: CMake and Tooling Path Updates

As a developer,
I want CMake and IDE tooling to reference `src/`, `builds/`, and `tests/`,
So that builds and CI use the target layout.

**Acceptance Criteria:**

**Given** Stories 0.1–0.3 complete
**When** `CMakeLists.txt` is updated
**Then** include root is `src/`, output directory is `builds/macos/` (and windows/linux paths documented), `tests/` subdirectory is wired
**And** `.cursorrules`, `project-context.md`, and README paths are updated; `.clangd` / compile_commands paths fixed if needed

### Story 0.5: Migration Validation and P-001 Sign-Off

As a developer,
I want a full macOS build and boot validators green after migration,
So that Epic 0 is safe to build upon.

**Acceptance Criteria:**

**Given** Stories 0.1–0.4 complete
**When** macOS Debug build runs and `ApvtsValidator` + `WidgetFactoryValidator` pass
**Then** decision log records **P-001 DONE** and `project-context` version is bumped
**And** Epic 1 stories may add files under `src/Core/Models/` only after this story merges

---

## Epic 1: Patch & Master Memory Model

Descriptor-driven packed buffers become the Core SSOT for patch and master data.

### Story 1.1: PatchModel Packed Buffer

As a sound designer,
I want a 134-byte packed patch buffer in Core,
So that patch data matches Oberheim Matrix-1000 layout for SysEx and file I/O (FR-49).

**Acceptance Criteria:**

**Given** descriptor offsets in `PluginDescriptors`
**When** `PatchModel` reads/writes the 134-byte buffer
**Then** byte layout matches Oberheim reference samples in `_bmad-output/reference-docs/oberheim/`
**And** unit tests round-trip at least three reference patches without manual offset tables outside descriptors

### Story 1.2: MasterModel Packed Buffer

As a sound designer,
I want a 172-byte packed master buffer in Core,
So that master SysEx serialization is accurate (FR-49).

**Acceptance Criteria:**

**Given** Story 1.1 patterns and master descriptors
**When** `MasterModel` reads/writes the 172-byte buffer
**Then** layout matches Oberheim master reference samples
**And** unit tests verify checksum-relevant fields

### Story 1.3: ApvtsPatchMapper Round-Trip

As a sound designer,
I want APVTS PATCH parameters synced with `PatchModel`,
So that widget edits and packed buffer stay consistent (FR-49, FR-10 foundation).

**Acceptance Criteria:**

**Given** Story 1.1 and existing APVTS layout
**When** a parameter changes in APVTS or buffer
**Then** `ApvtsPatchMapper` updates the other side using descriptors only
**And** no duplicate SysEx offset tables exist outside `PluginDescriptors`

### Story 1.4: ApvtsMasterMapper Round-Trip

As a sound designer,
I want APVTS MASTER parameters synced with `MasterModel`,
So that master editing uses the same SSOT pattern (FR-49, FR-16 foundation).

**Acceptance Criteria:**

**Given** Story 1.2
**When** a MASTER parameter changes
**Then** `ApvtsMasterMapper` updates `MasterModel` and vice versa via descriptors
**And** unit tests cover all 22 master parameters

### Story 1.5: Patch Name Bytes 0–7 Sync

As a sound designer,
I want the patch name in APVTS to reflect bytes 0–7 of the packed buffer,
So that PATCH NAME display and `.syx` filenames stay aligned (FR-13).

**Acceptance Criteria:**

**Given** Stories 1.1 and 1.3
**When** patch name is edited in PATCH EDIT or loaded from buffer
**Then** bytes 0–7 use 6-bit ASCII Matrix rules and APVTS property mirrors the display string
**And** max 8 characters enforced with validator feedback

---

## Epic 2: Live MIDI Editing Pipeline

Parameter edits reach the synth through a unified, rate-limited MIDI pipeline.

### Story 2.1: MidiOutboundQueue Core

As a performer,
I want all outbound MIDI merged in one queue with realtime priority,
So that playing notes is never blocked by SysEx floods (FR-7, NFR-2, NFR-3).

**Acceptance Criteria:**

**Given** Epic 1 complete
**When** Note/CC/PB and SysEx are enqueued
**Then** dequeue order always sends realtime messages before pending SysEx
**And** unit tests verify ordering under synthetic load; audio thread never calls `MidiOutput` directly

### Story 2.2: SysExDelayProfile and Inter-Message Delay

As a sound designer,
I want inter-SysEx delay selected from device firmware class,
So that stock and optimised EPROMs are not overloaded (FR-7, NFR-2, PRD §9 #2 spike).

**Acceptance Criteria:**

**Given** Story 2.1
**When** the queue dequeues SysEx messages
**Then** minimum delay follows profile (10 ms M-1000 baseline, 20 ms M-6/6R per assumptions) with EPROM class from Device Inquiry string match (rules documented if hardware TBD)
**And** unit tests mock delay enforcement

### Story 2.3: Instrument Path and Editor Path Producers

As a performer,
I want keyboard/CC forwarded when armed and editor SysEx always active,
So that I can play and edit independently (FR-5, FR-6).

**Acceptance Criteria:**

**Given** Story 2.1
**When** track is disarmed, instrument path enqueues nothing
**Then** editor SysEx and Program Change still enqueue on the dedicated MIDI thread
**And** Program Change and SysEx from instrument path are stripped before enqueue

### Story 2.4: APVTS PATCH Parameter to SysEx (0x06)

As a sound designer,
I want PATCH slider/combo changes to send Remote Parameter Edit SysEx,
So that live editing works with DAW automation throttled by the queue (FR-10, FR-48).

**Acceptance Criteria:**

**Given** Stories 1.3 and 2.1
**When** any PATCH `AudioParameter` changes
**Then** appropriate 0x06 message is enqueued per descriptor type (6-bit, 7-bit, choice)
**And** `PluginProcessor` or delegated service listens — not GUI panels

### Story 2.5: APVTS MASTER Parameter to Full Master SysEx (0x03)

As a sound designer,
I want each MASTER change to send a complete master SysEx,
So that master edits are reliable (FR-16, FR-48).

**Acceptance Criteria:**

**Given** Stories 1.4 and 2.1
**When** any MASTER parameter changes
**Then** full 0x03 message is built from `MasterModel` and enqueued
**And** unit test verifies message size and opcode

### Story 2.6: Matrix Mod Bus Parameter SysEx

As a sound designer,
I want Matrix Mod bus edits to update SOURCE, AMOUNT, and DESTINATION together,
So that one bus change sends one coherent SysEx block (FR-14, PRD §9 #3).

**Acceptance Criteria:**

**Given** Story 2.4
**When** any of the three bus parameters changes
**Then** SysEx updates all three fields for that bus index
**And** opcode choice (0x0B vs 0x06 block) is documented from hardware spike story note in AC or story comments; Matrix-6/6R support verified or flagged

### Story 2.7: Audio Passthrough and Peak Indicator

As a performer,
I want synth audio passed through with gain and peak display,
So that I can monitor input level in the plugin (FR-8, UX-DR3 partial).

**Acceptance Criteria:**

**Given** Story 2.1 and `AudioPassthroughProcessor` in `src/Core/Audio/`
**When** audio input is present
**Then** output reflects user Input Gain without blocking the audio thread
**And** peak indicator uses solid fill colour matching envelope curve colour from active skin

**Revision note (Sprint Change Proposal 2026-06-06):** Scope narrows to **standalone** passthrough after Story R-1 (AD-11). Hosted plugin = silent output only.

### Story 2.8: Activity LEDs on Queue Traffic

As a performer,
I want Instrument and Editor LEDs to reflect MIDI activity,
So that I see which path is active (FR-9, UX-DR3).

**Acceptance Criteria:**

**Given** Story 2.3
**When** messages enqueue/dequeue on instrument vs editor paths
**Then** header LEDs update on message thread from queue hooks
**And** behaviour matches agreed UX pattern (document in story notes if UX spec pending)

### Story 2.9: Wire MidiManager Queue Consumer

As a developer,
I want `MidiManager::run()` to consume the unified queue,
So that the brownfield transport stack sends through `MidiSender` (AD-3).

**Acceptance Criteria:**

**Given** Stories 2.1–2.3
**When** the MIDI thread runs
**Then** `MidiManager` dequeues and sends via `MidiSender` only
**And** brownfield direct-send paths in editor/instrument producers are removed or redirected to queue

### Story 2.9b: Header Routing Controls (UAT Slice)

As a sound designer validating the Epic 2 MIDI stack,
I want MIDI From, MIDI To, and Keyboard From combos wired in HeaderPanel,
So that I can configure hardware routing for manual UAT without waiting for Story 7.8 (partial FR-39).

**Acceptance Criteria:**

**Given** Story 2.9 done (queue consumer draining outbound traffic)
**When** user selects MIDI From / MIDI To in the header
**Then** `PluginProcessor::setMidiInputPort` / `setMidiOutputPort` open the chosen devices and persist `midiInputPortId` / `midiOutputPortId`
**And** Keyboard From is grayed with **HOST** in plugin mode; standalone selects a keyboard port with minimal Core forwarder to the unified queue
**And** Audio From, Input Gain, peak, and activity LEDs remain Stories 2.7 / 2.8; full shell polish remains Story 7.8

**Revision note (Sprint Change Proposal 2026-06-06):** Audio From / Input Gain / peak **standalone only** after Story R-1.

### Story 2.10: Matrix Mod Bus Reorder SysEx

As a sound designer,
I want bus content reorder to send SysEx for affected buses,
So that permuted Source/Amount/Destination values reach the synth (FR-50, SysEx side).

**Acceptance Criteria:**

**Given** Story 2.6
**When** a bus reorder operation completes in Core
**Then** SysEx updates all affected bus rows without renumbering indices 0–9
**And** inactive buses (NONE/0%/NONE) can be permuted like active rows

### Story 2.11: Header Panel Layout and Widget Styling

As a performer,
I want the header routing controls laid out with consistent gaps and correctly styled audio/MIDI indicators,
So that I can read port selection, activity, and input level at a glance (FR-8, FR-9, FR-39 polish).

**Acceptance Criteria:**

**Given** Stories 2.7, 2.8, and 2.9b (routing cluster present in `HeaderPanel`)
**When** the header is laid out at any UI scale
**Then** the left routing row uses `kGap_` (5 px design) from the left edge, `kGap_` between elements within a logical packet, and `2×kGap_` between packets (MIDI FROM → MIDI TO → KEYBOARD FROM → AUDIO FROM → INPUT GAIN)
**And** Editor, Instrument, and new outbound activity LEDs sit immediately to the right of the MIDI FROM, KEYBOARD FROM, and MIDI TO combos respectively (`kGap_` spacing); INSTRUMENT/EDITOR text labels are removed
**And** port combos are 96×20; input gain slider is 60×20 (−∞…+12 dB, unit `dB`); peak indicator is 12×20; activity LEDs are 12×12 squares with slider-matched border and colours
**And** `MidiActivityTracker::Path::kOutbound` drives the MIDI TO LED from successful `MidiManager` dispatch
**And** full FR-41 shell polish remains Story 7.8

**Revision note (Sprint Change Proposal 2026-06-06):** AUDIO FROM packet and peak indicator visible **standalone only** after Story R-1 (AD-11).

---

## Epic R: Architectural Revision (Audio/MIDI Host Model)

### Story R-1: Remove Plugin Audio Input Bus

As an Ableton/Logic/Cubase user,
I want Matrix-Control to behave as a standard MIDI instrument without routing synth audio through the plugin,
So that my DAW routing is natural and unambiguous (AD-11, Option R-A).

**Acceptance Criteria:**

**Given** Stories 2.7, 2.9b, and 2.11 complete
**When** the plugin loads in a hosted VST3/AU context
**Then** `BusesProperties` declares no audio input bus and stereo silent output
**And** standalone retains input bus + `StandaloneAudioInputRouter` passthrough
**And** `AudioPassthroughProcessor::process()` in plugin mode clears output without reading input
**And** HeaderPanel hides Audio From, Input Gain, and PeakIndicator in plugin mode
**And** Activity LEDs (Story 2.8) remain primary MIDI feedback in plugin mode
**And** unit tests and VST3 + Standalone builds pass

### Story R-2: Hardware Latency GUI

As a user recording Matrix-1000 audio in my DAW,
I want to set Hardware Latency (ms) in Matrix-Control,
So that my DAW can compensate for analog round-trip delay (FR-4b).

**Acceptance Criteria:**

**Given** Story R-1 or parallel
**When** user sets Hardware Latency (0–100 ms, 0.1 ms step)
**Then** value is converted to samples and exposed via `reportLatency()`
**And** value persists in `apvts.state`
**And** user docs include typical values table

### Story R-3: MIDI Port Open Error Feedback

As a Windows user sharing MIDI ports with my DAW,
I want clear GUI feedback when port open fails,
So that I can diagnose exclusive-access conflicts.

**Acceptance Criteria:**

**Given** `MidiInputPort` or `MidiOutputPort` open is attempted
**When** open fails
**Then** footer `uiMessageText` names the port and suggests likely cause (Windows exclusive access)
**And** `MidiLogger` records the error
**And** unit test: mock port failure → footer message asserted

### Story R-4: MidiOutboundQueue MPSC Audit

As a developer,
I want documented MPSC contract on `MidiOutboundQueue`,
So that concurrent producers (audio + message threads) are safe before Epic 3.

**Acceptance Criteria:**

**Given** Stories 2.1–2.9 complete
**When** queue contract is reviewed
**Then** `MidiOutboundQueue.h` documents producer/consumer contract (MPSC or fix applied)
**And** stress test: dual producer threads + single consumer without data loss
**And** findings logged in code comment

### Story R-5: Windows MIDI Multi-Client Documentation

As a Windows user,
I want setup documentation for MIDI port conflicts,
So that I can use loopMIDI or rtpMIDI when winMM blocks shared access.

**Acceptance Criteria:**

**Given** Story R-3 complete
**When** user reads Windows MIDI setup guide in `Documentation/`
**Then** guide explains winMM exclusive-access limitation and recommends loopMIDI 1.0.16+ (or verified alternative)
**And** in-app error message (R-3) links to guide

---

## Epic 3: Init Templates & Module Defaults

Users reset modules and patches from templates or hardcoded defaults.

### Story 3.1: InitDefaults Hardcoded Buffers

As a sound designer,
I want fallback init data when template files are missing,
So that INIT always works offline (FR-36, PRD §9 #4).

**Acceptance Criteria:**

**Given** Epic R complete (R-4 recommended before E3)
**When** `InitDefaults` is queried for patch or master
**Then** hardcoded buffers match owner-supplied reference values or documented placeholders pending owner input
**And** unit tests verify buffer sizes 134/172

### Story 3.2: InitTemplateLoader for PatchInit and MasterInit

As a sound designer,
I want INIT to load user `.syx` templates from Settings paths,
So that custom init templates apply without Computer Patches combobox (FR-36, FR-37).

**Acceptance Criteria:**

**Given** Story 3.1
**When** `PatchInit.syx` or `MasterInit.syx` exists at configured path
**Then** `InitTemplateLoader` loads validated buffer into model
**And** missing file falls back to `InitDefaults` with footer info, not modal

### Story 3.3: Matrix Mod Init Defaults

As a sound designer,
I want section and per-bus Matrix Mod init to reset to NONE/0%/NONE,
So that modulation matrix clears predictably (FR-15).

**Acceptance Criteria:**

**Given** Story 2.6
**When** section init or per-bus init is invoked
**Then** all targeted buses reset to SOURCE=NONE, AMOUNT=0%, DESTINATION=NONE via SysEx
**And** no user INIT file is required for Matrix Mod v1

### Story 3.4: Master Module Init Confirmation Path

As a sound designer,
I want a confirmation dialog before MASTER module init,
So that accidental master resets are prevented (FR-17, FR-38).

**Acceptance Criteria:**

**Given** Story 3.2
**When** MASTER module Init is triggered
**Then** confirmation is required before SysEx is sent; PATCH module Init has no confirmation
**And** confirmed init sends full master SysEx reflecting reset module

---

## Epic 4: Computer Patches Library

Folder-based `.syx` library with scan, navigation, and save policies.

### Story 4.1: PatchFileService Folder Scan

As a sound designer,
I want to open a folder and scan valid `.syx` files,
So that I can browse my patch library (FR-25).

**Acceptance Criteria:**

**Given** Epic 2 complete and `PatchFileService` under `src/Core/Services/`
**When** user selects OPEN folder
**Then** scan validates structure, counts valid vs invalid files, reports in footer
**And** invalid files are skipped without crash

### Story 4.2: Combobox Sentinel States

As a sound designer,
I want EMPTY and SELECT combobox states,
So that I understand when no folder or no file is chosen (FR-26).

**Acceptance Criteria:**

**Given** Story 4.1
**When** no folder, empty folder, or folder with files but no selection
**Then** combobox shows `<EMPTY>` or `<SELECT>` as specified — not as real list entries
**And** states are grayed appropriately

### Story 4.3: Folder Path Persistence

As a sound designer,
I want my last folder remembered across sessions,
So that I resume browsing without re-picking each launch (FR-27).

**Acceptance Criteria:**

**Given** Story 4.1
**When** plugin relaunches
**Then** last folder path is rescanned without auto-loading a patch or sending SysEx
**And** path persists via session XML prefs

### Story 4.4: Save with Filename Injection

As a sound designer,
I want Save/Save As to write sanitized 8-char names into patch bytes,
So that files and patch name stay consistent (FR-28).

**Acceptance Criteria:**

**Given** Story 4.1 and Story 1.5
**When** user saves a `.syx`
**Then** filename is sanitized to 8 Matrix characters and injected into bytes 0–7
**And** written file passes SysEx validation tests

### Story 4.5: Import Name Reconciliation

As a sound designer,
I want load to apply my reconciliation policy for name vs filename mismatches,
So that imports behave predictably (FR-29).

**Acceptance Criteria:**

**Given** Story 4.4 and Settings policy property (wired in Epic 7)
**When** loaded file internal name and filename differ after sanitization
**Then** policy applies (default: prefer internal name) without mandatory modal
**And** optional footer notice is shown

### Story 4.6: Previous and Next File Navigation

As a sound designer,
I want `<` `>` buttons to cycle patch files in the folder,
So that I can audition files quickly (FR-52).

**Acceptance Criteria:**

**Given** Stories 4.1–4.2
**When** valid folder loaded and combobox lists files
**Then** Previous/Next cycle circularly through sorted list with same load validation as combobox
**And** buttons disabled in `<EMPTY>` / `<SELECT>` states

---

## Epic 5: Type-Aware Module Clipboard

Module, patch, and matrix-modulation clipboard with compatibility rules.

### Story 5.1: ClipboardService Compatibility Matrix

As a sound designer,
I want copy/paste to respect module type compatibility,
So that invalid pastes are prevented (FR-35, addendum § Clipboard).

**Acceptance Criteria:**

**Given** Epic 3 complete
**When** `ClipboardService` stores a module, full patch, or matrix-modulation snapshot
**Then** compatibility matrix matches addendum (ENV full interchange; DCO/LFO partial rules; matrix-modulation full 10-bus snapshot)
**And** matrix-modulation snapshot captures PatchModel bytes 104–133 (source + amount + destination × 10 buses)
**And** unit tests cover matrix cases including matrix-modulation mode

### Story 5.2: Module Copy Paste Enable and Gray Rules

As a sound designer,
I want Paste enabled only on compatible targets,
So that UI reflects clipboard state (FR-35).

**Acceptance Criteria:**

**Given** Story 5.1
**When** user copies a module, full patch, or matrix modulation
**Then** Paste buttons gray per FR-35 rules (full-patch copy grays PATCH EDIT paste; Internal Patches paste remains when applicable; matrix-modulation copy grays all PATCH EDIT and MASTER module Paste buttons — only Matrix Modulation Paste remains active)
**And** last copy defines clipboard mode until replaced

### Story 5.3: Matrix Modulation Section I/C/P GUI

As a sound designer,
I want Init/Copy/Paste buttons in the Matrix Modulation section header,
So that I can reset, copy, or paste the full 10-bus matrix between patches (FR-35).

**Acceptance Criteria:**

**Given** Stories 5.1–5.2 and existing Matrix Mod Init (Epic 3.3)
**When** the Matrix Modulation panel is displayed
**Then** the section header exposes I/C/P buttons (20×20) to the right of the ModulationBusHeader DESTINATION label
**And** DESTINATION header label width is 68 px with zero gap before the I button (68 + 0 + 60 = 128 px column alignment preserved)
**And** bus-row DESTINATION combo width remains 104 px unchanged
**And** `PluginIDs`, `PluginDisplayNames`, and `PluginDescriptors` register Copy/Paste standalone widgets adjacent to the existing Init entry (not appended at file end)
**And** Copy/Paste APVTS properties follow the `matrixModulationInit` naming pattern (`matrixModulationCopy`, `matrixModulationPaste`)

---

## Epic 6: Patch Mutator Workflow

Absynth-style mutation session with two-level history.

### Story 6.1: MutationAlgorithm Specification

As a product owner,
I want Amount/Random and module-toggle mutation rules documented,
So that MUTATE/RETRY implementation has a testable spec (PRD §9 #7, AD-6).

**Acceptance Criteria:**

**Given** FR-30–FR-34 and FR-54–FR-60
**When** spec is written in architecture addendum or Epic 6 story artifact
**Then** rules cover per-parameter jitter, Amount/Random curves, module toggle mask, and Matrix Mod inclusion
**And** Guillaume approves spec before Story 6.4 starts

### Story 6.2: MutationHistoryStore Two-Level M/R

As a sound designer,
I want session history stored as roots M00–M99 and retries R00–R99 per root,
So that MUTATE/RETRY semantics have a Core backing (FR-54 foundation, FR-55, FR-56).

**Acceptance Criteria:**

**Given** Epic 2 complete
**When** entries are added, deleted, or queried
**Then** `MutationHistoryStore` maintains parent snapshots, result buffers, numeric indices with gaps preserved
**And** unit tests cover max limits 100 roots / 100 retries per root

### Story 6.3: MutationNaming and Display Names

As a sound designer,
I want mutation entries named Mxx and Mxx-Ryy in buffer and UI,
So that names match EXPORT filenames (FR-55, FR-56, FR-13 integration).

**Acceptance Criteria:**

**Given** Story 6.2
**When** MUTATE or RETRY allocates a new index
**Then** `MutationNaming` assigns max+1 with gaps; first root M00; first retry R00
**And** PATCH NAME and bytes 0–7 updated; max 7 chars for retry names

### Story 6.4: MUTATE Action

As a sound designer,
I want MUTATE to create a new root from the auditioned patch,
So that I explore variations without losing other roots (FR-55, FR-30).

**Acceptance Criteria:**

**Given** Stories 6.1–6.3 and `PatchMutatorEngine`
**When** user clicks MUTATE with Mi or Mi-Rj auditioned
**Then** new root max(Mi)+1 is created, History M sorts numerically, full patch 0x01 SysEx sent
**And** disabled when M00–M99 full; existing roots never removed

### Story 6.5: RETRY from Parent Snapshot

As a sound designer,
I want RETRY to branch from parent snapshot of selected entry,
So that Absynth-style retries do not cascade-delete siblings (FR-56, FR-30).

**Acceptance Criteria:**

**Given** Story 6.4
**When** user clicks RETRY on Mi or Mi-Rj
**Then** new retry max(R)+1 under same Mi uses `parentSnapshot` as input, not selected result
**And** full patch SysEx sent; disabled when 100 retries for current Mi

### Story 6.6: History M and R UI Properties

As a sound designer,
I want History M and R comboboxes mirroring store state,
So that I can select mutations in the panel (FR-54, UX-DR6).

**Acceptance Criteria:**

**Given** Story 6.2
**When** store changes
**Then** APVTS properties expose `historyMList`, `historyRList`, `selectedM`, `selectedR` with `<EMPTY>` and `—` sentinels
**And** no `<` `>` navigation buttons (D-026)

### Story 6.7: History Selection Audition with Debounce

As a sound designer,
I want selecting history entries to audition after debounce,
So that rapid scrolling does not SysEx-flood the synth (FR-57, FR-30).

**Acceptance Criteria:**

**Given** Stories 6.6 and 2.1
**When** user changes History M or R selection
**Then** full patch 0x01 sends after debounce matching Computer Patches pattern
**And** PATCH NAME follows auditioned entry

### Story 6.8: COMPARE Mode

As a sound designer,
I want COMPARE to toggle initial snapshot against current mutations,
So that I hear the original patch (FR-32, UX-DR4).

**Acceptance Criteria:**

**Given** Story 6.2 with at least one mutation
**When** COMPARE is toggled on
**Then** initial snapshot auditions, label blinks, History M/R grayed, no bank STORE
**And** re-toggle restores prior selection; disabled when history empty

### Story 6.9: DELETE and CLEAR History

As a sound designer,
I want to delete entries or clear all history,
So that I manage session mutations (FR-58).

**Acceptance Criteria:**

**Given** Story 6.2
**When** DELETE root Mi
**Then** all Mi-R* cascade removed with footer notice; gaps remain
**When** CLEAR
**Then** all history purged, History M = `<EMPTY>`, COMPARE disabled

### Story 6.10: HistoryDefrag on Limit

As a sound designer,
I want Defrag when M or R slots exhaust due to gaps,
So that I can continue mutating (FR-59).

**Acceptance Criteria:**

**Given** Story 6.2
**When** MUTATE/RETRY cannot allocate next index
**Then** modal offers Defrag or Cancel; Defrag renumbers M00… and R00… contiguously, updates names, preserves content
**And** footer confirms completion

### Story 6.11: Mutator EXPORT Layout

As a sound designer,
I want EXPORT to write Initial.syx and Mxx/ folder structure,
So that mutations are saved for hardware or archive (FR-33).

**Acceptance Criteria:**

**Given** Story 6.2 and `PatchFileService::exportMutatorHistory`
**When** user EXPORTs to chosen folder
**Then** `Initial.syx` at root plus `Mxx/Mxx.syx` and `Mxx-Ryy.syx` files with matching name bytes
**And** no auto-export on each mutation (D-027)

### Story 6.12: Recipe Persistence and Action Enabled States

As a sound designer,
I want Amount/Random/toggles persisted and buttons enabled correctly,
So that my recipe survives sessions and UI reflects limits (FR-34, FR-60).

**Acceptance Criteria:**

**Given** Stories 6.4–6.5
**When** session reloads
**Then** Amount, Random, module toggles restore from APVTS prefs; history does not (FR-31)
**And** RETRY/MUTATE/EXPORT/DELETE enabled states match FR-60; blocked cases show footer messages

### Story 6.13: Clear History on Patch Load

As a sound designer,
I want Mutator history cleared when I load a new patch,
So that history always belongs to the current patch context (FR-31).

**Acceptance Criteria:**

**Given** Story 6.2 wired in `PatchMutatorEngine`
**When** patch loads from synth or `.syx`
**Then** history store clears; recipe sliders/toggles unchanged
**And** COMPARE disabled until next MUTATE

---

## Epic 7: Patch Manager & Action Dispatch

StandaloneWidget actions route through handlers; panels stay thin.

### Story 7.1: ActionDispatcher and Handler Interfaces

As a developer,
I want a central dispatcher with three handlers,
So that GUI panels only call `setProperty` with timestamps (FR-47, AD-5).

**Acceptance Criteria:**

**Given** Epics 2–6 services available
**When** `ActionDispatcher` receives StandaloneWidget property changes
**Then** routes to `ModuleActionHandler`, `PatchManagerActionHandler`, or `MutatorActionHandler`
**And** unit tests mock routing without GUI

### Story 7.2: ModuleActionHandler I C P and Matrix Mod Init

As a sound designer,
I want module I/C/P and Matrix Mod section actions to run through Core,
So that PATCH/MASTER headers and Matrix Modulation section actions work (FR-35, FR-15, FR-36 integration).

**Acceptance Criteria:**

**Given** Stories 5.1–5.3, 3.3–3.4, and 7.1
**When** user triggers module I/C/P, Matrix Modulation section I/C/P, or per-bus Matrix Mod init from panel
**Then** handler invokes `ClipboardService` (module copy/paste, matrix-modulation copy/paste), `InitTemplateLoader`, or `MatrixModInitService` as appropriate
**And** matrix-modulation paste writes all 10 buses to `PatchModel` and APVTS, then enqueues SysEx via existing `MatrixModBusParameterSysExDispatcher` (same suppress/coalesce pattern as init/reorder)
**And** no SysEx in GUI code paths

### Story 7.3: PatchManagerActionHandler Bank and Internal

As a sound designer,
I want bank and internal patch actions dispatched to Core,
So that Patch Manager buttons behave per PRD (FR-19–FR-24).

**Acceptance Criteria:**

**Given** Story 7.1 and Epic 2 SysEx paths
**When** user selects bank, navigates patch, INIT/COPY/PASTE/STORE
**Then** handler updates APVTS and enqueues appropriate MIDI/SysEx
**And** ROM gating FR-23 enforced before STORE/PASTE

### Story 7-3b: Bank Utility UNLOCK Semantics and ID Rename

As a sound designer,
I want the UNLOCK button to send Unlock Bank SysEx only,
So that bank lock behaviour matches Oberheim spec (FR-20 corrected).

**Acceptance Criteria:**

**Given** Story 7.3 Core handler scaffold
**When** user clicks UNLOCK
**Then** handler sends only `0CH`, sets `patchManagerBanksLocked` false, never sends `0AH`
**When** user selects bank 0–9 or navigation sends Set Bank
**Then** `patchManagerBanksLocked` set true
**And** all IDs renamed: `bankUtilityUnlockBank`, `patchManagerBanksLocked`, display UNLOCK
**And** unit tests updated; grep anchors clean

### Story 7.4: MutatorActionHandler

As a sound designer,
I want Mutator buttons routed to `PatchMutatorEngine`,
So that MUTATE/RETRY/DELETE/CLEAR/EXPORT/COMPARE work from panel (FR-30–FR-34, FR-54–FR-60).

**Acceptance Criteria:**

**Given** Epic 6 complete and Story 7.1
**When** user triggers any Mutator StandaloneWidget action
**Then** `MutatorActionHandler` delegates to engine/services with debounce where required
**And** panel contains no mutation business logic

### Story 7.5: Bank Utility UI Wiring

As a sound designer,
I want bank buttons 0–9 and UNLOCK wired to Core state,
So that bank selection matches synth semantics (FR-19, FR-20, FR-21).

**Acceptance Criteria:**

**Given** Story 7-3b
**When** user selects bank or clicks UNLOCK
**Then** selected bank shows red text; Internal Patches bank NumberBox shows red dot when `patchManagerBanksLocked`; UNLOCK button has no padlock icon; no duplicate bank-select control in Internal Patches
**And** `selectedBank` APVTS property stays in sync

### Story 7.6: Internal Patches Panel Wiring

As a sound designer,
I want internal patch navigation and I/C/P/STORE wired,
So that RAM/ROM rules apply in UI (FR-22, FR-23, FR-24).

**Acceptance Criteria:**

**Given** Story 7.3
**When** user navigates with `<` `>` or NumberBox
**Then** wrap across banks when unlocked; ROM banks 2–9 gray PASTE/STORE with footer on attempt
**And** STORE sends patch per Matrix-1000 RAM rules

### Story 7.7: Settings Page Consolidation

As a power user,
I want Settings to host Master file ops, policies, Defrag, and logging opt-in,
So that advanced options live in one place (FR-40, FR-18, FR-59 manual Defrag, NFR-7).

**Acceptance Criteria:**

**Given** Stories 3.2, 6.10, 4.5
**When** user opens Settings
**Then** Master load/save/init, reconciliation policy, unsaved-warning policy, Defrag mutation history, INIT paths, logging opt-in are accessible
**And** manual Defrag disabled when history empty

### Story 7.8: Header Footer Shell and Persistence

As a user,
I want header routing, skin, scale, footer messages, and prefs persistence wired,
So that shell behaviour matches PRD (FR-3, FR-39, FR-42, FR-43, FR-44, FR-45, FR-41 conditional, UX-DR7–8).

**Acceptance Criteria:**

**Given** brownfield header/footer and Story 7.1
**When** user changes ports, skin, scale, or triggers grayed control
**Then** routing controls, skin/scale (or logo popup if FR-41 approved), and footer messages behave per PRD
**And** prefs persist across sessions per FR-3; graying rules FR-45 applied

### Story 7.9: Matrix Mod Bus Reorder UX Handler

As a sound designer,
I want drag bus labels to reorder modulation contents,
So that I can reorganize active buses visually (FR-50 UX side, UX-DR5).

**Acceptance Criteria:**

**Given** Stories 2.10 and 7.1
**When** user completes drag on bus number label
**Then** handler permutes Source/Amount/Destination values between rows and invokes Core reorder SysEx
**And** bus indices 0–9 remain fixed in UI

### Story 7.10: About Modal (Logo Popup)

As a Matrix-Control user,
I want an About dialog from the logo popup menu,
So that I can see version, authorship, and project links (FR-41 partial, UAT traceability).

**Acceptance Criteria:**

**Given** Story 7.7 Settings window pattern
**When** user opens logo popup and chooses **ABOUT...**
**Then** a read-only modal shows product info, version (`0.0.x-alpha` until 1.0.0 release), and release date
**And** logo popup entry reads **SETTINGS...** ; both menu columns are wide enough not to truncate labels

---

## Epic 8: Synth Connection & Instrument Role

Device detection, UI lock, and instrument plugin registration.

### Story 8.1: DeviceTypeRegistry and Member Byte Fix

As a developer,
I want device type resolved from Inquiry member bytes correctly,
So that M-1000 vs M-6/6R detection is accurate (FR-1, D-080, PRD §9 #6).

**Acceptance Criteria:**

**Given** Epic 2 MIDI stack
**When** Device ID reply is parsed
**Then** `DeviceTypeRegistry` maps member bytes to `deviceType`; known `SysExConstants` order bug fixed
**And** hardware verification note recorded if M-6 bytes still TBD

### Story 8.2: Device Inquiry and Footer Identity

As a user,
I want automatic Device Inquiry on port configure with footer identity,
So that I know which synth is connected (FR-1, FR-53).

**Acceptance Criteria:**

**Given** Story 8.1
**When** MIDI ports are configured
**Then** Universal Device Inquiry runs; valid Matrix family sets `deviceDetected`, `deviceType`, `deviceVersion`; footer right zone shows type and firmware
**And** timeout sets `deviceDetected=false`

### Story 8.3: UI Lock Without Synth

As a user,
I want editing disabled when no synth is detected,
So that I am guided to fix connection (FR-2).

**Acceptance Criteria:**

**Given** Story 8.2
**When** `deviceDetected=false`
**Then** PATCH/MASTER/Matrix Mod/PM/module I/C/P disabled; footer shows guidance; no SysEx or Program Change sent
**And** port reconfigure re-triggers inquiry

### Story 8.4: Virtual Instrument Registration and Bus Layout

As a user,
I want the plugin as AU/VST3 instrument with correct bus layout per host context,
So that Live/Reason/GarageBand host it without MIDI-effect workarounds (FR-4, FR-46, AD-11).

**Acceptance Criteria:**

**Given** Story 8.1 and Story R-1
**When** plugin loads and device type is known
**Then** hosted VST3/AU: Music Device / Instrument with **stereo output only** (no input bus)
**And** standalone: input channel layout follows device type (mono M-1000, stereo M-6/6R) via `StandaloneAudioInputRouter`
**And** M-6/6R grays entire MASTER EDIT section (FR-46)

### Story 8.5: Matrix-6/6R Patch Memory Limits

As a Matrix-6 or Matrix-6R owner,
I want PATCH MANAGER to use 100-patch memory semantics without banks,
So that navigation and STORE match my synth hardware (FR-46 extension, Story 8.5).

**Acceptance Criteria:**

**Given** Stories 8.1–8.2 (`deviceType` known) and `Matrix6Or6RLimits.h` added
**When** Matrix-6 or Matrix-6R is detected
**Then** `DeviceMemoryLimits` (or `DeviceTypeRegistry` helper) resolves patch/bank bounds from `Matrix6Or6RLimits` vs `Matrix1000Limits`; BANK UTILITY grayed; Internal Patches navigates 00–99 with cyclic wrap only; no Set Bank `0x0A` SysEx; FR-23 ROM gating applies to Matrix-1000 only
**And** Matrix-1000 FR-19–FR-24 behaviour unchanged; unit tests cover limit resolution

---

## Epic 9: Unsaved Edit Protection

Warn before losing unsaved patch edits.

### Story 9.1: DirtyPatchTracker

As a sound designer,
I want Core to track unsaved edits vs last loaded snapshot,
So that navigation can warn before data loss (FR-51 foundation).

**Acceptance Criteria:**

**Given** Epics 1 and 4 complete
**When** APVTS patch buffer diverges from last loaded/synced snapshot
**Then** `DirtyPatchTracker` reports dirty state
**And** unit tests cover load, edit, save, and reset cases

### Story 9.2: Unsaved Edit Confirmation Dialog

As a sound designer,
I want a confirmation when navigating away with unsaved edits,
So that I do not lose work accidentally (FR-51).

**Acceptance Criteria:**

**Given** Story 9.1 and Epic 7 navigation handlers
**When** user navigates internal patch or Computer Patches file while dirty
**Then** modal warns per Settings policy (always default / never expert); cancel stays on edit, confirm discards and loads
**And** does not block silent flows covered by FR-29/D-025

---

## Epic 10: Direct Interactive Display Editing

Remove Display→Slider bridge; direct APVTS writes from displays.

### Story 10.1: Remove InteractiveDisplayApvtsSync Bridge

As a developer,
I want the legacy display sync bridge removed,
So that displays write APVTS directly per AD refactor (E10, D-018).

**Acceptance Criteria:**

**Given** Epic 2 APVTS sync stable
**When** `PatchEditDisplaysPanel` is refactored
**Then** `InteractiveDisplayApvtsSync` (or equivalent bridge) is removed without regression in slider-driven edits
**And** GUI tests or manual checklist documented

### Story 10.2: EnvelopeDisplay Direct APVTS Editing

As a sound designer,
I want to drag envelope curves to edit parameters,
So that editing matches addendum geometry (FR-11, UX-DR1).

**Acceptance Criteria:**

**Given** Story 10.1
**When** user drags curve or moves slider on any of three envelope modules
**Then** APVTS updates bidirectionally; markers are small filled squares (D-017)
**And** geometry matches addendum § EnvelopeDisplay

### Story 10.3: TrackGeneratorDisplay Direct APVTS Editing

As a sound designer,
I want to drag track generator points on Y axis,
So that track editing is interactive (FR-12, UX-DR2).

**Acceptance Criteria:**

**Given** Story 10.1
**When** user drags any of five Y-only control points
**Then** APVTS updates per addendum geometry
**And** behaviour consistent with envelope direct-write pattern

---

## Epic U: UI Scale Audit & Pixel-Perfect Layout

Exhaustive prod UI scale audit per Figma ÷4 grid. **Canonical document:** `epic-ui-scale-audit-pixel-perfect-layout.md` (v1.1).

**Prerequisite track:** U-IDs (widget ID harmonization) → U-0 (`Design*` SSOT) → U-0b (Factory injection) before layout audits U-2…U-9.

### Story U-IDs: Widget ID Harmonization

As a maintainer,
I want all widget and parameter IDs to follow a single module-scoped naming convention,
So that Shared descriptors stay pixel-free and the Factory registry stays maintainable.

**Acceptance Criteria:**

**Given** `implementation-artifacts/u-ids-widget-id-harmonization-inventory.md` (75 renames)
**When** Story U-IDs merges
**Then** `PluginIDs.h`, `PluginDisplayNames.h`, and `PluginDescriptors*` reflect the inventory ; zero grep hits on old ID strings
**And** `bankUtilityLockBank` / `miscBankLockEnable` replace unlock/bank-lock IDs per epic spec

### Story U.0: Figma Intake & Design* Reconciliation

As a product owner,
I want Figma mockup dimensions captured in the compile-time design plan,
So that every panel and widget audit has authoritative 100% reference values.

**Acceptance Criteria:**

**Given** `_local/Documents/Notes/figma-mockup.md`
**When** Story U.0 merges
**Then** `DesignAtoms.h`, `DesignRecipes.h`, `DesignPanels.h`, `DesignChecks.h` under `Source/GUI/Layout/Design/` replace monolithic `PluginDesignDimensions.h`
**And** per-zone dimension tables feed U.2–U.9 UAT checklists

### Story U.0b: Factory Dimension Registry & Descriptor Decoupling

As a developer,
I want the Factory to be the sole consumer of the design plan,
So that widgets and panels stay generic and Descriptors contain no pixel fields.

**Acceptance Criteria:**

**Given** U.0 Design* headers and U-IDs complete
**When** Story U.0b merges
**Then** `WidgetDimensions.h` / `PanelDimensions.h` + Factory injection are in place ; `buttonWidth` removed from Descriptors
**And** `WidgetDimensionRegistry` + `PluginEditorDimensions` via Factory ; no Design includes in panels/widgets

### Story U.1: TestComponent Enrichment (D-064)

As a developer,
I want TestComponent to showcase every custom widget at all UI scale presets,
So that I can validate widgets in isolation before prod panel sign-off.

**Acceptance Criteria:**

**Given** `JUCE_DEBUG` build
**When** TestComponent is shown
**Then** all custom widget types are testable at presets 50–200%
**And** release build excludes sandbox sources (prep for U.10)

### Story U.2: Transversal Widgets Scale Audit

As a performer,
I want every custom widget to respect design dimensions at all UI scale presets,
So that controls look crisp inside any panel (UX-DR8).

**Acceptance Criteria:**

**Given** U.0b complete (Factory-injected dimensions)
**When** each widget in `Source/GUI/Widgets/` is reviewed
**Then** `ScaledLayout` + `ScaledDrawing` used consistently ; widgets do not include Design headers
**And** manual UAT passes at all seven presets

### Story U.3: Header Right Cluster Layout & Sign-Off

As a performer,
I want the header SCALE/SKIN/UI Elements cluster verified against Figma,
So that the bar matches mockup without reopening Epic 2 routing work.

**Acceptance Criteria:**

**Given** Story 2.11 frozen (regression only)
**When** header is audited at all presets
**Then** right cluster layout matches Figma ÷4
**And** FR-41 layout slots documented — popup behaviour remains Story 7.8

### Story U.4: Footer Panel Layout Audit

As a user,
I want footer zones and padding aligned to Figma at every UI scale,
So that messages read clearly.

**Acceptance Criteria:**

**Given** U.0 footer dimensions
**When** `FooterPanel` resizes at each preset
**Then** scaled padding/gaps match design
**And** manual UAT checklist completed

### Story U.5: Body Shell — Padding, Separators & Column Gaps

As a sound designer,
I want body columns and separators positioned per Figma,
So that the three-column grid aligns at every scale.

**Acceptance Criteria:**

**Given** U.0 shell dimensions
**When** `BodyPanel` lays out at each preset
**Then** column widths and separator heights match Design GUI stack ; originX float workaround removed
**And** manual UAT 50–200%

### Story U.6: Patch Edit Panels Layout Audit

As a sound designer,
I want PATCH EDIT sub-panels pixel-aligned to Figma,
So that the main editing area matches the mockup.

**Acceptance Criteria:**

**Given** U.0 Patch Edit dimensions
**When** top/middle/bottom panels resize at each preset
**Then** module grid and display band match design stack
**And** manual UAT per sub-panel

### Story U.7: Matrix Modulation Panel Layout Audit

As a sound designer,
I want ten modulation bus rows aligned to Figma,
So that Matrix Mod reads as a scalable grid.

**Acceptance Criteria:**

**Given** U.0 Matrix Mod dimensions
**When** `MatrixModulationPanel` resizes at each preset
**Then** row geometry matches `ModulationBusRow` SSOT
**And** manual UAT 50–200%

### Story U.8: Patch Manager Panels Layout Audit

As a sound designer,
I want all four Patch Manager modules aligned to Figma,
So that the manager column matches mockup density.

**Acceptance Criteria:**

**Given** U.0 Patch Manager dimensions
**When** four module panels resize at each preset
**Then** module height stack matches `PatchManagerSection`
**And** manual UAT per module

### Story U.9: Master Edit Panel Layout Audit

As a sound designer,
I want master modules stacked per Figma,
So that MASTER EDIT column height matches PATCH EDIT.

**Acceptance Criteria:**

**Given** U.0 Master Edit dimensions
**When** `MasterEditPanel` resizes at each preset
**Then** three-module stack matches design
**And** manual UAT 50–200%

### Story U.10: Release Gate — Prod Audit Sign-Off & D-062/D-063

As a product owner,
I want documented sign-off and release-safe sandbox policy,
So that v1 ships without debug UI clutter.

**Acceptance Criteria:**

**Given** U.2–U.9 UAT sheets signed
**When** Release build is produced
**Then** TestComponent excluded from release; debug retains sandbox
**And** D-062 criterion met with owner sign-off

---

## Final Validation (Step 4)

**Validated:** 2026-05-29 · **Workflow status:** complete

### FR Coverage Validation ✅

All **60 FRs** (FR-1–FR-60) mapped in § FR Coverage Map and addressed by at least one story acceptance criteria block. Spot-check: Mutator FR-54–FR-60 → Epic 6 stories 6.2–6.13; shell FR-39–FR-45 → Epic 7 story 7.8; FR-51 → Epic 9.

### Architecture Compliance ✅

- **Brownfield starter:** Architecture specifies extend baseline — Epic 0 covers P-001 migration (not greenfield clone). Correct.
- **Incremental Core creation:** PatchModel (1.1), queue (2.1), services created per story — no upfront “all services” story.
- **D-058 sequence:** E0→E10 preserved in epic ordering and story dependencies.
- **AD-10 / D-094:** Epic 0 story 0.3 covers ProjectPaths and logs gitignore.

### Story Quality Validation ✅

- **66 stories** — each with Given/When/Then AC and FR references (Epic U: U-IDs, U-0, U-0b added v1.1).
- **No forward within-epic dependencies** detected (e.g. Epic 6: 6.1 spec before 6.4 MUTATE).
- **Scope:** sized for single dev-agent sessions (Epic 2: 2.1–2.11 plus 2.9b UAT slice).

### Epic Structure Validation ✅

- User-value framing retained; Epic 0 is explicit enabler (acceptable brownfield).
- **File overlap note:** Epics 2, 7, 8 touch `PluginProcessor` — justified by D-058 phased wiring; consolidation rejected at architecture validation.

### Dependency Validation ✅

| Check | Result |
|---|---|
| Epic N usable without Epic N+1 | ✅ (E2 works without E3; E1 models testable without hardware) |
| Epic 6.4 blocked until 6.1 spec | ✅ intentional gate for Mutator algorithm |
| E0 before E1 new files under `src/` | ✅ Story 0.5 sign-off |

### UX-DR Coverage ✅

UX-DR1–UX-DR8 covered by stories 10.2, 10.3, 2.7/2.8, 6.8, 7.9, 6.6, 7.8.

### Open Items (non-blocking for sprint start)

- PRD §9 spikes embedded in stories: Matrix Mod opcode (2.6), SysEx throttle (2.2), M-6 member bytes (8.1), InitDefaults values (3.1), MutationAlgorithm spec (6.1).
- Dedicated UX spec (`bmad-ux`) still optional before E7 polish.

### Readiness Assessment

**Status:** **READY FOR DEVELOPMENT**

**First sprint recommendation:** Epic 0 (Stories 0.1–0.5), then Epic 1.

---

*Epics & Stories workflow complete — 2026-05-29.*

