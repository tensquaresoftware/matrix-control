# Forged idea — Skin Editor (post Skin SSOT)

## Locked

- **Job V1** : inventer / explorer (palette + mappings Skin SSOT) ; structure pour skin **nommé** Dev chargeable ; premier usage réel = travailler à partir de Black/Cream.
- **Seuil utile** : Duplicate / créer / supprimer / sélectionner skin Dev + paint live Standalone + Unbound actionnable. **JSON export/import** = cible ; **filet** = shipper sans I/O fichier si déborde (JSON = lot suivant). Pas de parque total pour cause JSON.
- **Forme** : app **Standalone JUCE** séparée (*Matrix-Control Skin Editor*), widgets natifs ; workflow laptop plein écran Matrix-Control + éditeur sur écran externe.
- **Contrat** : télécommande **pousse-modèle seulement** (palette + mappings + choix skin) → Standalone Dev applique / repeint. Pas d’aperçu GUI produit dans l’éditeur. Pas d’IDE skins bidirectionnel (undo partagé, etc.) en V1.
- **Surface vérité** : **Standalone Dev seulement** (pas plugin-in-DAW auteur V1).
- **Repo** : **même repo** Matrix-Control, **cible CMake Dev** séparée (pas repo séparé V1).
- **Vérité données** : **C++ = SSOT produit** ; éditeur = **calque runtime Dev** (+ JSON = save/load de ce calque). Intégration skin « fier » dans le produit = étape séparée. JSON ≠ nouvelle SSOT Release.
- **Black/Cream compilés** : **lecture seule** ; toute modif = **Duplicate** vers skin Dev d’abord.
- **Unbound** : central V1 — liste / relier **dans l’éditeur** (texte, noms objet × propriété × état). Pas d’overlay Unbound sur le paint Matrix-Control.
- **Priorité chantier** : éditeur d’abord (puis peaufinage fier via éditeur → intégration C++).
- **Filet alpha** : alpha multi-OS avec Black/Cream Skin SSOT actuels ; éditeur **non bloquant**.
- **Héritage** : Skin SSOT fait ; Skin Studio gelé (pas Lot C / pas coquille web V1) ; contributeurs après V1 ; pas de 3ᵉ système couleur.

## Rejected

- Éditeur = seulement calibrer Black/Cream, ou outiller contributeurs en V1.
- In-app panneau d’édition mélangé au layout Release (favorite écartée).
- UI web / Skin Studio comme coquille V1.
- Preview / GUI reconstituée / highlight Unbound sur Matrix-Control.
- IPC auteur vers plugin dans un DAW (V1).
- Sync riche / undo partagé éditeur↔plugin (V1).
- JSON comme SSOT produit / skins fichier sur chemin Release (V1).
- Édition en place de Black/Cream compilés.
- Repo séparé V1 (sans module `skin-model` partagé).
- Alpha otage de l’éditeur.

## Why (downstream)

- Vérité visuelle = paint JUCE Standalone ; l’outil auteur ne doit pas re-peindre le produit.
- Pollution Dev = receiver mince + cible séparée, pas un second Skin Studio.
- Calque Dev évite de rouvrir la porte modèle ; C++ reste ce qui ship.
- Duplicate-only sur BC protège la SSOT mentale produit.

## Next

- **`bmad-spec`** : Skin Editor **uniquement** (contrat IPC mince, calque runtime, UI Unbound texte, cible CMake, filets JSON/alpha). Entrée : ce fichier.
- Pas de `bmad-build` tant que la spec n’a pas verrouillé : forme du pont, schéma calque/JSON Dev, inventaire écrans éditeur, critères de fin seuil-sans-JSON vs lot JSON.
- Ne pas relancer spec / build Skin SSOT modèle ni Lot C Skin Studio.
