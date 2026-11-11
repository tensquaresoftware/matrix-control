#!/usr/bin/env bash
# Shared runner for release + quality Python tests — same command as CI
# job `release-script-tests` in `.github/workflows/build-and-test.yml`.
#
# Usage:
#   Scripts/quality/run_script_tests.sh
#
# Called by:
#   - Scripts/quality/run_quality_gate.sh (after lint; pre-push / manual)
#   - CI job release-script-tests
#
# Dependencies (once per machine / venv):
#   python3 -m pip install -r Scripts/release/requirements-test.txt
#   python3 -m pip install -r Scripts/quality/requirements.txt

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

cd "$REPO_ROOT"

missing_hint() {
  cat >&2 <<'EOF'
Install test dependencies with:
  python3 -m pip install -r Scripts/release/requirements-test.txt
  python3 -m pip install -r Scripts/quality/requirements.txt
EOF
}

if ! python3 -c "import pytest" 2>/dev/null; then
  echo "Missing dependency: pytest." >&2
  missing_hint
  exit 2
fi

if ! python3 -c "import yaml" 2>/dev/null; then
  echo "Missing dependency: PyYAML (required by Scripts/quality/tests)." >&2
  missing_hint
  exit 2
fi

echo "Running release + quality script tests (CI release-script-tests mirror)…"
python3 -m pytest Scripts/release/tests/ Scripts/quality/tests/ -q
