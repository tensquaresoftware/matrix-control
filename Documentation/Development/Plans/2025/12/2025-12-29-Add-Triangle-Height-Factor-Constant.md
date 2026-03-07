---
name: Add triangle height factor constant
overview: Ajouter la constante kComboBoxTriangleHeightFactor dans SkinDimensions.h et l'utiliser dans ComboBox.cpp pour remplacer la valeur magique 0.866f
todos: []
---

# Ajouter la constante kComboBoxTriangleHeightFactor

## Modifications

### 1. Ajouter la constante dans SkinDimensions.h

Dans `[Source/UI/Skin/SkinDimensions.h](Source/UI/Skin/SkinDimensions.h)`, ajouter la constante juste après `kComboBoxTriangleBaseSize` :

```cpp
inline constexpr float kComboBoxTriangleBaseSize = 7.0f;
inline constexpr float kComboBoxTriangleHeightFactor = 0.8660254f; // √3 / 2
```

Cette constante représente le facteur de hauteur d'un triangle équilatéral (√3 / 2 ≈ 0.8660254).

### 2. Utiliser la constante dans ComboBox.cpp

Dans `[Source/UI/Widgets/ComboBox.cpp](Source/UI/Widgets/ComboBox.cpp)`, remplacer les deux occurrences de `0.866f` :

- Ligne 100 dans `drawTriangle()` :
  ```cpp
  auto triangleHeight = triangleBaseSize * tss::SkinDimensions::Widget::kComboBoxTriangleHeightFactor;
  ```

- Ligne 113 dans `createTrianglePath()` :
  ```cpp
  auto height = baseSize * tss::SkinDimensions::Widget::kComboBoxTriangleHeightFactor;
  ```

Vérifier que `SkinDimensions.h` est bien inclus dans `ComboBox.cpp` (il devrait déjà l'être via les includes existants).