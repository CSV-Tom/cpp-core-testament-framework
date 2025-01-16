cmake_minimum_required(VERSION 3.31)

project(CoreTestamentFramework LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(CoreTestamentFramework STATIC)

target_include_directories(CoreTestamentFramework PUBLIC include)

target_sources(CoreTestamentFramework
    PRIVATE
        src/Asserts.cpp
	#src/EventDispatcher.cpp
	#src/ExecutionTimer.cpp
	#src/TestCase.cpp
	#src/TestEventHandler.cpp
	#src/TestRegistry.cpp
	#src/TestStatistics.cpp
	#src/TestStatus.cpp
	#        src/TestSuite.cpp
)

target_compile_options(CoreTestamentFramework PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow
)

#add_subdirectory(tests)
