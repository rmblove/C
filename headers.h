#ifndef _headers_h
#define _headers_h
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <sqlite3.h>
#include <semaphore.h>

#define Fn_apply(type, fn, ...) {                                    \
    void *stopper_for_apply = (int[]){0};                            \
    void **list_for_apply=(type*[]){__VAR_ARGS__,stopper_for_apply)};\
    for(int i=0; list_for_apply[i]!=stopper_for_apply ; i++)         \
        fn(list_for_apply[i]);                                       \
}

#define Free_all(void, free, ...) Fn_apply(void, free, __VAR_ARGS__)

/*this is the errorcode*/
#define OK 0
#define IOError 1
#define SQLError 2



#include "emlparser.h"
#include "myscandir.h"
#include "savetosql.h"
#include "threadpool.h"
#endif /*_headers_h*/
