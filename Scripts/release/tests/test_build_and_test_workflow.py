"""Structure tests for .github/workflows/build-and-test.yml (Story 11.3)."""

from __future__ import annotations

from pathlib import Path

import yaml

PROJECT_ROOT = Path(__file__).resolve().parents[3]
BUILD_AND_TEST_WORKFLOW = PROJECT_ROOT / ".github" / "workflows" / "build-and-test.yml"
RELEASE_WORKFLOW = PROJECT_ROOT / ".github" / "workflows" / "release.yml"


def _load_workflow(path: Path) -> dict:
    return yaml.safe_load(path.read_text(encoding="utf-8"))


def test_build_and_test_workflow_structure():
    workflow_text = BUILD_AND_TEST_WORKFLOW.read_text(encoding="utf-8")
    workflow = _load_workflow(BUILD_AND_TEST_WORKFLOW)
    triggers = workflow.get("on") or workflow.get(True) or {}

    assert "push" in triggers
    assert triggers["push"]["branches"] == ["main"]
    assert "pull_request" in triggers
    assert triggers["pull_request"]["branches"] == ["main"]
    pr_types = triggers["pull_request"]["types"]
    assert "ready_for_review" in pr_types
    assert "synchronize" in pr_types
    assert "labeled" in pr_types

    jobs = workflow["jobs"]
    assert set(jobs) >= {
        "release-script-tests",
        "resolve-ci-tier",
        "build-and-test",
        "ci-success",
    }

    assert jobs["resolve-ci-tier"]["needs"] == "release-script-tests"
    assert "matrix" in jobs["resolve-ci-tier"]["outputs"]
    assert "tier" in jobs["resolve-ci-tier"]["outputs"]

    build_job = jobs["build-and-test"]
    assert build_job["needs"] == ["release-script-tests", "resolve-ci-tier"]
    assert "fromJSON(needs.resolve-ci-tier.outputs.matrix)" in str(
        build_job["strategy"]["matrix"]
    )
    assert build_job["strategy"]["fail-fast"] is False

    ci_success = jobs["ci-success"]
    assert ci_success["name"] == "ci-success"
    assert ci_success["needs"] == [
        "release-script-tests",
        "resolve-ci-tier",
        "build-and-test",
    ]
    assert "needs.build-and-test.result == 'success'" in ci_success["if"]

    assert "macos-debug-arm64" in workflow_text
    assert "windows-debug" in workflow_text
    assert "linux-debug" in workflow_text
    assert "MATRIX_BUILD_TESTS=ON" in workflow_text
    assert "USER_COPY_TO_SYSTEM_FOLDERS=OFF" in workflow_text
    assert "USER_COPY_TO_ARTEFACTS_DIR=OFF" in workflow_text


def test_release_workflow_unchanged_by_story_11_3():
    """AC6 regression guard: release.yml must not be modified by Story 11.3."""
    release_text = RELEASE_WORKFLOW.read_text(encoding="utf-8")
    release = _load_workflow(RELEASE_WORKFLOW)
    triggers = release.get("on") or release.get(True) or {}

    assert "pull_request" not in triggers
    assert "tags" in triggers["push"]
    assert "v*.*.*" in triggers["push"]["tags"]
    assert set(release["jobs"]) == {"validate-tag", "build", "publish"}
    assert "notarytool submit" in release_text


def test_resolve_tier_logic_documented_in_workflow():
    workflow_text = BUILD_AND_TEST_WORKFLOW.read_text(encoding="utf-8")
    assert "ci-full" in workflow_text
    assert 'tier="fast"' in workflow_text or "tier=fast" in workflow_text
    assert "ready_for_review" in workflow_text
    assert "gh pr view" in workflow_text
    assert "GH_TOKEN" in workflow_text
    assert 'any(. == "ci-full")' in workflow_text
