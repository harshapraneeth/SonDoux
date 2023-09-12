#pragma once

#include <stdlib.h>
#include <pthread.h>

#include "sd_constants.h"

typedef enum {Running, Stopped} SDThreadState;

typedef struct SDThread
{
    pthread_t id;
    pthread_mutex_t lock;
    SDThreadState state;
} SDThread;

void stop_thread(SDThread*);
void delete_thread(SDThread* _thread)
{
    if(!_thread) return;

    stop_thread(_thread);
    pthread_mutex_destroy(&_thread->lock);
    
    free(_thread);
}

SDThread* create_thread()
{
    SDThread* thread = NULL;
    int result = 0;

    thread = (SDThread *) malloc(sizeof(SDThread));
    if(!thread) return NULL;

    result = pthread_mutex_init(&thread->lock, NULL);
    if(result)
    {
        delete_thread(thread);
        return NULL;
    }

    thread->state = Stopped;
    return thread;
}

int is_running(SDThread*);
void run_thread(SDThread* thread, void* (*func)(void*), void* args)
{
    int result = 0;

    if(!thread) return;
    if(is_running(thread)) return;

    result = pthread_create(&thread->id, NULL, func, args);
    if(result) return;

    pthread_mutex_lock(&thread->lock);
    thread->state = Running;
    pthread_mutex_unlock(&thread->lock);
}

void stop_thread(SDThread* thread)
{
    int result = 0;

    if(!thread) return;
    if(!is_running(thread)) return;

    result = pthread_kill(thread->id, 0);
    if(result) return;

    pthread_mutex_lock(&thread->lock);
    thread->state = Stopped;
    pthread_mutex_unlock(&thread->lock);
}

void join_thread(SDThread* thread)
{
    int result;

    if(!thread) return;
    if(!is_running(thread)) return;

    result = pthread_join(thread->id, NULL);
    if(result) return;
    
    pthread_mutex_lock(&thread->lock);
    thread->state = Stopped;
    pthread_mutex_unlock(&thread->lock);
}

int is_running(SDThread* thread)
{
    int running = false;

    if(!thread) return running;

    pthread_mutex_lock(&thread->lock);
    running = thread->state == Running;
    pthread_mutex_unlock(&thread->lock);

    return running;
}