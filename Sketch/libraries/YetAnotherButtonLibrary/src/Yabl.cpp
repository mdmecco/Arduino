/*
 * =============================================================================
 * Yet Another Button Library (for Arduino)
 * https://github.com/yergin/Yabl
 * Copyright 2017 Gino Bollaert
 * MIT License (https://opensource.org/licenses/mit-license.php)
 * -----------------------------------------------------------------------------
 */

#include "Yabl.h"
#include <Arduino.h>

namespace Yabl {

bool EventInfo::operator==(const EventInfo& other) const {
  return other.button == button && other.event == event;
}
  

bool Button::update() {
  /* store as `previousMillis()` will be updated in `Bounce::update` */
  unsigned long elasped = millis() - previousMillis();

  if (!gestureStarted()) {
    _suppressOnce = _suppressAlways; // always reset this between gestures
  }
  
  /* defered reset after a gesture is finished */
  if (_reset) {
    reset();
  }
  
  /* debounces and stores the state of the button */
  Bounce::update();

  clearEvents();

  if (pressed()) {
    triggerEvent(PRESS);
    
    /*
     * it is a second press if the button was already released in the current
     * gesture.
     */
    if (gestureIncludes(RELEASE) && elasped < _doubleTapInterval) {
      triggerEvent(DOUBLE_TAP);
    }
  }
  else if (released()) {
    /* ignore the first release if the button was down on initialization */
    if (!gestureStarted()) {
      return false;
    }
    triggerEvent(RELEASE);
      
    if (gestureIncludes(HOLD)) {
      triggerEvent(LONG_RELEASE);
      _reset = true; // end of gesture, defer reset to next `update`
    }
    else if (gestureIncludes(DOUBLE_TAP)) {
      _reset = true; // double-tap gesture ends when the button is released
      /* do not trigger short or long release events for double-taps */
    }
    else {
      if (enableHold()) {
        triggerEvent(SHORT_RELEASE);
      }

      if (!enableDoubleTap()) {
        /* no need to wait until doubleTapInterval to trigger single tap */
        triggerEvent(SINGLE_TAP);
        _reset = true; // end of gesture, defer reset to next `update`
      }
    }
  }
  else if (down()) {
    /*
     * prevent hold events from being retriggered or triggered as part of
     * double-taps
     */
    if (gestureStarted() && enableHold() && !gestureIncludes(RELEASE)) {
      if (!gestureIncludes(HOLD)) {
        if (elasped >= _holdDuration) {
          triggerEvent(HOLD);
          _nextHoldRepeat = _holdDuration + validatedHoldRepeatDelay();
        }
      }
      else if (elasped >= _nextHoldRepeat) {
        triggerEvent(HOLD_REPEAT);
        _nextHoldRepeat += _holdRepeatInterval;
      }
    }
  }
  else {
    /*
     * single-taps should only be triggered once we know a double-tap can't
     * occur
     */
    if (gestureIncludes(PRESS) && elasped >= _doubleTapInterval) {
      triggerEvent(SINGLE_TAP);
      _reset = true; // end of gesture, defer reset to next `update`
    }
  }
  
  return activity(); // return whether any events were triggered
}

void Button::reset()
{
  _currentEvents = 0;
  _gestureEvents = 0;
  _suppressOnce = 0;
  _reset = false;
}

const char* Button::eventName(Event event) const {
  switch (event) {
    case PRESS: return "PRESS";
    case RELEASE: return "RELEASE";
    case SHORT_RELEASE: return "SHORT_RELEASE";
    case SINGLE_TAP: return "SINGLE_TAP";
    case DOUBLE_TAP: return "DOUBLE_TAP";
    case HOLD: return "HOLD";
    case LONG_RELEASE: return "LONG_RELEASE";
    default: break;
  }
  return "<UNKNOWN_EVENT>";
}

void Button::wakeup() {
  previous_millis = millis(); // pretend no time has passed when sleeping
}

void Button::triggerEvent(Event event) {
  if (_suppressOnce & event) {
    return;
  }
  
  _currentEvents |= event;
  _gestureEvents |= event;
  
  Callback* cb = callback(event);
  switch (cb->type) {
    case Callback::SIMPLE: (cb->callback.simple)(); break;
    case Callback::WITH_EVENT_INFO: (cb->callback.withEventInfo)(EventInfo{*this, event}); break;
    case Callback::NONE: break;
  }
}
  
void Button::enableEvent(Event event, bool enable) {
  if (enable) {
    _suppressAlways &= ~event;    
  }
  else {
    _suppressAlways |= event;
  }
}

Button::Callback* Button::callback(Event forEvent) {
  for (int i = 0; i < EVENT_COUNT; ++i) {
    if (forEvent & (1 << i)) {
      return _callbacks + i;
    }
  }
  
  return 0;
}

void Button::callback(CallbackSimple callback, Event forEvents) {
  for (int i = 0; i < EVENT_COUNT; ++i) {
    if (forEvents & (1 << i)) {
      _callbacks[i].type = Callback::SIMPLE;
      _callbacks[i].callback.simple = callback;
    }
  }
}

void Button::callback(CallbackWithEventInfo callback, Event forEvents) {
  for (int i = 0; i < EVENT_COUNT; ++i) {
    if (forEvents & (1 << i)) {
      _callbacks[i].type = Callback::WITH_EVENT_INFO;
      _callbacks[i].callback.withEventInfo = callback;
    }
  }
}

void Button::noCallback(Event forEvents) {
  for (int i = 0; i < EVENT_COUNT; ++i) {
    if (forEvents & (1 << i)) {
      _callbacks[i].type = Callback::NONE;
    }
  }
}

} // namespace Yabl

