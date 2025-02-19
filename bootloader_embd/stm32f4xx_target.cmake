
SET(MCU_TYPE Cortex-M4 )
SET(MCU_NAME STM32F405xx)
ADD_DEFINITIONS(-DSTM32F405xx)

SET(MCU_FLOAT_TYPE hard)
SET(MCU_FPU_NAME fpv4-sp-d16)


include(${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32F4xx_StdPeriph_Driver/STM32F4xx_StdPeriph_Driver.cmake) #define mcu board

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/mcu/stm32f405xx_mem.cmake) 
