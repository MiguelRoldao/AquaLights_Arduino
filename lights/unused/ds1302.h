#ifndef _MIRO_DS1302_H
#define _MIRO_DS1302_H


#include <stdint.h>


void DS1302_Init (uint8_t clock_pin, uint8_t data_pin, uint8_t enable_pin);

uint8_t DS1302_Seconds();
uint8_t DS1302_Minutes();
uint8_t DS1302_Hours();

void DS1302_WriteByte (uint8_t addr, uint8_t data);
uint8_t DS1302_ReadByte (uint8_t addr);

#endif
