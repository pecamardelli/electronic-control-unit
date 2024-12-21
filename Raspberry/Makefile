DIR_SRC		= ./src
DIR_LIB			= ${DIR_SRC}/lib
DIR_INCLUDES	= ${DIR_SRC}/includes
DIR_HELPERS		= ${DIR_SRC}/helpers
DIR_ASSETS		= ${DIR_SRC}/assets
DIR_IMAGES		= ${DIR_ASSETS}/images

DIR_DISPLAY				= ${DIR_LIB}/LCD_display
DIR_DISPLAY_CONFIG		= ${DIR_DISPLAY}/Config
DIR_DISPLAY_EPD			= ${DIR_DISPLAY}/LCD
DIR_DISPLAY_FONTS		= ${DIR_DISPLAY}/Fonts
DIR_DISPLAY_GUI			= ${DIR_DISPLAY}/GUI

DIR_STEPPER		= $(DIR_LIB)/Stepper
DIR_GPS			= $(DIR_LIB)/GPS

TARGET = torino-ecu
BUILD_DIR := build
DEST_DIR := /usr/local/bin
CONFIG_DIR := /etc/$(TARGET)
CONFIG_FILE := $(DIR_SRC)/config.ini

# Rules
all: prepare $(TARGET)

# Pre-build step
prepare:
	@echo "Running pre-build scripts..."
	@mkdir -p $(BUILD_DIR)
	@echo "Checking dependencies..."
	@if ! dpkg -s libbcm2835-dev > /dev/null 2>&1; then \
	    echo "'libbcm2835-dev' is not installed."; \
	    apt-get install -y libbcm2835-dev; \
	fi

SRC_C = $(wildcard ${DIR_DISPLAY}/*.c ${DIR_DISPLAY_EPD}/*.c ${DIR_DISPLAY_FONTS}/*.c ${DIR_DISPLAY_GUI}/*.c ${DIR_DISPLAY_CONFIG}/*.c)
SRC_CPP = $(wildcard ${DIR_SRC}/*.cpp ${DIR_INCLUDES}/*.cpp ${DIR_HELPERS}/*.cpp ${DIR_IMAGES}/*.cpp ${DIR_STEPPER}/*.cpp ${DIR_GPS}/*.cpp)
OBJ_O = $(patsubst %.c,${BUILD_DIR}/%.o,$(notdir ${SRC_C})) $(patsubst %.cpp,${BUILD_DIR}/%.o,$(notdir ${SRC_CPP}))

USELIB = USE_BCM2835_LIB
#USELIB = USE_WIRINGPI_LIB
#USELIB = USE_DEV_LIB
DEBUG = -D $(USELIB)
ifeq ($(USELIB), USE_BCM2835_LIB)
    LIB = -lbcm2835 -lm 
else ifeq ($(USELIB), USE_WIRINGPI_LIB)
    LIB = -lwiringPi -lm 
else ifeq ($(USELIB), USE_DEV_LIB)
    LIB = -lpthread -lm 
endif

CC = g++
MSG = -g -O0 -Wall
CFLAGS += $(MSG) $(DEBUG)

DISPLAY = -I $(DIR_DISPLAY_CONFIG) -I $(DIR_DISPLAY_GUI) -I $(DIR_DISPLAY_EPD) -I $(DIR_DISPLAY) -I $(DIR_IMAGES)

${TARGET}:${OBJ_O}
	$(CC) $(CFLAGS) $(OBJ_O) -o $@ $(LIB)
    
${BUILD_DIR}/%.o:$(DIR_SRC)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@ -I $(DIR_SRC) $(DISPLAY) -I $(DIR_STEPPER) -I $(DIR_GPS) -I $(DIR_HELPERS) -I $(DIR_INCLUDES)
    
${BUILD_DIR}/%.o:$(DIR_DISPLAY)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I $(DIR_DISPLAY_CONFIG) -I $(DIR_DISPLAY_GUI) -I $(DIR_DISPLAY_EPD)

${BUILD_DIR}/%.o:$(DIR_DISPLAY_EPD)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I $(DIR_DISPLAY_CONFIG)
    
${BUILD_DIR}/%.o:$(DIR_DISPLAY_FONTS)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
    
${BUILD_DIR}/%.o:$(DIR_DISPLAY_GUI)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I $(DIR_DISPLAY_CONFIG) -I $(DIR_DISPLAY_EPD) -I $(DIR_DISPLAY)

${BUILD_DIR}/%.o:$(DIR_DISPLAY_CONFIG)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LIB)

${BUILD_DIR}/%.o:$(DIR_INCLUDES)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

${BUILD_DIR}/%.o:$(DIR_HELPERS)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

${BUILD_DIR}/%.o:$(DIR_IMAGES)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

${BUILD_DIR}/%.o:$(DIR_STEPPER)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

${BUILD_DIR}/%.o:$(DIR_GPS)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@ -I $(DIR_INCLUDES)
	
# Install
install: all
	@echo "Installing to $(DEST_DIR)..."
	@mkdir -p $(DEST_DIR)
	@cp $(TARGET) $(DEST_DIR)
	@echo "Copying configuration file to $(CONFIG_DIR)..."
	@mkdir -p $(CONFIG_DIR)
	@cp $(CONFIG_FILE) $(CONFIG_DIR)
	@echo "Copying assets to $(CONFIG_DIR)..."
	@echo "Running initial setup script..."
	@cp -R $(DIR_ASSETS) $(CONFIG_DIR)
	@bash setup.sh $(TARGET) $(DEST_DIR) $(CONFIG_DIR)

# Clean up
clean:
	@echo "Cleaning up..."
	@rm -rf $(BUILD_DIR) $(TARGET)

# Phony targets
.PHONY: all prepare install clean