"""Wiring tests for local quality gate scripts and CI integration."""

from __future__ import annotations

from pathlib import Path

import yaml

PROJECT_ROOT = Path(__file__).resolve().parents[3]
RUN_GATE = PROJECT_ROOT / "Scripts" / "quality" / "run_quality_gate.sh"
RUN_SCRIPT_TESTS = PROJECT_ROOT / "Scripts" / "quality" / "run_script_tests.sh"
PRE_PUSH_HOOK = PROJECT_ROOT / "Scripts" / "quality" / "hooks" / "pre-push"
INSTALLER = PROJECT_ROOT / "Scripts" / "quality" / "install-git-hooks.sh"
LINT = PROJECT_ROOT / "Scripts" / "quality" / "lint_touched.py"
WORKFLOW = PROJECT_ROOT / ".github" / "workflows" / "build-and-test.yml"


def test_shared_runner_delegates_to_lint_touched():
    text = RUN_GATE.read_text(encoding="utf-8")
    assert RUN_GATE.is_file()
    assert "lint_touched.py" in text
    assert "--base" in text
    assert "SKIP_QUALITY_GATE" in text
    assert "python3 -m pip install -r Scripts/quality/requirements.txt" in text


def test_shared_runner_runs_script_tests_after_lint():
    text = RUN_GATE.read_text(encoding="utf-8")
    assert "run_script_tests.sh" in text
    assert "--lint-only" in text
    assert "run_script_tests_once" in text


def test_script_tests_runner_mirrors_ci_pytest_command():
    text = RUN_SCRIPT_TESTS.read_text(encoding="utf-8")
    assert RUN_SCRIPT_TESTS.is_file()
    assert "python3 -m pytest Scripts/release/tests/ Scripts/quality/tests/ -q" in text
    assert "import pytest" in text
    assert "Scripts/release/requirements-test.txt" in text


def test_pre_push_hook_calls_shared_runner():
    text = PRE_PUSH_HOOK.read_text(encoding="utf-8")
    assert PRE_PUSH_HOOK.is_file()
    assert "run_quality_gate.sh" in text
    assert "--pre-push" in text


def test_installer_links_pre_push_hook():
    text = INSTALLER.read_text(encoding="utf-8")
    assert INSTALLER.is_file()
    assert "Scripts/quality/hooks/pre-push" in text
    assert ".git/hooks/pre-push" in text
    assert "ln -sf" in text
    assert "run_script_tests.sh" in text
    assert "Scripts/release/requirements-test.txt" in text


def test_lint_touched_supports_head_ref_for_push_range():
    text = LINT.read_text(encoding="utf-8")
    assert "--head" in text
    assert "base}...{head}" in text or "base}...{head_ref}" in text


def test_lint_touched_includes_skin_designer_in_quality_gate_scope():
    """Matrix-Control Skin Designer must pass the same Clean Code gate as plugin C++."""
    import importlib.util
    import sys

    spec = importlib.util.spec_from_file_location("lint_touched_under_test", LINT)
    assert spec is not None and spec.loader is not None
    mod = importlib.util.module_from_spec(spec)
    # Python 3.14 dataclasses need the module registered before @dataclass runs.
    sys.modules[spec.name] = mod
    try:
        spec.loader.exec_module(mod)
        assert any(p.startswith("Tools/SkinDesigner") for p in mod.SCOPE_PREFIXES)
        assert mod.is_gui_path("Tools/SkinDesigner/SkinDesignerMainComponent.cpp")
        assert mod.is_gate_target("Tools/SkinDesigner/SkinDesignerChrome.h")
    finally:
        sys.modules.pop(spec.name, None)


def test_ci_quality_gate_uses_shared_runner_lint_only():
    workflow_text = WORKFLOW.read_text(encoding="utf-8")
    workflow = yaml.safe_load(workflow_text)
    quality_gate = workflow["jobs"]["quality-gate"]
    runs = [
        step.get("run", "")
        for step in quality_gate["steps"]
        if isinstance(step, dict) and "run" in step
    ]
    assert any("Scripts/quality/run_quality_gate.sh" in run for run in runs)
    assert any("--lint-only" in run for run in runs)
    assert any("origin/${{ github.base_ref }}" in run for run in runs)
    assert any("github.event.before" in run for run in runs)


def test_ci_release_script_tests_uses_shared_script_tests_runner():
    workflow_text = WORKFLOW.read_text(encoding="utf-8")
    workflow = yaml.safe_load(workflow_text)
    job = workflow["jobs"]["release-script-tests"]
    runs = [
        step.get("run", "")
        for step in job["steps"]
        if isinstance(step, dict) and "run" in step
    ]
    joined = "\n".join(runs)
    assert "Scripts/quality/run_script_tests.sh" in joined
    assert "Scripts/release/requirements-test.txt" in joined
    # SSOT: CI must not keep a divergent inlined pytest command beside the shared runner.
    assert "python3 -m pytest Scripts/release/tests/" not in joined
