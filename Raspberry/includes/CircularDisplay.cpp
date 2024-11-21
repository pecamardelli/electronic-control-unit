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

void CircularDisplay::drawBmpFile(char *pathToImageFile)
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

void CircularDisplay::drawTest(char *string, int x, int y)
{
    const size_t string_length = strlen(string);
    uint8_t offset;

    for (int i = 0; i < string_length; ++i)
    {
        if ((int)string[i] >= 33 && (int)string[i] <= 57) // Punctuation or Number
        {
            offset = 33;
        }
        else if ((int)string[i] >= 65 && (int)string[i] <= 90) // Uppercase
        {
            offset = 65 - 10; // Minus numbers
        }
        else if ((int)string[i] >= 97 && (int)string[i] <= 122) // lowercase
        {
            offset = 97 - 10 - 26; // Minus numbers and uppercase letters
        }
        else // Character not allowed
        {
            continue;
        }

        // Paint_DrawImage(characters.characters[(int)string[i] - offset], x + characters.width * i, y, characters.width, characters.height);

        Paint_DrawString_EN(63, 13, "78.5", &LiberationSansNarrow_Bold24, BLACK, GREEN);
        Paint_DrawString_EN(43, 63, "78.5", &LiberationSansNarrow_Bold36, BLACK, GREEN);
        Paint_DrawString_EN(43, 103, "78.5", &LiberationSansNarrow_Bold48, BLACK, GREEN);
        Paint_DrawString_EN(43, 143, "78.5", &LiberationSansNarrow_Bold54, BLACK, GREEN);
    }

    LCD_1IN28_Display(BlackImage);
}