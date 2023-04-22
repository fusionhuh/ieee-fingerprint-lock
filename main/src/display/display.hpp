#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*

Header file for the code interfacing with the OLED display

Basically utilizes basic functions from the Adafruit library to achieve
higher-level functionality

*/


// set up OLED display
void display_setup();

// display a string
void display_message(String message);

// clear screen
void display_clear();
