#ifndef _MIRO_AQUARIUMLIGHTS_UI_H_
#define _MIRO_AQUARIUMLIGHTS_UI_H_

#include <stdint.h>
#include "buttons.h"
#include "times.h"
#include <LiquidCrystal_I2C.h>

#define STATE_FIRST_TIME				0
#define STATE_SLEEP						1
#define STATE_IDLE						2
#define STATE_MENU_VALUES				3
#define STATE_MENU_BRIGHTNESS			4
#define STATE_MENU_SPECTRUMS			5
#define STATE_MENU_TIME					6
#define STATE_MENU_SCHEDULE				7
#define STATE_SET_TIME					8
#define STATE_SET_BRIGHTNESS			9
#define STATE_SET_SCHEDULE_CHANNEL		10
#define STATE_SET_SCHEDULE_TIMESTAMP	11
#define STATE_SET_SCHEDULE_TIME			12
#define STATE_SET_SPECTRUM_CHANNEL		13
#define STATE_SET_SPECTRUM_BRIGHTNES	14
#define STATE_DSPLAY_VALUES				15

#define TIMESTATE_HOURS		0
#define TIMESTATE_MINUTES	1
#define TIMESTATE_SECONDS	2

#define CHSTATE_N	   5
#define CHSTATE_UV	  0
#define CHSTATE_ROYAL   1
#define CHSTATE_BLUE	2
#define CHSTATE_WHITE   3
#define CHSTATE_WARM	4

#define SCHEDULESTATE_N			 4
#define SCHEDULESTATE_SUNRISE_START 0
#define SCHEDULESTATE_SUNRISE_END   1
#define SCHEDULESTATE_SUNSET_START  2
#define SCHEDULESTATE_SUNSET_END	3

#define CHINTSTATE_N 10
#define CHINTSTATE_UV_MIN 0
#define CHINTSTATE_UV_MAX 1
#define CHINTSTATE_ROYAL_MIN 2
#define CHINTSTATE_ROYAL_MAX 3
#define CHINTSTATE_BLUE_MIN 4
#define CHINTSTATE_BLUE_MAX 5
#define CHINTSTATE_WHITE_MIN 6
#define CHINTSTATE_WHITE_MAX 7
#define CHINTSTATE_WARM_MIN 8
#define CHINTSTATE_WARM_MAX 9

#define UI_RET_NOP	  0
#define UI_RET_DONE	 1
#define UI_RET_EXIT	 2
#define UI_RET_UPDATE   3


// Aux functions
void LCD_printButtons (LiquidCrystal_I2C *lcd, Buttons_t *b);
void LCD_printTime (LiquidCrystal_I2C *lcd, Times_t *t);
void LCD_state_toSleep(LiquidCrystal_I2C *lcd, Times_t *realTime);
void LCD_state_toIdle(LiquidCrystal_I2C *lcd, Times_t *realTime);
void LCD_state_toMenuCheckValues(LiquidCrystal_I2C *lcd);
void LCD_state_toMenuChangeBrightness(LiquidCrystal_I2C *lcd);
void LCD_state_toMenuChangeSpectrums(LiquidCrystal_I2C *lcd);
void LCD_state_toMenuChangeTime(LiquidCrystal_I2C *lcd);
void LCD_state_toMenuChangeSchedule(LiquidCrystal_I2C *lcd);

// UI functions
uint8_t UI_selectTime(uint8_t *timeState);
uint8_t UI_changeBrightness(uint8_t *b);
uint8_t UI_displayValues(uint8_t *values);
uint8_t UI_selectChannel(int8_t *channelState);
uint8_t UI_selectTimeStamp(int8_t *stampState);
uint8_t UI_selectChannelIntensity(int8_t *chIntState);

#endif
