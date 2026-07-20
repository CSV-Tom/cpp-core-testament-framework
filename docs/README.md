# Core Testament Documentation

This is the main entry point for Core Testament documentation. Start with the
[step-by-step tutorial](tutorial/README.md) to build a small test executable, or
use this page as the complete DSL and integration guide. For the project
overview, see the top-level [README](../README.md).

## Contents

- [Adding Core Testament to your project](#adding-core-testament-to-your-project)
- [Minimal example](#minimal-example)
- [Fixtures and options](#fixtures-and-options)
- [Registration model](#registration-model)
- [Multiple translation units and linking](#multiple-translation-units-and-linking)
- [JUnit reports](#junit-reports)
- [Getting-started tutorial](tutorial/README.md)

## Adding Core Testament to your project

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

## Minimal example

The DSL lives in `namespace Testament`. The first example below spells every
name out in full so you can see where each symbol comes from:

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

In practice, a `using namespace Testament;` at the top of a test file removes
the repeated prefix and reads just as clearly, since suite files are dedicated
test translation units rather than general-purpose code. The remaining
examples in this guide use that style:

```cpp
#include <Testament/Testament.hpp>

using namespace Testament;

namespace {

inline const auto mathSuite = Suite(
    "Math Suite",
    Test("Addition", [] {
        Asserts::assertEquals(4, 2 + 2);
    })
);

}

int main(int argc, char** argv) {
    return run(argc, argv);
}
```

## Fixtures and options

Every DSL function starts with `name`, then an optional options object, then
(for `ParameterizedTest`) the case data, and always ends with the test body:
`Test(name, [options,] body)`, `ParameterizedTest(name, [options,] cases, body)`.

A fixture can override all four lifecycle hooks: `beforeAll`/`afterAll` run
once for the whole suite (e.g. to set up and tear down a shared connection
pool), while `beforeEach`/`afterEach` run around every individual attempt
(e.g. to open and close a transaction per test):

```cpp
class DatabaseFixture : public LifecycleSuite {
protected:
    void beforeAll() override { connectPool(); }
    void afterAll() override { disconnectPool(); }
    void beforeEach() override { beginTransaction(); }
    void afterEach() override { rollbackTransaction(); }

public:
    void connectPool();
    void disconnectPool();
    void beginTransaction();
    void rollbackTransaction();
    void insert(int value);
    bool isValidLabel(const std::string& label);
};

inline const auto databaseSuite = Suite<DatabaseFixture>(
    "Database Suite",
    SuiteOptions{}.order(10).tag("integration"),
    Test(
        "Insert record",
        TestOptions{}
            .order(-5)
            .tag("database")
            .attribute("operation", "insert")
            .maxAttempts(3),
        [](DatabaseFixture& fixture) {
            fixture.insert(42);
        }
    ),
    ParameterizedTest(
        "Insert values",
        TestOptions{}.tag("parameterized"),
        Cases(
            TestCase("positive", 42),
            TestCase("zero", 0),
            TestCase("negative", -42)
        ),
        [](DatabaseFixture& fixture, int value) {
            fixture.insert(value);
        }
    ),
    ParameterizedTest(
        "Reject invalid labels",
        Cases(
            TestCase("empty label", std::string{}),
            TestCase("label with spaces", std::string{"has space"})
        ),
        [](DatabaseFixture& fixture, const std::string& label) {
            Asserts::assertFalse(fixture.isValidLabel(label), "label should be rejected");
        }
    )
);
```

Fixture and callable compatibility is checked while templates are instantiated.
An incompatible fixture body or parameter signature fails to compile. All
`TestCase` entries passed to one `Cases(...)` call must share the same
parameter types; mixing e.g. `int` and `std::string` cases fails to compile.

`maxAttempts(3)` means at most three total attempts. Every attempt receives its
own `beforeEach`/`afterEach` pair; `beforeAll` and `afterAll` run once per
suite regardless of how many attempts individual tests need.

`order(...)` controls execution order within its scope (suites among suites,
tests within a suite): lower values run first, and entries with the same
(or no) explicit order run in name order rather than in an unspecified one.

## Registration model

`Test(...)` and `ParameterizedTest(...)` create typed definition objects.
`Suite(...)` validates and materializes the complete suite, then registers it
atomically. The namespace-scope variable initialization performs the actual
registration before `main()`:

```cpp
namespace {
inline const auto suite = Suite(
    "Automatically registered",
    Test("Example", [] {})
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
