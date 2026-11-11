"""Tests for Scripts/quality/scan_gated_colours.py (CAP-6 gate proof)."""

from __future__ import annotations

import importlib.util
import subprocess
import sys
import uuid
from pathlib import Path

import pytest

PROJECT_ROOT = Path(__file__).resolve().parents[3]
SCAN_SCRIPT = PROJECT_ROOT / "Scripts" / "quality" / "scan_gated_colours.py"
PROBE_DIR = PROJECT_ROOT / "Source" / "GUI" / "Helpers"


def _load_scan_module():
    spec = importlib.util.spec_from_file_location("scan_gated_colours", SCAN_SCRIPT)
    assert spec is not None and spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


@pytest.fixture(scope="module")
def scan():
    return _load_scan_module()


def _unique_probe_path() -> Path:
    """Unique gated untracked .cpp under Helpers (avoid parallel / leftover races)."""
    name = f"_scan_gated_colours_probe_{uuid.uuid4().hex}_do_not_commit.cpp"
    return PROBE_DIR / name


def _run_scan() -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(SCAN_SCRIPT)],
        cwd=PROJECT_ROOT,
        capture_output=True,
        text=True,
        check=False,
    )


def test_scan_script_exits_zero_on_gated_tree():
    result = _run_scan()
    assert result.returncode == 0, result.stderr
    assert "OK" in result.stdout


def test_tests_tree_is_excluded(scan):
    assert scan.is_excluded("Source/GUI/Tests/TestColourPickerButton.cpp")
    assert not scan.is_excluded("Source/GUI/Helpers/LockDimmingFilm.cpp")


def test_lock_film_and_palette_are_allowlisted(scan):
    assert scan.is_allowlisted("Source/GUI/Helpers/LockDimmingFilm.cpp")
    assert scan.is_allowlisted("Source/GUI/Skins/ColourChart.h")
    assert scan.is_allowlisted("Source/GUI/Skins/SkinColoursPanels.h")
    assert scan.is_allowlisted("Source/GUI/Skins/Dev/DevSkinBridgeClient.cpp")
    assert scan.is_allowlisted("Source/GUI/Skins/Dev/DevSkinTypes.h")
    assert not scan.is_allowlisted("Source/GUI/Widgets/Led.cpp")


def test_footer_error_has_no_alpha_as_darken():
    text = (PROJECT_ROOT / "Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp").read_text(
        encoding="utf-8"
    )
    assert ".withAlpha(0.8f)" not in text
    assert "SkinColourState::kError" in text


def test_modulation_bus_drag_uses_skin_not_colour_chart_alpha():
    text = (PROJECT_ROOT / "Source/GUI/Widgets/ModulationBusCell.cpp").read_text(encoding="utf-8")
    assert "ColourChart::" not in text
    assert ".withAlpha(0.75f)" not in text
    assert "getColour(" in text


def test_popup_disabled_uses_look_disabled_fields():
    renderer = (PROJECT_ROOT / "Source/GUI/Widgets/PopupMenuRenderer.cpp").read_text(encoding="utf-8")
    header = (PROJECT_ROOT / "Source/GUI/Widgets/HeaderLogoPopupMenu.cpp").read_text(encoding="utf-8")
    assert "textDisabled" in renderer
    assert ".withAlpha(0.5f)" not in renderer
    assert "textDisabled" in header
    assert ".withAlpha(0.5f)" not in header


def test_alert_button_look_has_no_colour_chart_dig():
    builders = (PROJECT_ROOT / "Source/GUI/Looks/LookBuilders.cpp").read_text(encoding="utf-8")
    multi = (PROJECT_ROOT / "Source/GUI/Looks/LookBuildersMultiSet.cpp").read_text(encoding="utf-8")
    assert "ColourChart.h" not in builders
    assert "ColourChart.h" not in multi
    assert "buttonAlertLookFromSkin" in multi
    # Alert fill/text own their Button pastilles (ownership pass); no FooterMessage borrow.
    assert "SkinObjectType::kButton" in multi
    assert "kFooterMessage" not in multi


def test_scan_exits_one_on_untracked_hex_hit():
    """Untracked gated .cpp with a hex literal must fail the scan (exit 1)."""
    probe = _unique_probe_path()
    try:
        probe.write_text(
            "// temporary probe for scan_gated_colours failure path\n"
            "static constexpr auto kProbe = 0xFF112233;\n",
            encoding="utf-8",
        )
        result = _run_scan()
        assert result.returncode == 1, (
            f"expected exit 1, got {result.returncode}\n"
            f"stdout={result.stdout}\nstderr={result.stderr}"
        )
        assert "hex literal" in result.stderr
        assert probe.name in result.stderr
    finally:
        if probe.exists():
            probe.unlink()


def test_scan_exits_one_on_untracked_juce_colours_hit():
    """Untracked gated .cpp with juce::Colours:: must fail the scan (exit 1)."""
    probe = _unique_probe_path()
    try:
        probe.write_text(
            "// temporary probe for scan_gated_colours Colours failure path\n"
            "static auto kProbe = juce::Colours::black;\n",
            encoding="utf-8",
        )
        result = _run_scan()
        assert result.returncode == 1, (
            f"expected exit 1, got {result.returncode}\n"
            f"stdout={result.stdout}\nstderr={result.stderr}"
        )
        assert "juce::Colours" in result.stderr
        assert probe.name in result.stderr
    finally:
        if probe.exists():
            probe.unlink()


def test_empty_gated_file_list_exits_two(scan, monkeypatch):
    monkeypatch.setattr(scan, "gated_source_files", lambda: [])
    assert scan.main() == 2
