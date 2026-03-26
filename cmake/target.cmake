
function(create_device target_name )

    set(TARGET_NAME ${target_name})
    add_executable(${TARGET_NAME} $<TARGET_PROPERTY:STARTUP_FILE>)

    include(${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}/${TARGET_NAME}.cmake) #define device description path
    include(cmake/toolchains.cmake)

    target_include_directories(
        ${TARGET_NAME} PUBLIC
            ${PROJECT_BINARY_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}
            ${CMAKE_CURRENT_SOURCE_DIR}/../fifo_buffer
        )

    

    message("-- add target sources")
    FILE(GLOB APP_SRC
            ${APP_PATH}/main.c
            ${APP_PATH}/task_panel.c
            ${APP_PATH}/configMenu.c
            ${APP_PATH}/panelConfig.c
            ${APP_PATH}/advancedMenu.c
            ${APP_PATH}/task_master.c
            ${APP_PATH}/masterTransport.c
            ${APP_PATH}/task_modbus.c
            ${APP_PATH}/task_keyboard.c
            ${APP_PATH}/task_scope.c
            ${APP_PATH}/task_ethercat.c
            ${APP_PATH}/mbsupport.c
            ${APP_PATH}/http_server.c
            ${APP_PATH}/http_data.c
            ${APP_PATH}/devices/${TARGET_NAME}/*.c
            ${APP_PATH}/IDisplay.c
            ${APP_PATH}/task_tempmes.c
            ${APP_PATH}/meter.c
            ${APP_PATH}/meter_chint.c
            ${APP_PATH}/task_loggerQ.c
            ${APP_PATH}/clock.c
            ${APP_PATH}/clock_oven.c
            ${APP_PATH}/../fifo_buffer/fifo_buffer.c
            ${APP_PATH}/../fifo_buffer/sma.c
            ${APP_PATH}/crc16.c
            ${APP_PATH}/task_monitor.c

         #   ${CMAKE_CURRENT_SOURCE_DIR}/http/http_server.c
         #   ${CMAKE_CURRENT_SOURCE_DIR}/http/http_data.c
            )
 
 
    target_sources(${TARGET_NAME} PUBLIC ${APP_SRC})

    message("-- add target libraries")
    include(${MCU_DRV_LIBS}) # mcu library files

    # message("-- include ARM-DSP")
    add_definitions(-DARM_MATH_CM4)
    add_definitions(-D__FPU_PRESENT)

   
    #message("-- include modbus")
    include(${CMAKE_CURRENT_SOURCE_DIR}/drivers/modbus/config.cmake) 

    #message("-- include rtos ${RTOS}")
    include(${CMAKE_CURRENT_SOURCE_DIR}/drivers/RTOS/config.cmake)

    #message("-- include rtos ${RTOS}")
    include(${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/config.cmake)
    
    target_compile_definitions(${TARGET_NAME} PUBLIC BUILD_NAME_${TARGET_NAME})

    target_sources(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/syscalls/syscalls.c)

    #ADD version check script 
    add_custom_target(
            VATG_${TARGET_NAME} ALL
            COMMAND python3 ../scripts/gitstatus.py  ${CMAKE_CURRENT_SOURCE_DIR}/devices/${TARGET_NAME}
            COMMENT "check git version ${TARGET_NAME}" 
    )
    add_dependencies(${TARGET_NAME} VATG_${TARGET_NAME})

    
endfunction()