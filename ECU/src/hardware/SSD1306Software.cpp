#include "SSD1306Software.h"

SSD1306Software::SSD1306Software()
{
    description = "SSD1306Software";
    logger = std::make_unique<Logger>(description);
    config = std::make_unique<Config>(description);

    init();
    clear();
}

SSD1306Software::~SSD1306Software()
{
}

// Initialize the SSD1306 display
void SSD1306Software::init()
{
    i2cBus.start();
    sendInitCommands();
    i2cBus.stop();
}

// Send command to SSD1306
void SSD1306Software::sendCommand(uint8_t cmd)
{
    i2cBus.start();
    i2cBus.writeByte(SSD1306_I2C_ADDR << 1);
    i2cBus.writeByte(SSD1306_COMMAND);
    i2cBus.writeByte(cmd);
    i2cBus.stop();
}

// Update the display with buffer content
void SSD1306Software::display()
{
    sendCommand(0x21); // Set column address
    sendCommand(0x00);
    sendCommand(SSD1306_WIDTH - 1);
    sendCommand(0x22); // Set page address
    sendCommand(0x00);
    sendCommand((SSD1306_HEIGHT / 8) - 1);

    for (uint16_t i = 0; i < sizeof(buffer); i++)
    {
        i2cBus.start();
        i2cBus.writeByte(SSD1306_I2C_ADDR << 1);
        i2cBus.writeByte(SSD1306_DATA);
        i2cBus.writeByte(buffer[i]);
        i2cBus.stop();
    }
}

void SSD1306Software::loop()
{
    while (!terminateFlag.load())
    {
        currentRoundedValue = round(mileage->currentTotal * 10) / 10;
        if (lastRoundedValue != currentRoundedValue)
        {
            roundedValue.str("");
            roundedValue.clear();
            roundedValue << std::fixed << std::setprecision(1) << mileage->currentTotal;
            drawString(SSD1306_ALIGN_CENTER, roundedValue.str().c_str(), LiberationSansNarrow_Bold28);
            lastRoundedValue = currentRoundedValue;
        }

        std::this_thread::sleep_for(std::chrono::microseconds(loopInterval));
    }
}