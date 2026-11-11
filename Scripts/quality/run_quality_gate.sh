#!/usr/bin/env bash
# Shared quality gate runner — same lint logic for CI quality-gate, git pre-push,
# and manual use. Local / pre-push also run the CI release-script-tests pytest
# mirror (Scripts/quality/run_script_tests.sh) after lint.
#
# Usage:
#   Scripts/quality/run_quality_gate.sh
#   Scripts/quality/run_quality_gate.sh --base origin/main
#   Scripts/quality/run_quality_gate.sh --base "$REMOTE_SHA" --head "$LOCAL_SHA"
#   Scripts/quality/run_quality_gate.sh --pre-push          # called by git hook (reads stdin)
#   Scripts/quality/run_quality_gate.sh --lint-only         # CI quality-gate job (pytest is a sibling job)
#
# Bypass:
#   SKIP_QUALITY_GATE=1 git push
#   git push --no-verify

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
LINT="$REPO_ROOT/Scripts/quality/lint_touched.py"
SCRIPT_TESTS="$REPO_ROOT/Scripts/quality/run_script_tests.sh"

BASE=""
HEAD_REF=""
PRE_PUSH=false
LINT_ONLY=false

usage() {
  cat <<'EOF'
Usage: Scripts/quality/run_quality_gate.sh [options]

Runs lint_touched.py on the C++ diff (same thresholds as CI quality-gate),
then the CI release-script-tests pytest mirror (~1 s) unless --lint-only.

Options:
  --base REF     Git ref for diff base (CI / explicit manual use)
  --head REF     Git ref for diff tip (default: HEAD; pre-push uses local SHA)
  --pre-push     Pre-push hook mode: read refs from stdin, one lint per push ref
  --lint-only    Skip pytest (CI quality-gate job; pytest runs in release-script-tests)
  -h, --help     Show this help

Without options (manual):
  Uses upstream tracking ref (origin/<branch>) when set, else origin/main.

Environment:
  SKIP_QUALITY_GATE=1   Skip the gate (also: git push --no-verify)
EOF
}

while [ $# -gt 0 ]; do
  case "$1" in
    --base)
      BASE="$2"
      shift 2
      ;;
    --head)
      HEAD_REF="$2"
      shift 2
      ;;
    --pre-push)
      PRE_PUSH=true
      shift
      ;;
    --lint-only)
      LINT_ONLY=true
      shift
      ;;
    -h | --help)
      usage
      exit 0
      ;;
    *)
      echo "Unknown option: $1" >&2
      usage >&2
      exit 2
      ;;
  esac
done

cd "$REPO_ROOT"

if [ "${SKIP_QUALITY_GATE:-}" = "1" ]; then
  echo "Quality gate skipped (SKIP_QUALITY_GATE=1)."
  exit 0
fi

if ! python3 -c "import lizard" 2>/dev/null; then
  echo "Missing dependency: lizard." >&2
  echo "Install with:" >&2
  echo "  python3 -m pip install -r Scripts/quality/requirements.txt" >&2
  exit 2
fi

is_zero_sha() {
  local sha="$1"
  [[ "$sha" =~ ^0+$ ]]
}

ref_exists() {
  git rev-parse --verify "$1" >/dev/null 2>&1
}

resolve_fallback_base() {
  if ref_exists origin/main; then
    echo "origin/main"
  elif ref_exists main; then
    echo "main"
  else
    echo "HEAD~1"
  fi
}

resolve_manual_base() {
  local upstream=""
  upstream="$(git rev-parse --abbrev-ref --symbolic-full-name '@{upstream}' 2>/dev/null || true)"
  if [ -n "$upstream" ] && ref_exists "$upstream"; then
    echo "$upstream"
    return
  fi
  resolve_fallback_base
}

run_lint() {
  local base_ref="$1"
  local head_ref="${2:-HEAD}"
  local -a cmd=(python3 "$LINT" --base "$base_ref")
  if [ "$head_ref" != "HEAD" ]; then
    cmd+=(--head "$head_ref")
  fi
  "${cmd[@]}"
}

run_script_tests_once() {
  if [ "$LINT_ONLY" = true ]; then
    return 0
  fi
  bash "$SCRIPT_TESTS"
}

block_push_message() {
  cat >&2 <<'EOF'

Push bloqué par la porte qualité locale (quality gate).
Corrigez les findings ci-dessus, ou contournez temporairement :
  SKIP_QUALITY_GATE=1 git push
  git push --no-verify

Si pytest / PyYAML manquent :
  python3 -m pip install -r Scripts/quality/requirements.txt
  python3 -m pip install -r Scripts/release/requirements-test.txt

Commande manuelle de secours :
  python3 Scripts/quality/lint_touched.py
  Scripts/quality/run_script_tests.sh
EOF
}

if [ "$PRE_PUSH" = true ]; then
  had_pushable_ref=false
  failed=false

  while read -r local_ref local_sha remote_ref remote_sha; do
    [ -z "$local_ref" ] && continue

    # Delete-only lines (local SHA all zeros) skip lint and do not count as pushable.
    if is_zero_sha "$local_sha"; then
      continue
    fi

    had_pushable_ref=true

    if is_zero_sha "$remote_sha" || ! ref_exists "$remote_sha"; then
      base_ref="$(resolve_fallback_base)"
      echo "Pre-push quality gate: nouvelle branche distante ($remote_ref) — base $base_ref"
    else
      base_ref="$remote_sha"
      echo "Pre-push quality gate: $local_ref — base $base_ref (tip distant actuel)"
    fi

    if ! run_lint "$base_ref" "$local_sha"; then
      failed=true
    fi
  done

  if [ "$had_pushable_ref" = false ]; then
    echo "Pre-push quality gate: aucune ref à pousser — OK."
    exit 0
  fi

  if [ "$failed" = true ]; then
    block_push_message
    exit 1
  fi

  if ! run_script_tests_once; then
    block_push_message
    exit 1
  fi
  exit 0
fi

if [ -z "$BASE" ]; then
  BASE="$(resolve_manual_base)"
  echo "Quality gate base: $BASE"
fi

if ! run_lint "$BASE" "${HEAD_REF:-HEAD}"; then
  exit 1
fi

run_script_tests_once
