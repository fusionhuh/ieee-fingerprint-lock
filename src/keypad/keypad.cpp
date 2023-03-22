#include <heltec.h>
#include "keypad.hpp"

void keypad_setup() {
    pinMode(BUTTON_1, INPUT);
    pinMode(BUTTON_2, INPUT);
    pinMode(BUTTON_3, INPUT);
    pinMode(BUTTON_4, INPUT);
}

bool is_button_pressed(uint32_t button) {
    return digitalRead(button) == HIGH;
}