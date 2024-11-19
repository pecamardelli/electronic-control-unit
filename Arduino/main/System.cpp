#include "System.h"

System::System(/* args */) { loadSystemData(); }

System::~System() {}

int System::getFreeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char *>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else   // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

void System::loadSystemData() {
  // LOAD SYSTEM DATA
  EEPROM.get(0, systemData);

  if (systemData.hostname[0] == 0xff) strcpy(systemData.hostname, "arduino");
};

void System::saveSystemData() { EEPROM.put(0, systemData); }

sys_data System::getSystemData() { return systemData; }

void System::updateSystemData(sys_data updatedData) {
  systemData = updatedData;
  saveSystemData();
}

bool System::setSystemName(String name) {
  if (name.length() > MAX_HOSTNAME_CHARS) {
    Serial.print(F("Name too long! Max length is "));
    Serial.println(MAX_HOSTNAME_CHARS);
    return false;
  }

  name.toCharArray(systemData.hostname, name.length() + 1);
  saveSystemData();

  return true;
}

void System::shutdown() { digitalWrite(RELAY_PIN, HIGH); }

/**************************************************************************/
/*!
    @brief  Executes a user command.
    @param  args Array of Strings representing the command itself and its
   arguments.
*/
/**************************************************************************/
EXEC_STATUSES System::exec(String args[]) {
  if (args[0].equals("hostname")) {
    if (args[1].length() > 0) {
      if (setSystemName(args[1])) {
        return COMMAND_SUCCESSFUL;
      } else {
        return BAD_COMMAND;
      }

    } else {
      Serial.println(systemData.hostname);
      return COMMAND_SUCCESSFUL;
    }
  } else if (args[0].equals("debug")) {
    if (args[1].equals("readings")) {
      debugMode = READINGS;
      return COMMAND_SUCCESSFUL;
    } else if (args[1].equals("temp")) {
      debugMode = TEMP;
      return COMMAND_SUCCESSFUL;
    } else if (args[1].equals("disable")) {
      debugMode = DISABLED;
      return COMMAND_SUCCESSFUL;
    }
  }

  return NO_COMMAND;
}