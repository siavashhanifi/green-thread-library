#include "green.h"

void timer_handler(int sig) {
    green_yield();
}

void init() {

    sigemptyset(&block);
    sigaddset(&block, SIGVTALRM);

    struct sigaction act = { 0 };
    struct timeval interval;
    struct itimerval period;

    act.sa_handler = timer_handler;
    assert(sigaction(SIGVTALRM, &act, NULL) == 0);
    interval.tv_sec = 0;
    interval.tv_usec = PERIOD;
    period.it_interval = interval;
    period.it_value = interval;

    setitimer(ITIMER_VIRTUAL, &period, NULL);
    /*interrupts_end*/

    getcontext(&main_cntx);
}

void green_thread() {
    sigprocmask(SIG_BLOCK, &block, NULL);
    green_t* this = running;
    sigprocmask(SIG_UNBLOCK, &block, NULL);

    //runs the thread(routine), stores return value in result
    void* result = (*this->fun)(this->arg); //CUSTM ALTERED; calls function of the thread.

    sigprocmask(SIG_BLOCK, &block, NULL);
    //place waiting (joining) thread in ready queue
    if (this->join != NULL) {
        enqueue(&rqueue_head, this->join);
        this->join = NULL;
    }

    //save result of execution
    this->retval = result;

    //we're a zombie
    this->zombie = TRUE;

    //find the next thread to run
    cntx_set_next();

    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

int green_create(green_t* new, void* (*fun)(void*), void* arg) {
    sigprocmask(SIG_BLOCK, &block, NULL);

    ucontext_t* cntx = (ucontext_t*)malloc(sizeof(ucontext_t));
    getcontext(cntx);
    void* stack = malloc(STACK_SIZE);
    cntx->uc_stack.ss_sp = stack;
    cntx->uc_stack.ss_size = STACK_SIZE;
    makecontext(cntx, green_thread, 0); //CUSTOM new <-> green_thread

    new->context = cntx;
    new->fun = fun;
    new->arg = arg;
    new->next = NULL;
    new->join = NULL;
    new->retval = NULL;
    new->zombie = FALSE;

    //add new to ready queue
    enqueue(&rqueue_head, new);

    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}

int green_yield() {
    sigprocmask(SIG_BLOCK, &block, NULL);

    green_t* susp = running;

    //add susp to ready queue
    enqueue(&rqueue_head, susp);

    //select the next thread for execution
    cntx_swap_with_next(susp);

    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}

int green_join(green_t* thread, void** val) {
    sigprocmask(SIG_BLOCK, &block, NULL);

    if (!thread->zombie) {
        green_t* susp = running;

        //add as joining thread
        thread->join = susp; //susp is added to ready queue when 

        //select the next thread for execution
        cntx_swap_with_next(susp);
    }
    //collect result
    if (val != NULL)
        *val = thread->retval;

    //free context
    free(thread->context->uc_stack.ss_sp);
    free(thread->context);

    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}

