#  Matrix-Control — Manuel utilisateur

**Version du produit décrite :** 0.1.1-alpha

> **Lexique.** Les termes techniques et acronymes utilisés dans ce manuel utilisateur sont expliqués en fin de document, dans le [lexique](#lexique). Vous pouvez y revenir à tout moment.

---

## Table des matières

1. [À quoi sert Matrix-Control ?](#1-à-quoi-sert-matrix-control)
2. [Ce qu’il vous faut](#2-ce-quil-vous-faut)
3. [Le Matrix-1000 et le Matrix-6 / 6R en deux minutes](#3-le-matrix-1000-et-le-matrix-6--6r-en-deux-minutes)
4. [Installer et ouvrir le logiciel](#4-installer-et-ouvrir-le-logiciel)
5. [Brancher le synthé et établir le dialogue MIDI](#5-brancher-le-synthé-et-établir-le-dialogue-midi)
6. [Premier contact : l’écran principal](#6-premier-contact-lécran-principal)
7. [Éditer un son (PATCH EDIT)](#7-éditer-un-son-patch-edit)
8. [La matrice de modulation](#8-la-matrice-de-modulation)
9. [Choisir un patch dans le synthé (INTERNAL PATCHES)](#9-choisir-un-patch-dans-le-synthé-internal-patches)
10. [Banques du Matrix-1000 (BANK UTILITY)](#10-banques-du-matrix-1000-bank-utility)
11. [Sauvegarder sur le synthé et sur l’ordinateur](#11-sauvegarder-sur-le-synthé-et-sur-lordinateur)
12. [Le Patch Mutator](#12-le-patch-mutator)
13. [Réglages globaux du synthé (MASTER EDIT)](#13-réglages-globaux-du-synthé-master-edit)
14. [Apparence, échelle et Settings](#14-apparence-échelle-et-settings)
15. [Mode autonome (Standalone) et mode plugin](#15-mode-autonome-standalone-et-mode-plugin)
16. [Messages, dialogues et situations courantes](#16-messages-dialogues-et-situations-courantes)
17. [Ce qui n’est pas disponible dans cette version](#17-ce-qui-nest-pas-disponible-dans-cette-version)
18. [Dépannage rapide](#18-dépannage-rapide)
19. [Lexique](#lexique)

---

## 1. À quoi sert Matrix-Control ?

Matrix-Control est un **éditeur MIDI** pour les synthétiseurs Oberheim **Matrix-1000**, **Matrix-6** et **Matrix-6R**.

Il ne produit pas le son lui-même : le son est toujours généré par votre synthètiseur. Le logiciel agit comme une télécommande sophistiquée, il envoie et reçoit des messages MIDI (surtout des messages SysEx) pour que vous puissiez **voir et modifier** les paramètres du synthé depuis l’écran — avec des boutons, des curseurs et des graphiques — au lieu de tout faire depuis la façade du synthé.

Vous pouvez l’utiliser :

- **dans un séquenceur / un DAW** (Ableton Live, Logic Pro, Cubase, Reaper, etc.), comme instrument virtuel qui dialogue avec le synthé ;
- **en application autonome** (Standalone), sans DAW.

Éditeur, fabricant et contacts (fenêtre **ABOUT...**) :

- Produit : **MATRIX-CONTROL**
- Organisation : Ten Square Software
- Auteur : Guillaume DUPONT

> **Capture d’écran — À placer ici**  
> *Légende :* Fenêtre principale de Matrix-Control avec un Matrix-1000 détecté (en-tête MIDI, colonnes PATCH EDIT / MATRIX MODULATION / PATCH MANAGER / MASTER EDIT, pied de page).

---

## 2. Ce qu’il vous faut

### Matériel

- Un **Matrix-1000**, un **Matrix-6** ou un **Matrix-6R**, sous tension.
- Une interface MIDI (USB-MIDI, interface audio avec ports MIDI, ou câbles MIDI DIN selon votre installation).
- **Deux sens de communication** entre l’ordinateur et le synthé :
  - de l’ordinateur **vers** le synthé (pour envoyer les réglages) ;
  - du synthé **vers** l’ordinateur (pour que Matrix-Control lise le son courant et reconnaisse l’appareil).

Sans les deux sens, l’éditeur ne pourra pas piloter le synthé correctement.

### Ordinateur

| Plateforme | Formats disponibles aujourd’hui |
|------------|----------------------------------|
| **macOS** | AU, VST3, Standalone |
| **Windows** | VST3, Standalone |
| **Linux** | VST3, Standalone |

### Connaissances utiles (sans être expert)

- Savoir brancher MIDI IN / MIDI OUT (ou USB-MIDI).
- Savoir ouvrir un instrument dans votre DAW, **ou** lancer une application Standalone.
- Accepter que l’interface du logiciel soit en **anglais** : ce manuel traduit et explique chaque zone.

---

## 3. Le Matrix-1000 et le Matrix-6 / 6R en deux minutes

Ces machines partagent le même « moteur » de synthèse (deux oscillateurs, filtre, enveloppes, LFO, matrice de modulation, etc.). Ce qui change surtout pour vous dans Matrix-Control :

| | **Matrix-1000** | **Matrix-6 / 6R** |
|---|-----------------|-------------------|
| Mémoire des sons | **10 banques** (0 à 9), **100 patches** par banque (0 à 99) | **100 patches** seulement (pas de banques) |
| Zone **BANK UTILITY** | Active | Grisée (indisponible) |
| Zone **MASTER EDIT** | Active (réglages globaux MIDI, vibrato, accordage…) | Grisée (indisponible) |
| Façade | Très peu de boutons : l’éditeur logiciel est particulièrement utile | Clavier (6) ou rack (6R) avec plus de commandes directes |

**Banques ROM du Matrix-1000.** Sur le Matrix-1000, les banques **2 à 9** sont traitées comme de la mémoire en lecture seule (ROM) pour certaines actions : **INIT**, **PASTE** et **STORE** y sont bloqués. Les banques **0** et **1** sont celles où vous pouvez stocker vos modifications dans la machine.

**Détection.** Matrix-Control envoie une requête d’identité (Device Inquiry) dès que les ports MIDI sont correctement ouverts. Le pied de page affiche alors quelque chose comme le type d’appareil détecté, ou **No device** s’il n’y a pas encore de dialogue.

> **Note honnête.** Le code distingue les types Matrix-1000, Matrix-6 et Matrix-6R, mais la détection automatique ne sépare pas encore clairement le 6 du 6R : un Matrix-6R peut apparaître comme Matrix-6. Pour l’usage quotidien (éditer un patch), cela ne change presque rien.

---

## 4. Installer et ouvrir le logiciel

### Où trouver le logiciel

Selon votre installation (build local, release GitHub, copie manuelle) :

- **Plugin AU** (macOS) — dossier des composants Audio Units du système.
- **Plugin VST3** — dossier VST3 du système ou de l’utilisateur.
- **Application Standalone** — l’exécutable Matrix-Control.

Les chemins exacts dépendent de la façon dont le projet a été compilé ou installé chez vous. Si le plugin n’apparaît pas dans la DAW, rescannez la liste des plugins et vérifiez le dossier d’installation.

### Dans une DAW

1. Créez une piste **instrument**.
2. Chargez **Matrix-Control** comme instrument.
3. Assurez-vous que la piste peut envoyer et recevoir du MIDI vers/depuis votre interface (selon votre DAW).
4. Le son audio du Matrix doit être branché séparément dans la DAW (entrée audio de l’interface, ou retour de table de mixage) — Matrix-Control n’est pas un synthé logiciel qui génère le son.

### En Standalone

1. Lancez l’application **Matrix-Control**.
2. Au besoin, ouvrez **AUDIO/MIDI...** depuis le menu du logo (voir [section 15](#15-mode-autonome-standalone-et-mode-plugin)) pour choisir la carte audio et les périphériques MIDI système.

> **Capture d’écran — À placer ici**  
> *Légende :* Matrix-Control chargé comme instrument dans une DAW (exemple de piste).

---

## 5. Brancher le synthé et établir le dialogue MIDI

C’est l’étape la plus importante. Tant que le synthé n’est pas reconnu, une grande partie de l’éditeur reste verrouillée.

### Câblage type

1. **MIDI OUT de l’interface / de l’ordinateur** → **MIDI IN du Matrix**.
2. **MIDI OUT du Matrix** → **MIDI IN de l’interface / de l’ordinateur**.
3. Allumez le Matrix.
4. Vérifiez que le canal MIDI du synthé est cohérent avec ce que vous utiliserez plus tard (sur Matrix-1000, le canal se règle aussi dans **MASTER EDIT → MIDI → CHANNEL** une fois connecté).

### Choisir les ports dans l’en-tête

En haut de la fenêtre :

| Libellé à l’écran | Rôle |
|-------------------|------|
| **EDITOR MIDI FROM :** | Entrée MIDI **depuis le synthé** vers l’éditeur (réponses, dumps de patch). Choisissez le port qui reçoit le MIDI OUT du Matrix. Si rien n’est choisi : **NO INPUT**. |
| **MIDI TO :** | Sortie MIDI **vers le synthé**. Choisissez le port qui envoie vers le MIDI IN du Matrix. Si rien n’est choisi : **NO OUTPUT**. |
| **KEYBOARD FROM :** | D’où viennent les notes jouées. En **plugin** : fixé à **HOST** (la DAW) et non modifiable. En **Standalone** : vous choisissez un clavier / port MIDI. |

Des voyants d’activité à côté des listes clignotent quand du MIDI circule.

Il n’y a **pas** de bouton « AutoConnect ». Le logiciel :

- mémorise les ports choisis ;
- tente de les rouvrir au prochain lancement ;
- envoie automatiquement une **Device Inquiry** quand FROM et TO sont ouverts correctement.

### Verrouillage tant que le synthé n’est pas détecté

Si aucun appareil n’est reconnu, le pied de page affiche un message du type :

> *No synth detected — check MIDI cables, MIDI From / MIDI To, and power-cycle the Matrix.*

Les sections d’édition restent verrouillées jusqu’à détection réussie. C’est normal : Matrix-Control refuse d’envoyer des changements à l’aveugle.

### Windows : port déjà utilisé

Sous Windows, un port MIDI est souvent exclusif. Si un autre logiciel (ou la DAW) occupe déjà le port, Matrix-Control peut indiquer qu’il n’a pas pu l’ouvrir. Solutions typiques : fermer l’autre application, ou utiliser un port MIDI virtuel (par exemple loopMIDI) pour router le flux. Un document d’aide technique existe dans le dépôt pour ce cas (`Documentation/Development/windows-midi-multi-client.md`) — plutôt pour le dépannage avancé que pour le premier branchement.

> **Capture d’écran — À placer ici**  
> *Légende :* En-tête avec EDITOR MIDI FROM et MIDI TO correctement sélectionnés, voyants actifs, et identité du synthé visible en bas à droite.

---

## 6. Premier contact : l’écran principal

L’interface est organisée en trois bandes horizontales.

```
┌─ En-tête (Header) ──────────────────────────────────────────────┐
│ Logo · MIDI FROM / TO · KEYBOARD FROM · (audio Standalone)      │
├─ Corps (Body) ──────────────────────────────────────────────────┤
│  PATCH EDIT  │  MATRIX MODULATION + PATCH MANAGER  │ MASTER EDIT │
├─ Pied de page (Footer) ─────────────────────────────────────────┤
│  Messages (ℹ ✓ ⚠ ✗)                          Identité appareil │
└─────────────────────────────────────────────────────────────────┘
```

### En-tête

- Logo Matrix-Control (clic → menu : échelle d’interface, peau visuelle, Settings, About, et Audio/MIDI en Standalone).
- Sélection des ports MIDI (section précédente).
- En Standalone seulement : **AUDIO FROM :** et **INPUT GAIN :** pour écouter le retour audio du synthé dans l’appli.

### Corps — trois colonnes

| Colonne | Contenu |
|---------|---------|
| **Gauche — PATCH EDIT** | Tous les modules du son : DCO, filtre, enveloppes, LFO, etc. |
| **Centre** | **MATRIX MODULATION** (10 bus de modulation) + **PATCH MANAGER** (banques, patches internes, fichiers .syx, mutateur) |
| **Droite — MASTER EDIT** | Réglages globaux du Matrix-1000 (grisé sur Matrix-6 / 6R) |

### Pied de page

- **À gauche :** messages d’information, de succès, d’avertissement ou d’erreur (icônes ℹ ✓ ⚠ ✗).
- **À droite :** identité détectée (par ex. Matrix-1000 et version firmware) ou **No device**.

> **Capture d’écran — À placer ici**  
> *Légende :* Vue d’ensemble annotée des trois colonnes et du pied de page.

---

## 7. Éditer un son (PATCH EDIT)

Une fois le synthé détecté et un patch chargé (voir [section 9](#9-choisir-un-patch-dans-le-synthé-internal-patches)), vous travaillez dans **PATCH EDIT**.

### Principe

Chaque mouvement de contrôle envoie le changement au Matrix. Vous entendez le résultat sur le matériel (et éventuellement via l’entrée audio en Standalone).

**Il n’y a pas d’annulation / rétablissement (Undo / Redo)** dans cette version. Travaillez avec **STORE**, **SAVE** / **SAVE AS**, ou le **Compare** du Mutator si vous explorez des mutations.

### Boutons I / C / P sur les modules

Sur plusieurs modules vous verrez :

| Bouton | Signification | Action |
|--------|---------------|--------|
| **I** | Init | Remet le module à ses valeurs d’initialisation |
| **C** | Copy | Copie le module dans le presse-papiers interne |
| **P** | Paste | Colle le module depuis le presse-papiers |

Si le contenu collé ne correspond pas au module cible, un message indique que le presse-papiers n’est pas compatible.

### Nom du patch (PATCH NAME)

Une zone affiche le nom du patch (8 caractères, style Matrix).  
**Dans cette version, ce nom est affiché mais n’est pas éditable à la souris / au clavier dans l’interface.** Il est mis à jour quand un patch arrive du synthé, d’un fichier, ou du Mutator.

### Les modules, un par un

#### DCO 1 et DCO 2 (oscillateurs)

Les DCO sont les « voix » qui génèrent la forme d’onde de base.

**DCO 1 :** FREQUENCY, FREQ \< LFO 1, PULSE WIDTH, PW \< LFO 2, WAVE SHAPE, SYNC, WAVE SELECT, LEVERS, KEYBD/PORTA, KEY CLICK — avec **I / C / P**.

**DCO 2 :** comme DCO 1, plus **DETUNE**, sans SYNC — avec **I / C / P**.

Valeurs typiques de listes : SYNC OFF / SOFT / MEDIUM / HARD ; WAVE SELECT OFF / PULSE / WAVE / BOTH / NOISE ; LEVERS OFF / L1/BEND / L2/VIB / BOTH ; KEYBD/PORTA OFF / KEYBD / PORTA.

#### VCF/VCA (filtre et volume)

BALANCE, FREQUENCY, FREQ \< ENV 1, FREQ \< PRESSURE, RESONANCE, VCA 1 VOLUME, VCA 1 \< VELOCITY, VCA 2 \< ENV 2, LEVERS, KEYBD/PORTA — **I** seulement.

#### FM/TRACK

Modulation FM du filtre et générateur de tracking (courbe selon la note) :

VCF FM AMOUNT, FM \< ENV 3, FM \< PRESSURE, TRACK POINT 1 à 5, TRACK INPUT — **I** seulement.

L’affichage **Track** est interactif : vous pouvez déplacer les points à la souris.

#### RAMP/PORTAMENTO

RAMP 1 RATE, RAMP 2 RATE, PORTAMENTO RATE, PORTA \< VELOCITY, RAMP 1/2 TRIGGER, PORTA MODE, LEGATO PORTA, KEYBOARD MODE — **I** seulement.

Si LEGATO PORTA et KEYBOARD MODE **UNISON** sont incompatibles, un message en pied de page l’indique.

#### ENV 1, ENV 2, ENV 3 (enveloppes)

DELAY, ATTACK, DECAY, SUSTAIN, RELEASE, AMPLITUDE, AMP \< VELOCITY, TRIGGER MODE, ENVELOPE MODE, LFO 1 TRIGGER — **I / C / P**.

Les enveloppes ont un affichage graphique que vous pouvez manipuler (forme DADR-style).

#### LFO 1 et LFO 2

**LFO 1 :** SPEED, SPEED \< PRESSURE, RETRIGGER POINT, AMPLITUDE, AMP \< RAMP 1, WAVEFORM, TRIGGER MODE, LAG, SAMPLE INPUT — **I / C / P**.

**LFO 2 :** SPEED, SPEED \< KEYBD, RETRIGGER POINT, AMPLITUDE, AMP \< RAMP 2, WAVEFORM, TRIGGER MODE, LAG, SAMPLE INPUT — **I / C / P**.

Formes d’onde : TRIANGLE, UPSAW, DNSAW, SQUARE, RANDOM, NOISE, SAMPLED.

> **Capture d’écran — À placer ici**  
> *Légende :* Colonne PATCH EDIT avec DCO, VCF/VCA et une enveloppe visible.

---

## 8. La matrice de modulation

Section **MATRIX MODULATION** : dix bus numérotés **0 à 9**.

Chaque bus a :

- **SOURCE** — d’où vient la modulation (ENV 1–3, LFO 1–2, VIBRATO, RAMP, KEYBD, VELOCITY, PRESSURE, pedales, leviers, etc., ou NONE) ;
- **AMOUNT** — intensité ;
- **DESTINATION** — ce qui est modulé (fréquences DCO, largeur d’impulsion, VCF, VCA, paramètres d’enveloppe, LFO, portamento, etc., ou NONE).

Vous pouvez :

- initialiser un bus (**I** par bus) ou toute la section ;
- copier / coller la section (**C** / **P**) ;
- **réordonner** les bus en faisant glisser le numéro **#** (les indices restent 0–9 ; ce sont les contenus qui bougent).

C’est le cœur du caractère « Matrix » : relier n’importe quelle source à presque n’importe quelle destination.

> **Capture d’écran — À placer ici**  
> *Légende :* Les dix bus MATRIX MODULATION avec SOURCE / AMOUNT / DESTINATION.

---

## 9. Choisir un patch dans le synthé (INTERNAL PATCHES)

Dans **PATCH MANAGER → INTERNAL PATCHES** :

### Naviguer

- Zone **BROWSER** : boutons **\<** et **\>** pour le patch précédent / suivant.
- Numéro de **patch** (0–99) : affichage + saisie possible (double-clic selon le comportement du NumberBox).
- Numéro de **banque** (Matrix-1000) : affiché ; la sélection de banque se fait surtout via **BANK UTILITY** (boutons 0–9). Sur Matrix-6 / 6R, la notion de banque est absente.

Quand vous changez de banque ou de patch, Matrix-Control :

1. peut vous demander confirmation si des modifications n’ont pas été stockées / sauvées (selon vos réglages) ;
2. envoie le changement de programme au synthé ;
3. **demande le patch** au synthé et charge l’éditeur avec ce contenu ;
4. **n’envoie pas** automatiquement le patch en retour juste après cette lecture.

### Mémoire (MEMORY)

| Bouton | Rôle |
|--------|------|
| **INIT** | Initialise le patch (valeurs de départ) |
| **COPY** | Copie le patch courant |
| **PASTE** | Colle un patch précédemment copié |
| **STORE** | Écrit le patch dans l’emplacement courant de la **mémoire du synthé** |

Sur les banques ROM **2–9** du Matrix-1000 : INIT, PASTE et STORE sont indisponibles, avec un message explicite en pied de page.

> **Capture d’écran — À placer ici**  
> *Légende :* INTERNAL PATCHES avec BROWSER et MEMORY (INIT / COPY / PASTE / STORE).

---

## 10. Banques du Matrix-1000 (BANK UTILITY)

Réservé au **Matrix-1000**. Sur Matrix-6 / 6R, la section est grisée avec un message du type : les banques n’existent pas sur ces modèles.

### SELECT BANK (0–9)

Les boutons **0** à **9** sélectionnent la banque active, puis le flux de chargement du patch courant s’applique.

### UNLOCK

Envoie une commande de déverrouillage de banque au synthé.  
Cela **ne change pas** la banque / le patch affichés dans le plugin. Un message indique que vous pouvez utiliser la façade du Matrix pour une saisie à 3 chiffres ; le plugin ne suit pas automatiquement ce geste de façade.

---

## 11. Sauvegarder sur le synthé et sur l’ordinateur

Deux « mondes » distincts :

| Où | Comment | Ce que ça conserve |
|----|---------|-------------------|
| **Mémoire du Matrix** | **STORE** dans INTERNAL PATCHES | Le son dans un emplacement banque/patch (ou slot 0–99 sur 6/6R), dans la machine |
| **Disque de l’ordinateur** | **COMPUTER PATCHES** | Des fichiers **`.syx`** (un patch par fichier) dans un dossier que vous choisissez |

### COMPUTER PATCHES

Section **PATCH MANAGER → COMPUTER PATCHES**.

**BROWSER**

- Liste déroulante (**SELECT A PATCH**, ou noms de fichiers, ou **\<EMPTY!\>**).
- **\<** / **\>** pour parcourir les fichiers valides du dossier.

**STORAGE**

| Bouton | Rôle |
|--------|------|
| **OPEN** | Choisir / ouvrir le dossier de patches sur le disque |
| **SAVE AS** | Enregistrer le patch courant sous un nouveau nom `.syx` |
| **SAVE** | Enregistrer dans le fichier déjà associé |

Le format pris en charge est **uniquement `.syx`** (SysEx). Pas d’import de banque entière multi-patches, pas de tags, pas de favoris dans cette version.

Après un scan de dossier, le pied de page peut indiquer combien de fichiers sont valides ou invalides, ou confirmer un chargement / une sauvegarde.

### Conflit de nom (fichier vs nom interne)

Si le nom contenu dans le fichier et le nom du fichier ne correspondent pas, une boîte de dialogue peut proposer :

- **Internal** — garder le nom stocké dans le SysEx ;
- **Filename** — utiliser le nom dérivé du fichier ;
- **Cancel** — annuler.

La politique par défaut se règle dans **SETTINGS** (voir [section 14](#14-apparence-échelle-et-settings)).

### Modifications non sauvegardées

Si vous quittez le patch courant alors qu’il y a des edits non stockés sur le synthé et non sauvés en `.syx`, une boîte **Unsaved edits** peut apparaître (**Cancel** / **Continue**), selon la politique **UNSAVED** des Settings.

> **Capture d’écran — À placer ici**  
> *Légende :* COMPUTER PATCHES avec OPEN / SAVE AS / SAVE et une liste de fichiers `.syx`.

---

## 12. Le Patch Mutator

Le **PATCH MUTATOR** crée des variations aléatoires contrôlées du patch courant — idéal pour explorer sans tout programmer à la main.

### Réglages

| Contrôle | Rôle |
|----------|------|
| **AMOUNT** | Force de la mutation (1–100 %, défaut 50) |
| **RANDOM** | Probabilité / dispersion (1–100 %, défaut 25) |
| Cases **D1 D2 F/A F/T R/P E1 E2 E3 L1 L2 MM** | Modules concernés (DCO1, DCO2, VCF/VCA, FM/Track, Ramp/Porta, Env 1–3, LFO 1–2, Matrix Mod) |

Les deux curseurs doivent être au-dessus de 0, et au moins un module activé, sinon la mutation ne part pas.

Le Mutator **ne modifie pas les octets du nom** du patch dans l’algorithme ; en pratique, l’historique peut toutefois afficher des noms de session du type `M00`, `M00-R01`, etc.

### Actions

| Bouton | Rôle |
|--------|------|
| **MUTATE** | Nouvelle mutation « racine » ; envoie le résultat au synthé |
| **RETRY** | Nouveau tirage sous la même racine |
| **HISTORY** + **\<** **\>** | Naviguer dans l’historique de session |
| **C** (Compare) | Bascule entre la mutation et l’instantané initial ; **verrouille** le reste de l’édition tant que Compare est actif |
| **D** | Supprime l’entrée sélectionnée |
| **F** | Vide (flush) l’historique |
| **E** | Exporte la session (dossier avec `Initial.syx` et sous-dossiers `Mxx/` en `.syx`) |

L’historique est **limité à la session** : il n’est pas restauré quand vous rouvrez un projet DAW. Capacité élevée (jusqu’à de nombreuses racines et retries) ; s’il est plein, une boîte propose un **Defrag** pour compacter.

Si vous changez de patch alors que des mutations non exportées existent, une boîte peut proposer **Export**, **Discard** ou **Cancel**.

> **Capture d’écran — À placer ici**  
> *Légende :* PATCH MUTATOR avec AMOUNT, RANDOM, cases de modules et boutons MUTATE / RETRY / HISTORY.

---

## 13. Réglages globaux du synthé (MASTER EDIT)

Disponible uniquement quand un **Matrix-1000** est détecté. Sur Matrix-6 / 6R, toute la colonne est grisée.

Trois modules, chacun avec un **I** (réinitialisation du module, avec confirmation) :

### MIDI

CHANNEL (OMNI, CHANNEL 1–16, MONO G1–G9), MIDI ECHO, CONTROLLERS, PATCH CHANGES, PEDAL 1/2 SELECT, LEVER 2/3 SELECT.

### VIBRATO

SPEED, SPEED MOD SOURCE, SPEED MOD AMOUNT, WAVEFORM, AMPLITUDE, AMP MOD SOURCE, AMP MOD AMOUNT.

### MISC

MASTER TUNE, MASTER TRANSPOSE, BEND RANGE (+/-), UNISON, VOLUME INVERT, BANK LOCK, MEMORY PROTECT.

Ces réglages concernent le comportement global de la machine, pas un seul patch.

> **Capture d’écran — À placer ici**  
> *Légende :* Colonne MASTER EDIT (modules MIDI, VIBRATO, MISC) sur Matrix-1000.

---

## 14. Apparence, échelle et Settings

### Menu du logo

Clic sur le logo (en-tête) :

| Section / entrée | Contenu |
|------------------|---------|
| **UI SCALE** | 50 % … 200 % (pas de 25 %) |
| **SKIN** | **BLACK** ou **CREAM** |
| **AUDIO/MIDI...** | Uniquement en Standalone — dialogue JUCE des périphériques |
| **SETTINGS...** | Fenêtre des préférences |
| **ABOUT...** | Version, crédits, liens |

### Fenêtre SETTINGS

| Réglage | Disponible ? | Détail |
|---------|--------------|--------|
| **HARDWARE LATENCY** | Oui, **en mode plugin seulement** | 0 à 200 ms (pas de 0,1 ms). Indique à la DAW une latence correspondant au chemin matériel. |
| Politique de noms | Oui | PREFER INTERNAL NAME / PREFER FILENAME / ASK ONCE PER LOAD |
| **UNSAVED** | Oui | WARN ALWAYS / NEVER WARN |
| **MASTER OPERATIONS** | Non — *Coming soon...* | Opérations fichier master (non disponibles) |
| **DEFRAG** | Non — *Coming soon...* dans Settings (un Defrag existe via la boîte du Mutator quand l’historique est plein) |
| **LOGGING** | Non — *Coming soon...* | |

La peau et l’échelle ne sont **pas** dans Settings : elles sont dans le menu du logo.

> **Capture d’écran — À placer ici**  
> *Légende :* Menu du logo (UI SCALE / SKIN) et fenêtre SETTINGS.

---

## 15. Mode autonome (Standalone) et mode plugin

| Fonction | Plugin dans la DAW | Standalone |
|----------|--------------------|------------|
| **KEYBOARD FROM** | Fixé à **HOST** | Choix d’un port MIDI clavier |
| **AUDIO FROM** / **INPUT GAIN** | Masqués | Visibles (retour audio du synthé dans l’appli) |
| **AUDIO/MIDI...** | Non | Oui (menu logo) |
| **HARDWARE LATENCY** (Settings) | Visible | Masqué |
| Notes jouées | Depuis la DAW / le clavier routé vers la piste | Depuis le port Keyboard From |

Dans les deux cas, le **son** reste celui du Matrix branché en audio. Matrix-Control orchestre les paramètres.

### Ce qui est mémorisé d’une session à l’autre

En général : ports MIDI choisis, peau, échelle, latence matérielle (plugin), dossier Computer Patches, réglages du Mutator (amount, random, modules), politiques Settings.

En général **non** restaurés tels quels : le contenu du patch édité (si les ports MIDI étaient connectés, le logiciel privilégie une resynchronisation depuis le matériel), l’historique du Mutator, l’état Compare.

---

## 16. Messages, dialogues et situations courantes

### Pied de page (exemples)

- Synth non détecté → vérifier câbles et ports, éventuellement éteindre/rallumer le Matrix.
- MASTER EDIT ou BANK UTILITY utilisés sur un mauvais modèle → message « Matrix-1000 only ».
- ROM banks 2–9 → INIT / PASTE / STORE indisponibles.
- Compare Mutator actif → édition verrouillée jusqu’à un second clic sur **C**.
- Échec de lecture du patch depuis le synthé → vérifier que **EDITOR MIDI FROM** est bien branché sur le MIDI OUT du Matrix.
- Scan Computer Patches → « N valid, M invalid », « Saved… », « Loaded… ».

### Dialogues

| Titre | Quand | Choix typiques |
|-------|--------|----------------|
| **Unsaved edits** | Changement de patch avec edits non stockés / non sauvés | Cancel / Continue |
| **Patch name mismatch** | Nom interne ≠ nom de fichier | Internal / Filename / Cancel |
| **Reset master module?** | Init d’un module MASTER | Reset / Cancel |
| **Mutation history full** | Historique Mutator saturé | Defrag / Cancel |
| **Unsaved mutations** | Quitter alors que des mutations ne sont pas exportées | Export / Discard / Cancel |
| **Export folder exists** | Conflit de dossier d’export Mutator | Overwrite / Keep both / Cancel |

La touche **Échap** ferme en général Settings, About et certains dialogues.

---

## 17. Ce qui n’est pas disponible dans cette version

Pour vos tests UAT et pour éviter les fausses attentes, voici ce qui **n’est pas** (encore) offert à l’utilisateur final, même si cela apparaît parfois dans des documents de produit ou un README ancien :

| Sujet | État |
|-------|------|
| Édition du **nom de patch** dans l’UI | Affichage seulement |
| **Undo / Redo** | Absent |
| Bouton **AutoConnect** | Absent (restauration des ports + inquiry auto) |
| Import / export de **banque entière** multi-patches | Absent (fichiers `.syx` d’un seul patch) |
| Tags, recherche, favoris dans le navigateur de patches | Absent |
| Settings → Master Operations / Logging | *Coming soon...* |
| Settings → Defrag manuel | *Coming soon...* (Defrag via Mutator si historique plein) |
| Randomize global hors Mutator | Absent |
| Interface en français | Absent (UI anglaise) |
| Peaux personnalisées utilisateur | Absent |
| Assistant de premier lancement | Absent (guidance via le pied de page) |

Le README du dépôt peut encore lister d’anciennes pastilles « en cours / à venir » : **ce manuel prime pour l’état réel du code actuel**.

---

## 18. Dépannage rapide

| Symptôme | Pistes |
|----------|--------|
| **No device** / éditeur verrouillé | Câbles IN↔OUT dans le bon sens ; bons ports FROM / TO ; synthé allumé ; redémarrage du Matrix ; sous Windows, port non monopolisé par une autre appli |
| Les knobs bougent mais rien ne change sur le synthé | Vérifier **MIDI TO** ; canal MIDI (MASTER EDIT sur M-1000) ; MEMORY PROTECT / BANK LOCK |
| Changement de patch : l’éditeur ne se met pas à jour | Vérifier **EDITOR MIDI FROM** (le synthé doit pouvoir répondre) |
| STORE ou PASTE grisés | Banque ROM 2–9 sur Matrix-1000 — passer en banque 0 ou 1 |
| MASTER EDIT / BANK UTILITY grisés | Vous êtes sur Matrix-6 / 6R, ou aucun appareil détecté |
| Compare empêche toute édition | Désactiver **C** dans le Mutator |
| Plugin invisible dans la DAW | Rescan plugins ; vérifier format (AU/VST3) et dossier d’install ; architecture (Apple Silicon / Intel) |
| Latence audio dans la DAW | Ajuster **HARDWARE LATENCY** dans Settings (plugin) ; et les buffers de la DAW / de l’interface |

---

## Lexique

Définitions courtes, orientées usage. Les libellés entre guillemets sont ceux de l’interface (anglais).

| Terme | Définition |
|-------|------------|
| **AU** | Format de plugin audio sur macOS (Audio Unit). |
| **Banque (Bank)** | Sur le Matrix-1000 : tiroir de 100 sons. Il y a 10 banques (0–9). Le Matrix-6 / 6R n’a pas de banques. |
| **DAW** | Digital Audio Workstation — le séquenceur / studio logiciel (Live, Logic, Reaper…). |
| **DCO** | Digitally Controlled Oscillator — oscillateur qui produit la forme d’onde de base du son. |
| **Device Inquiry** | Message MIDI universel : « qui es-tu ? ». Matrix-Control s’en sert pour reconnaître le synthé. |
| **Dump** | Envoi ou réception du contenu complet d’un patch (ou d’un master) en SysEx. |
| **Edit buffer** | Zone temporaire du Matrix-1000 où l’on peut auditionner un son sans l’écrire tout de suite en mémoire. |
| **ENV** | Envelope — enveloppe (attaque, decay, sustain, release…) qui façonne l’évolution dans le temps. |
| **FM** | Frequency Modulation — ici, modulation du filtre par l’oscillateur, pour des timbres plus riches ou métalliques. |
| **LFO** | Low Frequency Oscillator — oscillateur lent pour faire vibrer, trembler, balayer un paramètre. |
| **MAO** | Musique Assistée par Ordinateur — produire de la musique avec un ordinateur. |
| **MASTER** | Réglages globaux de la machine (canal MIDI, vibrato global, accordage…), distincts d’un patch. |
| **Matrice de modulation** | Tableau de « câbles virtuels » : une source module une destination avec une intensité. |
| **MIDI** | Protocole pour faire communiquer instruments et ordinateurs (notes, contrôles, SysEx…). |
| **MIDI FROM / MIDI TO** | Dans Matrix-Control : entrée depuis le synthé / sortie vers le synthé. |
| **Modulation** | Faire varier automatiquement un paramètre (filtre, volume, hauteur…) à partir d’une source. |
| **Mutator** | Outil qui crée des variations aléatoires contrôlées d’un patch. |
| **NumberBox** | Case numérique cliquable / saisissable (numéro de patch, etc.). |
| **Patch** | Un son mémorisé — l’ensemble des réglages qui donnent un timbre. |
| **Plugin** | Logiciel qui s’ouvre *dans* une DAW. |
| **Portamento** | Glissando de hauteur entre deux notes. |
| **Presse-papiers (clipboard)** | Mémoire temporaire Copy/Paste *interne* à Matrix-Control pour modules ou patches. |
| **Program Change** | Message MIDI « passe au son numéro n ». |
| **Ramp** | Générateur de rampe (montée progressive) utilisable comme source de modulation. |
| **ROM** | Mémoire en lecture seule. Sur M-1000, banques 2–9 : pas de STORE / PASTE / INIT depuis l’éditeur. |
| **Standalone** | Application autonome, sans DAW. |
| **STORE** | Écrire le patch courant dans la mémoire du synthé. |
| **SysEx** | System Exclusive — messages MIDI détaillés propres à une marque / un modèle, utilisés pour éditer le Matrix en profondeur. |
| **`.syx`** | Fichier contenant un message SysEx (ici : un patch). |
| **Track (generator)** | Courbe qui transforme la position sur le clavier en valeur de modulation. |
| **VCA** | Voltage Controlled Amplifier — étage de volume du son. |
| **VCF** | Voltage Controlled Filter — filtre qui sculpte le timbre (grave / aigu, résonance). |
| **VST3** | Format de plugin très répandu (Windows, macOS, etc.). |
| **Unlock (bank)** | Commande qui libère le verrouillage de banque côté Matrix-1000 pour certaines saisies façade. |

---

*Document rédigé pour coller au comportement réel du code et de l’UI à la date de rédaction. Lors des tests UAT, notez toute divergence entre ce manuel et ce que vous observez à l’écran — ce sera le signal pour corriger le document et/ou le logiciel.*
