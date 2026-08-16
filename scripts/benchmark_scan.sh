#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
bin="${1:-${root}/build/devclean}"
iterations="${ITERATIONS:-5}"

[[ -x "${bin}" ]]

for ((i=1; i<=iterations; i++)); do
    printf 'scan iteration %d/%d: ' "$i" "$iterations"
    /usr/bin/time -f '%e seconds' "${bin}" scan --json >/dev/null
done
