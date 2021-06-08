#include "times.h"
#include <stdint.h>


int32_t cmptimes( Times_t *t1, Times_t *t2){ // t1-t2 in s
    return time2seconds(t1) - time2seconds(t2);
}

int32_t time2minutes( Times_t *t ){
    return t->h*60 + t->m;
}

int32_t time2seconds( Times_t *t ){
    return time2minutes(t)*60 + t->s;
}



int8_t decHour(Times_t *t) {
    if (t->h == 0) {
        t->h = 23;
        return -1;
    } else {
        t->h--;
        return 0;
    }
}

int8_t decMinute(Times_t *t) {
    if (t->m == 0) {
        t->m = 59;
        return -1;
    } else {
        t->m--;
        return 0;
    }
}

int8_t decSecond(Times_t *t) {
    if (t->s == 0) {
        t->s = 59;
        return -1;
    } else {
        t->s--;
        return 0;
    }
}

void decTime(Times_t *t) {
    if (decSecond(t) != 0) {
        if (decMinute(t) != 0) {
            decHour(t);
        }
    }
}


int8_t incHour(Times_t *t) {
    if (t->h == 23) {
        t->h = 0;
        return 1;
    } else {
        t->h++;
        return 0;
    }
}

int8_t incMinute(Times_t *t) {
    if (t->m == 59) {
        t->m = 0;
        return 1;
    } else {
        t->m++;
        return 0;
    }
}

int8_t incSecond(Times_t *t) {
    if (t->s == 59) {
        t->s = 0;
        return 1;
    } else {
        t->s++;
        return 0;
    }
}
void incTime(Times_t *t) {
    if (incSecond(t) != 0) {
        if (incMinute(t) != 0) {
            incHour(t);
        }
    }
}
