---
Organisation: Ten Square Software
Auteur: Guillaume DUPONT
Projet: Matrix-Control
Titre: Matrix-Control — Manuel utilisateur
Version: 0.1
Version-Produit: 0.1.2-alpha
Created: 2026-07-20
Updated: 2026-07-20
References:
  - Documentation/User/user-manual.md (à venir)
  - CONTRIBUTING.md
  - README.md
---

# Matrix-Control — Manuel utilisateur

> **Lexique.** Les termes techniques et acronymes utilisés dans ce manuel sont expliqués en fin de document, dans le [lexique](#lexique). Vous pouvez y revenir à tout moment.

---

## Table des matières

1. [À quoi sert Matrix-Control ?](#1-à-quoi-sert-matrix-control)
2. [Ce dont vous aurez besoin](#2-ce-dont-vous-aurez-besoin)
3. [Le Matrix-1000 et le Matrix-6/6R en deux minutes](#3-le-matrix-1000-et-le-matrix-66r-en-deux-minutes)
4. [Installer et ouvrir le logiciel](#4-installer-et-ouvrir-le-logiciel)
5. [Brancher le synthé et établir le dialogue MIDI](#5-brancher-le-synthé-et-établir-le-dialogue-midi)
6. [Premier contact : l’écran principal](#6-premier-contact-lécran-principal)
7. [Éditer un patch (PATCH EDIT)](#7-éditer-un-patch-patch-edit)
8. [La matrice de modulation](#8-la-matrice-de-modulation)
9. [Choisir un patch dans le synthé (INTERNAL PATCHES)](#9-choisir-un-patch-dans-le-synthé-internal-patches)
10. [Banques du Matrix-1000 (BANK UTILITY)](#10-banques-du-matrix-1000-bank-utility)
11. [Sauvegarder sur le synthé et sur l’ordinateur](#11-sauvegarder-sur-le-synthé-et-sur-lordinateur)
12. [Le Patch Mutator](#12-le-patch-mutator)
13. [Réglages globaux du synthé (MASTER EDIT)](#13-réglages-globaux-du-synthé-master-edit)
14. [Apparence, échelle et Settings](#14-apparence-échelle-et-settings)
15. [Mode autonome (Standalone) et mode plugin](#15-mode-autonome-standalone-et-mode-plugin)
16. [Dépannage rapide](#16-dépannage-rapide)
17. [Lexique](#lexique)

---

## 1. À quoi sert Matrix-Control ?

Matrix-Control est un **éditeur MIDI** très complet pour les synthétiseurs Oberheim **Matrix-1000**, **Matrix-6** et **Matrix-6R**.

Il ne produit pas le son lui-même : celui-ci est toujours généré par votre synthétiseur. Matrix-Control agit plutôt comme une télécommande sophistiquée : il envoie et reçoit des messages MIDI afin que vous puissiez **visualiser et modifier** les réglages du synthé à l’écran — à l’aide de boutons, de curseurs, de menus déroulants et d’afficheurs graphiques — au lieu de tout manipuler depuis la façade de la machine.

Vous pouvez l’utiliser :

- **Dans un DAW** (Ableton Live, Logic Pro, Reason Studios, Cubase, Reaper, GarageBand, etc.), comme instrument virtuel qui dialogue avec le synthé 
- **En application autonome** (`Standalone`), sans DAW

Pour les crédits et les contacts, ouvrez `ABOUT...` depuis le menu du logo `MATRIX-CONTROL`.

> **Capture d’écran — À placer ici**  
> *Légende :* Fenêtre principale de Matrix-Control avec un Matrix-1000 détecté.

---

## 2. Ce dont vous aurez besoin

### Matériel

- Un synthétiseur Oberheim **Matrix-1000**, **Matrix-6** ou **Matrix-6R**
- Une interface MIDI : boîtier USB-MIDI dédié, interface audio équipée de ports MIDI, ou câbles MIDI associés à une interface MIDI basique, selon votre installation
- **Deux sens de communication** opèrent simultanément entre l’ordinateur et le synthé :
  - De l’ordinateur **vers** le synthé (pour transmettre les patches et les réglages globaux) 
  - Du synthé **vers** l’ordinateur (pour que Matrix-Control lise le patch courant et reconnaisse la machine)

Sans ces deux directions, l’éditeur ne pourra pas piloter le synthé correctement.

### Ordinateur

| Système | Ce que vous pouvez installer |
|---------|------------------------------|
| **macOS** | Plugin AU, plugin VST3, application Standalone |
| **Windows** | Plugin VST3, application Standalone |
| **Linux** | Plugin VST3, application Standalone |

### Pour démarrer sereinement

- Savoir brancher des ports et câbles MIDI IN / MIDI OUT (ou USB-MIDI)
- Savoir ouvrir un instrument virtuel dans votre DAW, **ou** lancer une application comme n’importe quel autre programme sur votre ordinateur
- L’interface de Matrix-Control est en **anglais** : ce manuel traduit et explique chaque section, module et zone

---

## 3. Le Matrix-1000 et le Matrix-6/6R en deux minutes

Ces machines partagent le même genre de « moteur » sonore (deux oscillateurs, un filtre passe-bas résonant, trois enveloppes, deux LFO, une matrice de modulation, etc.). Ce qui change surtout pour vous dans Matrix-Control :

| | **Matrix-1000** | **Matrix-6/6R** |
|---|-----------------|-----------------|
| Mémoire des patches | **10 banques** (0 à 9), **100 patches** par banque (0 à 99) | **100 patches** seulement (pas de banques) |
| Module `BANK UTILITY` | Actif | Grisé (indisponible) |
| Section `MASTER EDIT` | Active (réglages globaux : canal MIDI, vibrato, accordage…) | Grisée (indisponible) |
| Façade | Très peu de boutons : l’éditeur MIDI à l’écran s’avère particulièrement utile | Clavier (Matrix-6) ou rack (Matrix-6R) avec davantage de commandes directes |

**Banques en lecture seule du Matrix-1000 : **Sur le Matrix-1000, les banques **2 à 9** sont en lecture seule (mémoire ROM) pour certaines actions : les boutons `INIT`, `PASTE` et `STORE` y sont indisponibles. Les banques **0** et **1** sont celles dans lesquelles vous pouvez enregistrer vos modifications dans la machine (mémoire RAM).

**Reconnaissance du synthé :** Dès que les ports MIDI sont correctement sélectionnés, Matrix-Control demande au synthé de s’identifier. En bas à droite de la fenêtre, vous voyez alors le type de machine détecté, ou `No device` s’il n’y a pas encore de dialogue.

> **À noter.** Un Matrix-6R peut pour l’instant apparaître simplement comme Matrix-6. Pour éditer un patch au quotidien, cela ne change presque rien : `BANK UTILITY` et `MASTER EDIT` restent réservés au Matrix-1000.

---

## 4. Installer et ouvrir le logiciel

### Télécharger et installer

Matrix-Control se télécharge sous la forme de fichiers ZIP comprenant une application autonome et les plugins compatibles avec votre système, accompagnés de la documentation utilisateur :

- **Plugin AU** (macOS) — à placer dans le dossier des composants Audio Units (en général celui proposé par l’installateur ou indiqué dans les instructions du téléchargement)
- **Plugin VST3** (macOS, Windows, Linux) — à placer dans le dossier VST3 habituel de votre système
- **Application Standalone** — l’application Matrix-Control, à lancer comme n’importe quel autre programme

Si le plugin n’apparaît pas dans le DAW, rescannez la liste des plugins et vérifiez que les fichiers ont bien été installés dans le dossier attendu (voir aussi les instructions fournies avec le téléchargement).

### Dans un DAW

1. Créez une piste **MIDI / instrument**
2. Chargez **Matrix-Control** comme instrument virtuel
3. Assurez-vous que la piste peut envoyer et recevoir du MIDI vers / depuis votre interface (selon votre DAW)
4. Le signal audio du synthé doit être branché séparément dans le DAW (connexion à une entrée de votre interface audio, ou retour de table de mixage). Matrix-Control ne remplace pas le synthé : il le pilote en MIDI

### En Standalone

1. Lancez l’application **Matrix-Control**
2. Au besoin, ouvrez `AUDIO/MIDI...` depuis le menu du logo `MATRIX-CONTROL` (voir [section 15](#15-mode-autonome-standalone-et-mode-plugin)) pour choisir l’interface audio et les périphériques MIDI

> **Capture d’écran — À placer ici**  
> *Légende :* Matrix-Control chargé comme instrument dans un DAW.

---

## 5. Brancher le synthé et établir le dialogue MIDI

C’est l’étape la plus importante. Tant que le synthé n’est pas reconnu, une grande partie de l’éditeur demeure verrouillée.

### Câblage type

1. **MIDI OUT de l’interface / de l’ordinateur** → **MIDI IN du synthé**
2. **MIDI OUT du synthé** → **MIDI IN de l’interface / de l’ordinateur**
3. Allumez le synthé
4. Vérifiez que le canal MIDI du synthé est cohérent avec celui que vous utiliserez (sur Matrix-1000, une fois connecté, vous pourrez également le régler dans `MASTER EDIT` → `MIDI` → `CHANNEL`)

### Choisir les ports en haut de la fenêtre

| Libellé à l’écran | Rôle |
|-------------------|------|
| `EDITOR MIDI FROM :` | Ce que le synthé **envoie** à l’éditeur (réponses, contenu d’un patch). Sélectionnez le port branché sur le MIDI OUT du synthé. Si rien n’est choisi : `NO INPUT`. |
| `MIDI TO :` | Ce que l’éditeur **envoie** au synthé. Sélectionnez le port branché sur le MIDI IN du synthé. Si rien n’est choisi : `NO OUTPUT`. |
| `KEYBOARD FROM :` | Provenance des notes jouées. En **plugin** : fixé à `HOST` (géré par le DAW), non modifiable. En **Standalone** : vous choisissez un clavier maître MIDI ou un port MIDI. |

Des voyants situés à côté des listes clignotent lorsque des messages MIDI circulent.

Il n’existe **pas** de bouton de connexion automatique. En pratique :

- Le logiciel mémorise les ports choisis 
- Il tente de les rouvrir au prochain lancement 
- Dès que `MIDI FROM` et `MIDI TO` sont ouverts, il demande au synthé de s’identifier

### Tant que le synthé n’est pas détecté

Si aucun appareil n’est reconnu, le bas de la fenêtre affiche un message du type :

> *No synth detected — check MIDI cables, MIDI From / MIDI To, and power-cycle the Matrix.*

Les sections d’édition restent alors verrouillées jusqu’à détection. Ce comportement est voulu : Matrix-Control n’envoie pas de changements à l’aveugle.

### Windows : port déjà utilisé

Sous Windows, un port MIDI est souvent réservé à une seule application à la fois. Si un autre logiciel (ou le DAW) l’occupe déjà, Matrix-Control peut indiquer qu’il n’a pas pu l’ouvrir. Fermez alors l’autre application, ou utilisez un port MIDI virtuel (par exemple loopMIDI) afin de partager le flux.

> **Capture d’écran — À placer ici**  
> *Légende :* Ports MIDI correctement choisis, voyants actifs, et type de synthé visible en bas à droite.

---

## 6. Premier contact : l’écran principal

L’écran s’organise en trois bandes :

```
┌─ En haut ─────────────────────────────────────────────────────────┐
│  Logo MATRIX-CONTROL · MIDI FROM / TO · KEYBOARD FROM · (audio)   │
├─ Au centre ───────────────────────────────────────────────────────┤
│  PATCH EDIT  │  MATRIX MODULATION + PATCH MANAGER  │ MASTER EDIT  │
├─ En bas ──────────────────────────────────────────────────────────┤
│  Messages                                        Type de machine  │
└───────────────────────────────────────────────────────────────────┘
```

### En haut

- Logo `MATRIX-CONTROL` (clic → menu : `UI SCALE` pour la taille de l’interface, `SKIN` pour l’apparence, `SETTINGS...`, `ABOUT...`, et `AUDIO/MIDI...` pour l’application Standalone)
- Sélection des ports MIDI (section précédente)
- En Standalone uniquement : `AUDIO FROM` et `INPUT GAIN` pour écouter le retour audio du synthé dans l’application

### Au centre — trois colonnes

Dans ce manuel, nous désignons les grands blocs de l’interface comme suit :

- **Section** — `PATCH EDIT`, `MATRIX MODULATION`, `PATCH MANAGER`, `MASTER EDIT` 
- **Module** — par exemple `DCO 1`, `BANK UTILITY`, `PATCH MUTATOR` 
- **Bus de modulation** — chacune des dix voies de `MATRIX MODULATION` 
- **Zone** — sous-ensembles comme `BROWSER` ou `MEMORY` 
- **Afficheur** — représentations graphiques (enveloppes, Track Generator) ou panneau de nom de patch

| Colonne | Contenu |
|---------|---------|
| **Gauche — section `PATCH EDIT`** | Les modules de synthèse du patch : oscillateurs, filtre, enveloppes, LFO, etc. |
| **Centre** | Section `MATRIX MODULATION` (dix bus de modulation) + section `PATCH MANAGER` (banques, patches du synthé, fichiers sur l’ordinateur, mutateur) |
| **Droite — section `MASTER EDIT`** | Réglages globaux du Matrix-1000 (grisée sur Matrix-6/6R) |

### En bas

- **À gauche :** messages d’information, de succès, d’avertissement ou d’erreur (icônes ℹ ✓ ⚠ ✗)
- **À droite :** machine détectée (par ex. Matrix-1000) ou `No device`

> **Capture d’écran — À placer ici**  
> *Légende :* Vue d’ensemble des trois colonnes et du bandeau du bas.

---

## 7. Éditer un patch (PATCH EDIT)

Une fois le synthé détecté et un patch chargé (voir [section 9](#9-choisir-un-patch-dans-le-synthé-internal-patches)), vous travaillez dans la section `PATCH EDIT`.

### Principe

Chaque mouvement de bouton, de curseur ou de menu déroulant est transmis au synthé. Vous entendez immédiatement le résultat sur la machine (et éventuellement via l’entrée audio en Standalone).

Matrix-Control ne propose pas de fonction annuler / rétablir (`Undo` / `Redo`). Pour sécuriser votre travail, utilisez `STORE` (écriture dans la mémoire du synthé), `SAVE` / `SAVE AS` (fichier sur l’ordinateur), ou le bouton `C` (Compare) du module `PATCH MUTATOR` lorsque vous explorez des variations.

### Boutons `I` / `C` / `P`

Sur plusieurs modules, l’en-tête propose trois actions :

| Bouton | Signification | Action |
|--------|---------------|--------|
| `I` | Init | Réinitialise le module avec ses valeurs par défaut |
| `C` | Copy | Copie les valeurs des paramètres du module |
| `P` | Paste | Colle les valeurs précédemment copiées dans ce module |

Le bouton `P` n’est actif que lorsque le contenu du presse-papiers est compatible avec le module cible. Dans le cas contraire, il apparaît grisé ; un clic sur ce bouton grisé affiche en bas de fenêtre un message indiquant que le presse-papiers n’est pas compatible.

### Nom du patch (`PATCH NAME`)

L’afficheur `PATCH NAME` présente le nom du patch courant (8 caractères). Il est en **lecture seule** : vous ne pouvez pas le modifier à l’écran. Il se met à jour lorsqu’un patch est reçu depuis le synthé, lorsqu’un fichier de patch est ouvert depuis votre ordinateur, ou lorsque vous créez des variantes à l’aide du Patch Mutator.

### Modules de synthèse

La section `PATCH EDIT` regroupe les briques classiques de la synthèse soustractive Oberheim. Les libellés à l’écran sont en anglais ; le [lexique](#lexique) précise les sigles (DCO, VCF, VCA, LFO, ENV, etc.).

- **`DCO 1` et `DCO 2`** — Les oscillateurs produisent les formes d’onde de base à l’origine du patch (hauteur, largeur d’impulsion, forme d’onde, synchronisation, etc.)
- **`VCF/VCA`** — Le filtre (`VCF`) sculpte le timbre dans le domaine fréquentiel (fréquence de coupure, résonance, balance des oscillateurs…) ; le `VCA` pilote le volume et sa réponse à la vélocité ou aux enveloppes
- **`FM/TRACK`** — Module de modulation de fréquence du filtre et générateur de tracking : cinq points définissent une courbe selon la note jouée. L’afficheur du Track Generator est **interactif** : vous pouvez déplacer les points à la souris
- **`RAMP/PORTAMENTO`** — Rampes de modulation et glissando entre les notes (portamento), ainsi que le mode clavier. Si `LEGATO PORTA` et le mode clavier `UNISON` sont incompatibles, un message l’indique en bas de fenêtre
- **`ENV 1`, `ENV 2` et `ENV 3`** — Les trois enveloppes façonnent l’évolution du patch dans le temps (attaque, decay, sustain, release…). Leurs **afficheurs sont éditables à la souris** : vous pouvez manipuler directement la forme graphique
- **`LFO 1` et `LFO 2`** — Oscillateurs lents qui animent hauteur, filtre, amplitude ou d’autres destinations, selon la matrice de modulation et les routages locaux

> **Capture d’écran — À placer ici**  
> *Légende :* Section `PATCH EDIT` avec les modules DCO, VCF/VCA et un afficheur d’enveloppe.

---

## 8. La matrice de modulation

La section `MATRIX MODULATION` constitue un peu le **centre nerveux** du synthé. C’est ici que l’on donne vie aux patches, en reliant des sources de modulation à des paramètres de destination — d’où le nom des machines Matrix-1000 et Matrix-6/6R.

Elle propose **dix bus de modulation** numérotés de `0` à `9`.

Chaque bus de modulation possède :

- Un menu déroulant `SOURCE` — vous y définissez le signal qui produit la modulation (enveloppes, LFO, clavier, vélocité, pression, pédales, leviers, etc., ou `NONE`) 
- Un contrôle `AMOUNT` — il détermine l’intensité de la modulation appliquée au paramètre de destination 
- Un menu déroulant `DESTINATION` — vous y choisissez le paramètre du synthé qui reçoit le signal de modulation, ce qui a pour effet de le faire évoluer en temps réel (hauteur, largeur d’impulsion, filtre, volume, enveloppes, LFO, portamento, etc., ou `NONE`)

Vous pouvez également :

- Réinitialiser un bus (`I` sur le bus) ou toute la section 
- Copier / coller la section (`C` / `P`) — là encore, le collage n’est possible que vers une destination compatible 
- **Réordonner** les bus en effectuant un **cliquer-glisser vertical** sur le numéro de bus (`#`). Pendant le geste, le point de départ apparaît en **gris** et le point d’arrivée en **rouge**. Les numéros de bus restent `0` à `9` ; ce sont les contenus (source, amount, destination) qui s’échangent

> **Capture d’écran — À placer ici**  
> *Légende :* Section `MATRIX MODULATION` avec les dix bus SOURCE / AMOUNT / DESTINATION.

---

## 9. Choisir un patch dans le synthé (INTERNAL PATCHES)

Dans `PATCH MANAGER` → module `INTERNAL PATCHES` :

### Zone `BROWSER`

- Boutons `<` et `>` pour passer au patch précédent / suivant
- Numéro de **patch** (0–99) : affiché ; un double-clic permet de saisir directement un numéro
- Numéro de **banque** (Matrix-1000) : affiché ; pour changer de banque, utilisez surtout le module `BANK UTILITY` (boutons `0` à `9`). Sur Matrix-6/6R, la notion de banque n’existe pas

Lorsque vous changez de banque ou de patch, Matrix-Control :

1. Peut afficher la boîte de dialogue `Unsaved edits` si des modifications n’ont été ni stockées dans le synthé (`STORE`) ni sauvegardées en fichier `.syx` (selon le réglage `UNSAVED` des Settings) — choix `Cancel` pour rester sur le patch courant, ou `Continue` pour poursuivre et abandonner ces modifications 
2. Demande au synthé de basculer sur cet emplacement 
3. **Lit** le patch depuis le synthé et l’affiche dans l’éditeur 
4. **N’écrit pas** automatiquement ce patch en retour juste après cette lecture

### Zone `MEMORY`

| Bouton | Rôle |
|--------|------|
| `INIT` | Réinitialise le patch avec ses valeurs par défaut |
| `COPY` | Copie le patch courant |
| `PASTE` | Colle un patch précédemment copié |
| `STORE` | Enregistre le patch dans l’emplacement courant **du synthé** |

Sur les banques en lecture seule **2–9** du Matrix-1000, `INIT`, `PASTE` et `STORE` sont indisponibles ; un message en bas de fenêtre l’explique.

> **Capture d’écran — À placer ici**  
> *Légende :* Module `INTERNAL PATCHES` avec les zones `BROWSER` et `MEMORY`.

---

## 10. Banques du Matrix-1000 (BANK UTILITY)

Le module `BANK UTILITY` est réservé au **Matrix-1000**. Sur Matrix-6/6R, il est grisé : ces modèles ne disposent pas de banques. Un message du type « Matrix-1000 only » peut alors apparaître en bas de fenêtre.

### `SELECT BANK` (`0`–`9`)

Les boutons `0` à `9` sélectionnent la banque active ; le patch courant est ensuite relu depuis le synthé (avec la même logique de confirmation `Unsaved edits` que pour un changement de patch).

### `UNLOCK`

Envoie une commande de déverrouillage de banque au synthé.  
Cela **ne modifie pas** la banque ni le patch affichés dans Matrix-Control. Un message indique que vous pouvez utiliser la façade du synthé pour une saisie à 3 chiffres ; le logiciel ne suit pas automatiquement ce geste effectué sur la machine.

---

## 11. Sauvegarder sur le synthé et sur l’ordinateur

Deux emplacements distincts :

| Où | Comment | Ce que cela conserve |
|----|---------|----------------------|
| **Mémoire du synthé** | `STORE` dans `INTERNAL PATCHES` | Le patch dans un emplacement de la machine (banque + numéro, ou numéro 0–99 sur Matrix-6/6R) |
| **Ordinateur** | Module `COMPUTER PATCHES` | Des fichiers `.syx` (un patch par fichier) dans un dossier que vous choisissez |

### Module `COMPUTER PATCHES`

**Zone `BROWSER`**

- Liste déroulante (`SELECT A PATCH`, noms de fichiers, ou `<EMPTY!>`)
- Boutons `<` / `>` pour parcourir les fichiers du dossier

**Zone `STORAGE`**

| Bouton | Rôle |
|--------|------|
| `OPEN` | Choisir / ouvrir le dossier de patches sur le disque |
| `SAVE AS` | Enregistrer le patch courant sous un nouveau nom `.syx` |
| `SAVE` | Enregistrer dans le fichier déjà associé |

Seul le format `.syx` est pris en charge. Il n’existe pas, dans cette version, d’import d’une banque entière d’un seul geste, ni de tags ou de favoris.

Après un parcours de dossier, le bas de fenêtre peut indiquer combien de fichiers sont valides ou invalides, ou confirmer un chargement / une sauvegarde (`Loaded…`, `Saved…`).

### Lorsque le nom du fichier et le nom interne du patch divergent

La boîte de dialogue `Patch name mismatch` peut proposer :

- `Internal` — conserver le nom contenu dans le fichier `.syx` 
- `Filename` — utiliser le nom du fichier 
- `Cancel` — annuler le chargement

Le comportement par défaut se règle dans `SETTINGS` (voir [section 14](#14-apparence-échelle-et-settings)).

> **Capture d’écran — À placer ici**  
> *Légende :* Module `COMPUTER PATCHES` avec `OPEN` / `SAVE AS` / `SAVE` et une liste de fichiers `.syx`.

---

## 12. Le Patch Mutator

Le module `PATCH MUTATOR` génère des variations aléatoires contrôlées du patch courant — un outil précieux pour explorer des timbres sans tout régler à la main.

### Réglages

| Contrôle | Rôle |
|----------|------|
| `AMOUNT` | Intensité de la variation (1–100 %, défaut 50) |
| `RANDOM` | Degré d’aléatoire (1–100 %, défaut 25) |
| Commutateurs `D1` `D2` `F/A` `F/T` `R/P` `E1` `E2` `E3` `L1` `L2` `MM` | Modules inclus dans la mutation (DCO 1/2, filtre/volume, FM/Track, rampes/portamento, enveloppes, LFO, matrice) |

Les curseurs `AMOUNT` et `RANDOM` couvrent la plage 1–100 %. Pour qu’une mutation s’exécute, **au moins un commutateur de module** doit être activé.

Le Mutator ne modifie pas le nom « officiel » du patch ; l’historique peut toutefois afficher des noms de session du type `M00`, `M00-R01`, etc.

### Actions

| Bouton | Rôle |
|--------|------|
| `MUTATE` | Crée une nouvelle variation de patch et l’envoie au synthé |
| `RETRY` | Effectue un autre tirage à partir de la même variation de départ |
| `HISTORY` + `<` / `>` | Parcourt l’historique des variations de la session |
| `C` (Compare) | Compare avec le patch de départ ; **verrouille** le reste de l’édition tant que Compare demeure actif (un message en bas de fenêtre le rappelle) |
| `D` | Supprime l’entrée sélectionnée |
| `F` | Vide l’historique |
| `E` | Exporte la session (dossier avec `Initial.syx` et sous-dossiers en `.syx`) |

L’historique ne survit pas à la fermeture du projet ou de l’application.

- Si l’historique est plein, la boîte `Mutation history full` propose de le compacter (`Defrag`) ou d’annuler (`Cancel`)
- Si vous changez de patch alors que des variations n’ont pas été exportées, la boîte `Unsaved mutations` propose `Export`, `Discard` ou `Cancel`
- Si un dossier d’export existe déjà, la boîte `Export folder exists` propose `Overwrite`, `Keep both` ou `Cancel`

> **Capture d’écran — À placer ici**  
> *Légende :* Module `PATCH MUTATOR` avec `AMOUNT`, `RANDOM`, commutateurs de modules et boutons `MUTATE` / `RETRY` / `HISTORY`.

---

## 13. Réglages globaux du synthé (MASTER EDIT)

La section `MASTER EDIT` regroupe les **paramètres globaux** du Matrix-1000 : des réglages qui concernent la machine entière et s’appliquent donc **à tous les patches**, et non à un seul son.

Elle n’est disponible que lorsqu’un **Matrix-1000** est détecté. Sur Matrix-6/6R, toute la colonne est grisée (message du type « Matrix-1000 only »).

Trois modules composent cette section — `MIDI`, `VIBRATO` et `MISC` — chacun muni d’un bouton `I` pour le réinitialiser. Avant d’appliquer cette réinitialisation, Matrix-Control affiche la boîte `Reset master module?` (`Reset` / `Cancel`).

En pratique, vous y trouverez notamment le canal MIDI, le vibrato global, l’accordage et la transposition maîtres, ainsi que des options de protection mémoire ou de verrouillage de banque. L’idée maîtresse : ce que vous réglez ici influence le comportement général du synthé, quel que soit le patch chargé.

> **Capture d’écran — À placer ici**  
> *Légende :* Section `MASTER EDIT` (modules `MIDI`, `VIBRATO`, `MISC`) sur Matrix-1000.

---

## 14. Apparence, échelle et Settings

### Menu du logo `MATRIX-CONTROL`

Un clic sur le logo (en haut à gauche) ouvre le menu suivant :

| Entrée | Contenu |
|--------|---------|
| `UI SCALE` | Taille de l’interface : 50 % à 200 % |
| `SKIN` | Apparence `BLACK` ou `CREAM` (petit clin d’œil aux versions Black et Cream du Matrix-1000) |
| `AUDIO/MIDI...` | Uniquement en Standalone — choix de l’interface audio et des périphériques MIDI |
| `SETTINGS...` | Préférences |
| `ABOUT...` | Version, crédits, liens |

La touche **Échap** ferme en général les fenêtres `SETTINGS` et `ABOUT`, ainsi que certains dialogues.

### Fenêtre `SETTINGS`

| Réglage | Disponible ? | Détail |
|---------|--------------|--------|
| `HARDWARE LATENCY` | Oui, **en mode plugin seulement** | 0 à 200 ms. Indique au DAW le retard du chemin matériel (synthé + câbles + interface audio), afin de mieux caler l’audio. |
| Politique de noms | Oui | `PREFER INTERNAL NAME` / `PREFER FILENAME` / `ASK ONCE PER LOAD` |
| `UNSAVED` | Oui | `WARN ALWAYS` / `NEVER WARN` |
| `MASTER OPERATIONS` | Non — *Coming soon...* | Pas encore proposé |
| `DEFRAG` | Non — *Coming soon...* dans Settings | Un compactage d’historique existe déjà via le Mutator lorsque l’historique est plein |
| `LOGGING` | Non — *Coming soon...* | Pas encore proposé |

L’apparence et la taille ne se règlent **pas** dans Settings : elles se trouvent dans le menu du logo `MATRIX-CONTROL`.

> **Capture d’écran — À placer ici**  
> *Légende :* Menu du logo (`UI SCALE` / `SKIN`) et fenêtre `SETTINGS`.

---

## 15. Mode autonome (Standalone) et mode plugin

| Fonction | Plugin dans le DAW | Standalone |
|----------|--------------------|------------|
| `KEYBOARD FROM` | Fixé à `HOST` | Choix d’un port MIDI clavier |
| `AUDIO FROM` / `INPUT GAIN` | Masqués | Visibles (retour audio du synthé dans l’application) |
| `AUDIO/MIDI...` | Non | Oui (menu du logo `MATRIX-CONTROL`) |
| `HARDWARE LATENCY` (Settings) | Visible | Masqué |
| Notes jouées | Depuis le DAW / le clavier routé vers la piste | Depuis le port Keyboard From |

Dans les deux cas, le **son** demeure celui du synthé. Matrix-Control pilote les réglages.

### Ce qui est mémorisé d’une session à l’autre

En général **conservé** : ports MIDI, apparence, taille d’interface, latence (plugin), dossier des patches ordinateur, réglages du Mutator, préférences Settings.

En général **non** conservé tel quel : le contenu du patch en cours d’édition (le logiciel privilégie une resynchronisation avec le synthé lorsque les ports sont reconnectés), l’historique du Mutator, l’état Compare.

---

## 16. Dépannage rapide

| Symptôme | Que faire |
|----------|-----------|
| `No device` / éditeur verrouillé | Vérifier le sens des câbles ; les ports `FROM` / `TO` ; que le synthé est allumé ; éventuellement l’éteindre puis le rallumer ; sous Windows, s’assurer qu’aucune autre application n’occupe le port |
| Les contrôles bougent mais le patch ne change pas | Vérifier `MIDI TO` ; le canal MIDI (`MASTER EDIT` sur Matrix-1000) ; `MEMORY PROTECT` / `BANK LOCK` |
| Changement de patch : l’écran ne se met pas à jour | Vérifier `EDITOR MIDI FROM` (le synthé doit pouvoir répondre) |
| `STORE` ou `PASTE` grisés | Banque 2–9 sur Matrix-1000 — passer en banque 0 ou 1 |
| `MASTER EDIT` / `BANK UTILITY` grisés | Vous êtes sur Matrix-6/6R, ou aucun appareil n’est détecté |
| Compare empêche toute édition | Cliquer à nouveau sur `C` dans le Mutator |
| Plugin invisible dans le DAW | Rescanner les plugins ; vérifier le format (AU/VST3) et le dossier indiqué avec le téléchargement |
| Audio en retard dans le DAW | Ajuster `HARDWARE LATENCY` dans Settings (plugin) ; et la taille de buffer du DAW / de l’interface audio |

---

## Lexique

Définitions courtes. Les libellés entre guillemets sont ceux de l’écran (anglais).

| Terme | Définition |
|-------|------------|
| **Afficheur** | Zone graphique ou panneau d’affichage (enveloppes et Track Generator interactifs ; `PATCH NAME` en lecture seule). |
| **AU** | Format de plugin sur macOS (Audio Unit). |
| **Banque (Bank)** | Sur le Matrix-1000 : regroupement de 100 patches. Il y a 10 banques (0–9). Le Matrix-6/6R n’a pas de banques. |
| **Bus de modulation** | Une des dix voies de la section `MATRIX MODULATION` (source, intensité, destination). |
| **DAW** | Digital Audio Workstation — un séquenceur / studio logiciel (Live, Logic, Reaper…). On dit *un* DAW. |
| **DCO** | Oscillateur : source qui produit la forme d’onde de base du patch. |
| **ENV** | Envelope — enveloppe (attaque, decay, sustain, release…) qui façonne l’évolution du patch dans le temps. |
| **FM** | Modulation de fréquence du filtre par l’oscillateur, pour des timbres plus riches ou métalliques. |
| **LFO** | Oscillateur lent : fait vibrer, trembler ou balayer un paramètre. |
| **MASTER** | Réglages globaux de la machine (canal MIDI, vibrato, accordage…), distincts d’un patch. |
| **Matrice de modulation** | Centre nerveux du synthé : tableau de « câbles virtuels » reliant sources et destinations. |
| **MIDI** | Protocole qui permet aux instruments et à l’ordinateur de communiquer (notes, commandes, messages détaillés…). |
| **MIDI FROM / MIDI TO** | Dans Matrix-Control : ce qui arrive du synthé / ce qui part vers le synthé. |
| **Module** | Bloc fonctionnel de l’interface (`DCO 1`, `BANK UTILITY`, `PATCH MUTATOR`, etc.). |
| **Modulation** | Faire évoluer automatiquement un paramètre (filtre, volume, hauteur…) à partir d’une source. |
| **Mutator** | Outil qui crée des variations aléatoires contrôlées d’un patch. |
| **Patch** | Ensemble des réglages qui définissent un timbre mémorisé (vocabulaire Oberheim). |
| **Plugin** | Logiciel qui s’ouvre *dans* un DAW. |
| **Portamento** | Glissando de hauteur entre deux notes. |
| **Presse-papiers** | Mémoire temporaire Copy / Paste *interne* à Matrix-Control (modules ou patches). |
| **Ramp** | Montée progressive utilisable comme source de modulation. |
| **RAM** | Mémoire en lecture/écriture (*Random Access Memory*) dans laquelle on peut stocker ses propres données puis les relire. |
| **ROM** | Mémoire en lecture seule (*Read-Only Memory*) , dans laquelle on ne peut pas stocker ses propres données. Sur Matrix-1000, banques 2–9 : pas de `STORE` / `PASTE` / `INIT` depuis l’éditeur. |
| **Section** | Grand ensemble de l’interface (`PATCH EDIT`, `MATRIX MODULATION`, `PATCH MANAGER`, `MASTER EDIT`). |
| **Standalone** | Application autonome, sans DAW. |
| **STORE** | Enregistrer le patch courant dans la mémoire du synthé. |
| **SysEx** | Messages MIDI détaillés propres à une marque / un modèle, utilisés pour éditer le synthé en profondeur. |
| **`.syx`** | Fichier qui contient un patch au format SysEx. |
| **Track Generator** | Courbe qui transforme la position sur le clavier en valeur de modulation (afficheur interactif). |
| **VCA** | Étage de volume du patch. |
| **VCF** | Filtre qui sculpte le timbre dans le domaine fréquentiel (grave / aigu, résonance). |
| **VST3** | Format de plugin très répandu (Windows, macOS, Linux…). |
| **Zone** | Sous-ensemble d’un module (`BROWSER`, `MEMORY`, `STORAGE`, etc.). |
| **Unlock** | Déverrouille la banque côté Matrix-1000 pour certaines saisies sur la façade. |

---

*Si quelque chose à l’écran ne correspond pas à ce manuel, notez-le : ce sera le signal pour corriger le document et/ou le logiciel.*
