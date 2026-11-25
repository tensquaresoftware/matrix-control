# Settings unifiée et header — récap produit

**Date :** 2026-10-04  
**Statut :** intentions figées après discussion ; le découpage en chantiers viendra ensuite  
**Hors de ce document :** ordre d’implémentation, spec technique, assistant de premier lancement (chantier dédié déjà prévu)

Ce texte rassemble les changements discutés pour y voir clair. Les noms à l’écran restent en anglais, comme dans le produit.

---

## L’idée en une phrase

Une seule fenêtre **Settings**, organisée en onglets comme les Réglages d’Ableton Live, et un header qui ne montre plus le câblage (plus de listes de ports) mais l’activité : voyants MIDI, gain audio, niveau crête.

---

## Pourquoi on y va

Aujourd’hui, en app autonome, il y a deux portes d’entrée : **Settings...** et **Audio/MIDI...**, donc deux menus et deux raccourcis. La fenêtre audio porte encore un nom JUCE alors qu’il n’y a plus de MIDI dedans. Les listes de ports MIDI (et la source d’écoute audio) dans le header sont pratiques, mais trop faciles à changer par un geste malheureux / dangeureux en contexte de scène / performance live.

Le plugin n’affiche déjà pas **Audio/MIDI...** (l’hôte gère l’audio). Une Settings unique aligne les deux modes et ressemble davantage à un logiciel « normal » : un menu **Settings...**, un raccourci.

L’app n’a pas encore été diffusée : pas de compatibilité à préserver sur les anciens noms de menu ni sur l’ancien raccourci audio.

---

## Une seule fenêtre Settings

- Un seul item **Settings...** dans la liste au clic sur le logo.
- Un seul raccourci clavier pour l’ouvrir (celui de Settings aujourd’hui). Pas de second raccourci « ancien Audio/MIDI ».
- **About** reste à part, toujours depuis le menu logo.
- **Skin** et **UI Scale** restent dans le menu logo, avec les raccourcis d’échelle déjà en place. Ils n’apparaissent pas une deuxième fois dans Settings.
- La fenêtre mémorise le **dernier onglet** ouvert et le réaffiche la fois suivante.
- Forme visée : plus large et moins haute que la Settings actuelle. Deux colonnes, séparées par un filet vertical fin :
  - à gauche, étroite : les noms d’onglets empilés ;
  - à droite, plus large : le contenu de l’onglet courant ;
  - l’onglet actif se lit tout de suite (fond un peu plus clair).
- Chrome entièrement Matrix (plus les labels et listes d’allure JUCE de la fenêtre audio actuelle).
- Un onglet un peu vide n’est pas un problème : on pourra y ajouter des réglages plus tard sans tout mélanger.

### Les onglets (app autonome)

1. **USER INTERFACE** — aujourd’hui la section **INTERFACE** (messages, aide contextuelle, etc.). On allonge le nom pour que ce soit évident.
2. **DEVICE**
3. **MIDI**
4. **AUDIO**
5. **PATCH**
6. **PATCH MUTATOR** — volontairement séparé de **PATCH**.
7. **MASTER**

On ne fusionne pas des notions seulement « voisines ». Mieux vaut un onglet clair et court qu’un fourre-tout.

### Onglet MIDI

Colonne de contenu, une ligne par port : label, liste, voyant à droite de la liste (le même flux que dans le header, pour valider le port **pendant** qu’on le choisit) :

- **KEYBOARD FROM** + liste + voyant (sous-entendu : port MIDI du clavier maître)
- **SYNTH FROM** + liste + voyant (port MIDI venant du Matrix)
- **SYNTH TO** + liste + voyant (port MIDI allant vers le Matrix)

Les voyants du header restent : Settings sert au câblage, le header au regard de scène. Ce n’est pas deux réglages, c’est le même témoin à deux endroits.

Pas besoin d’écrire « MIDI port » : l’onglet s’appelle déjà **MIDI**. **Synth** désigne le Matrix-1000 / 6 / 6R, plus clair ici qu’**Instrument**.

On écarte **MIDI Editor From / To** : « Editor To » se lit dans les deux sens et induit en erreur.

En **plugin**, la ligne **KEYBOARD FROM** disparaît (le clavier passe par l’hôte, ce n’est pas un choix de port). Il reste **SYNTH FROM** et **SYNTH TO**, chacune avec son voyant. Le voyant **FROM KEYBOARD** du header, lui, reste.

### Onglet AUDIO

C’est ici que vit tout le câblage audio, plus seulement un nouveau nom de fenêtre.

**Deux listes d’appareils (entrée / sortie), sur les trois OS.** Mac le fait nativement (Core Audio). Windows aussi, dès que le pilote n’est pas un bloc unique : WASAPI (le plus courant dans JUCE) permet une entrée et une sortie différentes. Linux (ALSA, etc.) aussi. Le cas à part, surtout sous Windows, c’est **ASIO** : le pilote est un **boîtier entier** (même Scarlett en entrée et en sortie). On **garde quand même** les deux lignes **INPUT DEVICE** et **OUTPUT DEVICE** partout, comme Live : sous ASIO, les deux listes montrent le même appareil et restent liées (changer l’une change l’autre). On ne bascule pas sur une seule ligne « Device » selon l’OS.

Ordre des lignes (app autonome) :

1. **DRIVER TYPE**
2. **INPUT DEVICE**
3. **OUTPUT DEVICE**
4. **SAMPLE RATE**
5. **BUFFER SIZE**
6. *(ligne vide — sépare « quelle machine / quelle horloge » de « quels fils »)*
7. **INPUT CHANNELS** — quelle **paire** de fils est allumée (un seul choix)
8. **SYNTH FROM** + indicateur de crête à droite de la liste, **sans** label — robinet d’écoute dans cette paire (mono 1, mono 2, ou stéréo 1/2). La liste est un peu plus courte pour que liste + crête ne dépassent pas la largeur des lignes du haut.
9. **OUTPUT CHANNELS** — même widget (une paire allumée)
10. **PLAY TEST TONE** — bouton d’action **seul**, dans la colonne des contrôles (pas de label **OUTPUT TEST** à gauche)

D’abord on choisit les boîtiers et l’horloge (taux, buffer), ensuite on câble : quelle paire d’entrée est ouverte, lequel de ces fils on écoute (avec le crête), quelle paire de sortie, puis un son de test vers ces sorties. Une **ligne vide** sépare le bloc entrée (**INPUT CHANNELS** + **SYNTH FROM**) du bloc sortie (**OUTPUT CHANNELS** + bouton).

**Deux colonnes dans le contenu :** à gauche les labels, à droite les contrôles. Un label ne glisse jamais dans la zone des contrôles ; une suite de paires ou le bouton ne glisse jamais sous les labels. Les lignes de suite (paires suivantes, bouton) ont la colonne label **vide**.

**Widget Matrix pour les canaux (dans ce chantier) : RadioButtonGroup.** On ne reprend pas les cases JUCE. Carrés anguleux, couleurs Matrix déjà utilisées, **pas des ronds**. Un seul choix à la fois, comme aujourd’hui avec le bus stéréo : paires **1 + 2**, **3 + 4**, **5 + 6** (exemple Scarlett 6i6). Autant de paires que possible **sur la même ligne** dans la colonne contrôles, puis retour à la ligne (une RME ne s’empile pas une paire par rangée). On n’ouvre pas un canal mono isolé à ce niveau — ça rouvrirait la carte à chaque changement d’écoute. **SYNTH FROM** reste le robinet logiciel dans la paire déjà ouverte (sans redémarrer l’audio).

Le bouton sous les sorties parle de la **sortie**. Le crête à droite de **SYNTH FROM** parle de l’**entrée**.

Le **gain** reste uniquement dans le header : c’est un réglage, on ne le duplique pas. Le crête, comme les voyants MIDI, est un **témoin** : header pour la scène, Settings pour le câblage.

Même règle que dans l’onglet **MIDI** : on ne répète pas le nom de l’onglet dans le label. **Synth From** dit « le flux audio émis par le synthé, tel qu’il arrive sur cette entrée » — par exemple Scarlett 1 ou 2 en mono pour un Matrix-1000, ou Scarlett 1/2 en stéréo pour un Matrix-6 / 6R. C’est plus parlant qu’**Audio From**, qui nomme le médium plutôt que la source.

**SYNTH FROM** apparaît donc deux fois dans Settings, une fois par onglet : port MIDI dans **MIDI**, entrée audio dans **AUDIO**. Ce n’est pas un doublon maladroit : l’onglet dit de quel fil on parle. Les deux listes restent indépendantes.

En **plugin**, l’onglet **AUDIO** disparaît entièrement. Il n’y aurait rien de propre à y mettre : la carte son et **Synth From** (audio) sont l’affaire de l’hôte ; le gain et le crête du header n’existent que dans l’app autonome. Une page vide ou une phrase d’excuse serait moins clair que pas d’onglet du tout.

Donc en plugin, Settings = **USER INTERFACE**, **DEVICE**, **MIDI** (synth seulement), **PATCH**, **PATCH MUTATOR**, **MASTER**.

Le bouton sous les sorties s’appelle **PLAY TEST TONE**.

### Croquis des onglets

Fenêtre **SETTINGS**, deux colonnes, filet vertical entre les deux. L’onglet courant est le seul à fond plus clair (ici : `[ … ]`). Les largeurs sont du dessin en caractères, pas des mesures d’écran. Les listes sont notées `[ … v ]`, les boutons `[ NOM ]`, les sliders `[====|==== ]`, les voyants `□` (carré vide), l’indicateur de crête `⬓` (moitié basse pleine, axe vertical), les radios `[■]` (choisi) et `[ ]` (libre).

Colonne de gauche en **plugin** : mêmes noms, **sans AUDIO**.

Les réglages repris tels quels sont ceux d’aujourd’hui (sections actuelles + fenêtre audio), rangés dans l’onglet qui leur correspond. Les canaux : **RadioButtonGroup** Matrix, paires exclusives, pas les cases JUCE.

#### USER INTERFACE — app autonome

Même contenu en plugin ; seule la colonne de gauche perd **AUDIO**.

```
┌───────────────────────────────────────────────────────────────────────────┐
│                                 SETTINGS                                  │
├──────────────────┬────────────────────────────────────────────────────────┤
│[USER INTERFACE]  │  INFO MESSAGE          [ KEEP                      v ] │
│ DEVICE           │  CONTEXTUAL HELP       [ SHOW                      v ] │
│ MIDI             │                                                        │
│ AUDIO            │                                                        │
│ PATCH            │                                                        │
│ PATCH MUTATOR    │                                                        │
│ MASTER           │                                                        │
└──────────────────┴────────────────────────────────────────────────────────┘
```

#### DEVICE — app autonome

Pas de **HARDWARE LATENCY** (ce réglage n’existe que dans le plugin, pour se caler sur le délai de l’hôte).

```
┌───────────────────────────────────────────────────────────────────────────┐
│                                 SETTINGS                                  │
├──────────────────┬────────────────────────────────────────────────────────┤
│ USER INTERFACE   │  EPROM TYPE            [ FACTORY                   v ] │
│[DEVICE]          │                                                        │
│ MIDI             │                                                        │
│ AUDIO            │                                                        │
│ PATCH            │                                                        │
│ PATCH MUTATOR    │                                                        │
│ MASTER           │                                                        │
└──────────────────┴────────────────────────────────────────────────────────┘
```

#### DEVICE — plugin

**AUDIO** absent à gauche. **HARDWARE LATENCY** apparaît.

```
┌───────────────────────────────────────────────────────────────────────────┐
│                                 SETTINGS                                  │
├──────────────────┬────────────────────────────────────────────────────────┤
│ USER INTERFACE   │  HARDWARE LATENCY      [====|====              ]  5 ms │
│[DEVICE]          │  EPROM TYPE            [ FACTORY                   v ] │
│ MIDI             │                                                        │
│ PATCH            │                                                        │
│ PATCH MUTATOR    │                                                        │
│ MASTER           │                                                        │
└──────────────────┴────────────────────────────────────────────────────────┘
```

#### MIDI — app autonome

Voyant à droite de chaque liste.

```
┌───────────────────────────────────────────────────────────────────────────┐
│                                 SETTINGS                                  │
├──────────────────┬────────────────────────────────────────────────────────┤
│ USER INTERFACE   │  KEYBOARD FROM         [ NO INPUT               v ] □  │
│ DEVICE           │  SYNTH FROM            [ NO INPUT               v ] □  │
│[MIDI]            │  SYNTH TO              [ NO OUTPUT              v ] □  │
│ AUDIO            │                                                        │
│ PATCH            │                                                        │
│ PATCH MUTATOR    │                                                        │
│ MASTER           │                                                        │
└──────────────────┴────────────────────────────────────────────────────────┘
```

#### MIDI — plugin

Pas de ligne **KEYBOARD FROM** (le clavier passe par l’hôte). Le voyant **FROM KEYBOARD** reste dans le header, pas ici.

```
┌───────────────────────────────────────────────────────────────────────────┐
│                                 SETTINGS                                  │
├──────────────────┬────────────────────────────────────────────────────────┤
│ USER INTERFACE   │  SYNTH FROM            [ NO INPUT               v ] □  │
│ DEVICE           │  SYNTH TO              [ NO OUTPUT              v ] □  │
│[MIDI]            │                                                        │
│ PATCH            │                                                        │
│ PATCH MUTATOR    │                                                        │
│ MASTER           │                                                        │
└──────────────────┴────────────────────────────────────────────────────────┘
```

#### AUDIO — app autonome seulement

Onglet **absent en plugin**. Les libellés d’appareil ne répètent pas **AUDIO**. Le gain reste dans le header. Labels à gauche, contrôles à droite (jamais mélangés). Paires côte à côte dans la colonne contrôles, retour à la ligne si besoin. **PLAY TEST TONE** sans label, aligné sur les contrôles.

```
┌───────────────────────────────────────────────────────────────────────────┐
│                                 SETTINGS                                  │
├──────────────────┬────────────────────────────────────────────────────────┤
│ USER INTERFACE   │  DRIVER TYPE           [ CoreAudio                 v ] │
│ DEVICE           │  INPUT DEVICE          [ Scarlett 6i6              v ] │
│ MIDI             │  OUTPUT DEVICE         [ Scarlett 6i6              v ] │
│[AUDIO]           │  SAMPLE RATE           [ 48000                     v ] │
│ PATCH            │  BUFFER SIZE           [ 512                       v ] │
│ PATCH MUTATOR    │                                                        │
│ MASTER           │  INPUT CHANNELS        [■] 1 + 2  [ ] 3 + 4  [ ] 5 + 6 │
│                  │  SYNTH FROM            [ SCARLETT 6I6 (1) v ]  ⬓       │
│                  │                                                        │
│                  │  OUTPUT CHANNELS       [■] 1 + 2  [ ] 3 + 4  [ ] 5 + 6 │
│                  │                        [       PLAY TEST TONE        ] │
└──────────────────┴────────────────────────────────────────────────────────┘
```

Si le périphérique a trop de paires pour une ligne, la suite reste dans la colonne contrôles :

```
│ MASTER           │  INPUT CHANNELS        [ ] 1 + 2  [ ] 3 + 4  [ ] 5 + 6 │
│                  │                        [ ] 7 + 8  [ ] 9 + 10 [■] 11+12 │
```

#### PATCH — app autonome

Même contenu en plugin ; colonne de gauche sans **AUDIO**.

```
┌───────────────────────────────────────────────────────────────────────────┐
│                                 SETTINGS                                  │
├──────────────────┬────────────────────────────────────────────────────────┤
│ USER INTERFACE   │  MATRIX-1000 PATCHES   [ DISPLAY MUSICAL NAMES     v ] │
│ DEVICE           │  COMPUTER PATCHES      [ DISPLAY SYSEX NAMES       v ] │
│ MIDI             │  UNSAVED STATE         [ ALWAYS WARN               v ] │
│ AUDIO            │  INIT TEMPLATE         [ SAVE AS INIT ] [ DELETE ]     │
│[PATCH]           │                                                        │
│ PATCH MUTATOR    │                                                        │
│ MASTER           │                                                        │
└──────────────────┴────────────────────────────────────────────────────────┘
```

#### PATCH MUTATOR — app autonome

Même contenu en plugin ; colonne de gauche sans **AUDIO**.

```
┌───────────────────────────────────────────────────────────────────────────┐
│                                 SETTINGS                                  │
├──────────────────┬────────────────────────────────────────────────────────┤
│ USER INTERFACE   │  DELETE WARNING        [ ALWAYS WARN               v ] │
│ DEVICE           │  MUTATION HISTORY      [ DEFRAG ]                      │
│ MIDI             │                                                        │
│ AUDIO            │                                                        │
│ PATCH            │                                                        │
│[PATCH MUTATOR]   │                                                        │
│ MASTER           │                                                        │
└──────────────────┴────────────────────────────────────────────────────────┘
```

#### MASTER — app autonome

Même contenu en plugin ; colonne de gauche sans **AUDIO**.

```
┌───────────────────────────────────────────────────────────────────────────┐
│                                 SETTINGS                                  │
├──────────────────┬────────────────────────────────────────────────────────┤
│ USER INTERFACE   │  UTILITY               [ LOAD ] [ SAVE AS ] [ INIT ]   │
│ DEVICE           │  INIT TEMPLATE         [ SAVE AS INIT ] [ DELETE ]     │
│ MIDI             │                                                        │
│ AUDIO            │                                                        │
│ PATCH            │                                                        │
│ PATCH MUTATOR    │                                                        │
│[MASTER]          │                                                        │
└──────────────────┴────────────────────────────────────────────────────────┘
```

---

## Le header : monitoring, plus câblage

Règle simple : **le header montre ce qui circule ; Settings règle les fils.**

### Ce qui sort du header

- Les trois listes MIDI (**Keyboard From**, **MIDI From**, **MIDI To**).
- La liste de source d’écoute (aujourd’hui **Audio From**, demain **Synth From** dans Settings > AUDIO).

Changer un port ou une source audio se fait dans Settings (et, au premier lancement, dans l’assistant dédié, qui portera déjà toutes ces listes).

### Ce qui reste dans le header

**Deux blocs encadrés : MIDI, puis AUDIO**

Chaque bloc commence à gauche par un **cartouche** : le mot **MIDI** ou **AUDIO**, fond du même gris clair que les labels, texte gras de la couleur du fond du header (le mot se lit en « inverse » dans une plage claire).

Les arêtes haute et basse de ce cartouche se prolongent en **lignes fines** qui courent au-dessus et au-dessous des éléments du bloc, puis un **trait vertical** à droite referme le cadre en joignant les deux bouts. L’intérieur du cadre (hors cartouche) reste le fond du header : on entoure, on ne remplit pas.

Même règle que dans Settings : le titre de bloc dit MIDI ou AUDIO une seule fois ; on ne le répète pas dans chaque label intérieur.

**Bloc MIDI** (app autonome et plugin)

Les trois voyants restent, label à droite de chaque voyant comme aujourd’hui. Les textes parlent du **flux**, pas du nom d’une liste :

- **FROM KEYBOARD** — messages MIDI reçus en provenance du clavier maître (en plugin : le clavier arrive via l’hôte ; on garde ce libellé, pas **FROM HOST**)
- **FROM SYNTH** — messages MIDI reçus en provenance du synthé
- **TO SYNTH** — messages MIDI envoyés au synthé

En **plugin** : on enlève la liste grisée **HOST** (header et Settings). On **garde** le voyant **FROM KEYBOARD** et son label : c’est le seul regard immédiat sur « des notes arrivent du DAW ».

**Bloc AUDIO** (app autonome seulement, comme aujourd’hui)

- **INPUT GAIN** (plus **AUDIO** devant : le cartouche le dit déjà)
- le slider de gain
- l’indicateur de niveau crête

L’aide en bas de fenêtre du slider devra dire que la source d’écoute se choisit dans **Settings > AUDIO**, ligne **SYNTH FROM**. Un crête à zéro dans le header, sans cette phrase, veut trop de choses à la fois. Le même témoin de crête apparaît à droite de cette liste, pour le câblage.

### Croquis des cadres (même lecture que le header)

Le rectangle de gauche est le cartouche rempli. Les traits haut, bas et droit sont les filets. Ce n’est pas une boîte grise autour de tout le bloc.

```
  cartouche (fond gris clair, mot en gras couleur du header)
  ┌──────┬─────────────────────────────────────────────────────┐
  │ MIDI │  □  FROM KEYBOARD    □  FROM SYNTH    □  TO SYNTH   │
  └──────┴─────────────────────────────────────────────────────┘
         └──────── lignes haut / bas = arêtes du cartouche ────┘
                                                      fermeture

  ┌───────┬────────────────────────────────────────────────────┐
  │ AUDIO │  INPUT GAIN   [================|=======     ]  ⬓   │
  └───────┴────────────────────────────────────────────────────┘
```

Les `□` sont les voyants (carré vide), `[====|=== ]` le slider, `⬓` l’indicateur de crête (moitié basse pleine, axe vertical). Les largeurs ici sont du dessin en caractères, pas des mesures d’écran.

---

## App autonome et plugin, côte à côte

| Sujet | App autonome | Plugin |
| --- | --- | --- |
| Menu logo | **Settings...**, **About...**, Skin, UI Scale | Pareil (plus de **Audio/MIDI...**) |
| Onglet AUDIO | Oui | Non |
| Listes MIDI dans Settings | Clavier + synthé (from / to), voyant à droite | Synthé seulement, voyant à droite |
| Voyant FROM KEYBOARD | Header + ligne Settings MIDI | Header seulement (pas de ligne clavier dans Settings) |
| Liste Keyboard / HOST | Liste dans Settings > MIDI | Absente |
| Gain | Header seulement | Absent (comme aujourd’hui) |
| Crête | Header + à droite de **SYNTH FROM** (Settings AUDIO) | Absent (comme aujourd’hui) |
| Bouton sous les sorties | **PLAY TEST TONE** (Settings AUDIO) | Absent |
| Synth From (audio) | Settings > AUDIO seulement | Absent (l’hôte route l’audio) |

---

## Ce que ce chantier n’est pas

- Ce n’est pas l’assistant de premier lancement. Cette modale aura les listes MIDI et audio ; on la traitera dans son chantier.
- Ce n’est pas un changement de **Skin** / **UI Scale** (ils restent au logo).
- Ce n’est pas **About**.
- Les croquis d’onglets et de header sont une lecture d’intention, pas des mesures d’écran.

---

## Découpage proposé (trois livraisons)

On ne vide pas le header tant que Settings n’a pas l’onglet qui reprend le câblage. L’assistant de premier lancement reste un chantier **à part**.

### 1 — Coquille Settings à onglets (contenu actuel)

La fenêtre **Settings** passe au format Live : colonne d’onglets, contenu à droite, titre **SETTINGS** centré, dernier onglet mémorisé.

Onglets : **USER INTERFACE**, **DEVICE**, **PATCH**, **PATCH MUTATOR**, **MASTER**. Même contenu qu’aujourd’hui (y compris **HARDWARE LATENCY** en plugin seulement).

**Pas encore** d’onglets **MIDI** ni **AUDIO**. Le menu **Audio/MIDI...** et le header restent tels quels. Livrable tout de suite, sans changer le câblage.

### 2 — Onglet AUDIO et une seule porte Settings

Nouveau widget **RadioButtonGroup** (carrés Matrix, une paire à la fois, plusieurs paires par ligne).

L’onglet **AUDIO** (autonome seulement) absorbe l’ancienne fenêtre audio : pilote, appareils, taux, buffer, paires, **SYNTH FROM** + crête, **PLAY TEST TONE**. On retire **Audio From** du header. Il reste gain + crête, avec le cartouche **AUDIO**.

Plus de menu **Audio/MIDI...**, plus de second raccourci : un seul **Settings...**. En plugin, pas d’onglet **AUDIO**.

### 3 — Onglet MIDI et header de monitoring

Onglet **MIDI** : listes **KEYBOARD FROM** / **SYNTH FROM** / **SYNTH TO**, voyant `□` à droite de chaque liste (pas de ligne clavier en plugin).

On retire les listes MIDI du header. Il reste les trois voyants avec **FROM KEYBOARD** / **FROM SYNTH** / **TO SYNTH**, cartouche **MIDI**. Plus de **HOST** grisé.

Après 3, le header ne fait plus que le regard de scène ; tout le câblage vit dans Settings.

---

## Décisions volontairement laissées pour plus tard

- Taille exacte de la fenêtre aux différents **UI Scale**, et comportement si l’écran est trop petit (surtout en plugin dans un hôte étroit).
- Mesures exactes des cartouches, filets et colonnes d’onglets (épaisseur, gaps, hauteur par rapport aux contrôles).
- Sous ASIO : les deux listes d’appareils restent visibles mais liées (même nom des deux côtés).
- Combien de paires tiennent par ligne du **RadioButtonGroup** selon la largeur réelle (les retours restent dans la colonne contrôles).

---

## Fil conducteur à relire avant le découpage

1. Un menu, un raccourci, une fenêtre **Settings** à onglets.
2. Câblage MIDI et audio dans Settings (et au premier lancement dans l’assistant).
3. Header = voyants MIDI + gain + crête ; Settings reprend les **témoins** au moment du câblage (voyants à droite des listes MIDI ; crête à droite de **SYNTH FROM** ; **PLAY TEST TONE** sans label, colonne contrôles). Canaux : **RadioButtonGroup** Matrix (paires exclusives, côte à côte puis retour à la ligne, toujours à droite des labels). **SYNTH FROM** reste le robinet dans la paire ouverte.
4. Vocabulaire : **FROM / TO** et **Synth** ; le nom d’onglet ou le cartouche de bloc n’est pas répété dans les labels (**SYNTH FROM** dans **MIDI** et dans **AUDIO**, **INPUT GAIN** dans le bloc header **AUDIO**, **INPUT CHANNELS** / **OUTPUT CHANNELS**, **PLAY TEST TONE**, **FROM KEYBOARD** partout y compris en plugin).
5. En plugin, on cache ce qui n’est pas un vrai choix (audio, liste clavier HOST), on garde ce qui informe (voyant clavier dans le header).
