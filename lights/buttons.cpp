
#include <PCF8574.h>
#include "buttons.h"

PCF8574 *buttons_pcf;
volatile uint8_t is_read_buttons;

void buttons_init(PCF8574 *pcf8574)
{
	is_read_buttons = 0;
	buttons_pcf = pcf8574;
}

void readButtons(Buttons_t *b)
{
	b->a = buttons_pcf->read(4);
	b->b = buttons_pcf->read(5);
	b->r = buttons_pcf->read(6);
	b->l = buttons_pcf->read(7);
}

void handleInputISR()
{
	is_read_buttons = 1;
	
}
