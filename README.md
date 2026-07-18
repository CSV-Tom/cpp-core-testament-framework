# Core Testament Framework

> **Note:** This is an evaluation project. No public release has been published yet.

Core Testament is a macro-free C++23 test framework with compile-time-validated
test definitions and automatic static registration before `main()`.

## Minimal example

```cpp
#include <Testament/Testament.hpp>

namespace {

inline const auto mathSuite = Testament::Suite(
    "Math Suite",
    Testament::Test("Addition", [] {
        Testament::Asserts::assertEquals(4, 2 + 2);
    }),
    Testament::ParameterizedTest(
        "Division",
        Testament::Cases(
            Testament::TestCase("10 divided by 2", 10, 2, 5),
            Testament::TestCase("20 divided by 4", 20, 4, 5)
        ),
        [](int dividend, int divisor, int expected) {
            Testament::Asserts::assertEquals(expected, dividend / divisor);
        }
    )
);

}

int main(int argc, char** argv) {
    return Testament::run(argc, argv);
}
```

Every named parameter case becomes an independent runtime test. The example
therefore reports `Division / 10 divided by 2` and
`Division / 20 divided by 4` separately, with independent timing, retries,
filtering, lifecycle hooks and JUnit `<testcase>` elements.

## Fixtures and options

The DSL consistently uses the order `name, options, data, body`:

```cpp
class DatabaseFixture : public Testament::LifecycleSuite {
protected:
    void beforeEach() override { connect(); }
    void afterEach() override { disconnect(); }

public:
    void connect();
    void disconnect();
    void insert(int value);
};

inline const auto databaseSuite = Testament::Suite<DatabaseFixture>(
    "Database Suite",
    Testament::SuiteOptions{}.order(10).tag("integration"),
    Testament::Test(
        "Insert record",
        Testament::TestOptions{}
            .order(-5)
            .tag("database")
            .attribute("operation", "insert")
            .maxAttempts(3),
        [](DatabaseFixture& fixture) {
            fixture.insert(42);
        }
    ),
    Testament::ParameterizedTest(
        "Insert values",
        Testament::TestOptions{}.tag("parameterized"),
        Testament::Cases(
            Testament::TestCase("positive", 42),
            Testament::TestCase("zero", 0),
            Testament::TestCase("negative", -42)
        ),
        [](DatabaseFixture& fixture, int value) {
            fixture.insert(value);
        }
    )
);
```

Fixture and callable compatibility is checked while templates are instantiated.
An incompatible fixture body or parameter signature fails to compile.
`maxAttempts(3)` means at most three total attempts. Every attempt receives its
own `beforeEach`/`afterEach` pair; `beforeAll` and `afterAll` run once per suite.

## Registration model

`Test(...)` and `ParameterizedTest(...)` create typed definition objects.
`Suite(...)` validates and materializes the complete suite, then registers it
atomically. The namespace-scope variable initialization performs the actual
registration before `main()`:

```cpp
namespace {
inline const auto suite = Testament::Suite(
    "Automatically registered",
    Testament::Test("Example", [] {})
);
}
```

This is not compile-time registration. Standard C++ cannot enumerate arbitrary
test objects across translation units during compilation. Runtime-dependent
configuration errors encountered during static initialization are collected by
the registry. The runner prints them, executes no tests and returns exit code 2.

The returned `SuiteRegistration` is a move-only RAII value and must remain alive.
For namespace-scope definitions this naturally lasts until program shutdown.

## Multiple translation units and linking

Put test translation units directly into the executable:

```cmake
add_executable(project-tests
    tests/main.cpp
    tests/DatabaseTests.cpp
    tests/MathTests.cpp
)
target_link_libraries(project-tests PRIVATE CoreTestament::CoreTestament)
```

An Object Library is also safe:

```cmake
add_library(project-test-objects OBJECT
    tests/DatabaseTests.cpp
    tests/MathTests.cpp
)

add_executable(project-tests
    tests/main.cpp
    $<TARGET_OBJECTS:project-test-objects>
)
target_link_libraries(project-tests PRIVATE CoreTestament::CoreTestament)
```

Only translation units actually linked into the executable can register suites.
Self-registering object files placed only in a static library may be discarded
by the linker when no referenced symbol pulls them in. Prefer direct sources or
an Object Library instead of relying on whole-archive flags.

## JUnit reports

The default runner always writes console output. Add a JUnit report with:

```bash
project-tests --junit test-results.xml
```

Custom synchronous reporters can be installed on a configurable `Runner` via
`addHandler()`.

## Quickstart

The quickstart builds the Podman image and runs the complete build and test suite:

```bash
./quickstart.sh
```

## Using in another project

### FetchContent

Until a release tag exists, pin an audited commit instead of tracking `main`:

```cmake
include(FetchContent)
FetchContent_Declare(
    CoreTestament
    GIT_REPOSITORY https://github.com/CSV-Tom/cpp-core-testament-framework.git
    GIT_TAG <commit-sha>
    SOURCE_SUBDIR CoreTestament
)
FetchContent_MakeAvailable(CoreTestament)

target_link_libraries(project-tests PRIVATE CoreTestament::CoreTestament)
```

### Installed package

```cmake
find_package(CoreTestament CONFIG REQUIRED)
target_link_libraries(project-tests PRIVATE CoreTestament::CoreTestament)
```

The repository validates this path with an install-and-consume integration test
that uses only the public DSL.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for code style, tests and Conventional
Commit conventions.

## License

This project is licensed under the [MIT License](LICENSE).
