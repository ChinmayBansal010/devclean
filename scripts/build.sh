#!/usr/bin/env bash
set -euo pipefail
cmake --build build --parallel "${BUILD_JOBS:-}"
