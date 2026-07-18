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
- Install-and-consume, compile-fail, multi-translation-unit and sanitizer-oriented
  regression coverage.

### Changed

- `TestOptions::maxAttempts()` now expresses the total number of attempts instead
  of exposing ambiguous retry counts.
- Suites and tests execute deterministically by configured order and name.
- Every retry executes a complete `beforeEach`, body and `afterEach` attempt.
- Lifecycle failures are explicit test errors and contribute to statistics.
- Test result events expose current statistics and immutable option snapshots.
- CoreTestament's development package version is `0.1.0`.

### Fixed

- Suite registration is atomic; invalid suites leave no partial registry state.
- Invalid namespace-scope definitions no longer terminate before `main()` for
  supported configuration errors; the runner exits with code 2 instead.
- Registry filtering returns snapshots instead of lazy views crossing lock scope.
- Repeated suite runs reset statistics, timers and hook errors.
- Assertion and option PImpl values remain readable after moves.
- Compiler flags, installation paths, exported headers and package targets are
  portable across supported CMake generators.

### Removed

- Legacy `makeTest`, `makeSuite` and `makeParameterizedTest` factories.
- Public runtime `Test` and `Suite` handle classes.
- Ambiguous `TestOptions::retries()` and `retryCount()` APIs.
- Unused event-system experiments, duplicate service locator and dead CMake/test
  fragments.

[Unreleased]: https://github.com/CSV-Tom/cpp-core-testament-framework/compare/HEAD
