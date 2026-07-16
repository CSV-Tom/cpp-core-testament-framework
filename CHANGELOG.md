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
- Lowered the minimum required CMake version to 3.23, matching the `FILE_SET HEADERS` feature already in use.
- `CoreTestament`'s test runner and CTest registration now respect the `BUILD_TESTING` option instead of always building.
- Selected compiler warning flags per compiler (GNU/Clang vs. MSVC) instead of hardcoding GCC-specific flags.
- Simplified `build.sh` to use `cmake --build`/`ctest` without deleting the build directory on every run.

### Fixed

- Undefined behavior in `makeSuite<T>`.
- `CoreServices` public headers are now installed via an explicit `FILE_SET HEADERS` file list, so the exported package includes every header.
- `CoreServices`' build-time include path now resolves relative to the component directory instead of the repository root.
- `CoreTestament` suites now reset their statistics, timers, and hook errors between repeated runs instead of accumulating stale state.
- Test execution timers are now stopped when a test throws, so reported durations stop increasing.
- A failing `beforeAll` lifecycle hook now aborts the suite and propagates as a runner failure instead of being silently ignored.
- `CoreServiceLocator` now rejects null service registrations with `invalid_argument` instead of allowing them into the registry.

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
