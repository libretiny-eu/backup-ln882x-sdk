# the name of the target operating system (cross-compling)
set(CMAKE_SYSTEM_NAME       Generic)
set(CMAKE_SYSTEM_VERSION    1)
set(CMAKE_SYSTEM_PROCESSOR  Arm)

# 检查mkimage是否要重新编译
function(CHECK_REBUILD_MKIMAGE  SYS_BIT)
    execute_process(COMMAND  bash  "-c"  "file ${CMAKE_SOURCE_DIR}/tools/bin/mkimage | awk -F '[ -]' '{print $4}'"
                    OUTPUT_VARIABLE MKIMAGE_BIT)
    if (NOT  ${SYS_BIT}  EQUAL  ${MKIMAGE_BIT})
        execute_process(COMMAND  bash  "-c"  "cd ${CMAKE_SOURCE_DIR}/tools/src/mkimage && make && cd -")
    endif()
endfunction(CHECK_REBUILD_MKIMAGE SYS_BIT)

# ==========  check && config cross-compiling environment  ==========
set(TOOL_SUFFIX     "")
if (WIN32)
    if($ENV{CROSS_TOOLCHAIN_ROOT}  STREQUAL "")
        message(FATAL_ERROR  "CROSS_TOOLCHAIN_ROOT must be set!!!")
    endif($ENV{CROSS_TOOLCHAIN_ROOT}  STREQUAL "")

    set(COMPILER_HOME  $ENV{CROSS_TOOLCHAIN_ROOT}  CACHE  PATH  "cross-compiler home"  FORCE)
    set(TOOL_SUFFIX     ".exe")

elseif (APPLE)
    MESSAGE(FATAL_ERROR  "NOT SUPPORT")
elseif (UNIX)
    execute_process(COMMAND  getconf  LONG_BIT
                    OUTPUT_VARIABLE  SYS_BIT
    )

    if($ENV{CROSS_TOOLCHAIN_ROOT}  STREQUAL "")
        message(FATAL_ERROR  "CROSS_TOOLCHAIN_ROOT must be set!!!")
    endif($ENV{CROSS_TOOLCHAIN_ROOT}  STREQUAL "")

    set(COMPILER_HOME  $ENV{CROSS_TOOLCHAIN_ROOT}  CACHE  PATH  "cross-compiler home"  FORCE)
    set(TOOL_SUFFIX     "")

    CHECK_REBUILD_MKIMAGE(${SYS_BIT})
endif ()

# set compiler prefix.
set(CROSS_COMPILE_PREFIX  ${COMPILER_HOME}/bin/arm-none-eabi-)

# set compiler tool path.
set(CMAKE_C_COMPILER    ${CROSS_COMPILE_PREFIX}gcc${TOOL_SUFFIX}        CACHE FILEPATH  "C compiler"    FORCE)
set(CMAKE_CXX_COMPILER  ${CROSS_COMPILE_PREFIX}g++${TOOL_SUFFIX}        CACHE FILEPATH  "CXX compiler"  FORCE)
set(CMAKE_ASM_COMPILER  ${CROSS_COMPILE_PREFIX}gcc${TOOL_SUFFIX}        CACHE FILEPATH  "ASM compiler"  FORCE)
set(CMAKE_OBJCOPY       ${CROSS_COMPILE_PREFIX}objcopy${TOOL_SUFFIX}    CACHE FILEPATH  "objcopy tool"  FORCE)
set(CMAKE_OBJDUMP       ${CROSS_COMPILE_PREFIX}objdump${TOOL_SUFFIX}    CACHE FILEPATH  "objdump tool"  FORCE)
set(CMAKE_SIZE          ${CROSS_COMPILE_PREFIX}size${TOOL_SUFFIX}       CACHE FILEPATH  "size tool"     FORCE)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# search for program/library/include in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
