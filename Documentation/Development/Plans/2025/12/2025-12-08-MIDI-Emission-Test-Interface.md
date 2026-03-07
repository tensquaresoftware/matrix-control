---
name: Interface de test émission MIDI
overview: Supprimer tous les widgets de test GUI, conserver logSysExDataDecimal() et la correction F0/F7, redimensionner la fenêtre à 1335x730, ajouter le label de version en bas à gauche, et vérifier l'indépendance des classes MIDI.
todos:
  - id: "1"
    content: Corriger l'organisation des includes dans MidiSender.cpp (headers système → JUCE → projet)
    status: completed
  - id: "2"
    content: Corriger l'organisation des includes dans MidiReceiver.cpp
    status: completed
  - id: "3"
    content: Corriger l'organisation des includes dans MidiLogger.cpp
    status: completed
  - id: "4"
    content: Corriger l'organisation des includes dans MidiManager.cpp
    status: completed
  - id: "5"
    content: Corriger l'organisation des includes dans SysExParser.cpp
    status: completed
  - id: "6"
    content: Corriger l'organisation des includes dans SysExEncoder.cpp
    status: completed
  - id: "7"
    content: Corriger l'organisation des includes dans SysExDecoder.cpp
    status: completed
  - id: "8"
    content: Vérifier et corriger l'espacement autour des opérateurs dans tous les fichiers
    status: completed
  - id: create-patch-model
    content: Créer PatchModel.h/.cpp pour stocker les données de patch (134 bytes)
    status: completed
  - id: create-master-model
    content: Créer MasterModel.h/.cpp pour stocker les données de master (172 bytes)
    status: completed
  - id: add-decimal-logging
    content: Ajouter méthode logSysExDataDecimal() dans MidiLogger pour afficher les données en décimal
    status: completed
  - id: add-gui-controls
    content: Ajouter les contrôles GUI dans PluginEditor.h (labels, boutons, modèles)
    status: completed
    dependencies:
      - create-patch-model
      - create-master-model
  - id: implement-patch-navigation
    content: Implémenter la navigation de patch (Previous/Next) avec envoi Program Change
    status: completed
    dependencies:
      - add-gui-controls
  - id: implement-file-loading
    content: Implémenter le chargement de fichiers .syx avec décodage et stockage dans les modèles
    status: completed
    dependencies:
      - add-gui-controls
      - add-decimal-logging
  - id: implement-sending
    content: Implémenter l'envoi de Patch/Master avec encodage SysEx
    status: completed
    dependencies:
      - add-gui-controls
  - id: update-layout
    content: Mettre à jour le layout de PluginEditor pour afficher les 3 lignes de contrôles
    status: completed
    dependencies:
      - add-gui-controls
  - id: todo-1764709175310-xjy7bh6yu
    content: Corriger l'organisation des includes dans MidiSender.cpp (headers système → JUCE → projet)
    status: completed
  - id: todo-1764709175311-29pi7aef3
    content: Corriger l'organisation des includes dans MidiReceiver.cpp
    status: completed
  - id: todo-1764709175311-w788wcktw
    content: Corriger l'organisation des includes dans MidiLogger.cpp
    status: completed
  - id: todo-1764709175311-rlha1arbc
    content: Corriger l'organisation des includes dans MidiManager.cpp
    status: completed
  - id: todo-1764709175311-hcj15v2bg
    content: Corriger l'organisation des includes dans SysExParser.cpp
    status: completed
  - id: todo-1764709175311-3bjggu7d5
    content: Corriger l'organisation des includes dans SysExEncoder.cpp
    status: completed
  - id: todo-1764709175311-1x4wxnntr
    content: Corriger l'organisation des includes dans SysExDecoder.cpp
    status: completed
  - id: todo-1764709175311-r1e3lryix
    content: Vérifier et corriger l'espacement autour des opérateurs dans tous les fichiers
    status: completed
  - id: todo-1764709175311-yujg1ixfn
    content: Créer PatchModel.h/.cpp pour stocker les données de patch (134 bytes)
    status: completed
  - id: todo-1764709175311-16apodzbc
    content: Créer MasterModel.h/.cpp pour stocker les données de master (172 bytes)
    status: completed
  - id: todo-1764709175311-ui49s5kew
    content: Ajouter méthode logSysExDataDecimal() dans MidiLogger pour afficher les données en décimal
    status: completed
  - id: todo-1764709175311-4tfd3c21d
    content: Ajouter les contrôles GUI dans PluginEditor.h (labels, boutons, modèles)
    status: completed
  - id: todo-1764709175311-cd3gqtruy
    content: Implémenter la navigation de patch (Previous/Next) avec envoi Program Change
    status: completed
  - id: todo-1764709175311-vilv7dggj
    content: Implémenter le chargement de fichiers .syx avec décodage et stockage dans les modèles
    status: completed
  - id: todo-1764709175311-1i5xbl2at
    content: Implémenter l'envoi de Patch/Master avec encodage SysEx
    status: completed
  - id: todo-1764709175311-uvjxfejcx
    content: Mettre à jour le layout de PluginEditor pour afficher les 3 lignes de contrôles
    status: completed
---

# Plan de purge GUI et vérification architecture MIDI

## 1. Suppression des widgets de test dans PluginEditor

### 1.1 PluginEditor.h
- Supprimer l'héritage `juce::Label::Listener` (ligne 17)
- Supprimer la méthode `labelTextChanged()` (ligne 27)
- Supprimer les includes `Business/Models/MasterModel.h` et `Business/Models/PatchModel.h` (lignes 11-12)
- Supprimer les méthodes privées de test (lignes 41-49) :
  - `setupTestControls()`
  - `updatePatchNumberDisplay()`
  - `onPreviousPatchClicked()`
  - `onNextPatchClicked()`
  - `onLoadPatchClicked()`
  - `onLoadMasterClicked()`
  - `onSendPatchClicked()`
  - `onSendMasterClicked()`
  - `loadSysExFile()`
- Supprimer les membres de test (lignes 59-76) :
  - `currentPatchNumber`
  - `patchModel`
  - `masterModel`
  - `patchNumberLabel`
  - `patchNumberEditor`
  - `previousPatchButton`
  - `nextPatchButton`
  - `loadPatchButton`
  - `loadMasterButton`
  - `sendPatchButton`
  - `sendMasterButton`
  - `fileChooser`
- Conserver `versionLabel` mais le repositionner en bas à gauche

### 1.2 PluginEditor.cpp
- Supprimer les includes `Business/Models/MasterModel.h` et `Business/Models/PatchModel.h` (lignes 7-8)
- Supprimer les initialisations dans le constructeur (lignes 17-28) :
  - `currentPatchNumber(0)`
  - `patchModel(std::make_unique<PatchModel>())`
  - `masterModel(std::make_unique<MasterModel>())`
  - `patchNumberLabel(...)`
  - `patchNumberEditor(...)`
  - `previousPatchButton(...)`
  - `nextPatchButton(...)`
  - `loadPatchButton(...)`
  - `loadMasterButton(...)`
  - `sendPatchButton(...)`
  - `sendMasterButton(...)`
- Conserver `versionLabel("VersionLabel", "v0.0.5")` mais modifier le texte pour récupérer la version depuis PluginProcessor
- Supprimer l'appel à `setupTestControls()` dans le constructeur (ligne 32)
- Supprimer les `removeListener()` dans le destructeur (lignes 42-48)
- Supprimer la méthode `labelTextChanged()` entière (lignes 174-190)
- Supprimer la méthode `buttonClicked()` et la remplacer par une version vide ou supprimer complètement si elle n'est plus utilisée
- Nettoyer `resized()` pour supprimer tout le code des widgets de test (lignes 87-131)
- Supprimer toutes les méthodes de test (lignes 338-547) :
  - `setupTestControls()`
  - `updatePatchNumberDisplay()`
  - `onPreviousPatchClicked()`
  - `onNextPatchClicked()`
  - `onLoadPatchClicked()`
  - `onLoadMasterClicked()`
  - `loadSysExFile()`
  - `onSendPatchClicked()`
  - `onSendMasterClicked()`
- Modifier `setSize()` pour utiliser 1335x730 (ligne 35)
- Ajouter le positionnement de `versionLabel` en bas à gauche dans `resized()` avec police Arial, 8px, gris clair

## 2. Suppression des fichiers de modèles

### 2.1 Supprimer les fichiers
- `Source/Business/Models/PatchModel.h`
- `Source/Business/Models/PatchModel.cpp`
- `Source/Business/Models/MasterModel.h`
- `Source/Business/Models/MasterModel.cpp`

### 2.2 CMakeLists.txt
- Supprimer les lignes 40-41 :
  ```cmake
  Source/Business/Models/MasterModel.cpp
  Source/Business/Models/PatchModel.cpp
  ```

## 3. Suppression des getters dans MidiManager

### 3.1 MidiManager.h
- Supprimer les getters (lignes 38-40) :
  ```cpp
  SysExParser* getSysExParser() noexcept { return sysExParser.get(); }
  SysExDecoder* getSysExDecoder() noexcept { return sysExDecoder.get(); }
  SysExEncoder* getSysExEncoder() noexcept { return sysExEncoder.get(); }
  ```

## 4. Configuration du label de version

### 4.1 PluginEditor.cpp - Constructeur
- Initialiser `versionLabel` avec le texte de version récupéré depuis `PluginProcessor::getVersionString()` ou utiliser "v0.0.5" en dur pour l'instant

### 4.2 PluginEditor.cpp - resized()
- Positionner `versionLabel` dans le coin inférieur gauche :
  ```cpp
  versionLabel.setFont(juce::Font("Arial", 8.0f, juce::Font::plain));
  versionLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
  versionLabel.setJustificationType(juce::Justification::bottomLeft);
  versionLabel.setBounds(10, getHeight() - 20, 100, 20);
  ```

## 5. Vérification de l'indépendance des classes MIDI

### 5.1 Classes à vérifier
- `MidiInputPort` : doit être indépendante
- `MidiOutputPort` : doit être indépendante
- `MidiSender` : dépend de `MidiLogger` (OK, singleton utilitaire)
- `MidiReceiver` : dépend de `SysExConstants` (OK, constantes) et `MidiLogger` (OK)
- `SysExParser` : dépend de `SysExConstants` (OK) et `MidiLogger` (OK)
- `SysExDecoder` : dépend de `SysExParser` via référence (OK, injection de dépendance)
- `SysExEncoder` : dépend de `SysExConstants` (OK) et `MidiLogger` (OK)

### 5.2 Vérifications à effectuer
- Aucune classe ne doit inclure directement une autre classe métier MIDI (sauf via MidiManager)
- Les dépendances acceptables sont :
  - `SysExConstants` (constantes partagées)
  - `MidiLogger` (singleton utilitaire)
  - `Exceptions` (exceptions partagées)
  - Types JUCE standard
- `SysExDecoder` peut dépendre de `SysExParser` via référence (injection de dépendance)

## 6. Nettoyage du code buttonClicked()

### 6.1 PluginEditor.cpp
- Si `buttonClicked()` n'est plus utilisée après suppression des boutons de test, supprimer complètement la méthode et l'héritage `juce::Button::Listener` de `PluginEditor.h`

## Fichiers à modifier

**Fichiers à modifier :**
- `Source/PluginEditor.h`
- `Source/PluginEditor.cpp`
- `Source/Business/MIDI/MidiManager.h`
- `CMakeLists.txt`

**Fichiers à supprimer :**
- `Source/Business/Models/PatchModel.h`
- `Source/Business/Models/PatchModel.cpp`
- `Source/Business/Models/MasterModel.h`
- `Source/Business/Models/MasterModel.cpp`

**Fichiers à vérifier (pas de modification nécessaire) :**
- `Source/Business/MIDI/Transport/MidiSender.cpp` (conserver correction F0/F7)
- `Source/Business/MIDI/Utilities/MidiLogger.h/.cpp` (conserver logSysExDataDecimal)