cmake_minimum_required(VERSION 3.23)

project(TestFramework LANGUAGES CXX)

include(CTest)

add_subdirectory(CoreTestament)
add_subdirectory(CoreServiceLocator)
