
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/mcu/stm32f405xx.cmake) #define mcu board

message("-- add ${TARGET_NAME} headers")
target_include_directories(${TARGET_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/cnfg/
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/hal/
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mb/
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mcu/
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/usb/
        )

message("-- add ${TARGET_NAME} sources")
FILE(GLOB_RECURSE LIB_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/cnfg/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/hal/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mb/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mcu/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/usb/*.c
        )

target_sources(${TARGET_NAME}  PUBLIC ${LIB_SRC} )

SET(RTOS_CONFIGH
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}
)