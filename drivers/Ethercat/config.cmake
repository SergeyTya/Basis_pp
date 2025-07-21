target_include_directories( ${TARGET_NAME}
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/BazisPanelDevice/Src
)

FILE(GLOB_RECURSE ETERCAT_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/BazisPanelDevice/Src/*.c
)

target_sources(${TARGET_NAME} PUBLIC ${ETERCAT_SRC} )
