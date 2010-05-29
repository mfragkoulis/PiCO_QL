#include "stl_to_sql.h"
#include "search.h"


#ifdef __cplusplus
extern "C" {
#endif

  int register_table(char *db, char *query, void *data);
  void search(int *size, int *resultset, void *st, char *constraint, sqlite3_value *val);

#ifdef __cplusplus
}
#endif
