#!/usr/bin/env bash

docker build -t core-testament-framework "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/.."
