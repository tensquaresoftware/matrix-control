"""Lightweight wiring checks for Skin Editor Release packaging gates."""

from __future__ import annotations

import json
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parents[3]
PRESETS = PROJECT_ROOT / "CMakeUserPresets.json"
PACKAGING_SCRIPT = PROJECT_ROOT / "Scripts" / "quality" / "check_skin_editor_packaging.py"
CMAKE_LISTS = PROJECT_ROOT / "CMakeLists.txt"


def test_release_presets_keep_skin_editor_off():
    data = json.loads(PRESETS.read_text(encoding="utf-8"))
    release_presets = [
        p for p in data["configurePresets"]
        if p.get("cacheVariables", {}).get("CMAKE_BUILD_TYPE") == "Release"
    ]
    assert release_presets, "expected at least one Release configure preset"
    for preset in release_presets:
        flag = preset["cacheVariables"].get("MATRIX_BUILD_SKIN_EDITOR")
        assert flag == "OFF", (
            f"{preset['name']} must set MATRIX_BUILD_SKIN_EDITOR=OFF, got {flag!r}"
        )


def test_debug_presets_enable_skin_editor():
    data = json.loads(PRESETS.read_text(encoding="utf-8"))
    debug_presets = [
        p for p in data["configurePresets"]
        if p.get("cacheVariables", {}).get("CMAKE_BUILD_TYPE") == "Debug"
    ]
    assert debug_presets
    for preset in debug_presets:
        flag = preset["cacheVariables"].get("MATRIX_BUILD_SKIN_EDITOR")
        assert flag == "ON", (
            f"{preset['name']} must set MATRIX_BUILD_SKIN_EDITOR=ON, got {flag!r}"
        )


def test_packaging_script_exists_and_gates_cmake():
    assert PACKAGING_SCRIPT.is_file()
    text = PACKAGING_SCRIPT.read_text(encoding="utf-8")
    assert "MATRIX_BUILD_SKIN_EDITOR=OFF" in text
    assert "Matrix-Control_SkinEditor" in text
    cmake = CMAKE_LISTS.read_text(encoding="utf-8")
    assert "option(MATRIX_BUILD_SKIN_EDITOR" in cmake
    assert "MATRIX_SKIN_EDITOR_RECEIVER" in cmake
    # Receiver define must not be the only shared-target flag: DEV_LAYER for apply helpers.
    assert "MATRIX_SKIN_EDITOR_DEV_LAYER" in cmake
