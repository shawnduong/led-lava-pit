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

void _handle_config_mode();
bool _select_config_option(uint8_t option);
void _display_config_option(uint8_t option);

struct _conf_t {
	uint16_t tick_interval;
	uint16_t row_length[MAX_ROWS];
	uint8_t effect;
	uint8_t direction;
};
static _conf_t _config;

/* --- Public functions. ------- */

void init_config()
{
	pinMode(KY_CLK, INPUT_PULLUP);
	pinMode(KY_DT , INPUT_PULLUP);
	pinMode(KY_SW , INPUT_PULLUP);

	_config.tick_interval = DEFAULT_TICK_INTERVAL;

	for (uint8_t i = 0; i < MAX_ROWS; i++)
		_config.row_length[i] = 0;
	_config.effect = EFFECT_STATIC;
	_config.direction = DIRECTION_NORMAL;

	write_lcd("Press knob to", "configure.");
}

void poll_config_mode()
{
	if (digitalRead(KY_SW) == HIGH)  return;
	_handle_config_mode();
}

/* --- Private functions. ------ */

void _menu(
	char *name,                   // menu name for debugging purposes
	void (*cb_display)(uint8_t),  // display function; cb_display(opt)
	bool (*cb_select)(uint8_t),   // selection function; cb_select(opt);
	                              //     ret=true => exit
	uint8_t opt_init,             // initial option value
	uint8_t opt_max               // max option value (inclusive)
){
	uint8_t clk = digitalRead(KY_CLK);
	uint8_t opt = opt_init;
	cb_display(opt);

	Serial.print(":: Entering menu: ");
	Serial.println(name);

	while (true)
	{
		/* Button press event. */
		if (digitalRead(KY_SW) == LOW)
		{
			if (cb_select(opt))  break;
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
		if (clk != digitalRead(KY_DT))  opt = (opt+1) % (opt_max+1);
		else                            opt = (opt < 1 ? opt_max : opt-1);
		cb_display(opt);
	}

	Serial.print(":: Exiting menu: ");
	Serial.println(name);
}

void _handle_config_mode()
{
	_menu(
		"Configuration Mode",
		&_display_config_option,
		&_select_config_option,
		CONFIG_OPTION_ROW_LENGTHS,
		CONFIG_OPTION_EXIT
	);
	write_lcd("Press knob to", "configure.");
}

void _display_config_option(uint8_t option)
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

bool _select_config_option(uint8_t option)
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
		return true;
	default:
		break;
	}

	return false;
}
