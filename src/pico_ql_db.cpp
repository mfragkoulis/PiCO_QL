/*
 *   Manage database connections and pass queries to SQLite.
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
#include <time.h>
#include <string.h>
#include "pico_ql.h"
#include "pico_ql_internal.h"
#include "pico_ql_db.h"
#ifdef PICO_QL_SWILL
#include "pico_ql_swill.h"
#endif
#include "pico_ql_vt.h"
#include "pico_ql_test.h"
#include "pico_ql_swill_access_func.h"

namespace picoQL {

static sqlite3 *db = NULL;

int pico_ql_exec_query(const char *query, stringstream &s,
		int (*callback)(sqlite3 *db, sqlite3_stmt*,
			stringstream &s)) {
  sqlite3_stmt *stmt;
  int prepare;
#ifdef PICO_QL_DEBUG
  fprintf(stderr, "Query to process: %s\n", query);
#endif
  if ((prepare = sqlite3_prepare_v2(db, query, -1, &stmt, 0)) == SQLITE_OK) {
#ifdef PICO_QL_TEST
    s << "Statement prepared.\n";
#endif
    if (callback)
      (*callback)(db, stmt, s);
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

int pico_ql_shutdown() {
  sqlite3_close(db);
  deinit_selectors();
  return SQLITE_OK;
}

/* Executes the SQL CREATE queries, opens the sqlite 
 * database connection and calls swill or pico_ql_test 
 * depending on the compile flag TEST.
 */
int register_table(int argc,
		   int view_index, 
		   const char **q, 
		   const char **sqlite_names, 
		   int port_number,
		   sqlite3 *this_db) {
  /* This definition implicitly constraints a table name 
   * to 140 characters. It should be more than enough.
   */
  char sqlite_query[200];
  int re, i = 0;

  db = this_db;
#ifdef PICO_QL_DEBUG
  for (i = 0; i < argc; i++) {
    printf("\nquery to be executed: %s.\n", q[i]);
  }
#endif
  sqlite3_module *mod;
  mod = (sqlite3_module *)sqlite3_malloc(sizeof(sqlite3_module));
  fill_module(mod);
  int output = sqlite3_create_module(db, "PicoQL", mod, NULL);
  if (output == 1) 
    printf("Error while registering module\n");
#ifdef PICO_QL_DEBUG
  else if (output == 0) 
    printf("Module registered successfully\n");
#endif
  // sqlite3_create_function() calls
  for (i = 0; i < argc; i++) {
    char sqlite_type[10];
    if (i < view_index)
      strcpy(sqlite_type, "table");
    else
      strcpy(sqlite_type, "view");
    stringstream s;
    sprintf(sqlite_query, "SELECT * FROM sqlite_master WHERE type='%s' AND name='%s';", sqlite_type, sqlite_names[i]);
    if (pico_ql_exec_query((const char *)sqlite_query, s, pico_ql_step_mute) != SQLITE_ROW) {
      re = pico_ql_exec_query((const char *)q[i], s, pico_ql_step_mute);
#ifdef PICO_QL_DEBUG
      printf("Query %s returned: %i\n", q[i], re);
#endif
      if ((re = sqlite3_extended_errcode(db)) != SQLITE_DONE && re != SQLITE_OK) {
	printf("Extended error code: %i.\n", sqlite3_extended_errcode(db));
	printf("Extended error message:\n%s.\n", sqlite3_errmsg(db));
	return re;
      }
    }
  }
  start_serving();
#ifndef PICO_QL_TEST
  if (port_number == -1)
    return re;
#ifdef PICO_QL_SWILL
  else {
    printf("Please visit http://localhost:%i to be served\n", port_number);
    init_pico_ql_swill(port_number);
  }
#endif
#else
  re = exec_tests();
#endif
  sqlite3_free(mod);
  return re;
}

} //namespace picoQL
