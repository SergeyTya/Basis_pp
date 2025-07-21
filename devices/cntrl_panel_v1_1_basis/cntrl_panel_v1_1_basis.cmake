
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/mcu/gd32f470zgt.cmake) #define mcu board

message("-- add ${TARGET_NAME} headers")
target_include_directories(${TARGET_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mb/
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mcu/
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/spi/
        
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/MUTABA/
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/ethercat/
        )

message("-- add ${TARGET_NAME} sources")
FILE(GLOB_RECURSE TARGET_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mb/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mcu/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/spi/*.c
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/ethercat/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/MUTABA/M204D08AA.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/MUTABA/M204D08AA_port_gd32.c

        )

target_sources(${TARGET_NAME}  PUBLIC ${TARGET_SRC} )
 
message("-- add ${TARGET_NAME} hardware")
include(${CMAKE_CURRENT_SOURCE_DIR}/drivers/GD25Qxx/GD32/config.cmake)  


SET(RTOS_CONFIGH  
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}
)

message("-- include lwip")

set( LWIP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/drivers/lwip-2.2.0 )

target_include_directories(${TARGET_NAME} PUBLIC 
        ${LWIP_DIR}/src/include/
        ${LWIP_DIR}/src/include/lwip
        ${LWIP_DIR}/port/GD32F4xx/
       # ${LWIP_DIR}/port/GD32F4xx/FreeRTOS/
        ${LWIP_DIR}/port/GD32F4xx/Basic/
        ${LWIP_DIR}/port/GD32F4xx/hw/
        ${LWIP_DIR}/port/GD32F4xx/app/
        
)

include(${LWIP_DIR}/src/Filelists.cmake)

target_sources(${TARGET_NAME}  PUBLIC 
 ${lwipcore_SRCS} 
 ${lwipcore4_SRCS}
 ${lwipapi_SRCS} 
 ${lwipnetif_SRCS} 
 #${LWIP_DIR}/port/GD32F4xx/FreeRTOS/ethernetif.c
 #${LWIP_DIR}/port/GD32F4xx/FreeRTOS/sys_arch.c
 ${LWIP_DIR}/port/GD32F4xx/Basic/ethernetif.c
 ${LWIP_DIR}/port/GD32F4xx/hw/gd32f4xx_enet_init.c
 ${LWIP_DIR}/port/GD32F4xx/app/netconf.c
 
)

#ADD_DEFINITIONS(-D__SYSTEM_CLOCK_240M_PLL_8M_HXTAL=240000000 )
ADD_DEFINITIONS(-D__SYSTEM_CLOCK_168M_PLL_8M_HXTAL=168000000 )
ADD_DEFINITIONS(-DHXTAL_VALUE=8000000)
