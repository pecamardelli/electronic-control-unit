#include "DEV_Config.h"
#include "LCD_0in96.h"
#include "GUI_Paint.h"
#include "GUI_BMP.h"
#include "test.h"
#include <stdio.h>		//printf()
#include <stdlib.h>		//exit()
#include <signal.h>     //signal()

void LCD_0IN96_test(void)
{
    // Exception handling:ctrl + c
    signal(SIGINT, Handler_0IN96_LCD);
    
    /* Module Init */
	if(DEV_ModuleInit() != 0){
        DEV_ModuleExit();
        exit(0);
    }
	
    /* LCD Init */
	printf("0.96inch LCD demo...\r\n");
	LCD_0IN96_Init();
	LCD_0IN96_Clear(WHITE);
	//LCD_SetBacklight(1023);
	
    UDOUBLE Imagesize = LCD_0IN96_HEIGHT*LCD_0IN96_WIDTH*2;
    UWORD *BlackImage;
    if((BlackImage = (UWORD *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        exit(0);
    }
	
    /*1.Create a new image cache named IMAGE_RGB and fill it with white*/
    Paint_NewImage(BlackImage, LCD_0IN96_WIDTH, LCD_0IN96_HEIGHT, 0, WHITE, 16);
    Paint_Clear(WHITE);
	//Paint_SetRotate(ROTATE_180);
    /* GUI */
    printf("drawing...\r\n");
    /*2.Drawing on the image*/
	Paint_DrawPoint(2,1, BLACK, DOT_PIXEL_1X1,  DOT_FILL_RIGHTUP);//240 240
	Paint_DrawPoint(2,6, BLACK, DOT_PIXEL_2X2,  DOT_FILL_RIGHTUP);
	Paint_DrawPoint(2,11, BLACK, DOT_PIXEL_3X3, DOT_FILL_RIGHTUP);
	Paint_DrawPoint(2,16, BLACK, DOT_PIXEL_4X4, DOT_FILL_RIGHTUP);
	Paint_DrawPoint(2,21, BLACK, DOT_PIXEL_5X5, DOT_FILL_RIGHTUP);
	Paint_DrawLine( 10,  5, 40, 35, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
	Paint_DrawLine( 10, 35, 40,  5, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);

	Paint_DrawLine( 80,  20, 110, 20, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
	Paint_DrawLine( 95,   5,  95, 35, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);

	Paint_DrawRectangle(10, 5, 40, 35, RED, DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
	Paint_DrawRectangle(45, 5, 75, 35, BLUE, DOT_PIXEL_2X2,DRAW_FILL_FULL);

	Paint_DrawCircle( 95,20, 15, RED  ,DOT_PIXEL_1X1,DRAW_FILL_EMPTY);
	Paint_DrawCircle(130,20, 15, GREEN   ,DOT_PIXEL_1X1,DRAW_FILL_FULL);
	
	Paint_DrawFloatNum (23, 37 ,987.654321,3, &Font12,  0x000f,  0xfff0);
	Paint_DrawString_EN(1, 37, "ABC", &Font12, 0x000f, 0xfff0);
	Paint_DrawString_CN(1,43, "Î¢Ñ©µç×Ó",  &Font24CN, WHITE, BLUE);
	   
    /*3.Refresh the picture in RAM to LCD*/
    LCD_0IN96_Display(BlackImage);
	DEV_Delay_ms(2000);
	
    /* show bmp */
	printf("show bmp\r\n");
	
	GUI_ReadBmp("./pic/LCD_0inch96.bmp");    
    LCD_0IN96_Display(BlackImage);
    DEV_Delay_ms(2000);
	

	
    /* Module Exit */
    free(BlackImage);
    BlackImage = NULL;
	DEV_ModuleExit();
}

