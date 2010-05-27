#ifdef __cplusplus
extern "C"
#endif

//#include "sqlite3.h"
#include "stl_to_sql.h"
#include "search.h"

void search(int *resultset, void *data, int icol, char *op, sqlite3_value *val);


