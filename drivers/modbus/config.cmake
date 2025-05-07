
target_include_directories(${TARGET_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/modbus/include
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/modbus/port
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/modbus/rtu
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/modbus/tcp
        )

target_sources(${TARGET_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/modbus/functions/mbfuncdiag.c
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/modbus/functions/mbfuncholding.c
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/modbus/functions/mbfuncother.c
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/modbus/functions/mbutils.c
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/modbus/port/porteventTCP.c
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/modbus/port/porttimer.c
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/modbus/port/porttcp.c
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/modbus/rtu/mbcrc.c
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/modbus/rtu/mbrtu.c
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/modbus/tcp/mbtcp.c
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/modbus/mb.c
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/modbus/mbhtable.c

        )