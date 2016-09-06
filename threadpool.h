#ifndef _threadpool_h
#define _threadpool_h
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
/*below is theading_pool*/
// static int id = 0;
// static __thread int THREAD_ID = 4;


typedef void* (*Function)(void *args);

typedef struct thread_works_s{
    Function func;
    void* args;
    struct thread_works_s *next;
} thread_cell;

typedef struct thread_pool_s{
    pthread_t *threads;
    pthread_mutex_t pool_lock;
    pthread_cond_t pool_cond;
    int threads_num;
    int current_work;
    int pool_join;
    thread_cell *head;

} thread_pool;



int pool_prepare(thread_pool *Pool, int threads_num);

int pool_add(thread_pool *Pool, Function func, void *args);

int pool_join(thread_pool *Pool);

#endif /*threadpool*/
