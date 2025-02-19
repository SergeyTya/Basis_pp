    set(TARGET_NAME "bootloader_gd32f")
    add_executable(${TARGET_NAME} $<TARGET_PROPERTY:STARTUP_FILE>)

    SET(MCU_TYPE Cortex-M4 )
    SET(MCU_NAME GD32F405VTGxx)
    SET(MCU_FLOAT_TYPE hard)
    SET(MCU_FPU_NAME fpv4-sp-d16)
    
    ADD_DEFINITIONS(-DGD32F405)
    ADD_DEFINITIONS(-D__SYSTEM_CLOCK_168M_PLL_8M_HXTAL=168000000 )
    ADD_DEFINITIONS(-DHXTAL_VALUE=8000000)
    add_definitions(-DARM_MATH_CM4)
    add_definitions(-D__FPU_PRESENT)

    SET(MCU_DRV_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/drivers/GD32F4xx_Firmware_Library_V3.0.2/GD32F4xx_Library.cmake)
    include(cmake/toolchains.cmake)

    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/mcu/stm32f405xx_mem.cmake) 
    
    include(${MCU_DRV_LIBS}) 
    include(${CMAKE_CURRENT_SOURCE_DIR}/drivers/loader_rs485/config.cmake) 

    target_include_directories(
        ${TARGET_NAME} PUBLIC
            ${PROJECT_BINARY_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}
        )

    FILE(GLOB APP_SRC
            ${CMAKE_CURRENT_SOURCE_DIR}/bootloader_embd/main.c
            ${CMAKE_CURRENT_SOURCE_DIR}/bootloader_embd/uuboot_port.c
    )
 
    target_sources(${TARGET_NAME} PUBLIC ${APP_SRC})




       
    target_compile_definitions(${TARGET_NAME} PUBLIC BUILD_NAME_${TARGET_NAME})

    set_target_properties(${TARGET_NAME} PROPERTIES
    LINKER_FILE    "${CMAKE_CURRENT_SOURCE_DIR}/bootloader_embd/GD32F405VTGxx.ld"
    )


    