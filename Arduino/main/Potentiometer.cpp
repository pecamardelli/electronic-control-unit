#include "Potentiometer.h"

Potentiometer::Potentiometer() {}
Potentiometer::Potentiometer(int _pin) { pin = _pin; }

Potentiometer::~Potentiometer() {}

void Potentiometer::setPin(int _pin) { pin = _pin; }

int Potentiometer::getMeasure() {
  return analogRead(pin);
  // return angle;

  // if (angleAverageStep <= 5) {
  //   angleSum += angle;
  //   angleAverage = angleSum / angleAverageStep;
  //   if (angleAverage % 2 != 0) angleAverage++;
  //   angleAverageStep++;
  // } else {
  //   angleSum = 0;
  //   angleAverageStep = 1;
  // }
  // if (angle % 2 == 0)
  //   return angle;
  // else
  //   return angle + 1;
}