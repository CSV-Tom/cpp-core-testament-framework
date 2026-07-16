# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- `TestEventHandlerChain` to support dispatching test events to multiple handlers (Observer pattern).
- `CONTRIBUTING.md` with contribution guidelines and Conventional Commits conventions.
- `CHANGELOG.md` to track notable changes.
- "Using in Your Project" section in `README.md` documenting `FetchContent`/`ExternalProject_Add` integration.

### Changed

- Applied consistent astyle formatting across all C++ sources.
- Switched the container tooling from Docker to Podman; renamed `Dockerfile` to `Containerfile` and `scripts/build-docker-image.sh`/`scripts/run-docker-image.sh` to `scripts/build-image.sh`/`scripts/run-container.sh`.
- Upgraded the build container to g++-16, the latest compiler available in Ubuntu 26.04 LTS.
- Restructured `README.md` with concrete, code-grounded feature descriptions.

### Fixed

- Undefined behavior in `makeSuite<T>`.

### Removed

- Dead code in `src/old/` and commented-out blocks in `InternalRegistry`.
- Commented-out `EventManager`-based `ServiceLocator` alternative.

## [0.1.0] - 2025

### Added

- Initial `CoreTestament` framework: macro-free, C++23 test suites, lifecycle hooks, and assertions.
- `CoreServiceLocator` module providing a lightweight service locator for dependency wiring.
- Console test event handler wired into `CoreTestament` test execution.
- Docker-based build environment (Ubuntu, g++-15) with CMake/Ninja toolchains for GCC 14, GCC 15, and Clang.
- `build.sh` and `quickstart.sh` scripts for building and running the test suites.

[Unreleased]: https://github.com/CSV-Tom/cpp-core-testament-framework/compare/HEAD
[0.1.0]: https://github.com/CSV-Tom/cpp-core-testament-framework/releases/tag/v0.1.0
