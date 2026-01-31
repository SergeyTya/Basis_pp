cmake_minimum_required(VERSION 3.19)

project("BASIS AERO CONTROL PANEL")

include(cmake/utils.cmake)
include(cmake/target.cmake)

enable_language(C ASM)
SET(CMAKE_EXECUTABLE_SUFFIX ".elf")
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message("-- Compiler ID ${CMAKE_C_COMPILER_ID}") 

#application source PATH definition 
SET(APP_PATH ${CMAKE_CURRENT_SOURCE_DIR}/app) 
    
 
include_directories( 
       ${CMAKE_CURRENT_SOURCE_DIR}/http
        ${APP_PATH}
        ${CMSIS_CORE_INC_PATH} 
        ${DEVICE_INC_PATH}
) 


SET(RTOS ${CMAKE_CURRENT_SOURCE_DIR}/drivers/RTOS/)


#create_device("test_panel") 
#create_device("bptest")
create_device("cntrl_panel_v1_1_basis")
 


