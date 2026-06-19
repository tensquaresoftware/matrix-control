#!/usr/bin/env python3
"""Write a filtered compile_commands.json for IDE indexing.

CMake's compile_commands.json includes JuceLibraryCode/BinaryData translation units
that can hang clangd on startup. Invoked automatically from CMakeLists.txt on configure.
"""

from __future__ import annotations

import argparse
import json
import shutil
import sys
from pathlib import Path

EXCLUDED_PATH_FRAGMENTS = (
    "/Builds/",
    "/builds/",
    "JuceLibraryCode/BinaryData",
    "/Applications/JUCE/",
)

BUILD_DIR_CLANGD = """Index:
  Background: Skip

If:
  PathMatch: .*
Diagnostics:
  Suppress: ['*']
"""


def should_include(file_path: str) -> bool:
    return not any(fragment in file_path for fragment in EXCLUDED_PATH_FRAGMENTS)


def filter_compile_commands(source: Path, destination: Path) -> int:
    entries = json.loads(source.read_text(encoding="utf-8"))
    filtered: list[dict] = []
    seen: set[str] = set()

    for entry in entries:
        file_path = entry["file"]
        if not should_include(file_path) or file_path in seen:
            continue
        seen.add(file_path)
        filtered.append(entry)

    if destination.is_symlink():
        destination.unlink()
    destination.write_text(json.dumps(filtered, indent=2), encoding="utf-8")
    return len(filtered)


def write_build_clangd(build_dir: Path) -> None:
    (build_dir / ".clangd").write_text(BUILD_DIR_CLANGD, encoding="utf-8")


def clear_clangd_cache(project_root: Path, build_dir: Path) -> None:
    for cache_dir in (project_root / ".cache" / "clangd", build_dir / ".cache" / "clangd"):
        if cache_dir.is_dir():
            shutil.rmtree(cache_dir, ignore_errors=True)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--project-root", type=Path, required=True)
    parser.add_argument("--build-dir", type=Path, required=True)
    args = parser.parse_args()

    source = args.build_dir / "compile_commands.json"
    destination = args.project_root / "compile_commands.json"

    if not source.is_file():
        print(f"Skipped IDE compile_commands filter (not found): {source}", file=sys.stderr)
        return 0

    count = filter_compile_commands(source, destination)
    write_build_clangd(args.build_dir)
    clear_clangd_cache(args.project_root, args.build_dir)
    print(f"Wrote {count} IDE compile_commands entries to {destination}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
