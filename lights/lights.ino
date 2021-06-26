#include <Encoder.h>
#include <stdint.h>
#include <EEPROM.h>
#include <PCF8574.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TimeLib.h>
#include "times.h"
#include "spectrum.h"
#include "buttons.h"
#include "lcdaux.h"


#define UI_FILE_TEST
#define BUTTONS_FILE_TEST

#define EEADDRESS 128
#define EEPROM_ADDR_LIGHTS 16
#define EEPROM_ADDR_BRIGHTNESS 15

#define BUTTON_REPEAT_TIME 5


#define CHANNEL_N 5
#define CHANNEL_0 0
#define CHANNEL_1 1
#define CHANNEL_2 2
#define CHANNEL_3 3
#define CHANNEL_4 4

#define LED_PIN_UV  11
#define LED_PIN_RB  10
#define LED_PIN_B   9
#define LED_PIN_CW  6
#define LED_PIN_WW  5


// hardware objects
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
PCF8574 pcf(0x20);

int8_t selectTimeStampState = SCHEDULESTATE_SUNSET_START;
Times_t setTimeTemp = {0};

// project globals
Buttons_t buttons = {0};
Buttons_t buttonsPressed = {0};
Spectrum_t lights[5];
uint8_t brightness;
uint8_t outputValues[] = {0, 0, 0, 0, 0};

// loop globals
Times_t realTime = {0, 0, 0};
uint8_t brightnessBKP = 0;
uint8_t update_time = 0;
uint8_t buttonCounter = 0;
uint8_t update_buttons = 0;

//ui state machine globals
uint8_t state = STATE_FIRST_TIME;
uint8_t setTimeState = TIMESTATE_HOURS;
int8_t selectChannelState = CHSTATE_BLUE;
int8_t selectChannelIntesityState = CHINTSTATE_BLUE_MIN;
uint8_t *spectrumIntensityToEdit = NULL;


void setup() {
	Serial.begin(9600);

	// Initialize 
	pcf.begin();


	// TODO: set time from RTC instead
	setTime(realTime.h, realTime.m, realTime.s, 20, 4, 2069);

	lcd.init();
	lcd.setCursor(0,0);
	lcd.print(F("AQUALIGHT: press"));
	lcd.setCursor(0,1);
	lcd.print(F("all the buttons "));
	lcd.backlight();
	state = STATE_FIRST_TIME;
	buttons_init(&pcf);

	

	pinMode(13, OUTPUT);

	pinMode(LED_PIN_UV, OUTPUT);
	pinMode(LED_PIN_RB, OUTPUT);
	pinMode(LED_PIN_B, OUTPUT);
	pinMode(LED_PIN_CW, OUTPUT);
	pinMode(LED_PIN_WW, OUTPUT);

	analogWrite(LED_PIN_UV, 1);
	analogWrite(LED_PIN_RB, 8);
	analogWrite(LED_PIN_B, 16);
	analogWrite(LED_PIN_CW, 0x60);
	analogWrite(LED_PIN_WW, 255);

	EEPROM.get(EEPROM_ADDR_BRIGHTNESS, brightness);
	EEPROM.get(EEPROM_ADDR_LIGHTS, lights);

	attachInterrupt(digitalPinToInterrupt(2), handleInputISR, FALLING);
}


unsigned int buttonsMillis = 0;

void loop() {

	//detectPowerloss();
	
	unsigned int currentMillis = millis();
	
	//Lets blink the same output
	if(currentMillis - buttonsMillis >= 100){
		// check if buttons have been pressed for long, if so reload button events
		if ( buttonCounter < BUTTON_REPEAT_TIME ) {
			buttonCounter++;
		} else {
			buttons = buttonsPressed;
			update_buttons = 1;
		}

		//Update time
		buttonsMillis = currentMillis;
		// if theres been an interrupt, read buttons
		if (is_read_buttons != 0) {
			is_read_buttons = 0;
			//buttons.bulk = pcf.read8();
			readButtons(&buttons);
			buttonsPressed = buttons;

			buttonCounter = 0;
			update_buttons = 1;
		}
	}

	// Update time
	if (second() != realTime.s) {
		realTime.h = hour();
		realTime.m = minute();
		realTime.s = second();

		update_time = 1;
		
		Serial.print(realTime.h);
		Serial.print(":");
		Serial.print(realTime.m);
		Serial.print(":");
		Serial.println(realTime.s);
	}


	// calculate light values
	if (update_time) {
		for (int i = 0; i < CHANNEL_N; i++) {
			outputValues[i] = SPECTRUM_Power(&lights[i], &realTime) * brightness / 255;
			
		}
	}

	// TODO: output values to PWMs

	switch (state) {
		case STATE_FIRST_TIME:
		if (buttons.bulk) {
			buttons.bulk = 0;
			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
		}
		break;
		
		case STATE_SLEEP:
		if (update_time) {
			lcd.setCursor(4,1);
			LCD_printTime(&lcd, &realTime);
		}
		if (buttons.bulk) {
			buttons.bulk = 0;
			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
		}
		break;

		case STATE_IDLE:
		if (update_time) {
			lcd.setCursor(4,1);
			LCD_printTime(&lcd, &realTime);
		}
		if (buttons.b) {
			buttons.b = 0;
			LCD_state_toSleep(&lcd, &realTime);
			state = STATE_SLEEP;
		} else if (buttons.a) {
			buttons.a = 0;
			LCD_state_toMenuCheckValues(&lcd);
			state = STATE_MENU_VALUES;
		}
		break;
		
		case STATE_MENU_VALUES:
		if (buttons.b) {
			buttons.b = 0;
			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
		} else if (buttons.a) {
			buttons.a = 0;
			lcd.setCursor(0,0);
			lcd.print(F(" UV RB BL CW WW "));
			update_time = 1;
			UI_displayValues(outputValues);
			state = STATE_DSPLAY_VALUES;
			// TODO: implement check values
		} else if (buttons.l) {
			buttons.l = 0;
			LCD_state_toMenuChangeBrightness(&lcd);
			state = STATE_MENU_BRIGHTNESS;
		} else if (buttons.r) {
			buttons.r = 0;
			LCD_state_toMenuChangeTime(&lcd);
			state = STATE_MENU_TIME;
		}
		break;

		case STATE_MENU_BRIGHTNESS:
		if (buttons.b) {
			buttons.b = 0;
			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
		} else if (buttons.a) {
			buttons.a = 0;
			brightnessBKP = brightness;
			state = STATE_SET_BRIGHTNESS;
			lcd.setCursor(0,0);
			lcd.print(F("Set brightness: "));
			lcd.setCursor(0,1);
			lcd.print(F("                "));
			lcd.setCursor(11,1);
			if (brightness < 10) lcd.print(F("  "));
			else if (brightness < 100) lcd.print(F(" "));
			lcd.print(brightness);
		   // TODO: implement change brightness
		} else if (buttons.l) {
			buttons.l = 0;
			LCD_state_toMenuChangeSpectrums(&lcd);
			state = STATE_MENU_SPECTRUMS;
		} else if (buttons.r) {
			buttons.r = 0;
			LCD_state_toMenuCheckValues(&lcd);
			state = STATE_MENU_VALUES;
		}
		break;

		case STATE_MENU_SPECTRUMS:
		if (buttons.b) {
			buttons.b = 0;
			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
		} else if (buttons.a) {
			buttons.a = 0;
			lcd.setCursor(0,0);
			lcd.print(F("Choose spectrum "));
			lcd.setCursor(0,1);
			lcd.print(F("Min Blue        "));
			selectChannelIntesityState = 4;
			state = STATE_SET_SPECTRUM_CHANNEL;
			// TODO: implement change spectrums
		} else if (buttons.r) {
			buttons.r = 0;
			LCD_state_toMenuChangeBrightness(&lcd);
			state = STATE_MENU_BRIGHTNESS;
		}
		break;
		
		case STATE_MENU_TIME:
		if (buttons.b) {
			buttons.b = 0;
			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
		} else if (buttons.a) {
			buttons.a = 0;
			state = STATE_SET_TIME;
			setTimeTemp = realTime;
			setTimeState = TIMESTATE_HOURS;
			lcd.setCursor(0,0);
			lcd.print(F("Change hours:   "));
			lcd.setCursor(0,1);
			lcd.print(F("                "));
			lcd.setCursor(4,1);
			LCD_printTime(&lcd, &setTimeTemp);
		} else if (buttons.l) {
			buttons.l = 0;
			LCD_state_toMenuCheckValues(&lcd);
			state = STATE_MENU_VALUES;
		} else if (buttons.r) {
			buttons.r = 0;
			LCD_state_toMenuChangeSchedule(&lcd);
			state = STATE_MENU_SCHEDULE;
		}
		break;

		case STATE_MENU_SCHEDULE:
		if (buttons.b) {
			buttons.b = 0;
			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
		} else if (buttons.a) {
			buttons.a = 0;
			state = STATE_SET_SCHEDULE_CHANNEL;
			lcd.setCursor(0,0);
			lcd.print(F("Select Channel: "));
			lcd.setCursor(0,1);
			lcd.print(F("Blue 465nm      "));
			selectChannelState = CHSTATE_BLUE;
		} else if (buttons.l) {
			buttons.l = 0;
			LCD_state_toMenuChangeTime(&lcd);
			state = STATE_MENU_TIME;
		}
		break;


		case STATE_DSPLAY_VALUES:
		switch (UI_displayValues(outputValues)) {
			case UI_RET_DONE:
			// FALL THROUGH
			case UI_RET_EXIT:
			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
			break;

			default:
			break;
		}
		break;
		
		case STATE_SET_SPECTRUM_CHANNEL:
		switch (UI_selectChannelIntensity(&selectChannelIntesityState)) {
			case UI_RET_EXIT:
			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
			break;
			
			case UI_RET_DONE:
			switch (selectChannelIntesityState) {
				case CHINTSTATE_UV_MIN:
				spectrumIntensityToEdit = &(lights[0].min);
				break;

				case CHINTSTATE_UV_MAX:
				spectrumIntensityToEdit = &(lights[0].max);
				break;
				
				case CHINTSTATE_ROYAL_MIN:
				spectrumIntensityToEdit = &(lights[1].min);
				break;

				case CHINTSTATE_ROYAL_MAX:
				spectrumIntensityToEdit = &(lights[1].max);
				break;

				case CHINTSTATE_BLUE_MIN:
				spectrumIntensityToEdit = &(lights[2].min);
				break;

				case CHINTSTATE_BLUE_MAX:
				spectrumIntensityToEdit = &(lights[2].max);
				break;

				case CHINTSTATE_WHITE_MIN:
				spectrumIntensityToEdit = &(lights[3].min);
				break;

				case CHINTSTATE_WHITE_MAX:
				spectrumIntensityToEdit = &(lights[3].max);
				break;

				case CHINTSTATE_WARM_MIN:
				spectrumIntensityToEdit = &(lights[4].min);
				break;

				case CHINTSTATE_WARM_MAX:
				spectrumIntensityToEdit = &(lights[4].max);
				break;
			}
			brightnessBKP = *spectrumIntensityToEdit;
			lcd.setCursor(0,0);
			lcd.print(F("Set brightness: "));
			lcd.setCursor(0,1);
			lcd.print(F("                "));
			lcd.setCursor(11,1);
			if (brightnessBKP < 10) lcd.print(F("  "));
			else if (brightnessBKP < 100) lcd.print(F(" "));
			lcd.print(brightnessBKP);
			state = STATE_SET_SPECTRUM_BRIGHTNES;
			break;
		}
		break;

		case STATE_SET_SPECTRUM_BRIGHTNES:
		switch (UI_changeBrightness(spectrumIntensityToEdit)) {
			case UI_RET_DONE:
			EEPROM.put(EEPROM_ADDR_LIGHTS, lights);
			// FALL THROUGH

			case UI_RET_EXIT:
			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
			break;

			default:
			break;
		}
		break;


		case STATE_SET_BRIGHTNESS:
		switch (UI_changeBrightness(&brightness)) {
			case UI_RET_EXIT:
			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
			break;

			case UI_RET_DONE:
			EEPROM.put(EEPROM_ADDR_BRIGHTNESS, brightness);
			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
			break;

			default:
			break;
		}
		break;		

		
		case STATE_SET_TIME:
		//UI_changeTime(&setTimeTemp, &setTimeState);
		switch (UI_selectTime(&setTimeState)) {
			case UI_RET_EXIT:
			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
			break;
			
			case UI_RET_DONE:
			setTime(setTimeTemp.h, setTimeTemp.m, setTimeTemp.s, 20, 4, 2069);
			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
			break;
		}
		break;

		
		case STATE_SET_SCHEDULE_CHANNEL:
		switch (UI_selectChannel(&selectChannelState)) {
			case UI_RET_EXIT:
			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
			break;
			
			case UI_RET_DONE:
			lcd.setCursor(0,0);
			lcd.print(F("Select TimeStamp"));
			lcd.setCursor(0,1);
			lcd.print(F("Start of Sunrise"));
			selectTimeStampState = 0;
			state = STATE_SET_SCHEDULE_TIMESTAMP;
			break;
		}
		break;

		case STATE_SET_SCHEDULE_TIMESTAMP:
		switch (UI_selectTimeStamp(&selectTimeStampState)) {
			case UI_RET_EXIT:
			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
			break;

			case UI_RET_DONE:
			state = STATE_SET_SCHEDULE_TIME;
			setTimeTemp = lights[selectChannelState].times[selectTimeStampState];
			setTimeState = TIMESTATE_HOURS;
			lcd.setCursor(0,0);
			lcd.print(F("Change hours:   "));
			lcd.setCursor(0,1);
			lcd.print(F("                "));
			lcd.setCursor(4,1);
			LCD_printTime(&lcd, &setTimeTemp);
			break;
		}
		break;

		case STATE_SET_SCHEDULE_TIME:
		switch (UI_selectTime(&setTimeState)) {
			case UI_RET_EXIT:
			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
			break;

			case UI_RET_DONE:
			lights[selectChannelState].times[selectTimeStampState] = setTimeTemp;
			EEPROM.put(EEPROM_ADDR_LIGHTS, lights);

			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
			break;
		}
		break;
		

		default:
		//Serial.println("default");
		buttons.bulk = 0;
		LCD_state_toSleep(&lcd, &realTime);
		state = STATE_SLEEP;
		break;
	}
		
	update_buttons = 0;
	update_time = 0;

}

uint8_t UI_selectTime(uint8_t *timeState) {
	switch (*timeState) {
		case TIMESTATE_HOURS:
		if (buttons.b) {
			buttons.b = 0;
			LCD_state_toIdle(&lcd, &realTime);
			state = STATE_IDLE;
			return UI_RET_EXIT;
		} else if (buttons.a) {
			buttons.a = 0;
			setTimeState = TIMESTATE_MINUTES;
			lcd.setCursor(0,0);
			lcd.print(F("Change minutes: "));
		} else if (buttons.l) {
			buttons.l = 0;
			decHour(&setTimeTemp);
			lcd.setCursor(4,1);
			LCD_printTime(&lcd, &setTimeTemp);
		} else if (buttons.r) {
			buttons.r = 0;
			incHour(&setTimeTemp);
			lcd.setCursor(4,1);
			LCD_printTime(&lcd, &setTimeTemp);
		}
		break;

		case TIMESTATE_MINUTES:
		if (buttons.b) {
			buttons.b = 0;
			setTimeState = TIMESTATE_HOURS;
			lcd.setCursor(0,0);
			lcd.print(F("Change hours:   "));
		} else if (buttons.a) {
			buttons.a = 0;
			setTimeState = TIMESTATE_SECONDS;
			lcd.setCursor(0,0);
			lcd.print(F("Change seconds: "));
		} else if (buttons.l) {
			buttons.l = 0;
			decMinute(&setTimeTemp);
			lcd.setCursor(4,1);
			LCD_printTime(&lcd, &setTimeTemp);
		} else if (buttons.r) {
			buttons.r = 0;
			incMinute(&setTimeTemp);
			lcd.setCursor(4,1);
			LCD_printTime(&lcd, &setTimeTemp);
		}
		break;

		case TIMESTATE_SECONDS:
		if (buttons.b) {
			buttons.b = 0;
			setTimeState = TIMESTATE_MINUTES;
			lcd.setCursor(0,0);
			lcd.print(F("Change minutes: "));
		} else if (buttons.a) {
			buttons.a = 0;
			return UI_RET_DONE;
		} else if (buttons.l) {
			buttons.l = 0;
			decSecond(&setTimeTemp);
			lcd.setCursor(4,1);
			LCD_printTime(&lcd, &setTimeTemp);
		} else if (buttons.r) {
			buttons.r = 0;
			incSecond(&setTimeTemp);
			lcd.setCursor(4,1);
			LCD_printTime(&lcd, &setTimeTemp);
		}
		break;
	}

	return UI_RET_NOP;
}

uint8_t UI_changeBrightness(uint8_t *b) {
	if (buttons.b) {
		buttons.b = 0;
		*b = brightnessBKP;
		return UI_RET_EXIT;
	} else if (buttons.a) {
		buttons.a = 0;
		return UI_RET_DONE;
	} else if (buttons.l) {
		buttons.l = 0;
		*b -= 1;
		lcd.setCursor(11,1);
		if (*b < 10) lcd.print(F("  "));
		else if (*b < 100) lcd.print(F(" "));
		lcd.print(*b);
	} else if (buttons.r) {
		buttons.r = 0;
		*b += 1;
		lcd.setCursor(11,1);
		if (*b < 10) lcd.print(F("  "));
		else if (*b < 100) lcd.print(F(" "));
		lcd.print(*b);
	}

	return UI_RET_NOP;
}

uint8_t UI_displayValues(uint8_t *values) {
	if (buttons.b) {
		buttons.b = 0;
		return UI_RET_EXIT;
	} else if (buttons.a) {
		buttons.a = 0;
		return UI_RET_DONE;
	}

	if (update_time) {
		lcd.setCursor(0,1);
		for(uint8_t i = 0; i < 5; i++) {
			if (values[i] < 10) lcd.print(F("  "));
			else if (values[i] < 100) lcd.print(F(" "));
			lcd.print(values[i]);
		}
		lcd.print(F("  "));
	}

	return UI_RET_NOP;
}

uint8_t UI_selectChannel(int8_t *channelState) {
	uint8_t update = 0;
	if (buttons.b) {
		buttons.b = 0;
		LCD_state_toIdle(&lcd, &realTime);
		state = STATE_IDLE;
		return UI_RET_EXIT;
	} else if (buttons.a) {
		buttons.a = 0;
		return UI_RET_DONE;
	} else if (buttons.l) {
		buttons.l = 0;
		(*channelState)--;
		if (*channelState < 0) *channelState = CHSTATE_N - 1;
		update = 1;
	} else if (buttons.r) {
		buttons.r = 0;
		(*channelState)++;
		if (*channelState >= CHSTATE_N) *channelState = 0;
		update = 1;
	}
	
	if (update) {
		lcd.setCursor(0,1);
		switch(*channelState) {
			case CHSTATE_UV:
			lcd.print(F("Violet 425nm	"));
			break;

			case CHSTATE_ROYAL:
			lcd.print(F("Royal Blue 450nm"));
			break;

			case CHSTATE_BLUE:
			lcd.print(F("Blue 465nm	 "));
			break;

			case CHSTATE_WHITE:
			lcd.print(F("Cold White 6500K"));
			break;

			case CHSTATE_WARM:
			lcd.print(F("Warm White 4500K"));
			break;
		}
	}

	return UI_RET_NOP;
}

uint8_t UI_selectTimeStamp(int8_t *stampState) {
	uint8_t update = 0;
	if (buttons.b) {
		buttons.b = 0;
		LCD_state_toIdle(&lcd, &realTime);
		state = STATE_IDLE;
		return UI_RET_EXIT;
	} else if (buttons.a) {
		buttons.a = 0;
		return UI_RET_DONE;
	} else if (buttons.l) {
		buttons.l = 0;
		(*stampState)--;
		if (*stampState < 0) *stampState = SCHEDULESTATE_N - 1;
		update = 1;
	} else if (buttons.r) {
		buttons.r = 0;
		(*stampState)++;
		if (*stampState >= SCHEDULESTATE_N) *stampState = 0;
		update = 1;
	}
	
	if (update) {
		lcd.setCursor(0,1);
		switch(*stampState) {
			case SCHEDULESTATE_SUNRISE_START:
			lcd.print(F("Start of Sunrise"));
			break;

			case SCHEDULESTATE_SUNRISE_END:
			lcd.print(F("End of Sunrise  "));
			break;

			case SCHEDULESTATE_SUNSET_START:
			lcd.print(F("Start of Sunset "));
			break;

			case SCHEDULESTATE_SUNSET_END:
			lcd.print(F("End of Sunset   "));
			break;

			default:
			return UI_RET_EXIT;
		}
	}

	return UI_RET_NOP;
}

uint8_t UI_selectChannelIntensity(int8_t *chIntState) {
	uint8_t update = 0;
	if (buttons.b) {
		buttons.b = 0;
		LCD_state_toIdle(&lcd, &realTime);
		state = STATE_IDLE;
		return UI_RET_EXIT;
	} else if (buttons.a) {
		buttons.a = 0;
		return UI_RET_DONE;
	} else if (buttons.l) {
		buttons.l = 0;
		(*chIntState)--;
		if (*chIntState < 0) *chIntState = CHINTSTATE_N - 1;
		update = 1;
	} else if (buttons.r) {
		buttons.r = 0;
		(*chIntState)++;
		if (*chIntState >= CHINTSTATE_N) *chIntState = 0;
		update = 1;
	}

	
	if (update) {
		lcd.setCursor(0,1);
		switch(*chIntState) {
			case CHINTSTATE_UV_MIN:
			lcd.print(F("Min Violet	     "));
			break;

			case CHINTSTATE_UV_MAX:
			lcd.print(F("Max Violet	     "));
			break;

			case CHINTSTATE_ROYAL_MIN:
			lcd.print(F("Min Royal Blue  "));
			break;

			case CHINTSTATE_ROYAL_MAX:
			lcd.print(F("Max Royal Blue  "));
			break;

			// TODO : this down here
			case CHINTSTATE_BLUE_MIN:
			lcd.print(F("Min Blue        "));
			break;

			case CHINTSTATE_BLUE_MAX:
			lcd.print(F("Max Blue        "));
			break;

			case CHINTSTATE_WHITE_MIN:
			lcd.print(F("Min Cold White  "));
			break;

			case CHINTSTATE_WHITE_MAX:
			lcd.print(F("Max Cold White  "));
			break;

			case CHINTSTATE_WARM_MIN:
			lcd.print(F("Min Warm White  "));
			break;

			case CHINTSTATE_WARM_MAX:
			lcd.print(F("Max Warm White  "));
			break;

			default:
			return UI_RET_EXIT;
		}
	}

	return UI_RET_NOP;
}

/**
uint8_t detectPowerloss() {
	uint16_t value = analogRead(POWERLOSS_PIN);
	Serial.println(value);
	if (value > 1010) {
		EEPROM.write(EEADDRESS, realTime.s);
		EEPROM.write(EEADDRESS+1, realTime.h);
		EEPROM.write(EEADDRESS+2, realTime.m);
		digitalWrite(13, HIGH);
		Serial.print(realTime.h);
		Serial.print(realTime.m);
		Serial.println(realTime.s);
	}
	return 1;
}
*/

void Error(char *str) {
	//Serial.println(str);
	digitalWrite(13, HIGH);
	while (true);
}
