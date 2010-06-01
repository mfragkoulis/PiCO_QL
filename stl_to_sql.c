#include "stl_to_sql.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

// to globally compile do: g++ -W -g test.cpp -lsqlite3 -o test

/*int main() {
  register_table("foo.db", "CREATE VIRTUAL TABLE classF USING stl(INTEGER PRIMARY KEY AUTOINCREMENT,att1 INT,att2 TEXT,att3 DOUBLE)");
  register_table("foo.db", "CREATE VIRTUAL TABLE classE USING stl(INTEGER PRIMARY KEY AUTOINCREMENT,att1 INT,att2 TEXT,att3 DOUBLE)");
  register_table("foo.db", "CREATE VIRTUAL TABLE classD USING stl(INTEGER PRIMARY KEY AUTOINCREMENT,att1 INT,att2 TEXT,att3 DOUBLE)");
  register_table("foo.db", "CREATE VIRTUAL TABLE classC USING stl(INTEGER PRIMARY KEY AUTOINCREMENT,att1 INT,att2 TEXT,att3 DOUBLE)");
  register_table("foo.db", "CREATE VIRTUAL TABLE classB USING stl(INTEGER PRIMARY KEY AUTOINCREMENT,account_no TEXT,balance FLOAT,classE_id references classE,classF_id references classF)"); 
  register_table("foo.db", "CREATE VIRTUAL TABLE classA USING stl(INTEGER PRIMARY KEY AUTOINCREMENT,model TEXT,km DOUBLE,classC_id references classC,classD_id references classD)");
  register_table("foo.db", "CREATE VIRTUAL TABLE test USING stl(nick_name TEXT,name TEXT,salary INT,classA_id references classA,classB_id references classB)");
  //  register_table("foo.db", "CREATE VIRTUAL TABLE classA USING stl(INTEGER PRIMARY KEY AUTOINCREMENT,model TEXT,km DOUBLE,classC_id references classC,classD_id references classD)");
  //  register_table("foo.db", "CREATE VIRTUAL TABLE classB USING stl(INTEGER PRIMARY KEY AUTOINCREMENT,account_no TEXT,balance FLOAT,classE_id references classE,classF_id references classF)"); 
  //  register_table("foo.db", "CREATE VIRTUAL TABLE classC USING stl(INTEGER PRIMARY KEY AUTOINCREMENT,att1 INT,att2 TEXT,att3 DOUBLE)");
  //  register_table("foo.db", "CREATE VIRTUAL TABLE classD USING stl(INTEGER PRIMARY KEY AUTOINCREMENT,att1 INT,att2 TEXT,att3 DOUBLE)");
  //  register_table("foo.db", "CREATE VIRTUAL TABLE classE USING stl(INTEGER PRIMARY KEY AUTOINCREMENT,att1 INT,att2 TEXT,att3 DOUBLE)");
  //  register_table("foo.db", "CREATE VIRTUAL TABLE classF USING stl(INTEGER PRIMARY KEY AUTOINCREMENT,att1 INT,att2 TEXT,att3 DOUBLE)");
  register_table("foo.db", "DROP TABLE classF");
  register_table("foo.db", "DROP TABLE classE");
  register_table("foo.db", "DROP TABLE classD");
  register_table("foo.db", "DROP TABLE classC");
  register_table("foo.db", "DROP TABLE classB");
  register_table("foo.db", "DROP TABLE classA");
  register_table("foo.db", "DROP TABLE test");

  register_table("foo.db", "CREATE VIRTUAL TABLE account USING stl(INTEGER PRIMARY KEY AUTOINCREMENT,account_no TEXT,balance FLOAT)",);		 
  register_table("foo.db", "CREATE VIRTUAL TABLE Car USING stl(INTEGER PRIMARY KEY AUTOINCREMENT,model TEXT,km DOUBLE)");
  register_table("foo.db", "CREATE VIRTUAL TABLE employees USING stl(nick_name TEXT,name TEXT,salary INT,account_id references account,Car_id references Car)");
  register_table("foo.db", "DROP TABLE account");
  register_table("foo.db", "DROP TABLE Car");
  register_table("foo.db", "DROP TABLE employees");
  
  return 0;
  }*/


// prepare and execute an sql query
int prep_exec(sqlite3 *db, char *q) {
  // printf("in prep_exec...\n");
  sqlite3_stmt  *stmt;
  int result;

  printf("\n NOW PREPARING...\n"); 
  if (sqlite3_prepare_v2(db, q, -1, &stmt, 0)==SQLITE_OK) {
    printf("prepared ok (virtual)\n");
  }
  printf("\n NOW STEPPING... \n");
  result=sqlite3_step(stmt);
  //  cout << "geeet outtahere" << endl;
  if (result==SQLITE_DONE) {
    printf("perfecto!\n");
    result=0;
    //what's this
  } else if (result==SQLITE_OK) {
    printf("ok!\n");
  }else if (result==SQLITE_ERROR) {
    printf("error\n");
  } else if (result==SQLITE_MISUSE) {
    printf("inappropriate use\n");
  } else printf("other\n");
  printf("\n");
  sqlite3_finalize(stmt);
  // printf("prep_exec finished exec query\n");
  return result;
}

// register the module with an open database connection
int register_table(char *ndb, char *q, void *data) {

  printf("\nquery to be executed: %s\n in database: %s\n\n", q, ndb);

  sqlite3 *db;
  int re=sqlite3_open(ndb,&db);
  if (re) {
    printf("can't open database\n");
    sqlite3_close(db);
    exit(1);
  }
  sqlite3_module mod;
  fill_module(&mod);
  //  char q[arrange_size(argc, as)];
  //  void *p;

  int output=sqlite3_create_module(db, "stl", &mod, data);              // hard-coded
  if (output==1) printf("Error while registering module\n");
  else if (output==0) printf("Module registered successfully\n");

  re=prep_exec(db,q);
  sqlite3_close(db);
  return re;
}

// construct the sql query
void create(sqlite3 *db, int argc, char **as, char *q) { 
  int i;

  q[0]='\0';
  // char query;
  strcat(q,"CREATE ");
  strcat(q,"TABLE ");
  strcat(q, as[2]);
  strcat(q,"(");
  for (i=3; i<argc; i++) {
  strcat(q,as[i]);
  if (i+1<argc)  strcat(q,",");
  else  strcat(q,");");
  }
  q[strlen(q)]='\0';
  //  printf("query is: %s with length %i \n", q, strlen(q));
}

int init_vtable(int iscreate, sqlite3 *db, void *paux, int argc, const char * const * argv, sqlite3_vtab **ppVtab, char **pzErr) {
  // printf("in init_vtable...\n");
  stl_table *stl;
  int nDb, nName, nByte, nCol, nString, count, i, k, t;
  char *temp, **store;
  nDb = (int)strlen(argv[1]) + 1;
  nName = (int)strlen(argv[2]) + 1;
  nString=0;
  /*
  mark the first space.name(space)type. then count the total length of the column names.
  assumption: datatype is valid. maybe use fts3 way when integrate it into sqlite3 source code.
  */
  store=(char **)sqlite3_malloc((argc-3) * sizeof(char *));
  memset(store,0,(argc-3) * sizeof(char *));
  for (i=3; i<argc; i++) {
    t=(strlen(argv[i])+1)*sizeof(char);
    store[i-3]=(char *)malloc(t);
    memcpy(store[i-3], argv[i], t);
    for (k=0; k<strlen(store[i-3]) && !isspace(store[i-3][k]); k++) {
      nString++;
    }
    nString++;    // '\0'
    store[i-3][k]='\0';
  }
  nCol=argc - 3;
  assert(nCol > 0);
  nByte = sizeof(stl_table) + nCol * sizeof(char *) + nDb + nName + nString;
  stl = (stl_table *)sqlite3_malloc(nByte);
  if( stl==0 ){
    int rc = SQLITE_NOMEM;
    return rc;
  }
  memset(stl, 0, nByte);
  stl->db=db;
  stl->data=paux;
  stl->nColumn=nCol;
  stl->azColumn=(char **)&stl[1];
  temp=(char *)&stl->azColumn[nCol];

  stl->zName = temp;
  memcpy(temp, argv[2], nName);
  temp += nName;
  stl->zDb = temp;
  memcpy(temp, argv[1], nDb);
  temp += nDb;

  int n;
  for (i=3; i<argc; i++) {
    n=(int)strlen(store[i-3])+1;
    memcpy(temp, store[i-3], n);
    temp[n]='\0';
    stl->azColumn[i-3]=temp;
    temp += n+1;
  }

  char query[arrange_size(argc, argv)];
  create(db, argc, argv, query);
  // query=q.c_str();           // cast from string to const char *
  // printf("query is: %s \n", query);
  if (!(*pzErr)) {
    int output=sqlite3_declare_vtab(db, query);
    if (output==1) {
      *pzErr=sqlite3_mprintf("Error while declaring the virtual table\n");
      printf("%s \n", *pzErr);
      return SQLITE_ERROR;
    } else if (output==0) {
      *ppVtab=&stl->vtab;
      printf("Virtual table declared successfully\n");
      return SQLITE_OK;
    }
  } else {
    *pzErr=sqlite3_mprintf("unknown error");
    printf("%s \n", *pzErr);
  } 
  sqlite3_free(store);
}

//xConnect
int connect_vtable(sqlite3 *db, void *paux, int argc, const char * const * argv, sqlite3_vtab **ppVtab, char **pzErr){
  //   printf("in connect_vtable... \n");
  return init_vtable(0, db, paux, argc, argv, ppVtab, pzErr);
}

// xCreate
int create_vtable(sqlite3 *db, void *paux, int argc, const char * const * argv, sqlite3_vtab **ppVtab, char **pzErr){
  // printf("in create_vtable... \n");
  return init_vtable(1, db, paux, argc, argv, ppVtab, pzErr);
}

int update_vtable(sqlite3_vtab *pVtab, int argc, sqlite3_value **argv, sqlite_int64 *pRowid){

  //update query


}

// xDestroy
int destroy_vtable(sqlite3_vtab *ppVtab) {
  // printf("in destroy_vtable...\n");
  int result;

  /*    need to destroy additional storage structures. so far not any.

  */

  result=disconnect_vtable(ppVtab);
  return result;
}

// xDisconnect
int disconnect_vtable(sqlite3_vtab *ppVtab) {
  stl_table *s=(stl_table *)ppVtab;
  sqlite3_free(s);
  return SQLITE_OK;
}

// xBestindex
int bestindex_vtable(sqlite3_vtab *pVtab, sqlite3_index_info *pInfo) {
  stl_table * st=(stl_table *)pVtab;
  if (pInfo->nConstraint > 0) {            // no constraint no setting up
    char nidxStr[pInfo->nConstraint*2];
    memset(nidxStr, 0, sizeof(nidxStr));

    assert(pInfo->idxStr==0);
    int i, j=0;
    for(i=0; i<pInfo->nConstraint; i++) {
      struct sqlite3_index_constraint *pCons = &pInfo->aConstraint[i];
      if( pCons->usable==0 ) continue;
      switch (pCons->op) {
      case SQLITE_INDEX_CONSTRAINT_LT:  nidxStr[j++]="A"; break;
      case SQLITE_INDEX_CONSTRAINT_LE:  nidxStr[j++]="B"; break;
      case SQLITE_INDEX_CONSTRAINT_EQ:  nidxStr[j++]="C"; break;
      case SQLITE_INDEX_CONSTRAINT_GE:  nidxStr[j++]="D"; break;
      case SQLITE_INDEX_CONSTRAINT_GT:  nidxStr[j++]="E"; break;
	//    case SQLITE_INDEX_CONSTRAINT_MATCH: nidxStr[i]="F"; break;
      }
      nidxStr[j++] = pCons->iColumn - 1 + 'a';
      
      
      
      //    UNUSED_PARAMETER(pVtab);
      
      pInfo->aConstraintUsage[i].argvIndex = 1;
      pInfo->aConstraintUsage[i].omit = 1;
    }
    pInfo->needToFreeIdxStr=1;
    if ( (j>0) && 0==(pInfo->idxStr=sqlite3_mprintf("%s", nidxStr)) ) return SQLITE_NOMEM;
  }
  return SQLITE_OK;
}

// xFilter
int filter_vtable(sqlite3_vtab_cursor *cur, int idxNum, const char *idxStr, int argc, sqlite3_value **argv) {
  stl_table *st=(stl_table *)cur->pVtab;
  stl_table_cursor *stc=(stl_table_cursor *)cur;

  int array_size=get_data_structure_size(st);
  stc->resultset=(int *)sqlite3_malloc(sizeof(int)*array_size);     // will need space at most equal to the data structure size
  memset(stc->resultset, -1 , sizeof(stc->resultset));

  // a data structure to hold index positions of resultset so that in the end of loops the remaining resultset is the wanted one.
  int i, j=0;
  stc->size=array_size;   // initialize size of resultset data structure
  int *initial;
  initial=(int *)sqlite3_malloc(sizeof(int));       // is this wrong? getting unaligned pointer being freed
  *initial=1;
  char constr[3];
  memset(constr, 0, sizeof(constr));
  if (argc==0) search((void *)stc, initial, (void *)st, NULL, NULL);        //empty where clause
  else {
    for(i=0; i<argc; i++) {
      constr[0]=idxStr[j++];
      constr[1]=idxStr[j++];
      constr[2]='\0';
      search((void *)stc, initial, (void *)st, constr, argv[i]);   // case-specific
      if (*initial==-1) break;
      else if (*initial==1) *initial=0;
    }
  }
  sqlite3_free(initial);
  stc->current=-1;
  return next_vtable(cur);
}

//xNext
int next_vtable(sqlite3_vtab_cursor *cur) {
  stl_table *st=(stl_table *)cur->pVtab;
  stl_table_cursor *stc=(stl_table_cursor *)cur;
  if (stc->current>=stc->size -1) stc->isEof=1;
  else stc->current++;
  return SQLITE_OK;
}

// xOpen
int open_vtable(sqlite3_vtab *pVtab, sqlite3_vtab_cursor **ppCsr) {
  sqlite3_vtab_cursor *pCsr;               /* Allocated cursor */
  
  // UNUSED_PARAMETER(pVtab);
  
  *ppCsr = pCsr = (sqlite3_vtab_cursor *)sqlite3_malloc(sizeof(stl_table_cursor));
  if( !pCsr ){
    return SQLITE_NOMEM;
  }
  memset(pCsr, 0, sizeof(stl_table_cursor));
  return SQLITE_OK;
}

//xColumn
int column_vtable(sqlite3_vtab_cursor *cur, sqlite3_context *con, int n) {
  stl_table_cursor *stc=(stl_table_cursor *)cur;
  sqlite3_value *value_back;
  return retrieve((void *)stc, n, con);          // case-specific
}

//xRowid
int rowid_vtable(sqlite3_vtab_cursor *cur, sqlite_int64 *pRowid) {
  
  //needed?

}

//xClose
int close_vtable(sqlite3_vtab_cursor *cur) {
  stl_table_cursor *stc=(stl_table_cursor *)cur;
  sqlite3_free(stc->resultset);
  sqlite3_free(stc);
  return SQLITE_OK;
}

//xEof
int eof_vtable(sqlite3_vtab_cursor *cur) {
  return ((stl_table_cursor *)cur)->isEof;

}

// fill module's function pointers with particular implementations found locally
void fill_module(sqlite3_module *m) {
  m->iVersion=1;
  m->xCreate=create_vtable;
  m->xConnect=connect_vtable;
  m->xBestIndex=bestindex_vtable;
  m->xDisconnect=disconnect_vtable;
  m->xDestroy=destroy_vtable;
  m->xOpen=open_vtable;
  m->xClose=close_vtable;
  m->xEof=eof_vtable;
  m->xFilter=filter_vtable;
  m->xNext=next_vtable;
  m->xColumn=column_vtable;
  m->xRowid=rowid_vtable;
  m->xUpdate=0;
  m->xFindFunction=0;
  m->xBegin=0;
  m->xSync=0;
  m->xCommit=0;
  m->xRollback=0;
  m->xRename=0;
}

// estimate of the query by counting the length of the parameters passed
int arrange_size(int argc, const char * const * argv) {
  int length=28;          // length of standard keywords of sql queries
  int i;
  for (i=0; i<argc; i++) {
    if (i!=1)  length+=strlen(argv[i]) +1;       // +1 for comma or space
  }
  length +=1;                          // sentinel character
  // printf("length is %i \n",length);
  return length;
}
