#include "headers.h"
#include "emlparser.h"

int emlparser(eml_s *s){
    assert(s->fullfilename);
    FILE *stream = fopen(s->fullfilename, "r");
    if(!stream) return IOError;
    int c=0;
    asprintf(&s->email_text,"");
    while((c=fgetc(stream))!=EOF){
        asprintf(&s->email_text,"%s%c",s->email_text,c);
    }
    char *buff = strdup(s->email_text);
    char* text;
    char* scratch;
    text = strtok_r(buff ,"\n\r" ,&scratch ); /*the first line has not what we want.*/
    while((text=strtok_r(NULL, "\n\r",&scratch))){
        if(strncmp(text,"From",4)==0) s->From=strdup(&text[5]);
        if(strncmp(text,"To",2)==0) s->To=strdup(&text[3]);
        if(strncmp(text,"Subject",7)==0) s->Subject=strdup(&text[8]);
        if(strncmp(text,"Date",4)==0) {
            s->Date=strdup(&text[5]);
            break;
        }
    }
    free(buff);
    fclose(stream);
    return OK;
}

void freeeml_s(eml_s *s){
    free(s->filename);
    free(s->fullfilename);
    free(s->email_text);
    free(s->From);
    free(s->To);
    free(s->Subject);
    free(s->Date);
}
