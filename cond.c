#include "cond.h"

void green_cond_init(green_cond_t* green_cond) {
    assert(green_cond != NULL);
    sigprocmask(SIG_BLOCK, &block, NULL);
    green_cond->susp_list = NULL;
    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

void green_cond_signal(green_cond_t* green_cond) {
    assert(green_cond != NULL);
    sigprocmask(SIG_BLOCK, &block, NULL);

    if (green_cond->susp_list != NULL) {
        green_t *next = dequeue(&(green_cond->susp_list));
        enqueue(&rqueue_head, next);
    }

    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

void green_cond_broadcast(green_cond_t* green_cond)
{
    assert(green_cond != NULL);
    sigprocmask(SIG_BLOCK, &block, NULL);

    green_t* to_wake = NULL;
    green_t* itr = green_cond->susp_list;

    while (itr != NULL) {
        itr = itr->next;
        to_wake = dequeue(&(green_cond->susp_list));
        enqueue(&rqueue_head, to_wake);
    }

    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

int green_cond_wait(green_cond_t* green_cond, green_mutex_t* green_mutex) {
    //block timer interrupt
    sigprocmask(SIG_BLOCK, &block, NULL);

    //suspend the running thread on condition
    green_t* susp = running;
    enqueue(&green_cond->susp_list, susp);

    if (green_mutex != NULL) {
        //release the lock if we have a mutex
        green_mutex->taken = FALSE;

        //move suspended thread to ready queue
        if (green_mutex->susp_list != NULL) {
            green_t* ready = dequeue(&green_mutex->susp_list);
            enqueue(&rqueue_head, ready);
        }

    }

    //schedule the next thread
    cntx_swap_with_next(susp);

    if (green_mutex != NULL) {
        //try to take the lock
        green_mutex_lock(green_mutex);
    }

    //unblock
    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}

