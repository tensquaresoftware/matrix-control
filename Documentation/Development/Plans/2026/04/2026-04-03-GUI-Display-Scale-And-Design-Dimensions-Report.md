# Matrix-Control / Dimensions de design & facteur d'échelle

**Date :** 2026-04-03  
**Contexte :** synthèse d’un échange avec Claude IA sur la stratégie de dessin / mise à l’échelle de la GUI Matrix-Control, et proposition cible (sans modification du code source à ce stade).

---

## 1. Résumé de tes échanges avec Claude

### 1.1 Rôles et tailles

- Le **`PluginEditor`** est le composant contractuellement responsable de la taille vue par l’hôte ; le **`MainComponent`** reçoit ses bornes du parent et adapte son contenu — les deux largeurs/hauteurs ne sont pas obligatoirement identiques, mais le cas « plein cadre » est le plus courant.

### 1.2 Vocabulaire (éviter la collision avec JUCE)

- JUCE parle de **scale DPI / facteur d’affichage système** (`getScaleFactor()`, densité d’écran).
- Ton facteur utilisateur est plutôt un **display scale** ou **plugin scale** (redimensionnement global de l’UI), à ne pas confondre avec le scale système.

### 1.3 Stratégie de layout

- Approche **descendante** (parent → enfants), alignée sur `resized()` et la propagation du facteur d’échelle, préférable à une approche **ascendante** (somme des enfants) qui couple fortement les niveaux.
- Utiliser **`removeFromTop` / `removeFromLeft`** (ou équivalent) pour que le **dernier enfant absorbe le reste** et limiter les écarts d’arrondi entre frères.

### 1.4 Arrondis

- `juce::roundToInt` : au plus proche, avec **banker’s rounding** sur les `.5` — peut rendre les sommes de sous-blocs **≠** largeur/hauteur parent selon le facteur.
- Recommandation discutée : politique **déterministe** (souvent **`floor`** sur les tranches fixes + **reste** sur une zone flexible) pour éviter fissures / chevauchements intermittents.

### 1.5 Widgets vectoriels et détails (séparateurs, sliders)

- Une ligne **1 px** dans une hauteur **paire** ne peut pas être centrée au pixel près : il faut une **règle de paint** stable (pair / impair, ou placement fixe), pas seulement multiplier des constantes.
- Pour label + contrôle + bandes vides : calculer le **padding ou le contenu à partir de l’empreinte réelle** (ou un **ratio**) évite que les arrondis cassent la symétrie à certains facteurs.

### 1.6 Presets vs « pixel-perfect » partout

- Une **liste arbitraire** de pourcentages (90 %, 110 %, etc.) + **dimensions entières fixes** ne peut pas garantir des produits `dimension × scale` entiers pour **toutes** les constantes.
- Piste retenue : **restreindre les presets** à une famille **rationnelle compatible** et/ou **harmoniser les dimensions de design** avec cette famille ; abandon du **resize libre au pas de 1 %** si tu veux limiter la complexité.

### 1.7 Ce que Claude s’était engagé à livrer

- Tableaux dimension × preset, recommandations d’ajustement, liste de constantes C++ — la conversation s’est **interrompue** avant livraison complète.

---

## 2. Ce qui est proposé pour simplifier la problématique de dessin

### 2.1 Objectif minimal réaliste

- **Entiers en layout** : pour chaque constante de design `d` et chaque preset `s` autorisé, viser **`d × s` entier** après multiplication (avant ou après la même convention d’arrondi que le code).  
- **Cohérence des sommes** : largeurs des colonnes du body + séparateurs = largeur utile ; hauteurs des bandes = hauteur utile — avec la même convention partout, ou **une colonne / une bande flexible** qui prend le reste.

### 2.2 Règle mathématique utile pour les presets de facteur d'échelle

Pour les six facteurs suivants, exprimés en fractions :

| Preset | Fraction |
|--------|----------|
| 50 %   | 1/2      |
| 75 %   | 3/4      |
| 100 %  | 1        |
| 125 %  | 5/4      |
| 150 %  | 3/2      |
| 200 %  | 2        |

**Propriété :** pour que **`d × s` soit entier pour tous ces `s`**, il suffit que **`d` soit un multiple de 4**.

*(Intuition : les dénominateurs impliqués sont 2 et 4 ; le ppcm des contraintes sur les entiers `d` est 4.)*

Conséquence : harmoniser **toutes** les dimensions de design « scalées linéairement » sur des **multiples de 4** supprime les **parties fractionnaires** du produit `d × s` à ces presets — ce qui aligne `scaledInt(d, s)` (ou `floor(d*s)`) sur des valeurs **prévisibles** et stabilise les sommes si le layout applique la même règle partout.

### 2.3 Ce que cette règle ne résout pas seule

- **Parité visuelle** (ligne 1 px dans une bande de hauteur paire) : à traiter dans **`paint()`** avec des règles explicites, pas uniquement dans `PluginDimensions.h`.
- **Scale système** : le produit **plugin scale × system scale** peut réintroduire des demi-pixels si l’hôte / JUCE fournit un facteur non rationnel simple — à documenter dans la stratégie (snap, ou acceptation du flottant côté `AffineTransform` uniquement, selon ton choix futur).

### 2.4 Simplifications de produit

- **Pas de resizer à pas de 1 %** : tu évites une grille de scales infinie et les tests combinatoires.
- **Pas de presets « décoratifs »** (90 %, 110 %, …) tant que tu n’as pas une autre grille mathématique ou une tolérance visuelle assumée.
- **Liste courte** : 50 % à 200 % avec pas « sûrs » couvre usage studio / petit écran / démo sans explosion de cas.

---

## 3. Liste simplifiée des presets de facteur d’échelle (recommandée)

**Presets à implémenter dans la ComboBox (ordre suggéré) :**

| Libellé UI | Valeur |
|------------|--------|
| 50 %       | 0,50   |
| 75 %       | 0,75   |
| 100 % (défaut) | 1,00 |
| 125 %      | 1,25   |
| 150 %      | 1,50   |
| 200 %      | 2,00   |

- **Hors scope volontaire** : 90 %, 110 %, redimensionnement libre fin (supprimé).
- **125 %** reste dans la famille **1/4** ; avec des dimensions **multiples de 4**, il reste compatible avec les autres de la liste ci-dessus.

---

## 4. Dimensions de design corrigées proposées (cible)

Les tableaux ci-dessous sont une **proposition cohérente** pour :

- respecter la **règle multiple de 4** ;
- conserver la **structure** de ton `PluginDimensions.h` actuel ;
- **réduire légèrement** la fenêtre par rapport à 1335×810 (souhait exprimé dans l’échange).

**À appliquer manuellement** dans le code quand tu seras prêt — le dépôt est resté sur les valeurs d’origine à la demande de ce rapport.

**Vérification largeur utile du body :**  
`kWidth − 2 × kPadding = 1312 − 24 = 1288`  
`patch + sep + shared + sep + master = 800 + 32 + 264 + 32 + 160 = 1288` ✓  

**Rangée Patch Edit (5 modules) :** `5 × 144 + 4 × 20 = 800` ✓  
*(Les `TopPanel` / `BottomPanel` utilisent aujourd’hui `kPadding` comme espacement ; pour cette géométrie cible, il faudra un écart **20** entre modules — constante dédiée type `kInterModuleGap`, ou aligner `kPadding` sur 20 si tu acceptes le même écart partout.)*

**Middle Panel :** espacement horizontal entre displays **16** au lieu de **15** pour rester multiple de 4 (`4 × (144 + 16) + reste = 800` pour la zone displays + colonne nom de patch).

### 4.1 Fenêtre et panneaux principaux

| Symbole / zone | Constante (référence namespace) | Largeur | Hauteur |
|----------------|----------------------------------|---------|---------|
| Fenêtre | `GUI::kWidth` × `kHeight` | **1312** | **800** |
| Coin resize | `GUI::kResizeCornerSize` | — | **8** |
| Header | `Panels::Header` | 1312 | **24** |
| Body | `Panels::Body::kWidth` × `kHeight` | 1312 | **752** |
| Padding body | `Panels::Body::kPadding` | — | **12** (×4 côtés) |
| Hauteur utile | `kEffectiveHeight` | — | **728** |
| Footer | `Panels::Footer` | 1312 | **24** |

### 4.2 Master Edit

| Zone | Constante | Largeur | Hauteur |
|------|-----------|---------|---------|
| Section | `MasterEditSection` | **160** | **728** |
| Midi | `MidiModule::kHeight` | — | **232** |
| Vibrato | `VibratoModule::kHeight` | — | **240** |
| Misc | `MiscModule::kHeight` | — | **232** |
| *(contrôle : 24 + 232 + 240 + 232 = 728)* | | | |

### 4.3 Patch Edit

| Zone | Constante | Largeur | Hauteur |
|------|-----------|---------|---------|
| Section | `PatchEditSection` | **800** | **728** |
| Module (largeur colonne) | `kModuleWidth` | **144** | — |
| Top / Bottom | `kTopBottomPanelHeight` | — | **288** |
| Middle | `MiddleModules::kHeight` | — | **128** |
| Écart horizontal entre modules (cible) | *(nouvelle ou réutilisation)* | — | **20** |
| Top/Bottom child | `ChildModules` | 144 | 288 |
| Middle child | `ChildModules` | 144 | 128 |

*(Contrôle vertical : 24 + 288 + 128 + 288 = 728 si `kSectionHeader` = 24 — voir § 4.5.)*

### 4.4 Colonne Matrix + Patch Manager

| Zone | Constante | Largeur | Hauteur |
|------|-----------|---------|---------|
| Colonne partagée | `SharedColumn::kWidth` | **264** | — |
| Matrix | `MatrixModulationSection` | 264 | **320** |
| Patch Manager | `PatchManagerSection` | 264 | **400** |
| Bank utility | `BankUtilityModule` | 264 | **88** |
| Internal patches | `InternalPatchesModule` | 264 | **84** |
| Computer patches | `ComputerPatchesModule` | 264 | **84** |
| Patch mutator | `PatchMutatorModule` | 264 | **120** |
| *(sous-panneaux + en-tête : ajuster `kSectionHeader` à 24 → 24+88+84+84+120 = 400)* | | | |

### 4.5 Séparateurs verticaux du body

| Élément | Constante | Largeur | Hauteur |
|---------|-----------|---------|---------|
| Séparateur | `VerticalSeparator::kStandard` | **32** | **728** (`kVerticalSeparator` = `kEffectiveHeight`) |

### 4.6 Widgets — largeurs (`Widgets::Widths`)

| Groupe | Constante | Largeur cible |
|--------|-----------|---------------|
| GroupLabel | `kInternalPatchesBrowser` | **108** |
| | `kInternalPatchesMemory` | **156** |
| | `kComputerPatchesBrowser` | **136** |
| | `kComputerPatchesStorage` | **128** |
| Label | `kMasterEditModule` | **100** |
| | `kPatchEditModule` | **88** |
| | `kModulationBusNumber` | **16** |
| | `kPatchManagerBankSelector` | **76** |
| | `kPatchMutator` | **44** |
| Button | `kHeaderPanelTheme` | **68** |
| | `kInit` / `kCopy` / `kPaste` / `kPatchMutatorDelete` | **20** |
| | `kPatchManagerBankSelect` | **36** |
| | `kPatchManagerUnlockBank` | **76** |
| | `kInternalPatchesMemory` / `kComputerPatchesStorage` | **36** |
| | `kComputerPatchesSaveAs` / `kPatchMutatorMutate` / `kPatchMutatorCompare` | **48** |
| | `kPatchMutatorClear` / `kPatchMutatorExport` | **44** |
| Toggle | `kPatchMutator` | **20** |
| Slider | `kStandard` | **60** |
| | `kPatchMutator` | **44** |
| ComboBox | `kMasterEditModule` / `kPatchEditModule` / `kMatrixModulationSource` | **60** |
| | `kMatrixModulationDestination` | **104** |
| | `kPatchManagerComputerPatches` | **84** |
| | `kPatchMutatorHistory` | **44** |
| NumberBox | `kPatchManagerBankNumber` | **24** |
| | `kPatchManagerPatchNumber` | **28** |

*(Les largeurs `HorizontalSeparator::*` suivent les largeurs de section / module / colonne : 160, 144, 264 — dérivées des constantes panneau.)*

### 4.7 Widgets — hauteurs (`Widgets::Heights`)

| Constante | Hauteur cible |
|-----------|---------------|
| `kSectionHeader` | **24** |
| `kModuleHeader` | **24** |
| `kModulationBusHeader` | **24** |
| `kGroupLabel` | **24** |
| `kLabel` | **20** |
| `kButton` | **20** |
| `kToggle` | **20** |
| `kSlider` | **20** |
| `kComboBox` | **20** |
| `kNumberBox` | **20** |
| `kPatchNameDisplay` | **72** |
| `kHorizontalSeparator` | **4** |
| `kVerticalSeparator` | **728** (= `kEffectiveHeight`) |

**Note `kHorizontalSeparator` :** passer de **5** à **4** garde **`kLabel + kHorizontalSeparator` = 24** (multiple de 4), ce qui aide les hauteurs de ligne Matrix / paramètres à rester stables sur les six presets. Le rendu fin des traits reste à valider dans `HorizontalSeparator::paint`.

### 4.8 Alignement optionnel `ModulationBusHeader`

Le header utilise encore des constantes internes (ex. largeur texte numéro de bus **15**) : pour coller à **`Label::kModulationBusNumber` = 16**, prévoir le même entier dans le look du header.

---

## 5. Prochaines étapes suggérées

1. Relire ce rapport et figer les **presets** ComboBox (§ 3).  
2. Décider si tu appliques **tel quel** le jeu de dimensions § 4 ou une variante (autre largeur totale, tant que **multiples de 4** et **sommes** cohérentes).  
3. Mettre à jour `PluginDimensions.h`, puis **un seul passage** sur les layouts qui supposent encore **15** px d’écart entre modules Patch Edit / Middle.  
4. Documenter dans `GUI-Dimensions-And-Hierarchy.md` les valeurs retenues une fois intégrées.

---

*Fin du rapport.*
