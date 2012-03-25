/*
 *   Setup the user interface (html pages that the local 
 *   server (SWILL) will serve.
 *   Manage database connections, pass queries to SQLite 
 *   and format resultset for appropriate presentation.
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

#include <stdlib.h>
#include <time.h>
#include <swill.h>
#include "user_functions.h"
#include "stl_test.h"

// Takes care of query preparation and execution.
int prep_exec(FILE *f, sqlite3 *db, const char *q){
  sqlite3_stmt  *stmt;
  int result, prepare;
  if ((prepare = sqlite3_prepare_v2(db, q, -1, &stmt, 0)) == SQLITE_OK) {
    if (f) {
      result = file_prep_exec(f, stmt, q);
      fprintf(f, "\n");
    } else
      /* Step only: queries with no resultset (check if 
       * table exists). For those queries preparation 
       * will always succeed.
       */ 
      result = sqlite3_step(stmt);
  } else {
    if (f) swill_fprintf(f, "Error in preparation of query: error no %i\n", prepare);
    return prepare;
  }
  sqlite3_finalize(stmt);
  return result;
}

/* Forwards  a query for execution to sqlite and 
 * presents the resultset of a query.
 */
int step_query(FILE *f, sqlite3_stmt *stmt) {
  int col, result;
  swill_fprintf(f, "<table>");
  swill_fprintf(f, "</tr>");
  for (col = 0; col < sqlite3_column_count(stmt); col++) {
    swill_fprintf(f, "<td><b>%s</td></b>", 
		  sqlite3_column_name(stmt, col));
  }
  swill_fprintf(f, "</tr>");
  while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
    swill_fprintf(f, "<tr>");
    for (col = 0; col < sqlite3_column_count(stmt); col++) {
      switch (sqlite3_column_type(stmt, col)) {
      case 1:
	swill_fprintf(f, "<td><b>%i</b></td>", 
		      sqlite3_column_int(stmt, col));
	break;
      case 2:
	swill_fprintf(f, "<td><b>%f</b></td>", 
		      sqlite3_column_double(stmt, col));
	break;
      case 3:
	swill_fprintf(f, "<td><b>%s</b></td>", 
		      sqlite3_column_text(stmt, col));
	break;
      case 4:
	swill_fprintf(f, "<td><b>%s</b></td>", 
		      (char *)sqlite3_column_blob(stmt, 
						  col));
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

/* Calls step_query for query execution. 
 * Collects and acts on the result status of a query 
 * execution.
 */
int file_prep_exec(FILE *f, sqlite3_stmt *stmt, 
		   const char *q) {
  int result;
  result = step_query(f, stmt);
  switch (result) {
  case SQLITE_DONE:
#ifdef DEBUG
    swill_fprintf(f, "<b>DONE<br></b>");
#endif
    break;
  case SQLITE_OK:
#ifdef DEBUG
    swill_fprintf(f, "<b>OK<br></b>");
#endif
    break;
  case SQLITE_ERROR:
    swill_fprintf(f, "<b>SQL error or missing database.\n</b>");
    break;
  case SQLITE_MISUSE:
    swill_fprintf(f, "<b>Library used incorrectly.<br></b>");
    break;
  }
  return result;
}


/* Builds the front page of the library's web interface, 
 * retrieves the database schema and promotes inputted 
 * queries to sqlite_engine.
 */
void app_index(FILE *f, sqlite3 *db) {
  swill_fprintf(f, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n\"http://www.w3.org/TR/html4/loose.dtd\">"
		"<html>"
		"<head>"
		"<style type=\"text/css\">"
		"img{height:116px;width:109px;align:left}"
		".div_style{width:500px; border:2px solid; background-color:#ccc;margin:0px auto;margin-top:-40px;}"
		".top{border-bottom:1px solid;padding-top:10px;padding-left:10px;padding-bottom:2px;}"
		".middle{padding-top:5px;padding-left:9px; padding-bottom:5px;}"
		".bottom{border-top:1px solid;padding-top:5px; padding-bottom:10px; text-align:center;}"
		".button{height:7em; width:10em; font-size:22px;}"
		".style_text{font-family:Times New Roman;font-size:20px;}"
		".style_input{font-family:Times New Roman;font-size:15px;}"
		"table,td{border:1px double;}"
		".div_tbl{margin-top:20px;}"
		"p.aligned{text-align:left;}"
		"</style>"
		"</head>"
		"<body>");
  swill_file("sqtl.png", "../bin/sqtl.png");
  swill_fprintf(f, "<img src=\"sqtl.png\" alt=\"SQTL logo\" />"
		"<div class=\"div_style\">"
		"<form action=\"serveQuery.html\" method=GET>"
		"<div class=\"top\">"
		"<span class=\"style_text\"><b>Input your SQL query:</b></span>"
		"</div>"
		"<div class=\"middle\">"
		"<textarea name=\"query\" cols=\"72\" rows=\"10\" class=\"style_input\"></textarea><br>"
  "</div>"
		"<div class=\"bottom\">"
		"<input type=\"submit\" value=\"Submit\" class=\"button\"></input>"
		"</div>"
		"</form>"
		"</div>"
		"<div class=\"div_tbl\">"
		"<span class=\"style_text\"><b>Your database schema is:</b></span>");
  prep_exec(f, db, "SELECT * FROM sqlite_master;");
  swill_fprintf(f, "</div>"
		"<br>"
		"<p class=\"aligned\">");
  swill_fprintf(f,"<a href=\"");
  swill_printurl(f,"terminateConnection.html", "", 0);
  swill_fprintf(f,"\">[ Terminate Server Connection ]</a>"
		"</p>"
		"</body>"
		"</html>");
}


/* Builds the html page of the result set of a query 
 * along with the time it took to execute and the query 
 * itself.
 */
void serve_query(FILE *f, sqlite3 *db) {
  const char *query = "\0";
  swill_fprintf(f, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n\"http://www.w3.org/TR/html4/loose.dtd\">"
		"<html>"
		"<head>"
		"<style type=\"text/css\">"
		"body{bgcolor=\"#ffffff\";}"
		"span.styled{color:blue;}"
		"table, td{border:1px double;}"
		"p.aligned{text-align:left;}"
		"</style>"
		"</head>"
		"<body>");
  if (swill_getargs("s(query)", &query)) {
    int rc;
    clock_t start_clock,finish_clock;
    double c_time;
    start_clock = clock();
    int j = 0;
    swill_fprintf(f, "<b>For SQL query: ");
    swill_fprintf(f, "<span class=\"styled\">%s</span><br><br>", query);
    swill_fprintf(f, "Result set is:</b><br><br>");
    // j for debugging, execute the query multiple times.
    while (j < 1 && (rc = prep_exec(f, db, query)) == SQLITE_DONE) {
      j++;
    }
    if (rc == SQLITE_DONE) {
      finish_clock = clock();
      c_time = ((double)finish_clock - 
		(double)start_clock)/CLOCKS_PER_SEC;
      swill_fprintf(f, "<b>\nQUERY SUCCESSFUL! </b><br><br>");
      swill_fprintf(f,"Ellapsed time given by C++ : <b>%f</b>s.<br><br>", c_time);
    } else {
      swill_fprintf(f, "<b>Error code %i.<br>Please advise </b><a href=\"", rc);
      swill_file("SQLite_error_codes.html", 
		 "../bin/SQLite_error_codes.html");
      swill_printurl(f, "SQLite_error_codes.html", "", 0);
      swill_fprintf(f,"\">SQLite error codes</a>.<br><br>");
    }
    swill_fprintf(f, "<p class=\"aligned\">");
    swill_fprintf(f, "<a href=\"");
    swill_printurl(f,"index.html", "", 0);
    swill_fprintf(f,"\">[ Input new Query ]</a>");
    swill_fprintf(f, "<a href=\"");
    swill_printurl(f,"terminateConnection.html", "", 0);
    swill_fprintf(f,"\">[ Terminate Server Connection ]</a>"
		  "</p>"
		  "</body>"
		  "</html>");
  }
}

// Terminates connection to the embedded web-server.
void terminate(FILE *f, sqlite3 *db) {
  swill_fprintf(f, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n\"http://www.w3.org/TR/html4/loose.dtd\">"
		"<html>"
		"<head>"
		"<style type=\"text/css\">"
		"body{bgcolor=\"#ffffff\";}"
		"</style>"
		"</head>"
		"<body>");
  sqlite3_close(db);
  swill_fprintf(f, "<b>TERMINATED CONNECTION...</b>"
		"</body>"
		"</html>");
  swill_close();
}

// Interface to the swill server functionality.
void call_swill(sqlite3 *db) {
  swill_init(8080);
  swill_handle("index.html", app_index, db);
  swill_handle("serveQuery.html", serve_query, db);
  swill_handle("terminateConnection.html", terminate, db);
  while (swill_serve()) {

  }
}

/* Executes the SQL CREATE queries, opens the sqlite 
 * database connection and calls swill or stl_test 
 * depending on the compile flag TEST.
 */
int register_table(const char *nDb, 
		   int argc, 
		   const char **q, 
		   const char **table_names, 
		   void *data) {
  /* This definition implicitly constraints a table name 
   * to 140 characters. It should be more than enough.
   */
  char table_query[200];
  int re, i=0;
  sqlite3 *db;
  re = sqlite3_open(nDb, &db);
  if (re) {
    printf("can't open database\n");
    sqlite3_close(db);
    return re;
  }

#ifdef DEBUG
  for (i = 0; i < argc; i++) {
    printf("\nquery to be executed: %s\n in database: %s\n\n", q[i], nDb);
  }
#endif
  sqlite3_module *mod;
  mod = (sqlite3_module *)sqlite3_malloc(sizeof(sqlite3_module));
  fill_module(mod);
  int output = sqlite3_create_module(db, "stl", mod, data);
  if (output == 1) 
    printf("Error while registering module\n");
#ifdef DEBUG
  else if (output == 0) 
    printf("Module registered successfully\n");
#endif
  for (i = 0; i < argc; i++) {
    sprintf(table_query, "SELECT * FROM sqlite_master WHERE type='table' AND name='%s';", table_names[i]);
    if (prep_exec(NULL, db, (const char *)table_query) != SQLITE_ROW) {
      re = prep_exec(NULL, db, (const char *)q[i]);
#ifdef DEBUG
      printf("Query %s returned %i\n", q[i], re);
#endif
      if (re != 101) return re;
    }
  }
#ifndef TEST
  printf("Please visit http://localhost:8080 to be served\n");
  call_swill(db);
#else
  re = call_test(db);
#endif
  sqlite3_free(mod);
  return re;
}
