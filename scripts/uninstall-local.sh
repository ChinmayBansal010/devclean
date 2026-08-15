#!/usr/bin/env bash
set -euo pipefail
prefix="${1:-$HOME/.local/bin}"
rm -f "$prefix/devclean"
printf 'Removed %s/devclean if it existed.\n' "$prefix"
