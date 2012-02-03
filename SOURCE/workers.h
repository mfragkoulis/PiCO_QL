#ifndef WORKERS_H
#define WORKERS_H

#include "sqlite3.h"
#include "stl_search.h"

int realloc_resultset(sqlite3_vtab_cursor *cur);
int compare(int dstr_value, int op, int value);
int compare(long int dstr_value, int op, long int value);
int compare(double dstr_value, int op, double value);
int compare(const void *dstr_value, int op, const void *value);
int compare(const unsigned char *dstr_value, int op,
	    const unsigned char *value);
int compare_res(int count, stlTableCursor *stcsr, int *temp_res);
void check_alloc(const char *constr, int &op, int &iCol);
int equals_base(const char *zCol);


#endif
