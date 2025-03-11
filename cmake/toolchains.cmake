
if(${CMAKE_C_COMPILER_ID} STREQUAL "IAR")

    set_target_properties(${TARGET_NAME} PROPERTIES
            DEVICE        ${MCU_NAME}
            LINKER_FILE   "${CMAKE_CURRENT_SOURCE_DIR}/startup/IAR/${MCU_NAME}_FLASH.icf"
            STARTUP_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/startup/IAR/startup_${MCU_NAME}.s"
            ARCHITECTURE  ${CMAKE_SYSTEM_PROCESSOR}
            CPU           ${MCU_TYPE}
           # FPU           ${FPU_TYPE}
            )

   # ADD_DEFINITIONS(${CPU_FLAGS})

    set(CPU_FLAGS  --cpu_mode thumb --endian little --cpu ${MCU_TYPE} --fpu VFPv4_sp)

    set(CMAKE_C_FLAGS "-Ohz")

    # Set the compiler flags for the "myProgram" target
    target_compile_options(${TARGET_NAME}
            PRIVATE ${CPU_FLAGS}
            PRIVATE $<$<COMPILE_LANGUAGE:C>: -Ol --dlib_config normal>
            )

    # Set the linker flags for the target

    target_link_options(${TARGET_NAME} PUBLIC
            # Genex evaluates if we are using the `Debug` configuration
            $<$<CONFIG:Debug>: --no_wrap_diagnostics --semihosting --redirect ___write=___write_buffered>
            # Create a map file from the target's ELF
            --map ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.map
            # Set the linker script
            --config $<TARGET_PROPERTY:LINKER_FILE>
            )

    # Set the custom target FORMAT property
    # to select the desired output format (ihex|srec|bin)
#    Generate hex file
    message("-- generate hex file")
    
    string(TIMESTAMP _output "%Y%m%d_%H%M%S")

    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_IAR_ELFTOOL}
            --silent
            # Genex evaluates FORMAT property to select the output format
            --ihex
            # Uses the target file as input
            $<TARGET_FILE:${TARGET_NAME}>
            # Name the output based on the selected format
            "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.hex"
            )

    #    Generate bin file
    message("-- generate bin file")
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_IAR_ELFTOOL}
            --silent
            # Genex evaluates FORMAT property to select the output format
            --bin
            # Uses the target file as input
            $<TARGET_FILE:${TARGET_NAME}>
            # Name the output based on the selected format
            "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.bin"
            )

           

    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND  size -B "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.elf" #need size tools
            )




endif()


if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")

    set_target_properties(${TARGET_NAME} PROPERTIES
            LINKER_FILE    "${CMAKE_CURRENT_SOURCE_DIR}/startup/GNU/${MCU_NAME}.ld"
            STARTUP_FILE   "${CMAKE_CURRENT_SOURCE_DIR}/startup/GNU/startup_${MCU_NAME}.s"
            DEVICE        ${MCU_NAME}
            CPU           ${MCU_TYPE}
            )

    set(CPU_FLAGS
            -mcpu=${MCU_TYPE}
            -mfloat-abi=${MCU_FLOAT_TYPE}
            -mfpu=${MCU_FPU_NAME}
            -mthumb
            )

    target_compile_options(${TARGET_NAME} PUBLIC $<$<COMPILE_LANGUAGE:ASM>:${CFLAGS} -x assembler-with-cpp>)

    target_compile_options(${TARGET_NAME}
            PUBLIC ${CPU_FLAGS}
            PUBLIC -fdata-sections
            PUBLIC -ffunction-sections
            PUBLIC -fsigned-char
            PUBLIC -ffreestanding
            PUBLIC -nostartfiles
            PUBLIC -nostdlib
            PUBLIC -O0
            PUBLIC -std=gnu17
            PUBLIC -Wextra
            PUBLIC -Wall
            )

    target_link_options(${TARGET_NAME}
            PUBLIC ${CPU_FLAGS}
            -T $<TARGET_PROPERTY:LINKER_FILE>
            -lgcc -lc
            -specs=nano.specs 
            -specs=nosys.specs 
            -Wl,-Map=${TARGET_NAME}.map,--cref,--print-memory-usage
            -Wl,-gc-sections
            )

    # https://www.opennet.ru/docs/RUS/gcc/gcc1-2.html
    # https://gcc.gnu.org/onlinedocs/gcc/C-Dialect-Options.html
    # https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html
    # https://gcc.gnu.org/onlinedocs/gnat_ugn/Compilation-options.html
    # https://gcc.gnu.org/onlinedocs/gcc/Spec-Files.html

    #firmware_size(${TARGET_NAME})

#    #-- Custom commands ------------------------------------------------------------
    ADD_CUSTOM_COMMAND(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} "-Oihex" ${TARGET_NAME}.elf ${CMAKE_BINARY_DIR}/${TARGET_NAME}.hex
            COMMAND ${CMAKE_OBJCOPY} "-Obinary" ${TARGET_NAME}.elf ${CMAKE_BINARY_DIR}/${TARGET_NAME}.bin
            COMMAND ${CMAKE_OBJDUMP} "-DS" ${TARGET_NAME}.elf > ${CMAKE_BINARY_DIR}/${TARGET_NAME}.dasm
            #COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
            )

endif()

