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
#ifdef PICO_QL_SWILL
#include <swill.h>
#endif
#include <string.h>

#include "pico_ql.h"

/* Takes care of query preparation and execution. 
 * Writes results to file.
 */
int pico_ql_step_text(sqlite3 *db, sqlite3_stmt *stmt, FILE *f) {
  int result, col;
  for (col = 0; col < sqlite3_column_count(stmt); col++)
    fprintf(f, "%s ", sqlite3_column_name(stmt, col));
  fprintf(f, "\n");
  while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
    fprintf(f, "\n");
    for (col = 0; col < sqlite3_column_count(stmt); col++) {
      switch (sqlite3_column_type(stmt, col)) {
      case 1:
	fprintf(f, "%i ", sqlite3_column_int(stmt, col));
	break;
      case 2:
	fprintf(f, "%f ", sqlite3_column_double(stmt, col));
	break;
      case 3:
	fprintf(f, "%s ", sqlite3_column_text(stmt, col));
	break;
      case 4:
	fprintf(f, "%s ", (char *)sqlite3_column_blob(stmt, col));
	break;
      case 5:
	fprintf(f, "(null) ");
	break;
      }
    }
  }
  switch (result) {
  case SQLITE_DONE:
    fprintf(f, "\n\nDone\n");
    break;
  case SQLITE_OK:
    fprintf(f, "\n\nOK\n");
    break;
  case SQLITE_ERROR:
    fprintf(f, "\n\nSQL error or missing database\n");
    fprintf(f, "\nExtended error message: \n<b>%s</b>\n\n",
		    sqlite3_errmsg(db));
    fprintf(f, "Extended error code <b>%i.\nPlease advise SQLite error codes in pico_ql_error_page.html",
		    sqlite3_extended_errcode(db));
    break;
  case SQLITE_MISUSE:
    fprintf(f, "\n\nLibrary used incorrectly\n");
    break;
  default:
    fprintf(f, "\n\nError code: %i.\nPlease advise Sqlite error codes (http://www.sqlite.org/c3ref/c_abort.html)", result);
  }
  fprintf(f, "\n");
  return result;
}

#ifdef PICO_QL_SWILL
/* Forwards  a query for execution to sqlite and 
 * presents the resultset of a query.
 */
int pico_ql_step_swill_html(sqlite3 *db, sqlite3_stmt *stmt, FILE *f) {
  int col, result, rows = 0;
  swill_fprintf(f, "<table>");
  swill_fprintf(f, "</tr>");
  for (col = 0; col < sqlite3_column_count(stmt); col++) {
    swill_fprintf(f, "<td><b>%s</td></b>",
		    sqlite3_column_name(stmt, col));
  }
  swill_fprintf(f, "</tr>");
  while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
    rows++;
    swill_fprintf(f, "<tr>");
    for (col = 0; col < sqlite3_column_count(stmt); col++) {
      switch (sqlite3_column_type(stmt, col)) {
      case 1:
	swill_fprintf(f, "<td><b>%li</b></td>",
		        (long)sqlite3_column_int64(stmt, col));
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
  swill_fprintf(f, "<b>%i rows in result set.</b><br>", rows);
  swill_fprintf(f, "<br>");
  switch (result) {
  case SQLITE_DONE:
    swill_fprintf(f, "<b>Done<br></b>");
    break;
  case SQLITE_OK:
    swill_fprintf(f, "<b>OK<br></b>");
    break;
  case SQLITE_ERROR:
    swill_fprintf(f, "<b>SQL error or missing database\n</b>");
    swill_fprintf(f, "<br><b>Extended error message:<br><b>%s</b><br><br>",
		    sqlite3_errmsg(db));
    swill_fprintf(f, "Extended error code <b>%i.<br>Please advise </b><a href=\"",
		    sqlite3_extended_errcode(db));
    swill_printurl(f, "pico_ql_error_page.html", "", 0);
    swill_fprintf(f,"\">SQLite error codes</a>.<br><br>");
    break;
  case SQLITE_MISUSE:
    fprintf(f, "<b>Library used incorrectly<br></b>");
    break;
  default:
    fprintf(f, "<b>Error code: %i.\nPlease advise Sqlite error codes (http://www.sqlite.org/c3ref/c_abort.html)<br></b>", result);
  }
  fprintf(f, "<br>");
  return result;
}

/* Forwards  a query for execution to sqlite and 
 * presents the resultset of a query.
 */
int pico_ql_step_swill_json(sqlite3 *db, sqlite3_stmt *stmt, FILE *f) {
  int col, result, rows = 0;
  swill_fprintf(f, "[");
  while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
    rows++;
    if (rows == 1)
      swill_fprintf(f, " {");
    else
      swill_fprintf(f, ", {");
    for (col = 0; col < sqlite3_column_count(stmt); col++) {
      if (col != 0)
        swill_fprintf(f, ",");
      switch (sqlite3_column_type(stmt, col)) {
      case 1:
	swill_fprintf(f, " \"%s\" : %li",
                        sqlite3_column_name(stmt, col),
		        (long)sqlite3_column_int64(stmt, col));
	break;
      case 2:
	swill_fprintf(f, " \"%s\" : %f",
                        sqlite3_column_name(stmt, col),
		        sqlite3_column_double(stmt, col));
	break;
      case 3:
	swill_fprintf(f, " \"%s\" : \"%s\"",
                        sqlite3_column_name(stmt, col),
		        sqlite3_column_text(stmt, col));
	break;
      case 4:
	swill_fprintf(f, " \"%s\" : \"%s\"",
                        sqlite3_column_name(stmt, col),
		        (char *)sqlite3_column_blob(stmt, col));
	break;
      case 5:
	swill_fprintf(f, " \"%s\" : \"(null)\"",
                        sqlite3_column_name(stmt, col));
	break;
      }
    }
    swill_fprintf(f, " }");
  }
  swill_fprintf(f, " ]");
  switch (result) {
  case SQLITE_DONE:
    swill_fprintf(f, "Done\n");
    break;
  case SQLITE_OK:
    swill_fprintf(f, "OK\n");
    break;
  case SQLITE_ERROR:
    swill_fprintf(f, "SQL error or missing database\n");
    swill_fprintf(f, "Extended error message:\n%s</b>\n\n",
		    sqlite3_errmsg(db));
    swill_fprintf(f, "Extended error code %i. Please advise <a href=\"",
		    sqlite3_extended_errcode(db));
    swill_printurl(f, "pico_ql_error_page.html", "", 0);
    swill_fprintf(f,"\">SQLite error codes</a>.\n\n");
    break;
  case SQLITE_MISUSE:
    swill_fprintf(f, "Library used incorrectly\n");
    break;
  default:
    swill_fprintf(f, "Error code: %i.\nPlease advise Sqlite error codes (http://www.sqlite.org/c3ref/c_abort.html)", result);
  }
  swill_fprintf(f, "\n");
  return result;
}
#endif

/* No output expected, e.g. CREATE query */
int pico_ql_step_mute(sqlite3 *db, sqlite3_stmt *stmt, FILE *f) {
  (void)db;
  (void)f;
  return sqlite3_step(stmt);
}
