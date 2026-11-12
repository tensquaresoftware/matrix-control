#!/usr/bin/env python3
"""
Gated-path colour scan for Matrix-Control (CAP-6 gate proof).

Fails on residual durable hex literals (6–8 hex digits after 0x) and
`juce::Colours::*` under gated Source/GUI C++.

File set = tracked (`git ls-files`) plus untracked
(`git ls-files --others --exclude-standard`) under Source/GUI/.

Allowlisted SSOT / policy paths (not scanned for violations):
  - Skins/ColourChart.h
  - Skins/SkinColours*.h
  - Helpers/LockDimmingFilm.cpp
  - Skins/Dev/DevSkinBridge*.cpp (IPC magic, not UI colours)
  - Skins/Dev/DevSkinTypes.h (default ARGB sentinel, not UI paint)
  - Skins/Dev/DevSkinMinimalSeed*.cpp (compiled MINIMAL seed tables; hex is the inventory)

Excluded entirely:
  - Source/GUI/Tests/** (legacy path; harness removed)
Usage:
  python3 Scripts/quality/scan_gated_colours.py

Exit codes: 0 = clean, 1 = residual hits, 2 = tooling / git error
(or empty gated file list after filters).
"""

from __future__ import annotations

import re
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]

GATED_PREFIX = "Source/GUI/"
CPP_SUFFIXES = {".h", ".hpp", ".hh", ".cpp", ".cc", ".cxx"}

# Paths relative to Source/GUI/ that are allowlisted (exact file match).
ALLOWLIST_UNDER_GUI = {
    "Skins/ColourChart.h",
    "Helpers/LockDimmingFilm.cpp",
    "Skins/Dev/DevSkinBridgeClient.cpp",
    "Skins/Dev/DevSkinBridgeServer.cpp",
    "Skins/Dev/DevSkinTypes.h",
    "Skins/Dev/DevSkinMinimalSeedBlack.cpp",
    "Skins/Dev/DevSkinMinimalSeedCream.cpp",
}

HEX_PATTERN = re.compile(r"0[xX][0-9A-Fa-f]{6,8}")
COLOURS_PATTERN = re.compile(r"juce::Colours::")


def git_lines(command: list[str]) -> list[str]:
    try:
        out = subprocess.check_output(
            command,
            cwd=REPO_ROOT,
            text=True,
            encoding="utf-8",
            errors="replace",
            stderr=subprocess.PIPE,
        )
    except subprocess.CalledProcessError as exc:
        detail = (exc.stderr or exc.stdout or str(exc)).strip()
        print(f"git failed: {detail}", file=sys.stderr)
        sys.exit(2)
    except FileNotFoundError:
        print("git not found on PATH", file=sys.stderr)
        sys.exit(2)

    return [line for line in out.splitlines() if line]


def is_skin_colours_map(under_gui: str) -> bool:
    name = Path(under_gui).name
    return name.startswith("SkinColours") and name.endswith(".h")


def is_allowlisted(rel_path: str) -> bool:
    if not rel_path.startswith(GATED_PREFIX):
        return False
    under_gui = rel_path[len(GATED_PREFIX) :]
    if under_gui in ALLOWLIST_UNDER_GUI:
        return True
    return is_skin_colours_map(under_gui)


def is_excluded(rel_path: str) -> bool:
    return rel_path.startswith("Source/GUI/Tests/")


def gated_source_files() -> list[Path]:
    rels: set[str] = set()
    for command in (
        ["git", "ls-files", "--", GATED_PREFIX],
        ["git", "ls-files", "--others", "--exclude-standard", "--", GATED_PREFIX],
    ):
        rels.update(git_lines(command))

    files: list[Path] = []
    for rel in sorted(rels):
        if Path(rel).suffix.lower() not in CPP_SUFFIXES:
            continue
        if is_excluded(rel):
            continue
        if is_allowlisted(rel):
            continue
        files.append(REPO_ROOT / rel)
    return files


def scan_file(path: Path) -> list[str]:
    hits: list[str] = []
    rel = path.relative_to(REPO_ROOT).as_posix()
    try:
        text = path.read_text(encoding="utf-8", errors="replace")
    except OSError as exc:
        print(
            f"scan_gated_colours: error: cannot read {rel}: {exc}",
            file=sys.stderr,
        )
        sys.exit(2)

    for line_no, line in enumerate(text.splitlines(), start=1):
        if HEX_PATTERN.search(line):
            hits.append(f"{rel}:{line_no}: hex literal: {line.strip()}")
        if COLOURS_PATTERN.search(line):
            hits.append(f"{rel}:{line_no}: juce::Colours: {line.strip()}")
    return hits


def main() -> int:
    files = gated_source_files()
    if not files:
        print(
            "scan_gated_colours: error: no gated Source/GUI files to scan after filters",
            file=sys.stderr,
        )
        return 2

    all_hits: list[str] = []
    for path in files:
        all_hits.extend(scan_file(path))

    if all_hits:
        print(
            f"scan_gated_colours: {len(all_hits)} residual hit(s) under gated Source/GUI:",
            file=sys.stderr,
        )
        for hit in all_hits:
            print(hit, file=sys.stderr)
        return 1

    print(f"scan_gated_colours: OK ({len(files)} gated files scanned)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
