/*
 *   Declare the piblic interface to pico_ql.
 *
 *   Copyright 2016 Marios Fragkoulis
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
 *   distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 *   express or implied.
 *   See the License for the specific language governing
 *   permissions and limitations under the License.
 */

#ifndef PICO_QL_H
#define PICO_QL_H

#include <stdio.h>
#include <pthread.h>
#include <sstream>
using namespace std;

#include "sqlite3.h"

namespace picoQL {

  void pico_ql_register(const void *collection, const char * col_name);
  int pico_ql_init(const char** pragmas, int npragmas, int port_number,
		  pthread_t *t);
  int pico_ql_exec_query(const char *query, stringstream &s,
  			 int (*callback)(sqlite3 *, sqlite3_stmt *,
				 			stringstream &));
  int pico_ql_step_mute(sqlite3 *db, sqlite3_stmt *stmt, stringstream &s);
  int pico_ql_step_text(sqlite3 *db, sqlite3_stmt *stmt, stringstream &s);
  int pico_ql_step_swill_html(sqlite3 *db, sqlite3_stmt *stmt,
		  					stringstream &s);
  int pico_ql_step_swill_json(sqlite3 *db, sqlite3_stmt *stmt,
		  					stringstream &s);
  int pico_ql_shutdown();

}

#endif
