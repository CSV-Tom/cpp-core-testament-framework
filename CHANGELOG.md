# Changelog

All notable changes will be documented in this file. No public version has been
released yet.

## [Unreleased]

### Added

- Declarative `Suite`, `Test`, `ParameterizedTest`, `Cases` and `TestCase` DSL.
- Compile-time validation for fixtures, test callables and parameter structures.
- Named parameter cases materialized as independent runtime tests with their own
  results, timing, retries, filters, lifecycle hooks and JUnit elements.
- Move-only parameter support when bodies consume values through const references.
- `SuiteRegistration`, a move-only RAII value that keeps automatic static suite
  registration active.
- Controlled collection and runner reporting of configuration errors discovered
  during static initialization.
- Public custom reporter API, console and JUnit reporters.
- Structured assertion failures with source locations and expected/actual values.
- Runtime skipping with an optional reason and skipped-result reporting.
- Global test environments with process-wide setup and teardown hooks.
- Suite/test glob filters, negation and tag expressions, including the
  `--filter` and `--list-tests` command-line options.
- Repeated and reproducibly shuffled runs through `--repeat`, `--shuffle` and
  `--seed`.
- Thread-local scoped trace contexts attached to assertion failures.
- Configurable parallel suite and fixture-less test execution with deterministic
  event delivery and explicit serial overrides.
- Non-fatal assertions, heterogeneous equality, smart-pointer null checks,
  range comparisons, floating-point comparisons and exception assertions.
- Install-and-consume, compile-fail, multi-translation-unit and sanitizer-oriented
  regression coverage, including AddressSanitizer, UndefinedBehaviorSanitizer
  and ThreadSanitizer configurations.
- Documentation entry point and a standalone getting-started tutorial.
- ARM Linux hard-float cross-compilation and QEMU execution coverage for
  `arm-linux-gnueabihf`.
- Clang-based AddressSanitizer, LeakSanitizer and UndefinedBehaviorSanitizer
  tooling, plus `gcovr` coverage support in the development container.

### Changed

- `TestOptions::maxAttempts()` now expresses the total number of attempts instead
  of exposing ambiguous retry counts.
- Suites and tests execute deterministically by configured order and name.
- Every retry executes a complete `beforeEach`, body and `afterEach` attempt.
- Lifecycle failures are explicit test errors and contribute to statistics.
- Test result events expose current statistics and immutable option snapshots.
- Public concrete value types hide their implementation behind PImpl; template
  definitions remain header-defined where required by C++.
- Reporters receive command-line arguments and structured suite, test and final
  run information.
- Runtime implementation sources are organized by API, configuration, reporting
  and execution responsibilities under `Testament::detail`.
- CoreTestament's development package version is `0.1.0`.
- CoreTestament can be built as either a static or shared library through
  `BUILD_SHARED_LIBS`.

### Fixed

- Suite registration is atomic; invalid suites leave no partial registry state.
- Invalid namespace-scope definitions no longer terminate before `main()` for
  supported configuration errors; the runner exits with code 2 instead.
- Registry filtering returns snapshots instead of lazy views crossing lock scope.
- Repeated suite runs reset statistics, timers and hook errors.
- Assertion and option PImpl values remain readable after moves.
- Fixture type mismatches are diagnosed instead of invoking undefined behavior.
- Lifecycle hooks and reporter failures remain contained and contribute to the
  final result without bypassing custom handlers.
- Test and suite definition locations are preserved for diagnostics and reports.
- Compiler flags, installation paths, exported headers and package targets are
  portable across supported CMake generators.
- Shared-library exports are limited to the supported public API and the
  installed DSL runtime bridge; implementation and PImpl symbols stay hidden.
- `AssertionFailure` and `SkipRequest` now transfer their PImpl storage in move
  operations instead of incrementing its reference count.
- MSVC compile-failure tests inherit the standard-library include directories
  detected by CMake when invoking the compiler directly.

### Removed

- Legacy `makeTest`, `makeSuite` and `makeParameterizedTest` factories.
- Public runtime `Test` and `Suite` handle classes.
- Ambiguous `TestOptions::retries()` and `retryCount()` APIs.
- Unused event-system experiments, duplicate service locator and dead CMake/test
  fragments.

[Unreleased]: https://github.com/CSV-Tom/cpp-core-testament-framework/compare/HEAD
