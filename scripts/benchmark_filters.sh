#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
bin="${1:-${root}/build/devclean}"

[[ -x "${bin}" ]]
"${bin}" scan --active-only --json >/dev/null
"${bin}" scan --min-size 100MB --json >/dev/null
"${bin}" scan --min-size 100MB --max-size 2GB --json >/dev/null
"${bin}" stats --active-only --min-size 100MB --max-size 2GB --json >/dev/null
"${bin}" analyze --active-only --min-size 100MB --max-size 2GB --json >/dev/null

echo "filter benchmark smoke checks passed"
