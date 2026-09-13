#include <Arduino.h>
#include "headers/animation.h"
#include "headers/config.h"
#include "headers/leds.h"

void _handle_static();
void _handle_still();
void _handle_flowing();

static uint8_t *channel_r;
static uint8_t *channel_g;
static uint8_t *channel_b;
static uint16_t len;

static conf_t *config;

/* Adjust these for the desired color range. */
#define R_LBOUND 180
#define R_UBOUND 220
#define G_LBOUND   0
#define G_UBOUND  30
#define B_LBOUND   0
#define B_UBOUND   0

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
		channel_r[i] = random(R_LBOUND, R_UBOUND);
		channel_g[i] = random(G_LBOUND, G_UBOUND);
		channel_b[i] = random(B_LBOUND, B_UBOUND);
	}
}

void advance_animation()
{
	uint16_t offset = 0;
	uint16_t index = 0;

	switch (config->effect)
	{
	default:
	case EFFECT_STATIC:
		_handle_static();
		break;
	case EFFECT_STILL:
		_handle_still();
		break;
	case EFFECT_FLOWING:
		break;
	}

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

/* Do nothing. */
void _handle_static() {}

/* Pixels may shift in brightness by +/- 2%, but must stay in the color range. */
void _handle_still()
{
	for (uint16_t i = 0; i < len; i++)
	{
		uint8_t r = channel_r[i];
		uint8_t g = channel_g[i];
		uint8_t b = channel_b[i];

		r += random(-5, 6); // +/- 2% of 255 is ~5
		g += random(-5, 6);
		b += random(-5, 6);

		if (r < R_LBOUND) r = R_LBOUND;
		if (r > R_UBOUND) r = R_UBOUND;
		if (g < G_LBOUND) g = G_LBOUND;
		if (g > G_UBOUND) g = G_UBOUND;
		if (b < B_LBOUND) b = B_LBOUND;
		if (b > B_UBOUND) b = B_UBOUND;

		channel_r[i] = r;
		channel_g[i] = g;
		channel_b[i] = b;
	}
}

/* Not implemented yet. */
void _handle_flowing()
{
}
