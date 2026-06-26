#!/usr/bin/env bash

find \( -name "*.cpp" -o -name "*.hpp" \) -type f -exec astyle {} \;
find -name "*.orig" -type f -delete