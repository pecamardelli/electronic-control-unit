#include "SDCard.h"

SDCard::SDCard(/* args */) {}

SDCard::~SDCard() {}

void SDCard::begin() {
  if (!SD.begin(pin)) {
    Serial.println("SD Card initialization failed!");
  } else {
    Serial.println("SD Card initialization succeded!");
  }
}

void SDCard::info() {
  // since we're just testing if the card is working!

  if (!card.init(SPI_HALF_SPEED, pin)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println(
        "* did you change the chipSelect pin to match your shield or "
        "module?");
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  // print the type of card

  Serial.println();
  Serial.print("Card type:         ");

  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or
  // FAT32

  if (!volume.init(card)) {
    Serial.println(
        "Could not find FAT16/FAT32 partition.\nMake sure you've formatted "
        "the card");
  }

  Serial.print("Clusters:          ");
  Serial.println(volume.clusterCount());
  Serial.print("Blocks x Cluster:  ");
  Serial.println(volume.blocksPerCluster());
  Serial.print("Total Blocks:      ");
  Serial.println(volume.blocksPerCluster() * volume.clusterCount());
  Serial.println();

  // print the type and size of the first FAT-type volume

  Serial.print("Volume type is:    FAT");
  Serial.println(volume.fatType(), DEC);

  volumesize = volume.blocksPerCluster();  // clusters are collections of blocks
  volumesize *= volume.clusterCount();     // we'll have a lot of clusters
  volumesize /= 2;  // SD card blocks are always 512 bytes (2 blocks are 1KB)

  Serial.print("Volume size (Kb):  ");
  Serial.println(volumesize);
  Serial.print("Volume size (Mb):  ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Gb):  ");
  Serial.println((float)volumesize / 1024.0);
  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size

  root.ls(LS_R | LS_DATE | LS_SIZE);
  root.close();
}

EXEC_STATUSES SDCard::exec(String args[]) {
  if (args[0].equals("sdcard")) {
    if (args[1].equals("info")) {
      info();
      return COMMAND_SUCCESSFUL;
    }
  }

  return NO_COMMAND;
}