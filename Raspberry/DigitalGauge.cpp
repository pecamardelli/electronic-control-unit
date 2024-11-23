#include "DigitalGauge.h"

DigitalGauge::DigitalGauge(/* args */)
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

    drawBmpFile("./assets/images/torino_logo.bmp");
    sleep(2);
    clear();
    drawBmpFile("./assets/images/digital_gauge.bmp");
    Paint_DrawString_EN(103, 76, "TEMP", &LiberationSansNarrow_Bold16, BLACK, WHITE);
    Paint_DrawString_EN(39, 145, "LITROS", &LiberationSansNarrow_Bold16, BLACK, WHITE);
    Paint_DrawString_EN(152, 145, "VOLTS", &LiberationSansNarrow_Bold16, BLACK, WHITE);

    LCD_1IN28_Display(BlackImage);
}

DigitalGauge::~DigitalGauge()
{
    free(BlackImage);
    BlackImage = NULL;
    DEV_ModuleExit();
}

void DigitalGauge::setup()
{
}

void DigitalGauge::loop()
{
    int temp = 0;
    float volts = 10;
    float liters = 0;

    while (1)
    {
        char buffer[16];
        uint16_t fontColor = WHITE;

        // Format the float to 1 decimal place
        if (liters < 10)
        {
            Paint_DrawString_EN(30, 100, "    ", &LiberationSansNarrow_Bold36, BLACK, fontColor);
            snprintf(buffer, sizeof(buffer), "%.1f", liters);
            Paint_DrawString_EN(38, 100, buffer, &LiberationSansNarrow_Bold36, BLACK, fontColor);
        }
        else
        {
            snprintf(buffer, sizeof(buffer), "%.1f", liters);
            Paint_DrawString_EN(30, 100, buffer, &LiberationSansNarrow_Bold36, BLACK, fontColor);
        }

        if (volts < 12)
        {
            fontColor = YELLOW;
        }
        else if (volts < 14.3)
        {
            fontColor = WHITE;
        }
        else
        {
            fontColor = RED;
        }

        snprintf(buffer, sizeof(buffer), "%.1f", volts);
        Paint_DrawString_EN(140, 100, buffer, &LiberationSansNarrow_Bold36, BLACK, fontColor);

        uint8_t Xtemp = 83;
        uint8_t Ytemp = 24;

        fontColor = WHITE;
        if (temp < 10)
        {
            Paint_DrawString_EN(Xtemp, Ytemp, "   ", &LiberationSansNarrow_Bold48, BLACK, fontColor);
            snprintf(buffer, sizeof(buffer), "%d", temp);
            Paint_DrawString_EN(Xtemp + LiberationSansNarrow_Bold48.Width, Ytemp, buffer, &LiberationSansNarrow_Bold48, BLACK, fontColor);
        }
        else if (temp < 100)
        {
            if (temp >= 90)
            {
                fontColor = YELLOW;
            }
            Paint_DrawString_EN(Xtemp, Ytemp, "   ", &LiberationSansNarrow_Bold48, BLACK, fontColor);
            snprintf(buffer, sizeof(buffer), "%d", temp);
            Paint_DrawString_EN(Xtemp + LiberationSansNarrow_Bold48.Width / 2, Ytemp, buffer, &LiberationSansNarrow_Bold48, BLACK, fontColor);
        }
        else
        {
            fontColor = RED;
            snprintf(buffer, sizeof(buffer), "%d", temp);
            Paint_DrawString_EN(Xtemp, Ytemp, buffer, &LiberationSansNarrow_Bold48, BLACK, fontColor);
        }

        // float minAngle = 151;
        // float maxAngle = 205;
        // float minTemp = 40;
        // float maxTemp = 120;
        // float tempRange = maxTemp - minTemp;
        // float angleRange = maxAngle - minAngle;
        // float tempToAngle = angleRange / tempRange;
        // float endAngle = minAngle + (temp - minTemp) * tempToAngle;
        // if (endAngle < minAngle)
        // {
        //     endAngle = minAngle;
        // }
        // // Draw a partial circle from 200° to 270°
        // printf("min: %.2f - max: %.2f\n", minAngle, endAngle);
        // Paint_DrawPartialCircleWithCaps(120, 120, 109, fontColor, DOT_PIXEL_3X3, minAngle, endAngle);
        // Paint_DrawPartialCircleWithCaps(120, 120, 104, fontColor, DOT_PIXEL_2X2, minAngle, endAngle);
        // Paint_DrawPartialCircleWithCaps(120, 120, 115, fontColor, DOT_PIXEL_2X2, 150, 210);

        LCD_1IN28_Display(BlackImage);

        if (temp < 125)
        {
            temp++;
        }
        else
        {
            temp = 0;
        }

        if (liters < 60)
        {
            liters += 0.1;
        }
        else
        {
            liters = 0;
        }

        if (volts < 14.5)
        {
            volts += 0.1;
        }
        else
        {
            volts = 10;
        }
        usleep(50000);
    }
}

void DigitalGauge::drawBmpFile(char *pathToImageFile)
{
    clear();
    GUI_ReadBmp(pathToImageFile);
    LCD_1IN28_Display(BlackImage);
}

void DigitalGauge::clear()
{
    Paint_NewImage(BlackImage, LCD_1IN28_WIDTH, LCD_1IN28_HEIGHT, 0, BLACK, 16);
    Paint_Clear(BLACK);
    LCD_1IN28_Display(BlackImage);
}
