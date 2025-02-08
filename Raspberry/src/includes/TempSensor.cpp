#include "TempSensor.h"

TempSensor::TempSensor(/* args */)
{
    uint8_t data[5] = {0};
    if (readDHT11(data))
    {
        uint8_t checksum = data[0] + data[1] + data[2] + data[3];
        if (checksum != data[4])
        {
            std::cerr << "Checksum error.\n";
        }
        else
        {
            std::cout << "Temperature: " << static_cast<int>(data[2]) << "°C\n";
            std::cout << "Humidity: " << static_cast<int>(data[0]) << "%\n";
        }
    }
    else
    {
        std::cerr << "Failed to read from DHT11 sensor.\n";
    }
}

TempSensor::~TempSensor()
{
}

void TempSensor::sendStartSignal()
{
    bcm2835_gpio_fsel(DHT_PIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(DHT_PIN, LOW);
    bcm2835_delay(18); // 18 ms for the start signal
    bcm2835_gpio_write(DHT_PIN, HIGH);
    bcm2835_delayMicroseconds(40); // 40 µs for the signal to settle
    bcm2835_gpio_fsel(DHT_PIN, BCM2835_GPIO_FSEL_INPT);
}

bool TempSensor::waitForState(int state, int timeout_us)
{
    auto start = std::chrono::steady_clock::now();
    while (bcm2835_gpio_lev(DHT_PIN) != state)
    {
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::microseconds>(now - start).count() > timeout_us)
        {
            return false; // Timeout
        }
    }
    return true;
}

bool TempSensor::readDHT11(uint8_t *data)
{
    sendStartSignal();

    if (!waitForState(LOW, 80) || !waitForState(HIGH, 80))
    {
        return false; // Sensor didn't respond properly
    }

    // Read 40 bits (5 bytes)
    for (int i = 0; i < 5; ++i)
    {
        data[i] = 0;
        for (int j = 0; j < 8; ++j)
        {
            if (!waitForState(LOW, 50))
                return false;
            auto start = std::chrono::steady_clock::now();
            if (!waitForState(HIGH, 70))
                return false;
            auto duration = std::chrono::steady_clock::now() - start;
            if (std::chrono::duration_cast<std::chrono::microseconds>(duration).count() > 40)
            {
                data[i] |= (1 << (7 - j)); // If HIGH lasted >40 µs, it's a '1'
            }
        }
    }
    return true;
}