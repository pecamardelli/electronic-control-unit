#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/watchdog.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "core/GC9A01.h"

// GC9A01 Display pin definitions
#define DISPLAY_SPI_PORT spi0
#define DISPLAY_PIN_MISO 16
#define DISPLAY_PIN_CS   17
#define DISPLAY_PIN_SCK  18
#define DISPLAY_PIN_MOSI 19
#define DISPLAY_PIN_DC   20
#define DISPLAY_PIN_RST  21
#define DISPLAY_PIN_BL   22  // Backlight control

// I2C defines (for future sensors)
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

// UART defines
#define UART_ID uart1
#define BAUD_RATE 115200
#define UART_TX_PIN 4
#define UART_RX_PIN 5

// Create display object
GC9A01 display(DISPLAY_SPI_PORT, DISPLAY_PIN_CS, DISPLAY_PIN_DC, DISPLAY_PIN_RST, DISPLAY_PIN_BL);

void demo_basic_graphics() {
    printf("Running basic graphics demo...\n");
    
    // Clear screen
    display.fillScreen(BLACK);
    
    // Draw some basic shapes
    display.drawCircle(120, 120, 100, WHITE);
    display.drawCircle(120, 120, 80, RED);
    display.drawCircle(120, 120, 60, GREEN);
    display.drawCircle(120, 120, 40, BLUE);
    
    // Draw some rectangles
    display.drawRect(50, 50, 60, 40, YELLOW);
    display.fillRect(130, 50, 60, 40, CYAN);
    
    // Draw lines
    display.drawLine(0, 0, 239, 239, MAGENTA);
    display.drawLine(0, 239, 239, 0, ORANGE);
    
    // Add some text
    display.print(60, 100, "GC9A01", WHITE, BLACK, &Font24);
    display.print(60, 130, "DISPLAY", WHITE, BLACK, &Font20);
    display.print(80, 160, "TEST", WHITE, BLACK, &Font16);
    
    sleep_ms(3000);
}

void demo_gauge() {
    printf("Running gauge demo...\n");
    
    display.fillScreen(BLACK);
    
    // Draw gauge title
    display.print(80, 20, "RPM GAUGE", WHITE, BLACK, &Font20);
    
    // Animate a gauge from 0 to max and back
    for (int cycle = 0; cycle < 3; cycle++) {
        // Ramp up
        for (float rpm = 0; rpm <= 8000; rpm += 100) {
            display.fillCircle(120, 140, 95, BLACK); // Clear gauge area
            display.drawGauge(120, 140, 90, rpm, 0, 8000, RED, WHITE);
            
            // Display RPM value
            display.fillRect(80, 200, 80, 20, BLACK);
            display.printf(85, 205, WHITE, BLACK, &Font12, "%.0f RPM", rpm);
            
            sleep_ms(50);
        }
        
        // Ramp down
        for (float rpm = 8000; rpm >= 0; rpm -= 100) {
            display.fillCircle(120, 140, 95, BLACK); // Clear gauge area
            display.drawGauge(120, 140, 90, rpm, 0, 8000, RED, WHITE);
            
            // Display RPM value
            display.fillRect(80, 200, 80, 20, BLACK);
            display.printf(85, 205, WHITE, BLACK, &Font12, "%.0f RPM", rpm);
            
            sleep_ms(50);
        }
    }
    
    sleep_ms(2000);
}

void demo_dashboard() {
    printf("Running dashboard demo...\n");
    
    display.fillScreen(BLACK);
    
    // Title
    display.print(60, 10, "DASHBOARD", WHITE, BLACK, &Font24);
    
    // RPM Gauge (center)
    display.drawGauge(120, 120, 70, 3500, 0, 8000, RED, WHITE);
    display.print(105, 85, "RPM", WHITE, BLACK, &Font12);
    display.print(100, 180, "3500", WHITE, BLACK, &Font12);
    
    // Temperature gauge (top left)
    display.drawGauge(60, 60, 35, 85, 0, 120, BLUE, WHITE);
    display.print(45, 25, "TEMP", WHITE, BLACK, &Font8);
    display.print(50, 95, "85C", WHITE, BLACK, &Font8);
    
    // Fuel gauge (top right)
    display.drawGauge(180, 60, 35, 75, 0, 100, GREEN, WHITE);
    display.print(165, 25, "FUEL", WHITE, BLACK, &Font8);
    display.print(170, 95, "75%", WHITE, BLACK, &Font8);
    
    // Speed indicator (bottom)
    display.fillRect(80, 200, 80, 30, CYAN);
    display.print(95, 210, "120 KMH", BLACK, CYAN, &Font12);
    
    // Warning lights
    display.fillCircle(20, 200, 8, RED);
    display.print(5, 215, "OIL", WHITE, BLACK, &Font8);
    
    display.fillCircle(220, 200, 8, GREEN);
    display.print(205, 215, "BAT", WHITE, BLACK, &Font8);
    
    sleep_ms(5000);
}

int main() {
    stdio_init_all();
    printf("Digital Gauge with GC9A01 starting...\n");

    // SPI initialization for display
    spi_init(DISPLAY_SPI_PORT, 10*1000*1000);  // 10MHz for display
    gpio_set_function(DISPLAY_PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(DISPLAY_PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(DISPLAY_PIN_MOSI, GPIO_FUNC_SPI);
    
    // I2C Initialization for future sensors
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Initialize display
    printf("Initializing GC9A01 display...\n");
    display.init();
    printf("Display initialized!\n");
    
    // Show startup screen
    display.fillScreen(BLACK);
    display.print(60, 100, "DIGITAL", WHITE, BLACK, &Font24);
    display.print(70, 130, "GAUGE", WHITE, BLACK, &Font24);
    display.print(30, 160, "INITIALIZING", WHITE, BLACK, &Font12);
    
    // Add some loading dots
    for (int i = 0; i < 5; i++) {
        display.fillCircle(80 + i * 20, 190, 3, GREEN);
        sleep_ms(200);
    }
    
    sleep_ms(1000);

    // UART setup for debugging
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_puts(UART_ID, "GC9A01 Display Test Ready!\n");
    
    printf("Starting demo sequence...\n");
    
    // Run demos in sequence
    while (true) {
        demo_basic_graphics();
        demo_gauge();
        demo_dashboard();
        
        // Show "Demo Complete" message
        display.fillScreen(BLACK);
        display.print(50, 100, "DEMO", WHITE, BLACK, &LiberationSansNarrow_Bold36);
        display.print(30, 140, "COMPLETE", WHITE, BLACK, &LiberationSansNarrow_Bold28);
        display.print(60, 200, "Restarting...", GREEN, BLACK, &Font16);
        sleep_ms(3000);
    }
}
