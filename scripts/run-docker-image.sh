#!/usr/bin/env bash

docker run --rm \
	--user "$(id -u)":"$(id -g)" \
	--volume="/etc/group:/etc/group:ro" \
	--volume="/etc/passwd:/etc/passwd:ro" \
	--volume="/etc/shadow:/etc/shadow:ro" \
	--volume "$(pwd)":"$(pwd)" \
	--workdir "$(pwd)" \
        cxx23-core-testament-framework $*
