#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
void *dosomething(void *threadnum ){
    int *num = threadnum;
    for(int i=0; i<100; i++){
        printf("this is thread %d : task [%d]\n", *num, i );
    }
    return NULL;
}

int buff[8] = {0};

sem_t *full;

sem_t *empty;

void *writer(){
    for(int i=0 ; i < 100 ; i++){
        sem_wait(full);
        buff[i%8]=i;
        sem_post(empty);
    }
    buff[100%8] = -1;
    sem_post(empty);
    return NULL;
}
void *reader(){
    int c=0;
    int count=0;
    while(1) {
        sem_wait(empty);
        c=buff[(count++)%8];
        if(c==-1) break;
        printf("%d|", c);
        sleep(1);
        //printf("\b\b\b");
        fflush(stdout);
        sem_post(full);
    }
    return NULL;
}

int multithreadingT(){
    //pthread_t theads[2];
    //int th[2]={1,2};
    //for(int i=0; i<2; i++){
    //    pthread_create(&theads[i], NULL, dosomething, &th[i]);
    //}
    //for(int i=0; i<2; i++){
    //    pthread_join(theads[i], NULL);

    empty = sem_open("empty", O_CREAT, 0644 ,0);
    full = sem_open("full", O_CREAT, 0644, 8);
    pthread_t theads[2];
    pthread_create(&theads[0], NULL, writer, NULL);
    pthread_create(&theads[1], NULL, reader, NULL);
    for(int i=0; i<2; i++) pthread_join(theads[i],NULL);
    sem_close(full);
    sem_close(empty);
    return 0;
}

typedef void *(*routine)(void *(*fn)(void *), void *args);


void *dothework(void *(*fn)(void *), void *args){
    (*fn)(args);
    return NULL;
}


void *p1(void *args){
    int *c = (int*) args;
    printf("thread ID  is %u | argu is %d\n", (unsigned int)pthread_self(), *c);
    return NULL;
}

/*below is theading_pool*/

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

typedef int *(*poolctr)(thread_pool *Pool, int theads_num);

void *pool_routine(void  *args){
    thread_pool *Pool = args;
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


int main(){
    int *task;
    task = malloc(100*sizeof(int));

    for(int i=0; i< 100 ;i++) task[i] = i+1;

    thread_pool *Pool = malloc(sizeof(thread_pool));
    pool_prepare(Pool, 4);
    for(int i =0; i< 100; i++) pool_add(Pool, p1, &task[i]);
    pool_join(Pool);
}
