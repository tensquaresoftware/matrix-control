---
organization: Ten Square Software
project: Matrix-Control
title: PRD Kickoff Prompt
author: BMad Agent
sources:
  - brief.md
  - brownfield-inventory-for-prd.md
created: 2026-05-23
---

# PRD Kickoff Prompt

**Purpose:** Paste this message (or reference this file) when starting a new chat with `@bmad-prd`.  
**Companion artifacts:** `brief.md` v0.3, `brownfield-inventory-for-prd.md`

---

## Message to send

```
@bmad-prd

Intent: **Create** — premier PRD Matrix-Control (aucun prd.md existant dans _bmad-output/planning-artifacts/prds/).

## Contexte projet

Matrix-Control est un plugin JUCE 8 (VST3/AU/Standalone) — éditeur MIDI + instrument virtuel pour Oberheim Matrix-1000. Projet brownfield : UI et infrastructure APVTS déjà largement en place ; Core MIDI/SysEx écrit mais non câblé. Baseline git : `v0.0.66-alpha-pre-bmad`.

## Documents à lire en priorité (extract, don't ingest)

**Brief (source produit — autoritaire)**
- `_bmad-output/planning-artifacts/briefs/brief-Matrix-Control-2026-05-22/brief.md` (v0.3, status: ready)
- `_bmad-output/planning-artifacts/briefs/brief-Matrix-Control-2026-05-22/addendum.md`
- `_bmad-output/planning-artifacts/briefs/brief-Matrix-Control-2026-05-22/.decision-log.md`

**Inventaire brownfield C++ (source technique — à réconcilier avec le brief)**
- `_bmad-output/planning-artifacts/briefs/brief-Matrix-Control-2026-05-22/brownfield-inventory-for-prd.md`

**Contexte agent & conventions**
- `_bmad-output/project-context.md`
- `.cursorrules`

**Specs legacy (draft — NON autoritaires, à réconcilier explicitement)**
- `Documentation/Development/Specifications/Functional-Specification.md`

## Mode de travail demandé

**Coaching path** — pas de Fast path pour cette session.

**Point d'entrée : Vision + Features** (capability-first), mais avec une étape préalable obligatoire :

### Phase 0 — Brownfield decision workshop (AVANT de rédiger le PRD)

Je veux une **discussion guidée par questions ouvertes**, section par section, pour fixer pour chaque zone du code existant l'une de ces décisions :

| Décision | Signification |
|---|---|
| **KEEP** | Garder en l'état — ne pas refactorer, seulement câbler/compléter |
| **KEEP + IMPROVE** | Garder l'architecture, améliorer ciblé (tests, perf, finitions) |
| **REWIRE** | Garder l'UI/le code, remplacer le wiring stub (ex. timestamps → Core) |
| **REFACTOR** | Refonte partielle ou totale de la zone |
| **DISCARD** | Abandonner (ex. plans OpenGL, patterns dev temporaires) |
| **BUILD NEW** | N'existe pas encore — spécifier from scratch |

Parcours suggéré (une zone à la fois, 2–4 questions max par tour) :

1. **Descriptors / APVTS / WidgetFactory** — garder comme invariant architectural ?
2. **GUI shell** (skins, UI scale, panels, widgets custom) — keep vs refactor ?
3. **Displays interactifs** (EnvelopeDisplay, TrackGeneratorDisplay) — keep ?
4. **Patch Manager UI** — keep UI + rewire Core ?
5. **Stubs I/C/P et boutons Patch Manager** — pattern timestamp → ActionDispatcher ?
6. **Stack SysEx** (Parser/Encoder/Decoder) — keep + étendre tests ?
7. **MidiManager + thread MIDI dédié** — keep API, implémenter inbound + queue dual-role ?
8. **PluginProcessor** — migration Effect → Instrument virtuel (CMake + processBlock) ?
9. **Composants manquants** (PatchModel, MasterModel, PatchFileService, Mutator, Clipboard) — BUILD NEW : prioriser dans le PRD ?
10. **Dev tooling** (TestComponent/UI Elements) — garder jusqu'à quand ?

Pour chaque réponse, enregistre la décision dans `.decision-log.md` du workspace PRD avec rationale courte.

Ne passe à la rédaction des FR qu'une fois la Phase 0 terminée (ou que j'indique explicitement « on peut avancer »).

## Calibrage enjeux

- **Launch** — produit open-source public (MIT), référence JUCE, release aspirational Noël 2026
- Audience PRD : moi (PO/dev solo expert) + futurs contributeurs + workflows BMad downstream (UX, Architecture, Epics)
- Langue conversation : **français** ; document PRD : **anglais**

## Contraintes à respecter dans le PRD

- Brief v0.3 prime sur les specs legacy
- Dual-role architecture (Instrument + Editor, un track DAW) — non négociable
- Critère succès #1 : fiabilité SysEx (4h+ sans hang du synthé)
- v1 ambitieuse complète (PATCH + MASTER + PATCH MANAGER incl. Mutator) — pas MVP
- Matrix-6/6R v1 : PATCH only ; GROUPS multi-unit, Request All Bank, OpenGL : out of scope
- Capabilities dans le PRD ; choix techniques dans addendum.md

## Ce que j'attends de toi maintenant

1. Confirmer intent Create + workspace path
2. Lire les documents listés (via subagents si disponible)
3. Me poser **3–5 premières questions ouvertes** sur la Phase 0 — commencer par la couche qui te semble la plus structurante pour les décisions downstream (probablement Descriptors/APVTS ou GUI shell)
4. Ne pas rédiger de FR ni de sections PRD avant la fin de la Phase 0

Commence quand tu es prêt.
```

---

## Short reference (for @-mention)

In a new chat, you can also start with:

```
@bmad-prd

Intent: Create. Coaching path. Phase 0 brownfield workshop before any FR drafting.

Read and follow: _bmad-output/planning-artifacts/briefs/brief-Matrix-Control-2026-05-22/prd-kickoff-prompt.md
```
