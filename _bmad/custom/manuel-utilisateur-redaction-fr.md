# Manuel utilisateur Matrix-Control — conventions de rédaction (SSOT)

**Public de ce document :** agents BMad, en particulier **Paige** (Technical Writer), et tout agent qui met à jour `Documentation/User/manuel-utilisateur.md`.

**Public du manuel lui-même :** utilisateurs débutants en MAO qui découvrent aussi le Matrix-1000 et le Matrix-6/6R — **pas** des développeurs.

**Fichier cible (unique) :** `Documentation/User/manuel-utilisateur.md`

**Langue du manuel :** français.  
**Langue de l’UI du produit :** anglais (libellés d’écran conservés tels quels, souvent en `backticks`).

Ce document capture les exigences de Guillaume pour un manuel **utilisable**, **honnête** (aligné sur le code) et **agréable à lire**. À appliquer à chaque évolution du produit : le manuel évolue **en synchro** avec le code.

---

## 1. Intention et portée

1. Produire un manuel **complet et utilisable de bout en bout**, pas un plan ni un squelette.
2. Organiser le texte comme un **parcours d’usage** (brancher, ouvrir, éditer un patch, sauvegarder, banques, mutateur, réglages…), **jamais** comme une architecture logicielle.
3. Le **code et l’UI** (`Source/`) sont la source de vérité. PRD / UX / stories servent seulement à vérifier la cohérence — **ne rien inventer** côté produit.
4. Ne pas recopier de prose technique depuis `Documentation/Development/`.
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

### Phrase-test utile

> Un musicien qui découvre le Matrix pourrait-il suivre ce paragraphe sans se demander « c’est quoi un build / un dump / un NumberBox » ?

Si non → réécrire.

---

## 3. Interdits (orientation développeur)

Ne **pas** parler au lecteur de :

- compilation, build local, presets CMake, chemins de dépôt
- « release GitHub » comme jargon d’installateur interne (parler plutôt de **téléchargement** de ZIP / paquets prêts à l’emploi)
- chemins vers `Documentation/Development/…`
- détails d’implémentation (octets, Device Inquiry brut, NumberBox, inquiry auto, APVTS, etc.) dans le parcours

**Installation :** l’utilisateur télécharge des **exécutables** (Standalone + plugins) et la doc — il ne compile rien. Décrire le geste d’install, pas la chaîne de build.

Référence de style pour l’intro d’installation (à conserver / imiter) :

> Matrix-Control se télécharge sous la forme de fichiers ZIP comprenant une application autonome et les plugins compatibles avec votre système, accompagnés de la documentation utilisateur.

---

## 4. Vocabulaire produit (non négociable)

| Préférer | Éviter / ne plus faire |
|----------|-------------------------|
| **synthé** | « Matrix » seul pour désigner la machine |
| **Matrix-1000**, **Matrix-6**, **Matrix-6R**, **Matrix-6/6R** | `Matrix-6 / 6R` (espaces autour du slash) |
| **Matrix-Control** / logo **`MATRIX-CONTROL`** (majuscules comme dans la GUI) | « logo Matrix-Control » sans casse GUI |
| **patch** (vocabulaire Oberheim) | « son » quand on parle d’un programme mémorisé |
| **un / le DAW** | une / la DAW |
| **interface audio** | carte son |
| **commutateur** (Mutator modules) | « case » |
| **bus de modulation** | « ligne » de modulation |
| Titre section besoins : **Ce dont vous aurez besoin** | « Ce qu’il vous faut » |

Conserver **Matrix-Control**, les noms de modèles, et les **citations exactes** des messages UI anglais (ex. *power-cycle the Matrix*).

---

## 5. Terminologie UI (métier du manuel)

Utiliser de façon cohérente :

| Terme | Désigne |
|-------|---------|
| **Section** | Grands blocs : `PATCH EDIT`, `MATRIX MODULATION`, `PATCH MANAGER`, `MASTER EDIT` |
| **Module** | Blocs fonctionnels : `DCO 1`, `BANK UTILITY`, `PATCH MUTATOR`, etc. |
| **Bus de modulation** | Une des dix voies de `MATRIX MODULATION` |
| **Zone** | Sous-ensembles : `BROWSER`, `MEMORY`, `STORAGE`, etc. |
| **Afficheur** | Graphiques / panneaux : enveloppes et Track Generator (**interactifs**) ; `PATCH NAME` (**lecture seule**) |

Contrôles GUI mentionnés dans le texte : toujours en **`backticks`** (`STORE`, `<` / `>`, `MUTATE`, etc.).

---

## 6. Structure du contenu

### Parcours type (ordre logique)

1. À quoi sert Matrix-Control  
2. Ce dont vous aurez besoin  
3. Matrix-1000 vs Matrix-6/6R  
4. Installer et ouvrir  
5. Brancher MIDI / détection  
6. Tour de l’écran  
7. Éditer un patch (`PATCH EDIT`)  
8. Matrice de modulation  
9. Patches internes  
10. Banques (M-1000)  
11. Sauvegarde synthé / ordinateur  
12. Patch Mutator  
13. Master Edit  
14. Apparence / Settings  
15. Standalone vs plugin  
16. Dépannage  
17. Lexique  

### Ce qu’il ne faut plus faire

- **Ne pas** centraliser toutes les boîtes de dialogue dans une section « Messages et dialogues » : les décrire **dans la section du scénario concerné** (meilleur flow de lecture)
- **Ne pas** maintenir une section catalogue « Ce qui n’est pas disponible dans cette version » (les absences se traitent au fil du récit si vraiment utile, sans inventaire produit)

### Captures d’écran

Format type :

```markdown
> **Capture d’écran — À placer ici**  
> *Légende :* …
```

---

## 7. Contenu par zone (règles de fond)

### `PATCH EDIT` — Modules de synthèse

- Expliquer le **rôle** des briques (DCO = formes d’onde de base ; VCF = sculpture fréquentielle du timbre ; etc.)
- **Ne pas** lister tous les paramètres de chaque module
- Conserver que les **afficheurs d’enveloppes** (et le Track Generator) sont **éditables à la souris**
- `PATCH NAME` : lecture seule à l’écran

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

### Patch Mutator

- `AMOUNT` / `RANDOM` : plage **1–100 %** (pas de consigne du type « au-dessus de 0 »)
- Modules ciblés = **commutateurs**, pas « cases »
- Condition utile : au moins un commutateur de module activé

### Plateformes

- macOS : AU, VST3, Standalone  
- Windows / Linux : VST3, Standalone  
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
2. Si le code change (nouveau module, dialogue, libellé) : **mettre à jour le manuel dans le même mouvement** que la livraison fonctionnelle, ou immédiatement après.
3. En cas de doute entre PRD et code : **le code gagne** ; le manuel décrit l’existant.
4. Les messages UI cités doivent rester fidèles à l’anglais affiché.

---

## 10. Checklist rapide (avant de considérer le manuel à jour)

- [ ] Parcours d’usage, pas architecture
- [ ] Aucun jargon de build / dépôt / compile
- [ ] `synthé` à la place de « Matrix » seul ; `Matrix-6/6R` orthographié correctement
- [ ] Logo `MATRIX-CONTROL` ; contrôles en backticks
- [ ] Section / Module / Bus / Zone / Afficheur utilisés correctement
- [ ] « patch » plutôt que « son » (contexte Oberheim)
- [ ] « un DAW » ; « interface audio »
- [ ] Modules de synthèse et MASTER EDIT sans inventaires de paramètres
- [ ] Matrice = centre nerveux + bus + prose développée + drag gris/rouge
- [ ] Dialogues dans le scénario concerné
- [ ] Listes : majuscule en tête de chaque item, sans ponctuation finale
- [ ] Lexique en fin de document, annoncé en intro
- [ ] Placeholders de captures présents là où une image aidera
- [ ] Aligné sur le code actuel

---

## 11. Fichiers liés

| Fichier | Rôle |
|---------|------|
| `Documentation/User/manuel-utilisateur.md` | Manuel utilisateur (livrable) |
| `_bmad/custom/manuel-utilisateur-redaction-fr.md` | Ce SSOT (conventions de rédaction) |
| `_bmad/custom/bmad-agent-tech-writer.toml` | Charge ce SSOT pour Paige à l’activation |
| `Source/Shared/Definitions/PluginDisplayNames.h` | Libellés UI (référence) |

---

*Document interne Matrix-Control / Ten Square Software — à maintenir quand les conventions de rédaction du manuel évoluent.*
