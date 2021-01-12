#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include "../green.h"

#define MAX 2000000

int buffer[MAX];
int fill_ptr = 0;
int use_ptr = 0;
int count = 0;

void put(int value){
    buffer[fill_ptr] = value;
    fill_ptr = (fill_ptr + 1) % MAX;
    count++;
}

int get(){
    int tmp = buffer[use_ptr];
    use_ptr = (use_ptr + 1) % MAX;
    count--;
    return tmp;
}

green_cond_t empty, fill;
green_mutex_t mutex;

void *producer(void *arg){
    int i;
    int loops = *(int*)arg;
    for(i = 0; i < loops; i++){
        green_mutex_lock(&mutex);
        while(count == MAX)
            green_cond_wait(&empty, &mutex);
        put(i);
        green_cond_signal(&fill);
        green_mutex_unlock(&mutex);
    }
    return NULL;    
}

void *consumer(void *arg){
    int i;
    int loops = *(int*)arg;
    for(i = 0; i < loops; i++){
        green_mutex_lock(&mutex);
        while(count == 0)
            green_cond_wait(&fill, &mutex);
        int tmp = get();
        printf("handling request: %d\n", tmp); //amount consumed or request handled this time
        printf("left to handle: %d\n", count); //produced
        green_cond_signal(&empty);
        green_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(){
    green_cond_init(&fill);
    green_cond_init(&empty);
    green_mutex_init(&mutex);

    green_t prod, cons;
    int loops1,loops2;
    loops1 = MAX;
    loops2 = MAX;
    green_create(&prod, &producer, &loops1);
    green_create(&cons, &consumer, &loops2);
    green_join(&prod, NULL);
    green_join(&cons, NULL);

    return 0;
}