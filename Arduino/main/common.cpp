#include "common.h"

/**************************************************************************/
/*!
    @brief  Print a standard error message.
    @param  message The error message.
    @return nothing
*/
/**************************************************************************/
void printErrorMessage(EXEC_STATUSES type, String message)
{
    switch (type)
    {
    case BAD_COMMAND:
        Serial.print(F("Bad command: "));
        break;
    case TOO_FEW_ARGUMENTS:
        Serial.print(F("Too few arguments for "));
        break;
    case BAD_INPUT:
        Serial.print(F("Bad input for "));
    case VALUE_OUT_OF_RANGE:
        Serial.print(F("Value out of range: "));
    default:
        break;
    }

    Serial.println(message);
}

/**************************************************************************/
/*!
    @brief  Print a separator for being used between lines.
    @return nothing
*/
/**************************************************************************/
void printSeparator()
{
    Serial.println(F("----------------------------------"));
}
