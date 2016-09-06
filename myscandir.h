#ifndef _myscandir_h
#define _myscandir_h
#include "headers.h"

typedef struct file_list {
    char* filename;
    struct file_list *next;
} file_list_s;

int myscandir(const char* dir, file_list_s *filelist);
void freefilelist(file_list_s *filelist);

#endif /*_myscandir_h*/
