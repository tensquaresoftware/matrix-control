# Sprint Change Proposal — Solo Dev Workflow & CI Gate Relaxation

**Project:** Matrix-Control  
**Date:** 2026-07-14  
**Author:** Correct Course workflow (Batch mode — user pre-approved)  
**Change signal:** Post–Epic 11 feedback — PR-per-story + required CI checks slow solo BMad loop; macOS-first velocity preferred  
**Source registry:** Epic 11 stories 11-1–11-3, CONTRIBUTING.md CI section, branch protection (`release-script-tests`, `ci-success`), D-047-R  
**Scope classification:** **Minor** — policy and documentation; no CI infrastructure rollback

---

## 1. Issue Summary

### 1.1 Problem Statement

After Epic 11, every user story follows **dev → PR → wait for CI (~5–12 min) → code-review → merge**. As a **solo maintainer** developing primarily on macOS (MBP M5) with AI-assisted iteration, this gate adds friction without proportional benefit on each story.

Guillaume's actual quality bar:

- **Daily:** local macOS Debug build + `Matrix-Control_Tests` (fast feedback).
- **Periodic:** full 3-OS CI matrix as a **safety net**, not a per-story blocker.
- **Release:** CI green + manual smoke before tagging (unchanged).

Epic 11 remains valuable — the issue is **coupling** the infrastructure to a **team-style merge gate**, not the infrastructure itself.

### 1.2 Trigger Type

- **Process mismatch** — team-oriented PR + required checks applied to solo workflow.
- **Not a technical failure** — CI works; Story 11-3 fast tier helped but full tier (~12 min) still blocks merge when checks are required.

### 1.3 Evidence

| Source | Finding |
|--------|---------|
| User feedback (2026-07-14) | Waiting on CI per story slows throughput; pre–Epic 11 direct pushes to `main` did not block progress |
| Story 11-3 | Fast tier ~5 min (draft PR); full tier ~12 min — still a context switch per story |
| Sprint change 2026-07-11 | Cross-platform compile failures (Win/Linux) justify **having** CI, not **blocking every merge** |
| Branch protection (pre-change) | Required: `release-script-tests`, `ci-success` |

---

## 2. Impact Analysis

### 2.1 Epic Impact

| Epic | Impact |
|------|--------|
| **Epic 11** | **Complete as-is** — CI/CD infrastructure retained; merge **policy** decoupled from story cycle |
| **Epics 7, 8, U** | **Unblocked for velocity** — default path: commit/push to `main`, local macOS gate |
| **Epic 9–10** | No change |
| **Future contributors** | CONTRIBUTING documents two paths: solo maintainer vs external PR |

No new epic. No rollback of workflows or release pipeline.

### 2.2 PRD / MVP Impact

**MVP unchanged.** NFR-1 and SM-6 still satisfied — CI runs full matrix on push to `main` and on PRs when opened.

| Requirement | Before | After |
|-------------|--------|-------|
| **NFR-1** | Unit tests in CI | Unchanged |
| **SM-6** | 3-OS green in CI | Unchanged — **release gate** still requires green CI before tag |
| **D-047-R** | 3-OS matrix | Unchanged — matrix still runs; not required before every push |

### 2.3 Architecture Impact

**[N/A]** — no code or spine invariant changes.

### 2.4 Technical Impact

| Artifact | Change |
|----------|--------|
| GitHub branch protection | Remove **required** status checks (`release-script-tests`, `ci-success`); CI remains informational |
| `CONTRIBUTING.md` | Solo fast lane + when to use PRs + release gate unchanged |
| `_bmad-output/project-context.md` | BMad story cycle without mandatory PR |
| `.decision-log.md` | **D-047-S** — solo merge policy |
| `epics.md` | Epic 11 note: infrastructure gate → periodic assurance |

### 2.5 Risk Assessment

| Risk | Mitigation |
|------|------------|
| Win/Linux compile breaks sit on `main` temporarily | Acceptable pre-v1 solo; fix when CI notifies; batch before release |
| Forgetting to check CI | End-of-epic or pre-release checklist; release workflow still runs tests |
| External contributor merges broken `main` | PR path documented for forks; maintainer can re-enable checks if team grows |

**Effort:** Low (~1 session). **Risk:** Low for solo pre-release.

---

## 3. Recommended Approach

**Selected: Option 1 — Direct Adjustment** (not rollback, not MVP change)

1. **Keep** Epic 11 CI/CD assets (workflows, fast tier, release pipeline).
2. **Remove** required status checks on `main` — pushes and merges no longer blocked by CI.
3. **Adopt** solo fast lane: local macOS build/tests → commit → push `main`; CI runs async.
4. **Retain** PR + full CI as **optional** for risky changes (CMake, toolchain, large refactors) or external contributors.
5. **Retain** release gate: tag only when CI green on target commit.

**Rationale:** Preserves Epic 11 investment; restores pre–Epic 11 velocity; aligns BMad loop with solo reality.

---

## 4. Detailed Change Proposals

### 4.1 GitHub — branch protection

**OLD:** Required checks `release-script-tests`, `ci-success` (strict).

**NEW:** No required status checks. Branch protection may remain for other settings; CI jobs still run and report on Actions tab.

**Implementation:**

```bash
gh api repos/tensquaresoftware/matrix-control/branches/main/protection/required_status_checks \
  -X PATCH \
  --input - <<'EOF'
{
  "strict": false,
  "contexts": []
}
EOF
```

### 4.2 CONTRIBUTING.md — Continuous Integration section

**OLD:** Merge gate requires checks; solo tip mentions parallel CI wait.

**NEW:**

- **Solo maintainer (default):** push to `main`; local macOS preset + tests before commit; CI informational.
- **PR path (optional):** draft PR for fast tier during review; `ci-full` or ready-for-review for full matrix before merge.
- **Release gate:** CI green on release commit before tag (unchanged).
- Remove or rewrite "Branch protection update" as **historical / optional re-enable** for team mode.

### 4.3 project-context.md — Git & BMad cycle

Add **Solo development workflow** under Git conventions:

- Default: direct commits to `main` after local macOS build + tests.
- `bmad-dev-story` → local verify → `bmad-code-review` in chat → commit/push (no mandatory PR).
- PR when external contribution or high-risk cross-platform change.
- Periodic: ensure `main` CI green before release tags.

### 4.4 Decision log — D-047-S

**Decision:** Solo merge policy — CI as periodic assurance, not per-story gate.

**Rationale:** Epic 11 matrix retained; branch protection required checks removed 2026-07-14 after velocity feedback.

**Follow-up:** Re-enable required checks if multi-contributor workflow adopted.

### 4.5 epics.md — Epic 11 note

**OLD:** `Blocks: confident merge gate for Epics 7, 8, U`

**NEW:** `Provides: periodic 3-OS CI assurance; solo default is local macOS gate + async CI on main`

---

## 5. Implementation Handoff

**Scope:** Minor — Developer agent implements immediately.

| Task | Owner | Status |
|------|-------|--------|
| Patch branch protection (remove required checks) | Agent | Done (2026-07-14) |
| Update CONTRIBUTING.md | Agent | Done |
| Update project-context.md | Agent | Done |
| Add D-047-S to decision log | Agent | Done |
| Update epics.md Epic 11 note | Agent | Done |
| Mark Epic 11 `done` in sprint-status.yaml | Agent | Done |

**Success criteria:**

- Push to `main` succeeds without waiting for CI checks.
- CI still runs on push to `main` (full 3-OS matrix).
- Documentation reflects solo fast lane and release gate.
- Guillaume resumes story work with `/bmad-create-story` / `/bmad-dev-story` without PR ceremony.

**Approved by:** Guillaume — 2026-07-14 ("OK pour le bmad-correct-course avec assouplissement de la protection de branche").

---

## Checklist Summary

| Section | Status |
|---------|--------|
| 1 — Trigger & context | [x] Done |
| 2 — Epic impact | [x] Done — no epic rollback |
| 3 — Artifact conflicts | [x] Done — docs only |
| 4 — Path forward | [x] Direct Adjustment |
| 5 — Proposal components | [x] Done |
| 6 — Approval | [x] User approved |
