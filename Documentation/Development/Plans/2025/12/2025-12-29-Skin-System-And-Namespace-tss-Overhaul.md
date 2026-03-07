---
name: Refonte Skin + namespace tss
overview: Refonte complète du système de thèmes vers un système de Skin unifié (couleurs + dimensions + polices), avec migration vers le namespace tss pour tous les widgets custom et les classes Skin/Theme.
todos:
  - id: create_skin_structure
    content: "Créer la structure Skin complète : SkinData.h (SkinColours, SkinDimensions, SkinFonts), Skin.h/cpp, SkinFactory.h/cpp"
    status: completed
  - id: migrate_module_panel_dimensions
    content: Migrer McModulePanel pour utiliser SkinDimensions au lieu de constantes locales
    status: completed
    dependencies:
      - create_skin_structure
  - id: migrate_widgets_namespace
    content: Migrer tous les widgets vers namespace tss et Skin (Button, Slider, ComboBox, ParameterLabel, ModuleName, SectionName, GroupLabel, ParameterSeparator, PanelSeparator, PopupMenu, FocusableWidget)
    status: completed
    dependencies:
      - create_skin_structure
  - id: migrate_panels
    content: Migrer tous les panneaux vers Skin (PatchEditPanel, MasterEditPanel, MatrixModulationPanel, PatchManagerPanel, MainPanel, HeaderPanel, FooterPanel, McModulePanel)
    status: completed
    dependencies:
      - migrate_widgets_namespace
  - id: cleanup_old_theme_system
    content: Supprimer les anciens fichiers de thèmes (McTheme, McBlackTheme, McCreamTheme, McDebugTheme) et mettre à jour tous les includes restants
    status: completed
    dependencies:
      - migrate_panels
---

# Refonte Skin + namespace tss

## Objectif

Refondre complètement le système de thèmes vers un système de **Skin** unifié qui regroupe couleurs, dimensions et polices, et migrer tous les widgets custom vers le namespace `tss` pour éviter les collisions avec JUCE.

## Structure cible

```
Source/UI/
├── Skin/
│   ├── Skin.h/cpp                    # Classe principale Skin
│   ├── SkinData.h                    # Données structurées
│   │   ├── SkinColours.h             # Tableaux de couleurs par élément/thème
│   │   ├── SkinDimensions.h          # Dimensions centralisées
│   │   └── SkinFonts.h              # Définitions de polices
│   └── SkinFactory.h/cpp            # Factory pour créer les skins
├── Widgets/                          # Tous dans namespace tss
│   ├── Button.h/cpp                  # tss::Button (ex McButton)
│   ├── Slider.h/cpp                  # tss::Slider (ex McSlider)
│   └── ...                           # Tous les autres widgets
└── Themes/                           # À supprimer après migration
    └── (anciens fichiers McTheme*)
```

## Phases de migration

### Phase 1 : Créer la structure Skin (sans casser l'existant)

**Fichiers à créer :**
- `Source/UI/Skin/SkinData.h` avec :
  - `SkinColours.h` : Tableaux de couleurs structurés par élément avec champs nommés (`blackTheme`, `creamTheme`, `debugTheme`)
  - `SkinDimensions.h` : Toutes les dimensions centralisées (widgets, layout, panels)
  - `SkinFonts.h` : Définitions de polices par type d'élément
- `Source/UI/Skin/Skin.h/cpp` : Classe principale avec méthodes pour couleurs, dimensions et polices
- `Source/UI/Skin/SkinFactory.h/cpp` : Factory pour créer les skins

**Décisions :**
- Namespace `tss` pour Skin et Theme : `tss::Skin`, `tss::Theme`
- Structure `ColourElement` avec champs nommés explicites
- Migration progressive : l'ancien système reste fonctionnel pendant la transition

### Phase 2 : Migrer McModulePanel vers SkinDimensions

**Modifications :**
- Modifier `McModulePanel.cpp` pour utiliser `SkinDimensions` au lieu de constantes locales
- Tester que le layout fonctionne correctement

### Phase 3 : Migrer les widgets vers namespace tss + Skin

**Widgets à migrer (16 widgets) :**
1. `McButton` → `tss::Button`
2. `McSlider` → `tss::Slider`
3. `McComboBox` → `tss::ComboBox`
4. `McParameterLabel` → `tss::ParameterLabel`
5. `McModuleName` → `tss::ModuleName`
6. `McSectionName` → `tss::SectionName`
7. `McGroupLabel` → `tss::GroupLabel`
8. `McParameterSeparator` → `tss::ParameterSeparator`
9. `McPanelSeparator` → `tss::PanelSeparator`
10. `McPopupMenu` → `tss::PopupMenu`
11. `McFocusableWidget` → `tss::FocusableWidget` (utilitaire)

**Pour chaque widget :**
- Renommer la classe dans le namespace `tss`
- Modifier le constructeur pour accepter `tss::Skin&` au lieu de `McTheme*`
- Remplacer les appels `getTheme()->...` par `skin->...`
- Utiliser `SkinDimensions` pour les tailles au lieu de constantes locales
- Mettre à jour tous les includes et utilisations dans le codebase

### Phase 4 : Migrer les panneaux vers Skin

**Panneaux à migrer :**
- `PatchEditPanel`
- `MasterEditPanel`
- `MatrixModulationPanel`
- `PatchManagerPanel`
- `MainPanel`
- `HeaderPanel`
- `FooterPanel`
- `McModulePanel`

**Modifications :**
- Remplacer `McTheme*` par `tss::Skin&`
- Mettre à jour les appels aux widgets
- Utiliser `SkinDimensions` pour les layouts

### Phase 5 : Nettoyer l'ancien système

**Fichiers à supprimer :**
- `McTheme.h/cpp`
- `McBlackTheme.h/cpp`
- `McCreamTheme.h/cpp`
- `McDebugTheme.h/cpp`

**Conserver :**
- `McColourPalette.h` (utile pour les références de couleurs)

## Détails techniques

### Structure SkinColours.h

```cpp
namespace tss
{
    namespace SkinColours
    {
        struct ColourElement
        {
            const char* elementName;
            juce::Colour blackTheme;   // Explicite
            juce::Colour creamTheme;   // Explicite
            juce::Colour debugTheme;   // Explicite
        };

        namespace Button
        {
            inline constexpr ColourElement kBackgroundOff = {
                "ButtonBackgroundOff",
                McColourPalette::kDarkGrey2,    // blackTheme
                McColourPalette::kLightGrey3,   // creamTheme
                McColourPalette::kRed           // debugTheme
            };
            // ... autres éléments
        }
        // ... autres catégories
    }
}
```

### Structure SkinDimensions.h

```cpp
namespace tss
{
    namespace SkinDimensions
    {
        namespace Widget
        {
            inline constexpr int kParameterLabelWidth = 90;
            inline constexpr int kParameterLabelHeight = 20;
            // ... toutes les dimensions des widgets
        }
        
        namespace Layout
        {
            inline constexpr int kParameterRowHeight = 25;
            inline constexpr int kHeaderHeight = 25;
            // ... dimensions de layout
        }
        
        namespace Panel
        {
            inline constexpr int kPatchEditPanelWidth = 810;
            // ... dimensions des panneaux
        }
    }
}
```

### Classe Skin

```cpp
namespace tss
{
    class Skin
    {
    public:
        enum class Theme
        {
            Black,
            Cream,
            Debug
        };

        explicit Skin(Theme theme);
        
        // Couleurs
        juce::Colour getButtonBackgroundOff() const;
        // ... toutes les méthodes de couleurs
        
        // Dimensions
        int getParameterLabelWidth() const;
        // ... toutes les méthodes de dimensions
        
        // Polices
        juce::Font getButtonFont() const;
        // ... toutes les méthodes de polices
        
        Theme getTheme() const { return currentTheme; }

    private:
        Theme currentTheme;
        juce::Colour getColour(const SkinColours::ColourElement& element) const;
    };
}
```

## Ordre d'exécution recommandé

1. **Phase 1** : Créer toute la structure Skin (sans toucher aux widgets existants)
2. **Phase 2** : Migrer McModulePanel vers SkinDimensions (test immédiat)
3. **Phase 3** : Migrer les widgets un par un (tester après chaque widget)
4. **Phase 4** : Migrer les panneaux
5. **Phase 5** : Nettoyer l'ancien système

## Points d'attention

- **Compatibilité** : Garder l'ancien système fonctionnel pendant la migration
- **Tests** : Tester après chaque phase pour éviter les régressions
- **Includes** : Mettre à jour tous les includes dans tout le codebase
- **Forward declarations** : Utiliser `namespace tss { class Skin; }` dans les headers