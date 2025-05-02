cmake_minimum_required(VERSION 3.10)

SET( LIB_LWIP_NAME lwip)

project(${LIB_LWIP_NAME})

add_library(${LIB_LWIP_NAME})

set( LWIP_DIR ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(${LIB_LWIP_NAME} PUBLIC 
        ${LWIP_DIR}/src/include/
        ${LWIP_DIR}/src/include/lwip
        ${LWIP_DIR}/port/GD32F4xx/
        ${LWIP_DIR}/port/GD32F4xx/FreeRTOS/
        ${LWIP_DIR}/port/GD32F4xx/hw/
        ${LWIP_DIR}/port/GD32F4xx/app/
        
)

include(${LWIP_DIR}/src/Filelists.cmake)

target_sources(${LIB_LWIP_NAME}  PUBLIC 
 ${lwipcore_SRCS} 
 ${lwipcore4_SRCS}
 ${lwipapi_SRCS} 
 ${lwipnetif_SRCS} 
 ${LWIP_DIR}/port/GD32F4xx/FreeRTOS/ethernetif.c
 ${LWIP_DIR}/port/GD32F4xx/FreeRTOS/sys_arch.c
 ${LWIP_DIR}/port/GD32F4xx/hw/gd32f4xx_enet_init.c
 ${LWIP_DIR}/port/GD32F4xx/app/netconf.c
 
)

target_include_directories(${LIB_LWIP_NAME}
          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
          )

# target_compile_options(
#             test_crclib PRIVATE
#             -Wall 
#             -Wextra 
#             -pedantic 
#             -std=c++17
#             #-Werror
# )

#ADD_DEFINITIONS(-DSELECT_DESCRIPTORS_ENHANCED_MODE=1)


