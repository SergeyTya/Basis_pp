

message("--- GD25Qxx driver for GD32F4xx mcu")

SET(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/drivers/GD25Qxx/GD32)

target_include_directories(${TARGET_NAME}
        PUBLIC ${LIB_PATH}
        PUBLIC ${LIB_PATH}/port/GD32F4xx
        )

FILE(GLOB_RECURSE LIB_SRC
        ${LIB_PATH}/port/GD32F4xx/GD25Q32E_port_GD32F4xx.c
        ${LIB_PATH}/GD25Q32_os.c
        ${LIB_PATH}/GD25Q32_tasks.c
        )

target_sources(${TARGET_NAME}  PUBLIC ${LIB_SRC} )
