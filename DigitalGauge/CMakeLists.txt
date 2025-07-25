# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(DigitalGauge C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(DigitalGauge 
    DigitalGauge.cpp 
    core/GC9A01.cpp
    core/MCP2515.cpp
    core/HolleySniper.cpp
    assets/torino_logo_sm.cpp
    fonts/font8.cpp
    fonts/font12.cpp
    fonts/font16.cpp
    fonts/font20.cpp
    fonts/font24.cpp
    fonts/font48.cpp
    fonts/font50.cpp
    fonts/LiberationSansNarrow_Bold16.cpp
    fonts/LiberationSansNarrow_Bold24.cpp
    fonts/LiberationSansNarrow_Bold28.cpp
    fonts/LiberationSansNarrow_Bold30.cpp
    fonts/LiberationSansNarrow_Bold36.cpp
    fonts/LiberationSansNarrow_Bold48.cpp
    fonts/LiberationSansNarrow_Bold54.cpp
    fonts/LiberationSansNarrow_Bold60.cpp
    fonts/LiberationSansNarrow_Bold72.cpp
    fonts/LiberationSansNarrow_Bold80.cpp
)

pico_set_program_name(DigitalGauge "DigitalGauge")
pico_set_program_version(DigitalGauge "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(DigitalGauge 0)
pico_enable_stdio_usb(DigitalGauge 1)

# Add the standard library to the build
target_link_libraries(DigitalGauge
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(DigitalGauge PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(DigitalGauge 
        hardware_spi
        hardware_watchdog
        hardware_gpio
        )

pico_add_extra_outputs(DigitalGauge)

