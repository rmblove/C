#include "headers.h"
int emlparser_test(){

    /* test for emlparser.c */
    eml_s mail;
    set_NULL_eml_s(&mail);
    mail.fullfilename = "./dnc.eml";
    int rc=emlparser(&mail);
    if (rc!=0) return rc;
    printf("\n\n- - - email test 0x0- - -\n\n");
    printf("%s\n%s\n%s\n%s\n", mail.From, mail.To, mail.Date, mail.Subject );
    printf("\n\n- - - email test 0x1- - -\n\n");
    printf("%s", mail.email_text);
    return 0;
}
int main(int argc, char* args[]){
    int rc = emlparser_test();
    assert(rc==0);

}
