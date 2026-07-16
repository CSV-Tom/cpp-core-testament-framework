#!/usr/bin/env bash

podman run --rm -it \
	--userns=keep-id \
	--user "$(id -u)":"$(id -g)" \
	--volume="/etc/group:/etc/group:ro" \
	--volume="/etc/passwd:/etc/passwd:ro" \
	--volume "$(pwd)":"$(pwd)" \
	--workdir "$(pwd)" \
        core-testament-framework "$@"
