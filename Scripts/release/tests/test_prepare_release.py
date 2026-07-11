"""Unit tests for Scripts/release/prepare_release.py helpers."""

from __future__ import annotations

import hashlib
import importlib.util
import subprocess
import sys
from pathlib import Path
from unittest.mock import patch

import pytest
import yaml

PROJECT_ROOT = Path(__file__).resolve().parents[3]
PREPARE_RELEASE = PROJECT_ROOT / "Scripts" / "release" / "prepare_release.py"
RELEASE_WORKFLOW = PROJECT_ROOT / ".github" / "workflows" / "release.yml"


def _load_prepare_release():
    spec = importlib.util.spec_from_file_location("prepare_release", PREPARE_RELEASE)
    module = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    sys.modules["prepare_release"] = module
    spec.loader.exec_module(module)
    return module


@pytest.fixture(scope="module")
def prepare_release():
    return _load_prepare_release()


@pytest.mark.parametrize(
    ("tag", "base", "suffix"),
    [
        ("v1.0.0", "1.0.0", ""),
        ("v1.0.0-rc1", "1.0.0", "rc1"),
        ("v0.2.0-alpha", "0.2.0", "alpha"),
        ("v0.0.66-alpha-pre-bmad", "0.0.66", "alpha-pre-bmad"),
    ],
)
def test_parse_version_from_tag(prepare_release, tag, base, suffix):
    parsed = prepare_release.parse_version_from_tag(tag)
    assert parsed.base_version == base
    assert parsed.suffix == suffix
    assert parsed.display_version == (base if not suffix else f"{base}-{suffix}")


@pytest.mark.parametrize(
    ("tag", "expected"),
    [
        ("v1.0.0", False),
        ("v1.0.0-rc1", True),
        ("v0.2.0-alpha", True),
        ("v2.3.4-alpha.1", True),
    ],
)
def test_is_prerelease_tag(prepare_release, tag, expected):
    assert prepare_release.is_prerelease_tag(tag) is expected


def test_validate_tag_rejects_invalid(prepare_release):
    with pytest.raises(ValueError):
        prepare_release.parse_version_from_tag("not-a-tag")


def _write_cmake_lists(root: Path, *, version: str, suffix: str) -> None:
    root.mkdir(parents=True, exist_ok=True)
    (root / "CMakeLists.txt").write_text(
        "\n".join(
            [
                f"project(Matrix-Control VERSION {version})",
                f'set(MATRIX_CONTROL_PRERELEASE_SUFFIX "{suffix}"',
                '    CACHE STRING "Pre-release suffix")',
            ]
        ),
        encoding="utf-8",
    )


def test_validate_tag_against_cmake_success(prepare_release, tmp_path):
    _write_cmake_lists(tmp_path, version="1.0.0", suffix="rc1")
    parsed = prepare_release.validate_tag_against_cmake("v1.0.0-rc1", root=tmp_path)
    assert parsed.display_version == "1.0.0-rc1"


def test_validate_tag_against_cmake_version_mismatch(prepare_release, tmp_path):
    _write_cmake_lists(tmp_path, version="1.0.0", suffix="")
    with pytest.raises(SystemExit, match="does not match"):
        prepare_release.validate_tag_against_cmake("v2.0.0", root=tmp_path)


def test_validate_tag_against_cmake_suffix_mismatch(prepare_release, tmp_path):
    _write_cmake_lists(tmp_path, version="1.0.0", suffix="alpha")
    with pytest.raises(SystemExit, match="suffix"):
        prepare_release.validate_tag_against_cmake("v1.0.0-rc1", root=tmp_path)


def test_verify_release_requires_all_platform_archives(prepare_release, tmp_path, capsys):
    release_dir = tmp_path / "1.0.0"
    release_dir.mkdir()
    paths = prepare_release.ReleasePaths(version="1.0.0", release_dir=release_dir)
    linux_zip = release_dir / "Matrix-Control-v1.0.0-Linux.zip"
    linux_zip.write_bytes(b"zip")
    (release_dir / "RELEASE_NOTES.md").write_text("# Notes", encoding="utf-8")
    digest = hashlib.sha256(linux_zip.read_bytes()).hexdigest()
    (release_dir / "SHA256SUMS.txt").write_text(
        f"{digest}  {linux_zip.name}\n",
        encoding="utf-8",
    )

    with pytest.raises(SystemExit):
        prepare_release.verify_release(paths)

    captured = capsys.readouterr()
    assert "Missing required platform archives" in captured.out


def test_discover_artefact_paths_rejects_empty_bundle(prepare_release, tmp_path):
    root = tmp_path / "artefacts"
    vst3 = root / "VST3" / "Matrix-Control.vst3"
    standalone = root / "Standalone" / "Matrix-Control"
    vst3.mkdir(parents=True)
    standalone.mkdir(parents=True)

    with pytest.raises(SystemExit, match="empty bundle"):
        prepare_release.discover_artefact_paths("linux", artefacts_root=root)


def test_publish_ci_reuploads_when_release_exists(prepare_release, tmp_path):
    release_dir = tmp_path / "1.0.0-rc1"
    release_dir.mkdir()
    paths = prepare_release.ReleasePaths(version="1.0.0-rc1", release_dir=release_dir)

    for suffix in ("macOS-arm64", "Windows", "Linux"):
        (release_dir / f"Matrix-Control-v1.0.0-rc1-{suffix}.zip").write_bytes(b"zip")

    (release_dir / "RELEASE_NOTES.md").write_text("# Notes", encoding="utf-8")
    (release_dir / "SHA256SUMS.txt").write_text("deadbeef  fake\n", encoding="utf-8")

    calls: list[list[str]] = []

    def fake_run(cmd, *, cwd=None, check=True):
        calls.append([str(part) for part in cmd])
        return subprocess.CompletedProcess(cmd, 0)

    with patch.object(prepare_release, "verify_release"), patch.object(
        prepare_release, "_gh_release_exists", return_value=True
    ), patch.object(prepare_release, "_run", side_effect=fake_run):
        prepare_release.publish_ci(paths, "v1.0.0-rc1", yes=True)

    joined = [" ".join(cmd) for cmd in calls]
    assert not any("gh release create" in line for line in joined)
    assert any("gh release upload" in line for line in joined)
    assert any("--notes-file" in line for line in joined)
    assert not any(cmd[0] == "git" for cmd in calls)


def test_publish_ci_invokes_gh_not_git(prepare_release, tmp_path):
    release_dir = tmp_path / "1.0.0-rc1"
    release_dir.mkdir()
    paths = prepare_release.ReleasePaths(version="1.0.0-rc1", release_dir=release_dir)

    for suffix in ("macOS-arm64", "Windows", "Linux"):
        (release_dir / f"Matrix-Control-v1.0.0-rc1-{suffix}.zip").write_bytes(b"zip")

    (release_dir / "RELEASE_NOTES.md").write_text("# Notes", encoding="utf-8")
    (release_dir / "SHA256SUMS.txt").write_text("deadbeef  fake\n", encoding="utf-8")

    calls: list[list[str]] = []

    def fake_run(cmd, *, cwd=None, check=True):
        calls.append([str(part) for part in cmd])
        return subprocess.CompletedProcess(cmd, 0)

    with patch.object(prepare_release, "verify_release"), patch.object(
        prepare_release, "_gh_release_exists", return_value=False
    ), patch.object(prepare_release, "_run", side_effect=fake_run):
        prepare_release.publish_ci(paths, "v1.0.0-rc1", yes=True)

    joined = [" ".join(cmd) for cmd in calls]
    assert any("gh release create" in line for line in joined)
    assert any("--prerelease" in line for line in joined)
    assert not any(cmd[0] == "git" for cmd in calls)


def test_publish_ci_stable_release_omits_prerelease_flag(prepare_release, tmp_path):
    release_dir = tmp_path / "1.0.0"
    release_dir.mkdir()
    paths = prepare_release.ReleasePaths(version="1.0.0", release_dir=release_dir)

    for suffix in ("macOS-arm64", "Windows", "Linux"):
        (release_dir / f"Matrix-Control-v1.0.0-{suffix}.zip").write_bytes(b"zip")
    (release_dir / "RELEASE_NOTES.md").write_text("# Notes", encoding="utf-8")
    (release_dir / "SHA256SUMS.txt").write_text("deadbeef  fake\n", encoding="utf-8")

    calls: list[list[str]] = []

    def fake_run(cmd, *, cwd=None, check=True):
        calls.append([str(part) for part in cmd])
        return subprocess.CompletedProcess(cmd, 0)

    with patch.object(prepare_release, "verify_release"), patch.object(
        prepare_release, "_gh_release_exists", return_value=False
    ), patch.object(prepare_release, "_run", side_effect=fake_run):
        prepare_release.publish_ci(paths, "v1.0.0", yes=True)

    gh_cmd = next(cmd for cmd in calls if cmd[0:3] == ["gh", "release", "create"])
    assert "--prerelease" not in gh_cmd


def test_build_gh_release_create_cmd_no_git(prepare_release, tmp_path):
    release_dir = tmp_path / "1.0.0"
    release_dir.mkdir()
    paths = prepare_release.ReleasePaths(version="1.0.0", release_dir=release_dir)
    (release_dir / "Matrix-Control-v1.0.0-Linux.zip").write_bytes(b"z")
    (release_dir / "RELEASE_NOTES.md").write_text("# Notes", encoding="utf-8")

    cmd = prepare_release.build_gh_release_create_cmd(paths, "v1.0.0", prerelease=False)
    assert cmd[0:3] == ["gh", "release", "create"]
    assert "v1.0.0" in cmd
    assert "git" not in cmd


def test_release_workflow_structure():
    workflow_text = RELEASE_WORKFLOW.read_text(encoding="utf-8")
    workflow = yaml.safe_load(workflow_text)
    triggers = workflow.get("on") or workflow.get(True) or {}

    assert "workflow_dispatch" not in triggers
    assert "pull_request" not in triggers
    assert "push" in triggers
    assert "tags" in triggers["push"]
    assert "v*.*.*" in triggers["push"]["tags"]
    assert "branches" not in triggers["push"]
    assert workflow["env"]["JUCE_VERSION"] == "8.0.12"
    assert workflow["permissions"]["contents"] == "write"
    assert workflow["concurrency"]["cancel-in-progress"] is False

    jobs = workflow["jobs"]
    assert set(jobs) == {"validate-tag", "build", "publish"}
    assert jobs["build"]["needs"] in (["validate-tag"], "validate-tag")
    assert jobs["publish"]["needs"] in (["validate-tag", "build"], ["build", "validate-tag"])
    assert jobs["validate-tag"]["timeout-minutes"] == 10
    assert jobs["build"]["timeout-minutes"] == 90
    assert jobs["publish"]["timeout-minutes"] == 20

    build_yaml = yaml.dump(jobs["build"])
    assert "macos-release-arm64" in build_yaml
    assert "windows-release" in build_yaml
    assert "linux-release" in build_yaml
    assert "prepare_release.py" in build_yaml
    assert "pack" in build_yaml
    assert "macos-latest" in build_yaml
    assert "windows-latest" in build_yaml
    assert "ubuntu-latest" in build_yaml
    assert "MATRIX_BUILD_TESTS=ON" in workflow_text
    assert "USER_COPY_TO_SYSTEM_FOLDERS=OFF" in workflow_text
    assert "USER_COPY_TO_ARTEFACTS_DIR=OFF" in workflow_text
    assert "fail-fast: false" in build_yaml or "fail-fast: false" in build_yaml.replace("'", '"')
    assert "notarytool submit" in workflow_text
    assert "codesign --force" in workflow_text

    publish_yaml = yaml.dump(jobs["publish"])
    assert "prepare_release.py" in publish_yaml
    assert "finalize" in publish_yaml
    assert "publish-ci --yes" in publish_yaml

    validate_yaml = yaml.dump(jobs["validate-tag"])
    assert "validate-tag" in validate_yaml
    assert "prepare_release.py" in validate_yaml
