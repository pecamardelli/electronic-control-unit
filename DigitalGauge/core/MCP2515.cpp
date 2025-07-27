#include "MCP2515.h"
#include <cstring>

// Configuration values for different bit rates (8MHz crystal)
struct BitRateConfig {
    uint8_t cnf1;
    uint8_t cnf2;
    uint8_t cnf3;
};

const BitRateConfig bitRateConfigs[] = {
    {0x1F, 0xBF, 0x87},  // 5KBPS
    {0x0F, 0xBF, 0x87},  // 10KBPS
    {0x07, 0xBF, 0x87},  // 20KBPS
    {0x07, 0xFA, 0x87},  // 31.25KBPS
    {0x07, 0xBC, 0x87},  // 33KBPS
    {0x03, 0xBF, 0x87},  // 40KBPS
    {0x03, 0xFA, 0x87},  // 50KBPS
    {0x01, 0xBF, 0x87},  // 80KBPS
    {0x01, 0xBC, 0x87},  // 83.3KBPS
    {0x01, 0xA4, 0x87},  // 95KBPS
    {0x01, 0xFA, 0x87},  // 100KBPS
    {0x01, 0xF0, 0x86},  // 125KBPS
    {0x00, 0xFA, 0x87},  // 200KBPS
    {0x00, 0xF0, 0x86},  // 250KBPS
    {0x00, 0xD0, 0x82},  // 500KBPS
    {0x00, 0x80, 0x80}   // 1000KBPS
};

MCP2515::MCP2515(spi_inst_t* spi, uint8_t cs, uint8_t interrupt_pin) 
    : spi_port(spi), pin_cs(cs), pin_int(interrupt_pin) {
}

void MCP2515::selectChip() {
    gpio_put(pin_cs, 0);
}

void MCP2515::deselectChip() {
    gpio_put(pin_cs, 1);
}

uint8_t MCP2515::spiTransfer(uint8_t data) {
    uint8_t result;
    spi_write_read_blocking(spi_port, &data, &result, 1);
    return result;
}

void MCP2515::spiWrite(uint8_t* data, size_t len) {
    spi_write_blocking(spi_port, data, len);
}

void MCP2515::spiRead(uint8_t* data, size_t len) {
    spi_read_blocking(spi_port, 0x00, data, len);
}

uint8_t MCP2515::readRegister(uint8_t reg) {
    selectChip();
    spiTransfer(MCP2515_READ);
    spiTransfer(reg);
    uint8_t result = spiTransfer(0x00);
    deselectChip();
    return result;
}

void MCP2515::writeRegister(uint8_t reg, uint8_t value) {
    selectChip();
    spiTransfer(MCP2515_WRITE);
    spiTransfer(reg);
    spiTransfer(value);
    deselectChip();
}

void MCP2515::modifyRegister(uint8_t reg, uint8_t mask, uint8_t value) {
    selectChip();
    spiTransfer(MCP2515_BIT_MODIFY);
    spiTransfer(reg);
    spiTransfer(mask);
    spiTransfer(value);
    deselectChip();
}

void MCP2515::reset() {
    selectChip();
    spiTransfer(MCP2515_RESET);
    deselectChip();
    sleep_ms(10);  // Wait for reset to complete
}

uint8_t MCP2515::getMode() {
    return readRegister(MCP2515_CANSTAT) & MCP2515_MODE_MASK;
}

bool MCP2515::setMode(uint8_t mode) {
    modifyRegister(MCP2515_CANCTRL, MCP2515_MODE_MASK, mode);
    
    // Wait for mode change with timeout
    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    while ((getMode() != mode) && (to_ms_since_boot(get_absolute_time()) - start_time < 100)) {
        sleep_ms(1);
    }
    
    return getMode() == mode;
}

void MCP2515::configureBitRate(CANBitRate bitRate) {
    if (bitRate >= sizeof(bitRateConfigs) / sizeof(bitRateConfigs[0])) {
        bitRate = CAN_500KBPS;  // Default fallback
    }
    
    writeRegister(MCP2515_CNF1, bitRateConfigs[bitRate].cnf1);
    writeRegister(MCP2515_CNF2, bitRateConfigs[bitRate].cnf2);
    writeRegister(MCP2515_CNF3, bitRateConfigs[bitRate].cnf3);
}

bool MCP2515::init(CANBitRate bitRate) {
    // Initialize GPIO pins
    gpio_init(pin_cs);
    gpio_set_dir(pin_cs, GPIO_OUT);
    gpio_put(pin_cs, 1);  // Deselect initially
    
    gpio_init(pin_int);
    gpio_set_dir(pin_int, GPIO_IN);
    gpio_pull_up(pin_int);  // MCP2515 INT is active low
    
    // Reset the MCP2515
    reset();
    
    // Check if we can communicate (read a known register)
    uint8_t canctrl = readRegister(MCP2515_CANCTRL);
    if ((canctrl & MCP2515_MODE_MASK) != MCP2515_MODE_CONFIG) {
        return false;  // Communication failed
    }
    
    // Configure bit rate
    configureBitRate(bitRate);
    
    // Clear interrupt flags
    writeRegister(MCP2515_CANINTF, 0x00);
    
    // Enable interrupts for RX and TX
    writeRegister(MCP2515_CANINTE, MCP2515_INT_RX0IF | MCP2515_INT_RX1IF | 
                                   MCP2515_INT_TX0IF | MCP2515_INT_TX1IF | 
                                   MCP2515_INT_TX2IF | MCP2515_INT_ERRIF);
    
    // Set normal mode
    return setNormalMode();
}

bool MCP2515::setBitRate(CANBitRate bitRate) {
    if (!setMode(MCP2515_MODE_CONFIG)) {
        return false;
    }
    
    configureBitRate(bitRate);
    
    return setNormalMode();
}

bool MCP2515::setNormalMode() {
    return setMode(MCP2515_MODE_NORMAL);
}

bool MCP2515::setLoopbackMode() {
    return setMode(MCP2515_MODE_LOOPBACK);
}

bool MCP2515::setListenOnlyMode() {
    return setMode(MCP2515_MODE_LISTENONLY);
}

bool MCP2515::setSleepMode() {
    return setMode(MCP2515_MODE_SLEEP);
}

bool MCP2515::sendFrame(const CANFrame& frame) {
    // Check if TX buffer 0 is available
    uint8_t status = readRegister(MCP2515_TXB0CTRL);
    if (status & 0x08) {  // TXREQ bit set, buffer busy
        return false;
    }
    
    // Load TX buffer 0
    selectChip();
    spiTransfer(MCP2515_LOAD_TX_BUFFER | 0x00);  // TX buffer 0
    
    // Send ID
    if (frame.extended) {
        // Extended frame (29-bit ID)
        spiTransfer((uint8_t)(frame.id >> 21));  // SIDH
        spiTransfer((uint8_t)(((frame.id >> 13) & 0xE0) | 0x08 | ((frame.id >> 16) & 0x03)));  // SIDL
        spiTransfer((uint8_t)(frame.id >> 8));   // EID8
        spiTransfer((uint8_t)(frame.id));        // EID0
    } else {
        // Standard frame (11-bit ID)
        spiTransfer((uint8_t)(frame.id >> 3));   // SIDH
        spiTransfer((uint8_t)(frame.id << 5));   // SIDL
        spiTransfer(0x00);  // EID8
        spiTransfer(0x00);  // EID0
    }
    
    // Send DLC and RTR
    uint8_t dlc = frame.dlc & 0x0F;
    if (frame.remote) dlc |= 0x40;  // Set RTR bit
    spiTransfer(dlc);
    
    // Send data
    for (uint8_t i = 0; i < frame.dlc && i < 8; i++) {
        spiTransfer(frame.data[i]);
    }
    
    deselectChip();
    
    // Request transmission
    selectChip();
    spiTransfer(MCP2515_RTS | 0x01);  // RTS for TX buffer 0
    deselectChip();
    
    return true;
}

bool MCP2515::sendFrame(uint32_t id, uint8_t* data, uint8_t dlc, bool extended) {
    CANFrame frame;
    frame.id = id;
    frame.dlc = dlc;
    frame.extended = extended;
    frame.remote = false;
    
    if (data && dlc > 0) {
        memcpy(frame.data, data, dlc > 8 ? 8 : dlc);
    }
    
    return sendFrame(frame);
}

bool MCP2515::readFrame(CANFrame& frame) {
    uint8_t status = getStatus();
    
    uint8_t buffer_offset;
    if (status & 0x01) {        // Message in RX buffer 0
        buffer_offset = 0x00;
    } else if (status & 0x02) { // Message in RX buffer 1
        buffer_offset = 0x04;
    } else {
        return false;  // No message available
    }
    
    // Read RX buffer
    selectChip();
    spiTransfer(MCP2515_READ_RX_BUFFER | buffer_offset);
    
    // Read ID
    uint8_t sidh = spiTransfer(0x00);
    uint8_t sidl = spiTransfer(0x00);
    uint8_t eid8 = spiTransfer(0x00);
    uint8_t eid0 = spiTransfer(0x00);
    
    // Check if extended frame
    if (sidl & 0x08) {
        // Extended frame
        frame.extended = true;
        frame.id = ((uint32_t)sidh << 21) | 
                   (((uint32_t)(sidl & 0xE0)) << 13) | 
                   (((uint32_t)(sidl & 0x03)) << 16) |
                   ((uint32_t)eid8 << 8) | 
                   eid0;
    } else {
        // Standard frame
        frame.extended = false;
        frame.id = ((uint32_t)sidh << 3) | (sidl >> 5);
    }
    
    // Read DLC and RTR
    uint8_t dlc_rtr = spiTransfer(0x00);
    frame.dlc = dlc_rtr & 0x0F;
    frame.remote = (dlc_rtr & 0x40) != 0;
    
    // Read data
    for (uint8_t i = 0; i < frame.dlc && i < 8; i++) {
        frame.data[i] = spiTransfer(0x00);
    }
    
    deselectChip();
    
    return true;
}

bool MCP2515::frameAvailable() {
    uint8_t status = getStatus();
    return (status & 0x03) != 0;  // Check if either RX buffer has data
}

uint8_t MCP2515::getStatus() {
    selectChip();
    spiTransfer(MCP2515_READ_STATUS);
    uint8_t status = spiTransfer(0x00);
    deselectChip();
    return status;
}

uint8_t MCP2515::getInterruptFlags() {
    return readRegister(MCP2515_CANINTF);
}

void MCP2515::clearInterruptFlags(uint8_t flags) {
    modifyRegister(MCP2515_CANINTF, flags, 0x00);
}

uint8_t MCP2515::getErrorFlags() {
    return readRegister(MCP2515_EFLG);
}

uint8_t MCP2515::getTxErrorCount() {
    return readRegister(MCP2515_TEC);
}

uint8_t MCP2515::getRxErrorCount() {
    return readRegister(MCP2515_REC);
}

bool MCP2515::hasInterrupt() {
    return gpio_get(pin_int) == 0;  // INT pin is active low
}

void MCP2515::enableInterrupts(uint8_t interrupts) {
    modifyRegister(MCP2515_CANINTE, interrupts, interrupts);
}

void MCP2515::disableInterrupts(uint8_t interrupts) {
    modifyRegister(MCP2515_CANINTE, interrupts, 0x00);
}

bool MCP2515::setFilter(uint8_t filter_num, uint32_t filter_id, bool extended) {
    if (filter_num > 5) return false;
    
    if (!setMode(MCP2515_MODE_CONFIG)) {
        return false;
    }
    
    uint8_t base_reg = 0x00;
    switch (filter_num) {
        case 0: base_reg = 0x00; break;  // RXF0
        case 1: base_reg = 0x04; break;  // RXF1
        case 2: base_reg = 0x08; break;  // RXF2
        case 3: base_reg = 0x10; break;  // RXF3
        case 4: base_reg = 0x14; break;  // RXF4
        case 5: base_reg = 0x18; break;  // RXF5
    }
    
    if (extended) {
        writeRegister(base_reg, (uint8_t)(filter_id >> 21));
        writeRegister(base_reg + 1, (uint8_t)(((filter_id >> 13) & 0xE0) | 0x08 | ((filter_id >> 16) & 0x03)));
        writeRegister(base_reg + 2, (uint8_t)(filter_id >> 8));
        writeRegister(base_reg + 3, (uint8_t)(filter_id));
    } else {
        writeRegister(base_reg, (uint8_t)(filter_id >> 3));
        writeRegister(base_reg + 1, (uint8_t)(filter_id << 5));
        writeRegister(base_reg + 2, 0x00);
        writeRegister(base_reg + 3, 0x00);
    }
    
    return setNormalMode();
}

bool MCP2515::setMask(uint8_t mask_num, uint32_t mask_value, bool extended) {
    if (mask_num > 1) return false;
    
    if (!setMode(MCP2515_MODE_CONFIG)) {
        return false;
    }
    
    uint8_t base_reg = (mask_num == 0) ? 0x20 : 0x24;  // RXM0 or RXM1
    
    if (extended) {
        writeRegister(base_reg, (uint8_t)(mask_value >> 21));
        writeRegister(base_reg + 1, (uint8_t)(((mask_value >> 13) & 0xE0) | 0x08 | ((mask_value >> 16) & 0x03)));
        writeRegister(base_reg + 2, (uint8_t)(mask_value >> 8));
        writeRegister(base_reg + 3, (uint8_t)(mask_value));
    } else {
        writeRegister(base_reg, (uint8_t)(mask_value >> 3));
        writeRegister(base_reg + 1, (uint8_t)(mask_value << 5));
        writeRegister(base_reg + 2, 0x00);
        writeRegister(base_reg + 3, 0x00);
    }
    
    return setNormalMode();
}
