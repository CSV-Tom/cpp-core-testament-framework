#!/usr/bin/env bash

dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/.."

podman build -f "$dir/Containerfile" -t core-testament-framework "$dir"
