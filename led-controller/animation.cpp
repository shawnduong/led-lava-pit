#include <Arduino.h>
#include "headers/animation.h"
#include "headers/config.h"
#include "headers/leds.h"

static uint8_t *channel_r;
static uint8_t *channel_g;
static uint8_t *channel_b;
static uint16_t len;

static conf_t *config;

void init_animation()
{
	len = 0;
	config = get_config();
	update_animation();
}

void update_animation()
{
	if (channel_r) free(channel_r);
	if (channel_g) free(channel_g);
	if (channel_b) free(channel_b);

	len = 0;
	for (uint8_t row = 0; row < MAX_ROWS; row++)
	{
		if (config->row_length[row] == 0)  break;
		len += config->row_length[row];
	}

	channel_r = (uint8_t *)calloc(len, sizeof(uint8_t));
	channel_g = (uint8_t *)calloc(len, sizeof(uint8_t));
	channel_b = (uint8_t *)calloc(len, sizeof(uint8_t));

	for (uint16_t i = 0; i < len; i++)
	{
		channel_r[i] = random(180, 220);
		channel_g[i] = random(  0,  30);
		channel_b[i] = random(  0,   0);
	}
}

void advance_animation()
{
	uint16_t offset = 0;
	uint16_t index = 0;

	clear_leds();
	for (uint8_t row = 0; row < MAX_ROWS; row++)
	{
		if (config->row_length[row] == 0)  break;

		offset += config->row_shift[row];
		for (uint16_t i = 0; i < config->row_length[row]; i++)
		{
			set_led_color(offset+i, channel_r[index], channel_g[index], channel_b[index]);
			index++;
		}
		offset += config->row_length[row];
	}
	show_leds();
}
