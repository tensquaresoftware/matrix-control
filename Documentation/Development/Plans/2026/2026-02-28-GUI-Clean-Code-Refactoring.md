---
name: Refactorisation Clean Code GUI
overview: Plan de refactorisation pour éliminer les violations DRY, SOLID et Clean Architecture identifiées dans la codebase GUI/Shared, en préparation de la fonctionnalité zoom.
todos:
  - id: phase1-exceptions
    content: Déplacer WidgetFactoryExceptions vers Source/Shared/Exceptions/
    status: completed
  - id: phase2-skin-init
    content: Factoriser initializeBlackVariantColours/initializeCreamVariantColours avec lambda
    status: completed
  - id: phase2-remove-getters
    content: Supprimer getters redondants Skin et migrer widgets vers getColour()
    status: completed
  - id: phase3-iskin
    content: Créer ISkin, faire implémenter par Skin, créer propagateSkin
    status: completed
  - id: phase3-migrate-widgets
    content: Migrer widgets/panels vers ISkin& et utiliser propagateSkin
    status: completed
  - id: phase4-headerpanel
    content: Éliminer magic numbers HeaderPanel (Skin/Zoom enums, kLeftPadding_)
    status: completed
  - id: phase5-toppanel
    content: Factoriser TopPanel::resized() avec tableau et boucle
    status: completed
  - id: phase6-buttonwidth
    content: Ajouter buttonWidth à StandaloneWidgetDescriptor et simplifier WidgetFactory
    status: completed
  - id: phase7-parameterpanel
    content: Implémenter getDimensionsForModuleType() dans ParameterPanel
    status: completed
  - id: phase8-skincolours
    content: Séparer SkinColours.h en fichiers par catégorie
    status: completed
isProject: false
---

# Plan de refactorisation Clean Code - GUI et Shared

## Réponses aux questions

### 2.4 Skin IDs et emplacement des constantes

**ColourVariant / SkinComboBoxItemId** : Option A retenue. Déclarer `enum class SkinComboBoxItemId : int { kBlack = 1, kCream = 2 };` dans [Source/GUI/Skins/Skin.h](Source/GUI/Skins/Skin.h), **immédiatement après le bloc ColourVariant** (lignes 15-19), en enum imbriqué dans la classe `Skin`. Référence : `tss::Skin::SkinComboBoxItemId`. Cet enum ne va pas dans PluginDisplayNames.h car il s'agit d'IDs, pas de noms d'affichage.

**Skin — Noms d'affichage** : Créer `ChoiceLists::SkinVariants` dans PluginDisplayNames.h avec `kBlack = "BLACK"` et `kCream = "CREAM"`.

**Zoom — Noms d'affichage** : Dans [Source/Shared/Definitions/PluginDisplayNames.h](Source/Shared/Definitions/PluginDisplayNames.h), créer le namespace `ZoomLevels` **à l'intérieur de `ChoiceLists`**, **juste avant** le sous-namespace `MidiChannel`. Contenu : `k50 = "50%"`, `k75 = "75%"`, `k90 = "90%"`, `k100 = "100%"`, `k125 = "125%"`, `k150 = "150%"`, `k200 = "200%"`.

**Zoom — IDs** : Dans [Source/Shared/Definitions/PluginIDs.h](Source/Shared/Definitions/PluginIDs.h), créer le namespace `Settings` **à l'intérieur de `PluginIDs`**, **juste avant** le namespace `Mode`. Dans `Settings`, créer le sous-namespace `ZoomLevels` pour y stocker les IDs : `k50 = 1`, `k75 = 2`, `k90 = 3`, `k100 = 4`, `k125 = 5`, `k150 = 6`, `k200 = 7`.

### 4.3 ComboBox addItem — Enum vs getNumItems()+1

**Cas 1 — Choix fixes (Skin, Zoom)** : Retenu. Les options sont connues à la compilation. Utiliser des enums/constantes.

### 3.3 ISkin et pattern setSkin()

**ISkin** = interface du fournisseur de skin (couleurs, polices). **ISkinnable** = interface des composants qui reçoivent un skin.

- **ISkin** : découple les widgets de la classe `Skin` (réutilisabilité, tests avec mocks).
- **setSkin()** : propagation du skin aux enfants.

Ces deux aspects sont complémentaires. **Implémenter ISkin règle les deux problématiques** : découplage des widgets ET factorisation du pattern setSkin() via le helper `propagateSkin()`.

---

## Phase 1 : Exceptions (Clean Architecture)

Déplacer WidgetFactoryExceptions vers Source/Shared/Exceptions/

## Phase 2 : Skin — DRY et suppression des getters

Factoriser initializeBlackVariantColours/initializeCreamVariantColours avec lambda. Supprimer getters redondants et migrer vers getColour().

## Phase 3 : Interface ISkin et helper setSkin()

Créer ISkin, faire implémenter par Skin, créer propagateSkin. Migrer widgets/panels vers ISkin& et utiliser propagateSkin.

## Phase 4 : HeaderPanel — Magic numbers

Éliminer magic numbers (Skin/Zoom enums, kLeftPadding_).

## Phase 5 : TopPanel::resized()

Factoriser avec tableau et boucle.

## Phase 6 : buttonWidth dans StandaloneWidgetDescriptor

Ajouter buttonWidth à StandaloneWidgetDescriptor et simplifier WidgetFactory.

## Phase 7 : ParameterPanel — getDimensionsForModuleType()

Implémenter getDimensionsForModuleType() dans ParameterPanel.

## Phase 8 : SkinColours.h — Séparation par catégories

Séparer SkinColours.h en fichiers par catégorie (Panels, SectionHeader, ModuleHeader, etc.).
