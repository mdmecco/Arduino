/*
 * =============================================================================
 * Yet Another Button Library (for Arduino)
 * https://github.com/yergin/Yabl
 * Copyright 2017 Gino Bollaert
 * MIT License (https://opensource.org/licenses/mit-license.php)
 * -----------------------------------------------------------------------------
 * This example demonstrates how to use simple callbacks for different events.
 * This sketch requires a button be connected to pin `BUTTON_PIN` defined below.
 * The pin is configured to pull-up meaning the button must also be connected to
 * ground.
 */

#include <Yabl.h>

#define BUTTON_PIN 5 // MCU pin connected to button

Button button;
bool buttonFlash = false;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  button.attach(BUTTON_PIN, INPUT_PULLUP); // pin configured to pull-up mode
  button.callback(onButtonPress, PRESS);
  button.callback(onButtonRelease, RELEASE);
  button.callback(onButtonHold, HOLD | DOUBLE_TAP); // called on either event
}

void onButtonPress() {
  digitalWrite(LED_BUILTIN, HIGH);
}

void onButtonRelease() {
  digitalWrite(LED_BUILTIN, LOW);
  buttonFlash = false;
}

void onButtonHold() {
  buttonFlash = true;
}

void loop() {
  /* update both states - callbacks are called from `update()` */
  button.update();
  if (buttonFlash) {
    digitalWrite(LED_BUILTIN, millis() % 80 < 40); // Flash every 80ms
  }
}
