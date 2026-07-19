cmake_minimum_required(VERSION 3.23)

project(CoreServices LANGUAGES CXX VERSION 0.1.0)

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
find_package(Threads REQUIRED)

# Allow user to build as SHARED or STATIC
option(BUILD_SHARED_LIBS "Build CoreServices as a shared library" OFF)


add_library(${PROJECT_NAME})
add_library(CoreServices::CoreServices ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

target_sources(${PROJECT_NAME}
    PRIVATE
        src/ServiceLocator.cpp
    PUBLIC FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/CoreServices/ServiceLocator.hpp
            include/CoreServices/ServiceLocator.tpp
)


# Set correct include paths for both build and install
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>    # Use source directory during build
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # Use install directory when installed
    PRIVATE
        src
)


# Library Export Configuration
install(
    TARGETS ${PROJECT_NAME}
    EXPORT CoreServicesTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}  # For SHARED libraries
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}  # For STATIC libraries
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # For executables (if needed)
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Generate a CMake config package
install(
    EXPORT CoreServicesTargets
    FILE CoreServicesTargets.cmake
    NAMESPACE CoreServices::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CoreServices
)

configure_package_config_file(
    cmake/CoreServicesConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CoreServicesConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CoreServices
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/CoreServicesConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/CoreServicesConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/CoreServicesConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CoreServices
)


if(BUILD_TESTING)
    function(add_core_services_test target source)
        add_executable(${target} ${source})
        target_link_libraries(${target} PRIVATE CoreServices::CoreServices)
        add_test(NAME ${target} COMMAND ${target})
    endfunction()

    add_core_services_test(CoreServicesBasicValidation tests/main.cpp)
    add_core_services_test(CoreServicesLifetimeValidation tests/LifetimeValidation.cpp)
    add_core_services_test(CoreServicesMoveValidation tests/MoveValidation.cpp)
    add_core_services_test(CoreServicesConcurrencyValidation tests/ConcurrencyValidation.cpp)

    add_test(
        NAME CoreServicesInstallConsumer
        COMMAND ${CMAKE_COMMAND}
            -DTEST_BINARY_DIR=${CMAKE_BINARY_DIR}
            -DTEST_CONSUMER_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/tests/consumer
            -DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/core-services-install-consumer-test
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
    set_tests_properties(CoreServicesInstallConsumer PROPERTIES LABELS integration)
endif()
