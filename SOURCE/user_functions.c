#include "stl_to_sql.h"
#include <stdlib.h>
#include <swill.h>
#include <time.h>

/*
#define DEBUGGING
*/

// Takes care of query preparation and execution.
int prep_exec(FILE *f, sqlite3 *db, const char *q){
  sqlite3_stmt  *stmt;
  int result, col, prepare;
  if( (prepare = sqlite3_prepare_v2(db, q, -1, &stmt, 0)) == SQLITE_OK ){
#ifdef DEBUGGING
    printf("Statement prepared.\n");  
#endif
    if (f != NULL) 
      result = file_prep_exec(f, stmt, q);
    else{
      result = sqlite3_step(stmt);
#ifdef DEBUGGING
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
	printf("Done\n");
      }else if( result==SQLITE_OK ){
	printf("OK\n");
      }else if( result==SQLITE_ERROR ){
	printf("SQL error or missing database\n");
      }else if( result==SQLITE_MISUSE ){
	printf("Library used incorrectly\n");
      }else {
	printf("Error code: %i.\nPlease advise Sqlite error codes (http://www.sqlite.org/c3ref/c_abort.html)", result);
      }
#endif
    }
  } else {
    printf("Error in preparation of query: error no %i\n", prepare);
    return prepare;
  }
  printf("\n");
  sqlite3_finalize(stmt);
  return result;
}

// Forwards  a query for execution to sqlite and 
// presents the resultset of a query.
int step_query(FILE *f, sqlite3_stmt *stmt) {
  int col, result;
  swill_fprintf(f, "<table>");
  swill_fprintf(f, "</tr>");
  for (col = 0; col < sqlite3_column_count(stmt); col++){
    swill_fprintf(f, "<td><b>%s</td></b>", sqlite3_column_name(stmt, col));
  }
  swill_fprintf(f, "</tr>");
  while ((result = sqlite3_step(stmt)) == SQLITE_ROW){
    swill_fprintf(f, "<tr>");
    for (col = 0; col < sqlite3_column_count(stmt); col++){
      switch(sqlite3_column_type(stmt, col)) {
      case 1: 
	swill_fprintf(f, "<td><b>%i</b></td>", sqlite3_column_int(stmt, col));
	break;
      case 2:
	swill_fprintf(f, "<td><b>%f</b></td>", sqlite3_column_double(stmt, col));
	break;
      case 3:
	swill_fprintf(f, "<td><b>%s</b></td>", sqlite3_column_text(stmt, col));
	break;
      case 4:
	swill_fprintf(f, "<td><b>%s</b></td>", (char *)sqlite3_column_blob(stmt, col));
	break;
      case 5:
	swill_fprintf(f, "<td><b>(null)</td></b>");
	break;
      }
    }
    swill_fprintf(f, "</tr>");
  }
    swill_fprintf(f,"</table>");
    swill_fprintf(f, "<br>");
    return result;
}

int file_prep_exec(FILE *f, sqlite3_stmt *stmt, const char *q){
  int result;
  result = step_query(f, stmt);
  if( result == SQLITE_DONE ){
#ifdef DEBUGGING
    swill_fprintf(f, "<b>DONE<br></b>");
#endif
  }else if( result == SQLITE_OK ){
#ifdef DEBUGGING
    swill_fprintf(f, "<b>OK<br></b>");
#endif
  }else if( result == SQLITE_ERROR ){
    swill_fprintf(f, "<b>SQL error or missing database.\n</b>");
  }else if( result == SQLITE_MISUSE ){
    swill_fprintf(f, "<b>Library used incorrectly.<br></b>");
  }else {
  }
  return result;
}


// Builds the front page of the library's web interface, 
// retrieves the database schema and promotes inputted queries 
// to sqlite_engine.
void app_index(FILE *f, sqlite3 *db){
  char *query="\0";
  swill_fprintf(f, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n\"http://www.w3.org/TR/html4/loose.dtd\">");
  swill_fprintf(f, "<html>");
  swill_fprintf(f, "<head>");
  swill_fprintf(f, "<style type=\"text/css\">");
  swill_fprintf(f, "img{height:116px;width:109px;align:left}");
  swill_fprintf(f, ".div_style{width:500px; border:2px solid; background-color:#ccc;margin:0px auto;margin-top:-40px;}");
  swill_fprintf(f, ".top{border-bottom:1px solid;padding-top:10px;padding-left:10px;padding-bottom:2px;}");
  swill_fprintf(f, ".middle{padding-top:5px;padding-left:9px; padding-bottom:5px;}");
  swill_fprintf(f, ".bottom{border-top:1px solid;padding-top:5px; padding-bottom:10px; text-align:center;}");
  swill_fprintf(f, ".button{height:7em; width:10em; font-size:22px;}");
  swill_fprintf(f, ".style_text{font-family:Times New Roman;font-size:20px;}");
  swill_fprintf(f, ".style_input{font-family:Times New Roman;font-size:15px;}");
  swill_fprintf(f, "table,td{border:1px double;}");
  swill_fprintf(f, ".div_tbl{margin-top:20px;}");
  swill_fprintf(f, "p.aligned{text-align:left;}");
  swill_fprintf(f, "</style>");
  swill_fprintf(f, "</head>");
  swill_fprintf(f, "<body>");
  swill_file("sqtl.png", NULL);
  swill_fprintf(f, "<img src=\"sqtl.png\" alt=\"SQTL logo\" />");
  swill_fprintf(f, "<div class=\"div_style\">");
  swill_fprintf(f, "<form action=\"serveQuery.html\" method=GET>");
  swill_fprintf(f, "<div class=\"top\">");
  swill_fprintf(f, "<span class=\"style_text\"><b>Input your SQL query:</b></span>");
  swill_fprintf(f, "</div>");
  swill_fprintf(f, "<div class=\"middle\">");
  swill_fprintf(f, "<textarea name=\"query\" cols=\"72\" rows=\"10\" class=\"style_input\"></textarea><br>");
  swill_fprintf(f, "</div>");
  swill_fprintf(f, "<div class=\"bottom\">");
  swill_fprintf(f, "<input type=\"submit\" value=\"Submit\" class=\"button\"></input>");
  swill_fprintf(f, "</div>");
  swill_fprintf(f, "</form>");
  swill_fprintf(f, "</div>");
  swill_fprintf(f, "<div class=\"div_tbl\">");
  swill_fprintf(f, "<span class=\"style_text\"><b>Your database schema is:</b></span>");
  prep_exec(f, db, "SELECT * FROM sqlite_master;");
  swill_fprintf(f, "</div>");
  swill_fprintf(f, "<br>");
  swill_fprintf(f, "<p class=\"aligned\">");
  swill_fprintf(f,"<a href=\"");
  swill_printurl(f,"terminateConnection.html", "", 0);
  swill_fprintf(f,"\">[ Terminate Server Connection ]</a>");
  swill_fprintf(f, "</p>");
  swill_fprintf(f, "</body>");
  swill_fprintf(f, "</html>");
}

// Builds the html page of the result set of a query 
// along with the time it took
// to execute and the query itself.
void serve_query(FILE *f, sqlite3 *db){
  const char *query="\0";
  swill_fprintf(f, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n\"http://www.w3.org/TR/html4/loose.dtd\">");
  swill_fprintf(f, "<html>");
  swill_fprintf(f, "<head>");
  swill_fprintf(f, "<style type=\"text/css\">");
  swill_fprintf(f, "body{bgcolor=\"#ffffff\";}");
  swill_fprintf(f, "span.styled{color:blue;}");
  swill_fprintf(f, "table, td{border:1px double;}");
  swill_fprintf(f, "p.aligned{text-align:left;}");
  swill_fprintf(f, "</style>");
  swill_fprintf(f, "</head>");
  swill_fprintf(f, "<body>");
  if( swill_getargs("s(query)", &query) ){
    int rc, i;
    clock_t start_clock,finish_clock;
    double c_time;
    start_clock = clock();
    int j=0;
    swill_fprintf(f, "<b>For SQL query: ");
    swill_fprintf(f, "<span class=\"styled\">%s</span><br><br>", query);
    swill_fprintf(f, "Result set is:</b><br><br>");
    // j for debugging, execute the query multiple times.
    while ( j<1 && (rc = prep_exec(f, db, query)) == SQLITE_DONE ){
      j++;
    }
    if (rc == SQLITE_DONE){
      finish_clock = clock();
      c_time = ((double)finish_clock - (double)start_clock)/CLOCKS_PER_SEC;
      swill_fprintf(f, "<b>\nQUERY SUCCESSFUL! </b><br><br>");
      swill_fprintf(f,"Ellapsed time given by C++ : <b>%f</b>s.<br><br>",c_time);
    } else {
      swill_fprintf(f, "<b>Error code %i.<br>Please advise </b><a href=\"", rc);
      swill_file("SQLite_error_codes.html", NULL);
      swill_printurl(f, "SQLite_error_codes.html", "", 0);
      swill_fprintf(f,"\">SQLite error codes</a>.<br><br>");
    }
    swill_fprintf(f, "<p class=\"aligned\">");
    swill_fprintf(f,"<a href=\"");
    swill_printurl(f,"index.html", "", 0);
    swill_fprintf(f,"\">[ Input new Query ]</a>");
    swill_fprintf(f,"<a href=\"");
    swill_printurl(f,"terminateConnection.html", "", 0);
    swill_fprintf(f,"\">[ Terminate Server Connection ]</a>");
    swill_fprintf(f, "</p>");
    swill_fprintf(f, "</body>");
    swill_fprintf(f, "</html>");
  }
}

// Terminates connection to the embedded web-server.
void terminate(FILE *f, sqlite3 *db){
  swill_fprintf(f, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n\"http://www.w3.org/TR/html4/loose.dtd\">");
  swill_fprintf(f, "<html>");
  swill_fprintf(f, "<head>");
  swill_fprintf(f, "<style type=\"text/css\">");
  swill_fprintf(f, "body{bgcolor=\"#ffffff\";}");
  swill_fprintf(f, "</style>");
  swill_fprintf(f, "</head>");
  swill_fprintf(f, "<body>");
  sqlite3_close(db);
  swill_fprintf(f, "<b>TERMINATED CONNECTION...</b>");
  swill_fprintf(f, "</body>");
  swill_fprintf(f, "</html>");
  swill_close();
}


void call_swill(sqlite3 *db){
  swill_init(8080);
  swill_handle("index.html", app_index, db);
  swill_handle("serveQuery.html", serve_query, db);
  swill_handle("terminateConnection.html", terminate, db);
  while( swill_serve() ){

  }
}

int register_table(const char *nDb, int argc, const char **q, const char **table_names, void *data){
  // This definition implicitly constraints a table name to 140 characters.
  // It should be more than enough.
  char table_query[200];
  int re, i=0;
  sqlite3 *db;
  re = sqlite3_open(nDb, &db);
  if( re ){
    printf("can't open database\n");
    sqlite3_close(db);
    return re;
  }

#ifdef DEBUGGING
  for(i=0; i<argc; i++){
    printf("\nquery to be executed: %s\n in database: %s\n\n", q[i], nDb);
  }
#endif
  sqlite3_module *mod;
  mod = (sqlite3_module *)sqlite3_malloc(sizeof(sqlite3_module));
  fill_module(mod);
  int output = sqlite3_create_module(db, "stl", mod, data);
  if( output==1 ) printf("Error while registering module\n");
#ifdef DEBUGGING
  else if( output==0 ) printf("Module registered successfully\n");
#endif
  for(i=0; i< argc; i++){
    sprintf(table_query, "SELECT * FROM sqlite_master WHERE type='table' AND name='%s';", table_names[i]);
    if (prep_exec(NULL, db, (const char *)table_query) != SQLITE_ROW)
      re = prep_exec(NULL, db, (const char *)q[i]);
  }
  printf("Please visit http://localhost:8080 to be served\n");
  call_swill(db);
  sqlite3_free(mod);
  return re;
}
