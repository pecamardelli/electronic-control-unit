#include "DEV_Config.h"
#include "LCD_1in28.h"
#include "GUI_Paint.h"
#include "GUI_BMP.h"
#include "test.h"
#include "image.h"
#include <stdio.h>  //printf()
#include <stdlib.h> //exit()
#include <signal.h> //signal()

void LCD_1IN28_test(void)
{
    // Exception handling:ctrl + c
    signal(SIGINT, Handler_1IN28_LCD);

    /* Module Init */
    if (DEV_ModuleInit() != 0)
    {
        DEV_ModuleExit();
        exit(0);
    }

    /* LCD Init */
    printf("1.28inch LCD demo...\r\n");
    LCD_1IN28_Init(HORIZONTAL);
    LCD_1IN28_Clear(BLACK);
    LCD_SetBacklight(1023);

    UWORD *BlackImage;
    UDOUBLE Imagesize = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
    printf("Imagesize = %d\r\n", Imagesize);
    if ((BlackImage = (UWORD *)malloc(Imagesize)) == NULL)
    {
        printf("Failed to apply for black memory...\r\n");
        exit(0);
    }
    // /*1.Create a new image cache named IMAGE_RGB and fill it with white*/
    Paint_NewImage(BlackImage, LCD_1IN28_WIDTH, LCD_1IN28_HEIGHT, 0, BLACK, 16);
    Paint_Clear(BLACK);
    Paint_SetRotate(ROTATE_0);
    // /* GUI */
    printf("drawing...\r\n");
    // /*2.Drawing on the image*/
    Paint_DrawCircle(120, 120, 118, BLUE, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);

    Paint_DrawLine(120, 1, 120, 12, GREEN, DOT_PIXEL_4X4, LINE_STYLE_SOLID);
    Paint_DrawLine(120, 228, 120, 240, GREEN, DOT_PIXEL_4X4, LINE_STYLE_SOLID);
    Paint_DrawLine(1, 120, 12, 120, GREEN, DOT_PIXEL_4X4, LINE_STYLE_SOLID);
    Paint_DrawLine(228, 120, 240, 120, GREEN, DOT_PIXEL_4X4, LINE_STYLE_SOLID);

    Paint_DrawImage(gImage_70X70, 85, 25, 70, 70);
    Paint_DrawString_CN(56, 140, "΢ѩ����", &Font24CN, BLACK, WHITE);
    Paint_DrawString_EN(123, 123, "WAVESHARE", &Font16, BLACK, GREEN);

    Paint_DrawLine(120, 120, 70, 70, YELLOW, DOT_PIXEL_3X3, LINE_STYLE_SOLID);
    Paint_DrawLine(120, 120, 176, 64, BLUE, DOT_PIXEL_3X3, LINE_STYLE_SOLID);
    Paint_DrawLine(120, 120, 120, 210, RED, DOT_PIXEL_2X2, LINE_STYLE_SOLID);

    // /*3.Refresh the picture in RAM to LCD*/
    LCD_1IN28_Display(BlackImage);
    DEV_Delay_ms(4000);

    // /* show bmp */
    printf("show bmp\r\n");
    GUI_ReadBmp("./pic/LCD_1inch28_1.bmp");

    LCD_1IN28_Display(BlackImage);
    DEV_Delay_ms(2000);

    // /* Module Exit */
    free(BlackImage);
    BlackImage = NULL;
    DEV_ModuleExit();
}
