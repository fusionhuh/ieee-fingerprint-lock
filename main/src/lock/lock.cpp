#include <Arduino.h>

static uint8_t output_pin = 8;

void lock_setup() {
    digitalWrite(output_pin, HIGH);
    delay(2000);
    digitalWrite(output_pin, LOW);
    delay(2000);
    digitalWrite(output_pin, HIGH);
    delay(2000);
    digitalWrite(output_pin, LOW);
}

void unlock(uint32_t output_pin) {
    digitalWrite(output_pin, HIGH);
    delay(3000);
    digitalWrite(output_pin, LOW);
}