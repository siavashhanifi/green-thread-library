#include "green.h"
int main(){
    green_t t1, t2;
    green_create(&t1, &routine1, NULL);
    green_create(&t2, &routine2, NULL);
    green_join(&t1, NULL);
    green_join(&t2, NULL);
    return 0;
}