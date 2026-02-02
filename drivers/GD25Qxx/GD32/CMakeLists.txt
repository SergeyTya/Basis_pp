cmake_minimum_required(VERSION 3.10)

SET( LIB_GD25Qxx_NAME lib_gd25qxx)

project(${LIB_GD25Qxx_NAME} )

add_library(${LIB_GD25Qxx_NAME})

target_include_directories(${LIB_GD25Qxx_NAME} 
        PUBLIC  ${LIB_GD25Qxx_DIR}
)

target_sources(${LIB_GD25Qxx_NAME}  PUBLIC 
        ${LIB_GD25Qxx_DIR}/GD25Q32E_port_GD32H7xx.c
        ${LIB_GD25Qxx_DIR}/GD25Q32_os.c
        ${LIB_GD25Qxx_DIR}/GD25Q32_tasks.c
)


target_link_libraries(${LIB_GD25Qxx_NAME} PUBLIC SPL)
target_link_libraries(${LIB_GD25Qxx_NAME} PUBLIC freertos_kernel)
