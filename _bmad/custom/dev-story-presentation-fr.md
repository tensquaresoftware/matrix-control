# Dev Story — contrat de présentation (Matrix-Control)

Ce document s’applique à **toute** sortie utilisateur de `bmad-dev-story` (démarrage, questions de clarification, bilans d’avancement, completion).

**Barème partagé (obligatoire) :** charger et appliquer `clarity-bar-fr.md` en premier.

**Rappel phrase-test :** si Guillaume dirait « Reformule en langage naturel, sans jargon excessif », **réécrire** avant d’envoyer — y compris pour **chaque** question, blocage et décision. SSOT : `clarity-bar-fr.md` + `.cursor/rules/bmad-clarity-bar.mdc`.

---

## Langue et artifacts

- **Chat** → français, phrases complètes, barème de clarté.
- **Mises à jour du fichier story** (cases à cocher, Dev Agent Record, etc.) → anglais dans le fichier.
- Ne jamais coller un log technique brut ou un output de build dense sans reformulation accessible.

## Structure des messages

### Démarrage / reprise

1. **En bref** — quelle story, où on reprend.
2. **Prochaine action** — en français simple.
3. Si choix (quelle story, comment traiter un blocage) → **Recommandation BMad**.

### Pendant l’implémentation (si message à Guillaume)

- Expliquer le fond (ce qui change pour le produit / le plugin), pas seulement les fichiers touchés.
- Preuves techniques en ligne complémentaire.

### Completion

1. **En bref** — story terminée et prête pour revue, en une phrase produit.
2. **Ce qui a changé** — points principaux accessibles.
3. **Suite** — proposer la revue (`bmad-code-review` ou équivalent) avec **Recommandation BMad** si plusieurs options.

| Interne | Dire à Guillaume |
|---------|------------------|
| `ready-for-dev` / `in-progress` / `review` | prêt à développer / en cours / prêt pour revue |
| AC / Acceptance criteria | critères d’acceptance |
| File List | liste des fichiers touchés |
| baseline commit | point de départ git de la story |

## Blocages et décisions

Toute question à options (dépendance manquante, ambiguïté d’AC, écart au plan) doit inclure **Recommandation BMad** sans que Guillaume la demande.
