---
title: 'Getting Started intro body copy — setup framing and Continue break'
type: 'chore'
created: '2026-10-09'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The Getting Started intro body opens the setup list with a bare "We'll set…", which feels abrupt for a first-run assistant, and the "Continue…" call to action sits in the same paragraph instead of breathing on its own line.

**Approach:** Keep the welcome paragraph. Reframe the second paragraph in standard setup-assistant English ("During this setup, we'll configure…"). Put "Continue, or choose Configure later…" after a blank line. Sync PluginDisplayNames, the frozen ui-copy companion, and the contract test.

</frozen-after-approval>

## Implementation Notes

- English framing chosen (product UI is English): "During this setup, we'll set…" (review softened away from "configure" to avoid echoing Configure later); kept ASCII punctuation.
- Split Continue CTA onto its own paragraph (`\n\n` before "Continue,").
- Touched: `PluginDisplayNames.h` (`kBodyIntro`), `GettingStartedWizardContractTests.cpp`, `ui-copy.md` companion, bumped `kBodyIntroDesignHeight` 110→156 for three paragraphs + slack; contract pins 156 and intro body for plugin + Standalone.
- Left historical decisions plan quote unchanged (archive); ui-copy provenance notes supersession.
- Follow-up (same session): Cream on STEP 1 made row labels vanish — Cream `kLabelText` is dark on Matrix dark plate. Wizard now always binds `skinBlack_` for chrome; control labels use `darkPanelLabelLookFromSkin`. Touched `PluginEditorGettingStarted.cpp`, `PluginEditorSkinScale.cpp`, `GettingStartedWizardDialogControls.cpp`, dialog header note.
- Follow-up: step title band separator `-` → `|` (e.g. `GETTING STARTED | STEP 1: USER INTERFACE`); sync PluginDisplayNames, contract test, ui-copy, journey-and-flags, user manual.

## Spec Change Log

## Review Triage Log

- Blind Hunter (intro height budget tight at 130) — medium → patch: bump `kBodyIntroDesignHeight` to 156 with wrap/slack comment; contract pins value.
- Blind Hunter (no measurement / HiDPI checklist) — low → rejected: measured body already grows under max-below-Settings; min budget is floor.
- Blind Hunter (tests don't pin height / plugin intro equality) — medium → patch: assert intro for plugin+Standalone and `kBodyIntroDesignHeight == 156`.
- Blind Hunter (ui-copy vs decisions plan archive) — medium → patch: ui-copy provenance supersession note; archive left unchanged → also deferred.
- Blind Hunter (GS-2 artifact stale geometry) — low → defer: done-spec drift only.
- Blind Hunter (empty Spec Change / Review Triage while in-progress) — false: filled on finalize.
- Blind Hunter ("configure" / Configure later echo) — medium → patch: wording → "we'll set".
- Blind Hunter (plugin format honesty of promissory setup list) — low → rejected: "(standalone application only)" already qualifies audio; keyboard remains optional wording.
- Blind Hunter (French `Étape 0` heading) — low → patch: rename to `STEP 0` in ui-copy.
- Blind Hunter (Black chrome vs Settings Cream plate mismatch) — false: product ask is Matrix monochrome independent of skin; Settings cream body is intentional form fill.
- Blind Hunter (setSkin API accepts any skin / no Cream chrome test) — low → defer: PluginEditor bind + dark-panel labels; no PluginEditor harness.
- Blind Hunter (Cream combo doesn't preview Cream in wizard) — false: intentional with Matrix chrome policy.
