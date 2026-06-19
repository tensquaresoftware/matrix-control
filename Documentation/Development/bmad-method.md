---
organization: Ten Square Software
project: All
title: BMad Method — Development Cycle Quick Reference
author: Guillaume DUPONT
status: reference
version: "1.2"
sources:
  - https://github.com/bmad-code-org/bmad-method
created: 2026-06-17
updated: 2026-06-19
---

# BMad Method — Development Cycle Quick Reference

Reference guide for [the **BMad** method](https://github.com/bmad-code-org/bmad-method) by Brian Madison:

*"**B**uild **M**ore **A**rchitect **D**reams" / "**B**reakthrough **M**ethod for **A**gile AI-Driven **D**evelopment"*

Recommended setup: chat in your preferred language with the AI assistant; deliverables in `_bmad-output/` in English.

**Command syntax**: `/bmad-help`, `/bmad-dev-story`, etc. Natural-language requests work too when they clearly match a skill.

---

## Overview

```
Analysis (optional) → Planning → Solution design → Implementation
                                              ↓
                                    Sprint → [Story → Dev → Review → UAT] × N
```

Each **skill** is a guided workflow. Ideally use a **new conversation** per major step.

### Starting a project with BMad

**Greenfield** (starting from scratch): Analysis *(if needed)* → PRD → Architecture → Epics; then `/bmad-generate-project-context` once stack and conventions are established.

**Brownfield** (code already exists): `/bmad-generate-project-context` and/or `/bmad-document-project` before or alongside planning; then continue with PRD, architecture, and epics as on greenfield.

### Why this order?

Mirrors a product/tech team: *why* first, then *what*, *how*, and finally *in what order to code* — rather than unstructured “vibe-coding” without guardrails.

---

## BMad command catalogue

When unsure: invoke **`/bmad-help`** — analyses project state and recommends the next skill.

### Phase 1 — Analysis *(optional)*

| Command | Code | Role |
|---------|------|------|
| `/bmad-brainstorming` | BP | Guided ideation when the concept is still fuzzy. |
| `/bmad-market-research` | MR | Competition, customers, and market framing. |
| `/bmad-domain-research` | DR | Industry/domain vocabulary and subject-matter depth. |
| `/bmad-technical-research` | TR | Feasibility, stack options, and technical approaches. |
| `/bmad-product-brief` | CB | Lightweight product summary when the concept is already mature. |
| `/bmad-prfaq` | WB | *Working Backwards* PRFAQ to stress-test the concept before the PRD. |

### Phase 2 — Planning

| Command | Code | Role |
|---------|------|------|
| `/bmad-prd` | PRD | Create, update, or validate the Product Requirements Document. |
| `/bmad-ux` | CU | UX specifications when the UI is central to the product. |

### Phase 3 — Solution design

| Command | Code | Role |
|---------|------|------|
| `/bmad-create-architecture` | CA | Technical architecture and structural decisions. |
| `/bmad-create-epics-and-stories` | CE | Epics and stories breakdown. |
| `/bmad-check-implementation-readiness` | IR | Alignment check across PRD, UX, architecture, and stories before coding. |

### Phase 4 — Implementation

| Command | Code | Role |
|---------|------|------|
| `/bmad-sprint-planning` | SP† | Build or refresh `sprint-status.yaml`. |
| `/bmad-sprint-status` | SS | Sprint summary and routing to the next workflow. |
| `/bmad-create-story` | CS | Draft the next (or a targeted) story with full dev context. |
| `/bmad-create-story validate` | VS | Validate story completeness and ACs before dev. |
| `/bmad-dev-story` | DS | Implement the story, run tests, update the story file. |
| `/bmad-code-review` | CR | Adversarial code review (Blind Hunter, Edge Case Hunter, Acceptance Auditor). |
| `/bmad-checkpoint-preview` | CK | Guided human review of a commit, branch, or PR. |
| `/bmad-qa-generate-e2e-tests` | QA | Generate automated API/E2E tests *(not a substitute for CR or UAT)*. |
| `/bmad-retrospective` | ER | End-of-epic retrospective and lessons learned. |

> † **Note:** menu code **SP** means *Sprint Planning* here; `/bmad-spec` also uses SP in some catalogues — different skill — see *Anytime — documents & quality* below.

### Anytime — development & change

| Command | Code | Role |
|---------|------|------|
| `/bmad-help` | BH | Orientation: where you are and what to run next. |
| `/bmad-quick-dev` | QQ | Intent → plan → code → review outside a full story. |
| `/bmad-correct-course` | CC | Major scope or direction change; sprint change proposal. |
| `/bmad-investigate` | IN | Structured forensic investigation of a bug or incident. |
| `/bmad-generate-project-context` | GPC | Generate `project-context.md` from the codebase (brownfield). |
| `/bmad-document-project` | DP | Document an existing project for agent context. |
| `/bmad-party-mode` | PM | Multi-agent roundtable on a cross-cutting topic. |

### Anytime — documents & quality

| Command | Code | Role |
|---------|------|------|
| `/bmad-spec` | SP | Distil intent into a concise SPEC contract (+ validation mode). |
| `/bmad-review-adversarial-general` | AR | Standalone cynical review of a document or artefact. |
| `/bmad-review-edge-case-hunter` | ECH | Method-driven edge-case review (orthogonal to AR). |
| `/bmad-editorial-review-prose` | EP | Copy-editing pass on written content. |
| `/bmad-editorial-review-structure` | ES | Structure, cuts, and reorganisation of a long document. |
| `/bmad-index-docs` | ID | Build or refresh an index of docs in a folder. |
| `/bmad-shard-doc` | SD | Split an oversized markdown doc into smaller files. |
| `/bmad-customize` | BC | Customise agent/workflow behaviour via `_bmad/custom/`. |
| `/bmad-advanced-elicitation` | — | Deep critique and refinement of recent agent output. |

### Tech writer (Paige) — `/bmad-agent-tech-writer`

| Invocation | Code | Role |
|------------|------|------|
| write action | WD | Draft or rewrite documentation to standards. |
| mermaid action | MG | Generate a Mermaid diagram from a description. |
| validate action | VD | Review a document against documentation standards. |
| explain action | EC | Explain a technical concept with examples. |
| update-standards action | US | Update Paige's documentation standards memory. |

### Named agent personas *(optional entry points)*

Not standalone workflows — conversational shortcuts to certain skills:

| Command | Persona | Typical use |
|---------|---------|-------------|
| `/bmad-agent-analyst` | Mary | Business analysis, research framing. |
| `/bmad-agent-pm` | John | Product vision and PRD coaching. |
| `/bmad-agent-ux-designer` | Sally | UX flows and interface specs. |
| `/bmad-agent-architect` | Winston | Architecture trade-offs. |
| `/bmad-agent-dev` | Amelia | Story implementation focus. |
| `/bmad-agent-tech-writer` | Paige | Documentation (see table above). |

Prefer the **workflow command** (`/bmad-dev-story`, `/bmad-code-review`, …) over a persona when one exists.

---

## Quality assurance — four complementary layers

Automated tests and code review alone are not enough. Think of **four different checks**, from fastest to closest to real usage:

1. **Unit tests** — one isolated piece (a function, a rule). Fast, but often in an empty or simplified environment.
2. **Code review (CR)** — a second look at the **diff** and the spec, hunting for gaps or mistakes in the code written.
3. **Integration tests** — several pieces wired together (as in the running app) to see if the whole still behaves correctly.
4. **Smoke test / UAT** — run the app (or test it like a user) and manually verify the important paths.

Each layer catches what the previous one may miss:

| Layer | In plain terms | What it often misses |
|-------|----------------|----------------------|
| **Unit** | “Is this rule correct on its own?” | Behaviour when everything is wired together; deferred effects (timers, callbacks…) |
| **Review (CR)** | “Does the code match the story? Any obvious omissions?” | What only shows up when running the app or connecting to the real world |
| **Integration** | “When A calls B which updates C, is the outcome right?” | User feel, external hardware, production conditions |
| **Smoke / UAT** | “When I actually use it, does it work for me?” | Every possible case — not an exhaustive automated suite |

### Concrete example

You add a button that should send **one** request to the server. Tests pass, code review is clean, the story moves to `done`. Then manual testing shows **ten requests** firing at once.

Why? Often because:
- the code *looks* correct on read-through;
- tests did not reproduce the real setup (data, state, sequencing);
- something fires **a little later** (after a delay, a screen refresh…) and tests did not wait for it.

**To avoid this**, in your Definition of Done:
- whenever a story touches **data, files, network, or session**: add at least one **replayable integration test** (“one action → one expected effect”);
- before closing an epic: run a **short manual checklist** on critical paths;
- keep three distinct roles: **CR** = the code; **QA** (`/bmad-qa-generate-e2e-tests`) = replayable automation; **UAT** = you (or a tester) validating the finished product.

---

## Story loop and Git

Here **story** = any **BMad work item** tracked in `sprint-status.yaml` and materialised by a story file under `_bmad-output/` (often a *user story* with Given / When / Then ACs, but also technical stories, bug fixes, refactors, spikes, hygiene/utility stories…). `/bmad-create-story` and `/bmad-dev-story` apply to **all of them**; only the file content differs.

```
CS → VS → DS → CR → [UAT if required] → done → CS (next) or ER (epic end)
```

**UAT** (*User Acceptance Testing*): **no** `/bmad-uat` command. A **manual** step — real paths, product checklist, validation of user-visible ACs. Usually **after CR** and **before** `done` in `sprint-status.yaml`. Distinct from `/bmad-code-review` (diff review) and `/bmad-qa-generate-e2e-tests` (automation).

| Step | Commit? | Notes |
|------|---------|-------|
| `create-story` | Optional | Spec-only commit if you want a locked story before code. |
| `dev-story` (during) | **No** | Preserves `baseline_commit` for the review diff. |
| `code-review` → `done` | **Yes — preferred** | One coherent unit: code + tests + story + `sprint-status.yaml`. |

Push when the story is `done` (or when opening a PR). Use a **different LLM** for `/bmad-code-review` than for `/bmad-dev-story`, in a **fresh conversation**.

Commit messages: follow the **project convention** (language, imperative, bullets — often documented in `project-context.md` or `CONVENTIONS.md`).

---

## When things drift

| Situation | Code | Command |
|-----------|------|---------|
| CR feedback | DS | `/bmad-dev-story` until clean |
| Small same-scope tweak | DS or QQ | `/bmad-dev-story` or `/bmad-quick-dev` |
| New cross-cutting scope | CC | `/bmad-correct-course` then update PRD / architecture / stories |
| Bug / incident | IN → DS | `/bmad-investigate` then `/bmad-dev-story` or a new story |
| Multiple viewpoints | PM | `/bmad-party-mode` |
| Urgent fix outside sprint | QQ | `/bmad-quick-dev` |

**Rule:** same story, same intent → DS (or QQ if tiny). New intent or architectural impact → CC first. Do not let code and `_bmad-output/` diverge.

---

## Terminology

### Workflow and artefacts

| Term | Meaning |
|------|---------|
| **Skill** | Workflow invoked via `/bmad-…` (e.g. dev-story, code-review). |
| **Workflow** | Step sequence a skill runs (create-story, dev, review…). |
| **Persona** | Named agent (Mary, Amelia…) — conversational shortcut, not a full workflow. |
| **`_bmad/`** | Versioned BMad install (config, scripts, team customisations). |
| **`_bmad-output/`** | Generated deliverables (PRD, epics, stories, sprint-status…). |
| **`planning_artifacts`** | Subfolder (often `_bmad-output/planning-artifacts/`): PRD, architecture, epics. |
| **`implementation_artifacts`** | Subfolder (often `_bmad-output/implementation-artifacts/`): story files, specs, deferred-work. |
| **`project-context.md`** | Project “constitution” for agents: stack, conventions, prohibitions. |
| **`sprint-status.yaml`** | Sprint dashboard: status of each epic and story. |
| **`baseline_commit`** | Reference commit at the start of a story’s dev — used for the CR diff. |

### Planning and breakdown

| Term | Meaning |
|------|---------|
| **PRD** | *Product Requirements Document* — product specification. |
| **Epic** | Large functional batch grouping several stories (e.g. epic 4 = patch management). |
| **Story** | BMad work item (file + sprint entry) — user story or other (tech, bug fix, spike…). |
| **AC** | *Acceptance Criteria* — Given / When / Then validation points. |
| **SPEC** | Short contract produced by `/bmad-spec` to lock intent before coding. |
| **Spike** | Exploratory technical story (feasibility) — little or no final product deliverable. |
| **Greenfield** | New project — starting from scratch. |
| **Brownfield** | Existing project — code is already there; document before planning. |
| **IR** | *Implementation Readiness* — alignment check before coding (`/bmad-check-implementation-readiness`). |
| **CC** | *Correct Course* — major direction change (`/bmad-correct-course`). |

### Implementation and statuses

| Term | Meaning |
|------|---------|
| **`backlog` → `ready-for-dev` → `in-progress` → `review` → `done`** | Story lifecycle in `sprint-status.yaml`. |
| **DoD** | *Definition of Done* — “truly finished” checklist (tests, UAT, docs…). |
| **Fresh conversation** | New chat session for a step (especially CR ≠ dev). |
| **Deferred work** | Work deliberately postponed (`deferred-work.md`) — out of scope for the current story. |

### Quality and review

| Term | Meaning |
|------|---------|
| **Diff** | List of **code changes** (lines added, removed, changed) between two states — in BMad, mainly between `baseline_commit` and the story’s current state. Visible in Git (`git diff`), a PR (*Files changed*), or the editor Source Control tab. |
| **CR** | *Code Review* — `/bmad-code-review`, adversarial review of the diff. |
| **Adversarial review** | Review that actively hunts for problems (not a casual “LGTM”). |
| **Blind Hunter** | CR layer: analyses the diff without story bias (what the code actually does). |
| **Edge Case Hunter** | CR layer: boundary conditions and rare paths. |
| **Acceptance Auditor** | CR layer: are the story and ACs actually covered? |
| **UAT** | *User Acceptance Testing* — manual user validation; no slash command. |
| **Smoke test** | Quick manual check: “do the essentials work in real conditions?” |
| **QA** | Here: generated automated tests (`/bmad-qa-generate-e2e-tests`) — distinct from CR and UAT. |
| **CR triage** | Sorting review findings: **decision needed**, **fix to apply**, **defer**. |

Full BMad vocabulary: [official docs](https://docs.bmad-method.org/llms.txt).

### Software team ↔ BMad personas

| Role | BMad equivalent |
|------|-----------------|
| Product Manager | John + `/bmad-prd` |
| Business Analyst | Mary + research / brief skills |
| Architect | Winston + `/bmad-create-architecture` |
| UX Designer | Sally + `/bmad-ux` |
| Developer | Amelia + `/bmad-dev-story` |
| QA / Test | `/bmad-qa-generate-e2e-tests` + UAT + smoke |
| Support / Ops | `/bmad-investigate`, CC feedback, or new stories |

---

## Recommendations

1. **New conversation** per major skill.
2. Keep **`project-context.md`** and project conventions up to date.
3. Update **`sprint-status.yaml`** after each story.
4. **Four-layer quality**: unit + integration where runtime matters + CR + smoke/UAT before epic sign-off.
5. **No automatic commits** — request explicitly after CR.
6. **`/bmad-help`** when in doubt.
7. **One story at a time** — next story only after CR (and UAT if required).

---

*BMad Method quick reference — updated 2026-06-19*
