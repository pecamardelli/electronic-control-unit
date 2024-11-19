#include "CircularDisplay.h"

CircularDisplay::CircularDisplay(/* args */)
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

    if ((BlackImage = (UWORD *)malloc(Imagesize)) == NULL)
    {
        printf("Failed to apply for black memory...\r\n");
        BlackImage = NULL;
    }
}

CircularDisplay::~CircularDisplay()
{
    free(BlackImage);
    BlackImage = NULL;
    DEV_ModuleExit();
}

void CircularDisplay::setup()
{
}

void CircularDisplay::loop()
{
}

void CircularDisplay::drawImage(char *pathToImageFile)
{
    clear();
    GUI_ReadBmp(pathToImageFile);
    LCD_1IN28_Display(BlackImage);
}

void CircularDisplay::clear()
{
    Paint_NewImage(BlackImage, LCD_1IN28_WIDTH, LCD_1IN28_HEIGHT, 0, BLACK, 16);
    Paint_Clear(BLACK);
    LCD_1IN28_Display(BlackImage);
}