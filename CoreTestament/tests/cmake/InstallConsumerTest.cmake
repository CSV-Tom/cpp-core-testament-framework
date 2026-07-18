if(NOT DEFINED TEST_ROOT OR TEST_ROOT STREQUAL "")
    message(FATAL_ERROR "TEST_ROOT must be set")
endif()
if(NOT DEFINED TEST_BINARY_DIR OR TEST_BINARY_DIR STREQUAL "")
    message(FATAL_ERROR "TEST_BINARY_DIR must be set")
endif()

cmake_path(ABSOLUTE_PATH TEST_ROOT NORMALIZE OUTPUT_VARIABLE normalized_test_root)
cmake_path(ABSOLUTE_PATH TEST_BINARY_DIR NORMALIZE OUTPUT_VARIABLE normalized_binary_dir)
cmake_path(
    IS_PREFIX normalized_binary_dir "${normalized_test_root}"
    NORMALIZE test_root_is_in_binary_dir
)
get_filename_component(test_root_name "${normalized_test_root}" NAME)

if(NOT test_root_is_in_binary_dir
   OR normalized_test_root STREQUAL normalized_binary_dir
   OR NOT test_root_name STREQUAL "install-consumer-test")
    message(FATAL_ERROR "Refusing to remove unsafe TEST_ROOT: ${normalized_test_root}")
endif()

set(TEST_ROOT "${normalized_test_root}")
file(REMOVE_RECURSE "${TEST_ROOT}/install" "${TEST_ROOT}/build")

set(install_dir "${TEST_ROOT}/install")
set(consumer_build_dir "${TEST_ROOT}/build")

set(install_command
    "${CMAKE_COMMAND}" --install "${TEST_BINARY_DIR}" --prefix "${install_dir}"
)
if(TEST_CONFIG)
    list(APPEND install_command --config "${TEST_CONFIG}")
endif()

execute_process(
    COMMAND ${install_command}
    RESULT_VARIABLE install_result
    OUTPUT_VARIABLE install_output
    ERROR_VARIABLE install_error
)
if(NOT install_result EQUAL 0)
    message(FATAL_ERROR "CoreTestament installation failed:\n${install_output}${install_error}")
endif()

set(configure_command
    "${CMAKE_COMMAND}"
    -S "${TEST_CONSUMER_SOURCE_DIR}"
    -B "${consumer_build_dir}"
    -G "${TEST_GENERATOR}"
    "-DCMAKE_PREFIX_PATH=${install_dir}"
)
if(TEST_GENERATOR_PLATFORM)
    list(APPEND configure_command -A "${TEST_GENERATOR_PLATFORM}")
endif()
if(TEST_GENERATOR_TOOLSET)
    list(APPEND configure_command -T "${TEST_GENERATOR_TOOLSET}")
endif()
if(TEST_TOOLCHAIN_FILE)
    list(APPEND configure_command "-DCMAKE_TOOLCHAIN_FILE=${TEST_TOOLCHAIN_FILE}")
endif()
if(TEST_CXX_FLAGS)
    list(APPEND configure_command "-DCMAKE_CXX_FLAGS=${TEST_CXX_FLAGS}")
endif()
if(TEST_EXE_LINKER_FLAGS)
    list(APPEND configure_command "-DCMAKE_EXE_LINKER_FLAGS=${TEST_EXE_LINKER_FLAGS}")
endif()
if(TEST_SHARED_LINKER_FLAGS)
    list(APPEND configure_command "-DCMAKE_SHARED_LINKER_FLAGS=${TEST_SHARED_LINKER_FLAGS}")
endif()

execute_process(
    COMMAND ${configure_command}
    RESULT_VARIABLE configure_result
    OUTPUT_VARIABLE configure_output
    ERROR_VARIABLE configure_error
)
if(NOT configure_result EQUAL 0)
    message(FATAL_ERROR "CoreTestament consumer configuration failed:\n${configure_output}${configure_error}")
endif()

set(build_command "${CMAKE_COMMAND}" --build "${consumer_build_dir}")
if(TEST_CONFIG)
    list(APPEND build_command --config "${TEST_CONFIG}")
endif()

execute_process(
    COMMAND ${build_command}
    RESULT_VARIABLE build_result
    OUTPUT_VARIABLE build_output
    ERROR_VARIABLE build_error
)
if(NOT build_result EQUAL 0)
    message(FATAL_ERROR "CoreTestament consumer build failed:\n${build_output}${build_error}")
endif()

set(test_command "${TEST_CTEST_COMMAND}" --test-dir "${consumer_build_dir}" --output-on-failure)
if(TEST_CONFIG)
    list(APPEND test_command -C "${TEST_CONFIG}")
endif()

execute_process(COMMAND ${test_command} RESULT_VARIABLE test_result)
if(NOT test_result EQUAL 0)
    message(FATAL_ERROR "CoreTestament consumer execution failed")
endif()
