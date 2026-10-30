#!/usr/bin/env python3
"""Ad-hoc codesign macOS Release bundles before packing.

Linker-only signatures leave AU/VST3/Standalone bundles with unsealed Resources.
macOS then reports the app as \"damaged\". Signing with identity ``-`` (ad-hoc)
reseals each bundle without an Apple Developer certificate or notarization.
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

from prepare_release import (  # noqa: E402
    default_artefact_roots,
    discover_artefact_paths,
)


def _run_codesign(args: list[str]) -> None:
    subprocess.run(["codesign", *args], check=True)


def adhoc_sign_bundle(path: Path) -> None:
    """Force ad-hoc sign a .app / .vst3 / .component, then strict-verify."""
    if not path.exists():
        raise SystemExit(f"Missing bundle: {path}")
    print(f"Ad-hoc codesign: {path.name}")
    _run_codesign(["--force", "--deep", "--sign", "-", str(path)])
    _run_codesign(["--verify", "--deep", "--strict", str(path)])
    print(f"  verified: {path.name}")


def sign_macos_release_artefacts(*, artefacts_root: Path | None = None) -> None:
    artefacts = discover_artefact_paths(
        "macos",
        artefacts_root=artefacts_root or default_artefact_roots("macos"),
    )
    bundles = [artefacts.vst3, artefacts.standalone]
    if artefacts.au is not None:
        bundles.append(artefacts.au)
    for bundle in bundles:
        adhoc_sign_bundle(bundle)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Ad-hoc codesign macOS Release AU/VST3/Standalone bundles"
    )
    parser.add_argument(
        "--artefacts-root",
        type=Path,
        default=None,
        help="Override Matrix-Control_artefacts/Release root",
    )
    args = parser.parse_args(argv)
    sign_macos_release_artefacts(artefacts_root=args.artefacts_root)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
