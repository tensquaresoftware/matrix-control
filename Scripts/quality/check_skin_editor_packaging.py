#!/usr/bin/env python3
"""Prove MATRIX_BUILD_SKIN_EDITOR=OFF omits Skin Editor target and Dev receiver linkage.

Exit 0 on success. Intended for Matrix Test Audit (Release / user packaging row).
"""

from __future__ import annotations

import argparse
import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


def run(cmd: list[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(cmd, cwd=cwd, text=True, capture_output=True, check=False)


def assert_cmake_gates(cmake_lists: Path) -> None:
    text = cmake_lists.read_text(encoding="utf-8")
    if "option(MATRIX_BUILD_SKIN_EDITOR" not in text:
        raise SystemExit("CMakeLists.txt missing MATRIX_BUILD_SKIN_EDITOR option")
    if not re.search(
        r"if\s*\(\s*MATRIX_BUILD_SKIN_EDITOR\s*\).*juce_add_gui_app\s*\(\s*Matrix-Control_SkinEditor",
        text,
        re.S,
    ):
        raise SystemExit("Skin Editor target is not gated behind MATRIX_BUILD_SKIN_EDITOR")
    if "MATRIX_SKIN_EDITOR_RECEIVER" not in text:
        raise SystemExit("Receiver compile define MATRIX_SKIN_EDITOR_RECEIVER missing from CMakeLists")
    if "DevSkinBridgeServer.cpp" not in text or "DevSkinReceiverController.cpp" not in text:
        raise SystemExit("Receiver sources not referenced in CMakeLists.txt")


def configure_off(project_root: Path, binary_dir: Path, juce_dir: str) -> None:
    binary_dir.mkdir(parents=True, exist_ok=True)
    cmd = [
        "cmake",
        "-S",
        str(project_root),
        "-B",
        str(binary_dir),
        "-G",
        "Ninja",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DMATRIX_BUILD_SKIN_EDITOR=OFF",
        "-DMATRIX_BUILD_TESTS=OFF",
        "-DUSER_COPY_TO_SYSTEM_FOLDERS=OFF",
        "-DUSER_COPY_TO_ARTEFACTS_DIR=OFF",
        f"-DJUCE_DIR={juce_dir}",
    ]
    if sys.platform == "darwin":
        cmd.append("-DCMAKE_OSX_ARCHITECTURES=arm64")
    result = run(cmd, project_root)
    if result.returncode != 0:
        sys.stderr.write(result.stdout)
        sys.stderr.write(result.stderr)
        raise SystemExit(f"cmake configure OFF failed ({result.returncode})")


def assert_no_skin_editor_target(binary_dir: Path) -> None:
    help_result = run(["cmake", "--build", str(binary_dir), "--target", "help"], binary_dir)
    combined = help_result.stdout + help_result.stderr
    if "Matrix-Control_SkinEditor" in combined:
        raise SystemExit("Matrix-Control_SkinEditor still listed when MATRIX_BUILD_SKIN_EDITOR=OFF")

    # Ninja build files should not mention the editor target.
    build_ninja = binary_dir / "build.ninja"
    if build_ninja.exists():
        ninja_text = build_ninja.read_text(encoding="utf-8", errors="replace")
        if "Matrix-Control_SkinEditor" in ninja_text:
            raise SystemExit("build.ninja still references Matrix-Control_SkinEditor with option OFF")


def assert_receiver_not_in_plugin_formats(binary_dir: Path) -> None:
    """When OFF, receiver TUs must not appear on AU/VST3/shared plugin compile lines."""
    build_ninja = binary_dir / "build.ninja"
    if not build_ninja.exists():
        # Fallback: scan all generated ninja / make files.
        texts = []
        for path in binary_dir.rglob("build.ninja"):
            texts.append(path.read_text(encoding="utf-8", errors="replace"))
        if not texts:
            raise SystemExit("No build.ninja found to audit receiver linkage")
        ninja_text = "\n".join(texts)
    else:
        ninja_text = build_ninja.read_text(encoding="utf-8", errors="replace")

    receiver_markers = (
        "DevSkinBridgeServer.cpp",
        "DevSkinReceiverController.cpp",
        "SkinEditorReceiverStandalone.cpp",
        "MATRIX_SKIN_EDITOR_RECEIVER",
    )
    for marker in receiver_markers:
        if marker in ninja_text:
            raise SystemExit(
                f"Receiver marker {marker!r} present in build graph with MATRIX_BUILD_SKIN_EDITOR=OFF"
            )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--project-root",
        type=Path,
        default=Path(__file__).resolve().parents[2],
    )
    parser.add_argument(
        "--keep-dir",
        type=Path,
        default=None,
        help="Optional persistent binary dir (default: temp dir removed after check)",
    )
    args = parser.parse_args()
    project_root = args.project_root.resolve()
    cmake_lists = project_root / "CMakeLists.txt"
    assert_cmake_gates(cmake_lists)

    juce_dir = os.environ.get("JUCE_DIR", "").strip()
    if not juce_dir:
        # Match project default macOS path when unset.
        candidate = "/Volumes/Guillaume/Dev/SDKs/JUCE-9"
        if Path(candidate).exists():
            juce_dir = candidate
    if not juce_dir or not Path(juce_dir).exists():
        raise SystemExit("JUCE_DIR is required for packaging configure check")

    temporary = args.keep_dir is None
    builds_root = project_root / "Builds"
    builds_root.mkdir(parents=True, exist_ok=True)
    binary_dir = args.keep_dir or Path(
        tempfile.mkdtemp(prefix="matrix-skin-editor-packaging-", dir=str(builds_root))
    )
    try:
        configure_off(project_root, binary_dir, juce_dir)
        assert_no_skin_editor_target(binary_dir)
        assert_receiver_not_in_plugin_formats(binary_dir)
    finally:
        if temporary and binary_dir.exists():
            shutil.rmtree(binary_dir, ignore_errors=True)

    print("check_skin_editor_packaging: OK (option OFF - no Skin Editor target, no receiver in build graph)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
