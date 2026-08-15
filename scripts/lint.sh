#!/usr/bin/env bash
set -euo pipefail
cmake --build build --target devclean devclean_core --parallel
