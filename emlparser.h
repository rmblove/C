#ifndef _emlparser_h_
#define _emlparser_h_
#include "headers.h"

/*this emlparser is not STABLE */

typedef struct {
    char* filename;
    char* fullfilename;
    char* email_text;
    char* From;
    char* To;
    char* Subject;
    char* Date;
} eml_s;

void set_NULL_eml_s(eml_s *s);
int emlparser(eml_s *s);
void free_eml_s(eml_s *s);
#endif /*_emlparser_h_*/
