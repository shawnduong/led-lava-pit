#pragma once

/* Change these based on your LCD module, if needed. */
#define LCD_COLUMNS  16
#define LCD_ROWS      2

/* Initialize the LCD module. */
void init_lcd(uint8_t address);

/* Clear the LCD screen. */
void clear_lcd();

/* Overwrite the entire LCD screen with given text contents. Every input string
   is another line on the LCD screen. */
void write_lcd(char *text, ...);

/* Write a string to a specific row on the LCD. */
void write_lcd_row(uint8_t row, char *text);
