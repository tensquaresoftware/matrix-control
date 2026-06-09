---
description: Processus 3 phases, principes de design, limites Clean Code
alwaysApply: true
---

# Process & Clean Code

Sources : `CONVENTIONS.md` §2–§4, §6.16 — référence complète : `Documentation/Development/principes-dev-qualite.md`.

## Avant tout code — 3 phases obligatoires

1. **DESIGN** — SRP, duplication (DRY/WET), abstractions **YAGNI**, solution **KISS**, choix **ETC** (facile à modifier), noms explicites, dépendances, méthodes < 15 lignes.
2. **IMPLEMENTATION** — Suivre le design. Dépassement de limite → **STOP** et extraire.
3. **AUTO-REVIEW** — SOLID + §4.5 (KISS, YAGNI, ETC, Boy Scout, CQS) + limites §3. Échec → retour phase 1.

**Exception :** prototype / POC / spike explicite → phase 1 skippable ; proposer refactor Clean + tests Core après validation.

## Principes de design (prioritaires)

| Principe | Consigne agent |
|---|---|
| **KISS** | Solution la plus simple qui fonctionne |
| **YAGNI** | Uniquement le besoin du story / FR en cours |
| **ETC** | Chaque choix facilite le prochain changement probable |
| **WET → DRY** | Dupliquer une fois OK ; factoriser quand duplication **stable** (≥ 2e occurrence confirmée) |
| **Boy Scout** | Fichier touché un peu plus propre qu'à l'arrivée |
| **CQS** | Fonction = commande (mute) **ou** requête (lit) — pas les deux |

**En cas de conflit :** correctness & thread-safety > KISS/YAGNI > SOLID > DRY prématuré.

## Limites strictes (non négociables)

| Métrique | Max |
|---|---|
| Fonction / méthode | 15 lignes (20 si orchestration `show()` qui délègue) |
| Classe | 200 lignes |
| Paramètres | 3 (sinon struct dédiée) |
| Complexité cyclomatique | < 5 |
| Niveaux d'indentation | 2 |
| Duplication | WET OK une fois ; extraire si stable (3e occurrence ou confirmée) |

## Signaux d'alarme

- « C'est un peu long mais ça va » → refactor
- Abstraction « pour le futur » sans besoin actuel → YAGNI, simplifier
- Nom de fonction introuvable → trop de responsabilités
- Commentaire « Part 1 / Part 2 » → chaque partie = fonction séparée

## Règle d'or

Un critère §3, §4 ou §6.16 échoue → ne pas présenter le code ; refactorer d'abord.
