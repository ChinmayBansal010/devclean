#!/usr/bin/env bash
set -euo pipefail
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/devclean --version
./build/devclean --help >/dev/null
