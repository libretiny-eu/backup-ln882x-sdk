# Target-specific flags
set(MCU_ARCH )
set(MCU_FAMILY )
set(MCU_NAME )
# set CFLAGS
set(CPU "-mcpu=cortex-m4")
set(FPU "-mfpu=fpv4-sp-d16")
set(FLOAT_ABI "-mfloat-abi=hard")
set(MCU "-mthumb -mabi=aapcs ${CPU} ${FPU} ${FLOAT_ABI}")
add_definitions(
    -DLN882x
    -DARM_MATH_CM4
)
# set Link script
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/project/${USER_PROJECT}/gcc/ln882x.ld")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${LINKER_SCRIPT}")
option(USE_SYSTEM_VIEW "Enable Segger SystemView library" OFF)
