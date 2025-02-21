target_include_directories( ${TARGET_NAME}
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/Inc
)


target_sources(${TARGET_NAME}      
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/src/ecataoe.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/src/ecatcoe.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/src/bootmode.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/src/ecateoe.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/src/ecatfoe.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/src/ecatslv.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/src/ecatsoe.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/src/emcy.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/src/eoeappl.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/src/mailbox.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/src/objdef.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/src/sdoserv.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/src/SSC-Device_200k.c 
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/src/coeappl.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/src/ecatappl.c
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/Ethercat/src/aoeappl.c  
)