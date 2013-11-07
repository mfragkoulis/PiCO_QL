/*
 *   Define the virtual table structure and the virtual 
 *   table cursor structure.
 *   Declare the implementation interface 
 *   to pico_ql_internal.cpp.
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
#ifdef _WIN64
#define ENVIRONMENT64
#elif defined(_WIN32)
#define ENVIRONMENT32
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

  class Cursor {
  public:
    sqlite3_vtab_cursor *cur;
    int open;
    int counter;

    Cursor(sqlite3_vtab_cursor *cu, int o, int cnt){cur = cu; 
                                                    open = o;
                                                    counter = cnt;};
    Cursor(){};
    ~Cursor(){};
  };

  class VtblImpl {
  public:
    VtblImpl() {};

    virtual ~VtblImpl() {};

    virtual int operator() (sqlite3_vtab_cursor *, int, 
			    int, sqlite3_value *) 
    {return SQLITE_ERROR;};

    virtual int operator() (sqlite3_vtab_cursor *, 
		    int, sqlite3_context *) 
    {return SQLITE_ERROR;};

    virtual void operator() (sqlite3_vtab *) {};

    virtual size_t operator() (sqlite3_vtab_cursor *, sqlite3_vtab *) {return SQLITE_ERROR;};

    virtual int operator() (sqlite3_vtab *, sqlite3_vtab_cursor *) {return SQLITE_ERROR;};

    virtual int operator() (sqlite3_vtab_cursor *)
    {return SQLITE_ERROR;};

    virtual void operator() (sqlite3_vtab_cursor *, void *) {};

    virtual int operator() (Cursor *, int, std::map<Cursor *, bool> *, long) {return SQLITE_ERROR;};

    virtual void operator() (const char *) {};
  };

  extern "C" {
#endif

    typedef struct {
      sqlite3_vtab vtab;
      sqlite3 *db;
      const char *zDb;
      const char *zName;
      int toOpen;
      int locked;
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
      int locked;
      long int lock;
      int active_verify;
      int active_checked;
      int active_owed;
      int max_size;        // 
      int size;            // For objects only.
      int offset;         // For objects only.
      int isEof; 
      int first_constr; 
      void *source;
    } picoQLTableCursor; /* The cursor struct for the 
			  * virtual table. 
			  */
    void * thread_sqlite(void *data);
    int register_vt(picoQLTable *picoQL);
    int equals(const char *zCol, const char *key);
    void set_selectors(void); // internal but inconvenient to position
    void deinit_temp_structs(void);
    void deinit_vt_selectors(void);
    int init_result_set(sqlite3_vtab *vtab, sqlite3_vtab_cursor *stc);
    int advance_result_set_iter(sqlite3_vtab_cursor *cur);
    void deinit_result_set(sqlite3_vtab_cursor *cur, void *);
    int toOpen(sqlite3_vtab *);
    size_t get_datastructure_size(sqlite3_vtab_cursor *cur, sqlite3_vtab *vtab);
    int search(sqlite3_vtab_cursor *cur, int op, 
	       int nCol, sqlite3_value *val);
    int retrieve(sqlite3_vtab_cursor *cur, int nCol, 
		 sqlite3_context *con);
    
    
#ifdef __cplusplus
  }
}
#endif

#endif
