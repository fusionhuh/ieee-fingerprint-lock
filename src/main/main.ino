#include <heltec.h>
#include <Adafruit_Fingerprint.h>

/*
The idea of this main sketch is to essentially act as a state
machine that manages transitions based on keypad and fingerprint
sensor input. Each state transition adds a line of text (could be empty)
to a buffer that is "flushed" and printed on each loop iteration

Avoid nasty code in this file, i.e. all sensor operations 
should be done through an library/API we have written as .cpp/.hpp files
*/

enum states {
  WAITING = 0,
  FINGERPRINT_ADD,
  FINGERPRINT_CHECK,
  DOOR_UNLOCK,
  FINGERPRINT_SEARCH
};

// correspond to buttons on keypad
enum buttons { 
  NO_BUTTON = -1,
  BUTTON_0,
  BUTTON_1,
  BUTTON_2,
  BUTTON_3
};

/* define several required states:

*** buttons for each state transition aren't supposed to be final
0) waiting state (WAITING):

transition to: once another state has been completed (successfully or not)
transition away: once a keypad button or the fingerprint sensor
sends a signal

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

4) state of manually searching through database entries to perform certain actions (FINGERPRINT_SEARCH)

  transition to:
  a) from WAITING when BUTTON_2 is pressed

  transition away:
  b) from FINGERPRINT_SEARCH when BUTTON_2 is pressed (acts as cancel button)



*/



void setup() {
  // put your setup code here, to run once:

}

uint8_t next_state = states.WAITING;
uint8_t last_button_press = buttons.NO_BUTTON;

void loop() {
  /*

  poll for button press here

  */
  switch(next_state) {
  case states.WAITING: // check for sensor/keypad inputs
  {  
  


  }
  case states.FINGERPRINT_ADD:
  {

  }
  default: // some severe error has occurred
    Serial.println("Severe error has occurred (no recognizable state)")
    return;
  }
  last_button_press = buttons.NO_BUTTON;
}
