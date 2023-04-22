#include "display.hpp"

void display_setup()
{
	Serial.begin(115200);
	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
	{
		Serial.println(F("SSD1306 allocation failed"));
	}
}

void display_string(std::string message)
{
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.setCursor(0, 10);
	display.println(message);
	display.display();
}

void display_clear()
{
	display.clearDisplay();
}
