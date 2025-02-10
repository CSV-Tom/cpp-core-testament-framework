
# Set Clang as the compiler
set(CMAKE_C_COMPILER "/usr/bin/clang-19" CACHE STRING "C compiler" FORCE)  #set(CMAKE_C_COMPILER "clang" CACHE STRING "C compiler" FORCE)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++-19" CACHE STRING "C++ compiler" FORCE) #set(CMAKE_CXX_COMPILER "clang++" CACHE STRING "C++ compiler" FORCE)

# Specify C++ standard settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type if none is specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

# Global compiler warnings and optimizations
add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Werror)

# Debug settings
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Og -g")
set(CMAKE_C_FLAGS_DEBUG_INIT "-Og -g")

# Release settings
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")

# RelWithDebInfo settings (optimized but includes debug symbols)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "-O2 -g -DNDEBUG")

# MinSizeRel settings (optimized for smallest binary size)
set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_MINSIZEREL_INIT "-Os -DNDEBUG")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS_RELEASE_INIT "-flto -s")
set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL_INIT "-flto -s")

# Additional optimizations for Clang
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wno-unused-parameter -Wno-missing-field-initializers)
endif()


# Code Coverage settings (only for Debug or Coverage build)
if(CMAKE_BUILD_TYPE STREQUAL "Coverage" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate -fcoverage-mapping)
endif()