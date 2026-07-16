#!/usr/bin/env bash

set -euo pipefail

dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
build_dir="${BUILD_DIR:-${dir}/build}"

cmake -S "$dir" -B "$build_dir" -G "${CMAKE_GENERATOR:-Ninja}" \
    -DBUILD_TESTING=ON "$@"
cmake --build "$build_dir" --parallel
ctest --test-dir "$build_dir" --output-on-failure --parallel "$(nproc)"
