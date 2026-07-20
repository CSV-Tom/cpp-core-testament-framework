# Getting Started Tutorial

This tutorial creates a standalone C++23 test executable with Core Testament.
It uses `FetchContent` because no public release has been published yet. For a
stable project, replace `<commit-sha>` with a reviewed commit.

## 1. Create the project

Start with this layout:

```text
calculator-tests/
├── CMakeLists.txt
└── tests/
    └── CalculatorTests.cpp
```

Create `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.23)
project(CalculatorTests LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    CoreTestament
    GIT_REPOSITORY https://github.com/CSV-Tom/cpp-core-testament-framework.git
    GIT_TAG <commit-sha>
    SOURCE_SUBDIR CoreTestament
)
FetchContent_MakeAvailable(CoreTestament)

add_executable(calculator-tests tests/CalculatorTests.cpp)
target_link_libraries(calculator-tests PRIVATE CoreTestament::CoreTestament)
target_compile_features(calculator-tests PRIVATE cxx_std_23)
```

## 2. Define the first suite

Create `tests/CalculatorTests.cpp`:

```cpp
#include <Testament/Testament.hpp>

using namespace Testament;

namespace {

inline const auto calculatorSuite = Suite(
    "Calculator",
    Test("adds two values", [] {
        Asserts::assertEquals(4, 2 + 2);
    }),
    ParameterizedTest(
        "multiplies values",
        Cases(
            TestCase("positive values", 2, 3, 6),
            TestCase("with zero", 8, 0, 0),
            TestCase("negative value", -2, 4, -8)
        ),
        [](int left, int right, int expected) {
            Asserts::assertEquals(expected, left * right);
        }
    )
);

}

int main(int argc, char** argv) {
    return Testament::run(argc, argv);
}
```

The namespace-scope `calculatorSuite` keeps its `SuiteRegistration` alive and
registers the suite before `main()`. Each named parameter case is reported as an
independent test.

## 3. Configure, build and run

```bash
cmake -S . -B build
cmake --build build --parallel
./build/calculator-tests
```

Always return the runner's exit code from `main`; otherwise a failing test could
still produce a successful process exit code.

## 4. Filter and report

Run matching tests only:

```bash
./build/calculator-tests --filter='Calculator/*zero*'
```

Write a JUnit report in addition to console output:

```bash
./build/calculator-tests --junit build/test-results.xml
```

List registered tests without executing them:

```bash
./build/calculator-tests --list-tests
```

## 5. Continue from here

The [documentation overview](../README.md) explains fixtures and lifecycle
hooks, test and suite options, retries, parallel execution, static registration
across translation units and custom reporters.
