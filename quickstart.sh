#!/usr/bin/env bash

set -euo pipefail

dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

pushd "$dir" > /dev/null || exit 2

./scripts/run-container.sh ./scripts/build.sh

popd > /dev/null || exit 2
