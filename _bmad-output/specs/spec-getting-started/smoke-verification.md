# Smoke / manual verification

Aligned with `guide-smoke-window-modal-look.md` § GETTING STARTED. Run after Build before calling Epic GS code stories done.

## Core checklist

- [ ] Chrome Matrix monochrome; width = Settings; height lower per step except STEP 2 (firmware suffix) and STEP 4 (AUDIO embed) which may exceed Settings
- [ ] Steps: 0 intro → 1 UI Scale + Skin → 2 SYNTH FROM/TO + DEVICE + EPROM → 3 Keyboard (Standalone combo + Skip; plugin informative, no combo) → 4 Audio Standalone only (same as Settings → AUDIO)
- [ ] Title band carries step titles; body copy matches `ui-copy.md`
- [ ] Buttons: CONFIGURE LATER / CONTINUE / PREVIOUS / NEXT / SKIP / FINISH only as specified
- [ ] Settings → GETTING STARTED tab first: SETUP WIZARD combo + `RUN SETUP AGAIN`; User Interface: UI SCALE, SKIN, INFO MESSAGE, CONTEXTUAL HELP; MIDI & DEVICE merged
- [ ] Combo `SHOW WHEN INCOMPLETE` / `NEVER SHOW AT LAUNCH` + `RUN SETUP AGAIN`
- [ ] Logo Scale / Skin shortcuts still work
- [ ] Per-step flags + targeted resume; no single shared plugin+Standalone “done”
- [ ] Configure later: one same-format reminder then silence (other format does not open while armed); rearm on newly applicable (first Standalone Audio)
- [ ] Device Setup one-shot no longer opens as parallel first-run
- [ ] `sceneAudioSafetyDefaultsApplied` Input None behavior still independent

## Format matrix

| Scenario | Expect |
|----------|--------|
| Fresh Standalone | Auto-open (if SHOW WHEN INCOMPLETE); full 0→4 path available |
| Fresh plugin | No Audio step; STEP 3 informative; Finish on STEP 3 |
| Plugin done → later Standalone | Targeted STEP 4 with resume Audio copy when Audio incomplete |
| NEVER SHOW AT LAUNCH | No auto-open; Run Setup Again works |
| RUN SETUP AGAIN | Resets applicable flags; opens step 0 |

## UltraWide / HiDPI

- [ ] On a wide / HiDPI display, STEP 1 Scale can be raised before STEPS 2–4 so dense controls remain readable (intentional Scale-first — not a mega-modal)

## Out of this smoke

- Audio Settings Matrix rebuild (execution order #6) — separate chantier
- Full DAW keyboard routing examples — GS-4 manual
