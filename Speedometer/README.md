# Speedometer - Raspberry Pi Pico 2

A GPS-based speedometer and odometer system designed for my 1979 IKA Torino. This project uses a Raspberry Pi Pico 2 to control a mechanical gauge with stepper motor and dual OLED displays that replace the old mechanical odometers.

This project was born because I made a gearbox swap on my car and the speedometer became useless. I wanted to keep the original gauge, so I started this project that gave me quite a fight. Here it is! I hope it helps inspire someone else.

## Hardware Components

### Microcontroller
- **Raspberry Pi Pico 2**: Main processing unit with dual-core ARM Cortex-M33. A very powerful and affordable piece of hardware. I love it!

### Displays

This was the biggest headache of the entire solution. I bought two of these tiny displays and discovered with some disappointment that their I2C address can't be changed. Fortunately, the Pico has two I2C buses.
- **2x SSD1306 OLED Displays (128x32px)**: I2C-based displays showing odometer readings, trip information, current speed, and time. Each of these display modes can be selected by pressing the push button.
  - I2C Address: 0x3C
  - Connection: SDA/SCL pins configurable

### Motor Control
- **Stepper Motor**: Drives the original mechanical speedometer gauge
  - Control pins: GPIO 18, 19, 20, 21 (IN4, IN3, IN2, IN1)
  - Position feedback: GPIO 27 (interrupt-based zero position detection)
  - Features backlash compensation and smooth movement algorithms. The stepper has some mechanical backlash that creates a dead zone in the mechanism that moves the gauge needle. So, every time the speedometer reverses direction, a certain number of steps must be added to compensate for this play. Sorry, this is what I had to work with... If your stepper doesn't have this issue, set this compensation value to 0. Check the code!

### Sensors
- **GPS Module**: Primary speed and distance measurement
  - UART interface: GPIO 0 (TX), GPIO 1 (RX)
  - Baud rate: 9600
  - Provides: speed, position, time, satellite count
- **Speed Sensor (The initial frustration)**: Optical sensor attached to the gearbox for mechanical speed detection
  - Input pin: GPIO 28 (configurable)
  - Not currently used... I left the code out of laziness

### User Interface
- **Push Button**: User input for trip reset and mode selection
  - GPIO pin configurable with internal pull-up
  - Debounced input handling

### Storage
- **Flash Memory**: Non-volatile storage for odometer data
  - Trip data persistence across power cycles
  - Automatic periodic saving
  - Wear leveling to extend flash memory life

## Key Features

### Speed Measurement
- GPS-based speed calculation with filtering for smooth readings
- Configurable speed limits and gauge ranges
- Real-time speed display on mechanical gauge

### Odometer Functions
- **Total Odometer**: Lifetime distance tracking
- **Partial/Trip Odometer**: Resettable trip distance with up to three additional trip views beyond the partial odometer
- Persistent storage in flash memory
- Sub-kilometer precision

### Safety Features
- **Watchdog Timer**: Automatic system recovery from crashes
- **Data Validation**: GPS data integrity checking
- **Backup Systems**: Multiple data sources for reliability

### Motor Control
- Smooth stepper motor movement with acceleration/deceleration curves
- Backlash compensation for accurate needle positioning
- Zero-position calibration on startup
- Dynamic speed adjustment based on target distance

## Pin Configuration

| Component | GPIO Pin | Function |
|-----------|----------|----------|
| GPS TX | 0 | UART TX to GPS RX |
| GPS RX | 1 | UART RX from GPS TX |
| SSD1306 SDA | Configurable | I2C Data |
| SSD1306 SCL | Configurable | I2C Clock |
| Stepper IN4 | 18 | Motor control |
| Stepper IN3 | 19 | Motor control |
| Stepper IN2 | 20 | Motor control |
| Stepper IN1 | 21 | Motor control |
| Position Sensor | 27 | Zero position interrupt |
| Speed Sensor | 28 | Optional mechanical speed input |
| Button | Configurable | User input |

## Build System

- **CMake**: Cross-platform build system
- **Pico SDK**: Official Raspberry Pi Pico development framework
- **Dependencies**: Standard Pico SDK libraries (I2C, UART, GPIO, Flash)
- **Raspberry Pi Pico extension for VS Code**: It has everything you need for development

## Software Architecture

- **Modular Design**: Separate classes for each hardware component
- **Helper Libraries**: Dedicated modules for odometer calculations, display formatting, and storage management
- **Real-time Processing**: Interrupt-driven sensor handling
- **Data Persistence**: Automatic saving of critical data to flash storage

## Calibration

The system automatically calibrates the speedometer needle position on startup using the position sensor. Make sure you place all parts correctly and the speedometer should work like a charm.

### Enjoy!
