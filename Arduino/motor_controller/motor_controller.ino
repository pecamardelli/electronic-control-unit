#include <AFMotor.h>

AF_DCMotor motor(3);

void setup()
{
    // Set initial speed of the motor & stop
    motor.setSpeed(200);
    motor.run(RELEASE);
}

void loop()
{
    uint8_t i;

    // Turn on motor
    motor.run(FORWARD);

    motor.setSpeed(200);
    // Now turn off motor
    // motor.run(RELEASE);
    delay(1000);
}