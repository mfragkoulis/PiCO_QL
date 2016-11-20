/*
 *   Setup the user interface (html pages that the local 
 *   server (SWILL) will serve.
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

#include <cstdlib>
#include <ctime>
#include <swill.h>
#include <cstring>
#include <unistd.h>
#include <sstream>
using namespace std;

#include "pico_ql_swill.h"
#include "pico_ql.h"
#include "pico_ql_db.h"
#include "pico_ql_swill_access_func.h"

namespace picoQL {

/* Calls the function that prints the PiCO QL error page (.html).
 */
void print_pico_ql_error_page(FILE *f) {
  error_page(f);
}

/* Calls the function that prints the PiCO QL logo (.PNG).
 */
void print_pico_ql_logo(FILE *f) {
  logo(f);
}

/* Builds the front page of the library's web interface, 
 * retrieves the database schema and promotes inputted 
 * queries to sqlite_engine.
 */
void app_index(FILE *f) {
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
  swill_fprintf(f, "<p><left><img src=\"pico_ql_logo.png\"></left>\n"
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
  stringstream s;
  pico_ql_exec_query("SELECT * FROM sqlite_master;", s, pico_ql_step_swill_html);
  swill_fprintf(f, "%s", s.str().c_str());
  s.str("");
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
void serve_query(FILE *f) {
  const char *query = "\0";
  char response_type[50];
  char *rt = swill_getheader("Http_Choose_Response_Type");
  if (rt)
    strcpy(response_type, rt);
  else
    strcpy(response_type, "text/html");   /* default */
  //swill_fprintf(f, "Response type should be: %s", response_type);
  if (!strcmp(response_type, "text/html"))
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
    int rc = 0;
    clock_t start_clock,finish_clock;
    double c_time;
    stringstream s;
    start_clock = clock();
    if (!strcmp(response_type, "text/html")) {
      swill_fprintf(f, "<b>For SQL query: ");
      swill_fprintf(f, "<span class=\"styled\">%s</span><br><br>", query);
      swill_fprintf(f, "Result set is:</b><br><br>");
    }
    rc = pico_ql_exec_query(query, s, pico_ql_step_swill_html);
    swill_fprintf(f, "%s", s.str().c_str());
    s.str("");
    if (rc == SQLITE_DONE) {
      finish_clock = clock();
      c_time = ((double)finish_clock - 
		(double)start_clock)/CLOCKS_PER_SEC;
      if (!strcmp(response_type, "text/html")) {
        swill_fprintf(f, "<b>\nQUERY SUCCESSFUL! </b><br><br>");
        swill_fprintf(f,"CPU time: <b>%f</b>s.<br><br>", c_time);
      }
    } 
    if (!strcmp(response_type, "text/html")) {
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
  //clear_temp_structs();
}

// Terminates connection to the embedded web-server.
void terminate(FILE *f) {
  char response_type[50];
  char *rt = swill_getheader("Http_Choose_Response_Type");
  if (rt)
    strcpy(response_type, rt);
  else
    strcpy(response_type, "text/html");   /* default */
  if (!strcmp(response_type, "text/html")) {
    swill_fprintf(f, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n\"http://www.w3.org/TR/html4/loose.dtd\">"
		"<html>"
		"<head>"
		"<style type=\"text/css\">"
		"body{bgcolor=\"#ffffff\";}"
		"</style>"
		"</head>"
		"<body>");
  }
  pico_ql_shutdown();
  if (!strcmp(response_type, "text/html")) {
    swill_fprintf(f, "<b>TERMINATED CONNECTION...</b>"
		"</body>"
		"</html>");
  }
  swill_close();
}

// Interface to the swill server functionality.
void init_pico_ql_swill(int port_number) {
  swill_init(port_number);
  swill_handle("pico_ql_logo.png", print_pico_ql_logo, 0);
  swill_handle("pico_ql_error_page.html", print_pico_ql_error_page, 0);
  swill_handle("index.html", app_index, 0);
  swill_handle("serveQuery.html", serve_query, 0);
  swill_handle("terminateConnection.html", terminate, 0);
  while (swill_serve()) {

  }
}

} // namespace picoQL
