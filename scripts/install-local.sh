#!/usr/bin/env bash
set -euo pipefail
prefix="${1:-$HOME/.local/bin}"
mkdir -p "$prefix"
cmake --build build --parallel
install -m 0755 build/devclean "$prefix/devclean"
printf 'Installed devclean to %s/devclean\n' "$prefix"
