"""Wiring tests for local quality gate scripts and CI integration."""

from __future__ import annotations

from pathlib import Path

import yaml

PROJECT_ROOT = Path(__file__).resolve().parents[3]
RUN_GATE = PROJECT_ROOT / "Scripts" / "quality" / "run_quality_gate.sh"
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


def test_lint_touched_supports_head_ref_for_push_range():
    text = LINT.read_text(encoding="utf-8")
    assert "--head" in text
    assert "base}...{head}" in text or "base}...{head_ref}" in text


def test_lint_touched_includes_skin_editor_in_quality_gate_scope():
    """Matrix-Control Skin Editor must pass the same Clean Code gate as plugin C++."""
    import importlib.util
    import sys

    spec = importlib.util.spec_from_file_location("lint_touched_under_test", LINT)
    assert spec is not None and spec.loader is not None
    mod = importlib.util.module_from_spec(spec)
    # Python 3.14 dataclasses need the module registered before @dataclass runs.
    sys.modules[spec.name] = mod
    try:
        spec.loader.exec_module(mod)
        assert any(p.startswith("Tools/SkinEditor") for p in mod.SCOPE_PREFIXES)
        assert mod.is_gui_path("Tools/SkinEditor/SkinEditorMainComponent.cpp")
        assert mod.is_gate_target("Tools/SkinEditor/SkinEditorChrome.h")
    finally:
        sys.modules.pop(spec.name, None)


def test_ci_quality_gate_uses_shared_runner():
    workflow_text = WORKFLOW.read_text(encoding="utf-8")
    workflow = yaml.safe_load(workflow_text)
    quality_gate = workflow["jobs"]["quality-gate"]
    runs = [
        step.get("run", "")
        for step in quality_gate["steps"]
        if isinstance(step, dict) and "run" in step
    ]
    assert any("Scripts/quality/run_quality_gate.sh" in run for run in runs)
    assert any("origin/${{ github.base_ref }}" in run for run in runs)
    assert any("github.event.before" in run for run in runs)
