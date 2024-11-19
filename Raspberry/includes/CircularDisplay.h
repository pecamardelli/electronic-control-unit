#ifndef _CIRCULAR_DISPLAY_H_
#define _CIRCULAR_DISPLAY_H_

#include <display.h>

class CircularDisplay
{
private:
  UWORD *BlackImage;
  UDOUBLE Imagesize = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;

public:
  CircularDisplay(/* args */);
  ~CircularDisplay();
  void setup();
  void loop();

  void drawImage(char *);
  void clear();
};

#endif