cmake_minimum_required(VERSION 3.31.2)

project(CoreServices LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Allow user to build as SHARED or STATIC
option(BUILD_SHARED_LIBS "Build CoreServices as a shared library" OFF)


add_library(${PROJECT_NAME})


target_sources(${PROJECT_NAME}
    PRIVATE 
        src/ServiceLocator.cpp
    PUBLIC FILE_SET HEADERS
        BASE_DIRS include
        PATTERN *.hpp 
)


# Set correct include paths for both build and install
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>    # Use source directory during build
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # Use install directory when installed
    PRIVATE 
        src
)


# Library Export Configuration
include(GNUInstallDirs)

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
    FILE CoreServicesConfig.cmake
    NAMESPACE CoreServices::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CoreServices
)

# Add version file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/CoreServicesConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/CoreServicesConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CoreServices
)


# Enable Testing
option(BUILD_TESTING "Enable tests" ON)

if(BUILD_TESTING)
    include(CTest)
    add_executable(Test${PROJECT_NAME})
    target_sources(Test${PROJECT_NAME} PRIVATE tests/main.cpp)
    target_link_libraries(Test${PROJECT_NAME} PRIVATE ${PROJECT_NAME})
    add_test(NAME CoreServicesTests COMMAND Test${PROJECT_NAME})
endif()
