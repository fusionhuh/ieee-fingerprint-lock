enum buttons {
  NO_BUTTON = '0',
  BUTTON_1 = '1',
  BUTTON_2 = '2',
  BUTTON_3 = '3',
  BUTTON_4 = '4'
};

void keypad_setup();

uint8_t get_pressed_button();

void refresh_keypad();