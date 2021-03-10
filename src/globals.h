#ifndef GLOBALS_H
#define GLOBALS_H
#include <ucontext.h>
#include <signal.h>
#include "green.h"

typedef struct green_t green_t;

extern ucontext_t main_cntx;
extern green_t main_green;
extern green_t* running;
extern green_t* rqueue_head;
extern sigset_t block;

#endif // !GLOBALS_H
