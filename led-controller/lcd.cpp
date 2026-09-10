#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "headers/lcd.h"

static LiquidCrystal_I2C lcd(0,0,0);

void _tee_to_serial(uint8_t row, char *text)
{
	char buffer[16+strlen(text)] = { 0 };
	sprintf(buffer, "   [ LCD (%d) ] : |%s|", row, text);
	Serial.println(buffer);
}

void init_lcd(uint8_t address)
{
	lcd = LiquidCrystal_I2C(address, LCD_COLUMNS, LCD_ROWS);
	lcd.init();
	lcd.backlight();
}

void clear_lcd()
{
	char buffer[LCD_COLUMNS+1] = { 0 };
	memset(buffer, ' ', LCD_COLUMNS);

	for (uint8_t i = 0; i < LCD_ROWS; i++)
	{
		lcd.setCursor(0, i);
		lcd.print(buffer);
	}
}

void write_lcd(char *text, ...)
{
	clear_lcd();

	va_list args;
	va_start(args, text);

	for (uint8_t i = 0; text && i < LCD_ROWS; i++)
	{
		write_lcd_row(i, text);
		text = va_arg(args, char*);
	}

	va_end(args);
}

void write_lcd_row(uint8_t row, char *text)
{
	char buffer[LCD_COLUMNS+1] = { 0 };
	memset(buffer, ' ', LCD_COLUMNS);
	memcpy(buffer, text, strlen(text));

	lcd.setCursor(0, row);
	lcd.print(buffer);
	_tee_to_serial(row, buffer);
}
