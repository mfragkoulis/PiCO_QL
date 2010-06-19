#ifndef BRIDGE_H
#define BRIDGE_H

#include "stl_to_sql.h"
//#include "search.h" -> out of use

#ifdef __cplusplus
extern "C" {
#endif

  //  void call_swill(sqlite3 *db);
  //  int prep_exec(sqlite3 *db, char * q);
  void fill_module(sqlite3_module *stl);
  int register_table(char *nDb, int argc, const char **queries, void *data, int create);
  int get_datastructure_size(void *st);
  void search(void *stc, char *constraint, sqlite3_value *val);
  int retrieve(void *stc, int n, sqlite3_context *con);


#ifdef __cplusplus
}
#endif

#endif
