# Barème de clarté BMad (Matrix-Control) — SSOT

Ce document est la **référence unique** pour le chat avec Guillaume pendant :

- `bmad-code-review`
- `bmad-quick-dev` (y compris la revue interne et le bilan)
- `bmad-create-story`
- `bmad-dev-story`

Il prime sur le style des sous-agents, des étapes internes et de tout jargon d’ingénierie.

---

## Phrase-test (non négociable)

Avant d’envoyer **n’importe quel** message important (constat de revue, question, décision, résumé d’avancement, bilan), se demander :

> Si Guillaume répondait maintenant « Reformule en langage naturel, sans jargon excessif », est-ce que je réécrirais ce texte ?

- **Oui** → le message n’est **pas** prêt. Réécrire.
- **Non** → on peut envoyer.

La **première** version doit déjà être au niveau de cette reformulation. Guillaume ne doit **jamais** avoir à la demander.

---

## Public cible

Écrire comme une explication orale à un **producteur de musique qui lit le code occasionnellement** :

- il connaît le produit et l’intention ;
- il ne veut pas décoder une pile de chemins, d’IDs techniques et d’acronymes ;
- il veut comprendre **en ~20 secondes** ce qu’on lui demande de trancher.

---

## Règles de langue et de ton

1. **Toujours le français** dans le chat (même si sous-agents / diffs / specs sont en anglais).
2. **Phrases complètes**, registre parlé — interdit le style télégraphique et les listes de mots-clés.
3. **Pas de jargon excessif.** Terme technique indispensable → glose courte à la première occurrence  
   (ex. « APVTS = stockage des paramètres du plugin »).
4. **Preuves techniques en second.** Chemins `fichier:ligne`, numéros de critères, pixels, noms de classes :  
   **une ligne complémentaire en bas**, jamais empilés dans la phrase qui porte le sens.
5. **Partir de l’intention produit** (« On devait… » / « Tu dois choisir si… »), pas des fichiers.

---

## Auto-contrôle avant envoi (tout cocher)

1. Un lecteur qui ignore les noms de fichiers comprend-il l’enjeu en ~20 secondes ?
2. Pour une décision : le titre est-il une **question claire** en français simple ?
3. A-t-on enlevé au moins une couche de jargon / références techniques de la phrase principale ?
4. Les options se distinguent-elles avec des **verbes du quotidien**, sans connaître le code ?
5. La phrase-test ci-dessus donne-t-elle « Non, je ne reformulerais pas » ?

Si un seul point échoue → **réécrire** avant d’envoyer.

---

## Décisions — Recommandation BMad obligatoire

Dès que Guillaume doit choisir (même une petite question à options), ajouter **toujours** :

### Recommandation BMad

- **Option recommandée** — numéro + intitulé court
- **Pourquoi** — 2 à 4 phrases **au même barème de clarté**
- **Confiance** — `ferme` ou `nuancée`

Il reste maître de la réponse. Il ne doit jamais avoir à demander la recommandation.

Critères de choix : (1) risque utilisateur / stabilité, (2) `project-context.md` et conventions du dépôt, (3) périmètre demandé (pas de sur-ingénierie).

---

## Artifacts vs chat

- **Chat** → français, barème ci-dessus.
- **Fichiers générés** (story, spec, code map, Review Findings techniques) → anglais (`document_output_language`), sauf demande contraire.
- Ne **jamais** coller tel quel un bloc dense d’artifact / sous-agent dans le chat : toujours reformuler au niveau « phrase-test ».
