#include"../green.h"
#include<stdio.h>
static int sum = 0;
green_mutex_t lock;

void *increment(void*args){
    green_mutex_lock(&lock);
        for(int i = 0; i < *(int*)args; i++)
            sum++;
    green_mutex_unlock(&lock);
    return NULL;
}

int main(){
green_t t1, t2;
int arg1, arg2;
arg1 = 20000000;
arg2 = 30000000;
green_mutex_init(&lock);
green_create(&t1, &increment, &arg1);
green_create(&t2, &increment, &arg2);
green_join(&t1, NULL);
green_join(&t2, NULL);
printf("sum: %d\n", sum);
return 0;
}