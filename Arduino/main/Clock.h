/**************************************************************************/
/*!
  @file     Clock.h

  Original library by Pablin defining the logic for date and time resources.

  License: MIT (see LICENSE)
*/
/**************************************************************************/

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <RTClib.h>

#include "common.h"

/**************************************************************************/
/*!
    @brief  This class defines all utilities to work with dates and time.
*/
/**************************************************************************/
class Clock {
 private:
  uint16_t _year;
  uint8_t _month;
  uint8_t _day;

  uint8_t _hour;
  uint8_t _min;
  uint8_t _sec;

  DateTime now;

 public:
  Clock();
  ~Clock();

  const String days[7] = {"DOMINGO", "LUNES",   "MARTES", "MIERCOLES",
                          "JUEVES",  "VIERNES", "SABADO"};

  RTC_DS3231 RTC;

  void begin();
  EXEC_STATUSES setDateTime(String, String);
  String getDate();
  String getFullDate();
  String getTime();
  EXEC_STATUSES exec(String[]);
};

#endif  // _CLOCK_H_