/*
 * =============================================================================
 * Yet Another Button Library (for Arduino)
 * https://github.com/yergin/Yabl
 * Copyright 2017 Gino Bollaert
 * MIT License (https://opensource.org/licenses/mit-license.php)
 * -----------------------------------------------------------------------------
 * This example demonstrates how to simply detect press() and release() events.
 * This sketch requires a button be connected to pin `BUTTON_PIN` defined below.
 * The pin is configured to pull-up meaning the button must also be connected to
 * ground.
 */

#include <Yabl.h>

#define BUTTON_PIN 5 // MCU pin connected to button

Button button;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  button.attach(BUTTON_PIN, INPUT_PULLUP); // button also connected to GND
}

/*
 * `pressed()` and `released()` will only return `true` if a state change
 * occured since the previous call to `update()`
 */
void loop() {
  button.update(); // check for button state changes
  if (button.pressed()) // `true` if pressed since previous `update()`
    digitalWrite(LED_BUILTIN, HIGH);
  else if (button.released()) // `true` if released since previous `update()`
    digitalWrite(LED_BUILTIN, LOW);
}
