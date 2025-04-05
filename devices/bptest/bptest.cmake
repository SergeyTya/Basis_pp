
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/mcu/stm32f401spl.cmake) #define mcu board

message("-- add ${TARGET_NAME} headers")
target_include_directories(${TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/cnfg/
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mb/
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mcu/
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/usb/
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/MUTABA/
        )

message("-- add ${TARGET_NAME} sources")
FILE(GLOB_RECURSE LIB_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/system_stm32f4xx.c
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/stm32fxxx_it.c
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/cnfg/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mb/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/mcu/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/usb/*.c


        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/MUTABA/*.c

        )

target_sources(${TARGET_NAME}  PUBLIC ${LIB_SRC} )

SET(RTOS_CONFIGH
        ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}
)


#message("-- include rtos ${RTOS}")
include(${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32_USB_DRV/config.cmake)

ADD_DEFINITIONS(-DUSE_STM324xG_EVAL)
