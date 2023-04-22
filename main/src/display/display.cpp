#include "display.hpp"


Adafruit_SSD1306 display(128, 64, &Wire, -1);


void display_setup()
{
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);
	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
	{
		Serial.println("fail");
		for (;;);
	}
	delay(500);
	display.clearDisplay();
	// Serial.println("done");
}

void display_message(String message)
{
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.setCursor(0, 0);
	display.println(message);
	display.display();
}

void display_clear()
{
	display.clearDisplay();
	display.display();
}
