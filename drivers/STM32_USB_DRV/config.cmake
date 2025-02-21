target_sources(
    ${TARGET_NAME}     
    
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32_USB_DRV/STM32_USB_Device_Library/Core/src/usbd_core.c
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32_USB_DRV/STM32_USB_Device_Library/Core/src/usbd_ioreq.c
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32_USB_DRV/STM32_USB_Device_Library/Core/src/usbd_req.c

    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32_USB_DRV/STM32_USB_Device_Library/Class/cdc/src/usbd_cdc_core_loopback.c
    #PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32_USB_DRV/STM32_USB_Device_Library/Class/cdc/src/usbd_cdc_core.c
    #PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32_USB_DRV/STM32_USB_Device_Library/Class/cdc/src/usbd_cdc_if_template.c

    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32_USB_DRV/STM32_USB_OTG_Driver/src/usb_core.c
    # PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32_USB_DRV/STM32_USB_OTG_Driver/src/usb_dcd_int.c
     PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32_USB_DRV/STM32_USB_OTG_Driver/src/usb_dcd.c
    # PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32_USB_DRV/STM32_USB_OTG_Driver/src/usb_hcd_int.c
    # PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32_USB_DRV/STM32_USB_OTG_Driver/src/usb_hcd.c
    # PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32_USB_DRV/STM32_USB_OTG_Driver/src/usb_otg.c
    # #PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32_USB_DRV/STM32_USB_OTG_Driver/src/usb_bsp_template.c

)

target_include_directories( 
    ${TARGET_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32_USB_DRV/STM32_USB_Device_Library/Core/inc/
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32_USB_DRV/STM32_USB_Device_Library/Class/cdc/inc/

    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32_USB_DRV/STM32_USB_OTG_Driver/inc/
)