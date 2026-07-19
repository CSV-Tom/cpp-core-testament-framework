cmake_minimum_required(VERSION 3.23)

project(CoreTestamentFramework LANGUAGES CXX VERSION 0.1.0)

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
find_package(Threads REQUIRED)

add_library(${PROJECT_NAME} STATIC)
add_library(CoreTestament::CoreTestament ALIAS ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_EXTENSIONS OFF
    EXPORT_NAME CoreTestament
)

target_sources(${PROJECT_NAME}
    PRIVATE
        src/core/Internal/InternalTest.cpp
        src/core/Internal/InternalSuite.cpp
        src/core/Internal/InternalRegistry.cpp
        src/core/Internal/ConfigurationDiagnostics.cpp
        src/core/Internal/SuiteAssembler.cpp
        src/core/Runner.cpp
        src/core/SuiteRegistration.cpp
        src/core/LifecycleSuite.cpp
        src/core/TestHandle.cpp
        src/core/SuiteOptions.cpp
        src/core/TestOptions.cpp
        src/core/AssertionFailure.cpp
        src/core/Asserts.cpp
        src/core/SkipRequest.cpp
        src/core/Trace.cpp
        src/EventHandlers/JUnitTestEventHandler.cpp
    PUBLIC FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/Testament/Asserts.hpp
            include/Testament/AssertionFailure.hpp
            include/Testament/Execution.hpp
            include/Testament/GlobalEnvironment.hpp
            include/Testament/LifecycleSuite.hpp
            include/Testament/ParameterizedTest.hpp
            include/Testament/Runner.hpp
            include/Testament/Reporters.hpp
            include/Testament/Skip.hpp
            include/Testament/SkipRequest.hpp
            include/Testament/Suite.hpp
            include/Testament/SuiteOptions.hpp
            include/Testament/SuiteRegistration.hpp
            include/Testament/Test.hpp
            include/Testament/TestCase.hpp
            include/Testament/TestEventHandler.hpp
            include/Testament/TestOptions.hpp
            include/Testament/Trace.hpp
            include/Testament/Testament.hpp
            include/Testament/detail/CaseSet.hpp
            include/Testament/detail/Concepts.hpp
            include/Testament/detail/LocatedName.hpp
            include/Testament/detail/NamedTestCase.hpp
            include/Testament/detail/ParameterizedTestDefinition.hpp
            include/Testament/detail/RuntimeBridge.hpp
            include/Testament/detail/TestDefinition.hpp
            include/Testament/detail/TestHandle.hpp
)


target_include_directories(
    ${PROJECT_NAME} PRIVATE src
)
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
    >
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT CoreTestamentTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT CoreTestamentTargets
    FILE CoreTestamentTargets.cmake
    NAMESPACE CoreTestament::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CoreTestament
)

configure_package_config_file(
    cmake/CoreTestamentConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CoreTestamentConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CoreTestament
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/CoreTestamentConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/CoreTestamentConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/CoreTestamentConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CoreTestament
)

if(BUILD_TESTING)
    function(add_testament_test target test_name)
        add_executable(${target} ${ARGN})
        target_link_libraries(${target} PRIVATE CoreTestament::CoreTestament)
        set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
        target_compile_options(${target} PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
                -Wall
                -Wextra
                -Wpedantic
                -Wshadow
            >
            $<$<CXX_COMPILER_ID:MSVC>:/W4>
        )
        add_test(NAME ${test_name} COMMAND ${target})
    endfunction()

    function(add_testament_internal_test target test_name source)
        add_testament_test(${target} ${test_name} ${source})
        target_include_directories(${target} PRIVATE src)
    endfunction()

    add_testament_test(TestRunner AllTests
            tests/Tests/LifecycleSuiteValidation.cpp
            tests/Tests/ParameterizedSuiteValidation.cpp
            tests/Tests/SuiteValidation.cpp
            tests/mainTest.cpp
    )

    add_testament_test(
        MultiTranslationUnitValidation MultiTranslationUnit
        tests/multi-tu/FirstSuite.cpp
        tests/multi-tu/SecondSuite.cpp
        tests/multi-tu/main.cpp
    )

    add_testament_internal_test(
        LifecycleSuiteNameValidation LifecycleSuiteName
        tests/Tests/LifecycleSuiteNameValidation.cpp
    )

    add_testament_test(
        FixtureTypeValidation FixtureType
        tests/Tests/FixtureTypeValidation.cpp
    )

    add_testament_test(
        FixturePerRunValidation FixturePerRun
        tests/Tests/FixturePerRunValidation.cpp
    )

    add_testament_test(
        ParameterizedBehaviorValidation ParameterizedBehavior
        tests/Tests/ParameterizedBehaviorValidation.cpp
    )

    add_testament_internal_test(
        ConfigurationErrorValidation ConfigurationError
        tests/Tests/ConfigurationErrorValidation.cpp
    )

    add_testament_internal_test(
        ConfigurationErrorLifetimeValidation ConfigurationErrorLifetime
        tests/Tests/ConfigurationErrorLifetimeValidation.cpp
    )

    add_testament_test(
        AssertsValidation Asserts
        tests/Tests/AssertsValidation.cpp
    )

    add_testament_test(
        NonFatalAssertionsValidation NonFatalAssertions
        tests/Tests/NonFatalAssertionsValidation.cpp
    )

    add_testament_internal_test(
        InternalSuiteRepeatRunValidation InternalSuiteRepeatRun
        tests/Tests/InternalSuiteRepeatRunValidation.cpp
    )

    add_testament_internal_test(
        InternalTestTimerValidation InternalTestTimer
        tests/Tests/InternalTestTimerValidation.cpp
    )

    add_testament_internal_test(
        InternalTestContextValidation InternalTestContext
        tests/Tests/InternalTestContextValidation.cpp
    )

    add_testament_internal_test(
        InternalRegistryFilterValidation InternalRegistryFilter
        tests/Tests/InternalRegistryFilterValidation.cpp
    )

    add_testament_internal_test(
        SuiteLifetimeValidation SuiteLifetime
        tests/Tests/SuiteLifetimeValidation.cpp
    )

    add_testament_test(
        HandlerArgumentsValidation HandlerArguments
        tests/Tests/HandlerArgumentsValidation.cpp
    )

    add_testament_test(
        NonStandardExceptionValidation NonStandardException
        tests/Tests/NonStandardExceptionValidation.cpp
    )

    add_testament_test(
        HandlerFailureIsolationValidation HandlerFailureIsolation
        tests/Tests/HandlerFailureIsolationValidation.cpp
    )

    add_testament_test(
        ConcurrentRunnerValidation ConcurrentRunner
        tests/Tests/ConcurrentRunnerValidation.cpp
    )

    add_testament_test(
        RunnerMoveValidation RunnerMove
        tests/Tests/RunnerMoveValidation.cpp
    )

    add_testament_test(
        RunnerFilterValidation RunnerFilter
        tests/Tests/RunnerFilterValidation.cpp
    )

    add_testament_test(
        ParallelSuiteValidation ParallelSuite
        tests/Tests/ParallelSuiteValidation.cpp
    )

    add_testament_test(
        ParallelTestValidation ParallelTest
        tests/Tests/ParallelTestValidation.cpp
    )

    add_testament_test(
        RuntimeSkipValidation RuntimeSkip
        tests/Tests/RuntimeSkipValidation.cpp
    )

    add_testament_test(
        ListTestsValidation ListTests
        tests/Tests/ListTestsValidation.cpp
    )

    add_testament_test(
        GlobalEnvironmentValidation GlobalEnvironment
        tests/Tests/GlobalEnvironmentValidation.cpp
    )

    add_testament_test(
        RepeatShuffleValidation RepeatShuffle
        tests/Tests/RepeatShuffleValidation.cpp
    )

    add_testament_test(
        TraceValidation Trace
        tests/Tests/TraceValidation.cpp
    )

    add_testament_test(
        OptionsValidation Options
        tests/Tests/OptionsValidation.cpp
    )

    add_testament_test(
        DefinitionLocationValidation DefinitionLocation
        tests/Tests/DefinitionLocationValidation.cpp
    )

    add_testament_test(
        TestOptionsExecutionValidation TestOptionsExecution
        tests/Tests/TestOptionsExecutionValidation.cpp
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang|MSVC")
        function(add_testament_compile_failure test_name source expected_diagnostic)
            add_test(
                NAME ${test_name}
                COMMAND ${CMAKE_COMMAND}
                    -DCXX_COMPILER=${CMAKE_CXX_COMPILER}
                    -DCOMPILER_ID=${CMAKE_CXX_COMPILER_ID}
                    -DINCLUDE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/include
                    -DSOURCE_FILE=${CMAKE_CURRENT_SOURCE_DIR}/${source}
                    -DEXPECTED_DIAGNOSTIC=${expected_diagnostic}
                    -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/CompileFailureTest.cmake
            )
            set_tests_properties(${test_name} PROPERTIES LABELS compile-fail)
        endfunction()

        add_test(
            NAME DiscardedHandles
            COMMAND ${CMAKE_COMMAND}
                -DCXX_COMPILER=${CMAKE_CXX_COMPILER}
                -DCOMPILER_ID=${CMAKE_CXX_COMPILER_ID}
                -DINCLUDE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/include
                -DSOURCE_FILE=${CMAKE_CURRENT_SOURCE_DIR}/tests/compile-fail/DiscardedHandles.cpp
                -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/DiscardedHandlesTest.cmake
        )
        set_tests_properties(DiscardedHandles PROPERTIES LABELS compile-fail)
        add_testament_compile_failure(
            InvalidFixtureDefinition tests/compile-fail/InvalidFixture.cpp DefinitionFor
        )
        add_testament_compile_failure(
            InvalidParameterDefinition tests/compile-fail/InvalidParameters.cpp DefinitionFor
        )
        add_testament_compile_failure(
            MixedCaseDefinition tests/compile-fail/MixedCases.cpp NamedTestCase
        )
    endif()

    add_testament_internal_test(
        InternalSuiteFilterValidation InternalSuiteFilter
        tests/Tests/InternalSuiteFilterValidation.cpp
    )

    add_testament_internal_test(
        LifecycleHookFailureValidation LifecycleHookFailure
        tests/Tests/LifecycleHookFailureValidation.cpp
    )

    add_testament_internal_test(
        LifecycleRetryValidation LifecycleRetry
        tests/Tests/LifecycleRetryValidation.cpp
    )

    add_testament_internal_test(
        JUnitReporterValidation JUnitReporter
        tests/Tests/JUnitReporterValidation.cpp
    )

    add_test(
        NAME CoreTestamentInstallConsumer
        COMMAND ${CMAKE_COMMAND}
            -DTEST_BINARY_DIR=${CMAKE_BINARY_DIR}
            -DTEST_CONSUMER_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/tests/consumer
            -DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/install-consumer-test
            -DTEST_GENERATOR=${CMAKE_GENERATOR}
            -DTEST_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}
            -DTEST_GENERATOR_TOOLSET=${CMAKE_GENERATOR_TOOLSET}
            -DTEST_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
            -DTEST_CXX_FLAGS=${CMAKE_CXX_FLAGS}
            -DTEST_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
            -DTEST_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
            -DTEST_CONFIG=$<CONFIG>
            -DTEST_CTEST_COMMAND=${CMAKE_CTEST_COMMAND}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/InstallConsumerTest.cmake
    )
    set_tests_properties(CoreTestamentInstallConsumer PROPERTIES LABELS integration)
endif()
