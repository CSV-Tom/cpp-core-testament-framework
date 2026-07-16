cmake_minimum_required(VERSION 3.23)

project(CoreTestamentFramework LANGUAGES CXX VERSION 1.0.0)

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

add_library(${PROJECT_NAME} STATIC)
add_library(CoreTestament::CoreTestament ALIAS ${PROJECT_NAME})

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
        src/core/Runner.cpp
        src/core/Suite.cpp
        src/core/LifecycleSuite.cpp
        src/core/Test.cpp
    PUBLIC FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/Testament/Asserts.hpp
            include/Testament/FunctionVariant.hpp
            include/Testament/LifecycleSuite.hpp
            include/Testament/Runner.hpp
            include/Testament/Suite.hpp
            include/Testament/Test.hpp
            include/Testament/Testament.hpp
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
        LifecycleSuiteNameValidation LifecycleSuiteName
        tests/Tests/LifecycleSuiteNameValidation.cpp
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
        LifecycleHookFailureValidation LifecycleHookFailure
        tests/Tests/LifecycleHookFailureValidation.cpp
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
            -DTEST_CONFIG=$<CONFIG>
            -DTEST_CTEST_COMMAND=${CMAKE_CTEST_COMMAND}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/InstallConsumerTest.cmake
    )
    set_tests_properties(CoreTestamentInstallConsumer PROPERTIES LABELS integration)
endif()


# MyLibrary/
# ├── CMakeLists.txt           # Haupt-CMake-Konfigurationsdatei
# ├── include/                 # Öffentliche Header für die Bibliothek
# │   ├── MyLibrary/           # Namespace-Struktur
# │   │   ├── PublicHeader1.h  # Beispiel für einen öffentlichen Header
# │   │   ├── PublicHeader2.h  # Beispiel für einen öffentlichen Header
# │   │   └── Config.h         # Öffentliche Konfigurationsdatei
# ├── src/                     # Quellcode der Bibliothek
# │   ├── CMakeLists.txt       # CMake-Konfiguration für die Bibliothek
# │   ├── InternalModule1.cpp  # Interne Implementierung
# │   ├── InternalModule2.cpp  # Interne Implementierung
# │   └── SharedImplementation.cpp # Gemeinsame Implementierung
# ├── modules/                 # C++20/23-Module (optional)
# │   ├── PublicModule.ixx     # Exportiertes Modul
# │   ├── InternalModule.ixx   # Nicht exportiertes Modul
# ├── tests/                   # Testcode
# │   ├── CMakeLists.txt       # CMake-Konfiguration für Tests
# │   ├── UnitTest1.cpp        # Beispiel für einen Unit-Test
# │   └── UnitTest2.cpp        # Beispiel für einen weiteren Test
# └── examples/                # Beispielanwendungen
#     ├── Example1.cpp         # Erste Beispielanwendung
#     ├── Example2.cpp         # Zweite Beispielanwendung
#     └── CMakeLists.txt       # CMake-Konfiguration für Beispiele
