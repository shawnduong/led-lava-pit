#pragma once

/* Change these based on your wiring, if needed. */
#define KY_CLK  4
#define KY_DT   3
#define KY_SW   2

/* Initialize the configuration module. */
void init_config();

/* See if there's a knob press and enter config mode if so. */
void poll_config_mode();
