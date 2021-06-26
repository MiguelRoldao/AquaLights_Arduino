#ifndef _MIRO_SPECTRUM_H
#define _MIRO_SPECTRUM_H

#include <stdint.h>
#include "times.h"

typedef union {
    struct {
        Times_t sunrise_start;
        Times_t sunrise_end;
        Times_t sunset_start;
        Times_t sunset_end;
        uint8_t max;
        uint8_t min;
    };
    struct {
        Times_t times[4];
        uint8_t intensities[2];
    };
} Spectrum_t;


#ifdef __cplusplus
extern "C" {
#endif

uint8_t SPECTRUM_Power( Spectrum_t *spec, Times_t *t );

#ifdef __cplusplus
}
#endif


#endif
