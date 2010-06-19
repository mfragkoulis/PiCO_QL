#include "stl_to_sql.h"
#include "bridge.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

// to globally compile do: g++ -W -g test.cpp -lsqlite3 -o test


// construct the sql query
void create(sqlite3 *db, int argc, char **as, char *q){ 
  int i;

  q[0] = '\0';
  // char query;
  strcat(q,"CREATE ");
  strcat(q,"TABLE ");
  strcat(q, as[2]);
  strcat(q,"(");
  for(i=3; i<argc; i++){
  strcat(q,as[i]);
  if( i+1<argc ) strcat(q,",");
  else strcat(q,");");
  }
  q[strlen(q)] = '\0';
  //  printf("query is: %s with length %i \n", q, strlen(q));
}

int init_vtable(int iscreate, sqlite3 *db, void *paux, int argc, 
		const char * const * argv, sqlite3_vtab **ppVtab,
		char **pzErr){
  // printf("in init_vtable...\n");
  stlTable *stl;
  int nDb, nName, nByte, nCol, nString, count, i, k, t;
  char *temp, **store;
  int *colType;
  colType = (int *)malloc(sizeof(int) * (argc-3));
  memset(colType, -1, (argc-3) * sizeof(int));
  nDb = (int)strlen(argv[1]) + 1;
  nName = (int)strlen(argv[2]) + 1;
  nString=0;
  
  /*
  mark the first space.name(space)type. then count the total length of the
  column names.
  assumption: datatype is valid. maybe use fts3 way when integrate it into
  sqlite3 source code.
  */
  
  store = (char **)sqlite3_malloc((argc-3) * sizeof(char *));
  memset(store,0,(argc-3) * sizeof(char *));

  //to make column datatype available
  char *result1 = NULL;
  char *result2 = NULL;

  const char *intType = "integer";
  const char *pkType = "primary";
  const char *fkType = "references";
  const char *textType = "text";
  const char *doubleType = "double";
  const char *floatType = "float";
  const char *blobType = "blob";

  int l, columnData=1;
  for(i=3; i<argc; i++){
    t = (strlen(argv[i])+1) * sizeof(char);
    store[i-3] = (char *)malloc(t);
    memcpy(store[i-3], argv[i], t);

    // column name
    result1 = strtok(store[i-3]," ");
    // column data type
    result2 = strtok(NULL, " ");
    for(l=0; l<(int)strlen(result2); l++){
      result2[l] = tolower(result2[l]);
    }
    if ( !strcmp(result2, intType) ){
      colType[i-3] = 0; 
    }else if( !strcmp(result2, pkType) ){
      colType[i-3] = 0;
      if( t<3*sizeof(char))
	store[i-3] = (char *)realloc(store[i-3], 3*sizeof(char));
      store[i-3] = "PK";
      columnData = 0; 
    }else if( !strcmp(result2, fkType) ) {
      colType[i-3] = 0;
      if( t< (4 + strlen(result1)) * sizeof(char)) 
	store[i-3] = (char *)realloc(store[i-3],
				     (4 + strlen(result1)) * sizeof(char));
      store[i-3] = "FK "; 
      strcat(store[i-3], result1); 
    }else if( !strcmp(result2, textType) ){
      colType[i-3] = 1;
    }else if( !strcmp(result2, doubleType) ){ 
      colType[i-3] = 2;
    }else if( !strcmp(result2, floatType) ){ 
      colType[i-3] = 2;
    }else if( !strcmp(result2, blobType) ){
      colType[i-3] = 3;
    }
    for(k=0; k<(int)strlen(store[i-3]) && !isspace(store[i-3][k]); k++){
      nString++;
    }
    if( columnData ){
      // '\0'
      nString++;
      store[i-3][k] = '\0';
    }
  }
  nCol = argc - 3;
  assert( nCol > 0 );
  nByte = sizeof(stlTable) + nCol * sizeof(char *) + nDb + nName + nString;
  stl = (stlTable *)sqlite3_malloc(nByte);
  if( stl==0 ){
    int rc = SQLITE_NOMEM;
    return rc;
  }
  memset(stl, 0, nByte);
  stl->db = db;
  stl->data = paux;
  stl->nColumn = nCol;
  stl->colDataType = colType;
  stl->azColumn=(char **)&stl[1];
  temp=(char *)&stl->azColumn[nCol];

  stl->zName = temp;
  memcpy(temp, argv[2], nName);
  temp += nName;
  stl->zDb = temp;
  memcpy(temp, argv[1], nDb);
  temp += nDb;

  int n;
  for(i=3; i<argc; i++){
    n=(int)strlen(store[i-3])+1;
    memcpy(temp, store[i-3], n);
    temp[n] ='\0';
    stl->azColumn[i-3] = temp;
    temp += n+1;
  }

  char query[arrange_size(argc, argv)];
  create(db, argc, argv, query);
  // query=q.c_str();           // cast from string to const char *
  // printf("query is: %s \n", query);
  if( !(*pzErr) ){
    int output = sqlite3_declare_vtab(db, query);
    if( output==1 ){
      *pzErr = sqlite3_mprintf("Error while declaring the virtual table\n");
      printf("%s \n", *pzErr);
      return SQLITE_ERROR;
    }else if( output==0 ){
      *ppVtab = &stl->vtab;
      printf("Virtual table declared successfully\n");
      return SQLITE_OK;
    }
  }else{
    *pzErr = sqlite3_mprintf("unknown error");
    printf("%s \n", *pzErr);
  } 
  sqlite3_free(store);
}

//xConnect
int connect_vtable(sqlite3 *db, void *paux, int argc,
		   const char * const * argv, sqlite3_vtab **ppVtab,
		   char **pzErr){
  //   printf("in connect_vtable... \n");
  return init_vtable(0, db, paux, argc, argv, ppVtab, pzErr);
}

// xCreate
int create_vtable(sqlite3 *db, void *paux, int argc,
		  const char * const * argv, sqlite3_vtab **ppVtab,
		  char **pzErr){
  // printf("in create_vtable... \n");
  return init_vtable(1, db, paux, argc, argv, ppVtab, pzErr);
}

int update_vtable(sqlite3_vtab *pVtab, int argc, sqlite3_value **argv,
		  sqlite_int64 *pRowid){

  //update query


}

// xDestroy
int destroy_vtable(sqlite3_vtab *ppVtab){
  // printf("in destroy_vtable...\n");
  int result;

  /*    need to destroy additional storage structures. so far not any.

  */

  result = disconnect_vtable(ppVtab);
  return result;
}

// xDisconnect
int disconnect_vtable(sqlite3_vtab *ppVtab){
  stlTable *s=(stlTable *)ppVtab;
  sqlite3_free(s);
  return SQLITE_OK;
}

// xBestindex
int bestindex_vtable(sqlite3_vtab *pVtab, sqlite3_index_info *pInfo){
  stlTable *st=(stlTable *)pVtab;
  if ( pInfo->nConstraint>0 ){            // no constraint no setting up
    char op, iCol;
    char nidxStr[pInfo->nConstraint*2];
    memset(nidxStr, 0, sizeof(nidxStr));

    assert( pInfo->idxStr==0 );
    int i, j=0;
    for(i=0; i<pInfo->nConstraint; i++){
      struct sqlite3_index_constraint *pCons = &pInfo->aConstraint[i];
      if( pCons->usable==0 ) continue;
      switch ( pCons->op ){
      case SQLITE_INDEX_CONSTRAINT_LT:
	op='A'; 
	break;
      case SQLITE_INDEX_CONSTRAINT_LE:  
	op='B'; 
	break;
      case SQLITE_INDEX_CONSTRAINT_EQ:  
	op='C'; 
	break;
      case SQLITE_INDEX_CONSTRAINT_GE:  
	op='D'; 
	break;
      case SQLITE_INDEX_CONSTRAINT_GT:  
	op='E'; 
	break;
	//    case SQLITE_INDEX_CONSTRAINT_MATCH: nidxStr[i]="F"; break;
      }
      iCol = pCons->iColumn - 1 + 'a';
      
      nidxStr[j++] = op;
      nidxStr[j++] = iCol;
      //    UNUSED_PARAMETER(pVtab);
      
      pInfo->aConstraintUsage[i].argvIndex = 1;
      pInfo->aConstraintUsage[i].omit = 1;
    }
    pInfo->needToFreeIdxStr = 1;
    if( (j>0) && 0==(pInfo->idxStr=sqlite3_mprintf("%s", nidxStr)) )
      return SQLITE_NOMEM;
  }
  return SQLITE_OK;
}

// xFilter
int filter_vtable(sqlite3_vtab_cursor *cur, int idxNum, const char *idxStr,
		  int argc, sqlite3_value **argv){

  stlTable *st=(stlTable *)cur->pVtab;
  stlTableCursor *stc=(stlTableCursor *)cur;

  memset(stc->resultSet, -1, sizeof(stc->resultSet));

  // initialize size of resultset data structure
  stc->size = 0;

  // initial cursor position
  stc->current=-1;

  // in case of a join xfilter will be called many times, x times for x 
  // eligible rows of the paired table in this case isEof will be set to 
  // terminate at row level and has to be reset to allow matching all 
  // eligible rows
  stc->isEof=0;

  int i, j=0;
  char *constr = (char *)sqlite3_malloc(sizeof(char) * 3);
  memset(constr, 0, sizeof(constr));

  //empty where clause
  if( argc==0 ) search((void *)stc, NULL, NULL);
  else{
    for(i=0; i<argc; i++) {
      constr[0] = idxStr[j++];
      constr[1] = idxStr[j++];
      constr[2] = '\0';
      search((void *)stc, constr, argv[i]);
    }
  }
  sqlite3_free(constr);
  return next_vtable(cur);
}

//xNext
int next_vtable(sqlite3_vtab_cursor *cur){
  stlTable *st=(stlTable *)cur->pVtab;
  stlTableCursor *stc=(stlTableCursor *)cur;
  if ( stc->current>=stc->size-1 ) stc->isEof = 1;
  else stc->current++;
  return SQLITE_OK;
}

// xOpen
int open_vtable(sqlite3_vtab *pVtab, sqlite3_vtab_cursor **ppCsr){
  sqlite3_vtab_cursor *pCsr;               /* Allocated cursor */
    
  *ppCsr = pCsr = 
    (sqlite3_vtab_cursor *)sqlite3_malloc(sizeof(stlTableCursor));
  if( !pCsr ){
    return SQLITE_NOMEM;
  }
  memset(pCsr, 0, sizeof(stlTableCursor));

  stlTableCursor *stc=(stlTableCursor *)pCsr;
  stlTable *st=(stlTable *)pVtab;

  int arraySize=get_datastructure_size(st);

  // a data structure to hold index positions of resultset so that in the end
  // of loops the remaining resultset is the wanted one.
  // will need space at most equal to the data structure size
  stc->resultSet=(int *)sqlite3_malloc(sizeof(int) * arraySize);

  return SQLITE_OK;
}

//xColumn
int column_vtable(sqlite3_vtab_cursor *cur, sqlite3_context *con, int n){
  stlTableCursor *stc=(stlTableCursor *)cur;

  // case specific
  return retrieve((void *)stc, n, con);
}

//xRowid
int rowid_vtable(sqlite3_vtab_cursor *cur, sqlite_int64 *pRowid){
  
  //needed?

}

//xClose
int close_vtable(sqlite3_vtab_cursor *cur){
  stlTableCursor *stc=(stlTableCursor *)cur;
  sqlite3_free(stc->resultSet);
  sqlite3_free(stc);
  return SQLITE_OK;
}

//xEof
int eof_vtable(sqlite3_vtab_cursor *cur){
  return ((stlTableCursor *)cur)->isEof;

}

// fill module's function pointers with particular implementations found 
// locally
void fill_module(sqlite3_module *m){
  m->iVersion = 1;
  m->xCreate = create_vtable;
  m->xConnect = connect_vtable;
  m->xBestIndex = bestindex_vtable;
  m->xDisconnect = disconnect_vtable;
  m->xDestroy = destroy_vtable;
  m->xOpen = open_vtable;
  m->xClose = close_vtable;
  m->xEof = eof_vtable;
  m->xFilter = filter_vtable;
  m->xNext = next_vtable;
  m->xColumn = column_vtable;
  m->xRowid = rowid_vtable;
  m->xUpdate = 0;
  m->xFindFunction = 0;
  m->xBegin = 0;
  m->xSync = 0;
  m->xCommit = 0;
  m->xRollback = 0;
  m->xRename = 0;
}

// estimate of the query by counting the length of the parameters passed
int arrange_size(int argc, const char * const * argv){
  int length = 28;          // length of standard keywords of sql queries
  int i;
  for(i=0; i<argc; i++){

    // +1 for comma or space
    if( i!=1 ) length+=strlen(argv[i]) +1;
  }
  // sentinel character
  length +=1;
  // printf("length is %i \n",length);
  return length;
}
