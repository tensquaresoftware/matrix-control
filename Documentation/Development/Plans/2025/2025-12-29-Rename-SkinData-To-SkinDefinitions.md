---
name: Rename SkinData to SkinDefinitions
overview: Renommer le fichier SkinData.h en SkinDefinitions.h et mettre à jour toutes les références.
todos: []
---

# Renommer SkinData.h en SkinDefinitions.h



## Objectif

Renommer `SkinData.h` en `SkinDefinitions.h` pour mieux refléter son rôle de fichier regroupant les définitions du Skin (couleurs, dimensions, polices).

## Modifications

### 1. Créer le nouveau fichier

- Créer `Source/UI/Skin/SkinDefinitions.h` avec le même contenu que `SkinData.h` actuel :
  ```cpp
    #pragma once
    
    #include "SkinColours.h"
    #include "SkinDimensions.h"
    #include "SkinFonts.h"
  ```




### 2. Mettre à jour les références

- Dans `Source/UI/Skin/Skin.h` (ligne 5) : remplacer `#include "SkinData.h"` par `#include "SkinDefinitions.h"`

### 3. Supprimer l'ancien fichier

- Supprimer `Source/UI/Skin/SkinData.h`

## Fichiers concernés

- `Source/UI/Skin/SkinData.h` → `Source/UI/Skin/SkinDefinitions.h` (nouveau fichier)