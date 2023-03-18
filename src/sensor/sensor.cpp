#include "sensor.hpp"


static uint8_t curr_fingerprint_status = 0;
static uint8_t curr_image_status = 0;
static uint8_t curr_search_status = 0;

void sensor_setup() {
    mySerial = SoftwareSerial(2, 3); // unsure about these numbers?
    finger = Adafruit_Fingerprint(&mySerial);

    Serial.begin(9600);
    while(!Serial);
    delay(100);
    Serial.println("\nAdafruit finger detect test\n");

    finger.begin(57600);
    delay(5);
    if (finger.verifyPassword()) {
        Serial.println("Found fingerprint sensor!");
    }
    else {
        Serial.println("Did not find fingerprint sensor.");
        while (1) {
            delay(1);
        }
    }

    Serial.println(F("Reading sensor parameters"));
    finger.getParameters();
    Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
    Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
    Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
    Serial.print(F("Security level: ")); Serial.println(finger.security_level);
    Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
    Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
    Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
}

void sensor_process() {
    curr_fingerprint_status = finger.getImage();
}

bool is_fingerprint_ok() {
    return curr_fingerprint_status == FINGERPRINT_OK;
}

void print_fingerprint_status() {
    switch(curr_fingerprint_status) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
}

void process_image() {
    curr_image_status = finger.image2Tz();
}

bool is_image_ok() {
    return curr_image_status == FINGERPRINT_OK;
}

void print_image_status() {
    switch(curr_image_status) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      break;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
}

void search_fingerprint() {
    curr_search_status = finger.fingerSearch();
}

bool is_fingerprint_found() {
    return curr_search_status == FINGERPRINT_OK;
}

void print_search_status() {
    if (curr_search_status == FINGERPRINT_OK) {
        Serial.println("Found a print match!");
    } 
    else if (curr_search_status == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return p;
    } 
    else if (curr_search_status == FINGERPRINT_NOTFOUND) {
        Serial.println("Did not find a match");
        return p;
    } 
    else {
        Serial.println("Unknown error");
        return p;
    }
}






