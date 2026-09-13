#include <Wire.h>
#include "headers/animation.h"
#include "headers/config.h"
#include "headers/lcd.h"
#include "headers/leds.h"

static conf_t *config;

void setup()
{
	Serial.begin(9600);
	Serial.print("\n\n\n");
	Wire.begin();

	Serial.println(":: Initializing...");
	Serial.print("   Looking for display...");
	for (uint8_t i = 1; i < 127; i++)
	{
		Wire.beginTransmission(i);
		if (Wire.endTransmission() == 0)
		{
			Serial.print(" found at 0x");
			Serial.println(i, HEX);
			init_lcd(i);
			break;
		}
	}
	init_config();
	init_leds();
	init_animation();

	config = get_config();

	Serial.println(":: Initialization complete.");
}

void loop()
{
	poll_config_mode();
	advance_animation();
	delay(config->tick_interval);
}
