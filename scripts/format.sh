#!/usr/bin/env bash

find . \( -path ./build -o -path ./.git \) -prune -o \
    -type f \( -name "*.cpp" -o -name "*.hpp" \) -exec clang-format -i {} \;
