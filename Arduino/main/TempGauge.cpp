#include "TempGauge.h"

TempGauge::TempGauge(/* args */) {
  Serial.println(F("Temperature Gauge Initiating..."));
  stepper.setSpeed(1);
}

TempGauge::~TempGauge() {}

void TempGauge::setup() {
  int angle = angleSensor.getMeasure();

  if (debugMode == TEMP) {
    Serial.print("TEMP: setting up -> ANGLE: ");
    Serial.println(angle);
  }

  if (angle < minAngle) {
    stepper.step(1);
  } else if (angle > minAngle) {
    stepper.step(-1);
  } else {
    Serial.println(F("Temperature Gauge Inited."));
    stepper.stop();
    inited = true;
  }
}

void TempGauge::loop() {
  if (!inited) return setup();
  int currentAngle = angleSensor.getMeasure();
  int nextAngle = tempToAngle(currentTemperature);
  unsigned long currentMillisBetweenSteps =
      getMillisBetweenSteps(currentAngle, nextAngle);
  if ((millis() - lastStepMillis) < currentMillisBetweenSteps)
    return stepper.stop();

  if (debugMode == TEMP) {
    Serial.print("ANGLE: ");
    Serial.print(currentAngle);
    Serial.print(" - TEMP: ");
    Serial.print(currentTemperature);
    Serial.print(" - LAST TEMP: ");
    Serial.print(lastTemperature);
    Serial.print(" - MILLIS: ");
    Serial.println(currentMillisBetweenSteps);
  }

  if (currentTemperature != lastTemperature) {
    if (currentAngle < nextAngle) {
      stepper.step(1);
      lastStepMillis = millis();
    } else if (currentAngle > nextAngle) {
      stepper.step(-1);
      lastStepMillis = millis();
    } else {
      stepper.stop();
      lastTemperature = currentTemperature;
    }
  }
}

unsigned long TempGauge::getMillisBetweenSteps(int currentAngle,
                                               int nextAngle) {
  float threshold = 6;
  int angleDifference = abs(nextAngle - currentAngle);

  if (angleDifference < threshold)
    return millisBetweenSteps * (1 - pow(angleDifference / threshold, 0.33));
  else
    return 0;
}

void TempGauge::setTemperature(float temp) {
  if (temp < minTemp)
    currentTemperature = minTemp;
  else if (temp > maxTemp)
    currentTemperature = maxTemp;
  else
    currentTemperature = temp;
}

int TempGauge::tempToAngle(float temp) {
  int lowerIndex = temp / 10 - 2;
  int upperIndex = lowerIndex + 1;
  int angleDifference =
      conversions[upperIndex].angle - conversions[lowerIndex].angle;
  int tempDifference =
      conversions[upperIndex].temp - conversions[lowerIndex].temp;

  return conversions[lowerIndex].angle + (temp - conversions[lowerIndex].temp) /
                                             tempDifference * angleDifference;
}

EXEC_STATUSES TempGauge::exec(String args[]) {
  if (args[0].equals("temp")) {
    currentTemperature = args[1].toFloat();
    return COMMAND_SUCCESSFUL;
  }

  return NO_COMMAND;
}
