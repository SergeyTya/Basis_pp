SET(MCU_TYPE Cortex-M4 )
SET(MCU_NAME STM32F405xx)
SET(MCU_FLOAT_TYPE hard)
SET(MCU_FPU_NAME fpv4-sp-d16)


SET(MCU_DRV_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_spl_driver/config.cmake)

add_definitions(-DSTM32F40XX)
add_definitions(-DSTM32F40_41xxx)
add_definitions(-DARM_MATH_CM4)
add_definitions(-D__FPU_PRESENT)
add_definitions(-DUSE_STDPERIPH_DRIVER)
add_definitions(-DUSE_USB_OTG_FS)
add_definitions(-DUSE_USB_OTG_HS)
add_definitions(-DUSE_EMBEDDED_PHY)