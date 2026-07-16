# Core Testament Framework

> **Note:** This is an evaluation project.

**Core Testament Framework** is a macro-free C++23 testing library.

## Key Features

- **Macro-Free Test Definitions**: Suites, tests, and assertions are plain C++23 — no `TEST()`/`ASSERT_*` macros.
- **Lifecycle Hooks**: `LifecycleSuite` provides setup/teardown hooks around test execution.
- **Parameterized Tests**: Run the same test logic across multiple input sets.
- **Pluggable Event Reporting**: An event-driven system (`TestEventHandlerChain`, `ConsoleTestEventHandler`, `LogEventHandler`) lets you observe or customize how results are reported.
- **Execution Timing & Statistics**: Built-in timers and pass/fail/skip statistics per suite.
- **Optional Dependency Injection**: The bundled `CoreServiceLocator` module offers lightweight service wiring for tests.

## Quickstart

```bash
./quickstart.sh
```

## Using in Your Project

### FetchContent (recommended)

Pulls the source in at configure time and builds it as part of your own
build tree, so the `CoreTestamentFramework` target is directly linkable:

```cmake
include(FetchContent)
FetchContent_Declare(
    CoreTestamentFramework
    GIT_REPOSITORY https://github.com/CSV-Tom/cpp-core-testament-framework.git
    GIT_TAG main
)
FetchContent_MakeAvailable(CoreTestamentFramework)

target_link_libraries(YourTarget PRIVATE CoreTestamentFramework)
```

### ExternalProject_Add (alternative)

Builds the framework as a fully separate, isolated CMake project during your
build. Since `CoreTestamentFramework` does not currently export an install
config, you'd need to point at the built artifacts manually (no
`find_package`/`target_link_libraries` support):

```cmake
include(ExternalProject)
ExternalProject_Add(
    core_testament
    GIT_REPOSITORY https://github.com/CSV-Tom/cpp-core-testament-framework.git
    GIT_TAG main
    INSTALL_COMMAND ""
)
```

## Contributing

Contributions are welcome. See [CONTRIBUTING.md](CONTRIBUTING.md) for code style and commit message conventions, and how to submit changes.

Maintainer: [@CSV-Tom](https://github.com/CSV-Tom)

## Changelog

See [CHANGELOG.md](CHANGELOG.md) for a history of notable changes to this project.

## License

This project is licensed under the [MIT License](LICENSE).
