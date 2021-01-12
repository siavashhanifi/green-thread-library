#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "../green.h"

#define COUNT_TO 20000000
#define TESTS 500

green_mutex_t lock;
static int shared = 0;

void *counter(void *args){
    
    green_mutex_lock(&lock);
    for(int i = 0; i < COUNT_TO / 2; i++)
        shared++;
    green_mutex_unlock(&lock);
    return NULL;
}

int test(){
    green_t t1, t2;
    green_mutex_init(&lock);
    green_create(&t1, counter, NULL);
    green_create(&t2, counter, NULL);
    green_join(&t1, NULL);
    green_join(&t2, NULL);
    int result = shared;
    shared = 0;
    return result;
}

int main(){
    int arr[TESTS];
    int anomalies = 0;
    for(int i = 0; i < TESTS; i++)
        arr[i] = test();

    printf("test results: \n");
    for(int i = 0; i< TESTS; i++){
        if(arr[i] != COUNT_TO)
            anomalies++;
    }
    printf("anomalies: %d\n", anomalies);

    return 0;
}