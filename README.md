# Core Testament Framework

> **Note:** This is an evaluation project.

**Core Testament Framework** is a macro-free C++23 testing library.

## Key Features

- **Macro-Free Test Definitions**: Suites, tests, and assertions are plain C++23 — no `TEST()`/`ASSERT_*` macros.
- **Lifecycle Hooks**: `LifecycleSuite` provides setup/teardown hooks around test execution.
- **Parameterized Tests**: Run the same test logic across multiple input sets.
- **Pluggable Event Reporting**: An event-driven system (`TestEventHandlerChain`, `ConsoleTestEventHandler`, `JUnitTestEventHandler`) lets you observe or customize how results are reported.
- **Execution Timing & Statistics**: Built-in timers and pass/fail/skip statistics per suite.
- **Optional Dependency Injection**: The bundled `CoreServiceLocator` module offers lightweight service wiring for tests.

## Quickstart

```bash
./quickstart.sh
```

## Using in Your Project

### FetchContent (recommended for building together)

Pulls the source in at configure time and builds it as part of your own
build tree:

```cmake
include(FetchContent)
FetchContent_Declare(
    CoreTestament
    GIT_REPOSITORY https://github.com/CSV-Tom/cpp-core-testament-framework.git
    GIT_TAG main
    SOURCE_SUBDIR CoreTestament
)
FetchContent_MakeAvailable(CoreTestament)

target_link_libraries(YourTarget PRIVATE CoreTestament::CoreTestament)
```

### find_package (recommended against an installed package)

`CoreTestament` installs a full CMake package config, including a
namespaced target alias. This is exercised end to end by an install/consumer
test in CI:

```cmake
find_package(CoreTestament CONFIG REQUIRED)
target_link_libraries(YourTarget PRIVATE CoreTestament::CoreTestament)
```

### ExternalProject_Add (alternative)

Builds the framework as a fully separate, isolated CMake project during your
build, with its own configure/build/install steps. Since `ExternalProject_Add`
does not add the dependency's targets to your build tree, you still pair it
with `find_package` against the install location it produces, rather than
linking against it directly:

```cmake
include(ExternalProject)
ExternalProject_Add(
    core_testament
    GIT_REPOSITORY https://github.com/CSV-Tom/cpp-core-testament-framework.git
    GIT_TAG main
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)
```

## Contributing

Contributions are welcome. See [CONTRIBUTING.md](CONTRIBUTING.md) for code style and commit message conventions, and how to submit changes.

Maintainer: [@CSV-Tom](https://github.com/CSV-Tom)

## Changelog

See [CHANGELOG.md](CHANGELOG.md) for a history of notable changes to this project.

## License

This project is licensed under the [MIT License](LICENSE).
