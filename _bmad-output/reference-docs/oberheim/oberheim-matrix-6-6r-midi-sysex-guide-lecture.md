---
title: Oberheim Matrix-6 et Matrix-6R — Guide MIDI et SysEx (lecture facile)
author: Guillaume DUPONT
language: fr
purpose: Usage personnel — synthèse lisible de la spec MIDI/SysEx Matrix-6/6R
source: Reformattage à partir d’un document communautaire (firmware 2.13 et plus)
companion: oberheim-matrix-1000-midi-sysex-implementation.md
created: 2026-05-29
---

# Oberheim Matrix-6 et Matrix-6R — Guide MIDI et SysEx

Document de lecture personnelle. Vocabulaire explicite, sans abréviations « internes » (pas de P/M/S, pas de M-6, etc.).

**Instruments concernés :** le synthé clavier Matrix-6 et le module rack Matrix-6R (même implémentation MIDI dans la mesure où rien n’est précisé autrement).

**Firmware visé par la source :** version **2.13 ou ultérieure**.

**Fiabilité :** document d’origine communautaire (pas un PDF Oberheim officiel). À croiser avec le manuel utilisateur et, si besoin, des tests sur le matériel.

**Document complémentaire (Matrix-1000) :** `oberheim-matrix-1000-midi-sysex-implementation.md` — transcription Oberheim officielle ; indispensable pour le format détaillé du patch single (134 octets), l’interrogation appareil (Device Inquiry) et certains opcodes absents ici.

---

## 1. Les trois modes de l’interface

Le Matrix-6 organise l’édition en **trois pages** (comme sur la face avant) :

| Page | Nom | Contenu |
|---|---|---|
| **Patch Edit** | Édition de patch | Paramètres qui **changent à chaque patch** (son programmé) |
| **Master Edit** | Édition master | Paramètres **globaux** (MIDI, vibrato, comportement général) — valables pour tous les patches |
| **Split Edit** | Édition de split | Paramètres d’un **split clavier** (deux patches joués en même temps sur des zones du clavier) |

Dans le manuel Oberheim, chaque paramètre a un **numéro** et une **page**. Ci-dessous : *« Master Edit, paramètre 01 »* plutôt que « M01 ».

---

## 2. Notation utilisée dans ce guide

- **Hexadécimal :** `19H` = 25 en décimal.
- **Canal MIDI :** les messages « sur le canal de base » utilisent le canal MIDI de base réglé dans Master Edit (paramètre 00 — Canal MIDI de base).
- **Plages :** « de 0 à 99 » = toutes les valeurs entières entre 0 et 99 inclus.
- **SysEx** = message MIDI *System Exclusive* (données réservées au fabricant), encadré par les octets `F0H` (début) et `F7H` (fin).
- **Émission / réception :** « le synthé envoie » = sortie MIDI ; « le synthé reçoit » = entrée MIDI.

---

## 3. Modes MIDI (Master Edit)

Les messages de **mode de canal** MIDI sont reçus sur le **canal de base**. Ils déclenchent un *All Notes Off* et mettent à jour notamment :

- **Master Edit, paramètre 01 — Mode Omni MIDI**
- **Master Edit, paramètre 18 — Mode Mono MIDI**

| Message MIDI reçu | Effet |
|---|---|
| All Notes Off | Les notes reçues sur le **même canal** que le message sont relâchées (gate off) |
| Autres changements de mode | Toutes les voix sont relâchées avant le nouveau mode |
| Mono On | Les **six voix** restent disponibles en mono, quel que soit le dernier octet du message |
| Omni On en mode Mono | Le mode Omni est **traité comme désactivé** tant que le mono est actif |

---

## 4. Notes et assignation des voix

Le synthé possède **six voix** polyphoniques internes.

### 4.1 Mode polyphonique MIDI

**Mode Omni (Master Edit, paramètre 01) :**

- **Activé :** le synthé répond aux notes sur les canaux MIDI 1 à 16.
- **Désactivé :** il ne répond qu’au **canal de base**.

#### Appui sur une touche / réception d’un Note On

1. Le synthé cherche une voix libre (non « gated »).
2. Le choix de la voix dépend de **Patch Edit, paramètre 48 — Mode clavier** :
   - **Rotate (rotation) :** prochaine voix libre dans l’ordre.
   - **Reassign (réassignation) :** préfère une voix qui jouait déjà la même hauteur ; sinon comme Rotate.
   - **Reassign avec Rob :** autorise à « voler » une voix déjà occupée pour une nouvelle note.

3. Si **toutes** les voix sont occupées :
   - **Rob activé :** une voix victime est choisie (même hauteur si possible), la hauteur est mise à jour, **déclenchement multiple** seulement (pas de nouveau déclenchement simple).
   - **Rob désactivé :** si **Master Edit, paramètre 13 — Spillover** est activé, un Note On est **émis en MIDI sortant** sur le canal de base **+ 2** (avec rebouclage si le canal dépasse 15 ou 16). Sinon, le Note On sort sur le **canal de base** seulement.

**Sortie MIDI des notes jouées au clavier :**

- Sur le **Matrix-6** (clavier) : les notes du clavier interne peuvent générer des messages en sortie MIDI.
- Sur le **Matrix-6R** (rack) : les notes ne viennent que du MIDI entrant ; **aucun** Note On/Off n’est renvoyé en sortie pour les voix jouées en interne.

#### Relâchement (Note Off)

Le synthé retrouve où la note avait été assignée (voix interne ou canal MIDI de spillover) et relâche la gate ; il peut renvoyer un Note Off en sortie selon l’origine de la note.

### 4.2 Mode monophonique MIDI

Chaque voix écoute un **canal MIDI distinct**, à partir du canal de base (bande de six canaux, avec rebouclage après le canal 16).

Le **mode 3 (Omni activé + Mono)** n’est **pas** supporté.

Si une deuxième note arrive sur un canal déjà occupé par une voix qui joue encore, la nouvelle note **vole** la voix (déclenchement multiple ; l’ancien Note Off ne libère pas la voix tant que le Note Off correspondant à la **nouvelle** note n’est pas reçu).

**Spillover :** sans effet en mode mono.

### 4.3 Mode Unisson

**Patch Edit, paramètre 48 = Unisson :** toutes les voix assignées jouent **la même** note (priorité à la **note la plus basse**). Avec spillover activé, la redirection MIDI peut survenir dès la **première** note (et non après la sixième).

En pratique, l’unisson est surtout utilisé avec le mode polyphonique.

---

## 5. Contrôleurs (levers, pédales, pression)

Un **contrôleur** est une valeur interne modifiable depuis le matériel ou depuis un message MIDI *Control Change*.

Les valeurs **locales** (matériel) s’**additionnent** avec celles reçues en MIDI.

| Contrôleur | Numéro MIDI par défaut | Comportement |
|---|---|---|
| Pédale 1 | 7 | Continue 0–127 ; **local** (une valeur par voix à la réception) |
| Pédale 2 | 64 | Interrupteur 0 ou 127 ; **local** |
| Levier 1 | Pitch Bend (pas un CC classique) | 8 bits (0–255) ; **non réassignable** |
| Levier 2 | 1 | Contrôle aussi le « levier 3 » sur la moitié « poussée » du levier physique |
| Levier 3 | 2 | Virtuel : autre moitié du levier 2 |

**Master Edit, paramètre 02 — Contrôleurs MIDI :** si désactivé, plus d’émission ni de réception des Control Change.

**Local vs global à la réception MIDI :**

- **Locaux** (valeur par voix selon le canal) : Levier 1, Pédale 1, Pression.
- **Globaux** (toutes les voix) : les autres.

---

## 6. Mode Split (deux patches sur le clavier)

Deux **patches simples** (Upper = partie haute, Lower = partie basse) avec un **point de split** programmable.

Répartition des six voix : 6/0, 4/2, 2/4 ou 0/6 (nombre de voix pour Lower / Upper).

| Aspect | Règle |
|---|---|
| Canal MIDI partie basse | Canal de base **N** |
| Canal MIDI partie haute | Canal de base **N + 1** |
| Spillover partie basse | Canal **N + 2** |
| Spillover partie haute | Canal **N + 3** |
| Paramètres Master Edit | S’appliquent aux **deux** patches (ex. Omni, Mono, Spillover) |

**Dump SysEx d’un split :** opcode **02H** (voir section 8).

---

## 7. Format général des messages SysEx Oberheim

Structure de tout message SysEx Matrix-6 / Matrix-6R :

```
F0H   10H   [identifiant appareil]   [code opération]   [données…]   F7H
```

| Octet | Signification |
|---|---|
| `F0H` | Début SysEx |
| `10H` | Identifiant fabricant Oberheim |
| Identifiant appareil | **`06H`** — format natif Matrix-6 / Matrix-6R |
| | **`02H`** — format compatible Matrix-12 / Xpander (accepté à la **réception** ; le Matrix-6 **émet** en `06H`) |
| `F7H` | Fin du message (EOX) |

### Délais importants

- Après avoir **envoyé** un SysEx, le Matrix-6 attend **20 millisecondes** avant tout autre trafic.
- À la **réception** de dumps (patches, etc.), laisser **au moins 20 ms** entre deux messages.

*(Le Matrix-1000 officiel indique **10 ms** entre patches — ne pas supposer la même valeur sur le Matrix-6.)*

### Condition d’activation

**Master Edit, paramètre 04 — System Exclusive :** doit être **activé** pour que le synthé émette ou interprète des SysEx.

Messages ignorés si : mauvais fabricant, identifiant appareil autre que `06H` ou `02H`, ou opcode invalide.

---

## 8. Opérations SysEx (identifiant appareil `06H`)

| Opcode | Nom | Sens |
|---|---|---|
| `00H` | Demande de patch simple | Réception seulement (legacy ; variante v1.xx : `F0 10 06 00 [numéro patch] F7`) |
| `01H` | Données d’un patch simple | Émission et réception — **134 octets** de données packées |
| `02H` | Données d’un patch split | Émission et réception — **18 octets** |
| `03H` | Données des paramètres Master | Émission et réception — **236 octets** |
| `04H` | Demande de dump | Réception seulement |
| `05H` | Entrer en édition distante rapide | Réception — à envoyer **avant** de changer des paramètres un par un |
| `06H` | Changer un paramètre | Réception — numéro de paramètre 0–99, valeur **non négative** |

**Opcodes documentés pour le Matrix-1000 mais absents de la spec Matrix-6 communautaire :** par exemple changement de banque (`0AH`), édition d’un bus de modulation Matrix (`0BH`). À vérifier sur matériel avant usage sur Matrix-6/6R.

### 8.1 Demande de dump (`04H`)

```
F0H  10H  06H  04H  [type de demande]  [numéro patch]  F7H
```

| Type de demande | Contenu transmis |
|---|---|
| 0 | Tous les patches simples, tous les splits, et le master (chaque bloc = message SysEx séparé) |
| 1 | Un patch simple |
| 2 | Un patch split |
| 3 | Les paramètres Master |

**Numéro patch :** 0–99 pour un patch simple, 0–49 pour un split ; ignoré pour les types 0 et 3 (octet de remplissage tout de même présent).

**Dump complet :** environ 29 Ko au total ; fin de transfert : confirmation utilisateur ou **plus de 500 ms** sans octet reçu.

### 8.2 Encodage des données (nibbles)

Comme sur le Matrix-1000 : chaque octet de données est envoyé en **deux demi-octets** MIDI (4 bits faibles d’abord, 4 bits forts ensuite). **Checksum** = somme sur 7 bits des octets **packés** (non transmis tels quels).

| Type de message | Taille packée | Octets transmis (approx.) |
|---|---|---|
| Patch simple | 134 | 275 |
| Patch split | 18 | 42 |
| Master | 236 | 477 |

À la réception d’un patch : vérification de la protection matérielle et de la protection du patch avant écriture en mémoire.

### 8.3 Édition distante paramètre par paramètre

1. Envoyer **`05H`** — le synthé doit déjà être en mode **Patch Edit** sur la face avant.
2. Envoyer **`06H [numéro paramètre] [valeur]`** — hors plage = ignoré ; **pas de valeurs négatives** par ce chemin (envoyer le patch complet si besoin).

**Compatibilité Matrix-12 / Xpander** (identifiant `02H`, opcode `0DH`) :

| Sous-code | Mode |
|---|---|
| `01H` | Mode patch simple |
| `02H` | Mode split (« multi-patch ») |

### 8.4 Envoi depuis la face avant (Master Edit)

| Paramètre Master Edit | Action |
|---|---|
| Paramètre 10 — Send Data | Envoie le patch simple ou split courant |
| Paramètre 11 — Send All | Envoie les 100 patches simples + 50 splits + master (~12 secondes) |

---

## 9. Autres fonctions MIDI

| Sujet | Règle |
|---|---|
| **Changement de patch (Program Change)** | Si Master Edit paramètre 03 activé : émission et réception ; max patch 99 (mode simple) / 49 (mode split) |
| **Table de correspondance des patches (Patch Map)** | Paramètres 15 et 16 traduisent les numéros de program change entrants/sortants |
| **Echo MIDI** | Paramètre 12 — recopie l’entrée MIDI vers la sortie (**pas** les SysEx) ; attention aux conflits avec spillover |
| **Running Status** | Supporté en émission et réception |
| **Note Off** | Toujours un vrai Note Off avec vélocité de relâchement (pas un Note On à vélocité 0 en émission) |
| **Contrôle local** | Paramètre 05 — désactivé : le clavier ne pilote plus les voix internes ; spillover aussi désactivé |
| **Active Sensing** | Paramètre 14 — émission d’un Active Sense après 240 ms d’inactivité sortante ; All Notes Off si 360 ms sans activité entrante |
| **Tune Request** | Accorde les oscillateurs haute fréquence ; distinct de la fonction CALIBRATE |

---

## 10. Formats de données

### 10.1 Patch simple (134 octets packés)

**Identique au Matrix-1000.** Tableau octet par octet : voir le document Matrix-1000 (`oberheim-matrix-1000-midi-sysex-implementation.md`, section *Single Patch Data Format*).

**Exemples de plages de valeurs** (tous les paramètres ne sont pas limités à 0–63) :

| Encodage | Exemples |
|---|---|
| 6 bits non signé | Temps d’enveloppe, vitesses LFO, largeur d’impulsion DCO |
| 6 bits signé | Détune oscillateur 2 |
| 7 bits non signé | Fréquence du filtre (paramètre patch n° 21) |
| 7 bits signé | Montants de modulation, montants des bus Matrix |
| Codes 5 bits | Source/destination des bus Matrix, entrée du générateur de tracking |

### 10.2 Patch split (18 octets packés)

| Octet | Paramètre Split Edit | Bits | Description |
|---|---|---|---|
| 0–5 | Nom du split | 6 chacun | Caractères ASCII sur 6 bits |
| 6–7 | — | | Non utilisés |
| 8 | — | 7 | Numéro du patch **Lower** (partie basse) |
| 9 | — | 7 | Numéro du patch **Upper** (partie haute) |
| 10 | Paramètre 00 | 7 | Limite de zone **gauche** du clavier |
| 11 | Paramètre 01 | 6 signé | Transposition zone gauche |
| 12 | Paramètre 02 | 1 | Sortie MIDI activée pour la zone gauche |
| 13 | Paramètre 03 | 7 | Limite de zone **droite** |
| 14 | Paramètre 04 | 6 signé | Transposition zone droite |
| 15 | Paramètre 05 | 1 | Sortie MIDI activée pour la zone droite |
| 16 | Paramètre 06 | 6 signé | Balance gauche/droite (−31 = gauche seule … +31 = droite seule) |
| 17 | Paramètre 07 | 2 | Répartition des voix : 0 = 2/4, 1 = 4/2, 2 = 6/0, 3 = 0/6 |

**42 octets** transmis au total (avec en-tête, checksum et fin).

### 10.3 Paramètres Master (236 octets packés)

| Octet | Réf. manuel | Bits | Description |
|---|---|---|---|
| 0 | — | | Non utilisé |
| 1 | Paramètre 30 | 6 | Vitesse du vibrato |
| 2 | Paramètre 31 | 3 | Forme d’onde du vibrato |
| 3 | Paramètre 32 | 6 | Amplitude du vibrato |
| 4 | Paramètre 33 | 2 | Source de modulation de la vitesse du vibrato |
| 5 | Paramètre 34 | 6 | Montant de modulation de la vitesse du vibrato |
| 6 | Paramètre 35 | 2 | Source de modulation de l’amplitude du vibrato |
| 7 | Paramètre 36 | 6 | Montant de modulation de l’amplitude du vibrato |
| 8 | Paramètre 55 | 6 signé | Accord général (Master Tune) |
| 9 | Paramètre 40 | 2 | Type d’échelle de vélocité |
| 10 | Paramètre 41 | 6 | Sensibilité vélocité |
| 11 | Paramètre 00 | 4 | Canal MIDI de base |
| 12 | Paramètre 01 | 1 | Mode Omni MIDI |
| 13 | Paramètre 02 | 1 | Contrôleurs MIDI activés |
| 14 | Paramètre 03 | 1 | Changements de patch MIDI activés |
| 15 | Paramètre 04 | 1 | SysEx activés |
| 16 | Paramètre 05 | 1 | Contrôle local (clavier → voix) |
| 17–20 | Paramètres 06–09 | 7 chacun | Numéros MIDI des pédales et leviers |
| 21 | Paramètre 42 | 1 | Inversion pédale 2 |
| 22 | Paramètre 43 | 1 | Inversion leviers |
| 23 | Paramètre 53 | 5 | Luminosité afficheur |
| 24 | Paramètre 56 | 1 | Mode SQUICK |
| 25 | Paramètre 17 | 1 | Echo de la table de patches |
| 26 | Paramètre 57 | 1 | **Sortie stéréo activée** |
| 27 | — | | Non utilisé |
| 28 | Paramètre 44 | 6 | Seuil de pression (standoff) |
| 29 | Paramètre 13 | 1 | Spillover activé |
| 30 | — | | Non utilisé |
| 31 | Paramètre 14 | 1 | Active Sensing activé |
| 32 | Paramètre 12 | 1 | Echo MIDI activé |
| 33 | Paramètre 15 | 1 | Table de correspondance des patches activée |
| 34 | — | | Non utilisé |
| 35 | Paramètre 18 | 1 | Mode Mono MIDI |
| 36–135 | — | 6 chacun | Table d’entrée : correspondance program change → patch (100 entrées) |
| 136–235 | — | 6 chacun | Table de sortie : correspondance patch → program change (100 entrées) |

**Comparaison Matrix-1000 :** le master du Matrix-1000 fait **172 octets** avec une disposition différente.

---

## 11. Comparaison Matrix-1000 / Matrix-6 / Matrix-6R

| Sujet | Matrix-1000 | Matrix-6 / Matrix-6R |
|---|---|---|
| Taille patch simple packé | 134 octets | **Identique** |
| Taille master packé | 172 octets | **236 octets** |
| Délai minimum entre dumps patch | 10 ms (doc officielle) | 20 ms (doc communautaire) |
| Patches split | 50 splits « factices » lors d’un « Request All » | 50 splits natifs, opcode `02H` |
| Identifiant dans `F0 10 [id] …` | `06H` | `06H` natif ; `02H` accepté à la réception |
| Opcode bus Matrix `0BH` | Documenté | Non listé ici — vérifier sur matériel |
| Device Inquiry (interrogation universelle) | Documenté | **Absent** de la source Matrix-6 — voir section 12 |

### Table des formes d’onde LFO (codes 0–7)

0 Triangle · 1 Dent de scie montante · 2 Dent de scie descendante · 3 Carré · 4 Aléatoire · 5 Bruit · 6 Modulation échantillonnée · 7 Non utilisé

### Table des sources de modulation (codes 0–20)

0 Inutilisé* · 1 Enveloppe 1 · 2 Enveloppe 2 · 3 Enveloppe 3 · 4 LFO 1 · 5 LFO 2 · 6 Vibrato · 7 Ramp 1 · 8 Ramp 2 · 9 Clavier · 10 Portamento · 11 Générateur de tracking · 12 Gate clavier · 13 Vélocité · 14 Vélocité de relâchement · 15 Pression · 16 Pédale 1 · 17 Pédale 2 · 18 Levier 1 · 19 Levier 2 · 20 Levier 3

\* Le code 0 « inutilisé » existe dans la liste des **sources** de modulation, mais **pas** comme entrée du générateur de tracking (paramètre patch n° 33 — entrée du tracking generator).

### Table des destinations de modulation (codes 1–32)

1 Fréquence oscillateur 1 · 2 Largeur d’impulsion osc. 1 · 3 Forme d’onde osc. 1 · 4 Fréquence oscillateur 2 · 5 Largeur d’impulsion osc. 2 · 6 Forme d’onde osc. 2 · 7 Niveau mix · 8 Montant FM filtre · 9 Fréquence filtre · 10 Résonance filtre · 11 Niveau VCA 1 · 12 Niveau VCA 2 · 13–16 Enveloppe 1 (delay, attack, decay, release) · 17 Amplitude enveloppe 1 · 18–22 Enveloppe 2 · 23–27 Enveloppe 3 · 28–29 LFO 1 · 30–31 LFO 2 · 32 Temps de portamento

---

## 12. Interrogation appareil (Device Inquiry) — spec Matrix-1000

La spec communautaire Matrix-6 **ne décrit pas** l’interrogation universelle. Elle est définie dans la doc **officielle Matrix-1000** :

**Demande (tout appareil MIDI) :**

`F0H  7EH  7FH  06H  01H  F7H`

(`7FH` = canal « tous les appareils »)

**Réponse typique Matrix-1000 :**

`F0H  7EH  [canal]  06H  02H  10H  06H  00H  [membre bas]  [membre haut]  [4 caractères version]  F7H`

| Champ | Matrix-1000 (officiel) |
|---|---|
| Fabricant | `10H` Oberheim |
| Famille (octet bas) | `06H` — famille Matrix-6 / Matrix-6R / Matrix-1000 |
| Famille (octet haut) | `00H` |
| Membre (octet bas) | **`02H`** — identifie le **Matrix-1000** |
| Membre (octet haut) | **`00H`** |
| Version | 4 caractères ASCII, alignés à droite (ex. espaces + `110` = version 1.10) |

**Matrix-6 / Matrix-6R :** les octets « membre » ne sont **pas** documentés dans les fichiers archivés ; ils diffèrent probablement du Matrix-1000. À confirmer en envoyant la demande et en lisant la réponse sur le matériel.

---

## 13. Annexe — Messages MIDI canal (résumé)

### Messages « voix » (notes, contrôleurs, etc.)

| Statut | Données | Description |
|---|---|---|
| `8nH` | note, vélocité | Note Off (vélocité de relâchement 1–127) |
| `9nH` | note, vélocité | Note On (vélocité 0 = Note Off en **réception** seulement) |
| `BnH` | numéro contrôleur, valeur | Control Change (si activé) |
| `CnH` | numéro de programme | Changement de patch (si activé) |
| `EnH` | octet faible, octet fort | Pitch Bend |

### Messages « mode canal »

| Numéro CC | Valeur | Fonction |
|---|---|---|
| 122 | 0 / 127 | Contrôle local désactivé / activé |
| 123 | 0 | All Notes Off |
| 124 | 0 | Omni Off |
| 125 | 0 | Omni On |
| 126 | 6 | Mono On |
| 127 | 0 | Mono Off |

---

## 14. Astuce — Volume MIDI sur Matrix-6 / Matrix-6R

Pour moduler le volume via le **Control Change 7 (Volume)** routé comme Pédale 1, pilotant l’enveloppe 2 → VCA 2 :

1. **Master Edit, paramètre 06 — Pédale 1 :** assigner le contrôleur MIDI **7**.
2. **Patch Edit :** régler VCA1/vélocité à +63, VCA2/enveloppe 2 à +63, amplitude enveloppe 2 à 0, enveloppe 2/vélocité à 0 (ajuster au goût).
3. **Matrix Modulation :** source Pédale 1, montant +63, destination amplitude enveloppe 2.

Ces réglages sont **par patch** (patch + matrix modulation).

---

*Fin du guide — usage personnel, 2026-05-29.*
