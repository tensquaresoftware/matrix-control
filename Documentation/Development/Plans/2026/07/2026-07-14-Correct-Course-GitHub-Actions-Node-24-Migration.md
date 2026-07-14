# Correct Course — GitHub Actions Node 24 migration

**Date:** 2026-07-14  
**Author:** Guillaume (with agent)  
**Status:** Implemented  
**Trigger:** Node.js 20 deprecation annotations on GitHub Actions CI runs (Build and Test #19, 2026-07-14)  
**Scope classification:** **Minor** — workflow pin bumps + documentation; no product or architecture change  
**Related:** Epic 11 (CI infrastructure), commit `8ac1354`, D-047-T

---

## 1. Executive summary

GitHub Actions began defaulting JavaScript actions to **Node.js 24** on hosted runners (from 2026-06-16). Workflows still pinned first-party actions at **v4** (`checkout`, `cache`, `upload-artifact`, `download-artifact`), which declare **Node 20**. CI remained green but every job emitted Annotations warnings:

> *Node.js 20 is deprecated. The following actions target Node.js 20 but are being forced to run on Node.js 24.*

**Decision:** Bump first-party actions to their Node-24 majors now, document pinned versions, and avoid `download-artifact@v8` until its breaking changes are explicitly needed.

**Outcome:** Warnings cleared on next CI run; workflows aligned before Node 20 removal from runners (fall 2026).

---

## 2. Problem statement

### 2.1 Trigger

| Source | Finding |
|--------|---------|
| GitHub Actions run #19 (`70d866e`) | 4× Node 20 deprecation warnings across `release-script-tests` and `build-and-test` matrix legs |
| GitHub changelog (2025-09, updated 2026-02) | Node 20 EOL April 2026; runners default to Node 24 from June 2026; Node 20 removed fall 2026 |
| `.github/workflows/*.yml` | All first-party JS actions at `@v4` (Node 20 runtime) |

### 2.2 Non-issues (no action)

| Annotation | Notes |
|------------|-------|
| `macos-latest` migration notice (macOS 14) | Generic GitHub infrastructure notice; unrelated to Node migration |
| `sudara/basic-macos-keychain-action@v1` | Composite action (shell/bash); not subject to Node runtime deprecation |

### 2.3 Constraints

- **GitHub-hosted runners only** — no self-hosted runners in repo today.
- **No GHES** — github.com; artifact action v6/v7 supported.
- **Release workflow** must keep upload/download artifact compatibility (pack → publish job chain unchanged).

---

## 3. Decision record

### D-047-T — Pin GitHub Actions to Node 24 runtimes

- **Decision:** Upgrade first-party actions to Node-24 majors:

  | Action | From | To | Workflow(s) |
  |--------|------|-----|-------------|
  | `actions/checkout` | `@v4` | `@v5` | Build and Test, Release |
  | `actions/cache` | `@v4` | `@v5` | Build and Test, Release |
  | `actions/upload-artifact` | `@v4` | `@v6` | Release |
  | `actions/download-artifact` | `@v4` | `@v7` | Release |

- **Rationale:** Proactive compliance before Node 20 runner removal; eliminates misleading Annotations noise; no behaviour change expected on GitHub-hosted runners (runner ≥ 2.327.1).
- **Explicit non-choice:** Stay on `@v4` + ignore warnings — rejected (technical debt with hard deadline).
- **Explicit non-choice:** Jump to `download-artifact@v8` — deferred (digest mismatch defaults to `error`, ESM migration; v7 sufficient for Node 24).
- **Follow-up:** Revisit pins when GitHub emits new deprecation annotations; require runner ≥ 2.327.1 if self-hosted runners are introduced.

---

## 4. Impact analysis

| Area | Impact |
|------|--------|
| **Epics / stories** | None — Epic 11 infrastructure unchanged in scope |
| **PRD / NFR-1** | Unchanged — same build + test matrix |
| **Architecture / product** | [N/A] |
| **Release pipeline** | Same artefact names and paths; upload/download API compatible at v6/v7 |
| **Contributors** | `CONTRIBUTING.md` documents pins and upgrade policy |

**Risk:** Low. **Effort:** One session (workflow edit + docs).

---

## 5. Implementation

### 5.1 Code changes

| Artifact | Change | Status |
|----------|--------|--------|
| `.github/workflows/build-and-test.yml` | `checkout@v5`, `cache@v5` | Done (`8ac1354`) |
| `.github/workflows/release.yml` | `checkout@v5`, `cache@v5`, `upload-artifact@v6`, `download-artifact@v7` | Done (`8ac1354`) |

### 5.2 Documentation

| Artifact | Change | Status |
|----------|--------|--------|
| `CONTRIBUTING.md` | New § **GitHub Actions maintenance** (pins, upgrade policy, third-party note) | Done |
| `_bmad-output/project-context.md` | One-line pointer to CONTRIBUTING § GitHub Actions maintenance | Done |
| `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md` | **D-047-T** | Done |
| This plan | Archived under `Documentation/Development/Plans/2026/07/` | Done |

### 5.3 Verification

- [x] Next **Build and Test** run on `main` shows no Node 20 deprecation Annotations (confirmed 2026-07-14).
- [ ] Next **Release** tag run (when applicable) upload/download artefacts succeed unchanged.

---

## 6. Upgrade playbook (future)

When GitHub shows new deprecation warnings:

1. Identify affected `uses:` lines in `.github/workflows/`.
2. Check each action's release notes for the first major declaring `runs.using: node24`.
3. Bump one major at a time; prefer minimal jump (e.g. v7 before v8 for download-artifact).
4. Update `CONTRIBUTING.md` pin table and this plan's decision table if versions change.
5. For composite/third-party actions, verify `action.yml` — only JavaScript/TypeScript actions need Node runtime bumps.

---

## 7. Approval

**Approved by:** Guillaume — 2026-07-14 (migrate to Node 24; document in work docs + correct course).

---

## Checklist summary

| Section | Status |
|---------|--------|
| 1 — Trigger & context | [x] Done |
| 2 — Impact | [x] Minor — CI pins only |
| 3 — Decision (D-047-T) | [x] Done |
| 4 — Workflow bumps | [x] Done (`8ac1354`) |
| 5 — Documentation | [x] Done |
| 6 — CI verification | [x] Build and Test green, no Node 20 warnings (2026-07-14) |
