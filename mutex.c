#include "mutex.h"

int green_mutex_init(green_mutex_t* mutex) {
    sigprocmask(SIG_BLOCK, &block, NULL);
    mutex->taken = FALSE;
    mutex->susp_list = NULL;
    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}

int green_mutex_lock(green_mutex_t* mutex) {
    //block timer interrupt
    sigprocmask(SIG_BLOCK, &block, NULL);

    green_t* susp = running;
    if (mutex->taken) {
        //suspend the running thread
        enqueue(&mutex->susp_list, susp);
        //find the next thread
        green_t* next = rqueue_deq();
        running = next;
        swapcontext(susp->context, next->context);
    }
    else
        mutex->taken = TRUE;

    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}

int green_mutex_unlock(green_mutex_t* mutex) {
    sigprocmask(SIG_BLOCK, &block, NULL);
    if (mutex->susp_list != NULL)
    {
        // move suspended thread to ready queue
        rqueue_enq(dequeue(&mutex->susp_list));
    }
    else
    {
        mutex->taken = FALSE;
    }
    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}