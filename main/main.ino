#include "Arduino.h"
#include <Adafruit_Fingerprint.h>
#include "src/sensor/sensor.hpp"
#include "src/lock/lock.hpp"
#include "src/keypad/keypad.hpp"

enum states {
  WAITING,
  FINGERPRINT_ADD,
  FINGERPRINT_CHECK,
  FINGERPRINT_CLEAR,
  DOOR_UNLOCK,
  DATABASE_SEARCH
};

#define transition_state(new_state) {\
  next_state = new_state;\ 
  sensor_process();\ 
  refresh_keypad();\
  if (new_state == FINGERPRINT_ADD || new_state == FINGERPRINT_CHECK) {\
    sensor_led_activate();\
  }\
  else if (new_state == WAITING) {\
    sensor_led_passive();\
  }\
  return;\
}

#define wait_for_finger_or_cancel() {\
  sensor_process();\
  while (no_fingerprint()) {\
    sensor_process();\
    delay(200);\
    auto button_press = get_pressed_button();\
    if (button_press == BUTTON_1) {\
      transition_state(WAITING);\
    }\
  }\
}\

#define repeat_state() return;

/*
The idea of this main sketch is to essentially act as a state
machine that manages transitions based on keypad and fingerprint
sensor input.

Avoid nasty code in this file, i.e. all sensor & lock operations 
should be done through a library/API we have written as .cpp/.hpp files
*/


/* define required states:

*** buttons for each state transition aren't supposed to be final
0) waiting state (WAITING):

  transition to: 
  a) once another state has been completed (successfully or not)
  
  transition away: 
  b) once a supported keypad button has been pressed or the fingerprint
  sensor returns a valid fingerprint (to FINGERPRINT_CHECK)

1) state of adding a new fingerprint (FINGERPRINT_ADD)

  transition to: 
  a) from WAITING state when BUTTON_0 is pressed on keypad
  b) from FINGERPRINT_ADD if fingerprint could not be properly added

  transition away: 
  a) to WAITING if fingerprint has been properly added
  b) to WAITING if BUTTON_0 is pressed (acts as a cancel button)

2) state of processing and checking fingerprint against database (FINGERPRINT_CHECK)

  transition to: 
  a) from WAITING when sensor returns some valid fingerprint ID

  transition away:
  a) to WAITING if fingerprint doesn't match any in database
  b) to DOOR_UNLOCK if fingerprint does match any in database

3) state of unlocking door (DOOR_UNLOCK)

  transition to:
  a) from FINGERPRINT_CHECK if check is successful

  transition away:
  a) to WAITING once door is unlocked

4) state of manually searching through database entries to perform certain actions (DATABASE_SEARCH)

  transition to:
  a) from WAITING when BUTTON_2 is pressed

  transition away:
  b) from DATABASE_SEARCH when BUTTON_2 is pressed (acts as cancel button)

*/

void setup() {
  sensor_setup();
  lock_setup();
  keypad_setup();
  clear_database();
}

uint8_t next_state = WAITING;

void loop() {
  switch(next_state) {
  case WAITING:
  {  
    auto button_press = get_pressed_button();
    if (button_press == BUTTON_1) {
      Serial.println("Transitioning to FINGERPRINT_ADD");
      sensor_led_activate();
      transition_state(FINGERPRINT_ADD);
    }
    else if (button_press == BUTTON_2) {
      Serial.println("Transitioning to FINGERPRINT_CHECK");
      transition_state(FINGERPRINT_CHECK);
    }
    else if (button_press == BUTTON_3) {
      Serial.println("Transitioning to FINGERPRINT_DELETE");
      transition_state(FINGERPRINT_DELETE);
    }
    else if (button_press == BUTTON_4) {
      Serial.println("lol");
    }

    delay(100);
  }
  break;

  case FINGERPRINT_ADD:
  {
    wait_for_finger_or_cancel();

    if (!is_fingerprint_ok()) {
      print_fingerprint_status();
      repeat_state();
    }

    process_image(1); // 1 means that this is the first fingerprint used for the model
    if (!is_image_ok()) {
      Serial.println("Sorry, your fingerprint wasn't clear enough. Please try again.");
      repeat_state();
    }

    Serial.println("Please remove your finger.");
    delay(2000);
    Serial.println("Now place the same finger again please.");
    
    wait_for_finger_or_cancel();

    if (!is_fingerprint_ok()) {
      print_fingerprint_status();
      repeat_state();
    }

    process_image(2); // 2 means that this is the second fingerprint used for the model
    if (!is_image_ok()) {
      Serial.println("Sorry, your second fingerprint wasn't clear enough. Please repeat this process again.");
      repeat_state();
    }

    if (attempt_fingerprint_enrollment() == false) {
      Serial.println("Enrollment failed, you may have already enrolled this fingerprint.");
      sensor_flash_warning();
      transition_state(WAITING);
    }

    sensor_led_passive();
    while ((is_fingerprint_ok() || is_image_ok())) {
      delay(100);
      sensor_process();
      process_image();
      Serial.println("Please remove your finger.");
    }
    sensor_signal_success();
    delay(1000);
    Serial.println("Fingerprint added successfully, going back to waiting state.");
    transition_state(WAITING);
  }
  break;
  
  case FINGERPRINT_CHECK:
  {
    wait_for_finger_or_cancel();

    process_image();
    if (!is_image_ok()) {
      print_image_status();
      transition_state(WAITING);
    }

    search_fingerprint();
    if (!is_fingerprint_found()) {
      print_search_status();
      sensor_flash_warning();
      transition_state(WAITING);
    }
    sensor_signal_success();
    Serial.println("Print matched! Unlocking door...");
    delay(1000);
    // YAY! fingerprint is found!
    // transition state to DOOR_UNLOCK
    transition_state(DOOR_UNLOCK);
  }
  break;

  case FINGERPRINT_DELETE:
  {
    delete_fingerprint();
    transition_state(WAITING);
  }
  break;

  case DOOR_UNLOCK:
  {
    //while (true) {
      //delay(10);
    //}
    unlock(5);

    // lock is done, transition back to
    // waiting state
    transition_state(WAITING);
  }
  break;

  case DATABASE_SEARCH:
  {
    // listen for button inputs
  }
  break;

  default: // some severe error has occurred
    Serial.println("Severe error has occurred (no recognizable state)");
    return;
  }
}
