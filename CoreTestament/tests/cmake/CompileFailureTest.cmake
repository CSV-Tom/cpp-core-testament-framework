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

set(diagnostics "${compiler_output}\n${compiler_error}")
if(NOT DEFINED EXPECTED_DIAGNOSTIC OR EXPECTED_DIAGNOSTIC STREQUAL "")
    message(FATAL_ERROR "EXPECTED_DIAGNOSTIC must identify the intended compile failure")
endif()
if(NOT diagnostics MATCHES "${EXPECTED_DIAGNOSTIC}")
    message(FATAL_ERROR
        "Compilation failed for an unexpected reason; expected '${EXPECTED_DIAGNOSTIC}':\n"
        "${diagnostics}"
    )
endif()
