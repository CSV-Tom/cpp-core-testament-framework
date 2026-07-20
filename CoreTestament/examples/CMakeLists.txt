option(TESTAMENT_BUILD_EXAMPLES "Build Testament usage examples" ON)

if(TESTAMENT_BUILD_EXAMPLES)
    add_executable(TestamentRunner
        main.cpp
        NormalTestExample.cpp
        FixtureExample.cpp
        ParameterizedTestExample.cpp
    )
    target_link_libraries(TestamentRunner PRIVATE CoreTestament::CoreTestament)
    set_target_properties(TestamentRunner PROPERTIES CXX_EXTENSIONS OFF)
    target_compile_options(TestamentRunner PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
        >
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
    if(BUILD_TESTING)
        add_test(NAME Examples COMMAND TestamentRunner)
        set_tests_properties(Examples PROPERTIES LABELS example)
    endif()
endif()
