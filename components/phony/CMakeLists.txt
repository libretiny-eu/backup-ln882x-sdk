set(PHONY_SRC
    ${MCU_LN882X_DIR}/driver_ln882x/src/ll/ll_gpio.c  # exclude from libdriver_ln882x.a
    ${COMP_WIFI_DIR}/wifi_manager/wifi_manager.c
    ${COMP_WIFI_DIR}/wifi_port/wifi_port.c
)

set(phony_static_target   "phony")
add_library(${phony_static_target}  STATIC  ${PHONY_SRC})
add_library(components::phony  ALIAS  ${phony_static_target})

target_include_directories(${phony_static_target}
    PUBLIC
    ${CMAKE_SOURCE_DIR}/project/${USER_PROJECT}/bsp
    ${CMAKE_SOURCE_DIR}/project/${USER_PROJECT}/cfg
    ${MCU_LN882X_DIR}/ln88xx
    ${MCU_LN882X_DIR}/CMSIS_5.3.0
    ${MCU_LN882X_DIR}/driver_ln882x/inc
    ${MCU_LN882X_DIR}/driver_ln882x/inc/reg
    ${CMAKE_SOURCE_DIR}/components
    ${COMP_KERNEL_DIR}
    ${COMP_KERNEL_DIR}/FreeRTOS/Source/include
    ${COMP_KERNEL_DIR}/FreeRTOS/Source/portable/GCC/ARM_CM4F
    ${COMP_LWIP_DIR}/include
    ${COMP_LWIP_DIR}/port
    ${COMP_LWIP_DIR}/port/arch
    ${COMP_DHCPD_DIR}
    ${COMP_KV_DIR}/kv
    ${COMP_KV_DIR}/kv_port
    ${COMP_ATCMD_DIR}
)
