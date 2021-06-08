#include <EEPROM.h>
#include "/home/miguelito/Documents/c/arduino/aquarium_lights/lights/spectrum.h"


#define EEPROM_ADDR_LIGHTS 16
#define EEPROM_ADDR_BRIGHTNESS 15

#define N_CHANNELS 5

Spectrum_t lights[N_CHANNELS];
uint8_t brightness;

void setup() {
    for (int i = 0; i < N_CHANNELS; i++) {
        lights[i].sunrise_start = {9, 30, 0};
        lights[i].sunrise_end = {11, 0, 0};
        lights[i].sunset_start = {20, 0, 0};
        lights[i].sunset_end = {21, 30, 0};
        lights[i].max = 255;
        lights[i].min = 0;
    }

    EEPROM.put(EEPROM_ADDR_BRIGHTNESS, 50);
    EEPROM.put(EEPROM_ADDR_LIGHTS, lights);
}

void loop() {
    while(1);    
}
