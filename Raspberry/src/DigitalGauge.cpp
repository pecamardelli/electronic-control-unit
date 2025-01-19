#include "DigitalGauge.h"

DigitalGauge::DigitalGauge(/* args */)
{
    BCM2835Manager::getInstance();
    logger.info("Initializing Round Display.");
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
        logger.error("Failed to apply for black memory...\r\n");
        BlackImage = NULL;
    }

    logger.info("Done!");
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

void DigitalGauge::setScreen(Screen screen)
{
    switch (screen)
    {
    case DIGITAL_GAUGE:
        clear();
        drawBmpFile(BACKGROUND.c_str());
        Paint_DrawString_EN(TEMP_LABEL_X, TEMP_LABEL_Y, TEMP_LABEL, &LABELS_FONT, BLACK, WHITE);
        Paint_DrawString_EN(KML_LABEL_X, KML_LABEL_Y, KML_LABEL, &LABELS_FONT, BLACK, WHITE);
        Paint_DrawString_EN(VOLTS_LABEL_X, VOLTS_LABEL_Y, VOLTS_LABEL, &LABELS_FONT, BLACK, WHITE);
        Paint_DrawString_EN(FUEL_CONS_LABEL_X, FUEL_CONS_LABEL_Y, FUEL_CONS_LABEL, &LABELS_FONT, BLACK, WHITE);
        LCD_1IN28_Display(BlackImage);
        break;
    default:
        break;
    }

    currentScreen = screen;
}

void DigitalGauge::showLogo()
{
    clear();
    drawBmpFile(TORINO_LOGO_PATH.c_str());
    LCD_1IN28_Display(BlackImage);
}

void DigitalGauge::clear()
{
    Paint_NewImage(BlackImage, LCD_1IN28_WIDTH, LCD_1IN28_HEIGHT, 0, BLACK, 16);
    Paint_Clear(BLACK);
    LCD_1IN28_Display(BlackImage);
}

void DigitalGauge::draw()
{
    switch (currentScreen)
    {
    case DIGITAL_GAUGE:
        // drawKml(engineValues->kml);
        // drawTemp(engineValues->temp);
        // drawVolts(engineValues->volts);
        // drawFuelConsumption(engineValues->fuelConsumption);
        break;
    default:
        break;
    }

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
    uint8_t kmlX = LCD_1IN28_WIDTH / 2 - KML_FONT.Width * 1.7;
    uint8_t textWidth;
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
        kmlX = LCD_1IN28_WIDTH / 2 - KML_FONT.Width * 1.2;
        textWidth = KML_FONT.Width * 2.4;
        Paint_DrawRectangle(kmlX, KML_Y, kmlX + textWidth, KML_FONT.Height, BLACK, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    }

    Paint_DrawString_EN(kmlX, KML_Y, buffer, &KML_FONT, BLACK, fontColor);
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

void DigitalGauge::drawFuelConsumption(float fuelConsumption)
{
    if (fuelConsumption == lastFuelConsValue)
        return;

    uint16_t fontColor = WHITE;
    uint8_t fuelConsX = LCD_1IN28_WIDTH / 2 - FUEL_CONS_FONT.Width * 2.2;
    uint8_t textWidth;
    char buffer[16];

    snprintf(buffer, sizeof(buffer), "%.1f", fuelConsumption);

    if (fuelConsumption < 10)
    {
        fuelConsX = LCD_1IN28_WIDTH / 2 - FUEL_CONS_FONT.Width * 1.2;
        textWidth = FUEL_CONS_FONT.Width * 2.4;
        Paint_DrawRectangle(fuelConsX, FUEL_CONS_Y, fuelConsX + textWidth, FUEL_CONS_FONT.Height, BLACK, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    }
    else if (fuelConsumption < 100)
    {
        fuelConsX = LCD_1IN28_WIDTH / 2 - FUEL_CONS_FONT.Width * 1.7;
        textWidth = FUEL_CONS_FONT.Width * 3.4;
        Paint_DrawRectangle(fuelConsX, FUEL_CONS_Y, fuelConsX + textWidth, FUEL_CONS_FONT.Height, BLACK, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    }

    Paint_DrawString_EN(fuelConsX, FUEL_CONS_Y, buffer, &FUEL_CONS_FONT, BLACK, fontColor);
    lastFuelConsValue = fuelConsumption;
}