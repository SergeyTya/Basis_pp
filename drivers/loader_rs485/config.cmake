

message("--- loader_rs485")

SET(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/drivers/loader_rs485/)

target_include_directories(${TARGET_NAME}
        PUBLIC ${LIB_PATH}
        )

FILE(GLOB_RECURSE LIB_SRC
        ${LIB_PATH}/uuboot.c
        )

target_compile_definitions(${TARGET_NAME} PUBLIC -DUUBOOT)

target_sources(${TARGET_NAME}  PUBLIC ${LIB_SRC} )
