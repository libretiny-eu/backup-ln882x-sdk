file(GLOB_RECURSE  FOTA_SRC  ${COMP_FOTA_DIR}  "*.c" "*.cpp")

set(PRO_SOURCE   ${PRO_SOURCE}   ${FOTA_SRC}  PARENT_SCOPE)
set(PRO_INCLUDE  ${PRO_INCLUDE}  ${COMP_FOTA_DIR}/ota_agent  PARENT_SCOPE)

set(COMP_PARTMGR_SUPPORT  ON  PARENT_SCOPE)

set(fota_static_target "fota")
add_library(${fota_static_target} STATIC ${FOTA_SRC})
add_library(components::fota ALIAS ${fota_static_target})

target_link_libraries(${fota_static_target}
    components::fs::partition_mgr
    components::fs::kv
)

target_include_directories(${fota_static_target}
    PUBLIC
    ${COMP_FOTA_DIR}/ota_agent
)
