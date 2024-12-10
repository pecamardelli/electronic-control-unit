#ifndef _CIRCULAR_DISPLAY_H_
#define _CIRCULAR_DISPLAY_H_

#include <LCD_display.h>
#include <images.h>
#include <iostream>
#include <string>
#include <cmath>
#include "includes/Common.h"

extern std::string IMAGES_PATH;
extern EngineValues engineValues;

class RoundDisplay
{
private:
  UWORD *BlackImage;
  UDOUBLE Imagesize = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
  uint8_t lowerCaseOffset = 97;
  uint8_t upperCaseOffset = 65;

  Screen currentScreen = TORINO_LOGO;

  std::string TORINO_LOGO_PATH = IMAGES_PATH + "/torino_logo.bmp";
  std::string BACKGROUND = IMAGES_PATH + "/digital_gauge.bmp";

  sFONT KML_FONT = LiberationSansNarrow_Bold48;
  const uint8_t KML_X = 75;
  const uint8_t KML_Y = 24;
  const uint8_t KML_LABEL_X = 105;
  const uint8_t KML_LABEL_Y = 76;
  const float KML_DANGER_THRESHOLD = 6;
  const float KML_WARN_THRESHOLD = 8;
  float lastKmlValue = 0;

  sFONT TEMP_FONT = LiberationSansNarrow_Bold36;
  const uint8_t TEMP_X = 30;
  const uint8_t TEMP_Y = 100;
  const uint8_t TEMP_LABEL_X = 39;
  const uint8_t TEMP_LABEL_Y = 145;
  const uint8_t TEMP_WARN_THRESHOLD = 90;
  const uint8_t TEMP_DANGER_THRESHOLD = 100;
  uint8_t lastTempValue = 0;

  sFONT VOLTS_FONT = LiberationSansNarrow_Bold36;
  const uint8_t VOLTS_X = 143;
  const uint8_t VOLTS_Y = 100;
  const uint8_t VOLTS_LABEL_X = 155;
  const uint8_t VOLTS_LABEL_Y = 145;
  const float VOLTS_WARN_THRESHOLD = 12.5;
  const float VOLTS_DANGER_THRESHOLD_LOW = 12;
  const float VOLTS_DANGER_THRESHOLD_HIGH = 15;
  float lastVoltsValue = 0;

  void drawTemp(uint8_t);
  void drawKml(float);
  void drawVolts(float);

public:
  RoundDisplay(/* args */);
  ~RoundDisplay();
  void setup();
  void draw();
  void setScreen(Screen);
  void showLogo();

  void drawBmpFile(const char *);
  void clear();
};

#endif