#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
bin="${1:-${root}/build/devclean}"

[[ -x "${bin}" ]]
"${bin}" --help >/dev/null
"${bin}" --version >/dev/null
"${bin}" scan --json >/dev/null
"${bin}" stats --json >/dev/null
"${bin}" analyze --json >/dev/null
"${bin}" clean --dry-run --json >/dev/null

echo "CLI smoke checks passed"
