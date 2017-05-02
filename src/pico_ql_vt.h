/*
 *   Declare the interface to pico_ql_vt.c
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

#ifndef PICO_QL_VT_H
#define PICO_QL_VT_H

#include <sqlite3.h>


int create_vtable(sqlite3 *db, 
		  void *paux, 
		  int argc, 
		  const char * const * argv, 
		  sqlite3_vtab **ppVtab, 
		  char **pzErr);
  
int connect_vtable(sqlite3 *db, 
		   void *paux, 
		   int argc, 
		   const char * const * argv, 
		   sqlite3_vtab **ppVtab, 
		   char **pzErr);
  
int init_vtable(int iscreate, 
		sqlite3 *db, 
		void *paux, 
		int argc, 
		const char * const * argv, 
		sqlite3_vtab **ppVtab, 
		char **pzErr);

int update_vtable(sqlite3_vtab *pVtab, 
		  int argc, 
		  sqlite3_value **argv, 
		  sqlite_int64 *pRowid);

int destroy_vtable(sqlite3_vtab  *ppVtab);

int best_index_vtable(sqlite3_vtab *pVTab,
		     sqlite3_index_info *pinfo);

int filter_vtable(sqlite3_vtab_cursor *cur, 
		  int idxNum, 
		  const char *idxStr, 
		  int argc, 
		  sqlite3_value **argv);

int next_vtable(sqlite3_vtab_cursor *cur);

int column_vtable(sqlite3_vtab_cursor *cur, 
		  sqlite3_context *con, 
		  int n);

int rowid_vtable(sqlite3_vtab_cursor *cur, 
		 sqlite_int64 *pRowid);

int open_vtable(sqlite3_vtab  *ppVtab, 
		sqlite3_vtab_cursor **ppCsr);

int eof_vtable(sqlite3_vtab_cursor *cur);

int close_vtable(sqlite3_vtab_cursor *cur);

void create(sqlite3 *db, 
	    int argc, 
	    const char * const * as, 
	    char *q);

int disconnect_vtable(sqlite3_vtab *ppVtab);

void clear_temp_structs(void);

void start_serving(void);

#endif
