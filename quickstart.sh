#!/usr/bin/env bash

set -euo pipefail

dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

pushd "$dir" > /dev/null || exit 2

./scripts/run-docker-image.sh ./CoreTestament/build.sh

popd > /dev/null || exit 2
