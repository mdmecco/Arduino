# Yabl [![CircleCI](https://circleci.com/gh/yergin/Yabl.svg?style=svg&circle-token=1b0a9ea68cca27109b3fce1641529184d060c373)](https://circleci.com/gh/yergin/Yabl)
Yet Another Button Library (for Arduino).

This library augments the popular debounce library [Bounce2](https://github.com/thomasfredericks/Bounce2) with the ability to set callbacks or easily check for the following button events:
* Press
* Release
* Hold
* Tap
* Double-tap

## Setting up and reading the button state
Setting up the button starts off the same way as setting up with [Bounce2](https://github.com/thomasfredericks/Bounce2). Here is how you would set up a button on pin 6, set the pin mode to pull-up and check whether it is initially pressed down:
```c++
#include <Yabl.h>

Button button;

void setup() {
  button.attach(6, INPUT_PULLUP);
  
  if (button.down()) {
    // button is down while switching on, prompt user to perform a factory reset
    factoryResetPrompt();
  }
}
```
`Button` assumes the switch is normally-open and connected to ground which means pressing it will cause the pin to go `LOW`. This is the default _inverted_ mode. If, on the other hand, the pin is set up to go `HIGH` when the button is pressed, call `button.inverted(false)` in `setup()`.

## Using callbacks
The following code snippet sets up a callback for button press and release events:
```c++
void setup() {
  button.attach(6, INPUT_PULLUP);
  button.callback(onButtonPress, PRESS);
  button.callback(onButtonRelease, RELEASE);
}

void onButtonPress() {
  // ...
}

void onButtonRelease() {
  // ...
}
```
Callbacks are triggered when the button's state is updated by calling update() in the main loop:
```c++
void loop() {
  button.update();
  // ...
}
```
Callbacks can be also be added for the events `SHORT_RELEASE`, `LONG_RELEASE`, `HOLD`, `SINGLE_TAP` and `DOUBLE_TAP`. In fact, sometimes you may want to have a single callback handle all events:
```c++
void setup() {
  button.attach(6, INPUT_PULLUP);
  button.callback(onButtonEvent);
}

void onButtonEvent(const EventInfo& info) {
  switch (info.event) {
    case SINGLE_TAP: /* ... */ break;
    case DOUBLE_TAP: /* ... */ break;
    case HOLD: /* ... */ break;
  }
}
```

## Checking for events
An alternative to using callbacks is to check the button for activity after each `update()`:
```c++
Button button;

void setup() {
  button.attach(6, INPUT_PULLUP);
}

void loop() {
  if (button.update()) {
    // an event was triggered
    if (button.triggered(SINGLE_TAP))
      // ...
    else if (button.triggered(DOUBLE_TAP))
      // ...
    else if (button.triggered(HOLD))
      // ...
  }
}
```
`Button` also has two convenience functions for checking if a button was pressed or released:
```c++
void loop() {
  button.update();
  if (button.pressed())
    digitalWrite(LED_BUILTIN, HIGH);
  else if (button.released())
    digitalWrite(LED_BUILTIN, LOW);
}
```
## License
This code is available under the [MIT License](http://opensource.org/licenses/mit-license.php).
