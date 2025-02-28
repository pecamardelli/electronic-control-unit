#include "MCP2525.h"

MCP2515::MCP2515(const char *device = SPI1_DEVICE, uint32_t speedHz = 4000000)
    : spiDevice(device), mode(SPI_MODE_0), bits(8), speed(speedHz), spi_fd(-1)
{
    description = "MCP2515";
    logger = std::make_unique<Logger>(description);
    config = std::make_unique<Config>(description);

    initialized = begin();
}

// Destructor
MCP2515::~MCP2515()
{
    closeSPI();
}

bool MCP2515::begin()
{
    spi_fd = open(spiDevice, O_RDWR);
    if (spi_fd < 0)
    {
        logger->error("Cannot open SPI device " + std::string(spiDevice));
        return false;
    }

    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) == -1)
    {
        logger->error("Can't set SPI mode!");
        return false;
    }

    if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1)
    {
        logger->error("Can't set bits per word!");
        return false;
    }

    if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1)
    {
        logger->error("Can't set max speed!");
        return false;
    }

    logger->info("MCP2515 SPI Initialized on " + std::string(spiDevice));
    return true;
}

// Send a single byte via SPI
uint8_t MCP2515::spiTransfer(uint8_t data)
{
    uint8_t rxData = 0;
    struct spi_ioc_transfer tr;
    memset(&tr, 0, sizeof(tr));

    tr.tx_buf = reinterpret_cast<unsigned long>(&data);
    tr.rx_buf = reinterpret_cast<unsigned long>(&rxData);
    tr.len = 1;
    tr.speed_hz = speed;
    tr.bits_per_word = bits;
    tr.delay_usecs = 0;

    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr) < 1)
    {
        logger->error("Error sending SPI message!");
    }

    return rxData;
}

void MCP2515::reset()
{
    std::cout << "Sending MCP2515 Reset Command..." << std::endl;
    spiTransfer(0xC0); // Reset command (0xC0)
    usleep(10000);     // Delay to allow reset to complete
}

uint8_t MCP2515::readRegister(uint8_t address)
{
    uint8_t txBuffer[3] = {0x03, address, 0x00}; // 0x03 = Read Command
    uint8_t rxBuffer[3] = {0};

    struct spi_ioc_transfer tr;
    memset(&tr, 0, sizeof(tr));

    tr.tx_buf = reinterpret_cast<unsigned long>(txBuffer);
    tr.rx_buf = reinterpret_cast<unsigned long>(rxBuffer);
    tr.len = 3;
    tr.speed_hz = speed;
    tr.bits_per_word = bits;
    tr.delay_usecs = 0;

    ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
    return rxBuffer[2];
}

// Write to a register on MCP2515
void MCP2515::writeRegister(uint8_t address, uint8_t value)
{
    uint8_t txBuffer[3] = {0x02, address, value}; // 0x02 = Write Command
    struct spi_ioc_transfer tr;
    memset(&tr, 0, sizeof(tr));

    tr.tx_buf = reinterpret_cast<unsigned long>(txBuffer);
    tr.len = 3;
    tr.speed_hz = speed;
    tr.bits_per_word = bits;
    tr.delay_usecs = 0;

    ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
}

// Close SPI connection
void MCP2515::closeSPI()
{
    if (spi_fd >= 0)
    {
        close(spi_fd);
    }
}

// Parse and decode Holley Sniper 2300 CAN messages
void MCP2515::parseCANMessage(struct can_frame &frame)
{
    if (frame.can_dlc != 8)
    {
        std::cerr << "Invalid CAN frame length: " << (int)frame.can_dlc << std::endl;
        return;
    }

    // Apply the mask to extract the relevant fields
    // uint32_t canID = frame.can_id & 0xFFFFF800;

    // Decode CAN ID fields
    uint8_t commandBit = (frame.can_id >> 28) & 0x1;
    uint8_t targetID = (frame.can_id >> 25) & 0x7;
    uint16_t targetSerial = (frame.can_id >> 14) & 0x7FF;
    uint8_t sourceID = (frame.can_id >> 11) & 0x7;
    uint16_t sourceSerial = frame.can_id & 0x7FF;

    // Extract float values (Big Endian conversion required)
    float value1, value2;
    uint32_t status1, status2;

    memcpy(&status1, &frame.data[0], 4);
    memcpy(&status2, &frame.data[4], 4);

    status1 = ntohl(status1); // Convert from network byte order
    status2 = ntohl(status2);

    memcpy(&value1, &status1, 4);
    memcpy(&value2, &status2, 4);

    // Print parsed data
    std::cout << "Received CAN Message:" << std::endl;
    std::cout << "  CAN ID: 0x" << std::hex << frame.can_id << std::dec << std::endl;
    std::cout << "  Command Bit: " << (int)commandBit << std::endl;
    std::cout << "  Target ID: " << (int)targetID << std::endl;
    std::cout << "  Target Serial: " << targetSerial << std::endl;
    std::cout << "  Source ID: " << (int)sourceID << std::endl;
    std::cout << "  Source Serial: " << sourceSerial << std::endl;
    std::cout << "  Value 1: " << value1 << " | Status 1: 0x" << std::hex << status1 << std::dec << std::endl;
    std::cout << "  Value 2: " << value2 << " | Status 2: 0x" << std::hex << status2 << std::dec << std::endl;
}

void MCP2515::receiveLiveData()
{
    if (!dbc.loadDBC("Sniper.json"))
    {
        std::cerr << "Error: Failed to load DBC file!" << std::endl;
        return;
    }

    int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0)
    {
        std::cerr << "Error: Unable to create CAN socket" << std::endl;
        return;
    }

    struct ifreq ifr;
    strcpy(ifr.ifr_name, "can0");
    ioctl(sock, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "Error: Unable to bind CAN socket" << std::endl;
        close(sock);
        return;
    }

    struct can_frame frame;
    while (true)
    {
        int nbytes = read(sock, &frame, sizeof(struct can_frame));
        if (nbytes > 0)
        {
            dbc.parseCANData(frame.can_id, frame.data, frame.can_dlc);
        }
    }
    close(sock);
}