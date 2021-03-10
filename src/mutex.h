#ifndef MUTEX_H
#define MUTEX_H
#include "green.h"
#include "globals.h"

typedef struct green_t green_t;

typedef struct green_mutex_t {
    volatile int taken;
    //handle the list
    green_t* susp_list;
} green_mutex_t;

int green_mutex_init(green_mutex_t* mutex);
int green_mutex_lock(green_mutex_t* mutex);
int green_mutex_unlock(green_mutex_t* mutex);

#endif // !1
