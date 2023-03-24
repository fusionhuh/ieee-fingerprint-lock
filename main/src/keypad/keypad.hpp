enum buttons {
  BUTTON_1 = '1',
  BUTTON_2 = '2',
  BUTTON_3 = '3',
  BUTTON_4 = '4'
};

void keypad_setup();

bool is_button_pressed(char button);