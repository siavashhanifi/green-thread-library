#include "utility.h"

void cntx_swap_with_next(green_t* susp) {
    green_t* next = dequeue(&rqueue_head);
    running = next;
    swapcontext(susp->context, next->context);
}

void cntx_set_next() {
    green_t* next = dequeue(&rqueue_head); //CUSTM 
    running = next;
    setcontext(next->context);
}