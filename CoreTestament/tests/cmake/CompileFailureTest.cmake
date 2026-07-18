if(COMPILER_ID STREQUAL "MSVC")
    execute_process(
        COMMAND "${CXX_COMPILER}" /nologo /std:c++latest /Zs "/I${INCLUDE_DIR}" "${SOURCE_FILE}"
        RESULT_VARIABLE compile_result
        OUTPUT_VARIABLE compiler_output
        ERROR_VARIABLE compiler_error
    )
else()
    execute_process(
        COMMAND "${CXX_COMPILER}" -std=c++23 -fsyntax-only "-I${INCLUDE_DIR}" "${SOURCE_FILE}"
        RESULT_VARIABLE compile_result
        OUTPUT_VARIABLE compiler_output
        ERROR_VARIABLE compiler_error
    )
endif()

if(compile_result EQUAL 0)
    message(FATAL_ERROR "Invalid DSL definition compiled successfully: ${SOURCE_FILE}")
endif()
