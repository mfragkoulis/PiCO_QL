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

#include "pico_ql.h" /* pico_ql_exec_query() */

#ifdef PICO_QL_SWILL
#include "pico_ql_swill.h"
#endif
#include "pico_ql_vt.h"
#include "pico_ql_swill_access_func.h"

extern sqlite3 *db;

/* Set virtual table module's function pointers
 * to the implemented callback functions.
 */
static const sqlite3_module pico_ql_mod = {
  /* iVersion */ 1,
  /* xCreate */ create_vtable,
  /* xConnect */ connect_vtable,
  /* xBestIndex */ best_index_vtable,
  /* xDisconnect */ disconnect_vtable,
  /* destroy_vtable is a dummy implementation
   * for xDestroy which returns SQLITE_MISUSE.
   * I do not see any benefit in allowing DROP table.
   * The virtual table schema is in-memory, hence
   * transient, and CREATE VIRTUAL TABLE calls
   * are auto-generated as per the DSL. So a manual
   * CREATE call would have no effect as the underlying
   * column mapping would be missing.
   */
  /* xDestroy */ destroy_vtable,
  /* xOpen */ open_vtable,
  /* xClose */ close_vtable,
  /* xFilter */ filter_vtable,
  /* xNext */ next_vtable,
  /* xEof */ eof_vtable,
  /* xColumn */ column_vtable,
  /* xRowid */ 0,
  /* xUpdate */ 0,
  /* xBegin */ 0,
  /* xSync */ 0,
  /* xCommit */ 0,
  /* xRollback */ 0,
  /* xFindFunction */ 0,
  /* xRename */ 0,
};

/* Executes the SQL CREATE queries, opens the sqlite 
 * database connection and calls swill or pico_ql_test 
 * depending on the compile flag TEST.
 */
int register_table(int argc,
		   int view_index, 
		   const char **q, 
		   const char **sqlite_names, 
		   int port_number) {
  /* This definition implicitly constraints a table name 
   * to 140 characters. It should be more than enough.
   */
  char sqlite_query[200];
  int re, i = 0, output;

#ifdef PICO_QL_DEBUG
  for (i = 0; i < argc; i++) {
    printf("\nquery to be executed: %s.\n", q[i]);
  }
#endif
  output = sqlite3_create_module(db, "PicoQL", &pico_ql_mod, NULL);
  if (output == 1) 
    printf("Error while registering module\n");
#ifdef PICO_QL_DEBUG
  else if (output == 0) 
    printf("Module registered successfully\n");
#endif
  /* sqlite3_create_function() calls */
  for (i = 0; i < argc; i++) {
    char sqlite_type[10];
    if (i < view_index)
      strcpy(sqlite_type, "table");
    else
      strcpy(sqlite_type, "view");
    sprintf(sqlite_query, "SELECT * FROM sqlite_master WHERE type='%s' AND name='%s';", sqlite_type, sqlite_names[i]);
    if (pico_ql_exec_query((const char *)sqlite_query, NULL, pico_ql_step_mute) != SQLITE_ROW) {
      re = pico_ql_exec_query((const char *)q[i], NULL, pico_ql_step_mute);
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
  return re;
}
