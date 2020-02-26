cmake_minimum_required (VERSION 3.5)

project(csp VERSION 1.0.1 LANGUAGES C)

set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
message("${PROJECT_SOURCE_DIR}")

include(GNUInstallDirs)
set(ARCH, posix)

get_filename_component(LIBCSP_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)

set (LIBCSP_SRC_LIST
    ${LIBCSP_PATH}/src/csp_bridge.c
    ${LIBCSP_PATH}/src/csp_buffer.c
    ${LIBCSP_PATH}/src/csp_conn.c
    #${LIBCSP_PATH}/src/csp_crc32.c
    #${LIBCSP_PATH}/src/csp_debug.c
    #${LIBCSP_PATH}/src/csp_dedup.c
    ${LIBCSP_PATH}/src/csp_endian.c
    ${LIBCSP_PATH}/src/csp_iflist.c
    ${LIBCSP_PATH}/src/csp_io.c
    ${LIBCSP_PATH}/src/csp_port.c
    #${LIBCSP_PATH}/src/csp_promisc.c
    ${LIBCSP_PATH}/src/csp_qfifo.c
    ${LIBCSP_PATH}/src/csp_route.c
    #${LIBCSP_PATH}/src/csp_service_handler.c
    #${LIBCSP_PATH}/src/csp_sfp.c
    #${LIBCSP_PATH}/src/csp_services.c
    )

set (LIBCSP_SRC_LIST 
    ${LIBCSP_SRC_LIST}
    ${LIBCSP_PATH}/src/interfaces/csp_if_kiss.c
    ${LIBCSP_PATH}/src/interfaces/csp_if_lo.c
    ${LIBCSP_PATH}/src/interfaces/csp_if_can.c
    ${LIBCSP_PATH}/src/transport/csp_udp.c
    ${LIBCSP_PATH}/src/rtable/csp_rtable_static.c
    )

if(ARCH STREQUAL posix)
    file(GLOB LIBCSP_ARCH_SRC_LIST "${LIBCSP_PATH}/src/arch/posix/*.c")
    list(APPEND LIBCSP_SRC_LIST ${LIBCSP_ARCH_SRC_LIST})
    list(APPEND LIBCSP_SRC_LIST
        ${LIBCSP_PATH}/src/drivers/can/can_socketcan.c
        ${LIBCSP_PATH}/src/csp_debug.c
    )
endif()

list(SORT LIBCSP_SRC_LIST)

include_directories("${LIBCSP_PATH}/include")
add_library(${PROJECT_NAME} SHARED ${LIBCSP_SRC_LIST})
