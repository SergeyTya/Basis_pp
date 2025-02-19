cmake_minimum_required(VERSION 3.19)
project(modbus)


add_library( modbus STATIC)

target_include_directories(modbus
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/port
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/rtu
        )

FILE(GLOB_RECURSE LIB_SRC  ${CMAKE_CURRENT_SOURCE_DIR}/functions/*.c)

target_sources(modbus
        PUBLIC ${LIB_SRC}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/port/portevent.c
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/port/porttimer.c
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/rtu/mbcrc.c
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/rtu/mbrtu.c
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mb.c
        )

if (DEFINED MODBUS_PORTSERH)
    target_include_directories(modbus PUBLIC "${MODBUS_PORTSERH}")
else()
    message("MODBUS_PORTSERH not defined")
endif()



install(TARGETS modbus DESTINATION .)
