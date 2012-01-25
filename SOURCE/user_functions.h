#ifndef USER_FUNCTIONS_H
#define USER_FUNCTIONS_H

#include <stdio.h>
#include "sqlite3.h"
#include "stl_test.h"

#ifdef __cplusplus
extern "C" {
#endif

int register_table(const char *nDb, int argc, const char **queries, const char **table_names, void *data);
int prep_exec(FILE *f, sqlite3 *db, const char *q);

#ifdef __cplusplus
}
#endif

#endif
