#include "headers.h"
#include "myscandir.h"

int myscandir(const char* dir, file_list_s *filelist){
    DIR *pDIR;
    struct dirent* entry=NULL;
    file_list_s *ptr = filelist;
    if(!(pDIR = opendir(dir))) return IOError;
    while((entry = readdir(pDIR))){
        ptr->filename = strdup(entry->d_name);
        file_list_s *nw_fl_s = malloc(sizeof(file_list_s));
        ptr->next = nw_fl_s;
        ptr = ptr->next;
    }
    closedir(pDIR);
    return OK;
}

void freefilelist(file_list_s *filelist){
    if(filelist) return;
    freefilelist(filelist->next);
    free(filelist->filename);
    free(filelist);
}
