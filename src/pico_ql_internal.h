/*
 *   Define the virtual table structure and the virtual 
 *   table cursor structure.
 *   Declare the implementation interface 
 *   to pico_ql_search.cpp.
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

#ifndef PICO_QL_INTERNAL_H
#define PICO_QL_INTERNAL_H

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
#include <map>
namespace picoQL {
  class VtblImpl {
  public:
    VtblImpl() {};
    virtual int operator() (sqlite3_vtab_cursor *, int, 
			    int, sqlite3_value *) 
    {return SQLITE_ERROR;};

    virtual int operator() (sqlite3_vtab_cursor *, 
		    int, sqlite3_context *) 
    {return SQLITE_ERROR;};

    virtual void operator() (sqlite3_vtab *, sqlite3_vtab_cursor *) {};

    virtual int operator() (sqlite3_vtab_cursor *)
    {return SQLITE_ERROR;};

    virtual void operator() (sqlite3_vtab_cursor *, void *) {};

    virtual void operator() (sqlite3_vtab_cursor *, int, std::map<sqlite3_vtab_cursor *, bool> *) {};
  };

  extern "C" {
#endif

    typedef struct {
      sqlite3_vtab vtab;
      sqlite3 *db; 
      const char *zDb;
      const char *zName; 
      int embedded; 
      int object;
      int nColumn; 
      char **azColumn;
      void *data; 
      char *zErr;
    } picoQLTable;  // The virtual table struct.
    
    typedef struct {
      sqlite3_vtab_cursor pCsr; 
      int isInstanceNULL;
      int isInstanceEmpty;
      int max_size;        // 
      int size;            // For objects only.
      int current;
      void *resultSet;     // For containers only.
      void *resultSetIter;  // For containers only.
      int resultSetIterState; // For containers only.
      void *resultSetIndex; // For containers only.
      void *textResults;
      int isEof; 
      int first_constr; 
      void *source;
    } picoQLTableCursor; /* The cursor struct for the 
			  * virtual table. 
			  */
    
    int register_vt(picoQLTable *picoQL);
    int equals(const char *zCol, const char *key);
    void set_selectors(); // internal but inconvinient to position
    int init_text_vector(picoQLTableCursor *stc);
    void deinit_text_vector(picoQLTableCursor *stc);
    void deinit_temp_structs();
    void deinit_vt_selectors();
    void init_result_set(sqlite3_vtab *vtab, sqlite3_vtab_cursor *stc);
    int advance_result_set_iter(sqlite3_vtab_cursor *cur);
    void deinit_result_set(sqlite3_vtab_cursor *cur, void *);
    size_t get_datastructure_size(sqlite3_vtab_cursor *cur);
    void get_type(sqlite3_vtab_cursor *cur);
    int search(sqlite3_vtab_cursor *cur, int op, 
	       int nCol, sqlite3_value *val);
    int retrieve(sqlite3_vtab_cursor *cur, int nCol, 
		 sqlite3_context *con);
    
    
#ifdef __cplusplus
  }
}
#endif

#endif
