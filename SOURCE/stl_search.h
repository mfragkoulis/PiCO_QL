#ifndef STL_SEARCH_H
#define STL_SEARCH_H

#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {sqlite3_vtab vtab;
    sqlite3 *db; const char *zDb;
    const char *zName; int embedded; int nColumn; char **azColumn;
    void *data; char *zErr;} stlTable;

  typedef struct {sqlite3_vtab_cursor pCsr; int max_size; int *resultSet;
    int size; int current; int isEof; int first_constr; void *source;} stlTableCursor; 

  int call_sqtl();
  void fill_module(sqlite3_module *stl);
  int register_table(const char *nDb, int argc, const char **queries, const char **table_names, void *data);
  void register_vt(stlTable *stl);
  int equals_base(const char *zCol);
  int get_datastructure_size(sqlite3_vtab_cursor *cur);
  int search(sqlite3_vtab_cursor *cur, char *constraint, sqlite3_value *val);
  int retrieve(sqlite3_vtab_cursor *cur, int n, sqlite3_context *con);

#ifdef __cplusplus
}
#endif

#endif
