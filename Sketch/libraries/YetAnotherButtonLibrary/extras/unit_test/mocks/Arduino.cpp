/*
 * =============================================================================
 * Yet Another Button Library (for Arduino)
 * https://github.com/yergin/Yabl
 * Copyright 2017 Gino Bollaert
 * MIT License (https://opensource.org/licenses/mit-license.php)
 * -----------------------------------------------------------------------------
 */

#include "Arduino.h"

MockArduino* _arduino = 0;

unsigned long millis() { return _arduino ? _arduino->millis() : 0; }

void CREATE_MOCK_ARDUINO() {
  _arduino = new MockArduino;
}

void DESTROY_MOCK_ARDUINO() {
  delete _arduino;
  _arduino = 0;
}

MockArduino& ARDUINO() {
  return *_arduino;
}
