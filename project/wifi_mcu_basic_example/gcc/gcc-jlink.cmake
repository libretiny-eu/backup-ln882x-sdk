# name of targets
set(BIN_TARGET ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.bin)
# flash

add_custom_target(jlink_flash
    DEPENDS ${BIN_TARGET}
    COMMAND ${CMAKE_SOURCE_DIR}/tools/JFlash/JFlash.exe -openprj${CMAKE_SOURCE_DIR}/tools/JFlash/LN881x.jflash -open${FLASH_TARGET},0 -auto -exit
)
add_custom_target(jlink_gdbserver
    DEPENDS ${BIN_TARGET}
    COMMAND ${CMAKE_SOURCE_DIR}/tools/JFlash/JLinkGDBServerCL.exe -select USB -device Cortex-M4 -endian little -if SWD -speed 4000 -noir -noLocalhostOnly
)
