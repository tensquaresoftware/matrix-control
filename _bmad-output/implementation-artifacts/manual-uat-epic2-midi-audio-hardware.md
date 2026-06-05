---
organization: Ten Square Software
project: Matrix-Control
title: UAT manuel Epic 2 — MIDI (Editor + Instrument) & Audio
author: Guillaume DUPONT
status: draft
created: 2026-06-05
scope: Stories 2.3–2.9b, 2.7, 2.8, 2.11 (avant US 2-10)
target_hardware: Oberheim Matrix-1000
target_host: Ableton Live 12 Suite (VST3 + AU) + Standalone macOS
---

# UAT manuel Epic 2 — MIDI & Audio avec Matrix-1000

Guide pas-à-pas pour compléter les tests automatisés (code review / unit tests) par une validation **réelle** sur ton Matrix-1000, avant de démarrer la story **2-10** (reorder SysEx Matrix Mod).

---

## 0. Comment noter tes résultats (Typora)

Chaque checklist est un **tableau** à remplir au fil de la session.

| Colonne | Usage |
|---|---|
| **ID** | Référence du test (ne pas modifier) |
| **Test** | Action + résultat attendu |
| **Résultat** | Colle **✅** ou **❌** après chaque test |
| **Notes** | Optionnel — détail si ❌ ou échec partiel |

### Raccourci Typora

Symboles à copier-coller : **✅** **❌** **⚠️**

1. Copie ✅ ou ❌ une fois (ligne ci-dessus ou légende).
2. Après chaque test : cellule **Résultat** → `⌘V`.
3. Échec partiel : **❌** + note courte dans **Notes** (ou **⚠️** si bloquant mineur).

### Légende

| Symbole | Signification |
|:---:|:---|
| ✅ | Test réussi |
| ❌ | Test échoué ou partiel |
| ⚠️ | Optionnel — bloquant mineur, à documenter en Notes |
| G1…G8 | Critère global couvert (voir §4.3) |

---

## 1. Objectif et périmètre

### Ce que ce guide valide

| Domaine | Stories | Comportement attendu |
|---|---|---|
| **Chemin Instrument** | 2.3, 2.9, 2.9b | Notes, CC, pitch bend → Matrix-1000 via **MIDI TO** ; PC et SysEx du clavier **filtrés** |
| **Chemin Editor** | 2.3–2.6, 2.9 | Twist paramètres UI → SysEx 0x06 / 0x03 / 0x0B → Matrix-1000 |
| **Routage header** | 2.9b, 2.11 | Combos **MIDI FROM**, **MIDI TO**, **KEYBOARD FROM** ; persistance session |
| **LEDs d'activité** | 2.8, 2.11 | Impulsions sur trafic **Instrument** (KEYBOARD FROM), **Editor** et **sortie MIDI** (MIDI TO) |
| **Audio** | 2.7 | Pass-through entrée → sortie, **INPUT GAIN** (−120…+12 dB), **crête-mètre** post-gain ; combo **AUDIO FROM** = entrées interface (mono + paires stéréo) |

### Hors périmètre (ne pas bloquer sur ces points)

- Story **2-10** — reorder SysEx Matrix Mod (backlog)
- Device Inquiry, verrouillage UI device type (Epic 8)
- Footer messages, logo popup, shell complet (Story 7.8)
- Hot-plug rafraîchissement automatique des ports MIDI
- Tests Patch Manager, clipboard, mutator, etc. (Epics 4+)

---

## 2. Ce que les tests automatiques couvrent déjà

Les tests unitaires (`Matrix-Control_Tests`) valident la **logique Core** sans hardware. Tu n'as pas besoin de les refaire — utilise-les comme référence de ce que le manuel doit **confirmer en conditions réelles**.

| Fichier de test | Couverture auto | Ce que le manuel doit prouver en plus |
|---|---|---|
| `InstrumentMidiForwarderTests` | Filtre Note/CC/PB ; strip PC/SysEx/aftertouch ; arming | Sons audibles sur le Matrix ; LED Instrument |
| `KeyboardFromMidiInputTests` | Même filtre en standalone | Oxygen 25 → Matrix en standalone |
| `EditorPathTests` | Enqueue SysEx + PC côté editor | Twist UI → changement sonore Matrix |
| `PatchParameterSysExDispatcherTests` | Encodage 0x06 depuis APVTS | DCO1 Frequency audible |
| `MasterParameterSysExDispatcherTests` | Encodage 0x03 complet | Master Tune audible |
| `MatrixModBusParameterSysExDispatcherTests` | Encodage 0x0B par bus | Bus 0 mod audible |
| `MidiManagerTests` | Drain queue + gate SysEx (sans port) | Messages atteignent le fil DIN |
| `MidiPortRoutingPropertyTests` | Persistance propriétés APVTS | Save/reload Live + standalone |
| `MidiActivityTrackerTests` | Décroissance LED 150 ms hold / 350 ms decay | Impulsion visible à l'œil |
| `AudioPassthroughProcessorTests` | Gain, mapping canal mono/stéréo, peak hold 1 s | Audio audible + barre crête |

**Commande de rappel (optionnelle, avant session) :**

```bash
cmake --preset macOS-ARM-Debug -DMATRIX_BUILD_TESTS=ON
cmake --build Builds/macOS/ARM --target Matrix-Control_Tests
./Builds/macOS/ARM/Matrix-Control_Tests_artefacts/Debug/Matrix-Control\ Tests
```

---

## 3. Matériel et câblage

### Schéma de ton setup

```
Oxygen 25 (USB) ──► Mac (hub USB)
                         │
Emagic MT4 ──────────────┤
  DIN Out 1 ────────────► Matrix-1000 MIDI IN
  DIN In 1  ◄──────────── Matrix-1000 MIDI OUT
                         │
Interface audio ◄──────── Matrix-1000 AUDIO OUT (jack(s))
  (entrée ligne)         │
                         └──► Matrix-Control (plugin ou standalone)
```

### Prérequis Matrix-1000

1. **Alimentation et démarrage** — attendre fin du boot (~8 s).
2. **Canal MIDI** — dans l'UI Matrix-Control : **MASTER EDIT → MIDI → CHANNEL** = canal utilisé par ton clavier (souvent **CHANNEL 1**). Le Matrix doit écouter le même canal que celui envoyé par Live ou l'Oxygen.
3. **MIDI ECHO** — pour ce UAT, laisse **OFF** dans **MASTER EDIT → MIDI → MIDI ECHO** (évite boucles si tu branches un moniteur MIDI plus tard).
4. **MEMORY PROTECT** — **OFF** si tu veux entendre les changements de paramètres en temps réel sans restriction.
5. **Volume Matrix** — niveau audible mais pas saturé (tu testeras aussi le gain plugin).

### Prérequis Mac

- Build **Debug** ou **Release** récente avec stories Epic 2 `done` (voir `sprint-status.yaml`).
- Plugins installés (si `USER_COPY_TO_SYSTEM_FOLDERS ON` dans `project-configuration.cmake`) :
  - VST3 : `~/Library/Audio/Plug-Ins/VST3/Matrix-Control.vst3`
  - AU : `~/Library/Audio/Plug-Ins/Components/Matrix-Control.component`
- Standalone : `Builds/macOS/ARM/Matrix-Control_artefacts/Debug/Standalone/Matrix-Control.app`
- **Ableton Live 12 Suite** — rescan plugins si tu viens de builder.
- **Audio MIDI Setup** (macOS) : vérifier que l'Emagic MT4 expose bien **Port 1** (noms exacts affichés dans les combos Matrix-Control).

### Identification des ports et LEDs dans Matrix-Control

Dans le header (gauche → droite), chaque **paquet logique** = label + combo (+ LED(s) le cas échéant) :

| Paquet | Contrôles | Rôle |
|---|---|---|
| **MIDI FROM** | Label + Combo | Réception depuis **Matrix OUT → MT4 In 1** (retour SysEx / MIDI entrant). *Pas de LED en v1 — trafic inbound non instrumenté.* |
| **MIDI TO** | Label + Combo + **2 LEDs** | Envoi vers **MT4 Out 1 → Matrix IN**. LED **Editor** = SysEx UI en file d'attente ; LED **sortie** = message MIDI effectivement envoyé. |
| **KEYBOARD FROM** | Label + Combo + **1 LED** | Plugin : **HOST** (grisé). Standalone : port clavier maître. LED **Instrument** = notes / CC / pitch bend. |
| **AUDIO FROM** | Label + Combo | Entrées de l'interface audio (voir §4.5). |
| **INPUT GAIN** | Label + Slider | −120…+12 dB (défaut 0 dB). |
| **Crête-mètre** | Barre verticale | Niveau crête post-gain (hold ~1 s, decay ~1 s). |

**Twist paramètre UI** → les **deux LEDs à droite de MIDI TO** pulsent (Editor immédiat, sortie si **MIDI TO** ouvert). La LED **Instrument** pulse à la frappe clavier.

### Format combo AUDIO FROM

Plugin **et** standalone affichent les entrées de l'interface sous la forme :

```
Scarlett 6i6 USB (1)
Scarlett 6i6 USB (2)
Scarlett 6i6 USB (3)
Scarlett 6i6 USB (4)
Scarlett 6i6 USB (1/2)
Scarlett 6i6 USB (3/4)
```

| Choix | Usage recommandé |
|---|---|
| `(1)` … `(4)` — mono | **Matrix-1000** (sortie mono) |
| `(1/2)` … `(3/4)` — stéréo | **Matrix-6 / 6R** (futur) |

Les libellés exacts viennent du driver / CoreAudio (Live peut afficher le même nom d'interface).

---

## 4. Préparation commune (toutes sessions)

### 4.1 Checklist avant d'ouvrir l'app

| ID | Test | Résultat | Notes |
|:---|:---|:---:|:---|
| PRE-1 | Matrix-1000 allumé, câbles MIDI DIN vérifiés (Out→In) | | |
| PRE-2 | Sortie audio Matrix → entrée **ligne** interface | | |
| PRE-3 | Interface audio sélectionnée (standalone ou Live) | | |
| PRE-4 | Oxygen 25 reconnu (LED MIDI si note test) | | |
| PRE-5 | Patch Matrix audible — **C3** sonne | | |

### 4.2 Réglages header communs

1. **MIDI TO** → port MT4 relié au **Matrix IN** (ex. `MT4 Port 1` ou libellé équivalent **Out**).
2. **MIDI FROM** → port MT4 relié au **Matrix OUT** (ex. même paire **In/Out 1**).
3. **INPUT GAIN** → `0` dB (milieu du slider).
4. **AUDIO FROM** → voir §4.5 (plugin vs standalone).

### 4.5 AUDIO FROM — plugin vs standalone

Deux niveaux de routage à distinguer en **plugin (Live)** :

| Niveau | Où | Rôle |
|---|---|---|
| **Hôte (Live)** | *Audio From* de la **piste** (ou device chain) | Route le **hardware** vers le bus **Audio From** du plugin. **Indispensable** pour que le signal Matrix arrive dans le plugin. |
| **Plugin (header)** | Combo **AUDIO FROM** Matrix-Control | Choisit l'**entrée logique** (mono `(n)` ou stéréo `(n/m)`) et le **mapping canal** sur le bus reçu. |

En **standalone**, la combo **AUDIO FROM** configure **directement** les canaux d'entrée via *Options → Audio/MIDI Settings* (pas de couche hôte).

**Recommandation Matrix-1000 (mono)** : sélectionner l'entrée mono correspondant au jack utilisé, ex. `Scarlett 6i6 USB (1)`.

**Checklist combo AUDIO FROM** (avant tests audio) :

| ID | Test | Résultat | Notes |
|:---|:---|:---:|:---|
| PRE-A1 | Combo **AUDIO FROM** peuplée (pas vide, pas seulement `—`) | | Standalone : attendre ~2 s ou rouvrir app si vide au démarrage |
| PRE-A2 | Entrées mono `(1)`…`(n)` **et** paires stéréo `(1/2)`… visibles | | Noms = interface active (ex. Scarlett) |
| PRE-A3 | Sélection mono `(1)` → pas de crash ; persistance après reload | | |

### 4.3 Critères de succès globaux

Valide chaque critère **au moins une fois** durant la session (colonne **Résultat** en fin de UAT).

| ID | Test | Résultat | Notes |
|:---|:---|:---:|:---|
| G1 | Une note du clavier produit du son Matrix | | |
| G2 | Twist paramètre PATCH change le timbre en < 500 ms | | |
| G3 | Twist paramètre MASTER change le comportement global | | |
| G4 | Matrix Mod bus 0 change le son | | |
| G5 | LED Instrument (frappe, KEYBOARD FROM) ; **2 LEDs MIDI TO** (twist UI : Editor + sortie) | | |
| G6 | Audio pass-through audible ; crête-mètre réagit | | |
| G7 | INPUT GAIN modifie volume perçu ; crête suit le gain | | |
| G8 | Fermer/rouvrir session restaure ports + gain | | |

### 4.4 Outils optionnels (non obligatoires)

- **MIDI Monitor** (macOS) sur **MIDI FROM** : voir SysEx `F0 41 10 06 … F7` lors des twists.
- **Enregistrement Live** : vérifier que le pass-through sort sur la piste.

---

## 5. Session A — Ableton Live 12 (VST3)

> Objectif : valider le chemin **HOST → Instrument** et le modèle **External Instrument** pour l'audio.

### 5.1 Création de la piste

1. Nouveau Set Live 12 — sample rate identique à ton interface (44.1 ou 48 kHz).
2. Crée une piste **MIDI**.
3. Insère **Matrix-Control** format **VST3**.
4. Vérifie header : **KEYBOARD FROM** affiche **HOST** et est **grisé** (normal).

### 5.2 Routage MIDI dans Live

1. **MIDI From** de la piste : **Oxygen 25** (ou **All Ins** si une seule source).
2. **Monitor** : **In** (ou **Auto** avec piste armée).
3. **Arm** la piste (bouton rouge).
4. Dans Matrix-Control : règle **MIDI TO** / **MIDI FROM** comme en §4.2.

### 5.3 Tests Instrument (chemin performance)

| ID | Test | Résultat | Notes |
|:---|:---|:---:|:---|
| A-VST3-I1 | Joue C3, piste armée → son Matrix ; LED Instrument pulse (G1, G5) | | |
| A-VST3-I2 | Piste désarmée, rejoue C3 → pas de notes Matrix ; LED inactive | | |
| A-VST3-I3 | Réarmé, CC1 (mod wheel) → réaction Matrix ; LED Instrument (G1) | | |
| A-VST3-I4 | Program Change depuis Live → pas de changement patch (PC filtré) | | |

### 5.4 Tests Editor (chemin édition SysEx)

Passe en mode **PATCH** dans l'UI (si sélecteur présent) ou section **PATCH EDIT**.

| ID | Test | Résultat | Notes |
|:---|:---|:---:|:---|
| A-VST3-E1 | PATCH EDIT → DCO 1 → FREQUENCY +2 → timbre change ; **2 LEDs à droite de MIDI TO** pulsent (G2, G5) | | |
| A-VST3-E2 | MASTER EDIT → MISC → MASTER TUNE → hauteur globale (G3, G5) | | |
| A-VST3-E3 | MATRIX MOD bus 0 : LFO 1 → VCF FREQ → modulation audible (G4, G5) | | |
| A-VST3-E4 | Pendant E1, jeu + édition simultanés sans coupure MIDI | | |

### 5.5 Tests Audio (pass-through + gain + crête)

Matrix-Control est un **instrument avec bus d'entrée audio** « Audio From ».

#### Routage audio en Live (VST3)

1. Route la **sortie audio Matrix** → entrée **ligne** de ton interface (ex. Scarlett Input 1).
2. Dans **Live → Preferences → Audio**, active les entrées mono/stéréo utilisées (ex. mono 1–4, stéréo 1/2 et 3/4).
3. Sur la **piste MIDI** Matrix-Control :
   - Ouvre le panneau **Input/Output** (ou *External Instrument* selon ton flux).
   - **Audio From** Live → **même entrée physique** que le jack Matrix (ex. `Scarlett 6i6 USB` → **Ext. In 1** ou **1**).
   - Vérifie que le bus **Audio From** du plugin reçoit du signal (monitoring piste ou crête Matrix-Control).
4. Dans Matrix-Control header : **AUDIO FROM** → `Scarlett 6i6 USB (1)` (ou l'entrée mono alignée avec le routage Live).
5. Joue une note tenue sur le Matrix.

> **Double routage** : Live choisit *quel* jack hardware alimente le plugin ; la combo Matrix-Control choisit *comment* mapper ce signal (mono vs stéréo). Les deux doivent correspondre à la **même** entrée physique.

| ID | Test | Résultat | Notes |
|:---|:---|:---:|:---|
| A-VST3-A0 | Combo AUDIO FROM liste l'interface (ex. Scarlett) avec mono `(1)`… et stéréo `(1/2)` (PRE-A1, PRE-A2) | | |
| A-VST3-A1 | Signal Matrix sur entrée plugin → pass-through ; crête monte (G6) | | |
| A-VST3-A2 | INPUT GAIN +6 dB → volume + fort ; crête haute (G7) | | |
| A-VST3-A3 | INPUT GAIN −24 dB → volume faible ; crête basse (G7) | | |
| A-VST3-A4 | Silence → crête retombe après ~1–2 s (G6) | | |
| A-VST3-A5 | AUDIO FROM `(1)` puis `(2)` (ou autre mono) → signal cohérent si jack correspondant routé dans Live | | |
| A-VST3-A6 | AUDIO FROM `(1/2)` → pass-through stéréo si Live route la paire 1/2 | | Optionnel Matrix-1000 |

### 5.6 Persistance Live (VST3)

| ID | Test | Résultat | Notes |
|:---|:---|:---:|:---|
| A-VST3-P1 | Sauvegarde le Set `.als` | | |
| A-VST3-P2 | Ferme/rouvre Live → MIDI TO/FROM, gain, AUDIO FROM OK ; sons OK (G8) | | |

---

## 6. Session B — Ableton Live 12 (AU)

> Répète la **Session A** à l'identique avec **Matrix-Control AU** pour détecter des écarts hôte/format.

### Checklist AU (reprise Session A)

Même protocole qu'en §5, plugin **AU**. Note les écarts VST3 ↔ AU dans **Notes**.

| ID | Test | Résultat | Notes |
|:---|:---|:---:|:---|
| B-AU-I1 | Joue C3, piste armée → son Matrix ; LED Instrument (G1, G5) | | |
| B-AU-I2 | Piste désarmée → pas de notes Matrix | | |
| B-AU-I3 | CC1 (mod wheel) → réaction Matrix ; LED Instrument | | |
| B-AU-E1 | DCO 1 FREQUENCY twist → timbre change ; **2 LEDs MIDI TO** (G2, G5) | | |
| B-AU-E2 | MASTER TUNE twist → hauteur globale (G3, G5) | | |
| B-AU-E3 | Matrix Mod bus 0 → modulation audible (G4, G5) | | |
| B-AU-E4 | Jeu + édition simultanés | | |
| B-AU-A1 | Pass-through + crête-mètre (G6) | | |
| B-AU-A0 | Combo AUDIO FROM peuplée (interface + mono/stéréo) | | |
| B-AU-A2 | INPUT GAIN +6 dB (G7) | | |
| B-AU-A3 | INPUT GAIN −24 dB (G7) | | |
| B-AU-P1 | Save/reload Set → ports + gain restaurés (G8) | | |
| B-AU-D1 | Ports MIDI identiques VST3 vs AU | | |
| B-AU-D2 | Bus « Audio From » visible et routable (AU Music Device) | | |

---

## 7. Session C — Standalone macOS

> Objectif : valider **KEYBOARD FROM** dédié (port Oxygen USB) et **AUDIO FROM** physique.

### 7.1 Lancement et audio device

1. Ouvre `Matrix-Control.app`.
2. Menu **Options → Audio/MIDI Settings** (fenêtre JUCE standalone) :
   - **Audio output** : interface principale (casque/moniteurs).
   - **Audio input** : interface où arrive le signal Matrix (ex. Scarlett 6i6 USB).
   - Sample rate stable (évite pops si tu changes en cours de test).
3. Header : **KEYBOARD FROM** est **actif** (pas HOST).
4. Header : **AUDIO FROM** doit se remplir en quelques secondes (retry auto au démarrage). Si combo vide (`—`) : rouvre *Audio/MIDI Settings*, confirme l'interface input, ferme — la combo se rafraîchit.
5. Sélectionne **AUDIO FROM** → entrée mono du jack Matrix (ex. `Scarlett 6i6 USB (1)`).

### 7.2 Routage MIDI standalone

| ID | Test | Résultat | Notes |
|:---|:---|:---:|:---|
| C-M1 | MIDI TO / FROM : mêmes ports MT4 qu'en Live → combos peuplés | | |
| C-M2 | KEYBOARD FROM → Oxygen 25 (USB) | | |
| C-M3 | Joue C3 → son Matrix ; LED Instrument (G1, G5) | | |
| C-M4 | KEYBOARD FROM → `—` → plus de notes Matrix | | |
| C-M5 | Re-sélectionne Oxygen → jeu OK | | |

### 7.3 Tests Editor (identiques à Live)

| ID | Test | Résultat | Notes |
|:---|:---|:---:|:---|
| C-E1 | DCO 1 FREQUENCY twist → timbre change ; **2 LEDs MIDI TO** (G2, G5) | | |
| C-E2 | MASTER TUNE twist (G3, G5) | | |
| C-E3 | Matrix Mod bus 0 (G4, G5) | | |
| C-E4 | Jeu + édition simultanés | | |

### 7.4 Tests Audio standalone

| ID | Test | Résultat | Notes |
|:---|:---|:---:|:---|
| C-A0 | Combo AUDIO FROM peuplée : mono `(1)`… et stéréo `(1/2)`… (PRE-A1, PRE-A2) | | |
| C-A1 | AUDIO FROM → entrée Matrix (mono) → pass-through audible (G6) | | ex. `(1)` |
| C-A2 | AUDIO FROM `(1/2)` (stéréo) → pas de crash ; signal si Matrix câblé stéréo | | Optionnel M-1000 mono |
| C-A3 | Change entrée dans *Audio/MIDI Settings* → combo se met à jour | | |
| C-A4 | INPUT GAIN +6 dB puis −24 dB (G7) | | |
| C-A5 | Note tenue puis relâche → crête décroît (G6) | | |

### 7.5 Persistance standalone

| ID | Test | Résultat | Notes |
|:---|:---|:---:|:---|
| C-P1 | Ports + gain + audio source réglés, quitte l'app | | |
| C-P2 | Relance → combos restaurés ; sons OK (G8) | | |

---

## 8. Tests de non-régression croisés (recommandés)

| ID | Test | Résultat | Notes |
|:---|:---|:---:|:---|
| NR-1 | Éditer PATCH pendant jeu 5 min → pas de freeze ; latence stable | | |
| NR-2 | MIDI FROM sur `—`, twist param → son via MIDI TO ; pas de crash | | |
| NR-3 | MIDI TO sur `—`, twist param → pas de changement sonore ; pas de crash | | |
| NR-4 | INPUT GAIN −120 dB → signal quasi inaudible ; crête ~0 | | |
| NR-5 | INPUT GAIN +12 dB → signal fort ; crête saturée (clipping ?) | | |

---

## 9. Synthèse

Compte les **✅** et **❌** dans chaque section, puis remplis les verdicts.

| Zone | Tests | ✅ | ❌ |
|:---|:---:|:---:|:---:|
| Préparation §4.1 | 5 | | |
| Préparation audio §4.5 | 3 | | |
| Critères G1–G8 §4.3 | 8 | | |
| VST3 §5.3–5.6 | 16 | | |
| AU §6 | 14 | | |
| Standalone §7 | 17 | | |
| Non-régression §8 | 5 | | |

**Critère de passage Epic 2** : **G1–G8** tous ✅ ; aucun ❌ bloquant sur Editor + Instrument + audio en **VST3** et **Standalone**. Écarts AU documentés en §11.

### Verdict session

| ID | Test | Résultat | Notes |
|:---|:---|:---:|:---|
| VERDICT-VST3 | Session A complète, critère atteint | | |
| VERDICT-AU | Session B complète, pas de régression vs VST3 | | |
| VERDICT-STANDALONE | Session C complète, critère atteint | | |
| VERDICT-GLOBAL | Prêt pour story 2-10 | | |

---

## 10. Dépannage rapide

| Symptôme | Pistes |
|---|---|
| Pas de son clavier | Piste armée (Live) ? **MIDI TO** correct ? Canal Matrix = canal Oxygen ? **KEYBOARD FROM** = Oxygen en standalone ? |
| Twist UI sans effet | **MIDI TO** vers Matrix IN ? Matrix allumé ? **MEMORY PROTECT** OFF ? |
| SysEx suspect mais pas de son | Normal si seul MIDI FROM manque — l'envoi utilise **MIDI TO** ; FROM sert surtout au retour |
| LED Instrument morte, son OK | Cosmétique — ❌ partiel sur G5 si son OK |
| LED Editor pulse mais pas LED sortie (MIDI TO) | **MIDI TO** sur `—` ou port fermé — Editor pulse quand même ; sortie seulement après envoi réussi |
| LEDs au mauvais endroit | Layout corrigé en 2.11 : **aucune LED** sur MIDI FROM ; **2 LEDs** à droite de **MIDI TO** ; **1 LED** à droite de **KEYBOARD FROM** |
| Pas d'audio pass-through (plugin) | **Live Audio From** piste → bon jack ? Bus **Audio From** plugin actif ? Combo Matrix-Control = même entrée mono `(n)` ? **INPUT GAIN** > −120 dB ? |
| Pas d'audio pass-through (standalone) | **AUDIO FROM** sur `—` ? *Options → Audio/MIDI Settings* : input device + canaux activés ? Attendre refresh combo (~2 s) |
| Combo AUDIO FROM vide | Standalone : device pas prêt — attendre ou rouvrir *Audio/MIDI Settings*. Plugin : rescan CoreAudio — redémarrer app / Live |
| Crête plate | **INPUT GAIN** trop bas ? Signal Matrix trop faible ? Joue note plus forte |
| Ports vides dans combos MIDI | Rebrancher MT4 ; **Audio MIDI Setup** ; redémarrage app |
| Boucle MIDI / notes fantômes | **MIDI ECHO** OFF sur Matrix ; ne pas router MT4 Out vers Out en boucle |
| Oxygen + MIDI FROM même device | Edge case connu (déféré) — évite de sélectionner le même port pour FROM et KEYBOARD FROM |

---

## 11. Après ce UAT

Si **PASS** :

- Tu peux attaquer **2-10** (Matrix Mod bus reorder SysEx) avec confiance sur la stack MIDI/audio.
- Enregistre date + commit testé dans ce fichier (section ci-dessous).

Si **FAIL** :

- Note commit hash, format (VST3/AU/Standalone), étape exacte.
- Ouvre un correctif ciblé ou une story de bug avant 2-10.

### Journal de session

| Champ | Valeur |
|:---|:---|
| Date | |
| Commit git | |
| Build preset | ex. `macOS-ARM-Debug` |
| Live version | 12.x |
| Interface audio | ex. Focusrite Scarlett 6i6 USB |
| Entrée audio Matrix (jack → combo) | ex. `(1)` |
| Noms ports MT4 (FROM / TO) | |
| Tests ❌ (IDs) | ex. `A-VST3-A1, C-M3` |
| Commentaires / écarts AU | |
| Journal complété | ✅ ou ❌ |

---

## Références projet

- Checklist SM-1 originale : `2-9b-header-routing-controls-uat-slice.md` (Completion Notes)
- Header layout + LEDs : `2-11-header-panel-layout-and-widget-styling.md`
- Audio : `2-7-audio-passthrough-and-peak-indicator.md`
- LEDs : `2-8-activity-leds-on-queue-traffic.md`
- Chemins MIDI : `2-3-instrument-path-and-editor-path-producers.md`
- SysEx Matrix-1000 : `_bmad-output/reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md`
