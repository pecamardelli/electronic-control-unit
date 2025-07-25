# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)
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

project(Raspberry_Pi_Pico_2 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(Raspberry_Pi_Pico_2 
    TorinoSpeedometer.cpp 
    FlashStorage.cpp 
    Stepper.cpp 
    Speedometer.cpp 
    SpeedSensor.cpp 
    Gauge.cpp 
    Button.cpp 
    SSD1306.cpp 
    GPS.cpp 
    fonts.h 
    LiberationSansNarrow_Bold28.cpp
    helpers/DisplayHelper.cpp
    helpers/TripHelper.cpp
    helpers/OdometerHelper.cpp
    helpers/StorageHelper.cpp
    helpers/TimeHelper.cpp
)

pico_set_program_name(Raspberry_Pi_Pico_2 "Raspberry_Pi_Pico_2")
pico_set_program_version(Raspberry_Pi_Pico_2 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Raspberry_Pi_Pico_2 0)
pico_enable_stdio_usb(Raspberry_Pi_Pico_2 1)

# Add the standard library to the build
target_link_libraries(Raspberry_Pi_Pico_2
    pico_stdlib
    hardware_i2c
)

# Add the standard include files to the build
target_include_directories(Raspberry_Pi_Pico_2 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(Raspberry_Pi_Pico_2)

