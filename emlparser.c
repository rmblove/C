#include "headers.h"
#include "emlparser.h"

int emlparser(eml_s *s){
    assert(s->fullfilename);
    FILE *stream = fopen(s->fullfilename, "r");
    if(!stream) return IOError;
    char BUFF[200];
    asprintf(&s->email_text,"");
    //int linenu=0;
	while((fgets(BUFF,200,stream))){
		if(BUFF[199]=='\n') BUFF[199]='\0';
		if(BUFF[199]=='\r') BUFF[199]='\0';
		//asprintf(&s->email_text,"%s%s",s->email_text,BUFF);
        //printf("%d::%s",++linenu,BUFF);
        if(strncmp(BUFF,"From",4)==0) s->From=strdup(&BUFF[5]);
        if(strncmp(BUFF,"To",2)==0) s->To=strdup(&BUFF[3]);
        if(strncmp(BUFF,"Subject",7)==0) s->Subject=strdup(&BUFF[8]);
        if(strncmp(BUFF,"Date",4)==0) s->Date=strdup(&BUFF[5]);
        if(strncmp(BUFF,"MIME",4)==0) break;
	}
    fclose(stream);
    return OK;
}
void free_eml_s(eml_s *s){
    if(s->filename!=NULL) free(s->filename);
    if(s->email_text!=NULL) free(s->email_text);
    if(s->fullfilename!=NULL) free(s->fullfilename);
    if(s->From!=NULL) free(s->From);
    if(s->To!=NULL) free(s->To);
    if(s->Subject!=NULL) free(s->Subject);
    if(s->Date!=NULL) free(s->Date);
}

void set_NULL_eml_s(eml_s *s){
    s->filename=NULL;
    s->fullfilename=NULL;
    s->email_text=NULL;
    s->From=NULL;
    s->To=NULL;
    s->Subject=NULL;
    s->Date=NULL;
}
