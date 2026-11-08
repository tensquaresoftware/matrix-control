#!/usr/bin/env python3
"""
Quality gate for Matrix-Control — anti-drift on the ticket diff.

Usage:
  python3 Scripts/quality/lint_touched.py
  python3 Scripts/quality/lint_touched.py --base origin/main
  Scripts/quality/run_quality_gate.sh          # manual / pre-push / CI wrapper
  python3 Scripts/quality/lint_touched.py --all
  python3 Scripts/quality/lint_touched.py --all --report-worst 20

Without --all:
  - Only Source/, Tests/, and Tools/SkinEditor/ C++ files in the git worktree /
    commits since --base
  - Function findings apply only to functions that intersect **changed lines**
    (Boy Scout limited: one-line fix in a large historical file does not force
    a whole-file cleanup)
  - File-size findings apply only if the file is new, or useful-line count grew
    while still over the threshold

Matrix-Control Skin Editor (`Tools/SkinEditor/`) is in the same quality gate as
plugin GUI code (GUI nloc/ccn thresholds).

With --all: full diagnostic of the tree (debt cleanup backlog).

Thresholds: CONVENTIONS.md §3
  - Function lines: 40 (Core) / 50 (GUI) — lizard nloc
  - Parameters: 4
  - Cyclomatic complexity: 10 (GUI up to 12)
  - Nesting depth: 4
  - Useful .cpp lines: ~400
"""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path

try:
    import lizard
except ImportError:
    print(
        "Missing dependency: lizard. Install with:\n"
        "  python3 -m pip install -r Scripts/quality/requirements.txt",
        file=sys.stderr,
    )
    sys.exit(2)

REPO_ROOT = Path(__file__).resolve().parents[2]

MAX_NLOC_CORE = 40
MAX_NLOC_GUI = 50
MAX_PARAMS = 4
MAX_CCN_CORE = 10
MAX_CCN_GUI = 12
MAX_NESTING = 4
MAX_CPP_USEFUL_LINES = 400

CPP_SUFFIXES = {".h", ".hpp", ".hh", ".cpp", ".cc", ".cxx"}
SCOPE_PREFIXES = ("Source/", "Tests/", "Tools/SkinEditor/")
SKIP_PATH_PARTS = ("JuceLibraryCode", "juce_binarydata")
# JUCE virtual overrides keep framework arity — do not rewrite signatures (CONVENTIONS.md §3).
JUCE_OVERRIDE_PARAM_ALLOWLIST = frozenset(
    {
        "paintListBoxItem",
        "drawScrollbar",
        "drawComboBox",
        "drawButtonBackground",
        "drawTextEditorOutline",
        "fillTextEditorBackground",
        "getLabelFont",
        "getTextButtonFont",
        "getComboBoxFont",
        "getPopupMenuFont",
    }
)


@dataclass(frozen=True)
class Finding:
    path: str
    kind: str
    detail: str
    severity: str
    score: float


def git_lines(command: list[str]) -> list[str]:
    try:
        out = subprocess.check_output(
            command,
            cwd=REPO_ROOT,
            text=True,
            stderr=subprocess.DEVNULL,
        ).strip()
    except subprocess.CalledProcessError:
        return []
    return [line for line in out.splitlines() if line]


def is_gate_target(rel: str) -> bool:
    if not any(rel.startswith(p) for p in SCOPE_PREFIXES):
        return False
    path = Path(rel)
    if path.suffix.lower() not in CPP_SUFFIXES:
        return False
    if any(skip in path.parts for skip in SKIP_PATH_PARTS):
        return False
    return (REPO_ROOT / rel).is_file()


def collect_changed_files(base: str, head: str = "HEAD") -> list[str]:
    sets: list[list[str]] = []
    if head == "HEAD":
        sets.extend(
            [
                git_lines(["git", "diff", "--name-only", "--diff-filter=ACMR", "HEAD"]),
                git_lines(
                    ["git", "diff", "--name-only", "--diff-filter=ACMR", "--cached"]
                ),
                git_lines(["git", "ls-files", "--others", "--exclude-standard"]),
            ]
        )
    if git_lines(["git", "rev-parse", "--verify", base]) and git_lines(
        ["git", "rev-parse", "--verify", head]
    ):
        sets.append(
            git_lines(
                [
                    "git",
                    "diff",
                    "--name-only",
                    "--diff-filter=ACMR",
                    f"{base}...{head}",
                ]
            )
        )
    return sorted({f for batch in sets for f in batch if is_gate_target(f)})


def collect_all_files() -> list[str]:
    tracked = git_lines(["git", "ls-files", "Source", "Tests", "Tools/SkinEditor"])
    return sorted(f for f in tracked if is_gate_target(f))


def is_gui_path(rel: str) -> bool:
    norm = rel.replace("\\", "/")
    return (
        "/GUI/" in norm
        or norm.startswith("Source/GUI/")
        or norm.startswith("Tools/SkinEditor/")
    )


def useful_line_count(text: str) -> int:
    count = 0
    for raw in text.splitlines():
        line = raw.strip()
        if not line or line.startswith("//"):
            continue
        count += 1
    return count


def max_nesting_in_range(lines: list[str], start: int, end: int) -> int:
    depth = 0
    max_depth = 0
    for idx in range(start - 1, min(end, len(lines))):
        line = lines[idx]
        if "//" in line:
            line = line.split("//", 1)[0]
        for ch in line:
            if ch == "{":
                depth += 1
                max_depth = max(max_depth, depth)
            elif ch == "}":
                depth = max(0, depth - 1)
    return max(0, max_depth - 1)


def parse_diff_line_ranges(base: str, head: str = "HEAD") -> dict[str, set[int]]:
    """Map path -> set of new-file line numbers touched (unified diff -U0)."""
    ranges: dict[str, set[int]] = defaultdict(set)
    hunk_re = re.compile(r"^@@ -\d+(?:,\d+)? \+(\d+)(?:,(\d+))? @@")

    def ingest(diff_text: str) -> None:
        current: str | None = None
        for line in diff_text.splitlines():
            if line.startswith("+++ b/"):
                current = line[6:]
                continue
            if line.startswith("+++ /dev/null"):
                current = None
                continue
            match = hunk_re.match(line)
            if not match or current is None:
                continue
            start = int(match.group(1))
            count = int(match.group(2) or "1")
            if count == 0:
                continue
            for n in range(start, start + count):
                ranges[current].add(n)

    blobs: list[str] = []
    if head == "HEAD":
        for cmd in (
            ["git", "diff", "-U0", "HEAD"],
            ["git", "diff", "-U0", "--cached"],
        ):
            try:
                blobs.append(
                    subprocess.check_output(
                        cmd, cwd=REPO_ROOT, text=True, stderr=subprocess.DEVNULL
                    )
                )
            except subprocess.CalledProcessError:
                pass
    if git_lines(["git", "rev-parse", "--verify", base]) and git_lines(
        ["git", "rev-parse", "--verify", head]
    ):
        try:
            blobs.append(
                subprocess.check_output(
                    ["git", "diff", "-U0", f"{base}...{head}"],
                    cwd=REPO_ROOT,
                    text=True,
                    stderr=subprocess.DEVNULL,
                )
            )
        except subprocess.CalledProcessError:
            pass

    for blob in blobs:
        ingest(blob)

    if head == "HEAD":
        for rel in git_lines(["git", "ls-files", "--others", "--exclude-standard"]):
            if not is_gate_target(rel):
                continue
            text = (REPO_ROOT / rel).read_text(encoding="utf-8", errors="replace")
            ranges[rel] = set(range(1, len(text.splitlines()) + 1))

    return ranges


def function_intersects(start: int, end: int, changed: set[int] | None) -> bool:
    if changed is None:
        return True  # --all mode
    if not changed:
        return False
    return any(line in changed for line in range(start, end + 1))


def severity_for_nloc(nloc: int, limit: int) -> str:
    if nloc >= limit * 2:
        return "critical"
    if nloc >= int(limit * 1.4):
        return "serious"
    return "light"


def severity_for_ccn(ccn: int, limit: int) -> str:
    if ccn >= limit + 8:
        return "critical"
    if ccn >= limit + 3:
        return "serious"
    return "light"


def severity_for_file(useful: int) -> str:
    if useful >= 800:
        return "critical"
    if useful >= 550:
        return "serious"
    return "light"


def base_file_useful_lines(rel: str, base: str) -> int | None:
    """Useful-line count of file at merge-base; None if file did not exist."""
    if not git_lines(["git", "rev-parse", "--verify", base]):
        return None
    try:
        blob = subprocess.check_output(
            ["git", "show", f"{base}:{rel}"],
            cwd=REPO_ROOT,
            text=True,
            stderr=subprocess.DEVNULL,
        )
    except subprocess.CalledProcessError:
        return None
    return useful_line_count(blob)


def analyse_file(
    rel: str,
    *,
    changed_lines: set[int] | None,
    base: str | None,
    full_tree: bool,
) -> list[Finding]:
    path = REPO_ROOT / rel
    text = path.read_text(encoding="utf-8", errors="replace")
    lines = text.splitlines()
    findings: list[Finding] = []
    gui = is_gui_path(rel)
    max_nloc = MAX_NLOC_GUI if gui else MAX_NLOC_CORE
    max_ccn = MAX_CCN_GUI if gui else MAX_CCN_CORE

    if path.suffix.lower() == ".cpp":
        useful = useful_line_count(text)
        if useful > MAX_CPP_USEFUL_LINES:
            report_file = full_tree
            if not full_tree and base is not None:
                before = base_file_useful_lines(rel, base)
                if before is None:
                    report_file = True  # new file over limit
                elif useful > before:
                    report_file = True  # grew while over threshold
            if report_file:
                sev = severity_for_file(useful)
                findings.append(
                    Finding(
                        path=rel,
                        kind="file-size",
                        detail=f"useful lines {useful} > {MAX_CPP_USEFUL_LINES}",
                        severity=sev,
                        score=float(useful),
                    )
                )

    analysis = lizard.analyze_file(str(path))
    for fn in analysis.function_list:
        start = int(fn.start_line)
        end = int(fn.end_line)
        if not function_intersects(start, end, changed_lines):
            continue

        name = fn.name
        nloc = int(fn.nloc)
        ccn = int(fn.cyclomatic_complexity)
        params = int(fn.parameter_count)
        nesting = max_nesting_in_range(lines, start, end)

        if nloc > max_nloc:
            findings.append(
                Finding(
                    path=rel,
                    kind="function-length",
                    detail=f"{name} nloc={nloc} > {max_nloc} (L{start}-{end})",
                    severity=severity_for_nloc(nloc, max_nloc),
                    score=float(nloc),
                )
            )
        simple_name = name.split("::")[-1]
        if params > MAX_PARAMS and simple_name not in JUCE_OVERRIDE_PARAM_ALLOWLIST:
            findings.append(
                Finding(
                    path=rel,
                    kind="parameters",
                    detail=f"{name} params={params} > {MAX_PARAMS} (L{start})",
                    severity="serious" if params >= 7 else "light",
                    score=float(params),
                )
            )
        if ccn > max_ccn:
            findings.append(
                Finding(
                    path=rel,
                    kind="complexity",
                    detail=f"{name} ccn={ccn} > {max_ccn} (L{start})",
                    severity=severity_for_ccn(ccn, max_ccn),
                    score=float(ccn),
                )
            )
        if nesting > MAX_NESTING:
            findings.append(
                Finding(
                    path=rel,
                    kind="nesting",
                    detail=f"{name} nesting≈{nesting} > {MAX_NESTING} (L{start})",
                    severity="serious" if nesting >= 7 else "light",
                    score=float(nesting),
                )
            )

    return findings


def maybe_run_clang_tidy(files: list[str]) -> int:
    compile_db = REPO_ROOT / "compile_commands.json"
    if not compile_db.is_file():
        return 0

    tidy = None
    for candidate in (
        "clang-tidy",
        "/opt/homebrew/opt/llvm/bin/clang-tidy",
        "/usr/local/opt/llvm/bin/clang-tidy",
    ):
        try:
            subprocess.check_output(
                [candidate, "-version"],
                stderr=subprocess.DEVNULL,
                text=True,
            )
            tidy = candidate
            break
        except (OSError, subprocess.CalledProcessError):
            continue
    if tidy is None:
        return 0

    cpp_files = [f for f in files if f.endswith((".cpp", ".cc", ".cxx"))]
    if not cpp_files:
        return 0

    print(f"Optional clang-tidy ({tidy}) on {len(cpp_files)} translation unit(s)…")
    result = subprocess.run(
        [
            tidy,
            "-p",
            str(REPO_ROOT),
            f"-header-filter={re.escape(str(REPO_ROOT / 'Source'))}/.*",
            *cpp_files,
        ],
        cwd=REPO_ROOT,
        check=False,
    )
    return result.returncode


def print_report(findings: list[Finding], *, report_worst: int) -> None:
    if not findings:
        print("Quality gate OK — no threshold violations.")
        return

    by_sev: dict[str, list[Finding]] = defaultdict(list)
    for f in findings:
        by_sev[f.severity].append(f)

    for sev in ("critical", "serious", "light"):
        items = by_sev.get(sev, [])
        if not items:
            continue
        print(f"\n=== {sev.upper()} ({len(items)}) ===")
        for item in sorted(items, key=lambda x: (-x.score, x.path, x.detail)):
            print(f"  [{item.kind}] {item.path}: {item.detail}")

    if report_worst > 0:
        file_scores: dict[str, float] = defaultdict(float)
        file_hits: dict[str, int] = defaultdict(int)
        for f in findings:
            file_hits[f.path] += 1
            weight = {"critical": 1000, "serious": 100, "light": 10}[f.severity]
            file_scores[f.path] = max(file_scores[f.path], weight + f.score)
        ranked = sorted(file_scores.items(), key=lambda kv: -kv[1])[:report_worst]
        print(f"\n=== WORST FILES (top {len(ranked)}) ===")
        for path, score in ranked:
            print(f"  {path}  (score≈{score:.0f}, findings={file_hits[path]})")


def main() -> int:
    parser = argparse.ArgumentParser(description="Matrix-Control C++ quality gate")
    parser.add_argument("--base", default="origin/main")
    parser.add_argument(
        "--head",
        default="HEAD",
        help="Diff tip ref (default HEAD; pre-push uses the local SHA being pushed)",
    )
    parser.add_argument(
        "--all",
        action="store_true",
        help="Full-tree diagnostic (historical debt)",
    )
    parser.add_argument(
        "--report-worst",
        type=int,
        default=0,
        metavar="N",
        help="Also print top-N worst files",
    )
    parser.add_argument(
        "--clang-tidy",
        action="store_true",
        help="Also run clang-tidy when available",
    )
    args = parser.parse_args()

    full_tree = args.all
    if not full_tree and not git_lines(["git", "rev-parse", "--verify", args.base]):
        print(
            f"Quality gate FAILED — git base ref not found: {args.base}",
            file=sys.stderr,
        )
        return 1
    if not full_tree and not git_lines(["git", "rev-parse", "--verify", args.head]):
        print(
            f"Quality gate FAILED — git head ref not found: {args.head}",
            file=sys.stderr,
        )
        return 1

    files = (
        collect_all_files()
        if full_tree
        else collect_changed_files(args.base, args.head)
    )

    if not files:
        print(
            "No C++ files under Source/ or Tests/ to analyse."
            if full_tree
            else "No touched C++ files under Source/ or Tests/ — quality gate OK."
        )
        return 0

    mode = "full tree" if full_tree else "touched files (hunk-aware)"
    print(f"Quality gate — {mode} ({len(files)} file(s))")
    if not full_tree:
        for f in files:
            print(f"  - {f}")

    line_map = None if full_tree else parse_diff_line_ranges(args.base, args.head)

    findings: list[Finding] = []
    for rel in files:
        changed = None if full_tree else line_map.get(rel, set())
        findings.extend(
            analyse_file(
                rel,
                changed_lines=changed,
                base=None if full_tree else args.base,
                full_tree=full_tree,
            )
        )

    worst = args.report_worst if args.report_worst > 0 else (20 if full_tree else 0)
    print_report(findings, report_worst=worst)

    tidy_rc = maybe_run_clang_tidy(files) if args.clang_tidy else 0

    if findings:
        if full_tree:
            # Diagnostic mode: report debt but do not fail the process
            print(
                f"\nDiagnostic complete — {len(findings)} finding(s) in the tree "
                "(exit 0). Touched-file mode still fails CI on new drift.",
            )
            return 0
        print(
            f"\nQuality gate FAILED — {len(findings)} finding(s). "
            "Fix signals on changed functions; do not redesign unrelated code.",
            file=sys.stderr,
        )
        return 1
    if tidy_rc != 0:
        print("clang-tidy reported issues.", file=sys.stderr)
        return tidy_rc
    return 0


if __name__ == "__main__":
    sys.exit(main())
