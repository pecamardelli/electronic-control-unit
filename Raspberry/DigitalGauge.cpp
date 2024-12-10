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

    drawBmpFile(TORINO_LOGO_PATH.c_str());
    sleep(2);
    clear();
    drawBmpFile(BACKGROUND.c_str());
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

void DigitalGauge::drawBmpFile(const char *pathToImageFile)
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
    uint8_t _tempX = TEMP_X;
    char buffer[16];

    snprintf(buffer, sizeof(buffer), "%d", temp);

    if (temp < 10)
    {
        Paint_DrawString_EN(TEMP_X, TEMP_Y, "   ", &TEMP_FONT, BLACK, fontColor);
        _tempX = TEMP_X + TEMP_FONT.Width - 2;
    }
    else if (temp < TEMP_DANGER_THRESHOLD)
    {
        if (temp >= TEMP_WARN_THRESHOLD)
        {
            fontColor = YELLOW;
        }
        Paint_DrawString_EN(TEMP_X, TEMP_Y, "   ", &TEMP_FONT, BLACK, fontColor);
        _tempX = TEMP_X + TEMP_FONT.Width / 2 - 3;
    }
    else
    {
        fontColor = RED;
    }

    Paint_DrawString_EN(_tempX, TEMP_Y, buffer, &TEMP_FONT, BLACK, fontColor);
    lastTempValue = temp;
}

void DigitalGauge::drawKml(float kml)
{
    if (kml == lastKmlValue)
        return;

    uint16_t fontColor = WHITE;
    uint8_t _kmlX = KML_X;
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
        _kmlX = KML_X + KML_FONT.Width / 2;
    }

    Paint_DrawString_EN(_kmlX, KML_Y, buffer, &KML_FONT, BLACK, fontColor);
    lastKmlValue = kml;
}

void DigitalGauge::drawVolts(float volts)
{
    if (volts == lastVoltsValue)
        return;

    uint16_t fontColor = WHITE;
    uint8_t _voltsX = VOLTS_X;
    char buffer[16];

    if (volts < VOLTS_DANGER_THRESHOLD_LOW)
    {
        fontColor = RED;
    }
    else if (volts < VOLTS_WARN_THRESHOLD)
    {
        fontColor = YELLOW;
    }
    else if (volts >= VOLTS_DANGER_THRESHOLD_HIGH)
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
        _voltsX = VOLTS_X + VOLTS_FONT.Width / 2;
    }

    Paint_DrawString_EN(_voltsX, VOLTS_Y, buffer, &VOLTS_FONT, BLACK, fontColor);
    lastVoltsValue = volts;
}