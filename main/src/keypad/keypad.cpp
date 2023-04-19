#include <Adafruit_Keypad.h>

#define KEYPAD_PID1332

#define R1 13
#define C2 12
#define C1 11
#define C4 10
#define C3 9

#include "keypad_config.h"

static Adafruit_Keypad custom_keypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void keypad_setup() {
    custom_keypad.begin();
}

bool is_button_pressed(char button) {
    custom_keypad.tick();
    if (custom_keypad.available()) {
        keypadEvent event = custom_keypad.read();
        if (event.bit.EVENT == KEY_JUST_PRESSED) {
            return ((char)event.bit.KEY == button);
        }
        else {
            return false;
        }
    }
    else {
        //Serial.println("Keypad is not available.");
        return false;
    }
}