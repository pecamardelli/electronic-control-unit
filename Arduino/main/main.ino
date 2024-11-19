#include "header.h"

System sys;
SmallDisplay smallDisplay;
Clock clock;
Commander commander;
CurrentSensor currentSensor;
Flowmeter flowmeter;
TempSensor tempSensor;
VoltSensor voltSensor;
SDCard sdCard;
CoolantTempSensor coolantTempSensor;
TempGauge tempGauge;
CircularDisplay circularDisplay;

void setup() {
  Serial.begin(9600);
  Serial.println(sys.systemData.hostname);

  // Relay that commands direct power input.
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  smallDisplay.begin();
  smallDisplay.drawTorinoLogo();
  clock.begin();
  tempSensor.begin();
  // sdCard.begin();
  circularDisplay.setup();
}

void loop() {
  if (Serial.available()) {
    String input = commander.getInput();
    commander.exec(input);
  }

  readings values = tempSensor.getReadings();

  // SMALL DISPLAY
  // smallDisplay.display(clock.getFullDate(), clock.getTime(),
  //                      String(round(values.temp)) + "c",
  //                      String(round(values.humidity)) + "%");

  volts = voltSensor.getMeasure();
  ignition = volts > 10;
  amperes = currentSensor.getMeasure();
  coolantTemp = coolantTempSensor.getMeasure();
  tempGauge.setTemperature(coolantTemp);
  tempGauge.loop();
  _pulses = flowmeter.checkPulse();
  circularDisplay.loop();

  if (debugMode == READINGS) {
    Serial.print("VOLTS: ");
    Serial.print(volts);
    Serial.print(" - AMPS: ");
    Serial.print(amperes);
    Serial.print(" - TEMP: ");
    Serial.print(coolantTemp);
    Serial.print(" - PULSES: ");
    Serial.print(_pulses);
    Serial.print(" - IGN: ");
    Serial.println(ignition);
  }
}
