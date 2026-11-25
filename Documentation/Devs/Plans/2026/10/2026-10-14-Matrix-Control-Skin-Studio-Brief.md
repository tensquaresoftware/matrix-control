# Brief produit — Matrix-Control Skin Studio

**Date :** 2026-10-14  
**Statut :** point de départ greenfield (à injecter dans un nouveau projet BMad)  
**Langue du brief :** français (clair)  
**Projet outil :** Matrix-Control Skin Studio  
**Projet consommateur :** Matrix-Control (plugin JUCE)

---

## 1. En une phrase

Créer un éditeur web léger pour concevoir, calibrer et exporter des skins Matrix-Control à partir d’une palette maîtrisée, puis réinjecter ces choix dans les sources du plugin via des fichiers JSON — en profitant du chantier pour clarifier et assainir les concepts de skin / look / couleurs dans Matrix-Control.

---

## 2. Noms du projet outil

| Usage | Nom |
|---|---|
| Nom en clair (description GitHub, docs) | **Matrix-Control Skin Studio** |
| Nom du dépôt Git | `matrix-control-skin-studio` |
| Dossier local de développement | `Matrix-Control-Skin-Studio` (sans espaces) |

Ce brief est rédigé aujourd’hui dans le dépôt Matrix-Control pour servir de **mémoire et de point de départ**. Le pilotage BMad (PRD, architecture, epics, stories, implémentation) se fera dans le **projet séparé** Skin Studio.

---

## 3. Pourquoi ce projet existe

### Contexte produit

Matrix-Control livre déjà deux skins symboliques :

- **Black** — esprit Matrix-1000 noir
- **Cream** — esprit Matrix-1000 crème / blanc cassé

Dans les deux, sliders et combos sont habillés en **verts** (clin d’œil Matrix-12 / Xpander).

Pour la v1 du plugin, on souhaite :

1. **Affiner** Black et Cream (contrastes, équilibre Cream vs Black).
2. Les **renommer** pour harmoniser la famille :
   - `Black/Green`
   - `Cream/Green`
3. **Ajouter** trois skins :
   - `Black/Duo` — base Black ; accents bleus (zone Patch) et oranges (zone Master) ; LEDs header éventuellement jaunes ou vertes
   - `Cream/Duo` — même idée sur fond crème
   - `Monochrome` — base Black ; accents en gris ; LEDs en gris

### Pourquoi Figma ne suffit plus

Jusqu’ici, une maquette Figma complète (rectangles, contours, textes, palette restreinte) permettait de juger l’esthétique globale. Avec **plusieurs skins** à calibrer, et le besoin de lier chaque propriété d’objet à une **entrée de palette nommée**, Figma devient trop lourd et trop peu structuré pour l’aller-retour vers le code.

### Pourquoi pas seulement « dans le plugin »

Un mode d’édition live dans Matrix-Control serait idéal pour la fidélité, mais trop risqué / trop coûteux avant / pendant l’alpha. Skin Studio reste un **outil de design hors process audio**, avec validation finale dans le vrai plugin.

---

## 4. Lien aller / retour avec Matrix-Control

Le projet n’est pas un gadget isolé. C’est une **boucle** en quatre temps.

```text
Matrix-Control (état actuel)
        │
        ▼
[1] Collecte — inventaire couleurs, objets, propriétés, états, looks
        │
        ▼
[2] Refonte conceptuelle — modèle clair : palette → tokens → skins → injection
        │
        ▼
[3] Skin Studio (web) — simulation + édition + export JSON
        │
        ▼
[4] Matrix-Control (sources) — mise en conformité du modèle + skins finaux
```

### Étape 1 — Collecte depuis Matrix-Control (forme actuelle)

Repérer et documenter ce qui existe déjà, sans encore tout réécrire :

- **Palette partagée** (`ColourChart` et constantes `k…`)
- **Associations skin** (aujourd’hui surtout `blackVariant` / `creamVariant` sur des éléments nommés)
- **Identifiants de couleur** utilisés au runtime (`SkinColourId`, etc.)
- **Looks** des widgets (structures passées aux contrôles : fond, piste, texte, bordure focus, …)
- **États** réellement peints : normal, hover, focus, disabled, on/off, variantes « button-like », etc.
- **Usages d’alpha** :
  - raccourcis pour assombrir / atténuer une couleur pleine → candidats à remplacement par RVB solides
  - vraies superpositions (film de verrouillage, overlay de dialogue, intensité LED, …) → **hors scope** Skin Studio

Livrable de cette étape : un inventaire structuré (liste d’objets, propriétés, états, couleurs) qui alimente le modèle de données de Skin Studio **et** la future refonte Matrix-Control.

### Étape 2 — Approche plus structurée (concepts à clarifier)

Aujourd’hui le modèle « 2 variantes » (Black / Cream) ne scale pas vers 5 skins ni vers des contributeurs externes plus tard.

Objectifs de clarification (côté Matrix-Control, guidés par ce qu’on apprend dans Skin Studio) :

| Concept | Rôle attendu |
|---|---|
| **Palette** | Ensemble fermé de couleurs nommées (`kGreen4`, `kBrandBlue`, …) avec code `#RRGGBB` opaque |
| **Token / élément** | Propriété colorée d’un type d’objet pour un état donné (ex. slider → value bar → enabled) |
| **Skin** | Jeu de mappings : chaque token pointe vers **une** entrée de palette |
| **Look** | Paquet de couleurs (et éventuellement polices) injecté dans un widget générique au moment de sa construction |
| **Orchestrateur** | Seul endroit qui connaît les zones produit (Patch vs Master, Header, …) et choisit quel look / quelles couleurs injecter selon le skin actif |

Principe non négociable pour Duo :

- Les widgets restent **génériques** : ils ne connaissent pas « Patch » ni « Master ».
- L’orchestrateur qui construit l’UI injecte les bonnes couleurs selon le skin et le contexte.

Cette étape peut avancer en parallèle de Skin Studio : le studio **force** à nommer proprement les tokens ; Matrix-Control **adopte** ensuite ce vocabulaire.

### Étape 3 — Simulation dans Skin Studio

Éditeur web (HTML/CSS/JS) qui permet de :

- voir une **maquette d’ensemble** du Matrix-Control (réalisme type Figma : rectangles, contours, textes, densités — pas un clone pixel-perfect du rendu JUCE) ;
- régler le **UI Scale** (comme dans le plugin) et **naviguer** dans la vue (scroll X/Y, cliquer-glisser, flèches clavier) quand la maquette dépasse la zone visible ;
- éditer la **palette** ;
- associer chaque propriété / état d’un type d’objet à une couleur de palette ;
- prévisualiser en temps réel ;
- gérer plusieurs skins (créer, renommer, dupliquer, supprimer) ;
- exporter un (ou des) fichier(s) **JSON**.

### Étape 4 — Passation JSON → Matrix-Control

Les JSON exportés servent de **contrat** pour un chantier BMad dans Matrix-Control :

1. Mettre les sources en conformité avec le modèle clarifié (palette, tokens, skins, injection par orchestrateur).
2. Remplacer les alphas « raccourci d’assombrissement » par des RVB solides issus de la palette (quand le fond est opaque et connu).
3. Implémenter les cinq skins livrables v1 :
   - `Black/Green`
   - `Cream/Green`
   - `Black/Duo`
   - `Cream/Duo`
   - `Monochrome`
4. Valider **dans le vrai plugin** (contrastes écran, DAW, macOS / Windows si besoin).

Le JSON n’est pas « magiquement appliqué » : un agent / développeur BMad dans Matrix-Control l’utilise comme spécification machine-readable pour modifier le C++.

---

## 5. Vision produit de Skin Studio

### À quoi ça ressemble

Fenêtre navigateur en deux grandes zones :

**Gauche — Vue GUI**

- Représentation d’ensemble du Matrix-Control, assez fidèle pour juger l’esthétique globale (comme la maquette Figma actuelle).
- Pas d’exigence de simulation parfaite du moteur de dessin JUCE.
- Clic sur un objet → sélection du **type** d’objet dans la barre latérale (tous les objets du même type se mettent à jour ensemble).
- **UI Scale** (comme dans Matrix-Control) : `50%`, `75%`, `100%` (Default), `125%`, `150%`, `175%`, `200%`. Le choix agrandit ou réduit la maquette dans la zone gauche ; il ne change pas les couleurs ni le mapping.
- **Pan / scroll** : à `100%` (échelle la plus utilisée) — et a fortiori aux échelles plus grandes — la zone gauche ne suffit en général pas à montrer toute la GUI d’un coup. La vue doit donc défiler en **X et Y**. Navigation attendue :
  - barres de défilement (ou équivalent) ;
  - **cliquer-glisser** pour déplacer la vue (pan) ;
  - **flèches clavier** haut / bas / gauche / droite pour naviguer quand la vue a le focus.

**Droite — Barre latérale** (trois blocs)

1. **Haut — Palette**  
   Liste scrollable : petit carré + nom de constante (`k…`) + hex `#RRGGBB`.  
   CRUD : créer / renommer / dupliquer / supprimer une couleur.

2. **Centre — Picker**  
   Sélecteur type Photoshop (teinte verticale + saturation/luminosité) + sliders R/V/B + champ hex copiable / collable.

3. **Bas — Skin + objet**  
   - Combo de sélection du skin.  
   - Type d’objet sélectionné (par clic GUI ou par combo).  
   - Combo **State** (Normal, Hover, Focus, Disabled, On, Off, … selon l’objet).  
   - Pour chaque propriété (noms alignés sur le code / le modèle cible) : label + combo vers une couleur de palette.

### Boucle d’édition temps réel

- Modifier une **couleur de palette** → tous les objets qui l’utilisent changent tout de suite dans la vue.
- Modifier le **mapping** d’une propriété d’un type d’objet → tous les exemplaires de ce type changent tout de suite.

### « Where used » (à prototyper)

Quand une couleur de palette est sélectionnée :

- surligner dans la maquette les objets qui l’utilisent ;
- lister en bas : type d’objet → propriété → état.

L’UI exacte se peaufinera en voyant le prototype.

### Persistance

- Sauvegarde automatique (ex. `localStorage`) pour retrouver palette + skins au prochain lancement.
- Préférence de vue (UI Scale, éventuellement position de pan) : raisonnable à mémoriser en local avec le reste de la session ; **pas** une donnée d’export skin JSON.
- Import / export fichier JSON (ne pas dépendre uniquement du navigateur : un clear de cache ne doit pas tout effacer).

### Undo / Redo

Obligatoire pour un outil de design utilisable.

### Verrouillage (souhaitable)

Skins « livrés » vs brouillons éditables — utile plus tard, pas bloquant pour le premier MVP utilisable.

---

## 6. Périmètre couleurs & alphas

### Dans Skin Studio

- Uniquement des couleurs **opaques** `#RRGGBB` (ou équivalent stocké sans canal alpha de design).
- Chaque propriété de skin mappe vers une entrée de palette.

### Côté Matrix-Control (chantier lié, pas dans le studio)

| Cas | Décision |
|---|---|
| Alpha utilisé comme raccourci pour assombrir / atténuer une couleur pleine (textes secondaires, idle, etc.) | Remplacer par un RVB solide en palette (attention : le résultat dépend du fond → souvent une entrée dédiée par usage / skin) |
| Grisement de contrôles entiers via `setAlpha` sur le composant | Remplacer par des couleurs pleines d’état disabled / dimmed quand c’est un choix de skin |
| Vraies superpositions (film de lock, overlay de dialogue) | Hors Skin Studio ; restent des effets UI dans le plugin |
| LEDs | Le studio définit la couleur **pleinement allumée** ; le code du plugin peut continuer à moduler l’intensité (y compris via alpha) |

---

## 7. Skins cibles pour la v1 Matrix-Control

| Nom affiché | Base | Accents contrôles (sliders, combos, …) | LEDs Header (piste) |
|---|---|---|---|
| `Black/Green` | Black actuel | Verts | Verts (comportement actuel à confirmer) |
| `Cream/Green` | Cream actuel | Verts | idem |
| `Black/Duo` | Black | Bleu (Patch) / Orange (Master) via injection orchestrateur | Vert ou jaune (à trancher en calibration) |
| `Cream/Duo` | Cream | Bleu / Orange idem | Vert ou jaune |
| `Monochrome` | Black | Gris | Gris |

Noms écartés : `Black/Two-tone`, `Cream/Two-tone` (trop vagues).  
`Duo` retenu à la place de `Split`.

---

## 8. Format de sortie — JSON

Un seul format d’export machine-readable. Pas de Markdown d’accompagnement obligatoire.

Le schéma exact sera fixé dans le PRD / l’architecture du projet Skin Studio. Intention minimale :

```json
{
  "formatVersion": 1,
  "exportedAt": "2026-10-14T22:00:00Z",
  "palette": [
    { "id": "green4", "constantName": "kGreen4", "hex": "#00DDAD" }
  ],
  "objectTypes": [
    {
      "id": "slider",
      "displayName": "Slider",
      "states": ["normal", "disabled", "focus"],
      "properties": ["background", "track", "valueBar", "text", "focusBorder"]
    }
  ],
  "skins": [
    {
      "id": "black-green",
      "displayName": "Black/Green",
      "locked": false,
      "mappings": [
        {
          "objectTypeId": "slider",
          "state": "normal",
          "property": "valueBar",
          "paletteColorId": "green4"
        }
      ]
    }
  ]
}
```

Notes :

- Les noms de propriétés / états doivent rester **stables** et alignés avec le vocabulaire cible Matrix-Control.
- On pourra exporter un fichier global ou un fichier par skin ; le PRD tranchera.
- Versionner `formatVersion` dès le début pour les allers-retours futurs (y compris V2 contributeurs externes).

---

## 9. Niveau de fidélité de la maquette

**Objectif :** le même niveau de réalisme que la maquette Figma actuelle — composition d’ensemble lisible, rectangles colorés, contours, textes, zones Header / Body (Patch & Master) / Footer, widgets reconnaissables.

**Hors objectif :**

- Réimplémenter tout le LookAndFeel JUCE en CSS.
- Garantir un rendu identique au pixel près avec anti-aliasing / focus rings du plugin.
- Simuler DAW, scaling OS, etc.

La **validation esthétique finale** se fait toujours dans Matrix-Control après intégration.

---

## 10. MVP vs plus tard

### MVP (premier outil vraiment utilisable)

- Palette CRUD + picker + hex
- Skins CRUD (créer / renommer / dupliquer / supprimer)
- Maquette d’ensemble « Figma-like »
- UI Scale (`50%` … `200%`, default `100%`) + pan / scroll X-Y (cliquer-glisser + flèches clavier)
- Sélection objet (clic ou combo) + états + mappings propriété → palette
- Preview temps réel (palette et mappings)
- Undo / redo
- Autosave navigateur + export / import JSON fichier
- Jeu initial bootstrapé depuis l’inventaire Matrix-Control actuel (Black/Cream → Black/Green & Cream/Green)

### Souhaitable peu après le MVP

- « Where used » (surlignage + liste)
- Verrouillage skins livrés / brouillons
- Indicateur simple de contraste sur paires texte/fond ou value/track (utile surtout pour Black)
- Comparaison côte à côte de deux skins (non prioritaire)

### V2 (hors MVP, vision longue)

- Contribuer de nouveaux skins **pas forcément** dérivés de Black/Cream (infographistes / alpha-testeurs design)
- Ils livrent un mapping JSON ; intégration Matrix-Control ensuite
- Prérequis : modèle de tokens stable + pipeline d’import clair dans le plugin

### Explicitement hors scope Skin Studio

- Édition live dans le process audio / UI du plugin (piste future éventuelle, pas ce projet)
- Polices / typographie complète (sauf si un besoin minimal apparaît ; priorité = couleurs)
- Overlays de superposition UI (lock film, dimming modal)
- Campagne marketing / site public de l’outil

---

## 11. Contraintes & risques

| Risque | Mitigation |
|---|---|
| Transformer Skin Studio en second Matrix-Control | Garder le réalisme Figma-like ; valider dans le plugin |
| Inventaire incomplet des propriétés / états | Étape Collecte obligatoire avant de figer le schéma JSON |
| Modèle `blackVariant`/`creamVariant` trop étroit | Refonte conceptuelle (étape 2) avant d’empiler 5 skins « à la main » |
| Perte de données navigateur | Export fichier + import ; ne pas compter sur `localStorage` seul |
| Dérive des noms entre studio et C++ | Vocabulaire partagé documenté ; JSON comme contrat |
| Scope qui mange l’alpha Matrix-Control | Projet BMad **séparé** ; MVP borné ; pas de V2 contributeurs avant stabilité du contrat |

---

## 12. Critères de succès

Skin Studio est un succès si :

1. On peut calibrer visuellement les 5 skins v1 sur une maquette d’ensemble crédible.
2. On n’utilise **que** la palette nommée (pas de codes couleurs sauvages dans les mappings).
3. Un export JSON permet à BMad / un développeur d’implémenter les skins dans Matrix-Control sans retaper des hex au jugé.
4. Le chantier Matrix-Control qui suit laisse un modèle skin/look plus clair (injection par orchestrateur, widgets génériques).
5. L’outil reste suffisamment léger pour ne pas retarder à lui seul la campagne d’alpha-tests du plugin.

---

## 13. Organisation recommandée

| Sujet | Décision |
|---|---|
| Pilotage BMad | **Projet séparé** `matrix-control-skin-studio` |
| Emplacement code outil | Dépôt dédié (éventuellement dossier miroir ou submodule plus tard — à trancher en archi) |
| Source de vérité design pendant la calibration | Skin Studio + JSON exportés |
| Source de vérité runtime | Sources Matrix-Control après intégration |
| Langue docs outil | Français pour brief / échanges Guillaume ; anglais pour artefacts BMad techniques si la config du nouveau projet le demande |

### Suite BMad suggérée (dans le nouveau projet)

1. Injecter ce brief comme point de départ greenfield.
2. `/bmad-product-brief` ou validation / enrichissement du brief.
3. `/bmad-prd` (périmètre MVP, schéma JSON, critères d’acceptance).
4. Architecture légère (données, persistance, structure UI éditeur).
5. Epics & stories → implémentation MVP.
6. En parallèle ou juste après : chantier Matrix-Control « conformité modèle + remplacement alphas raccourcis + skins finaux » alimenté par les JSON.

---

## 14. Décisions déjà tranchées (ne pas rouvrir sans raison)

- Nom produit / repo / dossier local : voir §2.
- Noms skins v1 : `Black/Green`, `Cream/Green`, `Black/Duo`, `Cream/Duo`, `Monochrome`.
- Export : **JSON uniquement** (pas de Markdown d’export obligatoire).
- Widgets génériques ; Patch/Master connus seulement de l’orchestrateur.
- Maquette : réalisme Figma-like, pas clone JUCE pixel-perfect.
- Vue GUI : UI Scale aligné sur Matrix-Control (`50%`–`200%`, default `100%`) + navigation pan/scroll (X/Y, cliquer-glisser, flèches clavier) ; hors contrat JSON d’export.
- Projet BMad **séparé** de Matrix-Control.
- LEDs : couleur « fully on » dans le studio ; modulation d’intensité laissée au plugin.
- Overlays de vraie transparence : hors éditeur de skin.

---

## 15. Points encore ouverts (à trancher dans le PRD / la calibration)

- LEDs Header en Duo : rester vert, ou introduire du jaune (nouvelle entrée palette) ?
- Liste exacte des `objectTypes` / propriétés / états (issue de l’inventaire Collecte).
- Un JSON global vs un JSON par skin.
- Stack technique web minimale (HTML/CSS/JS vanilla vs léger framework) — décider en architecture, privilégier la simplicité.
- Emplacement de l’inventaire Collecte (doc dans Skin Studio, ou note partagée depuis Matrix-Control).

---

## 16. Résumé pour l’agent greenfield

Tu construis **Matrix-Control Skin Studio** : un studio web de skins pour le plugin Matrix-Control.  
Tu n’embarques pas le moteur audio ni le plugin JUCE.  
Tu produis un éditeur + un contrat JSON.  
Tu prépares une refonte plus propre des concepts skin/look côté plugin, sans les implémenter ici.  
Le succès se mesure à : calibration visuelle des 5 skins, discipline de palette, export exploitable, et une passation claire vers le dépôt Matrix-Control.
)
