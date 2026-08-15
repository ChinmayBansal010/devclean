#!/usr/bin/env bash
set -euo pipefail
find include src tests -type f \( -name '*.cpp' -o -name '*.hpp' \) -print0 | xargs -0 clang-format -i
