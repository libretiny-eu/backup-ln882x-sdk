file(GLOB_RECURSE  PING_SRC  ${COMP_PING_DIR}  "*.c" "*.cpp")

set(PRO_SOURCE   ${PRO_SOURCE}   ${PING_SRC}  PARENT_SCOPE)
set(PRO_INCLUDE  ${PRO_INCLUDE}  ${COMP_PING_DIR}  PARENT_SCOPE)

set(ping_static_target "ping")

add_library(${ping_static_target} STATIC ${PING_SRC})
add_library(components::net::ping ALIAS ${ping_static_target})

target_link_libraries(${ping_static_target} components::net::lwip)
target_include_directories(${ping_static_target}
    PUBLIC
    ${COMP_PING_DIR}
)
