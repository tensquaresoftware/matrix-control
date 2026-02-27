---
name: Système de descripteurs pour ModulePanel
overview: Créer un système de descripteurs hiérarchique avec SynthDescriptors.h pour l'architecture générale et des fichiers spécifiques par module, puis adapter ModulePanel pour utiliser ces descripteurs et préparer l'intégration avec APVTS.
todos:
  - id: create_synth_descriptors
    content: Créer SynthDescriptors.h avec les structures de base (ParameterDescriptor, ModuleDescriptor, enums, fonctions utilitaires)
    status: completed
  - id: create_dco1_descriptors
    content: Créer Dco1ModuleDescriptors.h avec les 10 paramètres complets comme exemple de référence
    status: completed
    dependencies:
      - create_synth_descriptors
  - id: adapt_module_panel
    content: Adapter ModulePanel pour utiliser ModuleDescriptor au lieu de ParameterConfig
    status: completed
    dependencies:
      - create_synth_descriptors
  - id: test_dco1
    content: Tester ModulePanel avec le descripteur DCO 1 pour valider le fonctionnement
    status: completed
    dependencies:
      - create_dco1_descriptors
      - adapt_module_panel
  - id: create_other_patch_modules
    content: Créer les descripteurs pour les autres modules Patch Edit (DCO 2, VCF/VCA, FM/TRACK, etc.)
    status: completed
    dependencies:
      - test_dco1
  - id: create_master_modules
    content: Créer les descripteurs pour les modules Master Edit (MIDI, VIBRATO, MISC)
    status: completed
    dependencies:
      - test_dco1
  - id: prepare_apvts_factory
    content: Créer la structure de base de PluginParameterFactory pour l'intégration future avec APVTS
    status: completed
    dependencies:
      - create_synth_descriptors
---

# Système de descripteurs pour ModulePanel

## Architecture proposée

Structure de fichiers hiérarchique :
```
Source/GUI/Descriptors/
├── SynthDescriptors.h                    # Architecture générale (IDs, noms, structures)
├── PatchEdit/
│   ├── Dco1ModuleDescriptors.h
│   ├── Dco2ModuleDescriptors.h
│   ├── VcfVcaModuleDescriptors.h
│   ├── FmTrackModuleDescriptors.h
│   ├── RampPortamentoModuleDescriptors.h
│   ├── Env1ModuleDescriptors.h
│   ├── Env2ModuleDescriptors.h
│   ├── Env3ModuleDescriptors.h
│   ├── Lfo1ModuleDescriptors.h
│   └── Lfo2ModuleDescriptors.h
└── MasterEdit/
    ├── MidiModuleDescriptors.h
    ├── VibratoModuleDescriptors.h
    └── MiscModuleDescriptors.h
```

## Phase 1 : Structures de données centralisées

### 1.1 Créer `SynthDescriptors.h`

**Localisation :** `Source/GUI/Descriptors/SynthDescriptors.h`

**Contenu :**
- Enums pour les IDs : `ModeId`, `SectionId`, `ModuleId`
- Structures de base : `ParameterDescriptor`, `ComboBoxChoice`, `ModuleDescriptor`
- Tableaux de noms centralisés pour sections et modules
- Fonctions utilitaires pour récupérer les noms à partir des IDs

**Structures clés :**
```cpp
struct ParameterDescriptor {
    juce::String parameterId;        // "dco1Frequency"
    juce::String displayLabel;        // "FREQUENCY"
    ParameterWidgetType widgetType;    // Slider ou ComboBox
    float minValue;                  // Pour Slider
    float maxValue;                  // Pour Slider
    float defaultValue;              // Pour Slider
    int defaultChoiceIndex;          // Pour ComboBox (-1 si Slider)
    std::vector<ComboBoxChoice> choices; // Pour ComboBox (vide si Slider)
    uint8_t sysExOffset;             // Offset dans le message SysEx
    uint8_t sysExParameterId;        // ID du paramètre SysEx (0x00, 0x01, etc.)
};

struct ComboBoxChoice {
    juce::String displayText;        // "OFF", "SOFT", etc.
    int value;                       // Valeur numérique correspondante
};

struct ModuleDescriptor {
    ModuleId moduleId;
    juce::String moduleName;         // "DCO 1"
    std::vector<juce::String> buttonLabels; // ["I", "C", "P"] ou []
    std::vector<ParameterDescriptor> parameters;
    bool hasEmptyRow;                // true pour FM/TRACK (10ème ligne vide)
};
```

## Phase 2 : Descripteurs de modules spécifiques

### 2.1 Créer `Dco1ModuleDescriptors.h` (exemple de référence)

**Localisation :** `Source/GUI/Descriptors/PatchEdit/Dco1ModuleDescriptors.h`

**Contenu :**
- Include de `SynthDescriptors.h`
- Fonction `getDco1ModuleDescriptor()` qui retourne un `ModuleDescriptor` complet
- Utilise les IDs et noms définis dans `SynthDescriptors.h`
- Définit les 10 paramètres avec leurs valeurs (min/max/default, choix ComboBox, offsets SysEx)

**Structure :**
```cpp
inline ModuleDescriptor getDco1ModuleDescriptor()
{
    ModuleDescriptor descriptor;
    descriptor.moduleId = ModuleId::dco1;
    descriptor.moduleName = getModuleName(ModuleId::dco1).valueOr("DCO 1");
    descriptor.buttonLabels = {"I", "C", "P"};
    descriptor.hasEmptyRow = false;
    
    // Ajout des 10 paramètres avec leurs descripteurs complets
    // ...
    
    return descriptor;
}
```

### 2.2 Créer les autres descripteurs de modules Patch Edit

Créer les fichiers suivants sur le même modèle que Dco1ModuleDescriptors.h :
- `Dco2ModuleDescriptors.h`
- `VcfVcaModuleDescriptors.h`
- `FmTrackModuleDescriptors.h` (avec `hasEmptyRow = true`)
- `RampPortamentoModuleDescriptors.h`
- `Env1ModuleDescriptors.h`
- `Env2ModuleDescriptors.h`
- `Env3ModuleDescriptors.h`
- `Lfo1ModuleDescriptors.h`
- `Lfo2ModuleDescriptors.h`

### 2.3 Créer les descripteurs Master Edit

Créer dans `Source/GUI/Descriptors/MasterEdit/` :
- `MidiModuleDescriptors.h` (8 paramètres, pas de boutons)
- `VibratoModuleDescriptors.h` (7 paramètres, pas de boutons)
- `MiscModuleDescriptors.h` (7 paramètres, pas de boutons)

## Phase 3 : Adaptation de ModulePanel

### 3.1 Adapter `ModulePanel` pour utiliser `ModuleDescriptor`

**Fichier :** `Source/GUI/Panels/MainPanel/ModulePanel.h` et `.cpp`

**Changements :**
- Remplacer le constructeur actuel qui prend `ParameterConfig` par un constructeur qui prend `ModuleDescriptor`
- Adapter `createParameterRow()` pour utiliser `ParameterDescriptor` au lieu de `ParameterConfig`
- Gérer les ComboBox avec leurs choix depuis `ParameterDescriptor.choices`
- Gérer la ligne vide optionnelle (`hasEmptyRow`)

**Nouveau constructeur :**
```cpp
explicit ModulePanel(const ModuleDescriptor& descriptor, Skin& skin);
```

### 3.2 Gestion des ComboBox

Adapter `createParameterRow()` pour :
- Créer un `ComboBox` avec les choix définis dans `ParameterDescriptor.choices`
- Définir la valeur par défaut via `defaultChoiceIndex`
- Connecter le ComboBox à l'APVTS (préparation pour Phase 4)

## Phase 4 : Helper pour conversion ModuleDescriptor → ParameterConfig

Créer une fonction helper (optionnelle, pour compatibilité) :
```cpp
std::vector<ParameterConfig> convertToParameterConfig(const ModuleDescriptor& descriptor);
```

## Phase 5 : Intégration avec APVTS (préparation)

### 5.1 Créer `PluginParameterFactory.h/cpp`

**Localisation :** `Source/Business/PluginParameterFactory.h` et `.cpp`

**Objectif :** Factory pour créer les paramètres APVTS à partir des descripteurs

**Méthodes prévues :**
```cpp
class PluginParameterFactory {
public:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    static void addModuleParameters(
        juce::AudioProcessorValueTreeState::ParameterLayout& layout,
        const ModuleDescriptor& module);
private:
    static std::unique_ptr<juce::AudioParameterFloat> createSliderParameter(
        const ParameterDescriptor& param);
    static std::unique_ptr<juce::AudioParameterChoice> createComboBoxParameter(
        const ParameterDescriptor& param);
};
```

**Note :** Cette phase sera implémentée plus tard, mais la structure des descripteurs doit la préparer.

## Points d'attention

1. **Noms de fichiers :** Utiliser `Descriptors` (pas `Descriptor`) pour être cohérent avec le pluriel
2. **Organisation des includes :** Les descripteurs de modules incluent `SynthDescriptors.h`, pas l'inverse
3. **Valeurs par défaut :** Les ComboBox utilisent `defaultChoiceIndex` pour définir la valeur par défaut
4. **Ligne vide :** Le module FM/TRACK a `hasEmptyRow = true`, les autres `false`
5. **Boutons :** Les modules Master Edit ont `buttonLabels` vide
6. **SysEx :** Tous les paramètres incluent `sysExOffset` et `sysExParameterId` pour la communication MIDI

## Ordre d'implémentation recommandé

1. Créer `SynthDescriptors.h` avec les structures de base
2. Créer `Dco1ModuleDescriptors.h` comme exemple complet
3. Adapter `ModulePanel` pour utiliser `ModuleDescriptor`
4. Tester avec DCO 1
5. Créer les autres descripteurs de modules progressivement
6. Préparer `PluginParameterFactory` (structure de base, implémentation complète plus tard)

## Questions résolues

- **ModulePanel vs ModulePanelFactory :** Garder `ModulePanel` comme nom (c'est un Component). La Factory sera `PluginParameterFactory` pour créer les paramètres APVTS.
- **Organisation :** Approche hiérarchique validée avec `SynthDescriptors.h` + fichiers par module dans des sous-dossiers.