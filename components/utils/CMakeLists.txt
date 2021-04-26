file(GLOB_RECURSE UTILS_SRC ${COMP_UTILS_DIR} "*.c" "*.cpp")

set(PRO_SOURCE   ${PRO_SOURCE}   ${UTILS_SRC}  PARENT_SCOPE)
set(PRO_INCLUDE  ${PRO_INCLUDE}  ${COMP_UTILS_DIR}  PARENT_SCOPE)

set(utils_static_target "utils")

add_library(${utils_static_target} STATIC ${UTILS_SRC})
add_library(components::utils ALIAS ${utils_static_target})

target_link_libraries(${utils_static_target}
    PUBLIC
    components::phony
)

target_include_directories(${utils_static_target}
    PUBLIC
    ${CMAKE_SOURCE_DIR}/project/${USER_PROJECT}/bsp
    ${CMAKE_SOURCE_DIR}/project/${USER_PROJECT}/cfg
    ${CMAKE_SOURCE_DIR}/components
    ${COMP_UTILS_DIR}
    ${COMP_UTILS_DIR}/..
    ${COMP_UTILS_DIR}/runtime
    ${COMP_UTILS_DIR}/reboot_trace
    ${COMP_UTILS_DIR}/fifo
    ${COMP_UTILS_DIR}/debug
    ${COMP_UTILS_DIR}/debug/CmBacktrace
    ${COMP_UTILS_DIR}/debug/RTT
    ${MCU_LN882X_DIR}/ln88xx
    ${MCU_LN882X_DIR}/CMSIS_5.3.0
    ${MCU_LN882X_DIR}/driver_ln882x/inc
    ${COMP_SERIAL_DIR}
    ${COMP_DHCPD_DIR}
    ${COMP_KV_DIR}/kv
    ${COMP_KV_DIR}/kv_port
    ${COMP_KERNEL_DIR}
    ${COMP_KERNEL_DIR}/FreeRTOS/Source/include
    ${COMP_KERNEL_DIR}/FreeRTOS/Source/portable/GCC/ARM_CM4F
    ${COMP_LWIP_DIR}/include
    ${COMP_LWIP_DIR}/port
)
