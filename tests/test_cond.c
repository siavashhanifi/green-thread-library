#include <stdio.h>
#include <stdlib.h>
#include "../green.h"

green_mutex_t mutex;
green_cond_t cond;
int flag = 0;

void *test(void *arg) {
  int id = *(int *)arg;
  int loop = 20000;
  while(loop > 0) {
    green_mutex_lock(&mutex);
    while (flag != id) {
      green_cond_wait(&cond, &mutex);
    }  
    flag = (id + 1) % 2;
    printf("thread %d: %d\n", id, loop);
    green_cond_signal(&cond);
    green_mutex_unlock(&mutex);
    loop--;
  }
  return NULL;
}

int main() {
  green_t g0, g1;

  green_mutex_init(&mutex);
  green_cond_init(&cond);

  int a0 = 0;
  int a1 = 1;

  green_create(&g0, test, &a0);
  green_create(&g1, test, &a1);

  green_join(&g0, NULL);
  green_join(&g1, NULL);
  
  printf("Done!\n");

  return 0;
}


