#include "Flowmeter.h"

Flowmeter::Flowmeter(/* args */) { pinMode(pin, INPUT); }

Flowmeter::~Flowmeter() {}

void Flowmeter::setPulsesPerLiter(unsigned short pulses) {
  pulsesPerLiter = pulses;
}

unsigned short Flowmeter::getPulsesPerLiter() { return pulsesPerLiter; }

unsigned char Flowmeter::getPin() { return pin; }

uint64_t Flowmeter::checkPulse() {
  currentPinState = digitalRead(pin);
  if (currentPinState != lastPinState) {
    lastPinState = currentPinState;
    pulses++;
  }

  return pulses;
}

void Flowmeter::resetPulses() { pulses = 0; }
