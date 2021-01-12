#include <ucontext.h>

typedef struct green_t {
    ucontext_t *context;
    void *(*fun)(void*);
    void *arg;
    struct green_t *next;
    struct green_t *join;
    void *retval;
    int zombie;
} green_t;

int green_create(green_t *thread, void *(*fun)(void*), void *arg);
int green_yield();
int green_join(green_t *thread, void** val);

/***** mutex *****/

typedef struct green_mutex_t {
    volatile int taken;
    //handle the list
    green_t *susp_list;
} green_mutex_t;

int green_mutex_init(green_mutex_t *mutex);
int green_mutex_lock(green_mutex_t *mutex);
int green_mutex_unlock(green_mutex_t *mutex);

/***** mutex end *****/

/***** conditional variables ******/

typedef struct green_cond_t{
    green_t *susp_list;
} green_cond_t;

void green_cond_init(green_cond_t *green_cond);
int green_cond_wait(green_cond_t *green_cond, green_mutex_t *green_mutex);
void green_cond_wait_old(green_cond_t *green_cond);
void green_cond_signal(green_cond_t *green_cond);
void green_cond_broadcast(green_cond_t *green_cond);

/***** conditional variables end ******/