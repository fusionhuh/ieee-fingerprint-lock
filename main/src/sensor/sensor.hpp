#include <Adafruit_Fingerprint.h>

/*

Header file for the code interfacing with the fingerprint sensor

Basically utilizes basic functions from the Adafruit library to achieve
higher-level functionality

*/



//extern HardwareSerial mySerial;



// set up the fingerprint sensor
void sensor_setup();

// processes what is currently touching the sensor
void sensor_process();

// checks if last processed fingerprint is actually a fingerprint or something else
bool is_fingerprint_ok();

// checks if there is absolutely no fingerprint data
bool no_fingerprint();

// debug -- prints status of last processed fingerprint
void print_fingerprint_status();

// processes last read fingerprint's image
void process_image(uint8_t num=0);

// checks if successfully processed fingerprint's image
// is suitable to proceed
bool is_image_ok();

// debug -- prints status of last processed fingerprint's image
void print_image_status();

// compares fingerprint image against stored images to see if there's a match
void search_fingerprint();

// checks if fingerprint was found in database
bool is_fingerprint_found();

// debug -- prints status of fingerprint search
void print_search_status();

// attempts to generate a fingerprint model and enroll it in the database
bool attempt_fingerprint_enrollment();

// clears sensor's fingerprint database
void clear_database();

// deletes finger using finger id
void delete_fingerprint();

void sensor_led_control(uint8_t mode, uint8_t speed, uint8_t color, uint8_t duration);

void sensor_led_activate();

void sensor_led_passive();

void sensor_signal_success();

void sensor_flash_warning();