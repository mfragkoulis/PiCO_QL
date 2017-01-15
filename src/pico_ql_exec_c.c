/*
 *   Prepare and pass queries for execution.
 *
 *   Copyright 2012 Marios Fragkoulis
 *
 *   Licensed under the Apache License, Version 2.0
 *   (the "License");you may not use this file except in
 *   compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in
 *   writing, software distributed under the License is
 *   distributed on an "AS IS" BASIS.
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 *   express or implied.
 *   See the License for the specific language governing
 *   permissions and limitations under the License.
 */

#include <stdlib.h>
#include "pico_ql.h"
#include "pico_ql_internal.h"
#ifdef PICO_QL_SWILL
#include "pico_ql_swill.h"
#endif
#include "pico_ql_swill_access_func.h"

extern sqlite3 *db;

int pico_ql_exec_query(const char *query, FILE *f,
		int (*callback)(sqlite3 *db, sqlite3_stmt*, FILE *f)) {
  sqlite3_stmt *stmt;
  int prepare;
#ifdef PICO_QL_DEBUG
  fprintf(stderr, "Query to process: %s\n", query);
#endif
  if ((prepare = sqlite3_prepare_v2(db, query, -1, &stmt, 0)) == SQLITE_OK) {
#ifdef PICO_QL_TEST
    if (f)
      fprintf(f, "Statement prepared.\n");
#endif
    if (callback)
      (*callback)(db, stmt, f);
    else {
      fprintf(stderr, "Callback to step to query is NULL. Exiting now.");
      return SQLITE_ERROR;
    }
  } else {
    fprintf(stderr, "Error in preparation of query: error no %i\n", prepare);
    fprintf(stderr, "\nExtended error code %i.\n", sqlite3_extended_errcode(db));
    fprintf(stderr, "\nExtended error message:\n%s\n\n", sqlite3_errmsg(db));
    return prepare;
  }
  deinit_temp_structs();
  sqlite3_finalize(stmt);
  return SQLITE_OK;
}
