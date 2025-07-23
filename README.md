# Electronic Projects

This repository contains some of my automotive electronics projects. Some of them are abandoned and unfinished experiments.

## Project Structure

### Arduino
Initially, I started with Arduino, but the lack of processing power soon became evident. I enjoyed experimenting with some demanding operations on an Arduino Mega, but after experiencing several frustrations, I looked for more powerful hardware.

Here are some of the frustrations I faced:

#### Main Controller (`Arduino/main/`)
- **Purpose**: Primary vehicle monitoring system
- **Features**: 
  - Temperature and humidity sensing
  - Voltage and current monitoring
  - Coolant temperature measurement
  - Flow meter integration
  - Small display interface
  - SD card data logging
  - GPS tracking
- **Hardware**: Arduino with various sensors and displays

#### Motor Controller (`Arduino/motor_controller/`)
- **Purpose**: DC motor control using AF Motor shield
- **Features**: Basic forward motor control with speed adjustment

#### GPS Module (`Arduino/gps/`)
- **Purpose**: Standalone GPS tracking functionality. Never got it to work...

### ECU - Raspberry Pi 3 (`ECU/`)

The main problem with this approach was the physical size of the Pi and all its peripherals. The boot time was also a significant weakness. Nevertheless, there are some interesting features implemented here. Check them out!

- **Purpose**: Advanced electronic control unit with real-time processing
- **Features**:
  - BCM2835 library integration for GPIO control
  - Shared memory communication between processes
  - I2C sensor communication
  - Digital gauge displays
  - Temperature monitoring
  - Voltage sensing
  - Configuration management via INI files
- **Build**: CMake-based build system
- **Dependencies**: BCM2835 library (see ECU/README.md for installation)

### Speedometer - Raspberry Pi Pico 2 (`Speedometer/`)

Finally, some success! This project actually works! I finally built something functional! Now I know the feeling of Doc Brown!

- **Purpose**: Dedicated speedometer and odometer system for my 1979 IKA Torino (Argentine car based on the 1964 Rambler American 440)
- **Features**:
  - Stepper motor gauge control
  - OLED display (SSD1306) - two of them for total and partial odometer readings and other features
  - GPS integration - I tried using an optical sensor attached to the gearbox, but obviously, it didn't work
  - Flash storage for trip data
  - Watchdog timer for reliability
  - Button interface for user interaction
  - Trip and odometer calculations
- **Build**: CMake with Pico SDK

## Development Tools

The `tools/` directory contains various utility scripts, mostly used with the Pi 3:
- DBC to JSON conversion
- SSH key deployment
- Font and icon conversion utilities
- Raspberry Pi configuration scripts
- Network setup tools

## License

See LICENSE file for details.

## Credits

Special thanks to Claude! Let's be honest - he made the GPS speedometer work! Kudos!