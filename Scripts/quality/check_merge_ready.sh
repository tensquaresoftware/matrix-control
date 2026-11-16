#!/usr/bin/env bash
# Local merge-ready check vs origin/main — mirrors the gates that matter before a
# PR / merge to main (lint + script tests + Matrix-Control_Tests).
#
# Usage:
#   Scripts/quality/check_merge_ready.sh
#   MATRIX_PRESET=macos-debug-arm64 Scripts/quality/check_merge_ready.sh
#
# Exit 0 only when every step passes.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
cd "$REPO_ROOT"

PRESET="${MATRIX_PRESET:-}"
if [ -z "$PRESET" ]; then
  case "$(uname -s)" in
    Darwin) PRESET="macos-debug-arm64" ;;
    Linux)  PRESET="linux-debug" ;;
    MINGW*|MSYS*|CYGWIN*|Windows_NT) PRESET="windows-debug" ;;
    *)
      echo "Set MATRIX_PRESET (could not infer OS preset)." >&2
      exit 2
      ;;
  esac
fi

echo "== merge-ready: fetch origin/main =="
git fetch origin main --quiet

echo "== merge-ready: quality gate vs origin/main =="
Scripts/quality/run_quality_gate.sh --base origin/main

echo "== merge-ready: build Matrix-Control_Tests ($PRESET) =="
cmake --build --preset "$PRESET" --target Matrix-Control_Tests

TEST_BIN=""
case "$PRESET" in
  macos-debug-arm64)
    TEST_BIN="Builds/macOS/ARM/Debug/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests"
    ;;
  linux-debug)
    TEST_BIN="Builds/Linux/Debug/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests"
    ;;
  windows-debug)
    TEST_BIN="Builds/Windows/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests.exe"
    ;;
  *)
    echo "Unknown preset '$PRESET' — set TEST_BIN path manually." >&2
    exit 2
    ;;
esac

if [ ! -f "$TEST_BIN" ]; then
  echo "Test binary missing: $TEST_BIN" >&2
  exit 1
fi

echo "== merge-ready: run unit tests =="
"$TEST_BIN"

echo
echo "merge-ready OK (local gates vs origin/main)."
echo "Still cross-check GitHub CI on main and the PR before merging."
