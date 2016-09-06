#ifndef _savetosql_h
#define _savetosql_h
#include "headers.h"

typedef struct {
    sqlite3 *db;
    char* errormessege;
    int retcode;
} sql_s;

int initial_sql(sql_s *sqldb, const char* fulldbname);

int insert_sql(sql_s *sqldb, const char* sql_in);

void destory_sql(sql_s *sqldb);

#endif /*savetosql.h*/
