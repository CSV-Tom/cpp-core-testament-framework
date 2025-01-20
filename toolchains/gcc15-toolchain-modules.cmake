set(CMAKE_C_COMPILER /usr/local/bin/gcc) #/usr/bin/gcc-15)
set(CMAKE_CXX_COMPILER /usr/local/bin/g++) #/usr/bin/g++-15)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23 -fmodules-ts -Wall -Wextra -Wpedantic -O3 -s")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
