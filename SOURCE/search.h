#ifndef SEARCH_H
#define SEARCH_H

#include "stl_to_sql.h"

#ifdef __cplusplus
extern "C" {
#endif

  void fill_module(sqlite3_module *stl);
  int register_table(const char *nDb, int argc, const char **queries, const char **table_names, void *data);
  void register_vt(const char *vt_name);
  int equals_base(const char *zCol);
  int get_datastructure_size(sqlite3_vtab_cursor *cur);
  int search(sqlite3_vtab_cursor *cur, char *constraint, sqlite3_value *val);
  int retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con);

#ifdef __cplusplus
}
#endif

#endif
