#!/usr/bin/env bash

docker build -t cxx23-core-testament-framework "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/.."
