#!/usr/bin/env bash

set -euo pipefail

dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

pushd "$dir" > /dev/null || exit 2

#mkdir -p build/ && cd build && cmake .. && make -j


rm -rf build/ && mkdir -p build/ && cd build && cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=toolchains/clang-toolchain.cmake .. && ninja -v -j $(nproc)

# MODULES DO NOT WORK CORRECTLY WITH g++-14 
#rm -rf build/ && mkdir -p build/ && cd build && cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=toolchains/gcc14-toolchain.cmake .. && ninja -v -j $(nproc)

# MODULES WORK WITH g++-15
#rm -rf build/ && mkdir -p build/ && cd build && cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=toolchains/gcc15-toolchain.cmake .. && ninja -v -j $(nproc)

#find . -name "TestRunner"
./TestRunner


popd > /dev/null || exit 3
