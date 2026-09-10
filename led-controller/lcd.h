#pragma once

#define LCD_COLUMNS  16
#define LCD_ROWS      2

void init_lcd(uint8_t address);
void clear_lcd();
void write_lcd(char *text, ...);
void write_lcd_row(uint8_t row, char *text);
