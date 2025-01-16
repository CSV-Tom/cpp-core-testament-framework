#!/usr/bin/env bash

set -euo pipefail

dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

pushd "$dir" > /dev/null || exit 2

mkdir -p build/ && cd build &&  cmake ..

make -j

popd > /dev/null || exit 3
