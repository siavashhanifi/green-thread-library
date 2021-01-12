#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <assert.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include "green.h"

#define FALSE 0
#define TRUE 1

#define PERIOD 100

#define STACK_SIZE 4096

static ucontext_t main_cntx = {0};
static green_t main_green = {&main_cntx, NULL, NULL, NULL, NULL, NULL, FALSE}; //CUSTM ALTERED
static green_t *running = &main_green;
static void init() __attribute__((constructor));


/*********QUEUE UTILITY START *******/
void enqueue(green_t **queue_head, green_t *thread)
{
    assert(thread != NULL && queue_head != NULL);

    if (*queue_head == NULL){
        *queue_head = thread;
        return;
    }
    green_t *tail = NULL;
    green_t *itr = *queue_head;
    //find tile and point to it with itr
    while(TRUE){
        if (itr->next == NULL)
            break;
        itr = itr->next;
    }
    tail = itr;
    //insert at tail
    tail->next = thread;

}

green_t *dequeue(green_t **queue_head)
{
    assert(queue_head != NULL);

    //If susp_list empty
    if(*queue_head == NULL){
        fprintf(stderr, "dequeue(): Attempt to remove head from empty list, returning NULL\n");
        return NULL;
    }
    
    green_t *old_head; 

    old_head  = *queue_head;
    
    //if has next
    if(old_head->next != NULL)
        //set susp_list to point to susp_list->next
        *queue_head = old_head->next;
    else
        *queue_head = NULL;
    

    old_head->next = NULL;

    return old_head;
}

void *funA(void *arg){
    printf("A\n");
    return NULL;
}

void *funB(void *arg){
    printf("B\n");
    return NULL;
}

void *funC(void *arg){
    printf("C\n");
    return NULL;
}

void test_susplist()
{
    green_t *a, *b, *c;
    a = malloc(sizeof(green_t));
    b = malloc(sizeof(green_t));
    c = malloc(sizeof(green_t));

    a->fun = &funA;
    b->fun = &funB;
    c->fun = &funC;

    green_t *susplist = NULL;

    enqueue((green_t **)&susplist, a);
    enqueue((green_t **)&susplist, b);
    enqueue((green_t **)&susplist, c);

    c = dequeue((green_t **)&susplist);
    b = dequeue((green_t **)&susplist);
    a = dequeue((green_t **)&susplist);

    (c->fun)(NULL);
    (b->fun)(NULL);
    (a->fun)(NULL);

    free(a);
    free(b);
    free(c);
}

/*int main(){
   test_susplist();
   return 0; 
}*/
/************Queue Utility end*************/

/***********RQUEUE START*************/
green_t *rqueue_head = NULL; //CUSTM

void rqueue_enq(green_t *thread){ //CUSTM
    assert(thread != NULL);

    if (rqueue_head == NULL){
        rqueue_head = thread;
        return;
    }
    green_t *tail = NULL;
    green_t *itr = rqueue_head;
    //find tailthread != NULL && s and point to it with itr
    while(TRUE){
        if (itr->next == NULL)
            break;
        itr = itr->next;
    }
    tail = itr;
    //insert at tail
    tail->next = thread;
}

green_t *rqueue_deq(){
    //If susp_list empty
    if(rqueue_head == NULL){
        fprintf(stderr, "rqueue_deq(): Attempt to remove head from empty list, returning NULL\n");
        return NULL;
    }
    
    green_t *old_head; 

    old_head  = rqueue_head;
    
    //if has next
    if(old_head->next != NULL)
        //set susp_list to point to susp_list->next
        rqueue_head = old_head->next;
    else
        rqueue_head = NULL;

    old_head->next = NULL;

    return old_head;

}

void test_rqueue(){
    green_t *a,*b,*c;
    a = (green_t*)malloc(sizeof(green_t));
    b = (green_t*)malloc(sizeof(green_t));
    c = (green_t*)malloc(sizeof(green_t));

    a->fun = funA;
    b->fun = funB;
    c->fun = funC;

    //a->next = b;
    //b->next = c;

    rqueue_enq(a);
    rqueue_enq(b);
    rqueue_enq(c);
    (*rqueue_deq()->fun)(NULL);
    (*rqueue_deq()->fun)(NULL);
    (*rqueue_deq()->fun)(NULL);
    
    free(a);
    free(b);
    free(c);
    rqueue_deq(); //print error message
}

/*int main(){
test_rqueue();
return 0;
}*/

/**********RQUEUE_END***************/

/*interrupts_start*/
static sigset_t block;

void timer_handler(int sig){
    green_t *susp = running;
    
   /* char *msg = "interrupted\n\0";
    int rc = write(1 ,msg,13);
    assert(rc == 13);
    */
   
    //add the running thread to the ready queue;
    rqueue_enq(susp);

    //find the next thread for execution
    green_t *next = rqueue_deq();
    running = next;
    swapcontext(susp->context, next->context);
}

void init() {
    sigemptyset(&block);
    sigaddset(&block, SIGVTALRM);

    struct sigaction act = {0};
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
    green_t *this = running;
    sigprocmask(SIG_UNBLOCK, &block, NULL);

    //runs the thread(routine), stores return value in result
    void *result = (*this->fun)(this->arg); //CUSTM ALTERED; calls function of the thread.
    
    sigprocmask(SIG_BLOCK, &block, NULL);
    //place waiting (joining) thread in ready queue
    if(this->join != NULL){
        rqueue_enq(this->join); //CUSTOM'D WRONG?
        this->join = NULL;
    }
    
    //save result of execution
    this->retval = result;

    //we're a zombie
    this->zombie = TRUE;

    //find the next thread to run
    green_t *next = rqueue_deq(); //CUSTM 
    running = next;
    setcontext(next->context);
    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

int green_create (green_t *new, void *(*fun)(void*), void*arg) {
    sigprocmask(SIG_BLOCK, &block, NULL);

    ucontext_t *cntx = (ucontext_t*)malloc(sizeof(ucontext_t));
    getcontext(cntx);
    void *stack = malloc(STACK_SIZE);
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
    rqueue_enq(new);//CUSTM

    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}

int green_yield() {
    sigprocmask(SIG_BLOCK, &block, NULL);

    green_t *susp = running; 

   //add susp to ready queue
    rqueue_enq(susp);
   
   //select the next thread for execution
    green_t *next = rqueue_deq();
    running = next;
    swapcontext(susp->context, next->context);

    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}

int green_join(green_t *thread, void** val) {
sigprocmask(SIG_BLOCK, &block, NULL);

 if(!thread->zombie){
    green_t *susp = running;

    //add as joining thread
    thread->join = susp; //susp is added to ready queue when 

    //select the next thread for execution
    green_t *next = rqueue_deq();
    running = next;
    swapcontext(susp->context, next->context);
 }
 //collect result
 if(val != NULL)
    *val = thread->retval;

 //free context
 free(thread->context->uc_stack.ss_sp);
 free(thread->context);

 sigprocmask(SIG_UNBLOCK, &block, NULL);
 return 0;
}

void insrt_head(green_t **susp_list, green_t *head)
{
    assert(head != NULL && susp_list != NULL);

    if ((green_t *)*susp_list != NULL)
    {
        green_t *old_head = (green_t *)*susp_list;
        head->next = old_head;
    }
    *susp_list = head;
}

/***** mutex start *****/

int green_mutex_init(green_mutex_t *mutex){
    sigprocmask(SIG_BLOCK, &block, NULL);
    mutex->taken = FALSE;
    mutex->susp_list = NULL;
    sigprocmask(SIG_UNBLOCK, &block, NULL);

    return 0;
}

int green_mutex_lock(green_mutex_t *mutex){
    //block timer interrupt
    sigprocmask(SIG_BLOCK, &block, NULL);

    green_t *susp = running;
    if(mutex->taken) {
        //suspend the running thread
        enqueue(&mutex->susp_list, susp);
        //find the next thread
        green_t *next = rqueue_deq();
        running = next;
        swapcontext(susp->context, next->context);
    }else
    {
        mutex->taken = TRUE;
    }
    
    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}

int green_mutex_unlock(green_mutex_t *mutex){
    sigprocmask(SIG_BLOCK, &block, NULL);
    if (mutex->susp_list != NULL)
    {
        // move suspended thread to ready queue
        rqueue_enq(dequeue(&mutex->susp_list));
    }else
    {
        mutex->taken = FALSE;
    }
    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}

/***** mutex end *****/


/********conditional variable************/

void green_cond_init(green_cond_t *green_cond)
{
    assert(green_cond != NULL);
    sigprocmask(SIG_BLOCK, &block, NULL);
    green_cond->susp_list = NULL;    
    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

void green_cond_wait_old(green_cond_t *green_cond)
{
    assert(green_cond != NULL);
    sigprocmask(SIG_BLOCK, &block, NULL);

    green_t *this = running;

    //add to susplist
    enqueue(&(green_cond->susp_list), this);
    
    //run next

    green_t *next = rqueue_deq();
    running = next;
    swapcontext(this->context, next->context);

    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

void green_cond_signal(green_cond_t *green_cond){
    assert(green_cond != NULL);
    sigprocmask(SIG_BLOCK, &block, NULL);

    if(green_cond->susp_list != NULL )
        rqueue_enq(dequeue(&(green_cond->susp_list)));

    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

void green_cond_broadcast(green_cond_t *green_cond)
{
    assert(green_cond != NULL);
    sigprocmask(SIG_BLOCK, &block, NULL);

    green_t *to_wake = NULL;
    green_t *itr = green_cond->susp_list; 

    while(itr != NULL){
        itr = itr->next;
        to_wake = dequeue(&(green_cond->susp_list));
        rqueue_enq(to_wake);
    }

    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

int green_cond_wait(green_cond_t *green_cond, green_mutex_t *green_mutex){
    //block timer interrupt
    sigprocmask(SIG_BLOCK, &block, NULL);

    //suspend the running thread on condition
    green_t *susp = running;
    enqueue(&green_cond->susp_list, susp);


    if(green_mutex != NULL) {
        //release the lock if we have a mutex
        green_mutex->taken = FALSE;

        //move suspended thread to ready queue
        if(green_mutex->susp_list != NULL){
            green_t *ready = dequeue(&green_mutex->susp_list);
            rqueue_enq(ready);
        }

    }

    //schedule the next thread
    green_t *next = rqueue_deq();
    running = next;
    swapcontext(susp->context, next->context);

    if(green_mutex != NULL) {
        //try to take the lock
        green_mutex_lock(green_mutex);
        /*if(green_mutex->taken){
            //bad luck, suspend
            susp = running;
            enqueue(&green_mutex->susp_list, susp);
            
            next = rqueue_deq();
            running = next;
            swapcontext(susp->context, next->context);
        } else {
            //take the lock
            green_mutex->taken = TRUE;
        }*/
    }


    //unblock
    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}

/***** conditional variables end *****/

