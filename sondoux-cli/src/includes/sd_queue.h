#pragma once

#include "sd_qelem.h"

typedef struct SDQueue
{
    SDQElem* head;
    SDQElem* tail;
    
    int size;
    int max_size;
    int overwrite;
} SDQueue;

SDQElem* pop(SDQueue*);
void delete_queue(SDQueue* _queue)
{
    if(!_queue) return;

    while(_queue->head) 
    {
        delete_qelem(
            pop(_queue)
        );
    }

    free(_queue);
}

SDQueue* create_queue(int max_size, int overwrite)
{
    SDQueue* queue = NULL;
    
    if(max_size <= 0) return NULL;
    
    queue = (SDQueue *) malloc(sizeof(SDQueue));
    if(!queue) return NULL;

    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    queue->max_size = max_size;
    queue->overwrite = overwrite;

    return queue;
}

SDQElem* pop(SDQueue* queue)
{
    SDQElem* elem = NULL;

    if(!queue) return NULL;

    if(queue->head)
    {
        elem = queue->head;
        queue->head = queue->head->next;

        if(queue->head) queue->head->prev = NULL;
        else queue->tail = NULL;

        queue->size--;
    }

    return elem;
}

void push(SDQueue* queue, SDQElem* elem)
{
    SDQElem* new_elem = NULL;

    if(!queue) return;
    if(!elem) return;

    if(queue->size >= queue->max_size)
    {
        if(!queue->overwrite) return;

        while(queue->size >= queue->max_size)
        {
            delete_qelem(
                pop(queue)
            );
        }
    }

    new_elem = create_qelem(elem->data, elem->size);
    if(!new_elem) return;

    new_elem->prev = queue->tail;
    new_elem->next = NULL;

    if(queue->tail) queue->tail->next = new_elem;
    else queue->head = new_elem;

    queue->tail = new_elem;
    queue->size++;
}