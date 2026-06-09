---
organization: Ten Square Software
project: Matrix-Control
title: UAT manuel Epic 2 — Matrix-1000
author: Guillaume DUPONT
status: draft
created: 2026-06-05
updated: 2026-06-06
scope: Epic 2 (stories 2-3 à 2-11), Epic R (R-1 à R-3), stories 2-10 et 7-9, story 7-7
target_hardware: Oberheim Matrix-1000
target_host: Ableton Live 12 Suite (VST3 + AU) + Standalone macOS
audience: testeur musicien (non développeur)
---

# Tests manuels Epic 2 — Matrix-1000

Ce document sert à vérifier, **sur ton vrai Matrix-1000**, que Matrix-Control envoie bien le MIDI et que l’interface réagit comme prévu. Les tests automatisés du projet couvrent déjà la logique interne ; ici, on valide le comportement **audible et visible** avec ton câblage habituel.

**Stories / epics couverts** (passation dev) : Epic 2 · Epic R · 2-10 · 7-9 · 7-7.

---

## Comment noter tes résultats

Dans chaque tableau, remplis la colonne **Résultat** avec **✅** (succès) ou **❌** (échec). Utilise **Notes** seulement si tu dois préciser un détail.

---

## 1. Matériel et réglages

### Câblage attendu

```
Clavier USB (ex. Oxygen 25) ──► Mac
Emagic MT4 :
  Sortie DIN 1 ──────────────► Matrix-1000 MIDI IN
  Entrée DIN 1  ◄──────────── Matrix-1000 MIDI OUT
Sortie audio Matrix ───────► Entrée ligne de ton interface audio
```

### Sur le Matrix-1000

| Réglage | Valeur recommandée | Pourquoi |
|---|---|---|
| Démarrage | Attendre ~8 s après mise sous tension | Le synthé doit être prêt |
| **MASTER EDIT → MIDI → CHANNEL** | Même canal que ton clavier (souvent **1**) | Sinon le Matrix n’écoute pas tes notes |
| **MASTER EDIT → MIDI → MIDI ECHO** | **OFF** | Évite les boucles MIDI |
| **MEMORY PROTECT** | **OFF** | Sinon les changements depuis l’interface peuvent être ignorés |
| Volume Matrix | Audible, sans saturation | Tu dois entendre clairement les variations de timbre |

### Version de Matrix-Control à tester

Demande au développeur la **build du jour** (plugin VST3, plugin AU et application standalone). Après installation, **rescanne les plugins** dans Live si besoin.

---

## 2. Comprendre l’interface

### Bandeau du haut (de gauche à droite)

Chaque zone MIDI comporte un **petit carré vert** (voyant d’activité), puis un libellé, puis un menu déroulant.

| Zone | Rôle simple | Voyant s’allume quand… |
|---|---|---|
| **KEYBOARD FROM** | D’où viennent tes notes de clavier | Tu joues au clavier (notes, molette de modulation, pitch bend) |
| **EDITOR MIDI FROM** | Où Matrix-Control **écoute** le Matrix | Le Matrix **renvoie** du MIDI vers le Mac (retour synthé) |
| **MIDI TO** | Où Matrix-Control **envoie** vers le Matrix | L’app envoie un message au Matrix (édition d’un paramètre **ou** notes du clavier) |
| **SETTINGS** (bouton à droite) | Préférences | — |

En **plugin dans Live** : **KEYBOARD FROM** affiche **HOST** et est grisé — c’est normal, Live fournit le clavier.

En **application standalone** : tu choisis ton clavier USB dans **KEYBOARD FROM**.

### Fenêtre SETTINGS

Ouvre-la avec le bouton **SETTINGS**.

| Réglage | Plugin (Live) | Standalone |
|---|---|---|
| Skin, taille de l’interface | Oui | Oui |
| **HARDWARE LATENCY** (délai matériel) | Oui — pour compenser le temps que met le son du Matrix à revenir dans le DAW | Non affiché |
| **AUDIO FROM**, **INPUT GAIN**, barre de niveau | Non affiché — le son du Matrix passe par le routage audio de Live | Oui — écoute directe du Matrix dans l’app |

> **Important (Epic R, story R-1)** : en plugin, Matrix-Control **ne transporte plus l’audio** du Matrix. Tu routes l’audio du synthé comme d’habitude dans Live (piste audio ou monitoring d’entrée). Les tests audio de ce guide concernent **uniquement le standalone**.

### Réglages MIDI à faire avant chaque session

1. **MIDI TO** → port MT4 relié à l’**entrée MIDI** du Matrix (souvent « Port 1 » ou libellé *Out* côté Mac).
2. **EDITOR MIDI FROM** → port MT4 relié à la **sortie MIDI** du Matrix (souvent la paire *In* du même port).
3. Dans Matrix-Control : **MASTER EDIT → MIDI → CHANNEL** = canal de ton clavier.

---

## 3. Préparation — à faire une fois

| ID | Ce que tu fais | Succès si | Résultat | Notes |
|:---|:---|:---|:---:|:---|
| PRE-1 | Vérifie câbles MIDI et audio, Matrix allumé | Tout est branché, Matrix prêt | | |
| PRE-2 | Joue **Do central (C3)** avec la config MIDI ci-dessus | Tu entends le Matrix | | |
| PRE-3 | Ouvre **SETTINGS** → vérifie que la fenêtre s’affiche et se ferme | Pas de plantage | | |

---

## 4. Session A — Ableton Live 12, plugin VST3

### Mise en place

1. Nouveau Set Live — même fréquence d’échantillonnage que ton interface (44,1 ou 48 kHz).
2. Crée une piste **MIDI**, insère **Matrix-Control** en **VST3**.
3. Sur la piste : **MIDI From** = ton clavier ; **Monitor** = **In** ; **arme** la piste (bouton rouge).
4. Règle **MIDI TO** et **EDITOR MIDI FROM** comme en §2.

### A1 — Jouer au clavier

| ID | Ce que tu fais | Succès si | Résultat | Notes |
|:---|:---|:---|:---:|:---|
| A-VST3-1 | Piste armée : joue **C3** | Son Matrix audible ; voyant près de **KEYBOARD FROM** et voyant près de **MIDI TO** clignotent | | |
| A-VST3-2 | Désarme la piste, rejoue **C3** | Pas de nouvelle note sur le Matrix ; voyant clavier éteint | | |
| A-VST3-3 | Réarme : bouge la **molette de modulation** (CC1) | Le Matrix réagit (ex. vibrato/tremolo selon le patch) | | |

### A2 — Éditer le patch depuis l’interface

Ouvre la section **PATCH EDIT**.

| ID | Ce que tu fais | Succès si | Résultat | Notes |
|:---|:---|:---|:---:|:---|
| A-VST3-4 | **DCO 1 → FREQUENCY** : monte d’environ 2 crans | Le timbre change en moins d’une seconde ; voyant près de **MIDI TO** clignote | | |
| A-VST3-5 | Pendant que tu joues, continue à modifier des paramètres | Jeu et édition simultanés sans coupure ni blocage | | |

### A3 — Éditer le Master

| ID | Ce que tu fais | Succès si | Résultat | Notes |
|:---|:---|:---|:---:|:---|
| A-VST3-6 | **MASTER EDIT → MISC → MASTER TUNE** : modifie la valeur | La hauteur globale du Matrix change | | |

### A4 — Matrix Mod

| ID | Ce que tu fais | Succès si | Résultat | Notes |
|:---|:---|:---|:---:|:---|
| A-VST3-7 | **MATRIX MOD**, bus **0** : mets par ex. **LFO 1 → VCF FREQ** avec un amount audible | La modulation est audible sur le son | | |

### A5 — Réordonner les bus Matrix Mod (stories 2-10, 7-9)

Prépare deux bus avec des modulations **clairement différentes** (ex. bus 0 et bus 1).

| ID | Ce que tu fais | Succès si | Résultat | Notes |
|:---|:---|:---|:---:|:---|
| A-VST3-8 | Clique sur le **numéro** d’un bus, maintiens, glisse sur un autre bus, relâche | Les contenus des deux bus **échangent** (source, amount, destination) ; le son du Matrix reflète le changement ; voyant **MIDI TO** clignote | | |
| A-VST3-9 | Les numéros de bus **0 à 9** à gauche n’ont pas bougé | Seul le contenu a été permuté, pas les numéros affichés | | |

### A6 — SETTINGS et sauvegarde du Set

| ID | Ce que tu fais | Succès si | Résultat | Notes |
|:---|:---|:---|:---:|:---|
| A-VST3-10 | **SETTINGS** : règle **HARDWARE LATENCY** (ex. 10 ms), ferme SETTINGS | La valeur reste affichée si tu rouvres SETTINGS | | Story R-2 |
| A-VST3-11 | Sauvegarde le Set, ferme Live, rouvre le Set | **MIDI TO**, **EDITOR MIDI FROM** et **HARDWARE LATENCY** sont retrouvés ; le jeu et l’édition fonctionnent encore | | |

---

## 5. Session B — Ableton Live 12, plugin AU

Reprends la **Session A** avec le plugin **AU**. Si un test échoue en AU mais passait en VST3, note-le.

| ID | Ce que tu fais | Succès si | Résultat | Notes |
|:---|:---|:---|:---:|:---|
| B-AU-1 | Tests A-VST3-1 à A-VST3-9 en AU | Même comportement qu’en VST3 | | |
| B-AU-2 | Test A-VST3-11 en AU | Ports MIDI et latency restaurés après rechargement | | |
| B-AU-3 | Compare les noms de ports MIDI VST3 vs AU | Mêmes ports disponibles (libellés équivalents) | | |

---

## 6. Session C — Application standalone (macOS)

### Mise en place

1. Lance **Matrix-Control.app**.
2. Menu **Options → Audio/MIDI Settings** :
   - **Sortie audio** : tes enceintes ou casque.
   - **Entrée audio** : l’interface où arrive le signal du Matrix.
3. Bandeau : **KEYBOARD FROM** = ton clavier USB ; **MIDI TO** / **EDITOR MIDI FROM** = mêmes ports MT4 qu’en Live.
4. **SETTINGS** → **AUDIO FROM** : choisis l’entrée où arrive le Matrix (ex. `Scarlett (1)` pour une entrée mono).

### C1 — MIDI

| ID | Ce que tu fais | Succès si | Résultat | Notes |
|:---|:---|:---|:---:|:---|
| C-1 | Joue **C3** | Son Matrix ; voyants **KEYBOARD FROM** et **MIDI TO** clignotent | | |
| C-2 | Mets **KEYBOARD FROM** sur « — » (aucun) | Plus de notes vers le Matrix | | |
| C-3 | Resélectionne le clavier | Le jeu refonctionne | | |
| C-4 | Modifie **DCO 1 → FREQUENCY** | Timbre change ; voyant **MIDI TO** clignote | | |
| C-5 | Échange deux bus Matrix Mod par glisser-déposer sur le numéro | Même résultat qu’en A-VST3-8/9 | | |

### C2 — Audio (standalone uniquement, stories 2-7, 7-7)

| ID | Ce que tu fais | Succès si | Résultat | Notes |
|:---|:---|:---|:---:|:---|
| C-6 | Joue une note tenue : écoute la sortie de l’app | Tu entends le Matrix ; la barre de niveau dans SETTINGS monte | | |
| C-7 | **INPUT GAIN** à +6 dB | Volume plus fort ; barre plus haute | | |
| C-8 | **INPUT GAIN** à −24 dB | Volume plus faible ; barre plus basse | | |
| C-9 | Silence après une note | La barre redescend en 1 à 2 secondes | | |

### C3 — Persistance standalone

| ID | Ce que tu fais | Succès si | Résultat | Notes |
|:---|:---|:---|:---:|:---|
| C-10 | Quitte l’app après avoir réglé ports, gain et **AUDIO FROM** | — | | |
| C-11 | Relance l’app | Tous les réglages sont retrouvés ; jeu et édition OK | | |

---

## 7. Tests de confort (recommandés, non bloquants)

| ID | Ce que tu fais | Succès si | Résultat | Notes |
|:---|:---|:---|:---:|:---|
| NR-1 | Édite le patch pendant 5 min en jouant | Pas de freeze, pas de latence qui dérive | | |
| NR-2 | **EDITOR MIDI FROM** sur « — », modifie un paramètre | Le son change quand même (l’envoi utilise **MIDI TO**) | | |
| NR-3 | **MIDI TO** sur « — », modifie un paramètre | Pas de changement sonore, mais l’app ne plante pas | | |

---

## 8. Critères de passage Epic 2

Coche mentalement chaque ligne avant de valider l’epic.

| ID | Critère | Couvert par |
|:---|:---|:---|
| G1 | Le clavier fait sonner le Matrix | A-VST3-1, C-1 |
| G2 | Modifier un paramètre de patch change le timbre rapidement | A-VST3-4 |
| G3 | Modifier un paramètre Master change le comportement global | A-VST3-6 |
| G4 | Matrix Mod bus 0 modifie le son | A-VST3-7 |
| G5 | Les voyants clignotent aux bons moments (jeu → clavier + MIDI TO ; édition → MIDI TO) | A-VST3-1, A-VST3-4 |
| G6 | Audio audible et barre de niveau réactive (**standalone seulement**) | C-6, C-9 |
| G7 | Le gain d’entrée modifie le volume perçu (**standalone seulement**) | C-7, C-8 |
| G8 | Ports MIDI et réglages SETTINGS survivent à une fermeture / réouverture | A-VST3-11, C-11 |
| G9 | Glisser-déposer d’un bus Matrix Mod met à jour le son du Matrix | A-VST3-8, C-5 |

**Verdict Epic 2** : **G1 à G9** tous ✅, et aucun ❌ bloquant en **VST3** et **standalone**. Les écarts AU sont documentés dans le journal ci-dessous.

| Verdict | Résultat | Notes |
|:---|:---:|:---|
| Session VST3 (§4) | | |
| Session AU (§5) | | |
| Session standalone (§6) | | |
| **Epic 2 validé** | | |

---

## 9. Si ça ne marche pas

| Problème | Vérifie en premier |
|---|---|
| Pas de son au clavier | Piste armée dans Live ? **MIDI TO** correct ? Canal Matrix = canal clavier ? En standalone : **KEYBOARD FROM** = bon clavier ? |
| L’interface ne change pas le son | **MIDI TO** vers l’entrée MIDI du Matrix ? **MEMORY PROTECT** OFF ? |
| Voyant **MIDI TO** éteint mais le son change | Cosmétique — note en **❌** sur G5 seulement si le son est OK |
| Voyant **MIDI TO** allumé, pas de changement sonore | **MIDI TO** sur « — » ou mauvais port |
| Pas d’audio en standalone | **SETTINGS → AUDIO FROM** sur « — » ? Entrée audio choisie dans *Options → Audio/MIDI Settings* ? |
| Pas d’audio en plugin Live | Normal : route l’audio du Matrix dans Live (entrée de piste audio), pas dans Matrix-Control |
| Menus MIDI vides | Rebrancher le MT4 ; redémarrer l’app ; vérifier **Audio MIDI Setup** (macOS) |
| Notes fantômes ou boucles | **MIDI ECHO** OFF sur le Matrix |
| Glisser-déposer bus sans effet | Clique bien sur le **numéro** du bus (pas sur un menu déroulant), glisse assez loin |

---

## 10. Journal de session (pour le développeur)

| Champ | Valeur |
|:---|:---|
| Date | |
| Version / build testée | |
| Live (version) | |
| Interface audio | |
| Entrée audio Matrix (standalone) | ex. entrée 1 |
| Ports MT4 (**EDITOR MIDI FROM** / **MIDI TO**) | |
| Tests ❌ (IDs) | ex. `A-VST3-4, C-6` |
| Écarts AU vs VST3 | |
| Epic 2 validé (G1–G9) | ✅ ou ❌ |

---

## Annexe — Références développeur

| Sujet | Story / epic | Fichier story |
|---|---|---|
| Chemins MIDI jeu + édition | Epic 2 · 2-3, 2-9 | `2-3-instrument-path-and-editor-path-producers.md` |
| Routage bandeau | 2-9b, 2-11 | `2-9b-header-routing-controls-uat-slice.md` |
| Voyants d’activité | 2-8, 2-11 | `2-8-activity-leds-on-queue-traffic.md` |
| Audio standalone | 2-7, 7-7 | `2-7-audio-passthrough-and-peak-indicator.md` |
| Pas d’audio en plugin | Epic R · R-1 | `sprint-change-proposal-2026-06-06.md` |
| Latence matérielle plugin | R-2 | story R-2 dans `sprint-status.yaml` |
| Reorder Matrix Mod (MIDI) | 2-10 | `2-10-matrix-mod-bus-reorder-sysex.md` |
| Reorder Matrix Mod (interface) | 7-9 | story 7-9 dans `sprint-status.yaml` |
| Fenêtre SETTINGS | 7-7 | story 7-7 dans `sprint-status.yaml` |
| Spécification SysEx Matrix-1000 | — | `_bmad-output/reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md` |
