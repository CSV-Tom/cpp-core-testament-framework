set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

find_program(
    CORE_TESTAMENT_ARM_GXX
    NAMES arm-linux-gnueabihf-g++
    REQUIRED
)

set(
    CMAKE_CXX_COMPILER
    "${CORE_TESTAMENT_ARM_GXX}"
    CACHE FILEPATH "ARM Linux C++ compiler" FORCE
)

set(
    CORE_TESTAMENT_ARM_SYSROOT
    "/usr/arm-linux-gnueabihf"
    CACHE PATH "ARM Linux runtime sysroot used by QEMU"
)

find_program(CORE_TESTAMENT_QEMU_ARM NAMES qemu-arm)
if(CORE_TESTAMENT_QEMU_ARM)
    set(
        CMAKE_CROSSCOMPILING_EMULATOR
        "${CORE_TESTAMENT_QEMU_ARM};-L;${CORE_TESTAMENT_ARM_SYSROOT}"
        CACHE STRING "Emulator for ARM Linux test executables" FORCE
    )
endif()
