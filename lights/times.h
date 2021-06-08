#ifndef _MIRO_TIMES_H
#define _MIRO_TIMES_H

#include <stdint.h>

typedef struct {
    uint8_t h;
    uint8_t m;
    uint8_t s;
} Times_t;


#ifdef __cplusplus
extern "C" {
#endif

int32_t cmptimes( Times_t *t1, Times_t *t2 );
int32_t time2minutes( Times_t *t );
int32_t time2seconds( Times_t *t );

int8_t decHour(Times_t *t);
int8_t decMinute(Times_t *t);
int8_t decSecond(Times_t *t);
void decTime(Times_t *t);

int8_t incHour(Times_t *t);
int8_t incMinute(Times_t *t);
int8_t incSecond(Times_t *t);
void incTime(Times_t *t);

#ifdef __cplusplus
}
#endif


#endif
