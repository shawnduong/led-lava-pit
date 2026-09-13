#pragma once

/* Change this based on your wiring, if needed. */
#define WS2812B_PIN 33

#define MAX_LEDS 256

/* Initialize the LEDs module. */
void init_leds();

/* Show the LEDs. */
void show_leds();

/* Clear the LEDs. */
void clear_leds();

/* Set a single LED's color. */
void set_led_color(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
