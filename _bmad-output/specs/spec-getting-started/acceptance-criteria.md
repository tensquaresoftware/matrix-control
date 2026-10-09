# Acceptance criteria (testable)

Mapped to Epic GS stories and SPEC capabilities. Prefer demonstrable Given/When/Then for Build and review.

## GS-1 — Settings (CAP-1)

**AC-GS1-1 — User Interface order**  
**Given** Settings → User Interface  
**When** the section is visible  
**Then** options appear in order: UI SCALE, SKIN, INFO MESSAGE, CONTEXTUAL HELP

**AC-GS1-2 — Getting Started tab**  
**Given** Settings → GETTING STARTED (first tab)  
**When** the user inspects the SETUP WIZARD row  
**Then** a combo offers `SHOW WHEN INCOMPLETE` (default) and `NEVER SHOW AT LAUNCH`  
**And** `RUN SETUP AGAIN` appears below the combo (no second label beside the button)

**AC-GS1-3 — Logo shortcuts**  
**Given** the logo menu  
**When** the user opens it  
**Then** UI Scale and Skin shortcuts remain available (not Settings-only)

**AC-GS1-4 — Run Setup Again**  
**Given** incomplete or complete applicable flags  
**When** the user clicks `RUN SETUP AGAIN`  
**Then** applicable-to-format step flags reset to incomplete  
**And** the wizard opens at step 0

**AC-GS1-5 — Never show at launch**  
**Given** combo = `NEVER SHOW AT LAUNCH` and incomplete applicable steps  
**When** the editor becomes ready  
**Then** GETTING STARTED does not auto-open

**AC-GS1-6 — MIDI & DEVICE**  
**Given** Settings  
**When** the user opens MIDI & DEVICE  
**Then** synth MIDI ports, DEVICE (read-only), EPROM TYPE, and (plugin) HARDWARE LATENCY appear on that merged tab

---

## GS-2 — Wizard shell and navigation (CAP-2)

**AC-GS2-1 — Chrome and geometry**  
**Given** the wizard is open  
**When** the user compares it to Settings  
**Then** chrome is Matrix monochrome matching Settings overlay rules  
**And** design width equals Settings (`SettingsShellMetrics::kDesignWidth`)  
**And** per-step height is lower than Settings tallest page, **except** STEP 4 (full AUDIO embed) and STEP 2 when the firmware-suggestion body suffix is shown, which may exceed Settings height

**AC-GS2-2 — Titles and copy**  
**Given** any step  
**When** the dialog is shown  
**Then** the step title appears in the title band (not as a body heading duplicate)  
**And** body help text matches `ui-copy.md` for that step / format / resume case

**AC-GS2-3 — Button vocabulary**  
**Given** each step  
**When** the button row is shown  
**Then** buttons match `journey-and-flags.md`  
**And** neither `QUIT` nor `SPECIFY LATER` appears  
**And** STEP 2 has no dedicated Confirm — live controls + Next only

**AC-GS2-4 — Navigation**  
**Given** step 1+  
**When** the user presses PREVIOUS / NEXT  
**Then** navigation walks applicable steps only (plugin never lands on Audio)

---

## GS-3 — Steps, flags, plugin vs Standalone (CAP-3…CAP-6)

**AC-GS3-1 — STEP 1 content**  
**Given** STEP 1  
**When** the user changes UI Scale or Skin  
**Then** changes apply through the same preference paths as Settings / logo (no private Scale/Skin store)

**AC-GS3-2 — STEP 2 absorbs Device Setup**  
**Given** STEP 2  
**When** the user selects Synth From/To, sees DEVICE, and chooses EPROM  
**Then** ports write the same APVTS keys as header/Settings  
**And** DEVICE live states follow the existing Device Setup searching/not-connected/connected behavior  
**And** Next marks Synth Communication done without CONFIRM  
**And** first-run no longer depends solely on `settingsEpromTypePromptDone` + DEVICE SETUP modal as the onboarding vehicle

**AC-GS3-3 — STEP 3 Standalone**  
**Given** Standalone STEP 3  
**When** the step is shown  
**Then** Keyboard From combo is present with Skip and Standalone copy  
**And** Skip marks MIDI Keyboard done without requiring a port

**AC-GS3-4 — STEP 3 plugin**  
**Given** plugin STEP 3  
**When** the step is shown  
**Then** no Keyboard From combo is shown  
**And** copy states notes come from the host and points to the user manual  
**And** Next or Finish marks MIDI Keyboard done  
**And** Skip is absent

**AC-GS3-5 — STEP 4 Standalone only**  
**Given** Standalone last steps  
**When** the user reaches Audio  
**Then** the same controls as Settings → AUDIO appear per `journey-and-flags.md` (full page order, including blank spacers)  
**And** Finish marks Audio done  
**Given** plugin path  
**When** the user completes Keyboard  
**Then** Finish ends the wizard — no Audio step

**AC-GS3-6 — Auto-open resume**  
**Given** combo = `SHOW WHEN INCOMPLETE` and e.g. User Interface done, Synth Communication incomplete  
**When** the editor opens  
**Then** wizard opens at STEP 2 (not intro)  
**And** intro still appears for true first contact / `RUN SETUP AGAIN`

**AC-GS3-7 — Plugin then Standalone Audio**  
**Given** plugin completed applicable steps 1–3  
**When** the user later launches Standalone with Audio incomplete  
**Then** auto-open (if allowed) targets STEP 4 with resume Audio copy  
**And** does not force replaying completed MIDI/UI steps

**AC-GS3-8 — Configure later policy**  
**Given** step 0 CONFIGURE LATER  
**When** dismissed once  
**Then** unvisited steps stay incomplete and one later auto reminder is allowed on the **same format** only (the other format does not auto-open while that reminder is armed)  
**When** Configure later a second time (or the reminder opens and is consumed)  
**Then** auto-open stops until `RUN SETUP AGAIN` or combo → `SHOW WHEN INCOMPLETE`  
**When** a newly applicable step appears  
**Then** one targeted open is rearmed

**AC-GS3-9 — Audio-safety separation**  
**Given** Standalone cold start  
**When** audio-safety first-run Input None applies via `sceneAudioSafetyDefaultsApplied`  
**Then** wizard Audio completion flags are unchanged by that gate alone  
**And** completing wizard Audio does not satisfy or clear the audio-safety one-shot by implication of shared naming

**AC-GS3-10 — No dual onboarding vehicles**  
**Given** GETTING STARTED ships  
**When** the editor is ready on a machine that would have opened Device Setup  
**Then** GETTING STARTED is the onboarding path  
**And** DEVICE SETUP one-shot is not shown as a parallel first-run modal

**AC-GS3-11 — Migration**  
**Given** legacy `settingsEpromTypePromptDone` = true and no new wizard flags yet  
**When** preferences migrate  
**Then** Synth Communication is treated complete  
**And** other step flags remain incomplete unless separately completed

---

## Never (cross-cutting)

- Pastilles / coach marks / mega-modal “all Settings”
- Auto-open Settings alone as the first-run vehicle
- Duplicate MIDI/audio device list logic in the wizard
- Single shared setup-done flag for plugin + Standalone
- Merging wizard flags with `sceneAudioSafetyDefaultsApplied`
- QUIT / SPECIFY LATER / second full-setup button
- Detailed DAW examples in wizard body
- Changing frozen English copy without artifact update
- Treating GS-4 manual as a code AC of this Spec (doc track only)
