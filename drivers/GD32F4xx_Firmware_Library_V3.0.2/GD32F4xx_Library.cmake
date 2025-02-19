message("--- GD32F4xx lib")
target_include_directories(${TARGET_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/GD32F4xx_Firmware_Library_V3.0.2/GD32F4xx_standard_peripheral/Include
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/GD32F4xx_Firmware_Library_V3.0.2/CMSIS/
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/GD32F4xx_Firmware_Library_V3.0.2/CMSIS/Include
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/GD32F4xx_Firmware_Library_V3.0.2/CMSIS/GD/GD32F4xx/Include
        )

FILE(GLOB_RECURSE LIB_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/GD32F4xx_Firmware_Library_V3.0.2/GD32F4xx_standard_peripheral/Source/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/GD32F4xx_Firmware_Library_V3.0.2/CMSIS/GD/GD32F4xx/Source/*.c
        )

target_sources(${TARGET_NAME}  PUBLIC ${LIB_SRC} )
