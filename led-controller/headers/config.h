#pragma once

/* Change these based on your wiring, if needed. */
#define KY_CLK  4
#define KY_DT   3
#define KY_SW   2

#define MAX_ROWS 32

/* Configuration struct. Get the config with get_config(). */
struct conf_t {
	uint16_t tick_interval;
	uint16_t row_shift[MAX_ROWS];
	uint16_t row_length[MAX_ROWS];
	uint8_t n_rows;
	uint8_t n_cols;
	uint8_t effect;
	uint8_t direction;
};

/* Initialize the configuration module. */
void init_config();

/* See if there's a knob press and enter config mode if so. */
void poll_config_mode();

/* Get the configuration. */
conf_t *get_config();
