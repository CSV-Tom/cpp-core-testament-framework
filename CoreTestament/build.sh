#!/usr/bin/env bash

set -euo pipefail

dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

pushd "$dir" > /dev/null || exit 2

#mkdir -p build/ && cd build && cmake .. && make -j


rm -rf build/ && mkdir -p build/ && cd build && cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=../toolchains/clang19-toolchain-modules.cmake .. && ninja -v -j $(nproc)

./TestRunner
#find . -name "TestRunner"
# Work g++-15
#rm -rf build/ && mkdir -p build/ && cd build && cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=../toolchains/gcc15-toolchain-modules.cmake .. && ninja -v -j $(nproc)

# DO NOT WORK g++-14
# rm -rf build/ && mkdir -p build/ && cd build && cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=../toolchains/gcc15-toolchain-modules.cmake .. && ninja -v -j $(nproc)



popd > /dev/null || exit 3
