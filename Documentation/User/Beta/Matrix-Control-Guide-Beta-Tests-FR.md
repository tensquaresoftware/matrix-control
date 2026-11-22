# MATRIX-CONTROL — GUIDE DE BÊTA-TESTS

> **Version de travail (Markdown).** Pour naviguer et mettre au point le contenu. La livraison bêta-testeurs restera en `.txt` une fois les textes figés.

- **Organisation :** Ten Square Software
- **Auteur :** Guillaume DUPONT
- **Email :** tensquaresoftware@gmail.com
- **Produit :** Matrix-Control
- **Version du guide :** 1.0
- **Date de révision :** 22/11/2026

## Sommaire

- [Bienvenue](#bienvenue)
- [À quoi sert ce guide ?](#a-quoi-sert-ce-guide)
- [1. Ce dont vous aurez besoin pour tester le Matrix-Control](#1-ce-dont-vous-aurez-besoin-pour-tester-le-matrix-control)
  - [1.1. Synthétiseur](#1-1-synthetiseur)
  - [1.2. Ordinateur](#1-2-ordinateur)
  - [1.3. Accessoires](#1-3-accessoires)
  - [1.4. Logiciels](#1-4-logiciels)
- [2. Points importants avant de commencer](#2-points-importants-avant-de-commencer)
  - [2.1. Distribution en fichier ZIP](#2-1-distribution-en-fichier-zip)
  - [2.2. Messages de sécurité de macOS ou Windows](#2-2-messages-de-securite-de-macos-ou-windows)
  - [2.3. Écoute audio Standalone sous Windows (ASIO)](#2-3-ecoute-audio-standalone-sous-windows-asio)
  - [2.4. Protégez vos patches avant de démarrer les tests](#2-4-protegez-vos-patches-avant-de-demarrer-les-tests)
  - [2.5. Si STORE semble ne pas enregistrer](#2-5-si-store-semble-ne-pas-enregistrer)
- [3. Fiche de votre session de tests](#3-fiche-de-votre-session-de-tests)
- [4. Installer le Matrix-Control](#4-installer-le-matrix-control)
  - [4.1. Contenu de l'archive ZIP](#4-1-contenu-de-l-archive-zip)
  - [4.2. Tester sous macOS](#4-2-tester-sous-macos)
  - [4.3. Tester sous Windows](#4-3-tester-sous-windows)
  - [4.4. Tester sous Linux](#4-4-tester-sous-linux)
- [5. Connecter le synthétiseur](#5-connecter-le-synthetiseur)
- [6. Premier lancement du Matrix-Control](#6-premier-lancement-du-matrix-control)
  - [6.1. Écran d'accueil](#6-1-ecran-d-accueil)
  - [6.2. Étapes communes (plugin et Standalone)](#6-2-etapes-communes-plugin-et-standalone)
  - [6.3. Standalone seulement](#6-3-standalone-seulement)
- [7. Vérifier la communication MIDI](#7-verifier-la-communication-midi)
  - [7.1. Choisir ou confirmer les ports MIDI](#7-1-choisir-ou-confirmer-les-ports-midi)
  - [7.2. Édition verrouillée puis débloquée](#7-2-edition-verrouillee-puis-debloquee)
  - [7.3. Sous Windows : port MIDI déjà réquisitionné](#7-3-sous-windows-port-midi-deja-requisitionne)
  - [7.4. Version du Matrix-Control et EPROM TYPE](#7-4-version-du-matrix-control-et-eprom-type)
- [8. Tester l'application autonome et le plugin](#8-tester-l-application-autonome-et-le-plugin)
  - [8.1. En plugin](#8-1-en-plugin)
  - [8.2. En Standalone](#8-2-en-standalone)
  - [8.3. Écoute audio sous Windows (Standalone)](#8-3-ecoute-audio-sous-windows-standalone)
- [9. Ableton Live et image floue sous Windows](#9-ableton-live-et-image-floue-sous-windows)
- [10. Éditer un patch](#10-editer-un-patch)
  - [10.1. Gestes représentatifs](#10-1-gestes-representatifs)
  - [10.2. Boutons I / C / P sur un module](#10-2-boutons-i-c-p-sur-un-module)
  - [10.3. Annuler / rétablir](#10-3-annuler-retablir)
  - [10.4. Nom du patch](#10-4-nom-du-patch)
- [11. Éditer la matrice de modulation](#11-editer-la-matrice-de-modulation)
- [12. Gérer les patches dans la mémoire du synthé](#12-gerer-les-patches-dans-la-memoire-du-synthe)
  - [12.1. Navigation et STORE](#12-1-navigation-et-store)
  - [12.2. INIT / COPY / PASTE](#12-2-init-copy-paste)
  - [12.3. Dialogue « Unsaved patch »](#12-3-dialogue-unsaved-patch)
- [13. Gérer les patches dans des fichiers informatiques](#13-gerer-les-patches-dans-des-fichiers-informatiques)
  - [13.1. OPEN / SAVE / parcours](#13-1-open-save-parcours)
  - [13.2. Glisser-déposer des fichiers sur la fenêtre](#13-2-glisser-deposer-des-fichiers-sur-la-fenetre)
- [14. Créer des variations de patch](#14-creer-des-variations-de-patch)
  - [14.1. Mutation de base](#14-1-mutation-de-base)
  - [14.2. MODE et PITCH](#14-2-mode-et-pitch)
- [15. Gérer les banques (Matrix-1000 uniquement)](#15-gerer-les-banques-matrix-1000-uniquement)
  - [15.1. Sélection de banque](#15-1-selection-de-banque)
  - [15.2. COPY / PASTE / IMPORT / EXPORT](#15-2-copy-paste-import-export)
- [16. Réglages globaux « Master » (Matrix-1000 uniquement)](#16-reglages-globaux-master-matrix-1000-uniquement)
  - [16.1. Édition MASTER EDIT](#16-1-edition-master-edit)
  - [16.2. Fichiers Master](#16-2-fichiers-master)
- [17. Préférences dans la fenêtre Settings](#17-preferences-dans-la-fenetre-settings)
- [18. Points connus à vérifier](#18-points-connus-a-verifier)
- [19. Comment signaler un problème](#19-comment-signaler-un-probleme)
- [20. Parcours rapide de tests](#20-parcours-rapide-de-tests)
- [21. Parcours complet de tests](#21-parcours-complet-de-tests)
- [22. Bilan de votre session de tests](#22-bilan-de-votre-session-de-tests)

## Bienvenue

Merci de participer aux bêta-tests du Matrix-Control ! :)

Le Matrix-Control est un éditeur MIDI moderne pour les synthétiseurs Oberheim Matrix-1000, Matrix-6 et Matrix-6R. Utilisable en application autonome ou en plugin, il sert de « télécommande » à l'écran : il envoie et reçoit des messages MIDI (réglages, patches, master) pour que vous puissiez voir et modifier le synthé plus confortablement. En revanche, il ne produit pas le son et ne l'enregistre pas non plus — le flux audio sort toujours du synthétiseur. Pour l'entendre, vous devrez brancher la sortie audio du synthé sur une entrée de votre interface audio, votre table de mixage ou vos enceintes.

## À quoi sert ce guide ?

Ce document n'est pas un cours sur la synthèse sonore, ni le manuel utilisateur complet du Matrix-Control. C'est un guide de TESTS : une liste de situations concrètes à rejouer chez vous, pour vérifier que tout se comporte correctement sur la version bêta du logiciel, avant la diffusion d'une version finale et stable par le développeur. Vos retours (surtout les bugs et les moments de confusion) sont précieux !

#### Comment utiliser ce guide de tests ?

Parcourez-le dans l'ordre, du début à la fin. Chaque section correspond à une étape naturelle : installer, brancher le synthé, démarrer, éditer, etc.

1. Ouvrez-le dans n'importe quel éditeur de texte basique (TextEdit sous macOS,
Bloc-notes sous Windows, gedit sous Linux, etc.), ou imprimez-le.

3. Au fur et à mesure, remplacez les [ ] par [x] devant les étapes que vous avez
effectuées, ou cochez-les à la main si vous imprimez ce document.

4. Quand une étape ne concerne pas votre installation (par exemple le format de
plugin AudioUnit « AU » si vous êtes sous Windows, ou BANK UTILITY si vous

testez sur un Matrix-6/6R et non un Matrix-1000), écrivez simplement

« sans objet » à côté, ou ignorez-la.

5. Notez vos observations dans la zone « Notes » de la section où le problème
apparaît — pas besoin de revenir en arrière plus tard.

6. Vous n'irez peut-être pas jusqu'au bout de ces tests : c'est normal. Les
premières sections (installation, démarrage, MIDI, édition de base) sont les plus importantes. Si vous vous arrêtez après le parcours rapide, vos retours restent très utiles au développeur !

7. Si quelque chose échoue, notez ce que vous voyez : le message exact à
l'écran aide beaucoup, n'hésitez pas à le recopier si vous le pouvez.

#### Deux rythmes possibles

- Le parcours RAPIDE (section 20) : environ 30 à 45 minutes, pour voir si
l'essentiel du logiciel fonctionne sur votre ordinateur.

- Le parcours COMPLET (section 21) : une session de tests plus longue, quand
vous avez le temps d'explorer davantage.

Une seule combinaison ordinateur + logiciel de MAO (DAW) + synthé Oberheim + Matrix-Control suffit déjà à être utile. Si vous pouvez tester sur plusieurs modèles de synthés, tant mieux — mais ce n'est pas obligatoire.

#### À propos des noms affichés à l'écran

L'interface graphique (GUI) du Matrix-Control est uniquement en anglais. Dans ce guide, les noms des écrans et boutons restent donc en anglais, comme vous les voyez : GETTING STARTED, PATCH EDIT, BANK UTILITY, etc. Le reste du texte est en français dans ce guide, pour mieux vous accompagner.

#### Repère rapide dans la fenêtre principale

Une fois le logiciel ouvert, la zone centrale s'organise à peu près ainsi :

```text
  +----------------------------------------------------------------------+
  |  Logo MATRIX-CONTROL    |   Ports MIDI (FROM SYNTH, TO SYNTH, etc.)  |
  +----------------------------------------------------------------------+
  |                         |                          |                 |
  |  PATCH EDIT             |  MATRIX MODULATION       |  MASTER EDIT    |
  |  - 10 modules de son :  |  (10 bus de modulation)  |  (Matrix-1000   |
  |    DCO, VCF, ENV, etc.  |                          |   seulement ;   |
  |  - 5 afficheurs :       |--------------------------|   grisée sur    |
  |    - 3 Enveloppes       |  PATCH MANAGER           |   Matrix-6/6R)  |
  |    - 1 Track Generator  |  - INTERNAL PATCHES      |                 |
  |    - 1 Patch Name       |  - COMPUTER PATCHES      |                 |
  |                         |  - BANK UTILITY          |                 |
  |                         |  - PATCH MUTATOR         |                 |
  +----------------------------------------------------------------------+
  |  Messages en bas de fenêtre               Badge DEVICE (type synthé) |
  +----------------------------------------------------------------------+
```

Vocabulaire utile (aligné sur le manuel utilisateur) :

- **Fenêtre** = la fenêtre principale du Matrix-Control, ou une fenêtre qui s'ouvre par-dessus (Settings, About)
- **Panneau** = grandes régions de la fenêtre principale (HEADER, BODY, FOOTER) — Settings n'est pas un panneau
- **Section** = grand bloc à l'intérieur du panneau BODY (PATCH EDIT, MATRIX MODULATION, PATCH MANAGER, MASTER EDIT)
- **Module** = bloc à l'intérieur d'une section (DCO 1, INTERNAL PATCHES, PATCH MUTATOR, MIDI, VIBRATO, MISC, BANK UTILITY, etc.)
- **Groupe** = sous-partie d'un module (BROWSER, MEMORY, STORAGE, etc.)
- **Bus de modulation** = une des dix lignes de MATRIX MODULATION
- **Dialogue** = petite boîte de confirmation ou de choix (Unsaved patch, RESET MASTER, alertes du Mutator, etc.)

Un manuel utilisateur plus détaillé peut être fourni avec le Matrix-Control ; ce guide de tests ne le remplace pas.

#### Contacter le développeur

Pour poser une question ou envoyer un rapport de tests, envoyez de préférence un mail à :

tensquaresoftware@gmail.com

Vous pouvez aussi ouvrir une « issue » sur le dépôt GitHub du projet si vous êtes à l'aise avec cet outil — ce n'est pas obligatoire. L'email suffit.

## 1. Ce dont vous aurez besoin pour tester le Matrix-Control

Cette section est une checklist « suis-je prêt ? ». Cochez seulement ce que vous avez sous la main. Les détails (quel Mac / PC, quelle version d'OS, quel DAW, quel synthé exact, etc.) se notent une seule fois dans la fiche de session (section 3).

### 1.1. Synthétiseur

Au moins un synthétiseur Oberheim compatible :

- [ ] Matrix-1000 : idéal pour couvrir presque tout le guide, y compris les banques et les réglages globaux
- [ ] Matrix-6 / Matrix-6R : très utile aussi, certaines zones du logiciel seront volontairement grisées — c'est normal

### 1.2. Ordinateur

- [ ] Un ordinateur équipé de l'un des systèmes d'exploitation supportés : macOS (Apple Silicon ou Intel), Windows 10 ou 11, ou Linux (Ubuntu 24.x / 26.x ou proche)

### 1.3. Accessoires

- [ ] Une interface MIDI (boîtier USB-MIDI, ou interface audio équipée de connecteurs MIDI au format DIN 5 broches), avec assez de câbles pour les DEUX sens (ordinateur ↔ synthé). Le branchement concret se fait en section 5.
- [ ] Un chemin audio pour entendre le synthé (sortie du synthé vers une entrée d'interface audio, table de mixage ou enceintes). Le Matrix-Control pilote uniquement le synthé en MIDI : il n'enregistre pas le son et ne le remplace pas.

### 1.4. Logiciels

- [ ] Soit un logiciel de MAO (DAW) pour tester le plugin (Ableton Live, Logic Pro, Cubase, Reason, Reaper, etc.), soit uniquement l'application autonome (Standalone) du Matrix-Control
- [ ] Le fichier ZIP du Matrix-Control fourni pour la bêta (il n'y a pas d'installateur : vous copierez les fichiers à la main — la section 4 explique comment)
- [ ] Un éditeur de texte basique pour cocher ce guide au fur et à mesure

## 2. Points importants avant de commencer

Ces points évitent de prendre pour des bugs des comportements voulus, ou des messages habituels sous macOS / Windows.

### 2.1. Distribution en fichier ZIP

Dans sa version bêta, le Matrix-Control est livré dans une archive ZIP, sans assistant d'installation. Vous extrayez le ZIP, puis vous copiez vous-même l'application et/ou les plugins aux endroits habituels de votre système (détail en section 4). Ce n'est pas un oubli : il n'y a pas encore d'assistant classique du type « Lancer > Suivant > Suivant > ... > Terminer ».

### 2.2. Messages de sécurité de macOS ou Windows

Les fichiers exécutables de la bêta ne passent pas encore par la certification payante d'Apple (« notarisation ») ni par la signature éditeur Microsoft Windows.

Résultat fréquent et NORMAL :

- sous macOS : avertissement du type « développeur non identifié » ;
- sous Windows : écran SmartScreen du type « Windows a protégé votre PC ».

Ce n'est en général PAS un virus ni un bug du plugin. La section 4 explique comment continuer en confiance (par exemple clic droit → Ouvrir sous macOS).

### 2.3. Écoute audio Standalone sous Windows (ASIO)

En mode Standalone (l'appli seule, hors DAW), le Matrix-Control peut écouter le retour audio de votre synthé via l'interface audio connectée à votre ordinateur. Sous Windows, cela passe souvent par le pilote audio du système (parfois appelé WASAPI).

À terme, le Matrix-Control devrait aussi proposer l'ASIO : un pilote très utilisé en MAO, en général plus réactif (latence plus faible). Pour l'instant, ASIO peut encore être absent des premières versions bêta : le développeur attend un accord de licence avec Steinberg, l'éditeur et propriétaire de cette technologie. Si vous ne voyez pas ASIO dans la liste audio, ce n'est donc pas un bug : notez-le, et vérifiez surtout que l'écoute marche avec le pilote système. L'essentiel reste d'entendre le synthé et de pouvoir l'éditer.

### 2.4. Protégez vos patches avant de démarrer les tests

Certaines étapes des tests demandent d'enregistrer un patch dans la mémoire du synthé (STORE) ou de générer des variations (PATCH MUTATOR). Si vous tenez à des patches déjà présents en mémoire dans votre Matrix-1000/6/6R, sauvegardez-les d'abord sur votre ordinateur une fois le logiciel installé (section 4) : module COMPUTER PATCHES, bouton SAVE AS, fichiers au format .syx — avant de faire des essais d'écriture dans le synthé.

### 2.5. Si STORE semble ne pas enregistrer

Sur Matrix-1000, une protection mémoire ou un verrouillage de banque (MEMORY PROTECT / BANK LOCK, sur la machine ou dans MASTER EDIT) peut empêcher l'écriture des patches.

Sur Matrix-6 / Matrix-6R, vérifiez surtout l'interrupteur Memory Protect à l'arrière de la machine (souvent sur Off pour pouvoir enregistrer), et éventuellement la protection par patch sur la façade. Ces réglages ne sont pas éditables dans le Matrix-Control (MASTER EDIT est grisé).

Vérifiez cela avant de conclure à un bug.

## 3. Fiche de votre session de tests

C'est ici que vous décrivez votre configuration de tests, une phrase ou quelques mots suffisent. Si vous changez d'ordinateur ou de synthé, merci de dupliquer ce fichier pour fournir une nouvelle fiche.

Le bilan global de votre session se trouve à la FIN du guide (section 22), après vos résultats de tests.

Date de la session (format JJ/MM/AAAA) :

Votre prénom et nom (ou pseudo) :

Votre adresse mail : (facultatif, afin que le développeur puisse vous répondre)

Système d'exploitation (avec la version si possible) : (ex. macOS 27.2.1, Windows 11 26H2, Ubuntu 26.04 LTS) :

Type d'ordinateur : (ex. MacBook Pro M5, PC portable Lenovo, PC de bureau ASUS, etc.)

Logiciel de MAO utilisé, avec sa version si vous la connaissez (ou « Standalone seulement » si vous n'ouvrez pas de DAW) :

Forme(s) testée(s) aujourd'hui :

- [ ] Application autonome (Standalone)
- [ ] Plugin VST3
- [ ] Plugin AU (macOS uniquement)

Synthé branché pour cette session :

- [ ] Matrix-1000
- [ ] Matrix-6
- [ ] Matrix-6R

EPROM / firmware du synthé (si vous le connaissez) (ex. EPROM Oberheim d'origine, Tauntek, Untergeek v1.21, etc.) :

Interface MIDI utilisée (modèle ou nom affiché sur l'ordinateur) :

Interface audio utilisée (modèle ou nom affiché) :

## 4. Installer le Matrix-Control

Le Matrix-Control est fourni sans assistant d'installation. Vous devez donc extraire l'archive ZIP, copier les fichiers exécutables aux endroits indiqués, puis éventuellement demander à votre DAW de rescanner la liste des plugins.

L'objectif ici : installer les fichiers et vérifier que le logiciel peut s'ouvrir (messages de sécurité compris).

Si GETTING STARTED s'ouvre déjà : choisissez CONFIGURE LATER, puis continuez l'installation.

### 4.1. Contenu de l'archive ZIP

Selon votre système, vous devriez trouver au moins :

- [ ] L'application autonome (Standalone)
- [ ] Le plugin VST3
- [ ] Sur macOS seulement : le plugin AU (sans objet sous Windows / Linux)
- [ ] De la documentation (manuel utilisateur et/ou ce guide de tests)

Si quelque chose manque, notez le nom exact du fichier ZIP et ce qui est absent.

### 4.2. Tester sous macOS

Cette section concerne macOS sur processeur Apple Silicon ou Intel.

Où copier les fichiers :

```text
Plugin AU   →  ~/Library/Audio/Plug-Ins/Components/
Plugin VST3 →  ~/Library/Audio/Plug-Ins/VST3/
Application →  /Applications/
               (ou à côté du ZIP, si vous préférez)
```

Étapes :

- [ ] Extraire le ZIP
- [ ] Copier Matrix-Control.component dans Components
- [ ] Copier Matrix-Control.vst3 dans VST3
- [ ] (Optionnel) Copier Matrix-Control.app dans Applications
- [ ] Après un téléchargement navigateur / GitHub, lever le « drapeau de quarantaine » (sinon macOS peut afficher que l'app ou le plugin est « endommagé » / « damaged »). Méthode pas à pas :

  1. Ouvrir l'application « Terminal » de macOS :
     - Raccourci le plus simple : touche Commande (⌘) + barre d'espace pour ouvrir Spotlight, taper Terminal, puis Entrée.
     - Autre chemin : Finder → Applications → Utilitaires → Terminal.
  2. Dans la fenêtre Terminal, taper exactement ceci, puis un espace (ne validez pas encore) :

     ```bash
     xattr -cr
     ```

  3. Dans le Finder, glisser-déposer sur la fenêtre Terminal chacun des fichiers que vous avez copiés (`Matrix-Control.app`, `Matrix-Control.vst3`, `Matrix-Control.component`). macOS écrit le chemin complet tout seul après chaque glisser-déposer. Séparez les fichiers par un espace. Vous pouvez omettre un fichier que vous n'avez pas installé.
  4. Quand la ligne ressemble à ceci (vos chemins peuvent différer) :

     ```bash
     xattr -cr /Applications/Matrix-Control.app \
               /Users/vous/Library/Audio/Plug-Ins/VST3/Matrix-Control.vst3 \
               /Users/vous/Library/Audio/Plug-Ins/Components/Matrix-Control.component
     ```

     Appuyez sur Entrée. En général, Terminal n'affiche rien de plus si la commande a réussi — c'est normal.
  5. Quitter Terminal : menu Terminal → Quitter Terminal, ou ⌘Q. Vous n'en aurez plus besoin pour la suite de ce guide.

  Variante sans glisser-déposer : copier la commande d'exemple ci-dessus (sélection à la souris, puis ⌘C), coller dans Terminal (⌘V), remplacer les chemins par les vôtres, puis Entrée. Sur succès, passer à l'étape 5.

- [ ] Premier lancement : si macOS dit « développeur non identifié », clic droit → Ouvrir, ou autoriser dans Réglages Système. C'est attendu pour cette bêta. Ce n'est pas un bug.
- [ ] Lancer le Standalone : la fenêtre s'ouvre. Si GETTING STARTED apparaît : CONFIGURE LATER
- [ ] Dans le DAW : rescanner si besoin ; le Matrix-Control apparaît en AU et/ou VST3
- [ ] Charger le plugin sur une piste instrument / MIDI

Ce qui est normal :

- l'appli et les plugins démarrent ;
- après xattr, plus de message « endommagé » / « damaged » en principe ;
- un avertissement « développeur non identifié » peut encore apparaître : clic droit → Ouvrir.

Notes personnelles :

### 4.3. Tester sous Windows

Cette section concerne Windows 10 ou Windows 11.

Où copier les fichiers :

```text
Plugin VST3 →  C:\Program Files\Common Files\VST3\
               (souvent réservé aux administrateurs)
Standalone  →  Matrix-Control.exe (où vous voulez)
```

Étapes :

- [ ] Extraire le ZIP
- [ ] Copier Matrix-Control.vst3 vers le dossier VST3 indiqué plus haut (`C:\Program Files\Common Files\VST3\`).

  Si Windows refuse la copie (message du type « vous devez être administrateur » ou « accès refusé »), ce n'est pas bloquant. Choisissez l'une de ces alternatives :

  **A.** Dossier VST3 de votre compte utilisateur — créez par exemple :

  ```text
  C:\Users\VotreNom\Documents\VST3\
  ```

  (remplacez VotreNom par le nom de votre session Windows), puis copiez Matrix-Control.vst3 dedans.

  **B.** Un dossier « plugins » déjà utilisé par votre DAW — certains logiciels (Ableton Live, Reaper, Cubase, etc.) ont leur propre dossier de plugins, ou permettent d'en ajouter un dans leurs préférences.

  Ensuite, dites au DAW où chercher : ouvrez les préférences / options du DAW, section Plugins / VST / VST3, ajoutez le dossier choisi (A ou B) s'il n'y est pas déjà, puis lancez un rescan (souvent un bouton du type « Rescan », « Scan », ou « Actualiser »). Sans cette étape, le Matrix-Control peut être bien copié sur le disque mais invisible dans la liste des instruments.

- [ ] Lancer Matrix-Control.exe (double-clic sur le fichier) : la fenêtre s'ouvre. Si GETTING STARTED apparaît : CONFIGURE LATER
- [ ] Si Windows affiche SmartScreen (« Windows a protégé votre PC ») :

  C'est attendu pour cette bêta : le fichier n'a pas encore la signature payante d'un éditeur Windows. Ce n'est en général PAS un virus.

  1. Sur l'écran bleu / sombre SmartScreen, cliquez « Informations complémentaires » (lien souvent en bas à gauche, facile à manquer).
  2. Un bouton « Exécuter quand même » apparaît : cliquez dessus.
  3. Si Windows demande une confirmation administrateur, acceptez.

  Si vous ne voyez que « Ne pas exécuter » sans le lien supplémentaire, agrandissez la fenêtre ou cherchez « Informations complémentaires » avant d'abandonner.

- [ ] Dans le DAW : rescanner les VST3 ; charger le Matrix-Control
- [ ] Sous Windows, un port MIDI est souvent réservé à une seule application. Si le Matrix-Control n'ouvre pas le port, fermez l'autre logiciel qui l'utilise, puis réessayez.

Ce qui est normal :

- Standalone et VST3 démarrent ;
- le plugin apparaît après un rescan dans le DAW ;
- SmartScreen peut réapparaître au premier lancement : même procédure.

Notes personnelles :

### 4.4. Tester sous Linux

Cette section concerne Linux, en pratique Ubuntu 24 / 26 ou une distribution proche.

Où copier les fichiers :

```text
Plugin VST3 →  ~/.vst3/
               (dossier « .vst3 » dans votre dossier personnel ; le point
               au début signifie qu'il est souvent masqué dans le
               gestionnaire de fichiers)
Standalone  →  binaire nommé Matrix-Control (ou proche) fourni dans le ZIP
               — placez-le où vous voulez (Bureau, Applications, etc.)
```

Étapes :

- [ ] Extraire le ZIP (clic droit → Extraire ici, ou double-clic selon votre gestionnaire d'archives)
- [ ] Créer le dossier du plugin s'il n'existe pas encore, puis y copier Matrix-Control.vst3.

  Méthode avec le gestionnaire de fichiers (Nautilus sous Ubuntu) :

  1. Ouvrez votre dossier personnel (Maison / Home).
  2. Affichez les fichiers cachés : Ctrl+H (ou menu Affichage → Afficher les fichiers cachés).
  3. Si le dossier « .vst3 » n'existe pas : clic droit → Nouveau dossier, nommez-le exactement `.vst3` (avec le point).
  4. Copiez Matrix-Control.vst3 depuis le ZIP extrait vers ce dossier `.vst3`.

  Variante en ligne de commande (si vous préférez) : ouvrir Terminal (touche Super / Windows, taper Terminal, Entrée), puis :

  ```bash
  mkdir -p ~/.vst3
  cp /chemin/vers/Matrix-Control.vst3 ~/.vst3/
  ```

  Remplacez `/chemin/vers/…` en glissant le fichier `.vst3` depuis le gestionnaire de fichiers vers la fenêtre Terminal après avoir tapé `cp` et un espace.

- [ ] Rendre le Standalone exécutable si Linux refuse de le lancer (« Permission denied », icône grisée, ou rien ne se passe).

  1. Ouvrir Terminal (comme ci-dessus).
  2. Taper exactement ceci, puis un espace (ne validez pas encore) :

     ```bash
     chmod +x
     ```

  3. Glisser-déposer le fichier Matrix-Control (Standalone) depuis le gestionnaire de fichiers vers Terminal : le chemin s'écrit tout seul.
  4. Appuyer sur Entrée. En général, rien ne s'affiche si la commande a réussi — c'est normal.
  5. Quitter Terminal : menu → Quitter, ou Ctrl+D puis fermer la fenêtre, ou fermer simplement la fenêtre.

- [ ] Lancer le Standalone :
  - double-clic sur le fichier dans le gestionnaire de fichiers, ou
  - depuis Terminal, pour voir d'éventuels messages d'erreur :

    ```bash
    /chemin/vers/Matrix-Control
    ```

    (même astuce : taper le début du chemin ou glisser le fichier, puis Entrée)

  Si GETTING STARTED apparaît : CONFIGURE LATER

- [ ] Dans le DAW : rescanner la liste des plugins VST3 ; le Matrix-Control doit apparaître. Chargez-le sur une piste instrument / MIDI.
- [ ] Ouvrir SETTINGS → AUDIO et noter ce qui est proposé (noms de périphériques, pilotes système type ALSA / PulseAudio / PipeWire, éventuellement JACK). Pas besoin de tout comprendre : l'idée est de savoir quel chemin audio Linux vous propose, pour le noter dans la fiche de session (section 3) si un problème audio apparaît plus tard.

Ce qui est normal :

- le Standalone et le VST3 démarrent ;
- aucun message bloquant du type « bibliothèque manquante » (shared library / `.so` introuvable) ;
- si un tel message apparaît quand même, copiez-le mot pour mot dans les notes — c'est utile pour le développeur.

Notes personnelles :

## 5. Connecter le synthétiseur

Branchez le matériel pour que le Matrix-Control puisse dialoguer avec le synthé et que vous entendiez les changements pendant les tests.

- [ ] Le synthé est allumé
- [ ] L'interface MIDI est allumée / reconnue par l'ordinateur
- [ ] Le câble MIDI de l'ordinateur vers le synthé est branché (MIDI OUT de l'interface → MIDI IN du synthé)
- [ ] Le câble MIDI du synthé vers l'ordinateur est branché (MIDI OUT du synthé → MIDI IN de l'interface). Les DEUX sens en même temps sont obligatoires.
- [ ] La sortie audio du synthé est branchée vers votre interface audio, table de mixage ou enceintes

Résultat attendu : synthé, MIDI (dans les deux sens) et écoute audio sont en place.

Notes personnelles :

## 6. Premier lancement du Matrix-Control

Lancez le Matrix-Control (ou rouvrez-le) et allez jusqu'au bout de l'assistant GETTING STARTED : skin (apparence), UI Scale (taille de l'interface), ports MIDI, clavier MIDI, et — en Standalone seulement — écoute audio.

Si GETTING STARTED ne s'ouvre pas : logo MATRIX-CONTROL → SETTINGS… → onglet GETTING STARTED → RUN SETUP AGAIN (ou laissez SETUP WIZARD sur SHOW WHEN INCOMPLETE).

### 6.1. Écran d'accueil

- [ ] La fenêtre GETTING STARTED s'ouvre (au lancement, ou après RUN SETUP AGAIN)
- [ ] CONTINUE fait passer à la suite
- [ ] (Optionnel) CONFIGURE LATER ferme sans tout marquer comme terminé

### 6.2. Étapes communes (plugin et Standalone)

- [ ] STEP 1 / USER INTERFACE : essayez UI SCALE (taille) et SKIN (apparence). La fenêtre se met à jour.
- [ ] STEP 2 / SYNTH COMMUNICATION : choisissez SYNTH FROM / SYNTH TO, regardez le badge DEVICE, confirmez ou choisissez EPROM TYPE.
- [ ] STEP 3 / MIDI KEYBOARD : - En plugin : les notes MIDI viennent du DAW ; pas de liste KEYBOARD FROM. L'étape se termine avec FINISH. - En Standalone : choisissez KEYBOARD FROM, ou SKIP si pas encore de clavier maître. Ensuite vient l'audio.

### 6.3. Standalone seulement

Cette étape correspond à STEP 4 / AUDIO dans l'assistant GETTING STARTED (elle n'apparaît pas en plugin).

- [ ] Choisissez votre interface audio
- [ ] Choisissez les canaux d'écoute du retour synthé (SYNTH FROM dans l'onglet AUDIO — ce n'est PAS le même « SYNTH FROM » que le port MIDI)
- [ ] FINISH ferme l'assistant

Résultat attendu : vous terminez GETTING STARTED jusqu'à FINISH. Les étapes « terminées » ne forcent pas à elles seules la détection du synthé.

Notes personnelles :

## 7. Vérifier la communication MIDI

C'est l'étape la plus importante côté logiciel. Tant que le synthé n'est pas reconnu sur le réseau MIDI, une grande partie de l'éditeur reste verrouillée — c'est voulu, pour éviter d'envoyer des changements « à l'aveugle ».

### 7.1. Choisir ou confirmer les ports MIDI

- [ ] Réglez ou vérifiez SYNTH FROM et SYNTH TO (bandeau FROM SYNTH / TO SYNTH, ou SETTINGS → MIDI & DEVICE)
- [ ] Quand du MIDI circule, les voyants à côté des listes clignotent
- [ ] En bas à droite, le badge DEVICE affiche Matrix-1000, Matrix-6, etc. (un Matrix-6R peut apparaître comme Matrix-6 : notez-le si c'est le cas)
- [ ] Si NOT CONNECTED : suivez le message en bas de fenêtre (câbles — section 5 —, ports, éteindre/rallumer le synthé)

### 7.2. Édition verrouillée puis débloquée

- [ ] Sans détection correcte, l'édition reste verrouillée (normal)
- [ ] Une fois le synthé reconnu, PATCH EDIT et les zones utiles se débloquent

### 7.3. Sous Windows : port MIDI déjà réquisitionné

- [ ] Si le port refuse de s'ouvrir : fermez l'autre application / DAW qui l'occupe, ou utilisez un port MIDI virtuel, puis réessayez

Résultat attendu : DEVICE reconnu, édition possible, pas de cascade d'erreurs si les ports sont bons.

### 7.4. Version du Matrix-Control et EPROM TYPE

Quand le synthé est reconnu, le Matrix-Control l'interroge en MIDI pour en déduire le modèle et le type d'EPROM / firmware. Notez ce qui s'affiche — surtout si cela diffère de ce que vous pensiez avoir.

Sur Matrix-1000 comme sur Matrix-6 / 6R : regardez SETTINGS → MIDI & DEVICE (liste EPROM TYPE, et éventuellement le badge DEVICE). Un Matrix-6R peut apparaître comme Matrix-6 ; les libellés d'EPROM proposés peuvent différer selon la machine. Si la liste indique UNKNOWN, notez-le aussi.

Version testée du Matrix-Control : (logo MATRIX-CONTROL → ABOUT…, ou message / page du téléchargement)

EPROM TYPE affiché :

Notes personnelles :

## 8. Tester l'application autonome et le plugin

La Matrix-Control peut s'utiliser de deux façons :

- application autonome (Standalone), sans DAW ;
- plugin dans le DAW (VST3 sous macOS, Windows et Linux ; AU en plus sous macOS).

### 8.1. En plugin

- [ ] Le plugin se charge sans planter
- [ ] Les notes arrivent via la piste du DAW (KEYBOARD FROM = HOST)
- [ ] Le son du synthé revient sur une piste audio séparée (le plugin ne génère pas l'audio du synthé)
- [ ] GETTING STARTED n'impose pas d'étape Audio
- [ ] SETTINGS… est accessible (logo MATRIX-CONTROL ou raccourci)

### 8.2. En Standalone

- [ ] La fenêtre autonome s'ouvre ; SETTINGS → AUDIO est disponible
- [ ] Vous entendez le retour du synthé via l'interface choisie
- [ ] KEYBOARD FROM est sélectionnable (ou SKIP dans Getting Started)
- [ ] Si INPUT GAIN / monitoring apparaît dans le bandeau : il réagit

### 8.3. Écoute audio sous Windows (Standalone)

Rappel ASIO : voir section 2 (accord de licence Steinberg en cours ; ASIO peut manquer dans les premières bêtas).

- [ ] Avec le pilote système (souvent WASAPI) : l'entrée est audible et la latence est acceptable pour éditer
- [ ] Si ASIO apparaît dans la liste : le périphérique est sélectionnable et l'audio fonctionne
- [ ] Si ASIO n'apparaît pas : notez-le et continuez avec le pilote système — ce n'est pas forcément un échec pour cette première bêta

Résultat attendu : plugin et Standalone utilisables ; au minimum, écoute Standalone via les pilotes système.

Notes personnelles :

## 9. Ableton Live et image floue sous Windows

Uniquement sous Windows + Ableton Live + plugin VST3 du Matrix-Control. (Sous macOS, cette option n'existe pas : section sans objet.)

Problème connu sous Windows : Live propose souvent « Auto-Scale Plugin Window ». Quand c'est activé, l'interface du Matrix-Control peut apparaître floue dans le plugin, quelle que soit UI Scale. Le Standalone reste net sur la même machine.

Comment désactiver l'option :

- [ ] Chargez d'abord le Matrix-Control sur une piste dans Live
- [ ] Clic droit sur la barre de titre du device (en bas, dans la vue Device) → désactiver « Auto-Scale Plugin Window »
- [ ] Rechargez le device
- [ ] Vérifiez que l'interface VST3 est nette
- [ ] Comparez avec le Standalone (doit rester net)

Ne cherchez pas cette option dans le navigateur (Browser) à gauche : un clic droit sur Matrix-Control dans la liste des plug-ins ne montre en général que le regroupement et les couleurs, pas Auto-Scale.

Notes personnelles :

## 10. Éditer un patch

Si quelque chose vous surprend pendant l'édition (Unison, mode clavier, STORE, etc.), consultez aussi la section 18 (Points connus à vérifier) — ce ne sont pas toujours des bugs.

Une fois le synthé détecté, chargez un patch via le module INTERNAL PATCHES (dans la section PATCH MANAGER — détail section 12), puis travaillez dans la section PATCH EDIT.

Chaque mouvement à l'écran est envoyé au synthé : vous devez entendre le changement sur la machine.

Modules typiques dans PATCH EDIT :

DCO 1 / DCO 2     oscillateurs

- VCF/VCA           filtre et volume
- FM/TRACK          FM + Track Generator
- RAMP/PORTAMENTO   rampes, portamento, mode clavier
- ENV 1 / 2 / 3     enveloppes
- LFO 1 / LFO 2     oscillateurs lents
- PATCH NAME        nom du patch (8 caractères)

### 10.1. Gestes représentatifs

- [ ] Dans le module DCO 1, changez un paramètre (forme d'onde, désaccord…) : le son change
- [ ] Dans VCF/VCA, bougez la fréquence de coupure ou la résonance : le son change
- [ ] Sur une enveloppe (ENV), déplacez un point à la souris : le son ou l'affichage réagit
- [ ] Dans FM/TRACK, déplacez un point du Track Generator à la souris : le son ou l'affichage réagit

### 10.2. Boutons I / C / P sur un module

Sur plusieurs modules, l'en-tête propose :

I = Init   (remettre le module a des valeurs par défaut)

C = Copy   (copier vers le presse-papiers)

P = Paste  (coller depuis le presse-papiers)

- [ ] I réinitialise le module
- [ ] C puis P sur un module compatible fonctionne
- [ ] Sur un module incompatible, P est grisé ; un clic peut afficher un message en bas de fenêtre

### 10.3. Annuler / rétablir

- [ ] Annuler (Cmd+Z sous macOS, Ctrl+Z sous Windows/Linux) revient en arrière
- [ ] Rétablir (Cmd+Shift+Z / Ctrl+Shift+Z) restaure
- [ ] Charger un autre patch efface cet historique d'annulation

### 10.4. Nom du patch

Le nom s'affiche et s'édite dans le champ PATCH NAME (8 caractères).

- [ ] L'afficheur PATCH NAME montre le nom actuel (8 caractères)
- [ ] Double-clic pour renommer quand c'est autorisé : - Matrix-1000 : possible surtout sur les banques 0 et 1 (mémoire réinscriptible) ; souvent impossible sur les banques 2 a 9 (usine) - Matrix-6/6R : en général possible (pas de banques ROM du même type)
- [ ] Sur une banque ou le renommage est refusé, le double-clic ne doit pas ouvrir d'édition (ou le champ reste bloqué) — notez ce que vous voyez
- [ ] Le nom se met aussi a jour au chargement d'un patch ou après certaines actions du PATCH MUTATOR

Résultat attendu : édition audible, annulation cohérente, pas de fermeture intempestive.

Notes personnelles :

## 11. Éditer la matrice de modulation

C'est le « câblage » interne du synthé : on relie une source (LFO, enveloppe, vélocité…) à une destination (filtre, hauteur…). Dix bus (lignes), numéros 0 à 9.

Préparez : synthé détecté, un patch chargé.

- [ ] Sur un bus, réglez SOURCE, AMOUNT et DESTINATION (autre que NONE)
- [ ] Jouez une note : la modulation s'entend / se sent
- [ ] Le bouton I sur un bus remet ce bus à zéro
- [ ] Si C / P existent au niveau de la section MATRIX MODULATION : copier / coller un bus (ou la section) se comporte de façon claire ; sinon notez « sans objet »
- [ ] Réordonnez deux bus en glissant le numéro # : départ gris, arrivée rouge ; les contenus s'échangent, les numéros restent 0 a 9
- [ ] Annuler après ce réordonnancement revient d'une étape

Résultat attendu : bus éditables, réordonnancement stable.

Notes personnelles :

## 12. Gérer les patches dans la mémoire du synthé

Dans PATCH MANAGER, le module INTERNAL PATCHES pilote la mémoire du synthétiseur (banque / numéro selon la machine). Les fichiers sur l'ordinateur sont traités à part (section 13).

### 12.1. Navigation et STORE

- [ ] Les boutons < et > changent de patch ; le Matrix-Control relit le patch depuis le synthé
- [ ] Double-clic sur le numéro de patch : la saisie directe 0 à 99 fonctionne
- [ ] STORE enregistre dans l'emplacement courant (Matrix-1000 : surtout banques 0 et 1, mémoire réinscriptible)
- [ ] Matrix-1000, banques 2 à 9 (mémoire d'usine) : INIT, PASTE et STORE sont grisés — c'est normal

### 12.2. INIT / COPY / PASTE

Ici, INIT / COPY / PASTE portent sur le **patch entier** (pas seulement un module comme en section 10.2). Travaillez sur une banque réinscriptible (Matrix-1000 : 0 ou 1) ou sur Matrix-6/6R.

- [ ] INIT : le patch est remis au modèle ; PATCH NAME peut afficher quelque chose du type `* INIT *` — notez ce que vous voyez
- [ ] COPY puis PASTE vers un autre emplacement : le son et l'écran restent cohérents
- [ ] Après PASTE, vous pouvez STORE pour figer dans la mémoire du synthé

### 12.3. Dialogue « Unsaved patch »

- [ ] Forcer le dialogue : modifier un paramètre, puis changer de numéro de patch (ou de banque) **sans** STORE ni SAVE — le dialogue Unsaved patch apparaît (sauf si UNSAVED STATE = NEVER WARN dans Settings)
- [ ] Tester Cancel : vous restez sur le patch courant, modifications conservées à l'écran
- [ ] Refaire : Discard — les modifications sont abandonnées, la navigation continue
- [ ] Refaire : Store et/ou Save / Save As selon les boutons proposés — les choix sont clairs, pas de perte silencieuse
- [ ] Après Discard / Store / Save : la suite de la navigation est cohérente

Résultat attendu : navigation fluide ; STORE / INIT / COPY / PASTE OK sur mémoire réinscriptible ; banques d'usine protégées ; Unsaved patch compréhensible.

Notes personnelles :

## 13. Gérer les patches dans des fichiers informatiques

Dans PATCH MANAGER, le module COMPUTER PATCHES gère les fichiers `.syx` sur le disque (un patch par fichier). La mémoire du synthé est traitée à part (section 12).

### 13.1. OPEN / SAVE / parcours

- [ ] OPEN : choisissez un dossier de patches
- [ ] Après OPEN : s'il y a un mélange de fichiers, un message du type « N valid, M invalid » (ou équivalent) peut apparaître en bas de fenêtre — notez le texte exact
- [ ] SAVE AS : créez un nouveau `.syx` ; SAVE : enregistrez dans le fichier déjà associé
- [ ] Parcourir la liste avec < / > et recharger un `.syx`
- [ ] Charger un `.syx` externe (format classique / communauté) puis SAVE AS : le nouveau fichier se recharge correctement
- [ ] Si « Patch name mismatch » (nom interne ≠ nom de fichier) : Internal / Filename / Cancel se comportent de façon compréhensible
- [ ] (Optionnel) SETTINGS : les politiques de noms fichier vs SysEx / ASK se comportent comme prévu — une passe suffit

### 13.2. Glisser-déposer des fichiers sur la fenêtre

- [ ] Glisser un `.syx` patch sur la fenêtre : un titre temporaire du type LOAD PATCH apparaît ; le drop charge le patch (Unsaved patch si besoin)
- [ ] Glisser plusieurs `.syx` ou un dossier de patches : la sélection est acceptée ou un message clair s'affiche
- [ ] Glisser un fichier non Matrix : le fichier est refusé, un message s'affiche en bas de fenêtre, pas de crash
- [ ] (Matrix-1000) Glisser un fichier Master valide (`.syx` / `.m1km`) : l'overlay LOAD MASTER apparaît et le chargement fonctionne ; plusieurs fichiers Master d'un coup sont refusés ou un message clair s'affiche

Résultat attendu : fichiers `.syx` lisibles et enregistrables ; drag-drop prévisible ; pas d'écrasement surprise.

Notes personnelles :

## 14. Créer des variations de patch

Le PATCH MUTATOR (dans PATCH MANAGER) crée des variations aléatoires contrôlées du patch courant — pratique pour explorer des timbres.

Préparez : synthé détecté, patch chargé. Sauvegardez d'abord si le son de départ vous est précieux.

Commutateurs de modules (choisir ce qui entre dans la mutation) :

- **D1** = DCO 1
- **D2** = DCO 2
- **F/A** = filtre / VCA (VCF/VCA)
- **F/T** = FM / Track Generator
- **R/P** = Ramp / Portamento
- **E1** / **E2** / **E3** = enveloppes 1, 2, 3
- **L1** / **L2** = LFO 1 et 2
- **MM** = matrice de modulation (MATRIX MODULATION)

Réglages supplémentaires :

- **AMOUNT** — intensité de la variation
- **RANDOM** — degré d'aléatoire
- **MODE** — caractère de la mutation (ex. KINDRED, DRIFT, WARP, WILD)
- **PITCH** — traitement des hauteurs (ex. KEEP, CONSONANT, DISSONANT, FREE ; éventuellement une fenêtre d'octaves)

Boutons principaux :

- **MUTATE** — crée une nouvelle variation et l'envoie au synthé
- **RETRY** — autre tirage à partir du même point de départ
- **HISTORY** + `<` / `>` — parcourt les variations de la session
- **C** (Compare) — compare au départ et verrouille le reste de l'édition tant qu'il est actif
- **D** (Delete) — supprime l'entrée sélectionnée dans l'historique
- **F** (Flush) — vide l'historique de la session
- **E** (Export) — exporte la session en fichiers `.syx`

### 14.1. Mutation de base

- [ ] Au moins un commutateur (D1, D2, F/A, …, MM) est activé
- [ ] Réglez AMOUNT et RANDOM autrement que les défauts, puis MUTATE : variation audible / visible envoyée au synthé
- [ ] RETRY propose une autre variante
- [ ] HISTORY < / > parcourt la session
- [ ] C (Compare) verrouille l'édition ; un message en bas de fenêtre le rappelle
- [ ] Avec Compare actif : Undo/Redo patch et édition restent bloqués (rappel section 10.3)
- [ ] Désactiver Compare restaure l'édition
- [ ] D et F se comportent comme attendu (suppression / vidage) — avec prudence
- [ ] E exporte la session ; vous pouvez la rouvrir via COMPUTER PATCHES → OPEN
- [ ] Les dialogues (historique plein, mutations non exportées, dossier déjà existant) proposent des choix clairs, sans perte silencieuse

### 14.2. MODE et PITCH

- [ ] Changer MODE (ex. KINDRED puis WILD) : MUTATE donne un caractère différent — décrivez ce que vous entendez
- [ ] Changer PITCH (KEEP vs FREE / CONSONANT, etc.) : la hauteur des variations est cohérente avec le choix

Résultat attendu : mutations audibles, Compare qui bloque bien, MODE/PITCH perceptibles, export rechargeable.

Notes personnelles :

## 15. Gérer les banques (Matrix-1000 uniquement)

Le Matrix-1000 organise ses patches en 10 banques (0 à 9) de 100 patches. Le module BANK UTILITY (dans PATCH MANAGER) sert à changer de banque **et** à transférer des banques entières (COPY / PASTE / IMPORT / EXPORT).

Sur Matrix-6/6R : ce module est entièrement grisé (pas de banques). C'est voulu.

### 15.1. Sélection de banque

- [ ] Matrix-6/6R : BANK UTILITY est grisé (sans objet sur Matrix-1000)
- [ ] Matrix-1000 : boutons 0 à 9 changent la banque ; le patch courant est relu
- [ ] Sur les banques 0 et 1, l'écriture est possible (STORE, etc.)
- [ ] Sur les banques 2 à 9, INIT / PASTE / STORE restent en lecture seule dans INTERNAL PATCHES
- [ ] Changer de banque avec des modifications non sauvées suit la même logique Unsaved patch qu'un changement de patch

### 15.2. COPY / PASTE / IMPORT / EXPORT

**Attention :** ces actions peuvent écraser toute une banque. Sauvegardez d'abord vos sons importants (EXPORT ou fichiers `.syx`). Travaillez de préférence sur les banques 0 ou 1.

- [ ] EXPORT de la banque sélectionnée : un dossier est créé (environ 100 fichiers), un message de succès s'affiche ; Cancel reste propre si proposé
- [ ] IMPORT vers banque 0 ou 1 : une confirmation apparaît, puis une barre / fenêtre de progression, avec un résultat du type found / valid / imported (notez le texte)
- [ ] IMPORT ou PASTE vers banque 2–9 est refusé ou grisé, avec un message clair
- [ ] COPY banque A → PASTE banque B (toutes deux en RAM, ex. 0 → 1) : une confirmation et une progression s'affichent ; Cancel restaure ou interrompt proprement
- [ ] Pendant un transfert : l'UI reste cohérente (pas de double-clic accidentel qui casse tout) ; un message « in progress » ou un titre de progression est visible

Résultat attendu : changement de banque fiable ; transferts contrôlés sur RAM ; banques d'usine protégées ; zone grisée sur Matrix-6/6R.

Notes personnelles :

## 16. Réglages globaux « Master » (Matrix-1000 uniquement)

MASTER EDIT regroupe des réglages pour tout le synthétiseur (canal MIDI, vibrato global, accordage…), pas pour un seul patch.

Modules : MIDI, VIBRATO, MISC.

Sur Matrix-6/6R, toute cette colonne est grisée — c'est voulu. La gestion du Master sur ces modèles est très différente de celle du Matrix-1000, et le Matrix-Control ne l'édite pas encore. Une version ultérieure pourra éventuellement en proposer le support ; pour cette bêta, considérez MASTER EDIT comme réservé au Matrix-1000.

### 16.1. Édition MASTER EDIT

- [ ] Matrix-6/6R : MASTER EDIT est grisé (sans objet sur Matrix-1000)
- [ ] Matrix-1000 : les modules MIDI, VIBRATO et MISC sont visibles et éditables
- [ ] Changez le CHANNEL MIDI avec prudence : comportement cohérent
- [ ] Bouton I sur un module : dialogue RESET MASTER MODULE? Reset / Cancel fonctionnent
- [ ] Un réglage master influence le comportement général, quel que soit le patch chargé

### 16.2. Fichiers Master

Vous pouvez charger ou enregistrer un Master depuis Settings, ou en glissant un fichier sur la fenêtre.

- [ ] SETTINGS → MASTER (ou UTILITY Master) : LOAD / SAVE AS d'un Master fonctionne ; INIT global demande une confirmation
- [ ] (Optionnel) INIT TEMPLATE Master : SAVE AS INIT / DELETE fonctionne
- [ ] Drag-drop d'un fichier Master valide : l'overlay LOAD MASTER apparaît et le chargement fonctionne ; plusieurs fichiers Master d'un coup sont refusés ou un message clair s'affiche

Résultat attendu : édition possible sur Matrix-1000 seulement ; Load/Save Master sans crash.

Notes personnelles :

## 17. Préférences dans la fenêtre Settings

Ces préférences (logo MATRIX-CONTROL → SETTINGS…) s'ouvrent dans la fenêtre Settings et modifient le comportement au quotidien. Une passe suffit.

- [ ] UNSAVED STATE : ALWAYS WARN vs NEVER WARN — le dialogue Unsaved patch apparaît ou non comme attendu
- [ ] CONTEXTUAL HELP est activé : le survol d'un contrôle affiche le texte HELP en bas de fenêtre
- [ ] INIT TEMPLATE (PATCH) : SAVE AS INIT, puis INIT dans INTERNAL PATCHES utilise le modèle
- [ ] Plugin seulement : HARDWARE LATENCY est réglable dans MIDI & DEVICE (pas besoin de mesurer la latence)
- [ ] Logo → ABOUT… : la version est lisible ; Échap ferme ABOUT et/ou SETTINGS

Résultat attendu : les options Settings se comportent comme leur libellé ; pas de plantage en ouvrant/fermant Settings.

Notes personnelles :

## 18. Points connus à vérifier

Certains comportements surprennent. Confirmez ce que vous voyez.

- [ ] Unison (surtout Matrix-1000) : dans le module RAMP/PORTAMENTO (et réglages Unison liés, y compris côté MASTER EDIT / MISC selon votre machine), activez / réglez Unison. Le rendu peut être contre-intuitif : ce que l'on croit commander à l'écran ne correspond pas toujours à ce que l'on entend. Cela vient surtout de la façon dont le Matrix-1000 gère l'Unison côté machine (plusieurs notions liées : mode clavier, Unison master, etc.), pas d'un simple bug d'affichage du Matrix-Control. Decrivez ce que vous voyez et entendez.
- [ ] Keyboard Mode : toujours dans RAMP/PORTAMENTO, changez le mode clavier. Un comportement bizarre côté firmware du synthé est connu ; il est souvent contourné ou résolu avec une EPROM Untergeek v1.21. Notez votre EPROM et ce qui se passe (notes collées, mode qui ne « prend » pas, etc.).
- [ ] Protections mémoire : Matrix-1000 — MEMORY PROTECT / BANK LOCK ; Matrix-6/6R — Memory Protect (arrière) et protect de patch sur la façade. Si actifs, STORE peut sembler cassé — vérifiez avant de signaler un bug.
- [ ] Sur Matrix-6/6R : MASTER EDIT et BANK UTILITY sont grisés (voulu)
- [ ] Les avertissements macOS / Windows au lancement sont souvent normaux (sections 2 et 3)
- [ ] Un Matrix-6R peut s'afficher comme Matrix-6
- [ ] ASIO peut manquer (section 2) — testez l'écoute avec le pilote système

Notes personnelles :

## 19. Comment signaler un problème

Adresse privilégiée :

tensquaresoftware@gmail.com

(GitHub possible si vous êtes à l'aise ; l'e-mail suffit.)

Le plus simple : joignez ce fichier de guide (fiche de session section 3 déjà remplie) et décrivez le problème. Sinon, indiquez au minimum :

1. Titre court : quoi + où
(ex. « STORE grisé sur banque 0, Matrix-1000 »)

2. Version du Matrix-Control + version de CE guide
3. Système (nom + version) et type d'ordinateur
— ou « voir fiche section 3 jointe »

4. Standalone ou plugin (AU / VST3) + DAW et sa version
5. Synthé + EPROM / firmware si connu
6. Étapes pour reproduire, numérotées
7. Ce que vous attendiez, et ce qui s'est passé
8. Toujours, ou seulement parfois ?
9. Texte exact du message en bas de fenêtre ou du dialogue
10. Si possible : une capture d'écran

Inutile d'envoyer mots de passe, licences, ou une copie entière du disque.


## 20. Parcours rapide de tests

Cochez ici ce que vous avez déjà fait dans les sections précédentes. Si une ligne ne s'applique pas : « sans objet ».

- [ ] 1. L'installation ZIP est faite et les messages de sécurité sont gérés (section 4)
- [ ] 2. Le synthé est branché (MIDI dans les deux sens + audio) (section 5)
- [ ] 3. Le Standalone s'ouvre
- [ ] 4. Le plugin se charge dans le DAW (après rescan si besoin)
- [ ] 5. GETTING STARTED est terminé jusqu'à FINISH (section 6)
- [ ] 6. MIDI : badge DEVICE reconnaît le synthé (section 7)
- [ ] 7. Le cadre d'usage Standalone et/ou plugin est OK (section 8)
- [ ] 8. Windows + Live : Auto-Scale si l'UI est floue (section 9 ; sinon sans objet)
- [ ] 9. Un patch est chargé via INTERNAL PATCHES ; le cutoff est modifié ; le son change
- [ ] 10. STORE fonctionne sur un emplacement réinscriptible (Matrix-1000 banque 0 ou 1, ou numéro sur Matrix-6/6R)
- [ ] 11. Un bus MATRIX MODULATION change le son
- [ ] 12. MUTATE a été lancé une fois ; Compare on puis off fonctionne
- [ ] 13. Matrix-1000 : BANK UTILITY — au moins un EXPORT (ou COPY) sans écraser une banque pleine d'originaux ; sinon sans objet
- [ ] 14. Matrix-1000 : un module MASTER EDIT est utilisable (sans objet sur Matrix-6/6R)
- [ ] 15. Matrix-6/6R : BANK UTILITY et MASTER EDIT sont grisés (sans objet si vous n'avez que le 1000)
- [ ] 16. En Standalone, vous entendez le retour audio du synthé
- [ ] 17. Quitter et relancer : ports MIDI, apparence, Getting Started se comportent de façon raisonnable

Vous pouvez vous arrêter ici et passer au bilan (section 22) : c'est déjà un très bon retour.

## 21. Parcours complet de tests

Si vous avez plus de temps, ajoutez ce qui suit (toujours sur votre installation : OS + DAW + synthé).

- [ ] GETTING STARTED : parcours jusqu'à FINISH ; aussi CONFIGURE LATER / rappel / RUN SETUP AGAIN si vous explorez ces chemins
- [ ] PATCH EDIT : I / C / P, enveloppes, Track Generator, annulations plus poussées ; renommage PATCH NAME selon banque / machine
- [ ] MATRIX MODULATION : réordonnancement + annulation (+ C/P de section si présent)
- [ ] INTERNAL PATCHES (section 12) : INIT + COPY/PASTE + Unsaved patch forcé
- [ ] COMPUTER PATCHES (section 13) : OPEN / SAVE AS / noms divergents + drag-drop `.syx`
- [ ] PATCH MUTATOR (section 14) : AMOUNT / RANDOM, MODE, PITCH, RETRY, HISTORY, D, F, E, dialogues
- [ ] BANK UTILITY (section 15) : EXPORT + IMPORT (ou COPY/PASTE) sur banque 0 ou 1 ; refus sur ROM (sans objet hors Matrix-1000)
- [ ] MASTER EDIT (section 16) : MIDI / VIBRATO / MISC + reset ; Load/Save Master si possible (sans objet hors Matrix-1000)
- [ ] SETTINGS (section 17) : UNSAVED STATE + CONTEXTUAL HELP (+ INIT TEMPLATE et HARDWARE LATENCY si vous voulez)
- [ ] Plugin vs Standalone : clavier via le DAW vs KEYBOARD FROM
- [ ] Audio Standalone avec et sans ASIO si les deux existent (sinon notez ce qui est disponible — rappel section 2)
- [ ] Points de la section 18 (Unison, Keyboard Mode, etc.)
- [ ] Un signalement d'essai pour vérifier que la liste de la section 19 est suffisante

Si vous testez plusieurs systèmes (une fiche de session par machine) :

- [ ] macOS Apple Silicon : AU + VST3 + Standalone
- [ ] macOS Intel
- [ ] Windows 10
- [ ] Windows 11
- [ ] Ubuntu 24.x
- [ ] Ubuntu 26.x (ou autre)

## 22. Bilan de votre session de tests

À remplir à la fin (ou lorsque vous arrêtez) :

- [ ] Parcours rapide : tout s'est bien passé
- [ ] Parcours complet : tout s'est bien passé
- [ ] J'ai trouvé un problème bloquant
- [ ] J'ai surtout pris des notes / questions

Ce qui vous a plu :

Ce qui a bloqué ou surpris :

Ce qui mériterait d'être plus clair (logiciel ou ce guide) :

================================================================================ FIN DU GUIDE — Matrix-Control, bêta-tests Merci infiniment pour votre aide ! :) Guillaume | Ten Square Software — tensquaresoftware@gmail.com

================================================================================
