/*
 *   Setup the user interface (html pages that the local 
 *   server (SWILL) will serve).
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
#include <stdio.h>
#include <time.h>
#include <swill.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "pico_ql_swill_access_func.h"


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
 * retrieves the database schema and promotes input 
 * queries to sqlite_engine.
 */
void app_index(FILE *f) {
  printf("Trying to begin service.\n");
  swill_fprintf(f, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n\"http://www.w3.org/TR/html4/loose.dtd\">"
		"<html>"
		"<head>"
		"<style type=\"text/css\">"
		"img{height:110px;width:100px;align:left}"
		".div_style{width:500px; border:2px solid; background-color:#ccc;margin:0px auto;margin-top:-40px;}"
		".top{border-bottom:1px solid;padding-top:10px;padding-left:10px;padding-bottom:2px;}"
		".middle{padding-top:5px;padding-left:9px; padding-bottom:5px;}"
		".bottom{border-top:1px solid;padding-top:5px; padding-bottom:10px; text-align:center;}"
		".button{height:2em; width:9em; font-size:18px;}"
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
		"<textarea name=\"query\" cols=\"50\" rows=\"10\" class=\"style_input\"></textarea><br>"
  "</div>"
		"<div class=\"bottom\">"
		"<input type=\"submit\" value=\"Submit\" class=\"button\"></input>"
		"</div>"
		"</form>"
		"</div>"
		"<div class=\"div_tbl\">"
		"<span class=\"style_text\"><b>Your database schema is:</b></span>");
  int picoQL_descr;
  int re = 0;
  picoQL_descr = open("/proc/picoQL", 0);
  if (picoQL_descr < 0) {
    printf("Can't open /proc/picoQL.\n");
    exit(1);
  }
  printf("Opened /proc/picoQL with file descriptor %i.\n", picoQL_descr);
  re = ioctl(picoQL_descr, 111, 0); // Activate metadata
  printf("ioctl for activating metadata returned %i.\n", re);
  re = ioctl(picoQL_descr, 11, 0); // Activate HTML output.
  printf("ioctl for activating HTML output returned %i.\n", re);
  close(picoQL_descr);
  printf("Closed file descriptor %i.\n", picoQL_descr);
  printf("Going to write to the procFile.\n");
  re = system("echo \"SELECT * FROM sqlite_master;\" > /proc/picoQL");
  printf("Just wrote to /proc/picoQL.\n");
  if (re)
    printf("echo to /proc/picoQL failed.\n");
  sleep(1); // seconds
  re = system("cat /proc/picoQL > picoQL-schema.sql");
  if (re)
    printf("Calling fetch script to store result set failed.\n");
  FILE *schema;
  long procFileSize;
  size_t nRead;
  // obtain file size:
  schema = fopen ("picoQL-schema.sql", "r");
  if (schema == NULL) {
    swill_fprintf (f, "File error: picoQL-schema does not exist.<br>"); 
    exit (1);
  }
  fseek (schema, 0 , SEEK_END);
  procFileSize = ftell (schema);
  rewind (schema);
  printf("PiCO QL schema is of size %li.\n", procFileSize);
  // allocate memory to contain the whole file:
  char *buffer = (char*)malloc(sizeof(char)*procFileSize);
  if (buffer == NULL) {
    swill_fprintf(f, "Memory error");
    exit (2);
  }

  // copy the file into the buffer:
  nRead = fread (buffer, 1, procFileSize, schema);
  if (nRead != procFileSize) {
    swill_fprintf(f, "Reading error"); 
    exit (3);
  }

  // terminate
  fclose (schema);

  int rc = atoi(&buffer[nRead-3]);
  printf("Query to retrieve the database schema returned %i.\n", rc);
  (void)rc;
  buffer[nRead - 3] = '\0';
  swill_fprintf(f, "%s", buffer);
  
  free (buffer);
  swill_fprintf(f, "<br>"
		"</body>"
		"</html>");
}


/* Builds the html page of the result set of a query 
 * along with the time it took to execute and the query 
 * itself.
 */
void serve_query(FILE *f) {
  const char *query = "\0";
  // allocate memory to contain the whole file:
  char *buffer;
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
    clock_t start_clock,finish_clock;
    double c_time;
    int rc;
    char echo_query[strlen(query) + 30];  // 24 precisely
    start_clock = clock();
    swill_fprintf(f, "<b>For SQL query: ");
    swill_fprintf(f, "<span class=\"styled\">%s</span><br><br>", query);
    swill_fprintf(f, "Result set is:</b><br><br>");
    sprintf(echo_query, "echo \"%s\" > /proc/picoQL", query);    
    int re = system(echo_query);
    printf("Just wrote query to /proc/picoQL.\n");
    if (re) {
      printf("echo query to /proc/picoQL failed.\n");
      exit(1);
    }
    sleep(1); // seconds
    re = system("../server/picoQL-fetch.sh > picoQL-resultSet.sql");
    if (re) {
      printf("Calling fetch script to store result set failed.\n");
      exit(1);
    }
    FILE *rs;
    long procFileSize;
    size_t nRead;
    // obtain file size:
    rs = fopen ("picoQL-resultSet.sql", "r");
    if (rs == NULL) {
      swill_fprintf (f, "File error: picoQL-resultSet.sql does not exist.<br>"); 
      exit (1);
    }
    fseek (rs, 0 , SEEK_END);
    procFileSize = ftell (rs);
    rewind (rs);
    printf("PiCO QL resultSet is of size %li.\n", procFileSize);
    // allocate memory to contain the whole file:
    buffer = (char*)malloc(sizeof(char)*procFileSize);
    if (buffer == NULL) {
      swill_fprintf(f, "Memory error");
      exit (2);
    }

    // copy the file into the buffer:
    nRead = fread (buffer, 1, procFileSize, rs);
    if (nRead != procFileSize) {
      swill_fprintf(f, "Reading error"); 
      exit (3);
    }

    // terminate
    fclose (rs);

    rc = atoi(&buffer[nRead-3]);
    printf("Query returned %i.\n", rc);
    (void)rc;
    buffer[nRead - 3] = '\0';
  
    if (rc == 0) {
      swill_fprintf(f, "%s", buffer);
      finish_clock = clock();
      c_time = ((double)finish_clock - 
		(double)start_clock)/CLOCKS_PER_SEC;
      swill_fprintf(f, "<br><br>");
      swill_fprintf(f,"CPU total roundtrip time: <b>%f</b>s.<br><br>", c_time);
    } else {
      swill_fprintf(f, "%s", buffer);
      // comprehend /proc output

      swill_fprintf(f, "<br>See <a href=\"");
      swill_printurl(f, "pico_ql_error_page.html", "", 0);
      swill_fprintf(f,"\">SQLite error codes</a>.<br><br>");
    }
    swill_fprintf(f, "<p class=\"aligned\">");
    swill_fprintf(f, "<a href=\"");
    swill_printurl(f,"index.html", "", 0);
    swill_fprintf(f,"\">[ Input new Query ]</a>");
    swill_fprintf(f,"</p>"
		  "</body>"
		  "</html>");
    free(buffer);
  }
}


// Interface to the swill server functionality.
void call_swill(int port_number) {
  swill_init(port_number);
  swill_handle("pico_ql_logo.png", print_pico_ql_logo, 0);
  swill_handle("pico_ql_error_page.html", print_pico_ql_error_page, 0);
  swill_handle("index.html", app_index, 0);
  swill_handle("serveQuery.html", serve_query, 0);
  while (swill_serve()) {

  }
  swill_close();
}

/* The main function.
 */
int main(int argc, char **argv) {
  int port_number = 8080;
  switch (argc) {
  case 2:
    port_number = atoi(argv[1]);
    // printf("Port number is %i, argv is %s.\n", port_number, argv[1]);
    break;
  default:
    port_number = 8080;
    break;
  }
  printf("Please visit http://localhost:%i to be served.\n", port_number);
  call_swill(port_number);
  return 0;
}
