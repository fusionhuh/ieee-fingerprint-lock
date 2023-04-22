#include <Arduino.h>

static const uint8_t output_pin = 7;

void lock_setup() {
    pinMode(output_pin, OUTPUT);
    digitalWrite(output_pin, LOW);
}

void unlock() {
    digitalWrite(output_pin, LOW);
}

void lock() {
    digitalWrite(output_pin, HIGH);
}