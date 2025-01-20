enable_testing()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(TestRunner)

target_sources(TestRunner
    PRIVATE
        main.cpp
)


target_compile_options(TestRunner PRIVATE
    -Wall
  #  -fmodules-ts
)

target_link_libraries(TestRunner PRIVATE CoreTestamentFramework)

add_test(NAME AllTests COMMAND TestRunner)


