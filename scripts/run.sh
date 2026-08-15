#!/usr/bin/env bash
set -euo pipefail
if [[ ! -x build/devclean ]]; then
  echo "build/devclean not found; run scripts/configure.sh and scripts/build.sh first" >&2
  exit 1
fi
exec ./build/devclean "$@"
