#include "DEV_Config.h"
#include "LCD_1in28.h"
#include "GUI_Paint.h"
#include "GUI_BMP.h"
#include "display.h"
#include "image.h"
#include <stdio.h>  //printf()
#include <stdlib.h> //exit()
#include <signal.h> //signal()

void LCD_init_logo(char *pathToImageFile)
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

    // /* show bmp */
    printf("show bmp\r\n");
    GUI_ReadBmp(pathToImageFile);

    LCD_1IN28_Display(BlackImage);
    DEV_Delay_ms(2000);

    // /* Module Exit */
    free(BlackImage);
    BlackImage = NULL;
    DEV_ModuleExit();
}
