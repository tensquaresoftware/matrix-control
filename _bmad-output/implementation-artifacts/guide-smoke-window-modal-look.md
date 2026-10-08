---
organization: Ten Square Software
project: Matrix-Control
title: Notes
author: Guillaume DUPONT
started: 2026-09-29
updated: 2026-09-30
---

# Smoke fenêtres et modales — Matrix-Control

Guide de parcours après le look Matrix des fenêtres et modales.
Ouvre ce fichier dans Typora et coche au fur et à mesure.
Pour chaque case : look Matrix (chrome, corps Montserrat lisible en minuscules, boutons MAJUSCULES style GUI), Escape ferme ou annule, Enter active le bouton principal quand c’est prévu.

## Avant de commencer

- [x] Build Debug macOS à jour installé (Standalone + AU ou VST3)
- [x] Synth ou MIDI simulé prêt si tu touches DEVICE SETUP / bank / Master

## Standalone seulement

- [x] Barre de titre native OS (pas une barre dessinée JUCE)
- [x] Glisser la fenêtre sur un second écran : elle y reste sans revenir d’un coup
- [x] Logo → Audio/MIDI Settings (ou Cmd/Ctrl+Alt+virgule) : titre AUDIO SETTINGS, chrome Matrix
- [x] Audio Settings : pas de liste Active MIDI inputs, pas de MIDI Output
- [x] Audio Settings : pas de Feedback Loop, pas de Mute audio input, pas de bannière bleue
- [x] Audio Settings : bouton TEST (Matrix, majuscules) joue le son de test
- [x] Audio Settings : PeakIndicator à droite du TEST réagit au signal (AUDIO FROM actif aide)
- [x] Audio Settings : changer d’interface audio garde sample rate / buffer si l’appareil les propose encore
- [x] Audio Settings : Escape ou clic hors cadre ferme ; à la réouverture pas de timer fantôme bizarre
- [x] AUDIO FROM = None : silence logiciel sans case Mute dans Audio Settings

## Commun Standalone et plugin — overlays

- [x] Settings (engrenage) : chrome Matrix, boutons Matrix majuscules
- [x] About : chrome Matrix
- [x] Fermer Settings / About avec Escape ou le bouton fermer

## DEVICE SETUP

- [x] Première session ou reset machine defaults : DEVICE SETUP s’ouvre (chrome Matrix)
- [x] Boutons CONFIRM et SPECIFY LATER en Matrix majuscules
- [x] Escape / SPECIFY LATER se comporte comme avant

## Master

- [x] Settings → Master Init d’un module : RESET MASTER MODULE? corps Montserrat, RESET / CANCEL Matrix
- [x] Settings → reset global Master : RESET ALL MASTER MODULES? même look
- [x] Charger un fichier .m1km : Load .m1km Master? avec choix MASTER SETTINGS ONLY / FULL MASTER / CANCEL Matrix

## Patch Mutator

- [x] Delete sur une mutation (avec warning actif) : Delete mutation? Matrix + option don’t ask again
- [x] Delete : CANCEL à gauche, DELETE à droite ; Escape = cancel ; Enter = DELETE
- [x] Clear history (flush) : Flush mutation history? Matrix, codes inchangés
- [x] Settings → Defrag history : Defrag mutation history? Matrix, DEFRAG / CANCEL

## Patch et fichiers (confirms Matrix)

- [x] Quitter / changer de patch avec unsaved : Unsaved patch Matrix (CANCEL / DISCARD / STORE ou SAVE…)
- [x] Reconciliation nom interne vs fichier : Patch name mismatch Matrix
- [x] Save As avec nom invalide : Invalid patch file name Matrix, bouton OK
- [x] Delete init template (patch ou master) : Delete init template? Matrix

## Banks

- [x] Import bank : Import bank? Matrix
- [x] Paste bank : Paste bank? Matrix
- [x] Export vers dossier existant : Replace export folder? Matrix
- [x] Overwrite .syx sibling : Overwrite existing .syx? Matrix
- [x] Pendant un transfert bank : dialogue de progression Matrix, CANCEL Matrix ; Escape annule si activé

## FileChooser OS (ne doit PAS être Matrix)

- [x] Ouvrir un dossier (import bank / export folder) : picker système OS
- [x] Save As / Open fichier .syx : picker système OS
- [x] Après Cancel du picker, l’UI Matrix revient correctement au premier plan

## Plugin hôte (AU ou VST3)

- [x] Pas de changement de chrome de fenêtre hôte (titre = hôte)
- [x] Settings / About / confirms / Mutator Delete / Master Init : même look Matrix que standalone
- [x] Audio Settings Matrix absent ou inerte (chemin standalone seulement)
- [x] FileChooser reste OS aussi en plugin

## Raccourcis clavier sur une confirm Matrix

- [x] Escape = cancel (code 0)
- [x] Enter = action principale (code 1)
- [x] Si trois boutons : ordre visuel CANCEL → milieu → primaire à droite

## Lenovo plus tard (Windows / Linux)

- [ ] Barre de titre native
- [ ] Drag multi-écran sans snap-back
- [ ] Une confirm Matrix + Audio Settings + un FileChooser OS

## Suivi smoke macOS (2026-09-30) — décisions + backlog

### Décisions tranchées

- [x] Ponctuation : règles anglaises partout (supprimer espaces avant ? ! :)
- [x] Audio safety : paquet scène complet (sync + défaut None + invalidation fantômes)
- [x] Combos vs barre de titre native : reporter → **corrigé et testé** (conversations parallèles)
- [x] Prochain polish modales : layout commun + copy/wrapping + About Montserrat corps → **livré + smoke OK**
- [x] Audio Settings : viser chrome Matrix complet (pas seulement footer) — faisabilité OK (rebuild) — **chantier 4 encore à Build**

### Audio / sécurité monitoring

- [x] Corriger désync AUDIO FROM ↔ Input Audio Settings (Larsen)
- [x] Premier démarrage : Input = None et AUDIO FROM = NONE
- [x] Invalider / nettoyer les entrées fantômes quand le device Input change
- [ ] Correctif smoke 2026-10-02 : après Scarlett éteinte, AUDIO FROM reste None (pas de réarmement préféré Matrix) ; labels catalogue = nom Input JUCE (pas « Haut-parleurs » pour un micro) ; Output None au premier lancement si l’OS l’accepte ; **Input/Output Audio Settings → None si le device sauvé a disparu** (pas de repli Micro/Haut-parleurs)

### Profils périphériques audio (après safety)

- [ ] Configurer Scarlett (canaux + rate + buffer) → vérifier `AudioDeviceProfiles/profiles.xml` contient ces valeurs
- [ ] Éteindre Scarlett → relancer → Input/Output None ; AUDIO FROM None
- [ ] Rallumer Scarlett → relancer → Input/Output + rate/buffer/canaux restaurés automatiquement ; AUDIO FROM reste None
- [ ] Autre génération / nom OS différent → pas de restauration de ce profil
- [ ] Purge manuelle v1 : supprimer `~/Library/Application Support/Ten Square Software/Matrix-Control/AudioDeviceProfiles/` (ou `profiles.xml`)
- [ ] Re-pick dans Audio Settings restaure aussi si besoin ; un restore raté n'écrase pas le profil disque

### GETTING STARTED (wizard multi-étapes — remplace Device Setup one-shot)

SSOT produit : `Documentation/Development/Plans/2026/10/2026-10-08-Getting-Started-Wizard-Decisions.md`

- [ ] Chrome Matrix monochrome, largeur = Settings, hauteur revue à la baisse (peu de contrôles / étape)
- [ ] Étapes : 0 intro → 1 UI Scale + Skin → 2 Synth From/To + DEVICE + EPROM → 3 Keyboard (Standalone : combo + Skip ; plugin : informatif, pas de combo) → 4 Audio Standalone only (driver / I/O / SYNTH FROM ; FE+buffer si place)
- [ ] Flags par étape + reprise ciblée ; Settings → User Interface : UI SCALE, SKIN, …, GETTING STARTED (combo SHOW WHEN INCOMPLETE / NEVER AT LAUNCH + RUN SETUP AGAIN)
- [ ] Configure later : un rappel puis silence ; réarmement si nouvel applicable (ex. premier Standalone / Audio)
- [ ] Smoke UltraWide / HiDPI : Scale en STEP 1 avant les étapes denses

### Audio Settings — rebuild Matrix (chantier dédié)

- [ ] Remplacer labels / combos / toggles canaux exclusifs / TEST / meter JUCE par équivalents Matrix
- [ ] PeakIndicator à la hauteur de la combo Input ; TEST + Peak dans la rangée Input (pas en bas à droite)
- [ ] Garder sample rate / buffer / output / type device

### Polish modales (prochain chantier)

- [x] Layout commun (titres caps, boutons centrés, inset texte ~10% ; mismatch aligné CANCEL, densité, Don't ask again) — implémenté + smoke OK (conversations parallèles)
- [x] About : Montserrat corps ; titre marque inchangé — implémenté + smoke OK (conversations parallèles)
- [x] Retouches DEVICE SETUP / m1km / Flush / mismatch / Delete init — implémenté + smoke OK (conversations parallèles)
- [x] Nettoyage ponctuation anglaise (SKIN: etc.)
- [x] Chrome détail : bande titre noire 24 px (= bouton), titre gris bouton, gaps 24 px titre / dernier contenu / boutons, paragraphes `\n\n` — 2026-09-30 ; règles SSOT `guide-matrix-modal-design.md`
- [x] Combos vs barre de titre native : fermeture au clic titre — livré + testé (conversations parallèles ; n’était plus « reporter »)

### Ordre d'exécution validé (2026-09-30 / mis à jour 2026-10-02)

1. Ponctuation anglaise
2. Polish modales + About Montserrat
3. Audio safety scène
4. **Profils périphériques audio** (capture / restore sur resélection explicite)
5. GETTING STARTED (wizard + Settings UI Scale/Skin/bloc)
6. Rebuild Audio Settings Matrix
