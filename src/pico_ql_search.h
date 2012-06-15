/*
 *   Define the virtual table structure and the virtual 
 *   table cursor structure.
 *   Declare the interface to stl_search.cpp.
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
  } stlTable;  // The virtual table struct.

  typedef struct {
    sqlite3_vtab_cursor pCsr; 
    int max_size; 
    int *resultSet;
    int size; 
    int current; 
    int isEof; 
    int first_constr; 
    void *source;
  } stlTableCursor; /* The cursor struct for the virtual 
		     * table. 
		     */

  int pico_ql_start();
  void pico_ql_register(void *collection, const char * col_name);
  void fill_module(sqlite3_module *stl);
  void register_vt(stlTable *stl);
  int equals_base(const char *zCol);
  int get_datastructure_size(sqlite3_vtab_cursor *cur);
  int search(sqlite3_vtab_cursor *cur, char *constraint, 
	     sqlite3_value *val);
  int retrieve(sqlite3_vtab_cursor *cur, int n, 
	       sqlite3_context *con);


#ifdef __cplusplus
}
#endif

#endif
