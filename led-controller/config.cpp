#include <Arduino.h>
#include <EEPROM.h>
#include "headers/config.h"
#include "headers/lcd.h"
#include "headers/leds.h"

/* --- Internal definitions. --- */

#define DEFAULT_TICK_INTERVAL 500
#define MAX_ROWS 32

#define ALLOW_LOOPBACK_SCROLLING     true
#define DISALLOW_LOOPBACK_SCROLLING  false

/* EEPROM address where the config is persisted, and a magic byte written
 * alongside it so we can tell valid saved config apart from blank/garbage
 * EEPROM contents (e.g. on a brand new board). */
#define CONFIG_EEPROM_ADDR 0
#define CONFIG_MAGIC 0xC5

#define CONFIG_OPTION_ROW_LENGTHS  0
#define CONFIG_OPTION_EFFECT       1
#define CONFIG_OPTION_DIRECTION    2
#define CONFIG_OPTION_EXIT         3

#define EFFECT_STATIC   0
#define EFFECT_STILL    1
#define EFFECT_FLOWING  2

#define DIRECTION_NORMAL   0
#define DIRECTION_REVERSE  1

void _handle_config_mode();
void _handle_configure_effect();
void _handle_configure_direction();
void _configure_row_lengths();
void _save_config();
void _load_default_config();

struct _conf_t {
	uint16_t tick_interval;
	uint16_t row_shift[MAX_ROWS];
	uint16_t row_length[MAX_ROWS];
	uint8_t effect;
	uint8_t direction;
};
static _conf_t _config;

static uint8_t tmp_row;

/* --- Public functions. ------- */

void init_config()
{
	pinMode(KY_CLK, INPUT_PULLUP);
	pinMode(KY_DT , INPUT_PULLUP);
	pinMode(KY_SW , INPUT_PULLUP);

	if (EEPROM.read(CONFIG_EEPROM_ADDR) == CONFIG_MAGIC)
		EEPROM.get(CONFIG_EEPROM_ADDR + 1, _config);
	else
		_load_default_config();

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
	uint16_t opt_init,            // initial option value
	uint16_t opt_max,             // max option value (inclusive)
	bool loop                     // allow loopback scrolling
){
	uint8_t clk = digitalRead(KY_CLK);
	uint16_t opt = opt_init;
	cb_display(opt);

	Serial.print(":: Entering menu: ");
	Serial.println(name);

	delay(1000);
	while (true)
	{
		/* Button press event. */
		if (digitalRead(KY_SW) == LOW)
		{
			if (cb_select(opt))  break;
			cb_display(opt);
			delay(500);

			/* Prevent leaking into the next iteration. */
			clk = digitalRead(KY_CLK);
			continue;
		}

		/* Rotation event. */
		if (clk == digitalRead(KY_CLK))
		{
			delay(10);
			continue;
		}
		clk = !clk;

		/* Clockwise */
		if (clk != digitalRead(KY_DT))
			opt = (opt+1) % (opt_max+1);
		/* Counter-clockwise */
		else
			opt = (loop ? (opt < 1 ? opt_max : opt-1) : (opt > 0 ? opt-1 : 0));

		cb_display(opt);
	}

	Serial.print(":: Exiting menu: ");
	Serial.println(name);
}

void _display_config_option(uint8_t option)
{
	write_lcd_row(0, "Configure:");
	switch (option)
	{
	default:
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
	}
}

bool _select_config_option(uint8_t option)
{
	switch (option)
	{
	case CONFIG_OPTION_ROW_LENGTHS:
		_configure_row_lengths();
		break;
	case CONFIG_OPTION_EFFECT:
		_handle_configure_effect();
		break;
	case CONFIG_OPTION_DIRECTION:
		_handle_configure_direction();
		break;
	case CONFIG_OPTION_EXIT:
		_save_config();
		return true;
	default:
		break;
	}

	return false;
}

void _handle_config_mode()
{
	_menu(
		"Configuration Mode",
		&_display_config_option,
		&_select_config_option,
		CONFIG_OPTION_ROW_LENGTHS,
		CONFIG_OPTION_EXIT,
		ALLOW_LOOPBACK_SCROLLING
	);
	write_lcd("Press knob to", "configure.");
}

/* color in [8b red][8b green][8b blue][8b unused] format */
void _display_row_option(char *type, uint8_t option, uint32_t color)
{
	char buffer[32];

	sprintf(buffer, "Row %s (%d)", type, tmp_row);
	write_lcd_row(0, buffer);

	sprintf(buffer, "%d", option);
	write_lcd_row(1, buffer);

	uint16_t offset = 0;
	for (uint8_t row = 0; row < MAX_ROWS; row++)
		offset += _config.row_shift[row] + _config.row_length[row];

	uint8_t r = (color >> 24) & 0xFF;
	uint8_t g = (color >> 16) & 0xFF;
	uint8_t b = (color >>  8) & 0xFF;

	clear_leds();
	for (uint16_t pos = 0; pos < option; pos++)
		set_led_color(offset+pos, r, g, b);
	show_leds();
}
void _display_row_shift_option(uint8_t option)
{
	_display_row_option("Shift", option, 0x7F000000);
}
void _display_row_length_option(uint8_t option)
{
	_display_row_option("Length", option, 0x007F0000);
}

bool _select_row_shift_option(uint8_t option)
{
	_config.row_shift[tmp_row] = option;
	return true;
}
bool _select_row_length_option(uint8_t option)
{
	_config.row_length[tmp_row] = option;
	return true;
}

void _configure_row_lengths()
{
	char buffer[32];

	for (uint8_t row = 0; row < MAX_ROWS; row++)
	{
		_config.row_shift[row] = 0;
		_config.row_length[row] = 0;
	}

	for (tmp_row = 0; tmp_row < MAX_ROWS; tmp_row++)
	{
		sprintf(buffer, "Row Shift (%d)", tmp_row);
		_menu(
			buffer,
			&_display_row_shift_option,
			&_select_row_shift_option,
			0,
			MAX_LEDS-1,
			DISALLOW_LOOPBACK_SCROLLING
		);

		sprintf(buffer, "Row Length (%d)", tmp_row);
		_menu(
			buffer,
			&_display_row_length_option,
			&_select_row_length_option,
			0,
			MAX_LEDS-1,
			DISALLOW_LOOPBACK_SCROLLING
		);

		/* If the last value entered was 0, finish. */
		if (_config.row_length[tmp_row] == 0)  break;
	}
}

void _display_effect_option(uint8_t option)
{
	write_lcd_row(0, "Effect:");
	switch (option)
	{
	default:
	case EFFECT_STATIC:
		write_lcd_row(1, "0. Static");
		break;
	case EFFECT_STILL:
		write_lcd_row(1, "1. Still");
		break;
	case EFFECT_FLOWING:
		write_lcd_row(1, "2. Flowing");
		break;
	}
}

bool _select_effect_option(uint8_t option)
{
	_config.effect = option;
	return true;
}

void _handle_configure_effect()
{
	_menu(
		"Configure Effect",
		&_display_effect_option,
		&_select_effect_option,
		_config.effect,
		EFFECT_FLOWING,
		ALLOW_LOOPBACK_SCROLLING
	);
}

void _display_direction_option(uint8_t option)
{
	write_lcd_row(0, "Direction:");
	switch (option)
	{
	default:
	case DIRECTION_NORMAL:
		write_lcd_row(1, "0. Normal");
		break;
	case DIRECTION_REVERSE:
		write_lcd_row(1, "1. Reverse");
		break;
	}
}

bool _select_direction_option(uint8_t option)
{
	_config.direction = option;
	return true;
}

void _handle_configure_direction()
{
	_menu(
		"Configure Direction",
		&_display_direction_option,
		&_select_direction_option,
		_config.direction,
		DIRECTION_REVERSE,
		ALLOW_LOOPBACK_SCROLLING
	);
}

void _load_default_config()
{
	_config.tick_interval = DEFAULT_TICK_INTERVAL;

	for (uint8_t i = 0; i < MAX_ROWS; i++)
	{
		_config.row_shift[i] = 0;
		_config.row_length[i] = 0;
	}
	_config.effect = EFFECT_STATIC;
	_config.direction = DIRECTION_NORMAL;
}

void _save_config()
{
	EEPROM.update(CONFIG_EEPROM_ADDR, CONFIG_MAGIC);
	EEPROM.put(CONFIG_EEPROM_ADDR + 1, _config);
}
