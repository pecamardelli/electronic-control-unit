#pragma once

#include <LCD_display.h>
#include <images.h>
#include <iostream>
#include <string>
#include <cmath>

#include "common.h"
#include "Logger.h"
#include "BCM2835Manager.h"

extern std::string IMAGES_PATH;

class DigitalGauge
{
private:
  Logger logger = Logger("DigitalGauge");
  UWORD *BlackImage;
  UDOUBLE Imagesize = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
  uint8_t lowerCaseOffset = 97;
  uint8_t upperCaseOffset = 65;

  Screen currentScreen = TORINO_LOGO;

  std::string TORINO_LOGO_PATH = IMAGES_PATH + "/torino_logo.bmp";
  std::string BACKGROUND = IMAGES_PATH + "/digital_gauge.bmp";

  sFONT LABELS_FONT = LiberationSansNarrow_Bold16;

  sFONT KML_FONT = LiberationSansNarrow_Bold48;
  const uint8_t KML_X = 75;
  const uint8_t KML_Y = 15;
  const char *KML_LABEL = "KML";
  const uint8_t KML_LABEL_X = (LCD_1IN28_WIDTH - LABELS_FONT.Width * strlen(KML_LABEL)) / 2;
  const uint8_t KML_LABEL_Y = KML_Y + KML_FONT.Height + 2;
  const float KML_DANGER_THRESHOLD = 6;
  const float KML_WARN_THRESHOLD = 8;
  float lastKmlValue = -1;

  sFONT TEMP_FONT = LiberationSansNarrow_Bold36;
  const uint8_t TEMP_X = 30;
  const uint8_t TEMP_Y = 100;
  const char *TEMP_LABEL = "TEMP";
  const uint8_t TEMP_LABEL_X = 39;
  const uint8_t TEMP_LABEL_Y = 142;
  const uint8_t TEMP_WARN_THRESHOLD = 90;
  const uint8_t TEMP_DANGER_THRESHOLD = 100;
  uint8_t lastTempValue = -1;

  sFONT VOLTS_FONT = LiberationSansNarrow_Bold36;
  const uint8_t VOLTS_X = 143;
  const uint8_t VOLTS_Y = 100;
  const char *VOLTS_LABEL = "VOLTS";
  const uint8_t VOLTS_LABEL_X = 155;
  const uint8_t VOLTS_LABEL_Y = 142;
  const float VOLTS_WARN_THRESHOLD = 12.5;
  const float VOLTS_DANGER_THRESHOLD_LOW = 12;
  const float VOLTS_DANGER_THRESHOLD_HIGH = 15;
  float lastVoltsValue = -1;

  sFONT FUEL_CONS_FONT = LiberationSansNarrow_Bold36;
  const uint8_t FUEL_CONS_X = 0;
  const uint8_t FUEL_CONS_Y = 170;
  const char *FUEL_CONS_LABEL = "LTS";
  const uint8_t FUEL_CONS_LABEL_X = (LCD_1IN28_WIDTH - LABELS_FONT.Width * strlen(FUEL_CONS_LABEL)) / 2;
  const uint8_t FUEL_CONS_LABEL_Y = FUEL_CONS_Y - LABELS_FONT.Height;
  float lastFuelConsValue = -1;

  void drawTemp(uint8_t);
  void drawKml(float);
  void drawVolts(float);
  void drawFuelConsumption(float);

public:
  DigitalGauge(/* args */);
  ~DigitalGauge();
  const std::string description = "DigitalGauge";

  void setup();
  void draw();
  void setScreen(Screen);
  void showLogo();

  void drawBmpFile(const char *);
  void clear();
};