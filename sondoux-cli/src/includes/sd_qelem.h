#pragma once

#include <stdlib.h>

typedef struct SDQElem
{
    void* data;
    int size;

    struct SDQElem* prev;
    struct SDQElem* next;
} SDQElem;

void delete_qelem(SDQElem* _elem)
{
    if(!_elem) return;

    if(_elem->data) free(_elem->data);
    free(_elem);
}

SDQElem* create_qelem(void* data, int size)
{
    SDQElem* elem = NULL;

    if(!data) return NULL;
    if(size <= 0) return NULL;

    elem = (SDQElem *) malloc(sizeof(SDQElem));
    if(!elem) return NULL;

    elem->data = malloc(size);
    if(!elem->data)
    {
        free(elem);
        return NULL;
    }

    memcpy(elem->data, data, size);
    elem->size = size;
    
    return elem;
}