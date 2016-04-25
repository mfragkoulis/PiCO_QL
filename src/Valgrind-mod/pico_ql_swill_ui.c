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

/*
#ifdef __APPLE__
#include <sys/vmparam.h>  PAGE_SIZE 
#endif
*/

#ifdef __linux__
#include <sys/stat.h>     /* S_* */
#endif

#include <fcntl.h>    /* O_* */
#include <unistd.h>   /* open, read, write, close */
#include <stdlib.h>
#include <swill.h>
#include <string.h>
#include <sys/time.h>  /* gettimeofday, struct timeval */
#include "pico_ql_swill_access_func.h"

#define PAGE_SIZE 4096
#define KB 1024

struct picoQL_fd {
	int q;
	int rs;
	int t;
};

/* Calls the function that prints the PiCO QL error page (.html).
 */
static void print_pico_ql_error_page(FILE *f) {
  error_page(f);
}

/* Calls the function that prints the PiCO QL logo (.PNG).
 */
static void print_pico_ql_logo(FILE *f) {
  logo(f);
}

/* Builds the front page of the library's web interface, 
 * retrieves the database schema and promotes inputted 
 * queries to sqlite_engine.
 */
static void app_index(FILE *f, struct picoQL_fd *fd) {
  char *buf = (char *)malloc(sizeof(char) * PAGE_SIZE);
  char *q_time = (char *)malloc(sizeof(char) * PAGE_SIZE);
  const char master_query[40] = "SELECT * FROM sqlite_master;";
  char rs_meta[20], *rs_metaP;
  int re;
  int error_handling = 0;
  int nReads = 1;
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
		"<span class=\"style_text\"><b>Your database schema is:</b></span><br>");
  re = write(fd->q, master_query, strlen(master_query) + 1);
  printf("Wrote to picoQL_query named pipe query: %s.\nSize: %d characters.\n", master_query, re);
  re = read(fd->t, q_time, PAGE_SIZE);

  re = read(fd->rs, buf, PAGE_SIZE);
  printf("Result set from picoQL_resultset named pipe ");
  rs_metaP = strstr(buf, "<%RS%>");
  strcpy(rs_meta, (rs_metaP + 6));
  if (!strcmp(rs_meta, "-ERROR-")) error_handling = 1;
  else nReads = atoi(rs_meta);
  printf("comes in %d chunks.\n", nReads);
  buf[strlen(buf) -6 -strlen(rs_meta)] = '\0';
  nReads--;
  swill_fprintf(f, "%s", buf);

  while (nReads) {
    re = read(fd->rs, buf, PAGE_SIZE);
#ifdef PICO_QL_DEBUG
    printf("Read result set chunk:\n%s\n\n of size %d.\n", buf, re);
#endif
    swill_fprintf(f, "%s", buf);
    nReads--;
  }
  printf("Completed reading result set.\n");

  if (error_handling) {
    swill_fprintf(f, "<br>Please advise <a href=\"");
    swill_printurl(f, "pico_ql_error_page.html", "", 0);
    swill_fprintf(f,"\">SQLite error codes</a>.<br><br>");
  }
  swill_fprintf(f, "</div>"
		"<p class=\"aligned\">");
  swill_fprintf(f,"<a href=\"");
  swill_printurl(f,"terminateConnection.html", "", 0);
  swill_fprintf(f,"\">[ Terminate Server Connection ]</a>"
		"</p>"
		"</body>"
		"</html>");
  free(q_time);
  free(buf);
}


/* Builds the html page of the result set of a query 
 * along with the time it took to execute and the query 
 * itself.
 */
static void serve_query(FILE *f, struct picoQL_fd *fd) {
  const char *query = "\0";
  int error_handling = 0;
  struct timeval start_time, finish_time;
//  char rs_size_buf[10];
//  int rs_size;
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
    int re;
    char *buf = (char *)malloc(sizeof(char) * PAGE_SIZE);
    char *q_time = (char *)malloc(sizeof(char) * PAGE_SIZE);
    char rs_meta[20],  *rs_metaP;
    int nReads = 1;

    re = write(fd->q, query, strlen(query) + 1);
    printf("Wrote to picoQL_query named pipe query: %s.\nSize: %d characters.\n", query, re);
    re = read(fd->t, q_time, PAGE_SIZE);
    system("./ps_vlg.sh");
    gettimeofday(&start_time, NULL);

    swill_fprintf(f, "<b>For SQL query: ");
    swill_fprintf(f, "<span class=\"styled\">%s</span><br><br>", query);
    swill_fprintf(f, "Result set is:</b><br><br>");
 
    re = read(fd->rs, buf, PAGE_SIZE);
    printf("Result set from picoQL_resultset named pipe ");
    rs_metaP = strstr(buf, "<%RS%>");
    strcpy(rs_meta, (rs_metaP + 6));
    if (!strcmp(rs_meta, "-ERROR-")) error_handling = 1;
    else nReads = atoi(rs_meta);
    printf("comes in %d chunks.\n", nReads);
    buf[strlen(buf) -6 -strlen(rs_meta)] = '\0';
    nReads--;
    swill_fprintf(f, "%s", buf);

    while (nReads) {
      re = read(fd->rs, buf, PAGE_SIZE);
#ifdef PICO_QL_DEBUG
      printf("Read result set chunk:\n%s\n\n of size %d.\n", buf, re);
#endif
      swill_fprintf(f, "%s", buf);
      nReads--;
    }
    system("./ps_vlg.sh");
    gettimeofday(&finish_time, NULL);
    printf("Completed reading result set.\n");

    if (error_handling) {
      swill_fprintf(f, "<br>Please advise <a href=\"");
      swill_printurl(f, "pico_ql_error_page.html", "", 0);
      swill_fprintf(f,"\">SQLite error codes</a>.<br><br>");
    } else {
      swill_fprintf(f,"<br>Real time: <b>%lu.%lu</b>s.<br>", (long)(finish_time.tv_sec - start_time.tv_sec), 
							    (long)(finish_time.tv_usec - start_time.tv_usec)); 
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
    free(q_time);
    free(buf);
  }
}

// Terminates connection to the embedded web-server.
static void terminate(FILE *f, struct picoQL_fd *fd) {
  swill_fprintf(f, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n\"http://www.w3.org/TR/html4/loose.dtd\">"
		"<html>"
		"<head>"
		"<style type=\"text/css\">"
		"body{bgcolor=\"#ffffff\";}"
		"</style>"
		"</head>"
		"<body>");
  write(fd->q, "exit", 5);
  close(fd->q);
  close(fd->rs);
  swill_fprintf(f, "<b>TERMINATED CONNECTION...</b>"
		"</body>"
		"</html>");
  swill_close();
}

// Interface to the swill server functionality.
static void call_swill(int port_number, struct picoQL_fd *fd) {
  swill_init(port_number);
  swill_handle("pico_ql_logo.png", print_pico_ql_logo, 0);
  swill_handle("pico_ql_error_page.html", print_pico_ql_error_page, 0);
  swill_handle("index.html", app_index, fd);
  swill_handle("serveQuery.html", serve_query, fd);
  swill_handle("terminateConnection.html", terminate, fd);
  while (swill_serve()) {

  }
}


int main(int argc, char **argv) {
  struct picoQL_fd fd;

  /* This check is not enough. You need to check for integrity also. */
  if (argc < 2) {
    printf("Fatal error: no port provided for accepting and serving queries. Exiting now.\n");
    exit(1);
  }

  fd.q = open("/home/mfg/test/picoQL_query", O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
  if (fd.q < 0) {
    printf("Opening picoQL query pipe at client side failed.\n");
    return 1;
  }
  printf("Opened picoQL_query named pipe for web interface.\n");

  fd.rs = open("/home/mfg/test/picoQL_resultset", O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
  if (fd.rs < 0) {
    printf("Opening picoQL query pipe at client side failed.\n");
    return 1;
  }
  printf("Opened picoQL_resultset named pipe for web interface.\n");

  fd.t = open("/home/mfg/test/picoQL_time", O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
  if (fd.rs < 0) {
    printf("Opening picoQL time pipe at client side failed.\n");
    return 1;
  }
  printf("Opened picoQL_time named pipe for query processing time measurement.\n");

  printf("Please visit http://localhost:%s to be served.\n", argv[1]);
  call_swill(atoi(argv[1]), &fd);
  return 0;
}
