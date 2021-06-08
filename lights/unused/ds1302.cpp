#include "Arduino.h"
#include <stdint.h>
#include <PCF8574.h>
#include <Wire.h>
#include "ds1302.h"


uint8_t _sclk_pin;
uint8_t _io_pin;
uint8_t _ce_pin;


// aux functions
void _start_comms ();
void _end_comms ();
void _write_byte (uint8_t data, uint8_t follow_read);
uint8_t _read_byte ();



void DS1302_Init (uint8_t clock_pin, uint8_t data_pin, uint8_t enable_pin) {
    _sclk_pin = clock_pin;
    _io_pin = data_pin;
    _ce_pin = enable_pin;
}

uint8_t DS1302_Seconds() {
    uint8_t seconds = 0;
    uint8_t temp = DS1302_ReadByte(0x81);

    seconds += temp & B00001111;
    seconds += ((temp & B01110000) >> 4) * 10;

    return seconds;
}

uint8_t DS1302_Minutes() {
    uint8_t minutes = 0;
    uint8_t temp = DS1302_ReadByte(0x83);

    minutes += temp & B00001111;
    minutes += ((temp & B01110000) >> 4) * 10;

    return minutes;
}

uint8_t DS1302_Hours() {
    uint8_t hours = 0;
    uint8_t temp = DS1302_ReadByte(0x83);

    hours += temp & B00001111;
    hours += ((temp & B00110000) >> 4) * 10;

    return hours;
}

void DS1302_WriteByte (uint8_t addr, uint8_t data) {
    addr |= B10000000; // set required bits 
    addr &= B11111110; // reset required bits 
    
    _start_comms();

    _write_byte(addr, 0);
    _write_byte(data, 0);

    _end_comms();
}

uint8_t DS1302_ReadByte (uint8_t addr) {
    uint8_t data;
    
    addr |= B10000001; // set required bits 
    
    _start_comms();

    _write_byte(addr, 1);
    data = _read_byte();

    _end_comms();

    return data;
}



void _start_comms () {
    pinMode(_ce_pin, OUTPUT);
    digitalWrite(_ce_pin, LOW);
    
    pinMode(_sclk_pin, OUTPUT);
    digitalWrite(_sclk_pin, LOW);

    pinMode(_io_pin, OUTPUT);

    digitalWrite(_ce_pin, HIGH);
    delayMicroseconds(4);
}

void _end_comms () {
    pinMode(_io_pin, INPUT);
    
    digitalWrite(_ce_pin, LOW);
    delayMicroseconds(4);
}


void _write_byte (uint8_t data, uint8_t follow_read) {
    for(uint8_t i = 0; i < 8; i++) {
        digitalWrite(_sclk_pin, LOW);
        digitalWrite(_io_pin, bitRead(data, i));
        delayMicroseconds(1);
        digitalWrite(_sclk_pin, HIGH);
        delayMicroseconds(1);
    }
    
    if (follow_read)
        pinMode(_io_pin, INPUT);
        // leaves sclk high
    else
        digitalWrite(_sclk_pin, LOW);
}

uint8_t _read_byte () {
    uint8_t data = 0;
    
    for(uint8_t i = 0; i < 8; i++) {
        digitalWrite(_sclk_pin, HIGH);
        delayMicroseconds(1);
        digitalWrite(_sclk_pin, LOW);
        delayMicroseconds(1);
        bitWrite(data, i, digitalRead(_io_pin));
    }
    Serial.print("_read_byte: ");
    Serial.println(data);
    return data;
}
