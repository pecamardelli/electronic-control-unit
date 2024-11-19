/**************************************************************************/
/*!
  @file     System.h

  Original library by Pablin defining all variables, structs and functions
  used by the system.

  License: MIT (see LICENSE)
*/
/**************************************************************************/

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

extern int debugMode;

#include <Arduino.h>
#include <EEPROM.h>

#include "common.h"

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char *sbrk(int incr);
#else   // __ARM__
extern char *__brkval;
#endif  // __arm__

#define MAX_HOSTNAME_CHARS 32
#define MAX_SYSTEM_DESCRIPTION_CHARS 64

#if defined(ARDUINO_AVR_MEGA2560)
#define MAX_DIGITAL_PINS 53
#define MAX_ANALOG_PINS 16
#elif defined(ARDUINO_AVR_UNO)
#define MAX_DIGITAL_PINS 13
#define MAX_ANALOG_PINS 6
#endif

typedef struct _sys_data {
  // System related
  char hostname[MAX_HOSTNAME_CHARS];
  char description[MAX_SYSTEM_DESCRIPTION_CHARS];
  // Flowmeter related
  uint16_t flowMeterPulsesPerLiter;
} sys_data;

/**************************************************************************/
/*!
    @brief  This class defines the basic properties and methods used by
            the system.
*/
/**************************************************************************/
class System {
 private:
  void loadSystemData();
  void saveSystemData();
  bool setSystemName(String);

 public:
  System(/* args */);
  ~System();
  sys_data systemData;
  int getFreeMemory();

  sys_data getSystemData();
  void updateSystemData(sys_data);
  void shutdown();
  EXEC_STATUSES exec(String[]);
};

#endif