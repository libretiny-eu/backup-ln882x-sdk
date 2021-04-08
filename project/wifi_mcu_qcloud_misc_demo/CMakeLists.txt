set(PRO_DIR  ${CMAKE_CURRENT_SOURCE_DIR})

set(QCLOUD_IOT_SAMPLES_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/qcloud_iot_samples)
set(QCLOUD_IOT_SAMPLES_SRC  ${QCLOUD_IOT_SAMPLES_DIR}/scenarized/light_data_template_sample.c)

set(PRO_SRC
    ${PRO_DIR}/app/main.c
    ${PRO_DIR}/app/ota_mqtt_example.c
    ${PRO_DIR}/app/temp_cal_app.c
    ${PRO_DIR}/app/usr_app.c
    ${PRO_DIR}/app/wifi_setup.c
    ${PRO_DIR}/bsp/serial_hw.c
    ${PRO_DIR}/bsp/drv_adc_measure.c
    ${PRO_DIR}/startup/startup_ln882x_gcc.c
    ${MCU_LN882X_DIR}/driver_ln882x/src/ll/ll_gpio.c
    ${CMAKE_SOURCE_DIR}/components/libc/newlib_stub.c
    ${QCLOUD_IOT_SAMPLES_SRC}
)

link_directories(${CMAKE_SOURCE_DIR}/lib/gcclib)

set(pro_executable_target  ${TARGET_ELF_NAME})

add_executable(${pro_executable_target}  ${PRO_SRC})

set(COMP_ATCMD_SUPPORT    ON  PARENT_SCOPE)
set(COMP_FOTA_SUPPORT     ON  PARENT_SCOPE)
set(COMP_AIRKISS_SUPPORT  ON  PARENT_SCOPE)
set(COMP_PARTMGR_SUPPORT  ON  PARENT_SCOPE)
set(COMP_SMARTCFG_SUPPORT  ON  PARENT_SCOPE)
set(COMP_SOFTAPCFG_SUPPORT  ON  PARENT_SCOPE)
set(COMP_QCLOUD_SDK_SUPPORT  ON  PARENT_SCOPE)

target_link_libraries(${pro_executable_target}
    PUBLIC
    components::qcloud
    components::net::mbedtls
    components::smartlink::airkiss
    components::smartlink::softapcfg
    components::smartlink::smartcfg
    wifi
    components::fota
    components::kernel
    components::fs::kv
    components::utils
    components::atcmd
    components::serial
    driver_ln882x
    -lc -lm -lnosys

    PRIVATE
    -T${LINKER_SCRIPT}
    ${EXTRA_LINK_FLAGS}
)

target_include_directories(${pro_executable_target}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/app
    ${CMAKE_CURRENT_SOURCE_DIR}/bsp
    ${CMAKE_CURRENT_SOURCE_DIR}/cfg
    ${COMP_FOTA_DIR}/ota_agent
    ${COMP_AIRKISS_DIR}
    ${COMP_SMARTCFG_DIR}
    ${COMP_SOFTAPCFG_DIR}
    ${COMP_QCLOUD_SDK_DIR}/include
    ${COMP_QCLOUD_SDK_DIR}/include/exports
    ${MCU_LN882X_DIR}/driver_ln882x/inc/reg
)

include(${CMAKE_CURRENT_SOURCE_DIR}/gcc/gcc-postbuild.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/gcc/gcc-jlink.cmake)
