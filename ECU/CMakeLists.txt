cmake_minimum_required(VERSION 3.16)

# Project Name
project(torino-ecu VERSION 1.0 LANGUAGES C CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Directories
set(DIR_SRC "${CMAKE_SOURCE_DIR}/src")
set(DIR_LIB "${DIR_SRC}/lib")
set(DIR_CORE "${DIR_SRC}/core")
set(DIR_HARDWARE "${DIR_SRC}/hardware")
set(DIR_HELPERS "${DIR_SRC}/helpers")
set(DIR_ASSETS "${DIR_SRC}/assets")
set(DIR_IMAGES "${DIR_ASSETS}/images")

set(DIR_DISPLAY "${DIR_LIB}/LCD_display")
set(DIR_DISPLAY_CONFIG "${DIR_DISPLAY}/Config")
set(DIR_DISPLAY_EPD "${DIR_DISPLAY}/LCD")
set(DIR_DISPLAY_FONTS "${DIR_DISPLAY}/Fonts")
set(DIR_DISPLAY_GUI "${DIR_DISPLAY}/GUI")

set(DIR_STEPPER "${DIR_LIB}/Stepper")

# Output Directories
set(BUILD_DIR "${CMAKE_BINARY_DIR}/build")
set(DEST_DIR "/usr/local/bin")
set(CONFIG_DIR "/etc")
set(CONFIG_FILE "${DIR_SRC}/config.ini")
set(DATA_DIR "/var/${PROJECT_NAME}")

# Collect Sources
file(GLOB SRC_CPP
    "${DIR_SRC}/*.cpp"
    "${DIR_CORE}/*.cpp"
    "${DIR_HARDWARE}/*.cpp"
    "${DIR_HELPERS}/*.cpp"
    "${DIR_IMAGES}/*.cpp"
    "${DIR_STEPPER}/*.cpp"
    "${DIR_DISPLAY}/*.cpp"
    "${DIR_DISPLAY_EPD}/*.cpp"
    "${DIR_DISPLAY_FONTS}/*.cpp"
    "${DIR_DISPLAY_GUI}/*.cpp"
    "${DIR_DISPLAY_CONFIG}/*.cpp"
)

# Include Directories
include_directories(
    ${DIR_DISPLAY_CONFIG}
    ${DIR_DISPLAY_GUI}
    ${DIR_DISPLAY_EPD}
    ${DIR_DISPLAY_FONTS}
    ${DIR_DISPLAY}
    ${DIR_IMAGES}
    ${DIR_STEPPER}
    ${DIR_HELPERS}
    ${DIR_HARDWARE}
    ${DIR_CORE}
)

# Library Options
option(USE_BCM2835_LIB "Use bcm2835 library" ON)
option(USE_WIRINGPI_LIB "Use WiringPi library" OFF)
option(USE_DEV_LIB "Use custom development library" OFF)

if(USE_BCM2835_LIB)
    add_definitions(-DUSE_BCM2835_LIB)
    set(LIBRARIES bcm2835 m)
elseif(USE_WIRINGPI_LIB)
    add_definitions(-DUSE_WIRINGPI_LIB)
    set(LIBRARIES wiringPi m)
elseif(USE_DEV_LIB)
    add_definitions(-DUSE_DEV_LIB)
    set(LIBRARIES pthread m)
else()
    message(FATAL_ERROR "No valid library option selected. Enable one of USE_BCM2835_LIB, USE_WIRINGPI_LIB, or USE_DEV_LIB.")
endif()

# Compiler Flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")

# Target
add_executable(${PROJECT_NAME} ${SRC_CPP})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

# Installation
install(TARGETS ${PROJECT_NAME} DESTINATION ${DEST_DIR})
install(FILES ${CONFIG_FILE} DESTINATION ${CONFIG_DIR}/${PROJECT_NAME})

# Dependency Checks
find_program(BCM2835_CHECK bcm2835-config PATHS /usr/local/bin)
if(NOT BCM2835_CHECK AND USE_BCM2835_LIB)
    execute_process(COMMAND sudo apt-get install -y libbcm2835-dev
                    OUTPUT_VARIABLE BCM2835_OUTPUT
                    ERROR_VARIABLE BCM2835_ERROR)
    message(STATUS "bcm2835 library installed: ${BCM2835_OUTPUT}")
endif()

find_program(CCACHE_CHECK ccache PATHS /usr/bin)
if(NOT CCACHE_CHECK)
    execute_process(COMMAND sudo apt-get install -y ccache
                    OUTPUT_VARIABLE CCACHE_OUTPUT
                    ERROR_VARIABLE CCACHE_ERROR)
    message(STATUS "ccache installed: ${CCACHE_OUTPUT}")
endif()

# Ensure Required Directories Exist
file(MAKE_DIRECTORY ${BUILD_DIR})
file(MAKE_DIRECTORY ${CONFIG_DIR}/${PROJECT_NAME})
file(MAKE_DIRECTORY ${DATA_DIR})

# Custom Clean Target
add_custom_target(custom_clean
    COMMAND echo "Cleaning up..."
    COMMAND rm -rf ${BUILD_DIR} ${PROJECT_NAME}
)
