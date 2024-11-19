DIR_LIB			= ./lib
DIR_INCLUDES	= ./includes

DIR_DISPLAY				= ${DIR_LIB}/display
DIR_DISPLAY_CONFIG		= ${DIR_DISPLAY}/lib/Config
DIR_DISPLAY_EPD			= ${DIR_DISPLAY}/lib/LCD
DIR_DISPLAY_FONTS		= ${DIR_DISPLAY}/lib/Fonts
DIR_DISPLAY_GUI			= ${DIR_DISPLAY}/lib/GUI
DIR_DISPLAY_EXAMPLES	= ${DIR_DISPLAY}/examples

DIR_BIN		= ./bin
DIR_MAIN	= ./

SRC_C = $(wildcard ${DIR_DISPLAY_EXAMPLES}/*.c ${DIR_DISPLAY_EPD}/*.c ${DIR_DISPLAY_FONTS}/*.c ${DIR_DISPLAY_GUI}/*.c ${DIR_DISPLAY_CONFIG}/*.c)
SRC_CPP = $(wildcard ${DIR_MAIN}/*.cpp ${DIR_INCLUDES}/*.cpp)
OBJ_O = $(patsubst %.c,${DIR_BIN}/%.o,$(notdir ${SRC_C})) $(patsubst %.cpp,${DIR_BIN}/%.o,$(notdir ${SRC_CPP}))

TARGET = torino-ecu

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

DISPLAY = -I $(DIR_DISPLAY_CONFIG) -I $(DIR_DISPLAY_GUI) -I $(DIR_DISPLAY_EPD) -I $(DIR_DISPLAY_EXAMPLES)

${TARGET}:${OBJ_O}
	$(CC) $(CFLAGS) $(OBJ_O) -o $@ $(LIB)
    
${DIR_BIN}/%.o:$(DIR_MAIN)/%.cpp
	$(CC) $(CFLAGS) -c  $< -o $@ $(DISPLAY)
    
${DIR_BIN}/%.o:$(DIR_DISPLAY_EXAMPLES)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ -I $(DIR_DISPLAY_CONFIG) -I $(DIR_DISPLAY_GUI) -I $(DIR_DISPLAY_EPD)

${DIR_BIN}/%.o:$(DIR_DISPLAY_EPD)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ -I $(DIR_DISPLAY_CONFIG)
    
${DIR_BIN}/%.o:$(DIR_DISPLAY_FONTS)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@
    
${DIR_BIN}/%.o:$(DIR_DISPLAY_GUI)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ -I $(DIR_DISPLAY_CONFIG)  -I $(DIR_DISPLAY_EPD) -I $(DIR_DISPLAY_EXAMPLES)

${DIR_BIN}/%.o:$(DIR_DISPLAY_CONFIG)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ $(LIB)

${DIR_BIN}/%.o:$(DIR_INCLUDES)/%.cpp
	$(CC) $(CFLAGS) -c  $< -o $@ $(DISPLAY)
	
clean :
	rm $(DIR_BIN)/*.* 
	rm $(TARGET) 
