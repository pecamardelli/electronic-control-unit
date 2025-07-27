#ifndef MCP2515_H
#define MCP2515_H

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

// MCP2515 Commands
#define MCP2515_RESET           0xC0
#define MCP2515_READ            0x03
#define MCP2515_READ_RX_BUFFER  0x90
#define MCP2515_WRITE           0x02
#define MCP2515_LOAD_TX_BUFFER  0x40
#define MCP2515_RTS             0x80
#define MCP2515_READ_STATUS     0xA0
#define MCP2515_RX_STATUS       0xB0
#define MCP2515_BIT_MODIFY      0x05

// MCP2515 Registers
#define MCP2515_CANSTAT         0x0E
#define MCP2515_CANCTRL         0x0F
#define MCP2515_TEC             0x1C
#define MCP2515_REC             0x1D
#define MCP2515_CNF3            0x28
#define MCP2515_CNF2            0x29
#define MCP2515_CNF1            0x2A
#define MCP2515_CANINTE         0x2B
#define MCP2515_CANINTF         0x2C
#define MCP2515_EFLG            0x2D

// TX Buffer registers
#define MCP2515_TXB0CTRL        0x30
#define MCP2515_TXB0SIDH        0x31
#define MCP2515_TXB0SIDL        0x32
#define MCP2515_TXB0EID8        0x33
#define MCP2515_TXB0EID0        0x34
#define MCP2515_TXB0DLC         0x35
#define MCP2515_TXB0DATA        0x36

#define MCP2515_TXB1CTRL        0x40
#define MCP2515_TXB1SIDH        0x41
#define MCP2515_TXB1SIDL        0x42
#define MCP2515_TXB1EID8        0x43
#define MCP2515_TXB1EID0        0x44
#define MCP2515_TXB1DLC         0x45
#define MCP2515_TXB1DATA        0x46

#define MCP2515_TXB2CTRL        0x50
#define MCP2515_TXB2SIDH        0x51
#define MCP2515_TXB2SIDL        0x52
#define MCP2515_TXB2EID8        0x53
#define MCP2515_TXB2EID0        0x54
#define MCP2515_TXB2DLC         0x55
#define MCP2515_TXB2DATA        0x56

// RX Buffer registers
#define MCP2515_RXB0CTRL        0x60
#define MCP2515_RXB0SIDH        0x61
#define MCP2515_RXB0SIDL        0x62
#define MCP2515_RXB0EID8        0x63
#define MCP2515_RXB0EID0        0x64
#define MCP2515_RXB0DLC         0x65
#define MCP2515_RXB0DATA        0x66

#define MCP2515_RXB1CTRL        0x70
#define MCP2515_RXB1SIDH        0x71
#define MCP2515_RXB1SIDL        0x72
#define MCP2515_RXB1EID8        0x73
#define MCP2515_RXB1EID0        0x74
#define MCP2515_RXB1DLC         0x75
#define MCP2515_RXB1DATA        0x76

// Control register bits
#define MCP2515_MODE_NORMAL     0x00
#define MCP2515_MODE_SLEEP      0x20
#define MCP2515_MODE_LOOPBACK   0x40
#define MCP2515_MODE_LISTENONLY 0x60
#define MCP2515_MODE_CONFIG     0x80
#define MCP2515_MODE_MASK       0xE0

// Interrupt flags
#define MCP2515_INT_RX0IF       0x01
#define MCP2515_INT_RX1IF       0x02
#define MCP2515_INT_TX0IF       0x04
#define MCP2515_INT_TX1IF       0x08
#define MCP2515_INT_TX2IF       0x10
#define MCP2515_INT_ERRIF       0x20
#define MCP2515_INT_WAKIF       0x40
#define MCP2515_INT_MERRF       0x80

// CAN frame structure
struct CANFrame {
    uint32_t id;
    uint8_t dlc;        // Data length code (0-8)
    uint8_t data[8];
    bool extended;      // Extended frame format
    bool remote;        // Remote transmission request
};

// CAN bit rates (for 8MHz crystal)
enum CANBitRate {
    CAN_5KBPS = 0,
    CAN_10KBPS,
    CAN_20KBPS,
    CAN_31K25BPS,
    CAN_33KBPS,
    CAN_40KBPS,
    CAN_50KBPS,
    CAN_80KBPS,
    CAN_83K3BPS,
    CAN_95KBPS,
    CAN_100KBPS,
    CAN_125KBPS,
    CAN_200KBPS,
    CAN_250KBPS,
    CAN_500KBPS,
    CAN_1000KBPS
};

class MCP2515 {
private:
    spi_inst_t* spi_port;
    uint8_t pin_cs;
    uint8_t pin_int;
    
    // Low-level SPI communication
    void selectChip();
    void deselectChip();
    uint8_t spiTransfer(uint8_t data);
    void spiWrite(uint8_t* data, size_t len);
    void spiRead(uint8_t* data, size_t len);
    
    // Register operations
    uint8_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t value);
    void modifyRegister(uint8_t reg, uint8_t mask, uint8_t value);
    
    // Internal helper functions
    void reset();
    uint8_t getMode();
    bool setMode(uint8_t mode);
    void configureBitRate(CANBitRate bitRate);
    
public:
    // Constructor
    MCP2515(spi_inst_t* spi, uint8_t cs, uint8_t interrupt_pin);
    
    // Initialization and control
    bool init(CANBitRate bitRate = CAN_500KBPS);
    bool setBitRate(CANBitRate bitRate);
    bool setNormalMode();
    bool setLoopbackMode();
    bool setListenOnlyMode();
    bool setSleepMode();
    
    // Frame transmission
    bool sendFrame(const CANFrame& frame);
    bool sendFrame(uint32_t id, uint8_t* data, uint8_t dlc, bool extended = false);
    
    // Frame reception
    bool readFrame(CANFrame& frame);
    bool frameAvailable();
    
    // Status and diagnostics
    uint8_t getStatus();
    uint8_t getInterruptFlags();
    void clearInterruptFlags(uint8_t flags = 0xFF);
    uint8_t getErrorFlags();
    uint8_t getTxErrorCount();
    uint8_t getRxErrorCount();
    
    // Interrupt handling
    bool hasInterrupt();
    void enableInterrupts(uint8_t interrupts);
    void disableInterrupts(uint8_t interrupts = 0xFF);
    
    // Filters and masks (for advanced filtering)
    bool setFilter(uint8_t filter_num, uint32_t filter_id, bool extended = false);
    bool setMask(uint8_t mask_num, uint32_t mask_value, bool extended = false);
};

#endif // MCP2515_H
