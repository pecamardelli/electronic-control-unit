#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/watchdog.h"
#include "hardware/gpio.h"
#include "core/GC9A01.h"
#include "core/MCP2515.h"
#include "core/HolleySniper.h"
#include "assets/torino_logo_sm.h"

// GC9A01 Display pin definitions (SPI0)
#define DISPLAY_SPI_PORT spi0
#define DISPLAY_PIN_MISO 16
#define DISPLAY_PIN_CS   17
#define DISPLAY_PIN_SCK  18
#define DISPLAY_PIN_MOSI 19
#define DISPLAY_PIN_DC   20
#define DISPLAY_PIN_RST  21
#define DISPLAY_PIN_BL   22  // Backlight control

// MCP2515 CAN Controller pin definitions (SPI1)
#define CAN_SPI_PORT     spi1
#define CAN_PIN_CS       5    // CS pin
#define CAN_PIN_INT      6    // Interrupt pin
// SPI1 pins: SCK=10, MOSI=11, MISO=12 (hardwired)

// Create objects
GC9A01 display(DISPLAY_SPI_PORT, DISPLAY_PIN_CS, DISPLAY_PIN_DC, DISPLAY_PIN_RST, DISPLAY_PIN_BL);
MCP2515 can_controller(CAN_SPI_PORT, CAN_PIN_CS, CAN_PIN_INT);
HolleySniper sniper(&can_controller);

int main() {
    stdio_init_all();
    printf("Digital Gauge with Holley Sniper CAN starting...\n");

    // SPI0 initialization for display (10MHz)
    spi_init(DISPLAY_SPI_PORT, 10*1000*1000);
    gpio_set_function(DISPLAY_PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(DISPLAY_PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(DISPLAY_PIN_MOSI, GPIO_FUNC_SPI);
    
    // SPI1 initialization for CAN controller (1MHz for MCP2515)
    spi_init(CAN_SPI_PORT, 1*1000*1000);
    gpio_set_function(10, GPIO_FUNC_SPI);  // SCK
    gpio_set_function(11, GPIO_FUNC_SPI);  // MOSI 
    gpio_set_function(12, GPIO_FUNC_SPI);  // MISO
    
    // Initialize display
    printf("Initializing GC9A01 display...\n");
    display.init();
    printf("Display initialized!\n");
    
    // Initialize CAN controller
    printf("Initializing MCP2515 CAN controller...\n");
    if (!can_controller.init(CAN_500KBPS)) {
        printf("ERROR: Failed to initialize CAN controller!\n");
        display.fillScreen(RED);
        display.print(20, 110, "CAN ERROR", WHITE, RED, &LiberationSansNarrow_Bold36);
        while(true) { sleep_ms(1000); }
    }
    printf("CAN controller initialized at 500kbps!\n");
    
    // Initialize Holley Sniper decoder
    printf("Initializing Holley Sniper decoder...\n");
    sniper.init();
    printf("Holley Sniper decoder ready!\n");
    
    // Show Torino logo for 5 seconds
    display.fillScreen(BLACK);
    
    // Center the 156x130 logo on the 240x240 screen
    uint16_t logo_x = (240 - 156) / 2 + 4;  // 42 pixels from left
    uint16_t logo_y = (240 - 130) / 2 - 12;  // 55 pixels from top
    
    printf("Displaying Torino logo...\n");
    display.drawImage(logo_x, logo_y, 156, 130, torino_logo_sm);
    
    // Keep logo visible for 5 seconds
    sleep_ms(5000);
    
    printf("Starting real-time engine monitoring...\n");
    
    // Variables for fuel consumption calculation
    float total_fuel_consumed_liters = 0.0f;
    uint32_t last_fuel_calc_time = to_ms_since_boot(get_absolute_time());
    
    // Main engine monitoring loop
    while (true) {
        // Process incoming CAN messages
        sniper.processCANMessages();
        
        // Clear screen for new data
        display.fillScreen(BLACK);
        
        // Get current time
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        
        // Calculate fuel consumption (integrate flow rate over time)
        if (sniper.isFuelFlowValid()) {
            float time_delta_hours = (current_time - last_fuel_calc_time) / 3600000.0f;  // Convert ms to hours
            float fuel_flow_gal_hr = sniper.convertLbHrToGalHr(sniper.getFuelFlow());
            float fuel_consumed_gal = fuel_flow_gal_hr * time_delta_hours;
            float fuel_consumed_liters = fuel_consumed_gal * 3.78541f;  // Convert gallons to liters
            total_fuel_consumed_liters += fuel_consumed_liters;
            last_fuel_calc_time = current_time;
        }
        
        // === TOP CENTER: TOTAL FUEL CONSUMPTION (LITERS) ===
        display.print(10, 10, "FUEL CONSUMED", YELLOW, BLACK, &LiberationSansNarrow_Bold16);
        if (sniper.isFuelFlowValid()) {
            char fuel_str[32];
            snprintf(fuel_str, sizeof(fuel_str), "%.2f L", total_fuel_consumed_liters);
            display.print(70, 35, fuel_str, WHITE, BLACK, &LiberationSansNarrow_Bold30);
        } else {
            display.print(85, 35, "-- L", RED, BLACK, &LiberationSansNarrow_Bold30);
        }
        
        // === LEFT SIDE: COOLANT TEMPERATURE (CELSIUS) ===
        display.print(10, 80, "COOLANT", CYAN, BLACK, &LiberationSansNarrow_Bold16);
        if (sniper.isCoolantTempValid()) {
            float temp_celsius = sniper.convertFahrenheitToCelsius(sniper.getCoolantTemp());
            char temp_str[32];
            snprintf(temp_str, sizeof(temp_str), "%.1f°C", temp_celsius);
            
            // Color coding for temperature
            uint16_t temp_color = GREEN;
            if (temp_celsius > 95.0f) temp_color = RED;       // Too hot
            else if (temp_celsius > 85.0f) temp_color = ORANGE; // Warm
            else if (temp_celsius < 70.0f) temp_color = BLUE;   // Cold
            
            display.print(10, 105, temp_str, temp_color, BLACK, &LiberationSansNarrow_Bold24);
        } else {
            display.print(10, 105, "--°C", RED, BLACK, &LiberationSansNarrow_Bold24);
        }
        
        // === BOTTOM CENTER: RPM ===
        display.print(95, 180, "RPM", YELLOW, BLACK, &LiberationSansNarrow_Bold16);
        if (sniper.isRPMValid()) {
            char rpm_str[32];
            snprintf(rpm_str, sizeof(rpm_str), "%.0f", sniper.getRPM());
            
            // Color coding for RPM
            uint16_t rpm_color = GREEN;
            float rpm = sniper.getRPM();
            if (rpm > 6500.0f) rpm_color = RED;        // Danger zone
            else if (rpm > 5500.0f) rpm_color = ORANGE; // High RPM
            else if (rpm > 4000.0f) rpm_color = YELLOW; // Medium RPM
            
            display.print(60, 205, rpm_str, rpm_color, BLACK, &LiberationSansNarrow_Bold36);
        } else {
            display.print(85, 205, "----", RED, BLACK, &LiberationSansNarrow_Bold36);
        }
        
        // === RIGHT SIDE: ADDITIONAL INFO ===
        // Current fuel flow rate
        if (sniper.isFuelFlowValid()) {
            display.print(130, 80, "FLOW", MAGENTA, BLACK, &LiberationSansNarrow_Bold16);
            char flow_str[32];
            float flow_lph = sniper.convertLbHrToGalHr(sniper.getFuelFlow()) * 3.78541f; // L/hr
            snprintf(flow_str, sizeof(flow_str), "%.1fL/h", flow_lph);
            display.print(130, 105, flow_str, WHITE, BLACK, &LiberationSansNarrow_Bold16);
        }
        
        // AFR (Air-Fuel Ratio)
        if (sniper.getEngineData().afr_valid) {
            display.print(130, 130, "AFR", GREEN, BLACK, &LiberationSansNarrow_Bold16);
            char afr_str[32];
            snprintf(afr_str, sizeof(afr_str), "%.1f", sniper.getAFR());
            display.print(130, 155, afr_str, WHITE, BLACK, &Font20);
        }
        
        // === STATUS INDICATORS ===
        // CAN communication status
        uint32_t last_update = sniper.getLastUpdateTime();
        bool can_ok = (current_time - last_update) < 2000; // 2 second timeout
        
        display.fillCircle(10, 220, 8, can_ok ? GREEN : RED);
        display.print(25, 215, "CAN", WHITE, BLACK, &LiberationSansNarrow_Bold16);
        
        // Engine running status (based on RPM > 500)
        bool engine_running = sniper.isRPMValid() && sniper.getRPM() > 500.0f;
        display.fillCircle(220, 220, 8, engine_running ? GREEN : GRAY);
        display.print(170, 215, "ENGINE", WHITE, BLACK, &LiberationSansNarrow_Bold16);
        
        // Update display
        sleep_ms(100);  // 10Hz update rate
    }
}
