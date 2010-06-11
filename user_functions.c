#include "bridge.h"
#include <stdlib.h>
#include <swill.h>

// prepare and execute an sql query

int prep_exec(sqlite3 *db, char *q){
  // printf("in prep_exec...\n");

  sqlite3_stmt  *stmt;
  int result;

  printf("\n NOW PREPARING...\n");
  if( sqlite3_prepare_v2(db, q, -1, &stmt, 0)==SQLITE_OK ){
    printf("prepared ok (virtual)\n");
  }
  printf("\n NOW STEPPING... \n");
  result = sqlite3_step(stmt);
  if( result==SQLITE_DONE ){
    printf("perfecto!\n");
  }else if( result==SQLITE_OK ){
    printf("ok!\n");
  }else if( result==SQLITE_ERROR ){
    printf("error\n");
  }else if( result==SQLITE_MISUSE ){
    printf("inappropriate use\n");
  }else if( result==SQLITE_ROW ){
    printf("row of resultset available\n");
  }else printf("other\n");
  printf("\n");
  sqlite3_finalize(stmt);
  // printf("prep_exec finished exec query\n");
  return result;
}

// register the module with an open database connection, prepare and execute

// create table query.


void querySqlite(FILE *f){

  char *query="\0";

  fprintf(f, "<HTML><BODY BGCOLOR=\"#ffffff\">\n");

  fprintf(f,"<p><form action=\"serveQuery.html\" method=GET>\n");
  fprintf(f,"Query : <input type=text name=query width=10 value=\"%s\"></input><br>\n", query);
  fprintf(f,"<input type=submit value=\"Submit\"></input>\n");
  fprintf(f,"</form>\n");
  fprintf(f,"</body></html>\n");

  fprintf(f, "<b>SAMPLE QUERIES:\n\n</b><br><br>");
  fprintf(f, "<b>SELECT * FROM account;\n\n</b><br>");
  fprintf(f, "<b>SELECT * FROM account WHERE account_no='10068';\n\n</b><br>");


}

void serveQuery(FILE *f, sqlite3 *db){

  char *query="\0";

  fprintf(f, "<HTML><BODY BGCOLOR=\"#ffffff\">\n");

  if( swill_getargs("s(query)", &query) ){

    char ***pazResult = (char ***)sqlite3_malloc(sizeof(char***));
    int *nRows = (int *)sqlite3_malloc(sizeof(int));
    int *nCols = (int *)sqlite3_malloc(sizeof(int));
    char **errMsg = (char **)sqlite3_malloc(sizeof(char**));
  
    int rc, i;
    if( (rc = sqlite3_get_table(db, query, pazResult, nRows, nCols, errMsg))
	==SQLITE_OK){
      //      fprintf(f, "\nquery is: %s\n", query);
      
      fprintf(f, "<b>\nQUERY SUCCESSFUL! \n\n</b><br><br><br>");
      fprintf(f, "<b>result table:\n\n</b><br><br>");
      for(i=0; i<(*nRows +1) * (*nCols); i++){
	fprintf(f, "<b>%s </b>", pazResult[0][i]);
	if( (i+(*nCols)+1) % (*nCols)==0 ) fprintf(f, "<b>\n</b><br>");
      }
      fprintf(f, "<b>\n\n</b><br>");
    }else fprintf(f, "<b>nope\n</b><br>");

    fprintf(f,"<a href=\"");
    swill_printurl(f,"querySqlite.html", "", 0);
    fprintf(f,"\">[ Input new Query ]</a>");
    
    fprintf(f,"<a href=\"");
    swill_printurl(f,"terminateConnection.html", "", 0);
    fprintf(f,"\">[ Terminate Server Connection ]</a>");

    sqlite3_free(pazResult);
    sqlite3_free(nRows);
    sqlite3_free(nCols);
    sqlite3_free(errMsg);
  }

}

void terminate(FILE *f, sqlite3 *db){
  fprintf(f, "<b>TERMINATING CONNECTION...</b>");
  sqlite3_close(db);
  swill_close();
}
  

void index(FILE *f){
  fprintf(f,"<a href=\"");
  swill_printurl(f,"querySqlite.html", "", 0);
  fprintf(f,"\">[ Input Query ]</a>");

}


void call_swill(sqlite3 *db){
  swill_init(8080);
  swill_handle("querySqlite.html", querySqlite, 0);
  swill_handle("serveQuery.html", serveQuery, db);
  swill_handle("terminateConnection.html", terminate, db);
  swill_handle("index.html", index, 0);
  while( swill_serve() ){

  }
}

int register_table(char *nDb, char *nModule, char *q, void *data, int create){

  int re;
  sqlite3 *db;
  re = sqlite3_open(nDb, &db);
  if( re ){
    printf("can't open database\n");
    sqlite3_close(db);
    exit(1);
  }

  printf("\nquery to be executed: %s\n in database: %s\n\n", q, nDb);

  sqlite3_module mod;
  fill_module(&mod);

  int output = sqlite3_create_module(db, nModule, &mod, data);
  if( output==1 ) printf("Error while registering module\n");
  else if( output==0 ) printf("Module registered successfully\n");


  if( create ) re = prep_exec(db,q);
  printf("Please visit http://localhost:8080 to be served\n");
  call_swill(db);
  return re;
}
