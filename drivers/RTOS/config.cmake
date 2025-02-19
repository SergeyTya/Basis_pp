message("--- FreeRTOS")
set(RTOS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/drivers/RTOS)

target_include_directories(${TARGET_NAME}
        PUBLIC ${RTOS_PATH}
        PUBLIC ${RTOS_PATH}/include
        PUBLIC  ${RTOS_PATH}/CMSIS_RTOS
        )

target_sources(${TARGET_NAME}
        PUBLIC  ${RTOS_PATH}/croutine.c
        PUBLIC  ${RTOS_PATH}/event_groups.c
        PUBLIC  ${RTOS_PATH}/list.c
        PUBLIC  ${RTOS_PATH}/queue.c
        PUBLIC  ${RTOS_PATH}/stream_buffer.c
        PUBLIC  ${RTOS_PATH}/tasks.c
        PUBLIC  ${RTOS_PATH}/timers.c
        PUBLIC  ${RTOS_PATH}/portable/MemMang/heap_2.c
        PUBLIC  ${RTOS_PATH}/CMSIS_RTOS/cmsis_os.c
        )

if(${CMAKE_C_COMPILER_ID} STREQUAL "IAR")

    if(${MCU_TYPE} STREQUAL "Cortex-M3" )
        target_include_directories(${TARGET_NAME} PUBLIC ${RTOS_PATH}/portable/IAR/ARM_CM3)
        target_sources(${TARGET_NAME} PUBLIC
            ${RTOS_PATH}/portable/IAR/ARM_CM3/port.c
            ${RTOS_PATH}/portable/IAR/ARM_CM3/portasm.s)
    endif()

    if(${MCU_TYPE} STREQUAL "Cortex-M4" )
        target_include_directories(${TARGET_NAME} PUBLIC ${RTOS_PATH}/portable/IAR/ARM_CM4F)
        target_sources(${TARGET_NAME} PUBLIC
                ${RTOS_PATH}/portable/IAR/ARM_CM4F/port.c
                ${RTOS_PATH}/portable/IAR/ARM_CM4F/portasm.s)
    endif()

endif()

if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")

    if(${MCU_TYPE} STREQUAL "Cortex-M3" )
        target_include_directories(${TARGET_NAME} PUBLIC ${RTOS_PATH}/portable/GCC/ARM_CM3)
        target_sources(${TARGET_NAME} PUBLIC
                ${RTOS_PATH}/portable/GCC/ARM_CM3/port.c
                )
    endif()

    if(${MCU_TYPE} STREQUAL "Cortex-M4" )
        target_include_directories(${TARGET_NAME} PUBLIC ${RTOS_PATH}/portable/GCC/ARM_CM4F)
        target_sources(${TARGET_NAME} PUBLIC
                ${RTOS_PATH}/portable/GCC/ARM_CM4F/port.c
                )
    endif()

endif()

