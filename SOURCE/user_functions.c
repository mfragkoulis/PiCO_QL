#include "bridge.h"
#include <stdlib.h>
#include <swill.h>
#include <time.h>



// prepare and execute an sql query

int prep_exec(FILE *f, sqlite3 *db, const char *q){
  // printf("in prep_exec...\n");

  sqlite3_stmt  *stmt;
  int result, col;

  printf("\n NOW PREPARING...\n");
  if( sqlite3_prepare_v2(db, q, -1, &stmt, 0)==SQLITE_OK ){
    printf("prepared ok (virtual)\n");  
    printf("\n NOW STEPPING... \n");
    if (f != NULL) 
      result = alias_prep_exec(f, stmt);
    else{
      for (col = 0; col < sqlite3_column_count(stmt); col++){
	printf("%s ", sqlite3_column_name(stmt, col));
      }
      printf("\n");
      while ((result = sqlite3_step(stmt)) == SQLITE_ROW){
	printf("\n");
	for (col = 0; col < sqlite3_column_count(stmt); col++){
	  switch(sqlite3_column_type(stmt, col)) {
	  case 1: 
	    printf("%i ", sqlite3_column_int(stmt, col));
	    break;
	  case 2:
	    printf("%f ", sqlite3_column_double(stmt, col));
	    break;
	  case 3:
	    printf("%s ", sqlite3_column_text(stmt, col));
	    break;
	  case 4:
	    printf("%s ", (char *)sqlite3_column_blob(stmt, col));
	    break;
	  case 5:
	    printf("(null) ");
	    break;
	  }
	}
      }
      if( result==SQLITE_DONE ){
	//	printf("perfecto!\n");
      }else if( result==SQLITE_OK ){
	//	printf("ok!\n");
      }else if( result==SQLITE_ERROR ){
	printf("error\n");
	//      sqlite3_finalize(stmt);
	//    exit(1);
      }else if( result==SQLITE_MISUSE ){
	printf("inappropriate use\n");
	//      sqlite3_finalize(stmt);
	//    exit(1);
      }else {
	printf("other\n");
	//      sqlite3_finalize(stmt);
	//    exit(1);
      }
    }
  }else 
    printf("Error in preparation of query\n");
  printf("\n");
  sqlite3_finalize(stmt);
  // printf("prep_exec finished exec query\n");
  return result;
}


int alias_prep_exec(FILE *f, sqlite3_stmt * stmt){
  // printf("in prep_exec...\n");

  int result, col;

  //    printf("row of resultset available\n");
  fprintf(f, "<b>result table:\n\n</b><br><br>");
  fprintf(f, "<table border=\"1\">");
  fprintf(f, "</tr>");
  
  for (col = 0; col < sqlite3_column_count(stmt); col++){
    fprintf(f, "<td><b>%s</td></b>", sqlite3_column_name(stmt, col));
  }
  fprintf(f, "</tr>");
  while ((result = sqlite3_step(stmt)) == SQLITE_ROW){
    fprintf(f, "<tr>");
    for (col = 0; col < sqlite3_column_count(stmt); col++){
      switch(sqlite3_column_type(stmt, col)) {
      case 1: 
	fprintf(f, "<td><b>%i</b></td>", sqlite3_column_int(stmt, col));
	break;
      case 2:
	fprintf(f, "<td><b>%f</b></td>", sqlite3_column_double(stmt, col));
	break;
      case 3:
	fprintf(f, "<td><b>%s</b></td>", sqlite3_column_text(stmt, col));
	break;
      case 4:
	fprintf(f, "<td><b>%s</b></td>", (char *)sqlite3_column_blob(stmt, col));
	break;
      case 5:
	fprintf(f, "<td><b>(null)</td></b>");
	break;
      }
    }
    fprintf(f, "</tr>");
  }
    fprintf(f,"</table>");
    fprintf(f, "<br>");
  if( result==SQLITE_DONE ){
    //    fprintf(f, "<b>perfecto!<br></b>");
  }else if( result==SQLITE_OK ){
    //    fprintf(f, "<b>ok!<br></b>");
  }else if( result==SQLITE_ERROR ){
    fprintf(f, "<b>error\n</b>");
    sqlite3_finalize(stmt);
    //    exit(1);
  }else if( result==SQLITE_MISUSE ){
    fprintf(f, "<b>inappropriate use<br></b>");
    sqlite3_finalize(stmt);
    //    exit(1); fix create
  }else {
    fprintf(f, "<b>other<br></b>");
    sqlite3_finalize(stmt);
    //    exit(1);
  }
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

  const char *query="\0";

  fprintf(f, "<HTML><BODY BGCOLOR=\"#ffffff\">\n");

  if( swill_getargs("s(query)", &query) ){
  
    int rc, i;
    clock_t start_clock,finish_clock;
    double c_time;
    start_clock = clock();
    // which of the two?                                                                               
    //    Timer t;
    //    t.start();
    int j=0;
    while ( j<1 && (rc = prep_exec(f, db, query)) == SQLITE_DONE){
      //      t.stop();
      //      printf("%i \n",j);
      j++;
    }
    if (rc==SQLITE_DONE){
      finish_clock = clock();
      c_time = ((double)finish_clock - (double)start_clock)/CLOCKS_PER_SEC;
      fprintf(f, "<b>\nQUERY SUCCESSFUL! \n\n</b><br><br><br>");
      //      fprintf(f,"\nQuery execution took <b>%f</b> seconds.\n\n",t);
      fprintf(f,"Ellapsed time given by c++ : <b>%f</b>s.<br><br>",c_time);
    }

    fprintf(f,"<br><a href=\"");
    swill_printurl(f,"querySqlite.html", "", 0);
    fprintf(f,"\">[ Input new Query ]</a>");
    
    fprintf(f,"<a href=\"");
    swill_printurl(f,"terminateConnection.html", "", 0);
    fprintf(f,"\">[ Terminate Server Connection ]</a>");

  }

}

void terminate(FILE *f, sqlite3 *db){
  fprintf(f, "<b>TERMINATING CONNECTION...</b>");
  sqlite3_close(db);
  swill_close();
}
  

void app_index(FILE *f){
  fprintf(f,"<a href=\"");
  swill_printurl(f,"querySqlite.html", "", 0);
  fprintf(f,"\">[ Input Query ]</a>");

}


void call_swill(sqlite3 *db){
  swill_init(8080);
  swill_handle("querySqlite.html", querySqlite, 0);
  swill_handle("serveQuery.html", serveQuery, db);
  swill_handle("terminateConnection.html", terminate, db);
  swill_handle("index.html", app_index, 0);
  while( swill_serve() ){

  }
}

int register_table(const char *nDb, int argc, const char **q, void *data, int create){

  int re, i=0;
  sqlite3 *db;
  re = sqlite3_open(nDb, &db);
  if( re ){
    printf("can't open database\n");
    sqlite3_close(db);
    exit(1);
  }

  for(i=0; i<argc; i++){
    printf("\nquery to be executed: %s\n in database: %s\n\n", q[i], nDb);
  }

  sqlite3_module *mod;
  mod = (sqlite3_module *)sqlite3_malloc(sizeof(sqlite3_module));
  fill_module(mod);

  int output = sqlite3_create_module(db, "stl", mod, data);
  if( output==1 ) printf("Error while registering module\n");
  else if( output==0 ) printf("Module registered successfully\n");


  if( create ){
    for(i=0; i< argc; i++){
      re = prep_exec(NULL, db, (const char *)q[i]);
    }
  }
  printf("Please visit http://localhost:8080 to be served\n");
  call_swill(db);

  //    re = prep_exec(NULL, db, "select * from Trucks,Customers as c,Customers as u where Trucks.Customers_id=c.pk and Trucks.Customers_id=u.pk and u.demand=c.demand order by c.code;");
      sqlite3_free(mod);
      return re;
}
