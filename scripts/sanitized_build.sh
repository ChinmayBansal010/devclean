#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_dir="${root}/build-sanitize"

cmake -S "${root}" -B "${build_dir}" -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DDEVCLEAN_ENABLE_SANITIZERS=ON \
  -DCMAKE_TOOLCHAIN_FILE="${root}/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build "${build_dir}" --parallel
ctest --test-dir "${build_dir}" --output-on-failure
