#include "threadpool.h"




// static _Thread_local int THREAD_ID;

void *pool_routine(void  *args){
    thread_pool *Pool = args;
    
//     THREAD_ID = __sync_fetch_and_add(&id, 1);
// 
//     printf("threading id is %d\n\n", THREAD_ID);
    //THREAD_ID = __sync_fetch_and_add(&_thread_id, 1);
    while(1){
        pthread_mutex_lock(&(Pool->pool_lock));
        while(Pool->current_work==0&&Pool->pool_join==0){
            pthread_cond_wait(&(Pool->pool_cond), &(Pool->pool_lock));
        }
        while(Pool->current_work==0 && Pool->pool_join==1){
            pthread_mutex_unlock(&(Pool->pool_lock));
            pthread_exit(NULL);
        }
        assert(Pool->current_work!=0);
        assert(Pool->head!=NULL);
        Pool->current_work--;
        thread_cell *Target = Pool->head;
        Pool->head=Pool->head->next;

        pthread_mutex_unlock(&(Pool->pool_lock));

        (Target->func)(Target->args);
        free(Target);
        Target=NULL;
    }
    pthread_exit(NULL);
}


int pool_prepare(thread_pool *Pool, int threads_num){
    pthread_mutex_init(&(Pool->pool_lock),NULL);
    pthread_cond_init(&(Pool->pool_cond),NULL);
    Pool->threads_num=threads_num;
    Pool->current_work=0;
    Pool->pool_join = 0;
    Pool->threads=malloc(threads_num*sizeof(pthread_t));
    for(int i=0; i<Pool->threads_num ;i++){
        pthread_create(&(Pool->threads[i]), NULL, pool_routine, Pool);
    }
    return 0;
}

int  pool_add(thread_pool *Pool, Function func, void *args){
    thread_cell *new_work = (thread_cell *)malloc(sizeof(thread_cell));
    new_work->func = func;
    new_work->args = args;
    new_work->next = NULL;
    pthread_mutex_lock(&(Pool->pool_lock));
    thread_cell *tail = Pool->head;
    /*find the work queue's tail*/
    if(Pool->head!=NULL){
        while(tail->next!=NULL) tail = tail->next;
        tail->next = new_work;
    }
    else Pool->head = new_work;
    assert(Pool->head!=NULL);
    Pool->current_work++;
    pthread_mutex_unlock(&(Pool->pool_lock));
    pthread_cond_signal(&(Pool->pool_cond));
    return 0;
}

int pool_join(thread_pool *Pool){
    Pool->pool_join = 1;
    pthread_cond_broadcast(&(Pool->pool_cond));

    for(int i = 0; i<Pool->threads_num ; i++){
        pthread_join((Pool->threads[i]), NULL);
    }
    thread_cell *cell=NULL;
    while(Pool->head!=NULL){
        cell = Pool->head;
        Pool->head = Pool->head->next;
        free(cell);
    }

    pthread_mutex_destroy(&(Pool->pool_lock));
    pthread_cond_destroy(&(Pool->pool_cond));

    free(Pool);
    Pool=NULL;

    return 0;
}


//int main(){
//    int *task;
//    task = malloc(100*sizeof(int));
//
//    for(int i=0; i< 100 ;i++) task[i] = i+1;
//
//    thread_pool *Pool = malloc(sizeof(thread_pool));
//    pool_prepare(Pool, 4);
//    for(int i =0; i< 100; i++) pool_add(Pool, p1, &task[i]);
//    pool_join(Pool);
//}
