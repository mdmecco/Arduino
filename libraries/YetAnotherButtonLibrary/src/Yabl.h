/*
 * =============================================================================
 * Yet Another Button Library (for Arduino)
 * https://github.com/yergin/Yabl
 * Copyright 2017 Gino Bollaert
 * MIT License (https://opensource.org/licenses/mit-license.php)
 * -----------------------------------------------------------------------------
 */

#pragma once

#include <Bounce2.h>
#include <inttypes.h>

/*
 * -----------------------------------------------------------------------------
 * The Yabl namespace prevents name colisions with other possible Button and
 * Event classes.
 */
namespace Yabl {

/*
 * -----------------------------------------------------------------------------
 * The following `Event` type and constants describe events the `Button` class
 * detects. These events can often be combined using a bit-wise OR when passed
 * as parameters to `Button` class methods. See `Button` class method comments.
 */
typedef uint16_t Event;

/* pressed */
static constexpr Event PRESS = 0x0001;
/* released */
static constexpr Event RELEASE = 0x0002;
/* relased (short) and not pressed again before `doubleTapInterval` */
static constexpr Event SINGLE_TAP = 0x0004;
/* elased (short) and pressed again within `doubleTapInterval` */
static constexpr Event DOUBLE_TAP = 0x0008;
/* pressed and held for `holdDuration` */
static constexpr Event HOLD = 0x0010;
/* pressed and held for `holdDuration` + `holdRepeatDelay` and every
   `holdRepeatInterval` after that. */
static constexpr Event HOLD_REPEAT = 0x0020;
/* released before `holdDuration` */
static constexpr Event SHORT_RELEASE = 0x0040;
/* relased after being held for at least `holdDuration` */
static constexpr Event LONG_RELEASE = 0x0080;

/* the following events are reserved for future functionality */
static constexpr Event EVENT_RESERVED_0x0100 = 0x0100;
static constexpr Event EVENT_RESERVED_0x0200 = 0x0200;
static constexpr Event EVENT_RESERVED_0x0400 = 0x0400;
static constexpr Event EVENT_RESERVED_0x0800 = 0x0800;

/* these are additional events available to subclasses */
static constexpr Event EVENT_EXTENDED_1 = 0x1000;
static constexpr Event EVENT_EXTENDED_2 = 0x2000;
static constexpr Event EVENT_EXTENDED_3 = 0x4000;
static constexpr Event EVENT_EXTENDED_4 = 0x8000;

/* mask for all events */
static constexpr Event ALL_EVENTS = 0xFFFF;

/*
 * -----------------------------------------------------------------------------
 * The `EventInfo` struct is passed as a parameter to event callbacks specifying
 * the button and type of event triggered.
 */
class Button;

struct EventInfo {
  Button& button;
  Event event;
  bool operator==(const EventInfo& other) const;
};

/*
 * =============================================================================
 * The `Button` class is the main class and represents a physical button
 * attached to the board. It subclasses Bounce and therefore all public Bounce
 * methods such as `attach(...)` and `interval()` are available to the user of
 * this class. `read()`, `fell()` and `rose()` are also available but the
 * methods `down()`, `pressed()` and `released()` are preferred as they are more
 * meaningful when describing a button state or event.
 * 
 * Example of use:
 *
 *   Button button;
 *
 *   // Specify the pin the button is attached to using the `Bounce` method
 *   //`attach(...)`:
 *   void setup() {
 *     button.attach(6, INPUT_PULLUP); // button attached to pin 6 and ground
 *   }
 *
 *   // respond to button events in the main loop
 *   void loop() {
 *     button.update(); // update button state 
 *     if (button.pressed()) {
 *       // respond to button press here...
 *     }
 *   }
 * -----------------------------------------------------------------------------
 */
  
class Button : public Bounce
{
public:
  /*
   * There are two possibile function signitures for callbacks. See comments for
   * both `callback(...)` methods below.
   */
  typedef void (*CallbackSimple)();
  typedef void (*CallbackWithEventInfo)(const EventInfo&);
  
  Button() {}

  /*
   * `Button` can only update its state when `update()` is called. It is
   * typically called in the main loop. It is within update() that callbacks are
   * are fired. This will return `true` if an event was triggered since the
   * previous call to update.
   */
  bool update();
  
  /*
   * This resets any current events, events recorded in the previous gesture and
   * supressed events.
   */
  void reset();
  
  /*
   * This returns the name of the event for debugging purposes.
   */
  virtual const char* eventName(Event event) const;
  
  /*
   * Call `sleep()` before putting the MCU to sleep to prevent false event
   * triggers on wake-up. Example (using the Snooze library):
   *    
   *    button.sleep();
   *    Snooze.deepSleep(buttonWakeupConfig);
   *    button.wakeup();
   */
  void sleep() { reset(); }
  
  /*
   * Call `wakeup` after waking the MCU. See `sleep()` example above.
   */
  void wakeup();
  
  /*
   *`down()` returns `true` if the button is currently pressed down.
   */
  bool down() { return read() != _inverted; }
  
  /*
   * `pressed()` returns `true` if the button was pressed sometime between the
   * last two calls to update.
   */
  bool pressed() { return _inverted ? fell() : rose(); }

  /*
   * `released()` returns `true` if the button was released sometime between the
   * last two calls to update.
   */
  bool released() { return _inverted ? rose() : fell(); }
  
  /*
   * `activity()` returns `true` if any event occured sometime between the last
   * two calls to update.
   */
  bool activity() const { return _currentEvents != 0; }
  
  /*
   * `triggered(Event events)` returns `true` if any of the events `events`
   * occured sometime between the last two calls to update. `events` can either
   * be a single event or events combined with the bit-wise OR operator.
   */
  bool triggered(Event events) const { return _currentEvents & events; }

  /*
   * A gesture is a sequence of related events that can be considered a single
   * action. For example, a `double-tap` is a button press followed by a short
   * release and then another press very soon after. `gestureStarted()` returns
   * `true` if a gesture has been started and is not finished.
   */
  bool gestureStarted() const { return _gestureEvents != 0; }
  
  /*
   * `gestureIncludes(Event events)` returns `true` if any of the events
   * `events` occured since the start of the current gesture. `events` can
   * either be a single event or events combined with the bit-wise OR operator.
   */
  bool gestureIncludes(Event events) const { return _gestureEvents & events; }
  
  /*
   * `suppressOnce(Event events)` stops any events of the type specified from
   * being triggered within the current gesture or next gesture if a gesture has
   * not being started. This can be useful for ignoring release events if a
   * a press event caused a change in the program's view and the new view should
   * only respond to button events once the current gesture is completed.
   */  
  void suppressOnce(Event events) { _suppressOnce |= events; }
  
  /*
   * When `enableDoubleTap` is `true`, a DOUBLE_TAP event will be triggered if
   * the button is pressed twice with the second press occuring within
   * `doubleTapInterval` after the first release. When `enableDoubleTap` is
   * `false`, SINGLE_TAP events occur immediately on release rather than waiting
   * on the possibility of a double-tap occuring. This is `true` by default.
   */
  void enableDoubleTap(bool enable) { enableEvent(DOUBLE_TAP, enable); }
  bool enableDoubleTap() const { return eventEnabled(DOUBLE_TAP); }
  
  /*
   * `doubleTapInterval` is the maximum time interval between a first release
   * and the second press to trigger a DOUBLE_TAP event.
   */
  void doubleTapInterval(unsigned short ms) { _doubleTapInterval = ms; }
  unsigned short doubleTapInterval() const { return _doubleTapInterval; }
  
  /*
   * `enableHold` sets whether HOLD, SHORT_RELEASE and LONG_RELEASE events can
   * can be triggered. When `false`, the button can be held for any length of
   * time to trigger SINGLE_TAP or DOUBLE_TAP events. This is `true` by default.
   */
  void enableHold(bool enable) { enableEvent(HOLD, enable); }
  bool enableHold() const { return eventEnabled(HOLD); }
  
  /*
   * `holdDuration` is the minimum time a button must be held down to trigger
   * a HOLD event.
   */
  void holdDuration(unsigned short ms) { _holdDuration = ms; }
  unsigned short holdDuration() const { return _holdDuration; }
  
  /*
   * `holdRepeatDelay` is the interval between the HOLD event and the first
   * HOLD_REPEAT event.
   */
  void holdRepeatDelay(unsigned short ms) { _holdRepeatDelay = ms; }
  unsigned short holdRepeatDelay() const { return _holdRepeatDelay; }

  /*
   * `holdRepeatInterval` is the minimum time a button must be held down to
   * trigger a HOLD event.
   */
  void holdRepeatInterval(unsigned short ms) { _holdRepeatInterval = ms; }
  unsigned short holdRepeatInterval() const { return _holdRepeatInterval; }

  /*
   * `inverted` is `true` by default signifying that the pin reads `HIGH` when
   * the button is released and `LOW` when it is pressed down. This supports
   * the typical button setup where a normally-open button is connected between
   * an MCU pin set to pull-up and ground. Set `inverted` to `false` if the pin
   * instead reads `LOW` when the button is released.
   */
  void inverted(bool inverted) { _inverted = inverted; }
  bool inverted() const { return _inverted; }
  
  /*
   * Callbacks with signiture `CallbackSimple` have no arguments and are
   * typically used when there is one button and seperate callbacks per-event.
   * There can only be one callback per event however multiple events can be
   * linked to the same callback by combining events using the bit-wise OR
   * operator. For example:
   *
   *   void setup() {
   *     ...
   *     button.callback(onButtonTap, SINGLE_TAP | HOLD);
   *     button.callback(onButtonDoubleTap, DOUBLE_TAP);
   *   }
   *   
   *   void onButtonTap() {
   *     // respond to button tap here...
   *   }
   *
   *   void onButtonDoubleTap() {
   *     // respond to button double-tap here...
   *   }
   *   
   *   void loop() {
   *     button.update();
   *     ...
   *   }
   */
  void callback(CallbackSimple callback, Event forEvents);
  
  /*
   * A callback with signiture `CallbackWithEventInfo` has one argument which
   * holds information on which button the events occured on and the type of
   * event triggered. There can only be one callback per event however multiple
   * events can be linked to the same callback by combining events using the
   * bit-wise OR operator. The `forEvents` parameter is optional and set to
   * set to trigger the callback on all events if left unspecified. This
   * callback is useful for dealing with different buttons and event types in a
   * dynamic way. For example:
   
   *   Button buttonA;
   *   Button buttonB;
   *
   *   void setup() {
   *     ...
   *     buttonA.callback(onButtonEvent);
   *     buttonB.callback(onButtonEvent);
   *   }
   *   
   *   void onButtonEvent(const EventInfo& info) {
   *     Serial.print(info.button == buttonA ?  "Button A: " : "Button B: ");
   *     Serial.println(info.button.eventName(info.event));
   *   }
   *   
   *   void loop() {
   *     button.update();
   *     ...
   *   }
   */
  void callback(CallbackWithEventInfo callback, Event forEvents = ALL_EVENTS);

  /*
   * Deregister callbacks for events `forEvents`.
   */
  void noCallback(Event forEvents = ALL_EVENTS);
  
  /*
   * See if two button variables are the same instance. See example above.
   */
  bool operator==(const Button& other) const { return this == &other; }

protected:
  /*
   * Sets the current event, adds it to the current gesture and fires a callback
   */
  void triggerEvent(Event event);
  
  /*
   * This `Bounce` variable holds the time since the last press or release
   * occured.
   */
  unsigned long previousMillis() const { return previous_millis; }
  
  /*
   * The maximum number of event types. See comments on the `Event` typedef
   * above.
   */
  static constexpr int EVENT_COUNT = sizeof(Event) * 8;
  
private:
  /*
   * The folowing struct stores a callback of type `CallbackSimple` or
   * `CallbackWithEventInfo`.
   */
  struct Callback {
    Callback() : type(NONE) {}
    enum {
      NONE = 0,
      SIMPLE,
      WITH_EVENT_INFO
    } type;
    union {
      CallbackSimple simple;
      CallbackWithEventInfo withEventInfo;
    } callback;
  };
  
  /*
   * Old events are cleared at the start of every `update`.
   */
  void clearEvents() { _currentEvents = 0; }
  
  /*
   * Enable and disable certain type of events.
   */
  void enableEvent(Event event, bool enable);
  bool eventEnabled(Event event) const { return !(_suppressAlways & event); }

  /*
   * Finds the pointer to the callback data associated with the event
   * `forEvent`.
   */
  Callback* callback(Event forEvent);

  /*
   * This method insures the interval between a `HOLD` event and `HOLD_REPEAT`
   * event is never smaller than the intetval between `HOLD_REPEAT` events.
   */
  unsigned int validatedHoldRepeatDelay() const {
    return _holdRepeatDelay < _holdRepeatInterval ? _holdRepeatInterval :
                                                    _holdRepeatDelay;
  }

  bool _inverted = true; // is the pin `LOW` when the button is down
  unsigned short _doubleTapInterval = 150;
  unsigned short _holdDuration = 400;
  unsigned short _holdRepeatDelay = 400; // after hold duration
  unsigned short _holdRepeatInterval = 150; // after hold repeat delay
  unsigned long _nextHoldRepeat = 0;
  bool _reset = false; // used to delay a reset until the following `update`
  Event _currentEvents = 0; // events triggered between updates
  Event _gestureEvents = 0; // events triggered since the start of a gesture
  Event _suppressOnce = 0; // events to be suppressed until the next gesture
  Event _suppressAlways = 0; // events which are always disabled
  Callback _callbacks[EVENT_COUNT];
};


} // namespace Yabl

/*
 * The following line is for user convenience when declaring objects of type
 * `Button`, `Event` and `EventInfo`. Users will need to prefix these with
 * `Yabl::` if these names conflict with other classes sharing the same name. 
 */
using namespace Yabl;
