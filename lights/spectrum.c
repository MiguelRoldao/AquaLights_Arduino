#include "spectrum.h"
#include "times.h"

uint8_t _MathFallFunc( Times_t *times, Spectrum_t *spec){ // In this case its linear
    int32_t t = time2seconds( times );
    int32_t t2 = time2seconds( &spec->sunset_end );
    int32_t t1 = time2seconds( &spec->sunset_start );
    int32_t res = spec->max + (t - t1)*(spec->min - spec->max)/(t2 - t1);
    if(res > 0xff) res = 0xff;
    if(res < 0x00) res = 0x00;
    return (uint8_t)res;
}

uint8_t _MathRiseFunc( Times_t *times, Spectrum_t *spec){ // In this case its linear
    int32_t t = time2seconds( times );
    int32_t t2 = time2seconds( &spec->sunrise_end);
    int32_t t1 = time2seconds( &spec->sunrise_start );
    int32_t res = spec->min + (t - t1)*(spec->max - spec->min)/(t2 - t1);
    if(res > 0xff) res = 0xff;
    if(res < 0x00) res = 0x00;
    return (uint8_t)res;
}

uint8_t SPECTRUM_Power( Spectrum_t *spec, Times_t *t ){ // math function
    if( cmptimes(t, &spec->sunrise_start) < 0 || cmptimes(t, &spec->sunset_end) > 0 ) {
        return spec->min;
    } else if( cmptimes(t, &spec->sunrise_end) > 0 && cmptimes(t, &spec->sunset_start) < 0) {
        return spec->max;
    } else if( cmptimes(t, &spec->sunrise_end) < 0) {
        return _MathRiseFunc(t, spec);
    } else {
        return _MathFallFunc(t, spec);
    }
}
