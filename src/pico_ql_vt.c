/*
 *   Implement the callback functions that will allow 
 *   SQLite to manage SQL queries towards the registered 
 *   virtual tables.
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

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "pico_ql_vt.h"
#include "pico_ql_internal.h"

// Constructs the SQL CREATE query.
void create(sqlite3 *db, 
	    int argc, 
	    const char * const * as, 
	    char *q) { 
  (void)db;
  int i;
  q[0] = '\0';
  strcat(q, "CREATE TABLE ");
  strcat(q, as[2]);
  strcat(q, "(");
  for (i = 3; i < argc; i++) {
    strcat(q, as[i]);
    if (i+1 < argc) strcat(q, ",");
    else strcat(q, ");");
  }
  q[strlen(q)] = '\0';
#ifdef PICO_QL_DEBUG
  printf("Query is: %s with length %i \n", q, 
	 (int)strlen(q));
#endif
}

/* Estimates the query length by counting the length of 
 * the parameters passed.
 */
int arrange_size(int argc, const char * const * argv) {
  /* Length of standard keywords of sql queries. */
  int length = 28;
  int i;
  /* + length for all keywords except db_name. + 1 for 
   * following identifier.
   */
  for (i = 0; i < argc; i++) {
    if (i != 1) length += strlen(argv[i]) + 1;
  }
  length += 1;         /*  Sentinel character. */
#ifdef PICO_QL_DEBUG
  printf("length is %i \n",length);
#endif
  return length;
}

/* Calls function to deallocate memory reserved
 * for storing copies of temporary variables.
 */
void clear_temp_structs() {
  deinit_temp_structs();
}

/* Calls function to deallocate memory reserved
 * for storing copies of temporary variables.
 */
void deinit_selectors() {
  deinit_vt_selectors();
}


/* Creates/connects a virtual table to the provided 
 * database.
 * isCreate for activating e.g. extra storage utilised. 
 * Not used so far.
 */
int init_vtable(int iscreate, 
		sqlite3 *db, 
		void *paux, 
		int argc, 
		const char * const * argv, 
		sqlite3_vtab **ppVtab,
		char **pzErr) {
  (void)iscreate;
  (void)paux;
  picoQLTable *picoQL;
  int nDb, nName, nByte, nCol, nString, i;
  char *temp;
  nDb = (int)strlen(argv[1]) + 1;
  nName = (int)strlen(argv[2]) + 1;
  nString=0;
  // explore fts3 way
  for (i = 3; i < argc; i++){
    nString += (int)strlen(argv[i]) + 1;
  }
  nCol = argc - 3;
  assert(nCol > 0);
  nByte = sizeof(picoQLTable) + nCol * sizeof(char *) + 
    nDb + nName + nString;
  picoQL = (picoQLTable *)sqlite3_malloc(nByte);
  if (picoQL == 0) {
    return SQLITE_NOMEM;
  }
  memset(picoQL, 0, nByte);
  picoQL->zErr = NULL;
  picoQL->db = db;
  picoQL->nColumn = nCol;
  picoQL->azColumn = (char **)&picoQL[1];
  temp = (char *)&picoQL->azColumn[nCol];

  picoQL->zName = temp;
  memcpy(temp, argv[2], nName);
  temp += nName;
  picoQL->zDb = temp;
  memcpy(temp, argv[1], nDb);
  temp += nDb;

  int n;
  for (i = 3; i < argc; i++){
    n = (int)strlen(argv[i]) + 1;
    memcpy(temp, argv[i], n);
    picoQL->azColumn[i-3] = temp;
    temp += n;
    assert(temp <= &((char *)picoQL)[nByte]);
  }

  char query[arrange_size(argc, argv)];
  create(db, argc, argv, query);
#ifdef PICO_QL_DEBUG
  printf("Query is: %s \n", query);
#endif
  if (!(*pzErr)) {
    int output = sqlite3_declare_vtab(db, query);
    if (output == 1) {
      *pzErr = sqlite3_mprintf("Error while declaring virtual table %s.\n", picoQL->zName);
      printf("%s \n", *pzErr);
      return SQLITE_ERROR;
    } else if (output == 0) {
      *ppVtab = &picoQL->vtab;
      if (register_vt(picoQL) == SQLITE_ERROR) {
	*pzErr = sqlite3_mprintf("WARNING: Virtual table %s is NULL at the time of registration.\n", picoQL->zName);
	printf("%s \n", *pzErr);
	//	return SQLITE_ERROR;
      }
#ifdef PICO_QL_DEBUG
      printf("Virtual table declared successfully.\n");
#endif
      return SQLITE_OK;
    }
  }else{
    *pzErr = sqlite3_mprintf("Unknown error");
    printf("%s \n", *pzErr);
    return SQLITE_ERROR;
  } 
  return SQLITE_INTERNAL;
}

//xConnect
int connect_vtable(sqlite3 *db, 
		   void *paux, 
		   int argc,
		   const char * const * argv, 
		   sqlite3_vtab **ppVtab,
		   char **pzErr) {
#ifdef PICO_QL_DEBUG
  printf("Connecting vtable %s \n\n", argv[2]);
#endif
  return init_vtable(0, db, paux, argc, argv, 
		     ppVtab, pzErr);
}

// xCreate
int create_vtable(sqlite3 *db, 
		  void *paux, 
		  int argc,
		  const char * const * argv, 
		  sqlite3_vtab **ppVtab,
		  char **pzErr) {
#ifdef PICO_QL_DEBUG
  printf("Creating vtable %s \n\n", argv[2]);
#endif
  return init_vtable(1, db, paux, argc, argv, ppVtab, pzErr);
}

// xDestroy
int destroy_vtable(sqlite3_vtab *ppVtab) {
#ifdef PICO_QL_DEBUG
  picoQLTable *st = (picoQLTable *)ppVtab;
  printf("Destroying vtable %s \n\n", st->zName);
#endif
  int result;
  // Need to destroy additional structures. So far none.
  result = disconnect_vtable(ppVtab);
  return result;
}

// xDisconnect. Called when closing a database connection.
int disconnect_vtable(sqlite3_vtab *ppVtab) {
  picoQLTable *s=(picoQLTable *)ppVtab;
#ifdef PICO_QL_DEBUG
  printf("Disconnecting vtable %s \n\n", s->zName);
#endif
  sqlite3_free(s);
  return SQLITE_OK;
}

/* Helper function for structuring an SQL WHERE 
 * constraint.
 */
void eval_constraint(int sqlite3_op, 
		     const char *colName,
		     int nCol, 
		     int score,
		     char *nidxStr, 
		     int nidxLen) {
  char iOp[2], iCol[4]; // iCol supports up to 999 columns.
  sprintf(iCol, "%d", nCol);
  int op = 0;
  switch (sqlite3_op) {
  case SQLITE_INDEX_CONSTRAINT_LT:
    op = 0;
    break;
  case SQLITE_INDEX_CONSTRAINT_LE:
    op = 1;
    break;
  case SQLITE_INDEX_CONSTRAINT_EQ:
    op = 2;
    break;
  case SQLITE_INDEX_CONSTRAINT_GE:
    op = 3;
    break;
  case SQLITE_INDEX_CONSTRAINT_GT:
    op = 4;
    break;
  }
  sprintf(iOp, "%d", op); 
  int iColLen = (int)strlen(iCol);
  char op_iCol[iColLen + 5];
  if (equals(colName, "base")) {
    assert(nCol == 0);
    nidxStr[0] = '{';
    nidxStr[1] = *iOp;     // 7 evaluation qualifiers
    nidxStr[2] = '-';
    nidxStr[3] = '0';      // should always be 0
    nidxStr[4] = '}';
  } else if (equals(colName, "rownum")) {
    assert((nCol == 0) || (nCol == 1));
    if (score > 2) {
      nidxStr[5] = '{';
      nidxStr[6] = *iOp;     
      nidxStr[7] = '-';
      nidxStr[8] = *iCol;    // 0 or 1 always
      nidxStr[9] = '}';
    } else {
      nidxStr[0] = '{';
      nidxStr[1] = *iOp;
      nidxStr[2] = '-';
      nidxStr[3] = *iCol;    // 0 or 1 always
      nidxStr[4] = '}';
    }
  } else {
    assert((int)strlen(nidxStr) < nidxLen - iColLen - 5);
    sprintf(op_iCol, "{%s-%s}", iOp, iCol);
    strcat(nidxStr, op_iCol);
  }
}

/*
 */
void order_constraints(int score, int *j, int *counter, 
		       char *nidxStr) {
  switch (score) {
  case 0:
    *j = 0;
    *counter = 1;
    break;
  case 1:
    *j = 5;
    *counter = 2;
    break;
  case 2:
    *j = 5;
    *counter = 2;
    break;
  case 3:
    *j = 10;
    *counter = 3;
    break;
  }
  if (*j > 0) strncpy(nidxStr, "reservedsp", (size_t)*j);
}

/* xBestindex. Defines the query plan for an SQL query. 
 * Might be called multiple times with alternate plans.
 */
int best_index_vtable(sqlite3_vtab *pVtab, 
		     sqlite3_index_info *pInfo) {
  picoQLTable *st=(picoQLTable *)pVtab;
  /* No constraint no setting up. */
  if (pInfo->nConstraint > 0) {
    int nCol;
    int nidxLen = pInfo->nConstraint*7 + 1;
    char nidxStr[nidxLen];
    memset(nidxStr, 0, sizeof(nidxStr));
    assert(pInfo->idxStr == 0);
    int i, j = 0, counter = 0, score = 0;
    for (i = 0; i < pInfo->nConstraint; i++){
      struct sqlite3_index_constraint *pCons = 
	&pInfo->aConstraint[i];
      if (pCons->usable == 0) 
	continue;
      nCol = pCons->iColumn;
      if (equals(st->azColumn[nCol], "base"))
	score += 2;
      else if (equals(st->azColumn[nCol], "rownum"))
	score += 1;
    }
    order_constraints(score, &j, &counter, nidxStr);
    for (i = 0; i < pInfo->nConstraint; i++) {
      struct sqlite3_index_constraint *pCons = 
	&pInfo->aConstraint[i];
      if (pCons->usable == 0) 
	continue;
      nCol = pCons->iColumn;
      if (equals(st->azColumn[nCol], "base")) {
	pInfo->aConstraintUsage[i].argvIndex = 1;
      } else if (equals(st->azColumn[nCol], "rownum")) {
	if (score > 2)
	  pInfo->aConstraintUsage[i].argvIndex = 2;
	else
	  pInfo->aConstraintUsage[i].argvIndex = 1;	  
      } else {
	pInfo->aConstraintUsage[i].argvIndex = counter++;
      }
      eval_constraint(pCons->op, st->azColumn[nCol], nCol,
		      score, nidxStr, nidxLen);
      pInfo->aConstraintUsage[i].omit = 1;
    }
    pInfo->needToFreeIdxStr = 1;
    if ((((int)strlen(nidxStr)) > 0) && 
      0 == (pInfo->idxStr = 
	    sqlite3_mprintf("%s", nidxStr)))
      return SQLITE_NOMEM;
  }
  return SQLITE_OK;
}

/* xFilter. Filters an SQL query. Calls the search 
 * family of callbacks at pico_ql_search.cpp.
 */
int filter_vtable(sqlite3_vtab_cursor *cur, 
		  int idxNum, 
		  const char *idxStr,
		  int argc, 
		  sqlite3_value **argv) {
  (void)idxNum;
  picoQLTableCursor *stc=(picoQLTableCursor *)cur;
  int re = 0;
  /* Initialize size of resultset data structure for objects. */
  /* Unused in containers. */
  stc->size = 0;
  stc->current = -1;      /* Initial cursor position. */

  /* In case of a join, xfilter will be called many times, 
   *  x times for x eligible rows of the paired table. 
   * In this case isEof will be set to terminate at row 
   * level and has to be reset to allow matching all 
   * eligible rows.
   */
  stc->isEof = 0;         
  /* First_constr is used to signal that the current 
   * constr encountered is the first (value 1) 
   * or not (value 0).
   */
  stc->first_constr = 1;  
  if (argc == 0) {        /* Empty where clause. */
    if ((re = search(cur, 0, 0, NULL)) != 0)
      return re;
  } else {
    int i = 0, op[argc], nCol[argc];
    char *token, where[(int)strlen(idxStr)+1];
    strcpy(where, idxStr);
    token = strtok((char *)where, "{-}"); 
    while (token != NULL) {     // constr: {<op>-<nCol>}
      op[i] = atoi(token);
      token = strtok(NULL, "{-}");
      nCol[i] = atoi(token);
      token = strtok(NULL, "{-}");
      i++;
    }
    for (i = 0; i < argc; i++) {
      if ((re = search(cur, op[i], nCol[i], argv[i])) != 0)
	return re;
    }
  }
  return next_vtable(cur);
}

//xNext. Advances the cursor to next record of resultset.
int next_vtable(sqlite3_vtab_cursor *cur) {
  return advance_result_set_iter(cur);
}


/* xOpen. Opens the virtual table struct to be used 
 * in an SQL query. Triggered by the FROM clause. 
 * Initialises cursor.
 */
int open_vtable(sqlite3_vtab *pVtab, 
		sqlite3_vtab_cursor **ppCsr) {
  picoQLTable *st = (picoQLTable *)pVtab;
#ifdef PICO_QL_DEBUG
  printf("Opening vtable %s\n\n", st->zName);
#endif
  sqlite3_vtab_cursor *pCsr;    /* Allocated cursor */

  *ppCsr = pCsr = 
    (sqlite3_vtab_cursor *)sqlite3_malloc(sizeof(picoQLTableCursor));
  if (!pCsr) {
    return SQLITE_NOMEM;
  }
  memset(pCsr, 0, sizeof(picoQLTableCursor));
  pCsr->pVtab = &st->vtab;
  picoQLTableCursor *stc = (picoQLTableCursor *)pCsr;
  /* Keep copy of initial data. Might change in search. 
   * Useful when multiple instances of the VT are open.
   */
  stc->source = st->data;
  st->locked = 0;
  /* To allocate space for the resultset.
   * Will need space at most equal to the data structure 
   * size. This is fixed for autonomous structs, variable 
   * for embedded ones (will be taken care of in search).
   */
  if (!st->embedded) {
    if (stc->source == NULL) {
      stc->isInstanceNULL = 1;
      stc->size = 1;
    } else {
      stc->isInstanceNULL = 0;
      int arraySize = (int)get_datastructure_size(pCsr, pVtab);
      if (arraySize == 0) {
	stc->isInstanceEmpty = 1;
	stc->size = 1;
      }	else {
	stc->isInstanceEmpty = 0;
	stc->max_size = arraySize;
      }
      //      pCsr->pVtab = NULL;   why set to NULL?
    }
  } else {
    /* Embedded struct. Size will be synced in search when 
     * powered from source. Calling get_datastructure_size()
     * to get type of data structure represented (object or 
     * container).
     */
    sqlite3_vtab_cursor *c;
    c = NULL;
    get_datastructure_size(c, pVtab);
    stc->max_size = 1;
  }
  init_result_set(pVtab, pCsr);
#ifdef PICO_QL_DEBUG
  printf("ppCsr = %lx, pCsr = %lx \n", 
	 (long unsigned int)ppCsr, 
	 (long unsigned int)pCsr);
#endif 
  return SQLITE_OK;
}

/* xColumn. Calls the retrieve family of functions at 
 * pico_ql_search.cpp. Returns the value of column $n for 
 * record pointed at by $cur.
 */
int column_vtable(sqlite3_vtab_cursor *cur, 
		  sqlite3_context *con, 
		  int n) {
  return retrieve(cur, n, con);
}

/* xClose. Closes the virtual table after the completion 
 * of a query.
 */
int close_vtable(sqlite3_vtab_cursor *cur) {
  picoQLTableCursor *stc = (picoQLTableCursor *)cur;
#ifdef PICO_QL_DEBUG
  picoQLTable *st = (picoQLTable *)cur->pVtab;
  printf("Closing vtable %s \n\n", st->zName);
#endif
  // Second argument dummy
  // (part of polymorphized arsenal of methods).
  deinit_result_set(cur, stc);
  sqlite3_free(stc);
  return SQLITE_OK;
}

//xEof. Signifies the end of resultset.
int eof_vtable(sqlite3_vtab_cursor *cur) {
  return ((picoQLTableCursor *)cur)->isEof;
}

/* Fills virtual table module's function pointers with 
 * implemented callback functions.
 */
void fill_module(sqlite3_module *m) {
  m->iVersion = 1;
  m->xCreate = create_vtable;
  m->xConnect = connect_vtable;
  m->xBestIndex = best_index_vtable;
  m->xDisconnect = disconnect_vtable;
  m->xDestroy = destroy_vtable;
  m->xOpen = open_vtable;
  m->xClose = close_vtable;
  m->xEof = eof_vtable;
  m->xFilter = filter_vtable;
  m->xNext = next_vtable;
  m->xColumn = column_vtable;
  m->xRowid = 0;
  m->xUpdate = 0;
  m->xFindFunction = 0;
  m->xBegin = 0;
  m->xSync = 0;
  m->xCommit = 0;
  m->xRollback = 0;
  m->xRename = 0;
}
