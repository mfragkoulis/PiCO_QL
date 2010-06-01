#ifndef BRIDGE_H
#define BRIDGE_H

#include "stl_to_sql.h"
#include "search.h"

#ifdef __cplusplus
extern "C" {
#endif

  int prep_exec(sqlite3 *db, char * q);
  void fill_module(sqlite3_module *stl);
  int register_table(char *db, char *query, void *data);
  int get_data_structure_size(void *st);
  void search(void *stc, int *initial, void *st, char *constraint, sqlite3_value *val);
  int retrieve(void *stc, int n, sqlite3_context * con);

#ifdef __cplusplus
}
#endif

#endif
