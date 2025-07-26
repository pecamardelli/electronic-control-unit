#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/watchdog.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "core/GC9A01.h"
#include "core/PicoComm.h"

// GC9A01 Display pin definitions
#define DISPLAY_SPI_PORT spi0
#define DISPLAY_PIN_MISO 16
#define DISPLAY_PIN_CS   17
#define DISPLAY_PIN_SCK  18
#define DISPLAY_PIN_MOSI 19
#define DISPLAY_PIN_DC   20
#define DISPLAY_PIN_RST  21
#define DISPLAY_PIN_BL   22  // Backlight control

// UART defines for GPS communication
#define GPS_UART_ID uart1
#define GPS_BAUD_RATE 115200
#define GPS_UART_TX_PIN 4
#define GPS_UART_RX_PIN 5

// Create display and communication objects
GC9A01 display(DISPLAY_SPI_PORT, DISPLAY_PIN_CS, DISPLAY_PIN_DC, DISPLAY_PIN_RST, DISPLAY_PIN_BL);
PicoComm gps_comm;

int main() {
    stdio_init_all();
    printf("Digital Gauge with GC9A01 starting...\n");

    // SPI initialization for display
    spi_init(DISPLAY_SPI_PORT, 10*1000*1000);  // 10MHz for display
    gpio_set_function(DISPLAY_PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(DISPLAY_PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(DISPLAY_PIN_MOSI, GPIO_FUNC_SPI);
    
    // Initialize display
    printf("Initializing GC9A01 display...\n");
    display.init();
    printf("Display initialized!\n");
    
    // GPS Communication setup
    gps_comm.configureUART(GPS_UART_ID, GPS_UART_TX_PIN, GPS_UART_RX_PIN, GPS_BAUD_RATE);
    if (gps_comm.begin()) {
        printf("GPS communication initialized successfully\n");
    } else {
        printf("Failed to initialize GPS communication\n");
    }
    
    // Clear screen and display centered text
    display.fillScreen(BLACK);
    display.print(60, 110, "DIGITAL", WHITE, BLACK, &LiberationSansNarrow_Bold36);
    display.print(75, 150, "GAUGE", WHITE, BLACK, &LiberationSansNarrow_Bold36);
    
    printf("Digital Gauge ready!\n");
    
    // Main loop - just keep the display on
    while (true) {
        sleep_ms(1000);
    }
}
