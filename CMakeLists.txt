cmake_minimum_required(VERSION 3.20)

project(nucleo_f401re_led C ASM)

option(BUILD_FIRMWARE "Build the STM32F401RE firmware" OFF)
option(BUILD_TESTING "Build host-side unit tests" ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(BUILD_FIRMWARE)
    if(NOT STM32CUBE_F4_DIR AND DEFINED ENV{STM32CUBE_F4_DIR})
        set(STM32CUBE_F4_DIR "$ENV{STM32CUBE_F4_DIR}")
    endif()

    if(NOT STM32CUBE_F4_DIR OR NOT EXISTS "${STM32CUBE_F4_DIR}/Drivers/STM32F4xx_HAL_Driver")
        message(FATAL_ERROR
            "STM32CubeF4 not found. Set STM32CUBE_F4_DIR to the STM32CubeF4 directory.")
    endif()

    set(CUBE_PROJECT_DIR
        "${STM32CUBE_F4_DIR}/Projects/STM32F401RE-Nucleo/Examples/GPIO/GPIO_IOToggle")
    set(CUBE_HAL_DIR "${STM32CUBE_F4_DIR}/Drivers/STM32F4xx_HAL_Driver")
    set(CUBE_CMSIS_DIR "${STM32CUBE_F4_DIR}/Drivers/CMSIS")
    set(CUBE_DEVICE_DIR "${CUBE_CMSIS_DIR}/Device/ST/STM32F4xx")

    set(HAL_SOURCES
        "${CUBE_HAL_DIR}/Src/stm32f4xx_hal.c"
        "${CUBE_HAL_DIR}/Src/stm32f4xx_hal_cortex.c"
        "${CUBE_HAL_DIR}/Src/stm32f4xx_hal_flash.c"
        "${CUBE_HAL_DIR}/Src/stm32f4xx_hal_flash_ex.c"
        "${CUBE_HAL_DIR}/Src/stm32f4xx_hal_gpio.c"
        "${CUBE_HAL_DIR}/Src/stm32f4xx_hal_rcc.c"
        "${CUBE_HAL_DIR}/Src/stm32f4xx_hal_rcc_ex.c")

    set(STARTUP_SOURCE
        "${CUBE_PROJECT_DIR}/STM32CubeIDE/Example/Startup/startup_stm32f401ceux.s")
    set(SYSTEM_SOURCE "${CUBE_PROJECT_DIR}/Src/system_stm32f4xx.c")

    foreach(required_file IN ITEMS ${HAL_SOURCES} "${STARTUP_SOURCE}" "${SYSTEM_SOURCE}")
        if(NOT EXISTS "${required_file}")
            message(FATAL_ERROR "Required STM32CubeF4 file not found: ${required_file}")
        endif()
    endforeach()

    add_executable(firmware
        src/main.c
        src/app.c
        src/board_stm32f4.c
        src/stm32f4xx_it.c
        ${HAL_SOURCES}
        "${STARTUP_SOURCE}"
        "${SYSTEM_SOURCE}")

    target_include_directories(firmware PRIVATE
        include
        config
        "${CUBE_HAL_DIR}/Inc"
        "${CUBE_CMSIS_DIR}/Include"
        "${CUBE_DEVICE_DIR}/Include")
    target_compile_definitions(firmware PRIVATE
        STM32F401xE
        USE_HAL_DRIVER)
    target_compile_options(firmware PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
        -ffunction-sections
        -fdata-sections
        -Wall
        -Wextra)
    target_link_options(firmware PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
        --specs=nosys.specs
        -T${CMAKE_SOURCE_DIR}/linker/STM32F401RETx_FLASH.ld
        -Wl,--gc-sections
        -Wl,-Map=${CMAKE_BINARY_DIR}/firmware.map
        -Wl,--print-memory-usage)
    add_custom_command(TARGET firmware POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:firmware>
                "${CMAKE_BINARY_DIR}/firmware.bin"
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:firmware>
                "${CMAKE_BINARY_DIR}/firmware.hex"
        VERBATIM)
    set_target_properties(firmware PROPERTIES
        SUFFIX ".elf"
        LINK_DEPENDS "${CMAKE_SOURCE_DIR}/linker/STM32F401RETx_FLASH.ld")
endif()

if(BUILD_TESTING)
    include(CTest)
    add_executable(test_app test/test_app.c src/app.c)
    target_include_directories(test_app PRIVATE include)
    if(MSVC)
        target_compile_options(test_app PRIVATE /W4)
    else()
        target_compile_options(test_app PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    add_test(NAME app_state_machine COMMAND test_app)
endif()
