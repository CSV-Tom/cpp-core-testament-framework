#!/usr/bin/env bash

find . -type f \( -name "*.cpp" -o -name "*.hpp" \) -exec astyle {} \;
find . -type f -name "*.orig" -delete
