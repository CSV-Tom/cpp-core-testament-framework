enable_testing()

add_executable(TestRunner)

target_sources(TestRunner
    PRIVATE
        tests/TestAsserts.cpp
        tests/TestEventDispatcher.cpp
        tests/TestExecutionTimer.cpp
        tests/TestSuite.cpp
        tests/TestRegistry.cpp
)

target_link_libraries(TestRunner PRIVATE CoreTestamentFramework)

add_test(NAME AllTests COMMAND TestRunner)
