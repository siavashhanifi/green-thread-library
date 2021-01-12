#include<unistd.h>
#include<stdio.h>
#include<assert.h>
#include<malloc.h>
#include "green.h"

#define TRUE 1
#define FALSE 0

static int volatile sum = 0;

green_mutex_t sum_lock;
int subtracted = FALSE;
green_cond_t subtrctd_cnd;
green_mutex_t subtrctd_cnd_lock;



typedef struct { int prev_val; int next_val; } myret_t;

void *metr_add(void *args){    
    myret_t *rvals = malloc(sizeof(myret_t)); 
    //Wait for metr_sub() to run first
    //Lock for resource: subtracted(condvar)   
    green_mutex_lock(&subtrctd_cnd_lock);
    while(!subtracted)
        //Put thread to sleep: Unlock for resource: subtracted(condvar), wait for signal, re-aquire lock.
        green_cond_wait(&subtrctd_cnd, &subtrctd_cnd_lock);
    //Unlock for resource: subtracted(condvar)
    green_mutex_unlock(&subtrctd_cnd_lock);

    //Lock for resource: sum
    green_mutex_lock(&sum_lock);
    rvals->prev_val = sum;
    
    for(int i = 0; i < 2000000; i++)
        sum++;

    rvals->next_val = sum;
    //Unlock for resource: sum
    green_mutex_unlock(&sum_lock);
    return (void *) rvals;
}

void *metr_sub(void *args){
    myret_t *rvals = malloc(sizeof(myret_t));
    //Lock for resource: sum
    green_mutex_lock(&sum_lock);
    rvals->prev_val = sum;

    for(int i = 0; i < 2000000; i++)
        sum--;

    rvals->next_val = sum;
    //Unlock for resource: sum
    green_mutex_unlock(&sum_lock);

    //Lock for resource: subtracted(condvar) 
    green_mutex_lock(&subtrctd_cnd_lock);
    subtracted = TRUE;
    //Signal sleeping thread(s) waiting for subtraction to be occurred;
    green_cond_signal(&subtrctd_cnd);
    //Unlock for resource: subtracted(condvar)
    green_mutex_unlock(&subtrctd_cnd_lock);
    return (void *) rvals;
}

int main(){

    green_mutex_init(&sum_lock);
    green_mutex_init(&subtrctd_cnd_lock);
    green_cond_init(&subtrctd_cnd);

    green_t *trd_one, *trd_two, trd_1, trd_2;
    trd_one = malloc(sizeof(green_t));
    trd_two = malloc(sizeof(green_t));
    myret_t *rval;

    green_create(&trd_1, metr_add, NULL);
    green_create(&trd_2, metr_sub, NULL);

    green_join(&trd_1, (void**)&rval);
    printf("before metr_add: %d, after: %d\n", rval->prev_val, rval->next_val);
    green_join(&trd_2, (void**)&rval);
    printf("before metr_sub: %d, after: %d\n", rval->prev_val, rval->next_val);
    /*rc = green_cond_destroy(&subtrctd_cnd);
    assert(rc == 0);
    rc = green_mutex_destroy(&subtrctd_cnd_lock);
    assert(rc == 0);
    rc = green_mutex_destroy(&sum_lock);*/
    free(rval);
    free(trd_one);
    free(trd_two);
    return 0;
}