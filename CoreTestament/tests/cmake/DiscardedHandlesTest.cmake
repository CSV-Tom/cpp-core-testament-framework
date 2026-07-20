if(COMPILER_ID STREQUAL "MSVC")
    string(REPLACE "|" ";" implicit_include_dirs "${IMPLICIT_INCLUDE_DIRS}")
    set(implicit_include_arguments)
    foreach(include_dir IN LISTS implicit_include_dirs)
        list(APPEND implicit_include_arguments "/I${include_dir}")
    endforeach()
    execute_process(
        COMMAND
            "${CXX_COMPILER}"
            /nologo
            /std:c++latest
            /Zs
            /we4834
            "/I${INCLUDE_DIR}"
            "/I${GENERATED_INCLUDE_DIR}"
            ${implicit_include_arguments}
            "${SOURCE_FILE}"
        RESULT_VARIABLE compile_result
        OUTPUT_VARIABLE compiler_output
        ERROR_VARIABLE compiler_error
    )
else()
    execute_process(
        COMMAND
            "${CXX_COMPILER}"
            -std=c++23
            -fsyntax-only
            -Werror=unused-result
            "-I${INCLUDE_DIR}"
            "-I${GENERATED_INCLUDE_DIR}"
            "${SOURCE_FILE}"
        RESULT_VARIABLE compile_result
        OUTPUT_VARIABLE compiler_output
        ERROR_VARIABLE compiler_error
    )
endif()

set(diagnostics "${compiler_output}\n${compiler_error}")

if(compile_result EQUAL 0)
    message(FATAL_ERROR "Discarding handles or runner exit codes compiled without an error")
endif()

if(NOT diagnostics MATCHES "nodiscard|unused-result|C4834")
    message(FATAL_ERROR "Compilation failed for an unexpected reason:\n${diagnostics}")
endif()
