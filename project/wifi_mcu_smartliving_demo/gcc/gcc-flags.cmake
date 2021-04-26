
# generate flags from user variables
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(DBGFLAGS "-ggdb")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    set(DBGFLAGS "-O1 -s")
endif()

set(WARNFLAGS "-Wall -Wextra -Wundef -Wshadow -Wimplicit-function-declaration \
-Wredundant-decls -Wstrict-prototypes -Wmissing-prototypes \
-Wdouble-promotion -Wfloat-conversion -pedantic"
)
set(PREPFLAGS "-MD -MP")
set(SPECSFLAGS "")
set(ASFLAGS ${MCU})
set(ARCHFLAGS ${MCU})
set(OPTFLAGS ${OPTFLAGS})

# compiler: language specific flags
set(CMAKE_C_FLAGS " ${ARCHFLAGS} ${OPTFLAGS} ${DBGFLAGS} ${WARNFLAGS} ${PREPFLAGS} \
-std=gnu99 \
-ffunction-sections -fdata-sections -fno-strict-aliasing \
${SPECSFLAGS}"
    CACHE INTERNAL "C compiler flags")


set(CMAKE_CXX_FLAGS "${ARCHFLAGS} ${OPTFLAGS} ${DBGFLAGS} ${CXXWARNFLAGS} ${PREPFLAGS} \
-std=gnu++11 \
-ffunction-sections -fdata-sections -fno-strict-aliasing \
-fno-rtti -fno-exceptions \
${SPECSFLAGS}"
    CACHE INTERNAL "Cxx compiler flags")
set(CMAKE_ASM_FLAGS "${ASFLAGS} -x assembler-with-cpp ${DBGFLAGS} " CACHE INTERNAL "ASM compiler flags")

set(CMAKE_EXE_LINKER_FLAGS "${LINK_FLAGS}" CACHE INTERNAL "Exe linker flags")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)
