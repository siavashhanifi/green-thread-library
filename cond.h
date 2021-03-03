#ifndef COND_H
#define COND_H
#include "globals.h"

typedef struct green_cond_t {
    green_t* susp_list;
} green_cond_t;

void green_cond_init(green_cond_t* green_cond);
void green_cond_signal(green_cond_t* green_cond);
void green_cond_broadcast(green_cond_t* green_cond);
int green_cond_wait(green_cond_t* green_cond, green_mutex_t* green_mutex);

#endif // !COND_H
