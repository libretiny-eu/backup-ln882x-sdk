set(LIBC_DIR ${CMAKE_SOURCE_DIR}/components/libc)

FILE(GLOB_RECURSE  LIBC_SRC  ${LIBC_DIR}  "*.c" "*.cpp")

set(PRO_SOURCE  ${PRO_SOURCE}  ${LIBC_SRC})
set(libc_static_target  "libc")

add_library(${libc_static_target} STATIC ${LIBC_SRC})
add_library(components::libc ALIAS ${libc_static_target})

# target_link_libraries(${libc_static_target} PRIVATE --specs=nano.specs --specs=nosys.specs)
# target_link_libraries(${libc_static_target} PRIVATE ${EXTRA_LINK_FLAGS})
