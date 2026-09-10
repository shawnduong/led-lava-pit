#include <Arduino.h>
#include "headers/config.h"
#include "headers/lcd.h"

/* --- Internal definitions. --- */

#define DEFAULT_TICK_INTERVAL 500
#define MAX_ROWS 16

#define EFFECT_STATIC   0
#define EFFECT_STILL    1
#define EFFECT_FLOWING  2

#define DIRECTION_NORMAL   0
#define DIRECTION_REVERSE  1

#define CONFIG_OPTION_ROW_LENGTHS 0
#define CONFIG_OPTION_EFFECT      1
#define CONFIG_OPTION_DIRECTION   2
#define CONFIG_OPTION_EXIT        3

/* ----------------------------- */

/* Forward declarations. */
void _handle_config_mode();
void _select_config_option(uint8_t option);
void _display_option(uint8_t option);

struct conf_t {
	uint16_t tick_interval;
	uint16_t row_length[MAX_ROWS];
	uint8_t effect;
	uint8_t direction;
};
static conf_t config;

void init_config()
{
	pinMode(KY_CLK, INPUT_PULLUP);
	pinMode(KY_DT , INPUT_PULLUP);
	pinMode(KY_SW , INPUT_PULLUP);

	config.tick_interval = DEFAULT_TICK_INTERVAL;

	for (uint8_t i = 0; i < MAX_ROWS; i++)
		config.row_length[i] = 0;
	config.effect = EFFECT_STATIC;
	config.direction = DIRECTION_NORMAL;

	write_lcd("Press knob to", "configure.");
}

void poll_config_mode()
{
	if (digitalRead(KY_SW) == HIGH)  return;
	_handle_config_mode();
}

void _handle_config_mode()
{
	Serial.println(":: Entering configuration mode...");
	_display_option(CONFIG_OPTION_ROW_LENGTHS);

	uint8_t clk = digitalRead(KY_CLK);
	uint8_t n = 0;
	while (true)
	{
		/* Button press event. */
		if (digitalRead(KY_SW) == LOW)
		{
			_select_config_option(n);
			if (n == CONFIG_OPTION_EXIT)  break;

			delay(1000);
			continue;
		}

		/* Rotation event. */
		if (clk == digitalRead(KY_CLK))
		{
			delay(10);
			continue;
		}
		clk = !clk;
		if (clk != digitalRead(KY_DT))  n = (n+1) % 4;
		else                            n = (n < 1 ? 3 : n-1);
		_display_option(n);
	}

	Serial.println(":: Exiting configuration mode.");

	write_lcd("Exiting...");
	delay(1000);
	write_lcd("Press knob to", "configure.");
}

void _display_option(uint8_t option)
{
	write_lcd_row(0, "Configure:");
	switch (option)
	{
	case CONFIG_OPTION_ROW_LENGTHS:
		write_lcd_row(1, "0. Row Lengths");
		break;
	case CONFIG_OPTION_EFFECT:
		write_lcd_row(1, "1. Effect");
		break;
	case CONFIG_OPTION_DIRECTION:
		write_lcd_row(1, "2. Direction");
		break;
	case CONFIG_OPTION_EXIT:
		write_lcd_row(1, "3. Exit");
		break;
	default:
		break;
	}
}

void _select_config_option(uint8_t option)
{
	switch (option)
	{
	case CONFIG_OPTION_ROW_LENGTHS:
//		_configure_row_lengths();
		break;
	case CONFIG_OPTION_EFFECT:
//		_configure_effect();
		break;
	case CONFIG_OPTION_DIRECTION:
//		_configure_direction();
		break;
	case CONFIG_OPTION_EXIT:
//		_save_config();
		return;
	default:
		break;
	}
}
