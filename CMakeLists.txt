cmake_minimum_required(VERSION 3.23)

option(BUILD_TESTING "Enable tests" ON)
if(BUILD_TESTING)
    enable_testing()
endif()

project(TestFramework)

add_subdirectory(CoreTestament)
add_subdirectory(CoreServiceLocator)
