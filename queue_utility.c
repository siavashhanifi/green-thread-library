#include "queue_utility.h"

void enqueue(green_t** queue_head, green_t* thread) {
    assert(thread != NULL && queue_head != NULL);
    if (*queue_head == NULL) {
        *queue_head = thread;
        return;
    }
    green_t* tail = NULL;
    green_t* itr = *queue_head;
    //find tail and point to it with itr
    while (TRUE) {
        if (itr->next == NULL)
            break;
        itr = itr->next;
    }
    tail = itr;
    //insert at tail
    tail->next = thread;
}

green_t* dequeue(green_t** queue_head) {
    assert(queue_head != NULL);
    //If susp_list empty
    if (*queue_head == NULL) {
        fprintf(stderr, "dequeue(): Attempt to remove head from empty list, returning NULL\n");
        return NULL;
    }
    green_t* old_head;
    old_head = *queue_head;
    //if has next
    if (old_head->next != NULL)
        //set susp_list to point to susp_list->next
        *queue_head = old_head->next;
    else
        *queue_head = NULL;
    old_head->next = NULL;
    return old_head;
}

void insrt_head(green_t** susp_list, green_t* head)
{
    assert(head != NULL && susp_list != NULL);

    if ((green_t*)*susp_list != NULL) {
        green_t* old_head = (green_t*)*susp_list;
        head->next = old_head;
    }
    *susp_list = head;
}


void* funA(void* arg) {
    printf("A\n");
    return NULL;
}

void* funB(void* arg) {
    printf("B\n");
    return NULL;
}

void* funC(void* arg) {
    printf("C\n");
    return NULL;
}

void test_susplist()
{
    green_t* a, * b, * c;
    a = malloc(sizeof(green_t));
    b = malloc(sizeof(green_t));
    c = malloc(sizeof(green_t));

    a->fun = &funA;
    b->fun = &funB;
    c->fun = &funC;

    green_t* susplist = NULL;

    enqueue((green_t**)&susplist, a);
    enqueue((green_t**)&susplist, b);
    enqueue((green_t**)&susplist, c);

    c = dequeue((green_t**)&susplist);
    b = dequeue((green_t**)&susplist);
    a = dequeue((green_t**)&susplist);

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