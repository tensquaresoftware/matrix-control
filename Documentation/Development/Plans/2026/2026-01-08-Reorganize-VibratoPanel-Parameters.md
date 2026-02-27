---
name: Réorganiser les paramètres VibratoPanel
overview: "Réorganiser l'ordre des paramètres dans VibratoPanel pour suivre l'ordre spécifié : Speed, Waveform, Amplitude, Speed Mod Source, Speed Mod Amount, Amp Mod Source, Amp Mod Amount."
todos:
  - id: reorder-constructor
    content: Réorganiser les blocs de création des widgets dans le constructeur de VibratoPanel.cpp selon le nouvel ordre
    status: pending
  - id: reorder-resized
    content: Réorganiser les blocs de positionnement dans resized() de VibratoPanel.cpp selon le nouvel ordre
    status: pending
  - id: reorder-settheme
    content: Réorganiser les appels dans setTheme() de VibratoPanel.cpp selon le nouvel ordre
    status: pending
---

Réorganiser l'ordre des paramètres dans `VibratoPanel` pour correspondre à l'ordre demandé.

## Ordre actuel vs ordre souhaité

**Ordre actuel :**

1. Speed
2. Speed Mod Source
3. Speed Mod Amount
4. Waveform
5. Amplitude
6. Amp Mod Source
7. Amp Mod Amount

**Ordre souhaité :**

1. Speed
2. Waveform
3. Amplitude
4. Speed Mod Source
5. Speed Mod Amount
6. Amp Mod Source
7. Amp Mod Amount

## Modifications à effectuer

### Fichier : `Source/GUI/Panels/BodyPanel/MasterEditPanel/Modules/VibratoPanel.cpp`

1. **Dans le constructeur** : Réorganiser les blocs de création des widgets et leurs séparateurs pour suivre le nouvel ordre :

- Conserver Speed en premier (lignes 26-40)
- Déplacer Waveform après Speed (actuellement lignes 74-88)
- Déplacer Amplitude après Waveform (actuellement lignes 90-104)
- Déplacer Speed Mod Source après Amplitude (actuellement lignes 42-56)
- Déplacer Speed Mod Amount après Speed Mod Source (actuellement lignes 58-72)
- Conserver Amp Mod Source et Amp Mod Amount en fin (lignes 106-136)

2. **Dans `resized()`** : Réorganiser les blocs de positionnement pour correspondre au nouvel ordre :

- Conserver Speed en premier (lignes 168-179)
- Déplacer Waveform après Speed (actuellement lignes 207-218)
- Déplacer Amplitude après Waveform (actuellement lignes 220-231)
- Déplacer Speed Mod Source après Amplitude (actuellement lignes 181-192)
- Déplacer Speed Mod Amount après Speed Mod Source (actuellement lignes 194-205)
- Conserver Amp Mod Source et Amp Mod Amount en fin (lignes 233-255)

3. **Dans `setTheme()`** : Réorganiser les appels pour correspondre au nouvel ordre :

- Conserver Speed en premier (lignes 265-272)
- Déplacer Waveform après Speed (actuellement lignes 292-299)
- Déplacer Amplitude après Waveform (actuellement lignes 301-308)
- Déplacer Speed Mod Source après Amplitude (actuellement lignes 274-281)
- Déplacer Speed Mod Amount après Speed Mod Source (actuellement lignes 283-290)
- Conserver Amp Mod Source et Amp Mod Amount en fin (lignes 310-326)

## Notes

- Les séparateurs (`horizontalSeparator1` à `horizontalSeparator7`) doivent suivre le même ordre que les paramètres qu'ils séparent.
- L'ordre des déclarations dans le header (`VibratoPanel.h`) peut rester tel quel, car l'ordre de déclaration n'affecte pas le fonctionnement.
- Aucune modification des IDs de paramètres ou des attachments n'est nécessaire, seule la séquence d'affichage change.