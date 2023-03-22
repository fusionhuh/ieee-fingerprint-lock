// correspond to buttons on keypad
// for now, say GPIO pins 1-4 map to buttons 1-4
enum buttons { 
  NO_BUTTON = -1,
  BUTTON_1 = GPIO_NUM_1,
  BUTTON_2 = GPIO_NUM_2,
  BUTTON_3 = GPIO_NUM_3,
  BUTTON_4 = GPIO_NUM_4
};


void keypad_setup();

bool is_button_pressed(uint32_t button);