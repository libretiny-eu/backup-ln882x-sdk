file(GLOB_RECURSE  IPERF_SRC  ${COMP_IPERF_DIR}  "*.c" "*.cpp")

set(PRO_SOURCE   ${PRO_SOURCE}   ${IPERF_SRC}  PARENT_SCOPE)
set(PRO_INCLUDE  ${PRO_INCLUDE}  ${COMP_IPERF_DIR}  PARENT_SCOPE)

set(iperf_static_target "iperf")

add_library(${iperf_static_target} STATIC ${IPERF_SRC})
add_library(components::net::iperf ALIAS ${iperf_static_target})

target_link_libraries(${iperf_static_target} components::net::lwip)
target_include_directories(${iperf_static_target}
    PUBLIC
    ${COMP_IPERF_DIR}
)
