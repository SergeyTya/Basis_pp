cmake_minimum_required(VERSION 3.19)

set(TARGET_NAME RTOS)

project(${TARGET_NAME})

add_library(${TARGET_NAME} STATIC)

set(RTOS_PATH   ${CMAKE_CURRENT_SOURCE_DIR})


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
        PUBLIC  ${RTOS_PATH}/portable/MemMang/heap_4.c
        PUBLIC  ${RTOS_PATH}/CMSIS_RTOS/cmsis_os.c
        )

# set(MCU_TYPE  "Cortex-M4")

# if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")

#     if(${MCU_TYPE} STREQUAL "Cortex-M3" )
#         target_include_directories(${TARGET_NAME} PUBLIC ${RTOS_PATH}/portable/GCC/ARM_CM3)
#         target_sources(${TARGET_NAME} PUBLIC
#                 ${RTOS_PATH}/portable/GCC/ARM_CM3/port.c
#                 )
#     endif()

#     if(${MCU_TYPE} STREQUAL "Cortex-M4" )
#         target_include_directories(${TARGET_NAME} PUBLIC ${RTOS_PATH}/portable/GCC/ARM_CM4F)
#         target_sources(${TARGET_NAME} PUBLIC
#                 ${RTOS_PATH}/portable/GCC/ARM_CM4F/port.c
#                 )
#     endif()

#     target_compile_options(${TARGET_NAME}
#         PUBLIC ${CPU_FLAGS}
#         PUBLIC -Og
#     )

# endif()


target_include_directories(${TARGET_NAME} PUBLIC ${RTOS_PATH}/portable/GCC/ARM_CM4F)
target_sources(${TARGET_NAME} PUBLIC
    ${RTOS_PATH}/portable/GCC/ARM_CM4F/port.c
)

target_compile_options(${TARGET_NAME}
        PUBLIC ${CPU_FLAGS}
        PUBLIC -Og
)


target_include_directories(${TARGET_NAME} PUBLIC "${RTOS_CONFIGH}")
target_include_directories(${TARGET_NAME} PUBLIC "${CMSISCORE}/Include")
target_include_directories(${TARGET_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


install(TARGETS ${TARGET_NAME} DESTINATION .)