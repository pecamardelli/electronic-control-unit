#ifndef _CIRCULAR_DISPLAY_H_
#define _CIRCULAR_DISPLAY_H_

#include <LCD_display.h>
#include <iostream>
#include <string>
#include <cmath>

class DigitalGauge
{
private:
  UWORD *BlackImage;
  UDOUBLE Imagesize = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
  uint8_t lowerCaseOffset = 97;
  uint8_t upperCaseOffset = 65;

public:
  DigitalGauge(/* args */);
  ~DigitalGauge();
  void setup();
  void loop();

  void drawBmpFile(char *);
  void clear();
};

#endif