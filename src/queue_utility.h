#ifndef QUEUE_UTIL_H
#define QUEUE_UTIL_H
#include "green.h"

typedef struct green_t green_t;

green_t* dequeue(green_t** queue_head);
void enqueue(green_t** queue_head, green_t* thread);

#endif // !QUEUE_UTIL_H
