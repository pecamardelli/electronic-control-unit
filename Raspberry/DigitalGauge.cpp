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
    Paint_DrawString_EN(TEMP_LABEL_X, TEMP_LABEL_Y, "TEMP", &LiberationSansNarrow_Bold16, BLACK, WHITE);
    Paint_DrawString_EN(KML_LABEL_X, KML_LABEL_Y, "KM/L", &LiberationSansNarrow_Bold16, BLACK, WHITE);
    Paint_DrawString_EN(VOLTS_LABEL_X, VOLTS_LABEL_Y, "VOLTS", &LiberationSansNarrow_Bold16, BLACK, WHITE);

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

void DigitalGauge::draw(int temp, float kml, float volts)
{
    drawKml(kml);
    drawTemp(temp);
    drawVolts(volts);
    LCD_1IN28_Display(BlackImage);
}

void DigitalGauge::drawTemp(uint8_t temp)
{
    if (temp == lastTempValue)
        return;

    uint16_t fontColor = WHITE;
    char buffer[16];

    snprintf(buffer, sizeof(buffer), "%d", temp);

    if (temp < 10)
    {
        Paint_DrawString_EN(TEMP_X, TEMP_Y, "   ", &TEMP_FONT, BLACK, fontColor);
        Paint_DrawString_EN(TEMP_X + TEMP_FONT.Width - 2, TEMP_Y, buffer, &TEMP_FONT, BLACK, fontColor);
    }
    else if (temp < TEMP_DANGER_THRESHOLD)
    {
        if (temp >= TEMP_WARN_THRESHOLD)
        {
            fontColor = YELLOW;
        }
        Paint_DrawString_EN(TEMP_X, TEMP_Y, "   ", &TEMP_FONT, BLACK, fontColor);
        Paint_DrawString_EN(TEMP_X + TEMP_FONT.Width / 2 - 3, TEMP_Y, buffer, &TEMP_FONT, BLACK, fontColor);
    }
    else
    {
        fontColor = RED;
        Paint_DrawString_EN(TEMP_X, TEMP_Y, buffer, &TEMP_FONT, BLACK, fontColor);
    }

    lastTempValue = temp;
}

void DigitalGauge::drawKml(float kml)
{
    if (kml == lastKmlValue)
        return;

    uint16_t fontColor = WHITE;
    char buffer[16];

    if (kml < KML_DANGER_THRESHOLD)
    {
        fontColor = RED;
    }
    else if (kml < KML_WARN_THRESHOLD)
    {
        fontColor = YELLOW;
    }

    snprintf(buffer, sizeof(buffer), "%.1f", kml);

    if (kml < 10)
    {
        Paint_DrawString_EN(KML_X, KML_Y, "    ", &KML_FONT, BLACK, fontColor);
        Paint_DrawString_EN(KML_X + KML_FONT.Width / 2, KML_Y, buffer, &KML_FONT, BLACK, fontColor);
    }
    else
    {
        Paint_DrawString_EN(KML_X, KML_Y, buffer, &KML_FONT, BLACK, fontColor);
    }

    lastKmlValue = kml;
}

void DigitalGauge::drawVolts(float volts)
{
    if (volts == lastVoltsValue)
        return;

    uint16_t fontColor = WHITE;
    char buffer[16];

    if (volts < VOLTS_WARN_THRESHOLD)
    {
        fontColor = YELLOW;
    }
    else if (volts >= VOLTS_DANGER_THRESHOLD)
    {
        fontColor = RED;
    }
    else
    {
        fontColor = WHITE;
    }

    snprintf(buffer, sizeof(buffer), "%.1f", volts);

    if (volts < 10)
    {
        Paint_DrawString_EN(VOLTS_X, VOLTS_Y, "    ", &VOLTS_FONT, BLACK, fontColor);
        Paint_DrawString_EN(VOLTS_X + VOLTS_FONT.Width / 2, VOLTS_Y, buffer, &VOLTS_FONT, BLACK, fontColor);
    }
    else
    {
        Paint_DrawString_EN(VOLTS_X, VOLTS_Y, buffer, &VOLTS_FONT, BLACK, fontColor);
    }

    lastVoltsValue = volts;
}