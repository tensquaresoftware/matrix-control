# Manuel utilisateur Matrix-Control — conventions de rédaction (SSOT)

**Public de ce document :** agents BMad, en particulier **Paige** (Technical Writer), et tout agent qui met à jour `Documentation/Users/Manuals/Matrix-Control-Manuel-Utilisateur.md`.

**Public du manuel lui-même :** utilisateurs débutants en MAO qui découvrent aussi le Matrix-1000 et le Matrix-6/6R — **pas** des développeurs.

**Fichier cible (unique) :** `Documentation/Users/Manuals/Matrix-Control-Manuel-Utilisateur.md`

**Langue du manuel :** français.  
**Langue de l’UI du produit :** anglais (libellés d’écran conservés tels quels, souvent en `backticks`).

Ce document capture les exigences de Guillaume pour un manuel **utilisable**, **honnête** (aligné sur le code) et **agréable à lire**. À appliquer à chaque évolution du produit : le manuel évolue **en synchro** avec le code.

---

## 1. Intention et portée

1. Produire un manuel **complet et utilisable de bout en bout**, pas un plan ni un squelette.
2. Organiser le texte comme un **parcours d’usage** (brancher, ouvrir, éditer un patch, sauvegarder, banques, mutateur, réglages…), **jamais** comme une architecture logicielle.
3. Le **code et l’UI** (`Source/`) sont la source de vérité. PRD / UX / stories servent seulement à vérifier la cohérence — **ne rien inventer** côté produit.
4. Ne pas recopier de prose technique depuis `Documentation/Devs/`.
5. Prévoir des **emplacements pour captures d’écran** (légendes / placeholders), sans inventer d’images.
6. Annoncer dès l’introduction qu’un **lexique** des termes et acronymes se trouve en fin de manuel ; terminer par ce lexique (définitions courtes et concrètes).

---

## 2. Public et ton

### Public

- Débutants MAO + découverte du matériel Oberheim Matrix
- Peuvent ne pas connaître SysEx, DCO, LFO, etc. → renvoyer au lexique, pas surcharger le parcours

### Ton (manuel « vrai », pas simpliste)

- Français **fluide**, phrases un peu **développées** — comme un manuel utilisateur professionnel
- Verbes précis (`sélectionner`, `transmettre`, `réinitialiser`, `parcourir`…) plutôt que constructions pauvres autour de *avoir* / *être*
- **Ne pas infantiliser** le lecteur : vocabulaire accessible ≠ vocabulaire basique
- Éviter le style télégraphique ou « checklist développeur »
- Partir d’**objets et gestes concrets** (ex. « fichiers ZIP », « brancher MIDI OUT → MIDI IN ») plutôt que de processus de fabrication
- **Décrire ce qui existe** : rôle des commandes et de l’écran. **Éviter** les phrases « ce n’est pas… / plus de… / pas dans… » issues de corrections internes agent↔Guillaume — le lecteur n’a pas vécu cet échange
- **Modèle de fluidité :** la section `GETTING STARTED` du manuel FR — intention d’abord, parcours court, détail d’état / cas limites allégés ou renvoyés plus loin. Éviter l’effet « usine à gaz » (mécaniques internes, exceptions empilées) surtout avant le cœur de l’édition

### Phrase-test utile

> Un musicien qui découvre le Matrix pourrait-il suivre ce paragraphe sans se demander « c’est quoi un build / un dump / un NumberBox » ?

Si non → réécrire.

---

## 3. Interdits (orientation développeur)

Ne **pas** parler au lecteur de :

- compilation, build local, presets CMake, chemins de dépôt
- « release GitHub » comme jargon d’installeur interne (parler plutôt de **téléchargement** de ZIP / paquets prêts à l’emploi)
- chemins vers `Documentation/Devs/…`
- détails d’implémentation (octets, Device Inquiry brut, NumberBox, inquiry auto, APVTS, etc.) dans le parcours

**Installation :** l’utilisateur télécharge des **exécutables** (application autonome + plugins) et la doc — il ne compile rien. Décrire le geste d’install, pas la chaîne de build.

Référence de style pour l’intro d’installation (à conserver / imiter) :

> Le Matrix-Control se télécharge sous la forme de fichiers ZIP comprenant une application autonome et les plugins compatibles avec votre système, accompagnés de la documentation utilisateur.

---

## 4. Vocabulaire produit (non négociable)

| Préférer | Éviter / ne plus faire |
|----------|-------------------------|
| **synthé** | « Matrix » seul pour désigner la machine |
| **Matrix-1000**, **Matrix-6**, **Matrix-6R**, **Matrix-6/6R** | `Matrix-6 / 6R` (espaces autour du slash) |
| **Matrix-Control** / logo **`MATRIX-CONTROL`** (majuscules comme dans la GUI) | « logo Matrix-Control » sans casse GUI |
| **le / du / au Matrix-Control** (article comme pour le Matrix-1000 : *À quoi sert le…*, *fenêtre du…*) | « À quoi sert Matrix-Control », « fenêtre de Matrix-Control » (sans article) |
| **patch** (vocabulaire Oberheim) | « son » quand on parle d’un programme mémorisé |
| **un / le DAW** | une / la DAW |
| **interface audio** | carte son |
| **application autonome** — introduire une seule fois **application autonome (Standalone)** en début de manuel, puis uniquement **application autonome** | répéter `Standalone` dans le corps du manuel |
| Couple des deux formats : toujours **application autonome** puis **version plugin** (ou **version plugin du Matrix-Control** si le contexte l’exige) | « En plugin », « dans le plugin » seuls ; « plugin et Standalone » ; « plugin ou application autonome » en tête |
| **commutateur** (Mutator modules) | « case » |
| **bus de modulation** | « ligne » de modulation |
| Titres de section : **verbe d’action** quand c’est naturel, sinon formulation claire ; **éviter** les parenthèses finales « d’appoint » | Titres du type *… (version plugin)*, *En haut (HEADER)*, *Parcourir (BROWSER)* |
| Exception — H2 avec rappel GUI en **MAJUSCULES** entre parenthèses : uniquement les titres déjà validés (*GETTING STARTED*, *PATCH EDIT*, *MATRIX MODULATION*, *INTERNAL PATCHES*, *BANK UTILITY*, *COMPUTER PATCHES*, *PATCH MUTATOR*, *MASTER EDIT*, *SETTINGS*) | Étendre ce motif à d’autres H2 / H3 « pour coller un libellé » |
| Sous-titres H3 de groupes / commandes GUI : préférer le **libellé seul** (`BROWSER`, `IMPORT`, `HEADER`…) ou un titre français **sans** parenthèses finales | Français + (`GUI`) en queue de titre |
| Matrix-1000 : **non programmable en façade** (intérêt historique du Matrix-Control) ; Matrix-6/6R : programmables en façade | « Au lieu de tout manipuler depuis la façade » comme si cela valait pour tous les modèles |

Conserver **Matrix-Control**, les noms de modèles, et les **citations exactes** des messages UI anglais (ex. *power-cycle the Matrix*).

---

## 5. Terminologie UI (métier du manuel)

Utiliser de façon cohérente :

| Terme | Désigne |
|-------|---------|
| **Panneau** | Grandes régions de la fenêtre principale : `HEADER`, `BODY`, `FOOTER` (schéma ASCII §2 du manuel). **Disposition chrome** (`HEADER` / `FOOTER` / Settings / assistant) : vérifier le panneau dans `Source/` (ex. `HeaderPanel`) et les libellés dans `PluginDisplayNames.h`. Le guide de tests bêta **n’est pas** la référence de layout — il peut être en retard. |
| **`HEADER` (état actuel)** | Logo + Undo/Redo + **LEDs** d’activité MIDI (`FROM SYNTH` / `TO SYNTH` / `FROM KEYBOARD`) +, en application autonome, `INPUT GAIN` / monitoring. Ports MIDI : `SETTINGS…` → `MIDI & DEVICE` et assistant `GETTING STARTED`. Dans le **manuel**, formuler positivement (voyants d’activité ; ports dans Settings) — pas de négations du type « ce ne sont pas des listes ». |
| **`FOOTER` (état actuel)** | Messages / aide à gauche ; badge `DEVICE` à droite. Pas de sélecteurs MIDI. |
| **Section** | Grands blocs dans le `BODY` : `PATCH EDIT`, `MATRIX MODULATION`, `PATCH MANAGER`, `MASTER EDIT` |
| **Module** | Blocs fonctionnels : `DCO 1`, `BANK UTILITY`, `PATCH MUTATOR`, etc. |
| **Bus de modulation** | Une des dix voies de `MATRIX MODULATION` |
| **Groupe** | Sous-partie d’un module : `BROWSER`, `MEMORY`, `STORAGE`, etc. (même sens que « Zone » dans d’anciennes notes) |
| **Afficheur** | Graphiques / panneaux : enveloppes et Track Generator (**interactifs**) ; `PATCH NAME` (**renommable par double-clic** quand le contexte l’autorise — pas toujours éditable) |

Contrôles GUI mentionnés dans le texte : toujours en **`backticks`** (`STORE`, `<` / `>`, `MUTATE`, etc.).

---

## 6. Structure du contenu

### Parcours type (ordre logique)

Aligné sur la table des matières du manuel FR (ne pas renuméroter « pour le plaisir ») :

1. Oberheim Matrix-1000, Matrix-6 et Matrix-6R (famille d’abord : points communs, écarts utiles)  
2. Qu’est-ce que le Matrix-Control ? (inclut plan ASCII + légende `HEADER` / `BODY` / `FOOTER` ; transition : refonte du Matrix-1000 Editor → Matrix-1000 non programmable en façade, utile aussi Matrix-6/6R)  
3. Équipements nécessaires pour utiliser le Matrix-Control  
4. Installer et lancer le Matrix-Control  
5. Assistant de premier lancement (`GETTING STARTED`)  
6. Connecter le synthé et établir la communication MIDI  
7. Éditer un patch (`PATCH EDIT`)  
8. Éditer la matrice de modulation (`MATRIX MODULATION`)  
9. Gérer les patches dans la mémoire du synthé (`INTERNAL PATCHES`) — inclure le focus de navigation Internal ↔ Computer  
10. Gérer les banques du Matrix-1000 (`BANK UTILITY`)  
11. Gérer les patches sur l’ordinateur (`COMPUTER PATCHES`) — pas de doublon `STORE` (déjà en §9)  
12. Créer des variations de patch (`PATCH MUTATOR`)  
13. Gérer les paramètres globaux du synthé (`MASTER EDIT`)  
14. Sélectionner un Skin et ajuster la taille de la fenêtre (`SETTINGS`)  
15. Utiliser l’application autonome et la version plugin  
16. Guide de dépannage  
17. Lexique

### Ce qu’il ne faut plus faire

- **Ne pas** centraliser toutes les boîtes de dialogue dans une section « Messages et dialogues » : les décrire **dans la section du scénario concerné** (meilleur flow de lecture)
- **Ne pas** maintenir une section catalogue « Ce qui n’est pas disponible dans cette version » (les absences se traitent au fil du récit si vraiment utile, sans inventaire produit)

### Captures d’écran

Dossier : `Documentation/Users/Manuals/Screenshots/`.

Nommage des fichiers (ASCII, stable) :

`NN-english-slug.png` — `NN` = section number in the manual; **English** slug (project primary language), e.g. `02-main-window-matrix-1000.png`, `04-matrix-control-in-daw.png`, `05-getting-started-intro.png`, `06-midi-audio-cabling.png`.

Cible d’affichage principale : **Typora** (édition) et **PDF** exporté depuis Typora (musiciens). Le `.md` source reste lisible tel quel pour qui préfère Markdown.

**Séparateurs :** ne **pas** mettre de lignes horizontales Markdown (`---`) entre les sections `##` — GitHub (et souvent Typora) souligne déjà les H2 ; doubler avec un `---` alourdit le rendu. Réserver `---` au **frontmatter YAML** uniquement.

Format type — **placeholder** (tant que l’image n’est pas prête) :

```markdown
> **Capture d’écran — À placer ici**  
> *…légende provisoire…*
```

Format type — **image placée** (centrage + italique ; pas de préfixe « Légende : »).

**Typora :** ne pas mélanger `![…](…)` Markdown *à l’intérieur* d’un `<p>` — Typora ferme le paragraphe avant l’image et affiche un `</p>` orphelin. Utiliser un bloc HTML pur :

```html
<div align="center">
<img src="Screenshots/NN-english-slug.png" alt="Same caption as below, for accessibility." />
<br />
<em>Caption in French, italic, no “Légende :” prefix.</em>
</div>
```

---

## 7. Contenu par zone (règles de fond)

### `PATCH EDIT` — Modules de synthèse

- Expliquer le **rôle** des briques (DCO = formes d’onde de base ; VCF = sculpture fréquentielle du timbre ; etc.)
- **Ne pas** lister tous les paramètres de chaque module
- Conserver que les **afficheurs d’enveloppes** (et le Track Generator) sont **éditables à la souris**
- `PATCH NAME` : renommable par double-clic **quand le contexte l’autorise** (fichier ordinateur ; mémoire machine selon banque / Hardware Names / Compare — ne pas écrire « toujours » ni « jamais » éditable) ; après `INIT`, marqueur `* INIT *` à remplacer avant `STORE` / `SAVE` / `SAVE AS`
- Computer Patches : `OPEN` / liste / glisser-déposer acceptent `.syx` **et** `.m1kp` ; `SAVE` / `SAVE AS` écrivent toujours un `.syx` (jamais réécrire le `.m1kp` source) ; après un `.m1kp`, confirmer l’écrasement d’un `.syx` homonyme si présent

### Coller (`P`)

- Le collage vers une destination incompatible est **empêché** (bouton grisé)
- Un clic sur le bouton grisé peut afficher un message en bas de fenêtre
- **Ne pas** écrire comme si l’utilisateur collait puis « échouait »

### `MATRIX MODULATION`

- Présenter comme le **centre nerveux** du synthé : c’est là qu’on donne vie aux patches via les modulations — d’où le nom Matrix-1000 / Matrix-6/6R
- Parler de **bus** (pas de lignes)
- Étoffer SOURCE / AMOUNT / DESTINATION (menus, intensité, paramètre modulé en temps réel) — ton manuel, pas énumération sèche
- Réordonnancement : **cliquer-glisser vertical** sur le numéro de bus ; point de départ **gris**, point d’arrivée **rouge**

### `MASTER EDIT`

- Philosophie : **paramètres globaux** qui affectent **tous les patches**
- **Ne pas** lister tous les paramètres ; indiquer les modules (`MIDI`, `VIBRATO`, `MISC`) et l’intention
- Expliquer clairement `MEMORY PROTECT` / `BANK LOCK` (et le cas Matrix-6/6R : interrupteur arrière, `MASTER EDIT` grisé) — `STORE` « qui semble cassé » n’est souvent pas un bug

### Patch Mutator

- `AMOUNT` / `RANDOM` : plage **1–100 %** (pas de consigne du type « au-dessus de 0 »)
- Modules ciblés = **commutateurs**, pas « cases »
- Condition utile : au moins un commutateur de module activé
- Rappel de sauvegarde avant les gestes qui écrivent dans le synthé (`MUTATE`, etc.)

### Prérequis (§2) et câblage (§6)

- Inclure un **chemin audio** (sortie synthé → interface / table / enceintes) dès les prérequis — pas seulement le MIDI
- Préciser **deux câbles MIDI** (deux sens) ; DAW **optionnel** si application autonome seule
- Câblage type : MIDI **et** audio ; idéalement avant / pendant le premier lancement

### Plateformes

- macOS (Apple Silicon ou Intel) : application autonome, AU, VST3
- Windows 10 / 11 : application autonome, VST3
- Linux (Ubuntu 24.x / 26.x ou proche) : application autonome, VST3
- (Ne plus marquer Linux comme indisponible)

---

## 8. Typographie des listes (français)

Listes à puces (et listes numérotées d’énumération) :

1. **Majuscule en tête** de chaque item — comme pour n’importe quelle phrase
2. **Pas de ponctuation finale** (ni point, ni point-virgule) sur chaque item

Exemple :

```markdown
- Aucun signe à la fin de chaque tiret
- Approche épurée et moderne
- Convient bien aux listes d'énumération simple
```

Les phrases hors liste gardent une ponctuation normale.

---

## 9. Exactitude et évolutions

1. Avant d’écrire ou de mettre à jour un passage : **vérifier le comportement réel** dans `Source/` (et les libellés dans `PluginDisplayNames.h` / l’UI).
2. Pour tout ce qui touche au **chrome** (`HEADER`, `FOOTER`, Settings, `GETTING STARTED`) : **ne pas** s’appuyer sur le guide bêta ni sur une formulation déjà présente dans le manuel sans relecture code — c’est la cause typique des descriptions périmées (ex. anciennes listes MIDI dans le bandeau).
3. Si le code change (nouveau module, dialogue, libellé) : **mettre à jour le manuel dans le même mouvement** que la livraison fonctionnelle, ou immédiatement après. Si le guide bêta décrit encore l’ancienne UI, le corriger aussi — ou au minimum ne pas le traiter comme SSOT de layout.
4. En cas de doute entre PRD / docs Users et code : **le code gagne** ; le manuel décrit l’existant.
5. Les messages UI cités doivent rester fidèles à l’anglais affiché.

---

## 10. Checklist rapide (avant de considérer le manuel à jour)

- [ ] Parcours d’usage, pas architecture
- [ ] Aucun jargon de build / dépôt / compile
- [ ] `synthé` à la place de « Matrix » seul ; `Matrix-6/6R` orthographié correctement
- [ ] Logo `MATRIX-CONTROL` ; contrôles en backticks
- [ ] Section / Module / Bus / Groupe / Afficheur utilisés correctement
- [ ] `HEADER` = LEDs MIDI (pas de listes de ports) ; ports = `SETTINGS…` → `MIDI & DEVICE` / assistant seulement
- [ ] Chrome vérifié dans `Source/` (pas recopié depuis le guide bêta)
- [ ] Pas de phrases « ce n’est pas… / pas dans… » de correction interne : décrire le rôle réel des commandes
- [ ] « patch » plutôt que « son » (contexte Oberheim)
- [ ] « un DAW » ; « interface audio » ; « application autonome » après une seule intro Standalone
- [ ] Modules de synthèse et MASTER EDIT sans inventaires de paramètres
- [ ] Matrice = centre nerveux + bus + prose développée + drag gris/rouge
- [ ] Dialogues dans le scénario concerné
- [ ] Listes : majuscule en tête de chaque item, sans ponctuation finale
- [ ] Lexique en fin de document, annoncé en intro
- [ ] Placeholders de captures présents là où une image aidera
- [ ] Pas de `---` thématiques entre sections `##` (YAML frontmatter excepté)
- [ ] Aligné sur le code actuel

---

## 11. Fichiers liés

| Fichier | Rôle |
|---------|------|
| `Documentation/Users/Manuals/Matrix-Control-Manuel-Utilisateur.md` | Manuel utilisateur FR (livrable) |
| `_bmad/custom/manuel-utilisateur-redaction-fr.md` | Ce SSOT (conventions de rédaction) |
| `_bmad/custom/bmad-agent-tech-writer.toml` | Charge ce SSOT pour Paige à l’activation |
| `Source/Shared/Definitions/PluginDisplayNames.h` | Libellés UI (référence) |

---

*Document interne Matrix-Control / Ten Square Software — à maintenir quand les conventions de rédaction du manuel évoluent.*
