/**************************************************************************/
/*!
  @file     Flowmeter.h

  Original library by Pablin defining the logic to measure fuel consumption.

  Wire description:
   - Black: GND
   - Yellow: Signal
   - Red: VCC

  License: MIT (see LICENSE)
*/
/**************************************************************************/

#ifndef _FLOWMETER_H_
#define _FLOWMETER_H_

#include <Arduino.h>

/**************************************************************************/
/*!
    @brief  This class defines the logic to calculate and print fuel
            consumption on the screen.
*/
/**************************************************************************/
class Flowmeter {
 private:
  uint64_t pulses = 0;
  uint16_t pulsesPerLiter = 287;  // Default experimental value.
  const uint8_t pin = 38;
  int lastPinState = -1;
  int currentPinState = -1;

 public:
  Flowmeter(/* args */);
  ~Flowmeter();
  void setPulsesPerLiter(unsigned short);
  unsigned short getPulsesPerLiter();
  void setPin(unsigned char);
  unsigned char getPin();
  uint64_t checkPulse();
  void resetPulses();
};

#endif  // _FLOWMETER_H_