#ifndef SEARCH_H
#define SEARCH_H

//#include "stl_to_sql.h"
#include "sqlite3.h"
#include "bridge.h"


//extern "C" int register_table(char *db, char * query, void *data);
//int get_data_structure_size(void *st);
//void search(void *stc, int *initial, char *constr, sqlite3_value *val);
//int retrieve(void *stc, int n, sqlite3_context * con);
void traverse(char *col_name, char *op, void *value);


#endif

