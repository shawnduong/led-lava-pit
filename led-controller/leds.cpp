#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "headers/leds.h"

static Adafruit_NeoPixel leds(MAX_LEDS, WS2812B_PIN, NEO_GRB + NEO_KHZ800);

void init_leds()
{
	leds.begin();
	clear_leds();
	show_leds();
}

void show_leds()
{
	leds.show();
}

void clear_leds()
{
	leds.clear();
}

void set_led_color(uint16_t index, uint8_t r, uint8_t g, uint8_t b)
{
	leds.setPixelColor(index, r, g, b);
}
