file(GLOB_RECURSE  HTTPSCLIENT_SRC  ${COMP_HTTPSCLIENT_DIR}  "*.c" "*.cpp")

set(PRO_SOURCE   ${PRO_SOURCE}   ${HTTPSCLIENT_SRC}  PARENT_SCOPE)
set(PRO_INCLUDE  ${PRO_INCLUDE}  ${COMP_HTTPSCLIENT_DIR}/inc  PARENT_SCOPE)

set(httpsclient_static_target  "httpsclient")

add_library(${httpsclient_static_target}  STATIC  ${HTTPSCLIENT_SRC})
add_library(components::net::httpsclient  ALIAS  ${httpsclient_static_target})

target_link_libraries(${httpsclient_static_target}
    PUBLIC
    components::net::lwip
    components::net::mbedtls
)

target_include_directories(${httpsclient_static_target}
    PUBLIC
    ${COMP_HTTPSCLIENT_DIR}/inc
    ${COMP_MBEDTLS_DIR}/include
)
