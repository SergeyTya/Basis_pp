target_sources(${TARGET_NAME}     
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_spl_driver/src/misc.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_spl_driver/src/stm32f4xx_gpio.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_spl_driver/src/stm32f4xx_rcc.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_spl_driver/src/stm32f4xx_syscfg.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_spl_driver/src/stm32f4xx_tim.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_spl_driver/src/stm32f4xx_usart.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_spl_driver/src/stm32f4xx_adc.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_spl_driver/src/stm32f4xx_fsmc.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_spl_driver/src/stm32f4xx_rtc.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_spl_driver/src/stm32f4xx_exti.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_spl_driver/src/stm32f4xx_pwr.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_spl_driver/src/stm32f4xx_dma.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_spl_driver/src/stm32f4xx_syscfg.c
)

target_include_directories( ${TARGET_NAME}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_spl_driver/inc/
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_spl_driver/CMSIS/Include
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/stm32f4xx_spl_driver/CMSIS/Device/ST/STM32F4xx/Include
)