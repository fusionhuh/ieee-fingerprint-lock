#include "Arduino.h"
#include <Adafruit_Fingerprint.h>
#include "src/sensor/sensor.hpp"
#include "src/lock/lock.hpp"
#include "src/keypad/keypad.hpp"

enum states {
  WAITING,
  FINGERPRINT_ADD,
  FINGERPRINT_CHECK,
  DOOR_UNLOCK,
  CLEAR_DATABASE
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
  Serial.begin(9600);
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
      Serial.println("Transitioning to CLEAR_DATABASE");
      transition_state(CLEAR_DATABASE);
    }
    delay(100);
  }
  break;

  case FINGERPRINT_ADD:
  {
    if (is_sensor_empty()) {
      Serial.println("Database is empty, enroll first fingerprint.");
    }
    else {
      Serial.println("Please have an authorized user give you permissions to add your fingerprint.");
      wait_for_finger_or_cancel();

      process_image();
      if (!is_image_ok()) {
        Serial.println("Issue with fingerprint image.");
        transition_state(WAITING);
      }

      search_fingerprint();
      if (!is_fingerprint_found()) {
        Serial.println("Fingerprint does not belong to authorized user.");
        sensor_flash_warning();
        transition_state(WAITING);
      }
      Serial.println("Authorized user detected.");
      sensor_signal_success();
      delay(1000);
      sensor_led_activate();
      while ((is_fingerprint_ok() || is_image_ok())) {
        delay(100);
        sensor_process();
        process_image();
        Serial.println("Please remove your finger.");
      }

    }

    Serial.println("Please place a new fingerprint to enroll.");
    wait_for_finger_or_cancel();

    if (!is_fingerprint_ok()) {
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
      transition_state(WAITING);
    }

    search_fingerprint();
    if (!is_fingerprint_found()) {
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

  case CLEAR_DATABASE:
  {
    Serial.println("Only the first authorized user can clear the database. Please touch your finger.");
    sensor_led_activate();
    wait_for_finger_or_cancel();
    process_image();
    if (!is_image_ok()) {
      transition_state(WAITING);
    }

    search_fingerprint();
    if (!is_finger_master()) {
      sensor_flash_warning();
      transition_state(WAITING);
    }

    Serial.println("Clearing database...");
    sensor_signal_success();
    clear_database();
    transition_state(WAITING);
  }
  break;

  case DOOR_UNLOCK:
  {
    auto button_press = get_pressed_button();
    if (button_press == BUTTON_1) {
      lock();
      transition_state(WAITING);
    }
    else if (button_press == BUTTON_2) {
      unlock();
    }
    else if (button_press == BUTTON_3) {
      lock();
    }
    delay(50);
  }
  break;

  default: // some severe error has occurred
    Serial.println("Severe error has occurred (no recognizable state)");
    return;
  }
}
