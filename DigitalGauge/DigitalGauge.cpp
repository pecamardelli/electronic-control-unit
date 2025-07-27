#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/watchdog.h"
#include "hardware/gpio.h"
#include "core/GC9A01.h"
#include "assets/torino_logo_sm.h"

// GC9A01 Display pin definitions
#define DISPLAY_SPI_PORT spi0
#define DISPLAY_PIN_MISO 16
#define DISPLAY_PIN_CS   17
#define DISPLAY_PIN_SCK  18
#define DISPLAY_PIN_MOSI 19
#define DISPLAY_PIN_DC   20
#define DISPLAY_PIN_RST  21
#define DISPLAY_PIN_BL   22  // Backlight control

// Create display object
GC9A01 display(DISPLAY_SPI_PORT, DISPLAY_PIN_CS, DISPLAY_PIN_DC, DISPLAY_PIN_RST, DISPLAY_PIN_BL);

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
    
    // Show Torino logo for 5 seconds
    display.fillScreen(BLACK);
    
    // Center the 156x130 logo on the 240x240 screen
    uint16_t logo_x = (240 - 156) / 2 + 4;  // 42 pixels from left
    uint16_t logo_y = (240 - 130) / 2 - 12;  // 55 pixels from top
    
    printf("Displaying Torino logo...\n");
    display.drawImage(logo_x, logo_y, 156, 130, torino_logo_sm);
    
    // Keep logo visible for 5 seconds
    sleep_ms(5000);
    
    // Clear screen and display centered text
    // display.fillScreen(BLACK);
    // display.print(60, 110, "DIGITAL", WHITE, BLACK, &LiberationSansNarrow_Bold36);
    // display.print(75, 150, "GAUGE", WHITE, BLACK, &LiberationSansNarrow_Bold36);
    
    printf("Digital Gauge ready!\n");
    
    while (true) {
        sleep_ms(1000);
    }
}
