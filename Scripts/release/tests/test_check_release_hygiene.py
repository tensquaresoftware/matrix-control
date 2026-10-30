"""Unit tests for Scripts/release/check_release_hygiene.py."""

from __future__ import annotations

import importlib.util
import sys
from pathlib import Path

import pytest
import yaml

PROJECT_ROOT = Path(__file__).resolve().parents[3]
HYGIENE = PROJECT_ROOT / "Scripts" / "release" / "check_release_hygiene.py"
RELEASE_WORKFLOW = PROJECT_ROOT / ".github" / "workflows" / "release.yml"


def _load_hygiene():
    spec = importlib.util.spec_from_file_location("check_release_hygiene", HYGIENE)
    module = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    sys.modules["check_release_hygiene"] = module
    spec.loader.exec_module(module)
    return module


@pytest.fixture(scope="module")
def hygiene():
    return _load_hygiene()


def _make_linux_artefacts(root: Path, *, payload: bytes) -> None:
    vst3 = root / "VST3" / "Matrix-Control.vst3"
    standalone = root / "Standalone" / "Matrix-Control"
    vst3.mkdir(parents=True)
    standalone.parent.mkdir(parents=True)
    (vst3 / "Contents" / "binary").mkdir(parents=True)
    (vst3 / "Contents" / "binary" / "plugin.so").write_bytes(payload)
    standalone.write_bytes(payload)


def test_find_forbidden_detects_test_component(hygiene):
    assert hygiene.find_forbidden_in_bytes(b"prefix TestComponent suffix") == ["TestComponent"]
    assert hygiene.find_forbidden_in_bytes(b"clean binary without markers") == []


def test_scan_file_positive_and_negative(hygiene, tmp_path):
    dirty = tmp_path / "dirty.bin"
    clean = tmp_path / "clean.bin"
    dirty.write_bytes(b"xxxTestComponentyyy")
    clean.write_bytes(b"release-safe payload")

    assert hygiene.scan_file_for_forbidden(dirty) == ["TestComponent"]
    assert hygiene.scan_file_for_forbidden(clean) == []


def test_check_release_artefacts_fails_on_forbidden_string(hygiene, tmp_path):
    root = tmp_path / "artefacts"
    _make_linux_artefacts(root, payload=b"contains TestComponent marker")

    violations = hygiene.check_release_artefacts("linux", artefacts_root=root)
    assert violations
    assert any("TestComponent" in line for line in violations)


def test_check_release_artefacts_passes_when_clean(hygiene, tmp_path):
    root = tmp_path / "artefacts"
    _make_linux_artefacts(root, payload=b"Matrix-Control Release binary")

    violations = hygiene.check_release_artefacts("linux", artefacts_root=root)
    assert violations == []


def test_check_release_artefacts_fails_when_no_files(hygiene, tmp_path):
    """Dir-only / empty-shell trees must fail before or during hygiene."""
    root = tmp_path / "artefacts"
    vst3 = root / "VST3" / "Matrix-Control.vst3"
    standalone = root / "Standalone" / "Matrix-Control"
    (vst3 / "Contents" / "empty").mkdir(parents=True)
    (standalone / "nested").mkdir(parents=True)

    with pytest.raises(SystemExit, match="empty bundle|no artefact files|Missing"):
        hygiene.check_release_artefacts("linux", artefacts_root=root)


def test_cli_exit_codes(hygiene, tmp_path):
    dirty_root = tmp_path / "dirty"
    clean_root = tmp_path / "clean"
    _make_linux_artefacts(dirty_root, payload=b"TestComponent")
    _make_linux_artefacts(clean_root, payload=b"ok")

    assert hygiene.main(["linux", "--artefacts-root", str(dirty_root)]) == 1
    assert hygiene.main(["linux", "--artefacts-root", str(clean_root)]) == 0


def test_release_workflow_invokes_hygiene_before_pack():
    workflow_text = RELEASE_WORKFLOW.read_text(encoding="utf-8")
    workflow = yaml.safe_load(workflow_text)
    build = workflow["jobs"]["build"]
    step_names = [step.get("name", "") for step in build["steps"]]

    assert any("hygiene" in name.lower() for name in step_names), step_names
    assert "check_release_hygiene.py" in workflow_text
    # SharedCode-only Matrix-Control is not enough on Win/Linux — formats must link.
    assert "Matrix-Control_VST3" in workflow_text
    assert "Matrix-Control_Standalone" in workflow_text
    assert "Matrix-Control_AU" in workflow_text
    assert any("ad-hoc codesign" in name.lower() for name in step_names), step_names
    assert "adhoc_sign_macos_bundles.py" in workflow_text
    assert not any("notariz" in name.lower() for name in step_names), step_names

    hygiene_idx = next(
        i for i, name in enumerate(step_names) if "hygiene" in name.lower()
    )
    adhoc_idx = next(
        i for i, name in enumerate(step_names) if "ad-hoc codesign" in name.lower()
    )
    pack_idx = next(i for i, name in enumerate(step_names) if name.startswith("Pack "))

    assert hygiene_idx < adhoc_idx < pack_idx
    # After unit tests / build, before packing.
    run_tests_idx = next(i for i, name in enumerate(step_names) if "unit test" in name.lower())
    assert hygiene_idx > run_tests_idx
