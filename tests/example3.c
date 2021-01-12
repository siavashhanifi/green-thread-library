#include"../green.h"
#include<stdio.h>

static int did_run1 = 0;
green_cond_t cond;
green_mutex_t cond_lock;
green_mutex_t lock;

void *first(void*args){
    green_mutex_lock(&cond_lock);
    printf("first\n");
    did_run1 = 1;
    green_cond_signal(&cond);
    green_mutex_unlock(&cond_lock);
    return NULL;
}


void *second(void*args){
    green_mutex_lock(&cond_lock);
    while(!did_run1)
        green_cond_wait(&cond, &cond_lock);
    green_mutex_unlock(&cond_lock);
    printf("second\n");
    return NULL;
}

int main(){
green_t t1, t2;
green_cond_init(&cond);
green_mutex_init(&cond_lock);
green_create(&t2, &second, NULL);
green_create(&t1, &first, NULL);
green_join(&t2, NULL);
green_join(&t1, NULL);
return 0;
}