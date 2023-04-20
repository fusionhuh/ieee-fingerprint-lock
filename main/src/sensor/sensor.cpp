#include "sensor.hpp"

static uint8_t curr_fingerprint_status = 0;
static uint8_t curr_image_status = 0;
static uint8_t curr_search_status = 0;
static uint16_t running_id = 1;
static uint8_t finger_id = 1;

SoftwareSerial mySerial(2, 3);
static Adafruit_Fingerprint finger(&mySerial);

void sensor_setup() {
    Serial.begin(9600);
    while(!Serial);
    delay(100);
    Serial.println("\nAdafruit finger detect test\n");

    finger.begin(57600);
    finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_RED);
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
    finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_BLUE);
}

void sensor_process() {
    curr_fingerprint_status = finger.getImage();
}

bool is_fingerprint_ok() {
    return curr_fingerprint_status == FINGERPRINT_OK;
}

bool no_fingerprint() {
    return curr_fingerprint_status == FINGERPRINT_NOFINGER || curr_fingerprint_status == FINGERPRINT_IMAGEMESS;
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

void process_image(uint8_t num) {
  if (num == 0) {
    curr_image_status = finger.image2Tz();
  }
  else {
    curr_image_status = finger.image2Tz(num);
  }

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
    } 
    else if (curr_search_status == FINGERPRINT_NOTFOUND) {
        Serial.println("Did not find a match");
    } 
    else {
        Serial.println("Unknown error");
    }
}

bool attempt_fingerprint_enrollment() {
  search_fingerprint();
  if (curr_search_status == FINGERPRINT_OK) {
    Serial.println("Fingerprint is already stored!");
    return false;
  }
  else if (curr_search_status != FINGERPRINT_NOTFOUND) {
    Serial.println("Unknown error has occurred while checking fingerprint against database");
    return false;
  }

  auto model_status = finger.createModel();
  if (model_status == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  }
  else if (model_status == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return false;
  }
  else if (model_status == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return false;
  }
  else {
    Serial.println("Unknown error");
    return false;
  }

  auto store_status = finger.storeModel(running_id++);
  Serial.print(running_id);
  if (store_status == FINGERPRINT_OK) {
    Serial.println("Fingerprint stored!");
    return true;
  }
  else if (store_status == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return false;
  }
  else if (store_status == FINGERPRINT_BADLOCATION) {
    Serial.println("It seems as though this ID is already in use.");
    return false;
  }
  else if (store_status == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash.");
    return false;
  }
  else {
    Serial.println("Unknown error");
    return false;
  }
}

void clear_database() {
  finger.emptyDatabase();
}

void delete_fingerprint() {
  //search_fingerprint();
  auto p = finger.deleteModel(finger.fingerID);

  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Deleted!");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_BADLOCATION:
      Serial.println("Could not delete in that location");
      break;
    case FINGERPRINT_FLASHERR:
      Serial.println("Error writing to flash");
      break;
  }
  // turn LED blue
  /*
  finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_BLUE);
  delay(2000);
  finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_BLUE);

  finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
  delay(2000);
  finger.LEDcontrol(FINGERPRINT_LED_OFF, 0, FINGERPRINT_LED_BLUE);
  */
}

void sensor_led_activate() {
  finger.LEDcontrol(FINGERPRINT_LED_BREATHING, 100, FINGERPRINT_LED_BLUE);
}

void sensor_led_passive() {
  finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_BLUE);
}

void sensor_flash_warning() {
  finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 10, FINGERPRINT_LED_RED);
  delay(2500);
}

void sensor_signal_success() {
  finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_PURPLE);
}

void sensor_led_control(uint8_t mode, uint8_t duration, uint8_t color, uint8_t lifetime) {
  finger.LEDcontrol(mode, duration, color, lifetime);
}






