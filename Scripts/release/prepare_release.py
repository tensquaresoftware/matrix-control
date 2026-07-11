#!/usr/bin/env python3
"""Prepare and publish Matrix-Control GitHub releases (JUCE AU/VST3/Standalone)."""

from __future__ import annotations

import argparse
import hashlib
import re
import shutil
import subprocess
import sys
import tempfile
import zipfile
from dataclasses import dataclass
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parents[2]
SCRIPT_DIR = Path(__file__).resolve().parent
TEMPLATE_DIR = SCRIPT_DIR / "templates"
GITHUB_REPO = "tensquaresoftware/matrix-control"
RELEASES_ROOT = PROJECT_ROOT / "_local" / "releases"
CHECKSUM_FILE = "SHA256SUMS.txt"
NOTES_FILE = "RELEASE_NOTES.md"
PRODUCT_NAME = "Matrix-Control"

TAG_PATTERN = re.compile(
    r"^v(?P<major>\d+)\.(?P<minor>\d+)\.(?P<patch>\d+)(?:-(?P<suffix>.+))?$"
)
CMAKE_VERSION_PATTERN = re.compile(
    r"project\s*\(\s*Matrix-Control\s+VERSION\s+(\d+\.\d+\.\d+)\s*\)",
    re.MULTILINE,
)
CMAKE_SUFFIX_PATTERN = re.compile(
    r'set\s*\(\s*MATRIX_CONTROL_PRERELEASE_SUFFIX\s+"([^"]*)"',
    re.MULTILINE,
)

PLATFORM_ASSETS = ("macos", "windows", "linux")


@dataclass(frozen=True)
class ParsedTag:
    raw: str
    base_version: str
    suffix: str

    @property
    def display_version(self) -> str:
        if self.suffix:
            return f"{self.base_version}-{self.suffix}"
        return self.base_version


@dataclass(frozen=True)
class CMakeVersion:
    version: str
    prerelease_suffix: str


@dataclass(frozen=True)
class ReleasePaths:
    version: str
    release_dir: Path

    def platform_archive(self, platform: str, *, arch: str = "arm64") -> Path:
        if platform == "macos":
            return self.release_dir / f"{PRODUCT_NAME}-v{self.version}-macOS-{arch}.zip"
        if platform == "windows":
            return self.release_dir / f"{PRODUCT_NAME}-v{self.version}-Windows.zip"
        if platform == "linux":
            return self.release_dir / f"{PRODUCT_NAME}-v{self.version}-Linux.zip"
        raise ValueError(f"Unknown platform: {platform!r}")

    @property
    def checksums(self) -> Path:
        return self.release_dir / CHECKSUM_FILE

    @property
    def notes(self) -> Path:
        return self.release_dir / NOTES_FILE

    def distributable_archives(self) -> tuple[Path, ...]:
        return tuple(
            path
            for path in sorted(self.release_dir.glob(f"{PRODUCT_NAME}-v{self.version}-*.zip"))
            if path.is_file()
        )


@dataclass(frozen=True)
class ArtefactPaths:
    vst3: Path
    au: Path | None
    standalone: Path


def parse_version_from_tag(tag: str) -> ParsedTag:
    match = TAG_PATTERN.match(tag)
    if not match:
        raise ValueError(
            f"Unsupported tag: {tag!r} (expected vX.Y.Z or vX.Y.Z-suffix)"
        )
    base = f"{match.group('major')}.{match.group('minor')}.{match.group('patch')}"
    suffix = match.group("suffix") or ""
    return ParsedTag(raw=tag, base_version=base, suffix=suffix)


def is_prerelease_tag(tag: str) -> bool:
    return bool(parse_version_from_tag(tag).suffix)


def read_cmake_version(root: Path = PROJECT_ROOT) -> CMakeVersion:
    text = (root / "CMakeLists.txt").read_text(encoding="utf-8")
    version_match = CMAKE_VERSION_PATTERN.search(text)
    suffix_match = CMAKE_SUFFIX_PATTERN.search(text)
    if not version_match:
        raise SystemExit("Could not read project(Matrix-Control VERSION …) from CMakeLists.txt")
    if suffix_match is None:
        raise SystemExit(
            "Could not read MATRIX_CONTROL_PRERELEASE_SUFFIX from CMakeLists.txt"
        )
    return CMakeVersion(
        version=version_match.group(1),
        prerelease_suffix=suffix_match.group(1),
    )


def validate_tag_against_cmake(tag: str, root: Path = PROJECT_ROOT) -> ParsedTag:
    parsed = parse_version_from_tag(tag)
    cmake = read_cmake_version(root)
    if parsed.base_version != cmake.version:
        raise SystemExit(
            f"Tag base version {parsed.base_version} does not match "
            f"CMakeLists.txt VERSION {cmake.version}"
        )
    if parsed.suffix != cmake.prerelease_suffix:
        raise SystemExit(
            f"Tag suffix {parsed.suffix!r} does not match "
            f"MATRIX_CONTROL_PRERELEASE_SUFFIX {cmake.prerelease_suffix!r}"
        )
    return parsed


def default_artefact_roots(platform: str) -> Path:
    if platform == "macos":
        return PROJECT_ROOT / "Builds/macOS/ARM/Release/Matrix-Control_artefacts/Release"
    if platform == "windows":
        return PROJECT_ROOT / "Builds/Windows/Matrix-Control_artefacts/Release"
    if platform == "linux":
        return (
            PROJECT_ROOT
            / "Builds/Linux/Release/Matrix-Control_artefacts/Release"
        )
    raise ValueError(f"Unknown platform: {platform!r}")


def discover_artefact_paths(
    platform: str,
    *,
    artefacts_root: Path | None = None,
) -> ArtefactPaths:
    root = artefacts_root or default_artefact_roots(platform)
    vst3 = root / "VST3" / f"{PRODUCT_NAME}.vst3"
    standalone = root / "Standalone" / (
        f"{PRODUCT_NAME}.app" if platform == "macos" else f"{PRODUCT_NAME}.exe"
        if platform == "windows"
        else PRODUCT_NAME
    )
    au = root / "AU" / f"{PRODUCT_NAME}.component" if platform == "macos" else None

    missing: list[str] = []
    for label, path in (
        ("VST3", vst3),
        ("Standalone", standalone),
        *((("AU", au),) if au is not None else ()),
    ):
        if not path.exists():
            missing.append(f"{label}: {path}")
        elif path.is_dir() and not any(path.rglob("*")):
            missing.append(f"{label} (empty bundle): {path}")

    if missing:
        raise SystemExit(
            "Missing build artefacts:\n  " + "\n  ".join(missing)
        )

    return ArtefactPaths(vst3=vst3, au=au, standalone=standalone)


def render_template(name: str, version: str) -> str:
    path = TEMPLATE_DIR / name
    if not path.is_file():
        raise SystemExit(f"Missing template: {path}")
    return path.read_text(encoding="utf-8").replace("{{VERSION}}", version)


def ensure_release_dir(paths: ReleasePaths) -> None:
    paths.release_dir.mkdir(parents=True, exist_ok=True)


def _add_path_to_zip(zf: zipfile.ZipFile, source: Path, arc_prefix: Path) -> None:
    if source.is_dir():
        for item in sorted(source.rglob("*")):
            if item.is_dir():
                continue
            arcname = arc_prefix / item.relative_to(source)
            zf.write(item, arcname=str(arcname).replace("\\", "/"))
    else:
        zf.write(source, arcname=str(arc_prefix / source.name).replace("\\", "/"))


def pack_platform(
    paths: ReleasePaths,
    platform: str,
    *,
    artefacts_root: Path | None = None,
    arch: str = "arm64",
    force: bool = False,
) -> Path:
    archive = paths.platform_archive(platform, arch=arch)
    if archive.is_file() and not force:
        raise SystemExit(f"Archive already exists: {archive}\nUse --force to overwrite.")

    artefacts = discover_artefact_paths(platform, artefacts_root=artefacts_root)
    ensure_release_dir(paths)
    if archive.is_file():
        archive.unlink()

    print(f"Packing {platform} -> {archive.name}")
    with zipfile.ZipFile(archive, "w", compression=zipfile.ZIP_DEFLATED) as zf:
        zf.writestr("README.txt", render_template("RELEASE_NOTES.template.md", paths.version))
        _add_path_to_zip(zf, artefacts.vst3, Path(""))
        if artefacts.au is not None:
            _add_path_to_zip(zf, artefacts.au, Path(""))
        _add_path_to_zip(zf, artefacts.standalone, Path(""))

    size_mb = archive.stat().st_size / (1024 * 1024)
    print(f"  Created {archive} ({size_mb:.1f} MiB)")
    return archive


def import_archive(
    paths: ReleasePaths,
    platform: str,
    source: Path,
    *,
    arch: str = "arm64",
    force: bool = False,
) -> None:
    if platform not in PLATFORM_ASSETS:
        raise SystemExit(f"Unknown platform: {platform!r}")

    source = source.resolve()
    if not source.is_file():
        raise SystemExit(f"Source file not found: {source}")

    target = paths.platform_archive(platform, arch=arch)
    if target.is_file() and not force:
        raise SystemExit(f"Archive already exists: {target}\nUse --force to overwrite.")

    ensure_release_dir(paths)
    shutil.copy2(source, target)
    print(f"Imported {source.name} -> {target}")


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as fh:
        for chunk in iter(lambda: fh.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def write_release_notes(paths: ReleasePaths, *, force: bool = False) -> None:
    if paths.notes.is_file() and not force:
        print(f"{NOTES_FILE} already exists (use --force to overwrite)")
        return
    ensure_release_dir(paths)
    paths.notes.write_text(
        render_template("RELEASE_NOTES.template.md", paths.version),
        encoding="utf-8",
    )
    print(f"Created {paths.notes.name}")


def write_checksums(paths: ReleasePaths) -> None:
    archives = paths.distributable_archives()
    if not archives:
        raise SystemExit("Cannot write checksums — no platform archives found")

    lines = [f"{sha256_file(path)}  {path.name}" for path in archives]
    ensure_release_dir(paths)
    paths.checksums.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"Created {paths.checksums.name}")


def verify_release(paths: ReleasePaths) -> None:
    errors: list[str] = []
    archives = paths.distributable_archives()
    if not archives:
        errors.append("No platform archives found")

    required_archives = {
        paths.platform_archive(platform).name for platform in PLATFORM_ASSETS
    }
    present_archives = {path.name for path in archives}
    missing_archives = sorted(required_archives - present_archives)
    if missing_archives:
        errors.append(
            "Missing required platform archives: " + ", ".join(missing_archives)
        )

    for path in archives:
        if path.stat().st_size == 0:
            errors.append(f"Empty archive: {path.name}")

    if not paths.notes.is_file():
        errors.append(f"Missing {NOTES_FILE}")

    if not paths.checksums.is_file():
        errors.append(f"Missing {CHECKSUM_FILE}")
    else:
        listed_names = set()
        for line in paths.checksums.read_text(encoding="utf-8").splitlines():
            if not line.strip():
                continue
            parts = line.split(maxsplit=1)
            if len(parts) != 2:
                errors.append(f"Invalid checksum line: {line!r}")
                continue
            expected, name = parts
            listed_names.add(name)
            file_path = paths.release_dir / name
            if not file_path.is_file():
                errors.append(f"Checksum references missing file: {name}")
                continue
            if sha256_file(file_path) != expected:
                errors.append(f"Checksum mismatch: {name}")

        missing_checksums = sorted(present_archives - listed_names)
        if missing_checksums:
            errors.append(
                "Archives missing from checksums: " + ", ".join(missing_checksums)
            )

    if errors:
        print("Verification FAILED:")
        for err in errors:
            print(f"  * {err}")
        raise SystemExit(1)

    print("Verification OK — all archives present and checksums match.")


def status(paths: ReleasePaths) -> None:
    ensure_release_dir(paths)
    print(f"Version    : {paths.version}")
    print(f"Release dir: {paths.release_dir}")
    print()
    for platform in PLATFORM_ASSETS:
        path = paths.platform_archive(platform)
        if path.is_file():
            size = path.stat().st_size / (1024 * 1024)
            print(f"  [OK] {platform:7} {path.name} ({size:.1f} MiB)")
        else:
            print(f"  [ ] {platform:7} {path.name}")
    for label, path in (("Checksums", paths.checksums), ("Notes", paths.notes)):
        if path.is_file():
            print(f"  [OK] {label:7} {path.name}")
        else:
            print(f"  [ ] {label:7} {path.name}")


def _run(cmd: list[str], *, cwd: Path | None = None) -> None:
    print("==>", " ".join(str(part) for part in cmd))
    subprocess.run(cmd, cwd=cwd or PROJECT_ROOT, check=True)


def _gh_release_exists(tag: str) -> bool:
    result = subprocess.run(
        ["gh", "release", "view", tag, "--repo", GITHUB_REPO],
        cwd=PROJECT_ROOT,
        capture_output=True,
    )
    return result.returncode == 0


def build_gh_release_create_cmd(
    paths: ReleasePaths,
    tag: str,
    *,
    prerelease: bool,
) -> list[str]:
    assets = [*[str(p) for p in paths.distributable_archives()], str(paths.checksums)]
    cmd = [
        "gh",
        "release",
        "create",
        tag,
        "--repo",
        GITHUB_REPO,
        "--title",
        f"{PRODUCT_NAME} {paths.version}",
        "--notes-file",
        str(paths.notes),
        *assets,
    ]
    if prerelease:
        cmd.append("--prerelease")
    return cmd


def gh_release_create(
    paths: ReleasePaths,
    tag: str,
    *,
    prerelease: bool,
) -> None:
    verify_release(paths)
    if not paths.notes.is_file():
        raise SystemExit(f"Missing {NOTES_FILE}. Run: prepare_release.py finalize")

    assets = [*[str(p) for p in paths.distributable_archives()], str(paths.checksums)]

    if _gh_release_exists(tag):
        print(f"Release {tag} already exists — uploading assets")
        _run(
            ["gh", "release", "upload", tag, "--repo", GITHUB_REPO, *assets, "--clobber"],
            cwd=paths.release_dir,
        )
        edit_cmd = [
            "gh",
            "release",
            "edit",
            tag,
            "--repo",
            GITHUB_REPO,
            "--notes-file",
            str(paths.notes),
        ]
        if prerelease:
            edit_cmd.append("--prerelease")
        else:
            edit_cmd.append("--prerelease=false")
        _run(edit_cmd, cwd=paths.release_dir)
    else:
        _run(build_gh_release_create_cmd(paths, tag, prerelease=prerelease), cwd=paths.release_dir)

    print()
    print(f"Published: https://github.com/{GITHUB_REPO}/releases/tag/{tag}")


def publish_ci(
    paths: ReleasePaths,
    tag: str,
    *,
    yes: bool,
    prerelease: bool | None = None,
) -> None:
    resolved_prerelease = (
        prerelease if prerelease is not None else is_prerelease_tag(tag)
    )

    print()
    print(f"Ready to publish {PRODUCT_NAME} {paths.version} (CI — tag must already exist)")
    print(f"  Tag   : {tag}")
    print(f"  Assets: {paths.release_dir}")
    print(f"  Notes : {paths.notes.name}")
    if resolved_prerelease:
        print("  Mode  : pre-release")
    print()

    if not yes:
        answer = input("Continue? [y/N] ").strip().lower()
        if answer not in {"y", "yes"}:
            print("Aborted.")
            return

    gh_release_create(paths, tag, prerelease=resolved_prerelease)


def build_paths(version: str | None, *, tag: str | None = None) -> ReleasePaths:
    if tag:
        parsed = parse_version_from_tag(tag)
        resolved = parsed.display_version
    elif version:
        resolved = version
    else:
        cmake = read_cmake_version()
        suffix = cmake.prerelease_suffix
        resolved = cmake.version if not suffix else f"{cmake.version}-{suffix}"

    return ReleasePaths(version=resolved, release_dir=RELEASES_ROOT / resolved)


def main() -> int:
    parser = argparse.ArgumentParser(
        description=(
            "Prepare Matrix-Control release assets under _local/releases/<version>/ "
            "(semver tags use v prefix on git, stripped for display version)."
        ),
    )
    parser.add_argument(
        "--version",
        help="Release display version X.Y.Z or X.Y.Z-suffix (default: CMakeLists.txt)",
    )
    parser.add_argument(
        "--tag",
        help="Git tag including v prefix (e.g. v1.0.0-rc1) — overrides --version",
    )
    parser.add_argument(
        "--force",
        action="store_true",
        help="Overwrite existing archives or RELEASE_NOTES.md",
    )

    sub = parser.add_subparsers(dest="command", required=True)
    sub.add_parser("status", help="Show which release assets exist")

    val = sub.add_parser("validate-tag", help="Validate git tag against CMakeLists.txt")
    val.add_argument("tag", help="Git tag (e.g. v1.0.0-rc1)")

    paths_cmd = sub.add_parser(
        "print-paths",
        help="Print expected Release artefact paths for a platform",
    )
    paths_cmd.add_argument("platform", choices=PLATFORM_ASSETS)
    paths_cmd.add_argument(
        "--artefacts-root",
        type=Path,
        help="Override Matrix-Control_artefacts/Release root",
    )

    pack = sub.add_parser("pack", help="Pack Release build outputs into a platform zip")
    pack.add_argument("platform", choices=PLATFORM_ASSETS)
    pack.add_argument(
        "--artefacts-root",
        type=Path,
        help="Override Matrix-Control_artefacts/Release root",
    )
    pack.add_argument(
        "--arch",
        default="arm64",
        help="macOS architecture label for zip filename (default: arm64)",
    )

    imp = sub.add_parser(
        "import",
        help="Copy an external platform archive into the release folder",
    )
    imp.add_argument("platform", choices=PLATFORM_ASSETS)
    imp.add_argument("source", type=Path)
    imp.add_argument("--arch", default="arm64")

    sub.add_parser(
        "finalize",
        help="Create RELEASE_NOTES.md and SHA256SUMS.txt",
    )
    sub.add_parser("verify", help="Verify archives and checksums")

    pub_ci = sub.add_parser(
        "publish-ci",
        help="Create GitHub release via gh (CI: tag already exists on remote)",
    )
    pub_ci.add_argument("-y", "--yes", action="store_true", help="Skip confirmation prompt")
    pub_ci.add_argument("--prerelease", action="store_true", help="Force pre-release on GitHub")
    pub_ci.add_argument(
        "--stable",
        action="store_true",
        help="Force stable release (override auto prerelease detection)",
    )

    args = parser.parse_args()
    paths = build_paths(args.version, tag=args.tag)

    if args.command == "validate-tag":
        parsed = validate_tag_against_cmake(args.tag)
        print(f"Tag OK: {args.tag} -> {parsed.display_version}")
    elif args.command == "status":
        status(paths)
    elif args.command == "print-paths":
        artefacts = discover_artefact_paths(
            args.platform,
            artefacts_root=args.artefacts_root,
        )
        print(artefacts.vst3)
        if artefacts.au is not None:
            print(artefacts.au)
        print(artefacts.standalone)
    elif args.command == "pack":
        pack_platform(
            paths,
            args.platform,
            artefacts_root=args.artefacts_root,
            arch=args.arch,
            force=args.force,
        )
    elif args.command == "import":
        import_archive(
            paths,
            args.platform,
            args.source,
            arch=args.arch,
            force=args.force,
        )
    elif args.command == "finalize":
        write_release_notes(paths, force=args.force)
        write_checksums(paths)
        print()
        status(paths)
    elif args.command == "verify":
        verify_release(paths)
    elif args.command == "publish-ci":
        if args.prerelease and args.stable:
            parser.error("Use only one of --prerelease or --stable")
        tag = args.tag or f"v{paths.version}"
        prerelease_override = None
        if args.prerelease:
            prerelease_override = True
        elif args.stable:
            prerelease_override = False
        publish_ci(paths, tag, yes=args.yes, prerelease=prerelease_override)
    else:
        parser.error(f"Unknown command: {args.command}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
