#include <stdio.h>
#include<malloc.h>
#include "green.h"

void *test (void *arg) {
    int i = *(int *) arg;
    int loop = 10000;
    while(loop > 0) {
        printf("thread: %d: %d\n", i , loop);
        loop--;
        green_yield();
    }
}


void *testA(void *arg){
    printf("HERE A\n");
    int *ret = malloc(sizeof(int)); 
    *ret = *((int*)(arg)) + 1; 
    return ret;
}

void *testB(void *arg){
    printf("HERE B\n");
     int *ret = malloc(sizeof(int)); 
    *ret = *((int*)(arg)) + 2; 
    return ret;
}

int main(){
    green_t g0, g1;
    int a0 = 0;
    int a1 = 1;
    //int *retval1;
    //int *retval2;

    green_create(&g0, test, &a0);
    green_create(&g1, test, &a1);

    green_join(&g0, NULL);
    green_join(&g1, NULL);

    //free(retval1);
    //free(retval2);
    printf("done\n");
    return 0;
}