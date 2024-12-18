# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
include(${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(thor-freertos C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(thor-freertos
    main.c
    src/ads1115.c
    src/l293d.c
    src/sensor.c
    src/movement.c
    src/uart_rx.c
    # FreeRTOS-Kernel/portable/MemMang/heap_4.c
    # FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c
    # FreeRTOS-Kernel/portable/GCC/ARM_CM0/portasm.c
    # FreeRTOS-Kernel/croutine.c
    # FreeRTOS-Kernel/event_groups.c
    # FreeRTOS-Kernel/list.c
    # FreeRTOS-Kernel/queue.c
    # FreeRTOS-Kernel/stream_buffer.c
    # FreeRTOS-Kernel/tasks.c
    # FreeRTOS-Kernel/timers.c
)

pico_set_program_name(thor-freertos "thor-freertos")
pico_set_program_version(thor-freertos "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(thor-freertos 1)
pico_enable_stdio_usb(thor-freertos 0)

# Add the standard library to the build
target_link_libraries(thor-freertos
    pico_stdlib
    hardware_i2c
    hardware_uart
    hardware_gpio
    hardware_timer
    hardware_irq
    pico_multicore
)

#target_link_libraries(thor-freertos pico_stdlib freertos hardware_i2c hardware_uart hardware_gpio)

# Add the standard include files to the build
target_include_directories(thor-freertos PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
    ${CMAKE_CURRENT_LIST_DIR}/include
    # ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/include
    # ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel/portable/GCC/ARM_CM0
)

# Add FreeRTOS configuration directory
target_include_directories(thor-freertos PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/Config
)
