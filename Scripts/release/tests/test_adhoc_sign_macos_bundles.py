"""Unit tests for Scripts/release/adhoc_sign_macos_bundles.py."""

from __future__ import annotations

import importlib.util
import sys
from pathlib import Path
from unittest.mock import patch

import pytest

PROJECT_ROOT = Path(__file__).resolve().parents[3]
ADHOC_SIGN = PROJECT_ROOT / "Scripts" / "release" / "adhoc_sign_macos_bundles.py"


def _load_adhoc_sign():
    spec = importlib.util.spec_from_file_location("adhoc_sign_macos_bundles", ADHOC_SIGN)
    module = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    sys.modules["adhoc_sign_macos_bundles"] = module
    spec.loader.exec_module(module)
    return module


@pytest.fixture(scope="module")
def adhoc_sign():
    return _load_adhoc_sign()


def _make_macos_artefacts(root: Path) -> None:
    payloads = {
        "VST3/Matrix-Control.vst3/Contents/MacOS/plugin": b"vst3",
        "AU/Matrix-Control.component/Contents/MacOS/plugin": b"au",
        "Standalone/Matrix-Control.app/Contents/MacOS/Matrix-Control": b"app",
    }
    for rel, data in payloads.items():
        path = root / rel
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(data)


def test_sign_macos_release_artefacts_invokes_codesign_for_each_bundle(
    adhoc_sign, tmp_path
):
    root = tmp_path / "artefacts"
    _make_macos_artefacts(root)
    calls: list[list[str]] = []

    def fake_run(cmd, check=True):
        calls.append([str(part) for part in cmd])
        return None

    with patch.object(adhoc_sign.subprocess, "run", side_effect=fake_run):
        adhoc_sign.sign_macos_release_artefacts(artefacts_root=root)

    assert len(calls) == 6  # sign + verify for each of 3 bundles
    signed = [c for c in calls if c[1:4] == ["--force", "--deep", "--sign"]]
    verified = [c for c in calls if c[1:4] == ["--verify", "--deep", "--strict"]]
    assert len(signed) == 3
    assert len(verified) == 3
    assert all(c[0] == "codesign" and c[4] == "-" for c in signed)
    names = {Path(c[5]).name for c in signed}
    assert names == {
        "Matrix-Control.vst3",
        "Matrix-Control.component",
        "Matrix-Control.app",
    }


def test_adhoc_sign_bundle_missing_path_exits(adhoc_sign, tmp_path):
    missing = tmp_path / "nope.app"
    with pytest.raises(SystemExit, match="Missing bundle"):
        adhoc_sign.adhoc_sign_bundle(missing)
