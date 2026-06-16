---
organization: Ten Square Software
project: Matrix-Control
title: Passation — notes MIDI coincées & lag 1ère édition slider
author: BMad Agent
status: ready-for-dev
sources:
  - implementation-artifacts/manual-uat-epic2-midi-audio-hardware.md
  - implementation-artifacts/2-6-matrix-mod-bus-parameter-sysex.md
  - implementation-artifacts/2-9-wire-midimanager-queue-consumer.md
  - implementation-artifacts/r-4-midioutboundqueue-mpsc-audit.md
created: 2026-06-16
updated: 2026-06-16
priority: high
---

# Passation — bugs MIDI post-UAT Epic 2

Document de passation dev pour deux anomalies observées par Guillaume lors de l'UAT hardware (`manual-uat-epic2-midi-audio-hardware.md`, session 2026-06-16). ComboBoxes **non testées** — périmètre limité aux **sliders** pour le lag.

---

## Synthèse exécutive

| ID | Symptôme | Sévérité | Tests UAT |
|:---|:---|:---:|:---|
| **BUG-MIDI-01** | Notes qui restent coincées (pas de Note Off) pendant édition Matrix Mod | **Haute** | `A-VST3-7`, `B-AU-1` |
| **BUG-MIDI-02** | Petit lag perceptible à la **première** édition d'un slider, puis réactif | Moyenne | `A-VST3-4`, `B-AU-1`, `C-4` |

Epic 2 reste **validé** (G1–G9 ✅) — ces bugs sont des **correctifs post-UAT**, pas des blockers de clôture epic.

---

## BUG-MIDI-01 — Notes MIDI coincées pendant Matrix Mod

### Symptôme utilisateur

Pendant l'édition du bus Matrix Mod (ex. bus 0 : LFO 1 → VCF FREQ), des notes tenues au clavier **restent parfois actives** après relâchement — comme si le Matrix-1000 n'avait pas reçu les Note Off.

Reproduit en **VST3** et **AU** (Live 12.4.2). En standalone (`C-5`), drag-reorder bus → **notes coupées** (différent) mais pas de notes coincées signalées.

### Contexte de reproduction

1. Live : piste MIDI armée, Monitor **In**
2. Jouer des notes **pendant** qu'on modifie SOURCE / AMOUNT / DESTINATION sur un bus Matrix Mod
3. Relâcher les touches → certaines notes restent actives sur le Matrix

### Hypothèses classées (à investiguer)

#### H1 — Consumer MIDI bloqué pendant `sendSysExWithDelay` (**piste principale**)

`MidiManager::run()` est le **seul** thread qui draine la queue. Chaque SysEx passe par :

```272:278:Source/Core/MIDI/MidiManager.cpp
void MidiManager::sendSysExWithDelay(const juce::MemoryBlock& sysExMessage, const juce::String& description)
{
    sysExDelay_.waitUntilReady();
    midiSender->sendSysEx(sysExMessage);
    // ...
    sysExDelay_.recordSysExSent(...);
}
```

`waitUntilReady()` peut **sleep** sur le thread consumer (10 ms stock M-1000, `SysExDelayProfile::kStockDelayMsMatrix1000`).

Pendant ce sleep :
- Les Note Off continuent d'être **enqueued** (`InstrumentMidiForwarder` → `enqueueRealtime`)
- Mais **aucun dequeue** tant que le consumer est bloqué dans `sendSysExWithDelay`
- La priorité realtime-before-SysEx (`MidiOutboundQueue::dequeue`) ne s'applique qu'**entre** deux appels `dequeue`, pas pendant un envoi SysEx en cours

**Scénario Matrix Mod :** chaque tweak de paramètre bus déclenche un 0x0B (`MatrixModBusParameterSysExDispatcher`). Édition rapide ou enchaînement SOURCE+AMOUNT+DESTINATION → rafale SysEx → consumer bloqué → Note Off retardés ou noyés si le synthé timeout localement.

#### H2 — Rafale APVTS → plusieurs SysEx par geste UI

`valueTreePropertyChanged` peut dispatcher un 0x0B par paramètre Matrix Mod touché. Un seul geste utilisateur peut générer plusieurs messages si plusieurs propriétés bougent.

Vérifier : logs MIDI (`Logs/MIDI/`) pendant repro — compter 0x0B/s et corréler avec Note Off manquants.

#### H3 — Conflit instrument path / flood édition (moins probable)

La queue priorise bien realtime avant SysEx **à chaque dequeue**. Si H1 est faux, chercher perte de messages côté `InstrumentMidiForwarder` (buffer host Live vidé ?) ou filtrage accidental.

### Pistes de correction

| Piste | Description | Effort |
|:---|:---|:---:|
| **P1** | Ne pas bloquer le consumer pendant le délai inter-SysEx : envoyer SysEx, enregistrer timestamp, **revenir à la boucle dequeue** ; reporter l'attente au prochain SysEx uniquement | Moyen |
| **P2** | **Coalescing** Matrix Mod : un seul 0x0B par bus par frame UI / debounce 5–15 ms sur `MatrixModBusParameterSysExDispatcher` | Faible |
| **P3** | **Budget realtime** : après chaque SysEx envoyé, drainer N messages realtime avant le suivant | Moyen |
| **P4** | **All-notes-off** de secours (CC 123 ou Note Off forcés) quand la queue realtime dépasse un seuil — filet de sécurité, pas fix root cause | Faible |

**Recommandation :** repro instrumentée → confirmer H1 → implémenter **P1 + P2** en premier.

### Plan de repro dev

1. Activer `MidiLogger` + trace queue depth (ajouter compteur temporaire si besoin)
2. Live armé, tenir 3 notes, tourner AMOUNT bus 0 en continu 5 s
3. Mesurer : temps entre Note Off host et Note Off sur fil MIDI OUT ; profondeur `realtimeQueue_` pendant rafale 0x0B
4. Répéter standalone (Keyboard From) pour comparer

### Critères d'acceptation correctif

- [ ] Tenir 3+ notes + éditer Matrix Mod (slider bus) 30 s → **aucune** note fantôme
- [ ] VST3 + AU + standalone
- [ ] Pas de régression latence édition patch (0x06) ni reorder bus (2.10)
- [ ] Tests unitaires queue / dispatcher mis à jour si comportement changé

### Fichiers probables

| Fichier | Rôle |
|:---|:---|
| `Source/Core/MIDI/MidiManager.cpp` | `run()`, `dispatchOutboundMessage`, `sendSysExWithDelay` |
| `Source/Core/MIDI/Queue/MidiOutboundQueue.{h,cpp}` | Priorité dequeue |
| `Source/Core/MIDI/Queue/SysExInterMessageDelay.{h,cpp}` | Sleep consumer |
| `Source/Core/MIDI/MatrixModBusParameterSysExDispatcher.cpp` | Coalescing candidat |
| `Source/Core/PluginProcessor.cpp` | `valueTreePropertyChanged` Matrix Mod branch |
| `Source/Core/Audio/InstrumentMidiForwarder.cpp` | Note On/Off enqueue |

---

## BUG-MIDI-02 — Lag première édition slider

### Symptôme utilisateur

À la **première** modification d'un slider (ex. DCO 1 → FREQUENCY), délai perceptible (~100–300 ms ressenti ?) avant changement sonore. Les mouvements suivants sont **réactifs**.

Observé : VST3, AU, standalone. **ComboBoxes non testées** — ne pas généraliser au-delà des sliders sans repro.

### Contexte de reproduction

1. Session fraîche (ou après longue pause sans édition)
2. Premier mouvement sur un slider PATCH ou MASTER
3. Lag ; puis fluidité normale

### Hypothèses classées

#### H1 — Cold start queue / consumer (`midiSender->isOutputAvailable()`)

```397:414:Source/Core/MIDI/MidiManager.cpp
if (!midiSender->isOutputAvailable())
{
    wait(5);
    continue;
}
```

Si le port MIDI TO vient d'être ouvert, les premiers cycles peuvent boucler sur `wait(5)` avant le premier envoi.

#### H2 — Premier SysEx 0x06 / 0x03 — chemin encode + enqueue froid

Premier `PatchParameterSysExDispatcher` ou `MasterParameterSysExDispatcher` : allocation, `apvtsToBuffer`, encode — coût one-shot message thread, pas consumer.

#### H3 — Slider JUCE : premier `valueTreePropertyChanged` vs automation

Premier drag peut déclencher focus / repaint / sync APVTS plus lourd. Vérifier si **un seul** SysEx part ou une rafale au premier pixel.

#### H4 — Délai inter-SysEx résiduel

Si un SysEx précédent (autre paramètre, inquiry manuel, test) a été envoyé récemment, `waitUntilReady()` applique le gap 10 ms — insuffisant pour expliquer seul un gros lag, sauf enchaînement.

### Pistes de correction

| Piste | Description |
|:---|:---|
| **P1** | Warm-up : après `setMidiOutputPort` réussi, envoyer un 0x06 bénin ou ping queue (à évaluer côté hardware) |
| **P2** | Réduire `wait(5)` quand port vient d'être ouvert ; ou flag `outputReady` |
| **P3** | Profiler premier `valueTreePropertyChanged` → encode → enqueue (message thread) |
| **P4** | Si combo boxes OK : classer bug slider-only (UI widget path) |

**Recommandation :** instrumentation timestamps (T0 slider moved → T1 enqueue → T2 send) sur **une** session ; trancher H1 vs H2 avant fix.

### Plan de repro dev

1. Log hi-res : `valueTreePropertyChanged`, `enqueueSysEx`, `dispatchOutboundMessage`, `sendSysEx`
2. Session neuve, un seul cran DCO 1 FREQUENCY
3. Comparer delta T avec second cran immédiat
4. Répéter après 30 s idle

### Critères d'acceptation correctif

- [ ] Premier mouvement slider PATCH et MASTER : latence **indistinguable** du second (à l'oreille + < 50 ms mesuré fil MIDI si possible)
- [ ] VST3 + standalone minimum
- [ ] Pas de SysEx spam additionnel au warm-up

### Fichiers probables

| Fichier | Rôle |
|:---|:---|
| `Source/Core/MIDI/MidiManager.cpp` | Consumer loop, `isOutputAvailable` |
| `Source/Core/PluginProcessor.cpp` | `valueTreePropertyChanged`, dispatchers |
| `Source/Core/MIDI/PatchParameterSysExDispatcher.cpp` | Premier 0x06 |
| `Source/GUI/Widgets/Slider.cpp` (si existe) | Premier drag |

---

## Hors périmètre de cette passation

| Sujet | Raison |
|:---|:---|
| ComboBoxes Matrix Mod / PATCH | Non testées par l'utilisateur |
| Peak meter bagotter (`C-6`) | UX audio — story séparée |
| HARDWARE LATENCY sémantique | Story R-2 / doc |
| Notes coupées au drag-reorder (`C-5`) | Lié possiblement à BUG-MIDI-01 — retester après fix |

---

## Proposition de story / epic

Ces bugs ne justifient pas une epic dédiée. Options :

1. **Epic R follow-up** : `r-6-midi-realtime-starvation-fix` (BUG-MIDI-01)
2. **Chore Epic 2** : `2-12-first-parameter-edit-latency` (BUG-MIDI-02) — après diagnostic
3. Ou ticket unique `bugfix/uat-midi-notes-and-slider-lag` si préférence hors BMad

Priorité implémentation : **BUG-MIDI-01** avant tout.

---

## Références

- UAT : `manual-uat-epic2-midi-audio-hardware.md` — §4 A-VST3-4/7, §5 B-AU-1, §6 C-4
- AD-3 queue : `architecture.md` — realtime before SysEx
- Audit R-4 : `r-4-midioutboundqueue-mpsc-audit.md` — MPSC mutex validé, pas de changement queue requis pour audit
