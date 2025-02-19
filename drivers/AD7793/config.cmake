target_include_directories(${TARGET_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/AD7793
        )

target_sources(${TARGET_NAME}  
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/AD7793/AD7793.c 
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/AD7793/AD7793_os.c 
)