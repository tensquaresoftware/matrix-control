---
name: Créer McModulePanel classe de base
overview: Créer la classe de base `McModulePanel` qui factorise la structure commune des modules de paramètres (PatchEditPanel et MasterEditPanel), avec calcul dynamique de la hauteur, support optionnel des boutons, et gestion automatique du layout.
todos:
  - id: create_header
    content: Créer McModulePanel.h avec enum ParameterWidgetType, struct ParameterConfig, et déclaration de la classe McModulePanel
    status: completed
  - id: create_implementation
    content: Créer McModulePanel.cpp avec implémentation du constructeur, calculateHeight(), createHeader(), createParameterRow(), layoutHeader(), layoutParameters(), resized(), paint(), et setTheme()
    status: completed
    dependencies:
      - create_header
  - id: test_compilation
    content: Vérifier que le code compile sans erreurs et que tous les includes nécessaires sont présents (McModuleName, McButton, McParameterLabel, McSlider, McComboBox, McParameterSeparator, McTheme)
    status: completed
    dependencies:
      - create_implementation
  - id: todo-1767010123127-vq52su76p
    content: Les fichiers McModulePanel.h/.cpp doivent être créés à la racine du dossier MainPanel
    status: pending
---

# Créer la classe de base McModulePanel

## Objectif

Créer une classe de base réutilisable qui encapsule la structure commune des modules de paramètres pour éviter la duplication de code entre les 10 modules de `PatchEditPanel` et les modules de `MasterEditPanel`.

## Structure de la classe

### Fichiers à créer

- `[Source/UI/Panels/MainPanel/PatchEditPanel/McModulePanel.h](Source/UI/Panels/MainPanel/PatchEditPanel/McModulePanel.h)` - Déclaration de la classe
- `[Source/UI/Panels/MainPanel/PatchEditPanel/McModulePanel.cpp](Source/UI/Panels/MainPanel/PatchEditPanel/McModulePanel.cpp)` - Implémentation

### Enum pour les types de widgets

Créer un enum class `ParameterWidgetType` dans le header :
```cpp
enum class ParameterWidgetType
{
    Slider,
    ComboBox
    // Extensible pour futurs types
};
```

### Structure ParameterConfig

```cpp
struct ParameterConfig
{
    juce::String label;
    juce::String id;
    ParameterWidgetType type;
};
```

### Classe McModulePanel

**Membres publics :**
- Constructeur prenant : `moduleName`, `buttonLabels` (peut être vide), `parameters`, `theme`
- `setTheme(McTheme* theme)` - Propagation du thème à tous les widgets enfants
- `resized()` override - Layout automatique
- `paint(juce::Graphics& g)` override - Dessin du fond

**Membres protégés (pour surcharge si besoin) :**
- `createHeader()` - Création de l'en-tête (McModuleName + boutons)
- `createParameterRow(int index, const ParameterConfig& config)` - Création d'une ligne de paramètre

**Membres privés :**
- `layoutHeader()` - Positionnement de l'en-tête
- `layoutParameters()` - Positionnement des paramètres
- `calculateHeight()` - Calcul dynamique de la hauteur : `(1 + parameters.size()) * 25`

**Variables membres :**
- `McTheme* mcTheme`
- `std::unique_ptr<McModuleName> name`
- `std::vector<std::unique_ptr<McButton>> buttons`
- `std::vector<std::unique_ptr<McParameterLabel>> labels`
- `std::vector<std::unique_ptr<juce::Component>> controls` (McSlider ou McComboBox)
- `std::vector<std::unique_ptr<McParameterSeparator>> separators`
- Configuration : `juce::String moduleName`, `std::vector<juce::String> buttonLabels`, `std::vector<ParameterConfig> parameters`
- `static constexpr int kWidth = 150`
- Constantes pour les hauteurs : `kTitleHeight = 25`, `kParameterRowHeight = 25`

## Calcul de la hauteur

La hauteur est calculée dynamiquement dans `calculateHeight()` :
- Formule : `(1 + nombre_de_paramètres) * 25`
- PatchEditPanel : toujours 10 paramètres visuels → `(1 + 10) * 25 = 275`
- MasterEditPanel : 7 ou 8 paramètres → `(1 + 7) * 25 = 200` ou `(1 + 8) * 25 = 225`

## Layout

**En-tête :**
- `McModuleName` en haut (hauteur 25px)
- `McButton` alignés horizontalement à droite du titre (si `buttonLabels` non vide)

**Corps :**
- Pour chaque paramètre :
  - `McParameterLabel` (90x20) à gauche
  - `McSlider` (60x20) ou `McComboBox` (60x20) à droite du label
  - `McParameterSeparator` (150x5) en dessous
- Un `McParameterSeparator` final après le dernier paramètre

## Gestion du thème

Tous les widgets enfants doivent recevoir le thème via `setTheme()` :
- `McModuleName`
- `McButton` (tous)
- `McParameterLabel` (tous)
- `McSlider` / `McComboBox` (tous)
- `McParameterSeparator` (tous)

## Points d'attention

- Les modules MasterEdit n'ont pas de boutons (`buttonLabels` vide)
- Les modules PatchEdit ont toujours 10 lignes visuelles (même si seulement 9 paramètres réels)
- Le dernier séparateur est toujours présent après le dernier paramètre
- Les widgets doivent être créés dans le constructeur et ajoutés comme enfants
- Respecter les conventions de nommage (lowerCamelCase pour méthodes, PascalCase pour classes)