
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/mcu/gd32f470zgt.cmake) #define mcu board

message("-- add ${TARGET_NAME} headers")
target_include_directories(${TARGET_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mb/
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mcu/
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/MUTABA/
        )

message("-- add ${TARGET_NAME} sources")
FILE(GLOB_RECURSE TARGET_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mb/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mcu/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/MUTABA/M204D08AA.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/MUTABA/M204D08AA_port_gd32.c
        )

target_sources(${TARGET_NAME}  PUBLIC ${TARGET_SRC} )
 
message("-- add ${TARGET_NAME} hardware")
include(${CMAKE_CURRENT_SOURCE_DIR}/drivers/GD25Qxx/GD32/config.cmake)  


SET(RTOS_CONFIGH  
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}
)

ADD_DEFINITIONS(-D__SYSTEM_CLOCK_168M_PLL_8M_HXTAL=168000000 )
ADD_DEFINITIONS(-DHXTAL_VALUE=8000000)