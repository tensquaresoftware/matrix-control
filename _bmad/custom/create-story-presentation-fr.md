# Create Story — contrat de présentation (Matrix-Control)

Ce document s’applique à **toute** sortie utilisateur de `bmad-create-story` (sélection, questions, résumé de la story créée, prochaines étapes).

**Barème partagé (obligatoire) :** charger et appliquer `clarity-bar-fr.md` en premier.

**Rappel phrase-test :** si Guillaume dirait « Reformule en langage naturel, sans jargon excessif », **réécrire** avant d’envoyer — y compris pour **chaque** question et décision. SSOT : `clarity-bar-fr.md` + `.cursor/rules/bmad-clarity-bar.mdc`.

---

## Langue et artifacts

- **Chat** → français, phrases complètes, barème de clarté.
- **Fichier story** généré → anglais (`document_output_language`).
- Ne jamais coller un extrait dense de la story dans le chat sans le reformuler.

## Structure des messages

1. **En bref** — ce qui se passe (quelle story, où on en est).
2. **Détail** — en langage accessible (pas de jargon de workflow non traduit).
3. Si décision / question à options → **Recommandation BMad** (option + pourquoi + confiance).

| Interne | Dire à Guillaume |
|---------|------------------|
| `backlog` / `ready-for-dev` | en attente / prêt à développer |
| Acceptance criteria | critères d’acceptance |
| epic context | contexte de l’epic |
| Dev Notes | notes pour le développeur |

## Fin de workflow

Résumer en français clair : story créée, chemin du fichier, statut « prêt à développer », et proposer la suite (souvent Quick Dev ou Dev Story) **avec recommandation**.
