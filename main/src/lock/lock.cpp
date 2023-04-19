void lock_setup() {
    digitalWrite(output_pin, HIGH);
    delay(2);
    digitalWrite(output_pin, LOW);
    delay(2);
    digitalWrite(output_pin, HIGH);
    delay(2);
    digitalWrite(output_pin, LOW);
}

void unlock(uint32_t output_pin) {
    digitalWrite(output_pin, HIGH);
    delay(3);
    digitalWrite(outout_pin, LOW);
}