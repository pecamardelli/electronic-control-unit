/**************************************************************************/
/*!
  @file     Commander.cpp

  @section intro Introduction

  Base library intended to set a basic command logic.

  @section license License

  Original library by Pablin defining basic commands to be sent via serial port.

  This version: MIT (see LICENSE)
*/
/**************************************************************************/

#include "Commander.h"

#include "common.h"

float average = 0;
float lastAverage = 0;

Commander::Commander(/* args */) {}

Commander::~Commander() {}

String Commander::getInput() {
  if (Serial.available() <= 0) return "";

  char c;
  String input;

  while (Serial.available() > 0) {
    c = Serial.read();
    // Serial.print(String(int(c)) + " ");
    //  Include letters, digits, and other allowed chars. Add more allowed
    //  characters at the definition of the specialChars array.
    if (isalpha(c) || isdigit(c) || charAllowed(c))
      input.concat(c);
    else
      input.concat((char)0x20);

    delay(5);
  }

  return input;
};

bool Commander::charAllowed(char c) {
  for (uint8_t i = 0; i < sizeof(specialChars) / sizeof(byte); i++) {
    if ((int)c == specialChars[i]) return true;
  }
  return false;
};

void Commander::exec(String input) {
  // Tokenizing arguments
  String args[MAX_COMMAND_ARGS];
  char buffer[MAX_COMMAND_LEN];
  uint8_t index = 0;

  input.toCharArray(buffer, MAX_COMMAND_LEN);

  char *token = strtok(buffer, DELIMITER);

  while (token != NULL && index < MAX_COMMAND_ARGS) {
    args[index].concat(token);
    index++;
    token = strtok(NULL, DELIMITER);
  }

  EXEC_STATUSES result = clock.exec(args);

  if (result == COMMAND_SUCCESSFUL) {
    return;
  }

  result = sdCard.exec(args);

  if (result == COMMAND_SUCCESSFUL) {
    return;
  }

  result = sys.exec(args);

  if (result == COMMAND_SUCCESSFUL) {
    return;
  }

  result = tempGauge.exec(args);

  if (result == COMMAND_SUCCESSFUL) {
    return;
  }

  if (result == NO_COMMAND) {
    printErrorMessage(BAD_COMMAND, args[0]);
  }
}