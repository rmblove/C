#include "headers.h"
#include "savetosql.h"

int initial_sql(sql_s *sqldb, const char *fulldbname){

    sqldb->retcode=sqlite3_open(fulldbname ,&(sqldb->db));
    if(sqldb->retcode) {
        fprintf(stderr,"cannot open the db :  %s\n", sqlite3_errmsg(sqldb->db));
        free(sqldb);
        return SQLError;
    }
    return OK;
}
void destory_sql(sql_s *sqldb){
    sqlite3_exec(sqldb->db,"commit;",0,0,0);
    sqlite3_close(sqldb->db);
}
int insert_sql(sql_s *sqldb, const char* sql_in){
    sqldb->retcode = sqlite3_exec(sqldb->db, sql_in,0,0,&(sqldb->errormessege));
    if(sqldb->retcode){
        fprintf(stderr, "cannot insert into db : %s\n", sqldb->errormessege);
        sqlite3_free(sqldb->errormessege);
        return SQLError;
    }
    return OK;
}
