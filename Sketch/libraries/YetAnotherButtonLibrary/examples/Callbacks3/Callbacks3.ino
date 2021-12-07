/*
 * =============================================================================
 * Yet Another Button Library (for Arduino)
 * https://github.com/yergin/Yabl
 * Copyright 2017 Gino Bollaert
 * MIT License (https://opensource.org/licenses/mit-license.php)
 * -----------------------------------------------------------------------------
 * This example demonstrates callback registration behaviors. The
 * sketch requires a button be connected to pin `BUTTON_PIN` defined below.
 * The pin is configured to pull-up meaning the button must also be connected to
 * ground.
 */

#include <Yabl.h>

#define BUTTON_PIN 5 // MCU pin connected to button

Button button;
bool buttonFlash = false;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  button.attach(BUTTON_PIN, INPUT_PULLUP); // button also connected to GND
  /*
   * There can only be one callback per event type. Each subsequent callback
   * registration replaces previous callbacks for the specified events. In the
   * code below, the first callback is registered for all events but these are
   * subsequntly replaced until it's only left registered for `SHORT_RELEASE`.
   */
  button.callback(onOtherEvent);
  button.callback(onGestureEvent, PRESS | SINGLE_TAP | DOUBLE_TAP | HOLD);
  button.callback(onButtonPress, PRESS); // this overrides `PRESS` above
  button.callback(onButtonRelease, RELEASE);
  button.noCallback(LONG_RELEASE); // check for it in `loop()` instead
  Serial.begin(9600); // initialize Serial
}

void onOtherEvent(const EventInfo& info) {
  if (info.event != SHORT_RELEASE)
    Serial.print("This event should not arrive here -> ");
    
  Serial.print(button.eventName(info.event));
  Serial.println(" [onOtherEvent]");
}

void onGestureEvent(const EventInfo& info) {
  if (info.event & ~(PRESS | SINGLE_TAP | DOUBLE_TAP | HOLD))
    Serial.print("This event should not arrive here -> ");
    
  Serial.print(button.eventName(info.event));
  Serial.println(" [onGestureEvent]");
}

void onButtonPress() {
  Serial.println("PRESS [onButtonPress]");
  digitalWrite(LED_BUILTIN, HIGH);
}

void onButtonRelease() {
  Serial.println("RELEASE [onButtonRelease]");
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  button.update();
  if (button.triggered(LONG_RELEASE)) {
    Serial.println("LONG_RELEASE [loop]");
  }
}
