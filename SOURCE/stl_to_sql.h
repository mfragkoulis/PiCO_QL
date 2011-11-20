#ifndef STL_TO_SQL_H
#define STL_TO_SQL_H

#include "sqlite3.h"
#include "search.h"

int create_vtable(sqlite3 *db, void *paux, int argc, 
		  const char * const * argv, sqlite3_vtab **ppVtab, 
		  char **pzErr);
  
int connect_vtable(sqlite3 *db, void *paux, int argc, 
		   const char * const * argv, sqlite3_vtab **ppVtab, 
		   char **pzErr);
  
int init_vtable(int iscreate, sqlite3 *db, void *paux, int argc, 
		const char * const * argv, sqlite3_vtab **ppVtab, 
		char **pzErr);

int update_vtable(sqlite3_vtab *pVtab, int argc, sqlite3_value **argv, 
		  sqlite_int64 *pRowid);

int destroy_vtable(sqlite3_vtab  *ppVtab);

int bestindex_vtable(sqlite3_vtab *pVTab, sqlite3_index_info *pinfo);

int filter_vtable(sqlite3_vtab_cursor *cur, int idxNum, 
		  const char *idxStr, int argc, sqlite3_value **argv);

int next_vtable(sqlite3_vtab_cursor *cur);

int column_vtable(sqlite3_vtab_cursor *cur, sqlite3_context *con, int n);

int rowid_vtable(sqlite3_vtab_cursor *cur, sqlite_int64 *pRowid);

int open_vtable(sqlite3_vtab  *ppVtab, sqlite3_vtab_cursor **ppCsr);

int eof_vtable(sqlite3_vtab_cursor *cur);

int close_vtable(sqlite3_vtab_cursor *cur);

void create(sqlite3 *db, int argc, const char * const * as, char *q);

int disconnect_vtable(sqlite3_vtab *ppVtab);

int arrange_size(int argc, const char * const * argv);

typedef struct {sqlite3_vtab vtab; sqlite3 *db; const char *zDb; 
  const char *zName; int nColumn; char **azColumn; 
  void *data;} stlTable;

typedef struct {sqlite3_vtab_cursor vtab; int *resultSet; int size; 
  int current; int isEof;int max_size; int first_constr;} stlTableCursor;

/*
typedef struct {long int **memories; const char** dsNames; int **set_memories; int size;} dsCarrier;

typedef struct {long int *mem; int *set_mem; dsCarrier *children;} data;

#endif
*/

typedef struct {const char *child; int set;} relationship;

typedef struct {long int *memory; const char *dsName; int *set_memory;} attrCarrier;

typedef struct {attrCarrier *attr; relationship **rlt; int rlt_size;} dsCarrier;

typedef struct {long int *mem; int *set_mem; attrCarrier **children; int children_size;} data;

typedef struct {dsCarrier **ds; int ds_size;} dsArray; 

#endif
