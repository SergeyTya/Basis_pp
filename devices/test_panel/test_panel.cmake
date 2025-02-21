
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/mcu/stm32f407spl.cmake) #define mcu board

message("-- add ${TARGET_NAME} headers")
target_include_directories(${TARGET_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/cnfg/
        #PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mb/
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mcu/
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/ethercat/
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/usb/
        )

message("-- add ${TARGET_NAME} sources")
FILE(GLOB_RECURSE LIB_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/system_stm32f4xx.c
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/cnfg/*.c
        #${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mb/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mcu/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/usb/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/ethercat/*.c
        )

target_sources(${TARGET_NAME}  PUBLIC ${LIB_SRC} )

SET(RTOS_CONFIGH
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}
)

ADD_DEFINITIONS(-DUSE_STM324xG_EVAL)
