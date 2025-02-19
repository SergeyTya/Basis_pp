message("--- stm32F4xx HAL lib")
target_include_directories(${TARGET_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Inc
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/CMSIS/Include
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/CMSIS/Device/ST/STM32F4xx/Include
        )

FILE(GLOB_RECURSE LIB_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal_adc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal_adc_ex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal_pcd.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal_pcd_ex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_ll_usb.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal_rcc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal_rcc_ex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal_flash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal_flash_ex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal_flash_ramfunc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal_gpio.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal_dma_ex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal_dma.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal_pwr.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal_pwr_ex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal_cortex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal_exti.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal_tim.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/Src/stm32f4xx_hal_tim_ex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/CMSIS/Device/ST/STM32F4xx/Source/Templates/*.c
        )


SET(CMSISCORE ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_hal_driver/CMSIS)
add_definitions(-DCMSISCORE=${CMSISCORE})



target_sources(${TARGET_NAME}  PUBLIC ${LIB_SRC} )


