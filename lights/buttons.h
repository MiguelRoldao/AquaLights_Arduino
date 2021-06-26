#ifndef _MIRO_AQUARIUMLIGHTS_BUTTONS_H_
#define _MIRO_AQUARIUMLIGHTS_BUTTONS_H_

#include <stdint.h>
#include <PCF8574.h>

typedef union {
	struct {
		uint8_t RESERVED0 : 4;
		uint8_t a : 1;
		uint8_t b : 1;
		uint8_t r : 1;
		uint8_t l : 1;
	};
	uint8_t bulk;
} Buttons_t;


extern volatile uint8_t is_read_buttons;


void buttons_init(PCF8574 *pcf8574);
void readButtons(Buttons_t *b);

void handleInputISR ();


#endif
