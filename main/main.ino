#include "Arduino.h"
#include <Adafruit_Fingerprint.h>
#include "src/sensor/sensor.hpp"
#include "src/lock/lock.hpp"
#include "src/keypad/keypad.hpp"

int lock_output_pin = 8;

/*
The idea of this main sketch is to essentially act as a state
machine that manages transitions based on keypad and fingerprint
sensor input.

Avoid nasty code in this file, i.e. all sensor & lock operations 
should be done through a library/API we have written as .cpp/.hpp files
*/

enum states {
  WAITING,
  FINGERPRINT_ADD,
  FINGERPRINT_CHECK,
  DOOR_UNLOCK,
  DATABASE_SEARCH
};

// from keypad.hpp:
/*
enum buttons { 
  NO_BUTTON,
  BUTTON_1,
  BUTTON_2,
  BUTTON_3,
  BUTTON_4
};
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
  //Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Enable*/, true /*Serial Enable*/, true /*LoRa use PABOOST*/); // taken from factory test example
  //sensor_setup();
  //clear_database();
  //lock_setup();
  //keypad_setup();
  pinMode(lock_output_pin, OUTPUT);

}

uint8_t next_state = WAITING;

void loop() {
  Serial.print("testing");
  Heltec.display->clear();
  Heltec.display->drawString(20, 20, "Testing!");
  //display.clear();
  //display.display();
  //display.drawString(64, 32-16/2, "Hello World");
}

void loop() {
  sensor_process();
  switch(next_state) {
  case WAITING:
  {  
    while (no_fingerprint()) {
      if (is_button_pressed(BUTTON_1)) {
        next_state = FINGERPRINT_ADD;
        break;
      }
    }

    if (!is_fingerprint_ok()) {
      print_fingerprint_status(); 
      break; // return to beginning
    }

    // some kind of fingerprint detected,
    // transition state to FINGERPRINT_CHECK
    next_state = FINGERPRINT_CHECK;
  }
  break;

  case FINGERPRINT_ADD:
  {
    while (no_fingerprint()) {
      // return to waiting state if cancel button is pressed
      if (is_button_pressed(BUTTON_1)) {
        next_state = WAITING;
        break;
      }
    }

    if (!is_fingerprint_ok()) {
      print_fingerprint_status();
      break; // go back to start
    }

    process_image(1); // 1 means that this is the first fingerprint used for the model
    if (!is_image_ok()) {
      Serial.println("Sorry, your fingerprint wasn't clear enough. Please try again.");
      break; // go back to start
    }

    Serial.println("Please remove your finger and place it again.");
    while (no_fingerprint()) {
      // return to waiting state if cancel button is pressed
      if (is_button_pressed(BUTTON_1)) {
        next_state = WAITING;
        break;
      }
    }

    if (!is_fingerprint_ok()) {
      print_fingerprint_status();
      break; // go back to start
    }

    process_image(2); // 2 means that this is the second fingerprint used for the model
    if (!is_image_ok()) {
      Serial.println("Sorry, your second fingerprint wasn't clear enough. Please repeat this process again.");
      break; // go back to start
    }

    if (attempt_fingerprint_enrollment() == false) {
      Serial.println("Enrollment failed, please try again.");
      break;
    }
  }
  break;
  
  case FINGERPRINT_CHECK:
  {
    process_image();
    if (!is_image_ok()) {
      print_image_status();
      next_state = WAITING; // return to waiting state
      break;
    }

    search_fingerprint();
    if (!is_fingerprint_found()) {
      print_search_status();
      next_state = WAITING; // return to waiting state
      break;
    }

    // YAY! fingerprint is found!
    // transition state to DOOR_UNLOCK
    next_state = DOOR_UNLOCK;
  }
  break;

  case DOOR_UNLOCK:
  {
    while (!is_lock_ready()) {
      delay(10);
    }
    unlock();

    // lock is done, transition back to
    // waiting state
    next_state = WAITING;
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
