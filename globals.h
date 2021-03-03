#include "green.h"

ucontext_t main_cntx = { 0 };
green_t main_green = { &main_cntx, NULL, NULL, NULL, NULL, NULL, FALSE }; //CUSTM ALTERED
green_t* running = &main_green;
green_t* rqueue_head = NULL; //CUSTM
sigset_t block;

