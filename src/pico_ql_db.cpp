/*
 *   Manage database connections: interrupt query and shutdown connection.
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

#include "pico_ql.h"
#include "pico_ql_internal.h"

extern "C" sqlite3 *db;

namespace picoQL {

extern "C" {

int pico_ql_create_function(const char *name, int argc, int text_rep, void *p,
		  void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
		  void (*xStep)(sqlite3_context*,int,sqlite3_value**),
		  void (*xFinal)(sqlite3_context*)) {
	return sqlite3_create_function(db, name, argc, text_rep, p,
			xFunc, xStep, xFinal);
}

int pico_ql_progress(int n, int (*callback)(void *p), void *p) {
  sqlite3_progress_handler(db, n, callback, p);
  return SQLITE_OK;
}

int pico_ql_interrupt() {
  sqlite3_interrupt(db);
  return SQLITE_INTERRUPT;
}

int pico_ql_shutdown() {
  sqlite3_close(db);
  deinit_vt_selectors();
  return SQLITE_OK;
}

int create_function(const char *name, int argc, int text_rep, void *p,
		  void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
		  void (*xStep)(sqlite3_context*,int,sqlite3_value**),
		  void (*xFinal)(sqlite3_context*)) {
	return pico_ql_create_function(name, argc, text_rep, p,
			xFunc, xStep, xFinal);
}

int progress(int n, int (*callback)(void*), void *p) {
	return pico_ql_progress(n, callback, p);
}

int interrupt() {
	return pico_ql_interrupt();
}

int shutdown() {
	return pico_ql_shutdown();
}

} // extern "C"

} // namespace picoQL
