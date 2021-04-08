# the name of the target operating system
set(CMAKE_SYSTEM_NAME   Generic)
set(CMAKE_SYSTEM_VERSION  1)
set(CMAKE_SYSTEM_PROCESSOR  Arm)

# specify the cross compiler
set(COMPILER_ROOT ${CMAKE_SOURCE_DIR}/tools/gcc_toolchain)

function(SET_ARM_GCC bit compiler_home tar_file tar_option)
    if (${bit} EQUAL 64)
        set(${compiler_home} ${CMAKE_SOURCE_DIR}/tools/gcc_toolchain/arm-none-eabi-gcc PARENT_SCOPE)
        set(${tar_file} ${COMPILER_ROOT}/split-arm-none-eabi-gcc.8.2.1-1.7-20190524-0603.tar.gz.* PARENT_SCOPE)
        set(${tar_option} -xz PARENT_SCOPE)
    elseif(${bit} EQUAL 32)
        set(${compiler_home} ${CMAKE_SOURCE_DIR}/tools/gcc_toolchain/gcc-arm-none-eabi-5_4-2016q3 PARENT_SCOPE)
        set(${tar_file} ${COMPILER_ROOT}/split-arm-none-eabi-gcc__5_4-2016q3-20160926-linux.tar.bz2.* PARENT_SCOPE)
        set(${tar_option} -xj PARENT_SCOPE)
    endif()
endfunction(SET_ARM_GCC bit compiler_home tar_file tar_option)

function(TAR_ARM_GCC)
    if (EXISTS ${COMPILER_HOME})
        message(STATUS "Found ${COMPILER_HOME}")
    else()
        message(STATUS "Not found ${COMPILER_HOME}")
        execute_process(COMMAND  bash "-c" "cat  ${COMPILER_TAR_FILE}  |  tar ${TAR_OPTION}  -C ${COMPILER_ROOT}")
    endif()
endfunction(TAR_ARM_GCC)

# 检查mkimage是否要重新编译
function(CHECK_MKIMAGE SYS_BIT)
    execute_process(COMMAND bash "-c" "file ${CMAKE_SOURCE_DIR}/tools/bin/mkimage | awk -F '[ -]' '{print $4}'"
                    OUTPUT_VARIABLE MKIMAGE_BIT)
    if (NOT ${SYS_BIT} EQUAL ${MKIMAGE_BIT})
        execute_process(COMMAND bash "-c" "cd ${CMAKE_SOURCE_DIR}/tools/src/mkimage && make && cd -")
    endif()
endfunction(CHECK_MKIMAGE SYS_BIT)

if (WIN32)
    MESSAGE(FATAL_ERROR "NOT SUPPORT")
elseif (APPLE)
    MESSAGE(FATAL_ERROR "NOT SUPPORT")
elseif (UNIX)
    execute_process(COMMAND getconf LONG_BIT
                    OUTPUT_VARIABLE SYS_BIT)
    SET_ARM_GCC(${SYS_BIT} COMPILER_HOME COMPILER_TAR_FILE TAR_OPTION)
    TAR_ARM_GCC()
    CHECK_MKIMAGE(${SYS_BIT})
endif ()

set(CROSS_COMPILE_PREFIX ${COMPILER_HOME}/bin/arm-none-eabi-)
set(CMAKE_C_COMPILER ${CROSS_COMPILE_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${CROSS_COMPILE_PREFIX}g++)
set(CMAKE_ASM_COMPILER ${CROSS_COMPILE_PREFIX}gcc)
set(CMAKE_OBJCOPY ${CROSS_COMPILE_PREFIX}objcopy)
set(CMAKE_OBJDUMP ${CROSS_COMPILE_PREFIX}objdump)
set(CMAKE_SIZE ${CROSS_COMPILE_PREFIX}size)

# find additional toolchain executables
find_program(ARM_SIZE_EXECUTABLE ${CROSS_COMPILE_PREFIX}size)
find_program(ARM_GDB_EXECUTABLE ${CROSS_COMPILE_PREFIX}gdb)
find_program(ARM_OBJCOPY_EXECUTABLE ${CROSS_COMPILE_PREFIX}objcopy)
find_program(ARM_OBJDUMP_EXECUTABLE ${CROSS_COMPILE_PREFIX}objdump)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# search for program/library/include in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)





