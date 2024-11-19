#include "CircularDisplay.h"

const unsigned char *image = number_2;
CircularDisplay::CircularDisplay(/* args */) {}

CircularDisplay::~CircularDisplay() {}

void CircularDisplay::setup() {
  Config_Init();
  LCD_Init();
  LCD_SetBacklight(1000);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 0, BLACK);
  Paint_Clear(BLACK);
  // Paint_DrawCircle(120,120, 120, BLUE ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
  // Paint_DrawLine  (120, 0, 120, 12,GREEN ,DOT_PIXEL_4X4,LINE_STYLE_SOLID);
  // Paint_DrawLine  (120, 228, 120, 240,GREEN ,DOT_PIXEL_4X4,LINE_STYLE_SOLID);
  // Paint_DrawLine  (0, 120, 12, 120,GREEN ,DOT_PIXEL_4X4,LINE_STYLE_SOLID);
  // Paint_DrawLine  (228, 120, 240, 120,GREEN ,DOT_PIXEL_4X4,LINE_STYLE_SOLID);
  Paint_DrawImage(number_1, 18, 75, 20, 49);
  Paint_DrawImage(number_1, 48, 75, 20, 49);
  Paint_DrawImage(temperature_gauge_icon, 20, 120, 60, 60);

  // Paint_DrawString_CN(56,140, "微雪电子",   &Font24CN,BLACK,  WHITE);
  // Paint_DrawString_EN(80, 80, "78",&Font24,  BLACK, RED);
  // Paint_DrawString_EN(80, 100, "78",&Font24,  BLACK, RED);
  // Paint_DrawString_EN(80, 120, "444",&Font24,  BLACK, RED);
  // Paint_DrawString_EN(80, 140, "5555",&Font24,  BLACK, RED);
  // Paint_DrawLine  (120, 120, 70, 70,YELLOW ,DOT_PIXEL_3X3,LINE_STYLE_SOLID);
  // Paint_DrawLine  (120, 120, 176, 64,BLUE ,DOT_PIXEL_3X3,LINE_STYLE_SOLID);
  // Paint_DrawLine  (120, 120, 120, 210,RED ,DOT_PIXEL_2X2,LINE_STYLE_SOLID);
}

void CircularDisplay::loop() {
  // Paint_DrawImage(number_0, 85, 55, 50, 50);
  // delay(1000);
  // image = millis() % 2 == 0 ? number_3 : number_4;
}