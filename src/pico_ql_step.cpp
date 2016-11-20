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
#include <string>
#include <sstream>
using namespace std;

#include "pico_ql.h"
#include "sqlite3.h"

namespace picoQL {

/* Takes care of query preparation and execution. 
 * Writes results to file.
 */
int pico_ql_step_text(sqlite3 *db, sqlite3_stmt *stmt, stringstream &s) {
  int result, col;
  for (col = 0; col < sqlite3_column_count(stmt); col++)
    s << sqlite3_column_name(stmt, col) << " ";
  s << "\n";
  while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
    s << "\n";
    for (col = 0; col < sqlite3_column_count(stmt); col++) {
      switch (sqlite3_column_type(stmt, col)) {
      case 1:
	s << sqlite3_column_int(stmt, col) << " ";
	break;
      case 2:
	s << sqlite3_column_double(stmt, col) << " ";
	break;
      case 3:
	s << sqlite3_column_text(stmt, col) << " ";
	break;
      case 4:
	s << (char *)sqlite3_column_blob(stmt, col) << " ";
	break;
      case 5:
	s << "(null) ";
	break;
      }
    }
  }
  switch (result) {
  case SQLITE_DONE:
    s << "\n\nDone\n";
    break;
  case SQLITE_OK:
    s << "\n\nOK\n";
    break;
  case SQLITE_ERROR:
    s << "\n\nSQL error or missing database\n";
    s << "\nExtended error message: \n<b>" << sqlite3_errmsg(db) << "%s</b>\n\n";
    s << "Extended error code <b>" << sqlite3_extended_errcode(db) <<
	    "\nPlease advise SQLite error codes in pico_ql_error_page.html";
    break;
  case SQLITE_MISUSE:
    s << "\n\nLibrary used incorrectly\n";
    break;
  default:
    s << "\n\nError code: " << result << "\nPlease advise Sqlite error codes (http://www.sqlite.org/c3ref/c_abort.html)";
  }
  s << "\n";
  return result;
}

#ifdef PICO_QL_SWILL
/* Forwards  a query for execution to sqlite and 
 * presents the resultset of a query.
 */
int pico_ql_step_swill_html(sqlite3 *db, sqlite3_stmt *stmt, stringstream &s) {
  int col, result, rows = 0;
  s << "<table>";
  s << "</tr>";
  for (col = 0; col < sqlite3_column_count(stmt); col++) {
    s << "<td><b>" << sqlite3_column_name(stmt, col) << "</td></b>";
  }
  s << "</tr>";
  while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
    rows++;
    s << "<tr>";
    for (col = 0; col < sqlite3_column_count(stmt); col++) {
      switch (sqlite3_column_type(stmt, col)) {
      case 1:
	s << "<td><b>" << (long)sqlite3_column_int64(stmt, col) << "</b></td>";
	break;
      case 2:
	s << "<td><b>" << sqlite3_column_double << "</b></td>";
	break;
      case 3:
	s << "<td><b>" << sqlite3_column_text(stmt, col) << "</b></td>";
	break;
      case 4:
	s << "<td><b>" << (char *)sqlite3_column_blob << "</b></td>";
	break;
      case 5:
	s << "<td><b>(null)</td></b>";
	break;
      }
    }
    s << "</tr>";
  }
  s << "</table>";
  s << "<br>";
  s << "<b>" << rows << " rows in result set.</b><br>";
  s << "<br>";
  switch (result) {
  case SQLITE_DONE:
    s << "<b>Done<br></b>";
    break;
  case SQLITE_OK:
    s << "<b>OK<br></b>";
    break;
  case SQLITE_ERROR:
    s << "<b>SQL error or missing database\n</b>";
    s << "<br><b>Extended error message:<br><b>" << sqlite3_errmsg(db) << "</b><br><br>";
    s << "Extended error code <b>" << sqlite3_extended_errcode(db) << "<br>Please advise </b><a href=\"";
    s << "pico_ql_error_page.html";
    s << "\">SQLite error codes</a>.<br><br>";
    break;
  case SQLITE_MISUSE:
    s << "<b>Library used incorrectly<br></b>";
    break;
  default:
    s << "<b>Error code: " << result << "\nPlease advise Sqlite error codes (http://www.sqlite.org/c3ref/c_abort.html)<br></b>";
  }
  s << "<br>";
  return result;
}

/* Forwards  a query for execution to sqlite and 
 * presents the resultset of a query.
 */
int pico_ql_step_swill_json(sqlite3 *db, sqlite3_stmt *stmt, stringstream &s) {
  int col, result, rows = 0;
  s << "[";
  while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
    rows++;
    if (rows == 1)
      s << " {";
    else
      s << ", {";
    for (col = 0; col < sqlite3_column_count(stmt); col++) {
      if (col != 0)
        s << ",";
      switch (sqlite3_column_type(stmt, col)) {
      case 1:
	s << " \"" << sqlite3_column_name(stmt, col) << "\" : " <<
		        (long)sqlite3_column_int64(stmt, col);
	break;
      case 2:
	s << " \"" << sqlite3_column_name(stmt, col) << "\" : " <<
		        sqlite3_column_double(stmt, col);
	break;
      case 3:
	s << " \"" << sqlite3_column_name(stmt, col) << "\" : \"" << sqlite3_column_text(stmt, col) << "\"";
	break;
      case 4:
	s << " \"" << sqlite3_column_name(stmt, col) << "\" : \"" << (char *)sqlite3_column_blob(stmt, col) << "\"";
	break;
      case 5:
	s << " \"" << sqlite3_column_name(stmt, col) << "\" : \"(null)\"";
	break;
      }
    }
    s << " }";
  }
  s << " ]";
  switch (result) {
  case SQLITE_DONE:
    s << "Done\n";
    break;
  case SQLITE_OK:
    s << "OK\n";
    break;
  case SQLITE_ERROR:
    s << "SQL error or missing database\n";
    s << "Extended error message:\n" << sqlite3_errmsg(db) << "</b>\n\n";
    s << "Extended error code " << sqlite3_extended_errcode(db) << ". Please advise <a href=\"";
    s << "pico_ql_error_page.html";
    s << "\">SQLite error codes</a>.\n\n";
    break;
  case SQLITE_MISUSE:
    s << "Library used incorrectly\n";
    break;
  default:
    s << "Error code: " << result << ".\nPlease advise Sqlite error codes (http://www.sqlite.org/c3ref/c_abort.html)";
  }
  s << "\n";
  return result;
}
#endif

/* No output expected, e.g. CREATE query */
int pico_ql_step_mute(sqlite3 *db, sqlite3_stmt *stmt, stringstream &s) {
  (void)db;
  (void)s;
  return sqlite3_step(stmt);
}

} // namespace picoQL
