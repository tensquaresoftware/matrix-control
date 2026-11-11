#!/usr/bin/env bash
# Opt-in installer for Matrix-Control git hooks (idempotent).
#
# After clone:
#   python3 -m pip install -r Scripts/quality/requirements.txt
#   python3 -m pip install -r Scripts/release/requirements-test.txt
#   bash Scripts/quality/install-git-hooks.sh

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
HOOK_SRC="$REPO_ROOT/Scripts/quality/hooks/pre-push"
HOOK_DST="$REPO_ROOT/.git/hooks/pre-push"

if [ ! -d "$REPO_ROOT/.git" ]; then
  echo "Not a git repository: $REPO_ROOT" >&2
  exit 1
fi

chmod +x "$REPO_ROOT/Scripts/quality/run_quality_gate.sh"
chmod +x "$REPO_ROOT/Scripts/quality/run_script_tests.sh"
chmod +x "$HOOK_SRC"
chmod +x "$SCRIPT_DIR/install-git-hooks.sh"

ln -sf "../../Scripts/quality/hooks/pre-push" "$HOOK_DST"
chmod +x "$HOOK_DST"

echo "Installed pre-push hook:"
echo "  $HOOK_DST -> Scripts/quality/hooks/pre-push"
echo
echo "Each push runs: lint_touched (touched C++) + pytest scripts (~1 s CI mirror)."
echo "It does not run the full Matrix-Control_Tests suite."
echo
echo "Dependencies (once per machine / venv):"
echo "  python3 -m pip install -r Scripts/quality/requirements.txt"
echo "  python3 -m pip install -r Scripts/release/requirements-test.txt"
echo
echo "Manual check:"
echo "  Scripts/quality/run_quality_gate.sh"
echo
echo "Bypass when needed:"
echo "  SKIP_QUALITY_GATE=1 git push"
echo "  git push --no-verify"
