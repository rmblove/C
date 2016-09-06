#include "headers.h"
#include <time.h>

static pthread_mutex_t CountLock=PTHREAD_MUTEX_INITIALIZER;
char *path;
#define BUFFSIZE 64
#define THREADSIZE 4
static __thread int threadno = 0;
static __thread int threadno_status = 1;
static __thread int COUNT = 0;
static int id = 0;

typedef struct EML_BUFF_WITHSEM_{
    sem_t *full;
    sem_t *empty;
    eml_s* BUFF[BUFFSIZE];

} EML_BUFF;

void emlbuff_init(EML_BUFF *EB, int  name){
    char * full;
    asprintf(&full, "full%d", name);
    char * empty;
    asprintf(&empty, "empty%d", name);
    EB->full = sem_open(full, O_CREAT , 0644, 0);
    EB->empty = sem_open(empty, O_CREAT , 0644, BUFFSIZE);
    for(int i=0; i< BUFFSIZE ; i++){
        EB->BUFF[i] = NULL;
    }
}

void emlbuff_destroy(EML_BUFF *EB){
    sem_close(EB->full);
    sem_close(EB->empty);
    for(int i=0; i< BUFFSIZE ; i++){
        EB->BUFF[i] = NULL;
    }
}

static EML_BUFF EML_B[THREADSIZE];


int endWith(const char * str, const char * end) {
    int  result = 0;
    if (str != NULL && end != NULL) {
        int l1 = strlen(str);
        int l2 = strlen(end);
        if (l1 >= l2) {
            if (strcmp(str + l1 - l2, end) == 0) {
                result = 1;
            }
         }
    }
    return result;
}

void *consumer(void *args){
    sql_s *mysqlite = malloc(sizeof(sql_s));
    int init_sql_rc = initial_sql(mysqlite,"./haha.db");
    assert(init_sql_rc==0);
    int data_NULL_num=0;
    int count[4] = {0,0,0,0};
    while(1){
        if(data_NULL_num>3) break;
        for(int i=0;i<THREADSIZE; i++){
            EML_BUFF *eb_ptr = &EML_B[i];
            if ((sem_trywait(eb_ptr->full))!=0) continue;
            else{
                eml_s *mail=eb_ptr->BUFF[(count[i]++)%BUFFSIZE];
                if(mail==NULL) {
                    data_NULL_num++;
                    continue;
                };
                char* sql;
                asprintf(&sql,"INSERT INTO csqltest (FROM_EMAIL, TO_EMAIL, SUBJECT, DATE, FILENAME) VALUES (%s,%s,%s,%s,%s)",\
                        mail->From, mail->To, mail->Subject, mail->Date, mail->filename);
                printf("sql :: %s\n", sql);
                assert(sql!=NULL);
                //int insert_sql_rc = insert_sql(mysqlite, sql);
                //assert(insert_sql_rc==0);
                printf("file<<%s>> has been save to the db ./haha.db\n", mail->filename);
                free(sql);
                free_eml_s(mail);
                free(mail);
                //mail=NULL;
                sem_post(eb_ptr->empty);

            }
        }
    }
    destory_sql(mysqlite);
    free(mysqlite);
    return NULL;
}

void *worker(void *args){
    char *filename = (char *)args;
    eml_s *mail;
    if(args!=NULL){
        mail = malloc(sizeof(eml_s));
        assert(mail!=NULL);
        set_NULL_eml_s(mail);
        asprintf(&(mail->filename),"%s",filename);
        asprintf(&(mail->fullfilename),"%s%s",path, filename);
        int rc = emlparser(mail);
        if(rc!=0) {
            fprintf(stderr, "error ): while in parse file %s\n", mail->filename);
            free_eml_s(mail);
            free(mail);
            mail=NULL;
            return NULL;
        }
        //printf("thread id %d done with :: %s :: %s\n", (unsigned int)pthread_self(), mail->filename, mail->From);
        //free_eml_s(mail);
        //free(mail);
        //mail=NULL;
    }
    else mail=NULL;

    if(threadno_status  == 1){
        threadno = __sync_fetch_and_add(&id, 1);
        threadno_status = 0;
    }
    //printf("thread no is :: %d\n", threadno);
    EML_BUFF *ebptr = &EML_B[threadno];
    sem_wait(ebptr->empty);
    (ebptr->BUFF)[(COUNT++)%BUFFSIZE] = mail;
    sem_post(ebptr->full);


    return NULL;
}


void producer(){
    file_list_s *Files;
    Files = malloc(sizeof(file_list_s));
    assert(Files!=NULL);
    printf("Search the files in dir : %s\n", path);
    myscandir(path, Files);
    printf("Files has been searched. (:\n");
    char* file_end = ".eml";
    thread_pool *Pool = malloc(sizeof(thread_pool));
    assert(Pool!=NULL);
    pool_prepare(Pool, THREADSIZE);
    file_list_s *ptr = Files;
    assert(ptr!=NULL);
    while(ptr->next!=NULL){
        if((endWith(ptr->filename,file_end))==0) {
            ptr=ptr->next;
            continue;
        }
        //printf("add dirt %s\n",ptr->filename);
        pool_add(Pool,worker, ptr->filename);

        ptr = ptr->next;
    }
    pool_add(Pool,worker,NULL);
    pool_add(Pool,worker,NULL);
    pool_add(Pool,worker,NULL);
    pool_add(Pool,worker,NULL);
    pool_join(Pool);
    freefilelist(Files);
}
int main(int argc, char* args[]){
    assert(argc>1);
    path=args[1];
    int start = time((time_t *)NULL);
    for(int i=0; i<THREADSIZE ; i++){

        emlbuff_init(&EML_B[i], i);
    }
    int end = time ((time_t*)NULL);
    printf("\n time for searching the dir is %d seconds\n", start-end);

    //pthread_mutex_unlock(&CountLock);
    pthread_t consume_thrd;
    pthread_create(&consume_thrd, NULL, consumer , NULL);
    //pthread_attr_t consume_attr;
    //pthread_attr_init(&consume_attr);
    //pthread_attr_setdetachstate(&consume_attr, PTHREAD_CREATE_DETACHED);
    //pthread_create(&consume_thrd , &consume_attr, consumer, NULL);
    producer();
    pthread_join(consume_thrd,NULL);
    int end2 = time((time_t *)NULL);
    printf("\n time for reading all files is %d seconds\n", end2-end);
    //pthread_mutex_destroy(&CountLock);
    for(int i=0; i<THREADSIZE ; i++){
        emlbuff_destroy(&EML_B[i]);
    }
}
