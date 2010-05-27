#ifndef STL_TO_SQL_H
#define STL_TO_SQL_H

#include "sqlite3.h"
#include "bridge.h"

typedef unsigned char boolean;
#define true (0==0)
#define false (!true)

int register_table(char *ndb, char *query);

int create_vtable(sqlite3 *db, void *paux, int argc, const char * const * argv, sqlite3_vtab **ppVtab, char **pzErr);
  
int connect_vtable(sqlite3 *db, void *paux, int argc, const char * const * argv, sqlite3_vtab **ppVtab, char **pzErr);
  
int init_vtable(int iscreate, sqlite3 *db, void *paux, int argc, const char * const * argv, sqlite3_vtab **ppVtab, char **pzErr);

int update_vtable(sqlite3_vtab *pVtab, int argc, sqlite3_value **argv, sqlite_int64 *pRowid);

int destroy_vtable(sqlite3_vtab  *ppVtab);

int bestindex_vtable(sqlite3_vtab *pVTab, sqlite3_index_info *pinfo);

int filter_vtable(sqlite3_vtab_cursor *cur, int idxNum, const char *idxStr, int argc, sqlite3_value **argv);

int next_vtable(sqlite3_vtab_cursor *cur);

int column_vtable(sqlite3_vtab_cursor *cur, sqlite3_context *con, int n);

int rowid_vtable(sqlite3_vtab_cursor *cur, sqlite_int64 *pRowid);

int open_vtable(sqlite3_vtab  *ppVtab, sqlite3_vtab_cursor **ppCsr);

int eof_vtable(sqlite3_vtab_cursor *cur);

int close_vtable(sqlite3_vtab_cursor *cur);

void create(sqlite3 *db, int argc, char **as, char *q);

int prep_exec(sqlite3 *db, char *query);

int disconnect_vtable(sqlite3_vtab *ppVtab);

void fill_module(sqlite3_module *m);

int arrange_size(int argc, const char * const * argv);

//int sql_register_stl(char *name, collect *c);                                                                                                                                                                  


typedef struct {boolean e_map;boolean e_collection;} e_datastructure;

typedef struct {sqlite3_module *module;} my_module;

typedef struct {sqlite3_vtab vtab; sqlite3 *db; const char *zDb; const char *zName; int nColumn; char **azColumn; sqlite3_index_info *pInfo; void *data;} stl_table;

typedef struct {sqlite3_vtab_cursor vtab; int *resultset; int isEof;} stl_table_cursor;

#endif
