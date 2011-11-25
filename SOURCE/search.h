#ifndef SEARCH_H
#define SEARCH_H

#include "stl_to_sql.h"

#ifdef __cplusplus
extern "C" {
#endif

  void fill_module(sqlite3_module *stl);
  int register_table(const char *nDb, int argc, const char **queries, const char **table_names, void *data);
  int realloc_carrier(sqlite3_vtab *pVtab, void *ds, const char *tablename, char **pzErr);
  int fill_check_dependencies(sqlite3_vtab *pVtab);
  int update_structures(sqlite3_vtab_cursor *cur);
  void unset_mem(sqlite3_vtab_cursor *st);
  int get_datastructure_size(sqlite3_vtab *pVtab);
  int search(sqlite3_vtab_cursor *cur, char *constraint, sqlite3_value *val);
  int retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con);


#ifdef __cplusplus
}
#endif

#endif
