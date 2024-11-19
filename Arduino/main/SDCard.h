#ifndef _SD_CARD_H_
#define _SD_CARD_H_

#include <SD.h>
#include <SPI.h>

#include "common.h"

class SDCard {
 private:
  const uint8_t pin = 53;
  Sd2Card card;
  SdVolume volume;
  SdFile root;
  uint32_t volumesize;
  /* data */

 public:
  SDCard(/* args */);
  ~SDCard();
  void begin();
  void info();
  EXEC_STATUSES exec(String[]);
};

#endif  // _SD_CARD_H_