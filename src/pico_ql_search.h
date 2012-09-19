/*
 *   Define the virtual table structure and the virtual 
 *   table cursor structure.
 *   Declare the interface to pico_ql_search.cpp.
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
 *   distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 *   express or implied.
 *   See the License for the specific language governing
 *   permissions and limitations under the License.
 */

#ifndef PICO_QL_SEARCH_H
#define PICO_QL_SEARCH_H

// Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

#include <sqlite3.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {
    sqlite3_vtab vtab;
    sqlite3 *db; 
    const char *zDb;
    const char *zName; 
    int embedded; 
    int nColumn; 
    char **azColumn;
    void *data; 
    char *zErr;
  } picoQLTable;  // The virtual table struct.

  typedef struct {
    sqlite3_vtab_cursor pCsr; 
    int isInstanceNULL;
    int isInstanceEmpty;
    int max_size; 
    int *resultSet;
    int size; 
    int current; 
    void *textResults;
    int isEof; 
    int first_constr; 
    void *source;
  } picoQLTableCursor; /* The cursor struct for the 
			* virtual table. 
			*/

  int pico_ql_serve();
  void pico_ql_register(const void *collection, const char * col_name);
  int register_vt(picoQLTable *picoQL);
  int equals(const char *zCol, const char *key);
  int init_text_vector(picoQLTableCursor *stc);
  void deinit_text_vector(picoQLTableCursor *stc);
  void deinit_temp_lists();
  int get_datastructure_size(sqlite3_vtab_cursor *cur);
  int search(sqlite3_vtab_cursor *cur, int op, 
	     int nCol, sqlite3_value *val);
  int retrieve(sqlite3_vtab_cursor *cur, int nCol, 
	       sqlite3_context *con);


#ifdef __cplusplus
}
#endif

#endif
