#include <Adafruit_Keypad.h>

#define KEYPAD_PID1332

#define R1 9
#define C2 10
#define C1 11
#define C4 12
#define C3 13

#include "keypad_config.h"

static Adafruit_Keypad custom_keypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void keypad_setup() {
    custom_keypad.begin();
}

uint8_t get_pressed_button() {
    custom_keypad.tick();
    if (custom_keypad.available()) {
        keypadEvent event = custom_keypad.read();
        if (event.bit.EVENT == KEY_JUST_PRESSED) {
            return (uint8_t) event.bit.KEY;
        }
        else {
            return 0;
        }
    }
    else {
        return 0;
    }
}

void refresh_keypad() {
    custom_keypad.tick();
}