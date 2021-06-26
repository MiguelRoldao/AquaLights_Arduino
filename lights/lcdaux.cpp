
#include "lcdaux.h"
#include "buttons.h"
#include "times.h"
#include <stdint.h>
#include <LiquidCrystal_I2C.h>




// strings in progmem

void LCD_printButtons (LiquidCrystal_I2C *lcd, Buttons_t *b) {
	lcd->print(b->l);
	lcd->print(b->r);
	lcd->print(b->b);
	lcd->print(b->a);
}

void LCD_printTime (LiquidCrystal_I2C *lcd, Times_t *t) {
	if (t->h < 10) lcd->print("0");
	lcd->print(t->h);
	lcd->print(":");			
	if (t->m < 10) lcd->print("0");
	lcd->print(t->m);
	lcd->print(":");
	if (t->s < 10) lcd->print("0");
	lcd->print(t->s);
}

void LCD_state_toSleep(LiquidCrystal_I2C *lcd, Times_t *realTime) {
	lcd->noBacklight();
	lcd->setCursor(0,0);
	lcd->print(F("   AQUA-LIGHT   "));
	lcd->setCursor(0,1);
	lcd->print(F("    "));
	LCD_printTime(lcd, realTime);
	lcd->print(F("    "));
}

void LCD_state_toIdle(LiquidCrystal_I2C *lcd, Times_t *realTime) {
	lcd->backlight();
	lcd->setCursor(0,0);
	lcd->print(F("   AQUA-LIGHT   "));
	lcd->setCursor(0,1);
	lcd->print(F("    "));
	LCD_printTime(lcd, realTime);
	lcd->print(F("    "));
}

void LCD_state_toMenuCheckValues(LiquidCrystal_I2C *lcd) {
	lcd->setCursor(0,0);
	lcd->print(F("< Check values >"));
	lcd->setCursor(0,1);
	lcd->print(F("             3/5"));
}

void LCD_state_toMenuChangeBrightness(LiquidCrystal_I2C *lcd) {
	lcd->setCursor(0,0);
	lcd->print(F("< Brightness   >"));
	lcd->setCursor(0,1);
	lcd->print(F("             2/5")); 
}

void LCD_state_toMenuChangeSpectrums(LiquidCrystal_I2C *lcd) {
	lcd->setCursor(0,0);
	lcd->print(F("  Spectrums    >"));
	lcd->setCursor(0,1);
	lcd->print(F("             1/5")); 
}

void LCD_state_toMenuChangeTime(LiquidCrystal_I2C *lcd) {
	lcd->setCursor(0,0);
	lcd->print(F("< Time         >"));
	lcd->setCursor(0,1);
	lcd->print(F("             4/5")); 
}

void LCD_state_toMenuChangeSchedule(LiquidCrystal_I2C *lcd) {
	lcd->setCursor(0,0);
	lcd->print(F("< Schedule      "));
	lcd->setCursor(0,1);
	lcd->print(F("             5/5")); 
}
