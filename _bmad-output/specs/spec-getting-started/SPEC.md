---
id: SPEC-getting-started
companions:
  - journey-and-flags.md
  - acceptance-criteria.md
  - code-map.md
  - ui-copy.md
  - smoke-verification.md
  - ../../implementation-artifacts/guide-matrix-modal-design.md
  - ../../implementation-artifacts/guide-smoke-window-modal-look.md
  - ../../../CONVENTIONS.md
  - ../../project-context.md
sources:
  - ../../../Documentation/Development/Plans/2026/10/2026-10-08-Getting-Started-Wizard-Decisions.md
  - ../../planning-artifacts/sprint-change-proposal-2026-10-08-getting-started.md
  - ../../planning-artifacts/epics.md
  - ../../implementation-artifacts/deferred-work.md
  - ../../implementation-artifacts/spec-device-setup-assistant.md
  - ../../implementation-artifacts/spec-device-setup-cross-project-friction.md
  - ../../implementation-artifacts/spec-device-setup-welcome-intro.md
---

> **Canonical contract.** This SPEC and the files in `companions:` are the complete, preservation-validated contract for what to build, test, and validate. Source documents listed in frontmatter are for traceability — consult them only if you need narrative rationale or prose color this contract intentionally omits.

# GETTING STARTED wizard (Epic GS)

## Why

Matrix-Control users must unlock **edit, play, and hear** on first useful contact. Today’s Device Setup is a one-shot MIDI + EPROM modal; Settings already unifies audio and MIDI, but Standalone still lacks a guided path that includes appearance, keyboard, and monitoring, while plugin hosts must not walk OS Audio / Keyboard From like Standalone. A vague “First-run setup assistant” intention was retired in Correct Course 2026-10-08 in favor of a dedicated multi-step **GETTING STARTED** wizard with Settings controls — this Spec is the implementation contract for that locked product direction (Epic GS-1…GS-3).

## Capabilities

- **CAP-1** (GS-1)
  - **intent:** User can set UI Scale and Skin from Settings → User Interface, and control Getting Started auto-open / re-run from Settings → GETTING STARTED, while logo menu Scale/Skin shortcuts remain available.
  - **success:** Settings tabs include GETTING STARTED (first) with row SETUP WIZARD (combo `SHOW WHEN INCOMPLETE` / `NEVER SHOW AT LAUNCH` + `RUN SETUP AGAIN`); User Interface order is UI SCALE, SKIN, INFO MESSAGE, CONTEXTUAL HELP; MIDI and DEVICE are merged as MIDI & DEVICE; logo shortcuts still open Scale/Skin. See `acceptance-criteria.md` §GS-1.

- **CAP-2** (GS-2)
  - **intent:** User can move through a Matrix-chrome multi-step GETTING STARTED dialog with few controls per step, title-band step titles, frozen English body copy, and the locked button vocabulary.
  - **success:** Dialog uses Matrix monochrome chrome like Settings; design width equals Settings; per-step height is lower than Settings; titles live in the title band; body copy matches `ui-copy.md`; buttons are only CONFIGURE LATER / CONTINUE / PREVIOUS / NEXT / SKIP / FINISH as specified in `journey-and-flags.md` (no QUIT, no SPECIFY LATER). See `acceptance-criteria.md` §GS-2.

- **CAP-3** (GS-3 content)
  - **intent:** User completes only format-applicable steps: appearance, synth MIDI/DEVICE/EPROM, keyboard path for their format, and Standalone audio monitoring when applicable.
  - **success:** Step contents and applicability match `journey-and-flags.md` (0 intro → 1 Scale/Skin → 2 Synth From/To + DEVICE + EPROM live → 3 Keyboard Standalone combo+Skip or plugin informative → 4 Audio Standalone only). Plugin never shows Keyboard From combo or Audio step. UltraWide/HiDPI can fix Scale in STEP 1 before denser steps.

- **CAP-4** (GS-3 flags)
  - **intent:** System tracks durable per-step progress and auto-opens or resumes only when applicable incomplete steps remain and the user allows auto-open.
  - **success:** One flag per step (User Interface, Synth Communication, MIDI Keyboard, Audio); Skip Keyboard marks Keyboard done; auto-open with `SHOW WHEN INCOMPLETE` resumes at first applicable incomplete step without replaying intro except first contact / never Continued; `RUN SETUP AGAIN` resets flags applicable to the current format and opens step 0; plugin and Standalone do not share a single “setup done” flag. Distinct from `sceneAudioSafetyDefaultsApplied`. See `journey-and-flags.md`.

- **CAP-5** (GS-3 Configure later)
  - **intent:** User can defer setup from the intro without being harassed, and still get a targeted prompt when a newly applicable step appears.
  - **success:** CONFIGURE LATER closes without marking unvisited steps done; at most one auto reminder on next launch (same format only — other format does not open while armed) while incomplete applicable work remains; consuming that reminder or a second Configure later silences until `RUN SETUP AGAIN` or combo returns to `SHOW WHEN INCOMPLETE`; newly applicable step (e.g. first Standalone Audio) rearms one targeted open.

- **CAP-6** (GS-3 absorb)
  - **intent:** Device Setup one-shot onboarding is absorbed into GETTING STARTED STEP 2 so users and tracking have a single first-run path.
  - **success:** STEP 2 reuses shared MIDI/DEVICE/EPROM bricks (no duplicated port lists); live writes like Settings/header; Next marks Synth Communication done (no dedicated Confirm); CONFIRM / SPECIFY LATER / sole `settingsEpromTypePromptDone` completion model retired as the onboarding gate; no parallel Device Setup élargi vehicle remains. Migration assumption in Assumptions.

## Constraints

- Product decisions in the 2026-10-08 plan are **frozen** — do not reopen pastilles, mega-modal, Settings-only auto-open, manual-only, or Device Setup-only Standalone paths.
- English UI strings only; ASCII display strings per project rules; copy text is SSOT in `ui-copy.md` (plan §6).
- Reuse Settings/header MIDI and audio population logic — **never** fork a second device-list SSOT inside the wizard.
- Wizard chrome follows `guide-matrix-modal-design.md`: Matrix monochrome; **same design width as Settings** (`SettingsShellMetrics::kDesignWidth` = 400); per-step height normally lower than Settings; **exceptions:** STEP 4 (full AUDIO embed) and STEP 2 when the firmware-suggestion body suffix is shown may exceed Settings height; step titles in title band.
- Plugin ≠ Standalone applicability must be reflected in navigation, flags, and AC (Keyboard From / Audio Standalone-only; plugin STEP 3 informative).
- Wizard per-step flags must **not** be merged with audio-safety first-run `sceneAudioSafetyDefaultsApplied` (Input None gate).
- No second “full setup” button; `RUN SETUP AGAIN` is the sole re-run control.
- STEP 4 shows the same controls as Settings → AUDIO (full order including blank spacers); the wizard may grow taller than Settings for this step.
- Implementation order: **GS-1** (Settings UI) before or as opening work of the wizard Build; then shell (GS-2); then steps/flags/absorb (GS-3). See `code-map.md`.
- GS-4 (user manual first launch + host keyboard examples) is a **doc dependency**, not a code deliverable of this Spec.
- Historical Device Setup specs remain implementation record only — superseded for future work by this Spec / Epic GS.
- Quality: JUCE 9.0.1; builds under `Builds/`; `Scripts/quality/lint_touched.py` on touched C++.

## Non-goals

- Implementing or rewriting the user manual (GS-4) beyond noting the dependency.
- Reopening discarded UX approaches (pastilles, mega-modal, Settings auto-open alone, manual alone).
- Inventing alternate frozen copy or a second full-setup button.
- Detailed DAW examples inside the wizard body (manual only).
- Audio Settings Matrix rebuild (smoke execution order #6) — separate chantier.
- Expanding Device Setup as a parallel “First-run setup assistant” / mega-modal.
- Changing Epic 7 / Epic 8 shipped Settings unification as incomplete work.

## Success signal

On a fresh machine preference state, Standalone auto-opens GETTING STARTED and the user can set Scale/Skin, wire synth MIDI + EPROM, optionally Skip keyboard, finish Audio digeste controls, and hear the synth — without a second Settings-sized mega-modal. Plugin path skips Audio and presents informative Keyboard STEP 3. Settings → GETTING STARTED can suppress auto-open or `RUN SETUP AGAIN`. Device Setup one-shot no longer owns first-run. UltraWide smoke confirms Scale in STEP 1 before denser steps. Checklist: `smoke-verification.md`.

## Assumptions

- Wizard design width equals Settings (`SettingsShellMetrics::kDesignWidth` = 400); exact per-step heights are Build layout choices — normally below Settings, with STEP 2 (firmware suffix) and STEP 4 (AUDIO embed) allowed to exceed.
- Migration: if legacy `settingsEpromTypePromptDone` is true, mark **Synth Communication** complete; leave other step flags incomplete so Scale / Keyboard / Audio can still auto-open under `SHOW WHEN INCOMPLETE`.
- STEP 2 Next persists current live MIDI/EPROM selections (same APVTS / machine defaults as header/Settings) and marks the step done — no Confirm gate.
- Exact ApplicationProperties / PluginIDs key names for wizard flags, auto-open combo, and Configure-later silence are a Build naming choice consistent with existing MachineDefaults style (see Open Questions).

## Open Questions

- Exact property key names for per-step flags, auto-open preference, and Configure-later silence counter — Build may choose names; document them in the story/spec notes when implemented (not a product reopen).

## Review Findings

Epic-wide code review (`efadd3b0^..HEAD`, 2026-10-09).

### Decision-needed

- [x] [Review][Decision] Settings IA vs CAP-1 SSOT — resolved 2026-10-09: **adopt polish** (tab GETTING STARTED + SETUP WIZARD + MIDI & DEVICE); update SPEC / journey / smoke / ui-copy → see Patch below
- [x] [Review][Decision] Configure Later cross-format while OneReminder armed — resolved 2026-10-09: **fix** — do not auto-open on the other format while OneReminder is armed → see Patch below
- [x] [Review][Decision] Wizard height vs Settings ceiling — resolved 2026-10-09: **validate exceptions** (STEP 2 firmware suffix + STEP 4 Audio may exceed Settings) in AC / smoke → see Patch below
- [x] [Review][Decision] STEP 2 port labels MIDI FROM/TO — resolved 2026-10-09: **rename now** to SYNTH FROM / SYNTH TO → see Patch below

### Patch

- [x] [Review][Patch] Align CAP-1 / journey / smoke / ui-copy / epic context with shipped Settings IA (GETTING STARTED tab, SETUP WIZARD row, MIDI & DEVICE) — applied 2026-10-09
- [x] [Review][Patch] `decideAutoOpen`: while OneReminder armed, open only on the same format that armed; update flags contract test — applied 2026-10-09
- [x] [Review][Patch] AC-GS2-1 / smoke: document STEP 2 (firmware suffix) and STEP 4 may exceed Settings height — applied 2026-10-09
- [x] [Review][Patch] Wizard STEP 2 port labels → SYNTH FROM / SYNTH TO; update ui-copy + manual — applied 2026-10-09
- [x] [Review][Patch] Manual §15 menu table Settings tabs — applied 2026-10-09
- [x] [Review][Patch] Troubleshooting HARDWARE LATENCY → MIDI & DEVICE — applied 2026-10-09
- [x] [Review][Patch] Footer no-synth copy → Settings > MIDI & DEVICE > SYNTH FROM / SYNTH TO — applied 2026-10-09
- [x] [Review][Patch] SETUP WIZARD contextual help (NEVER SHOW + silence clear) — applied 2026-10-09
- [x] [Review][Patch] Re-entering STEP 2 uses live preferred EPROM resolver — applied 2026-10-09
- [x] [Review][Patch] Entering STEP 4 refreshes SYNTH FROM catalog live — applied 2026-10-09
- [x] [Review][Patch] ComboBox popup-only ` *` mark unit test (`ComboBoxPopupMark`) — applied 2026-10-09
- [x] [Review][Patch] Inquiry→EPROM `writeStoredTypeAfterInquirySuccess` + ValueTree test — applied 2026-10-09
- [x] [Review][Patch] One-reminder silence via `configureLaterArmAfterAutoOpen` asserted — applied 2026-10-09

### Defer

- [x] [Review][Defer] Escape / outside-click dismiss vs CONFIGURE LATER arm advance — deferred: GUI keyPressed/hit-test; NavButton dismiss-only already contracted; pure dismiss-kind collaborator only if policy keeps biting
- [x] [Review][Defer] STEP 4 Finish with null audio page / null AudioDeviceManager — deferred: maybe-false Standalone timing; related null-manager deferral already in deferred-work from GS-3 review

### Rejected

- Manual Configure Later silence incomplete — false: §5 already states reminder open or second CONFIGURE LATER silences
- Inquiry always overwriting user EPROM / UNKNOWN on unmapped firmware — false: intentional auto-track (manual + `storedTypeAfterInquirySuccess`); not a defect
- `nextDeviceSetupEpromPreferredId` ignoring `userTouched` as product bug — false: documented auto-track; unused touch flag is leftover API (not user-facing harm by itself)
- CMakeLists MATRIX_BUILD_TESTS indent next to EpromTypePolicy — low: cosmetic, unlikely everyday harm
- Orphan `kGettingStartedLabel` alone — absorbed into Settings IA decision (not a separate patch)
