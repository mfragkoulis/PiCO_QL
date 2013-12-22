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

#ifdef __linux__
#include <fcntl.h> /* O_* */
#endif

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "pico_ql_interface.h"
#include "pico_ql_vt.h"
#include "pico_ql_test.h"

#define KB 1024

/* Forwards  a query for execution to sqlite and 
 * presents the resultset of a query.
 */
static int step_query(char ***res, int *argc_slots, sqlite3_stmt *stmt) {
  int col, result, rows = 0;
  char *buf = (char *)sqlite3_malloc(sizeof(char) * PAGE_SIZE);
  char *bufCol = (char *)sqlite3_malloc(sizeof(char) * PAGE_SIZE);
  char *resP = (*res)[*argc_slots - 1];
  strcpy(resP, "<table>");
  strcat(resP, "</tr>");
  for (col = 0; col < sqlite3_column_count(stmt); col++) {
    sprintf(buf, "<td><b>%s</td></b>", 
		  sqlite3_column_name(stmt, col));
    strcat(resP, buf);
  }
  strcat(resP, "</tr>");
  while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
    rows++;
    sprintf(buf, "<tr>");
    for (col = 0; col < sqlite3_column_count(stmt); col++) {
      switch (sqlite3_column_type(stmt, col)) {
      case 1:
	sprintf(bufCol, "<td><b>%ld</b></td>", 
		      (long)sqlite3_column_int64(stmt, col));
	break;
      case 2:
	sprintf(bufCol, "<td><b>%f</b></td>", 
		      sqlite3_column_double(stmt, col));
	break;
      case 3:
	sprintf(bufCol, "<td><b>%s</b></td>", 
		      sqlite3_column_text(stmt, col));
	break;
      case 4:
	sprintf(bufCol, "<td><b>%s</b></td>", 
		      (char *)sqlite3_column_blob(stmt, 
						  col));
	break;
      case 5:
	sprintf(bufCol, "<td><b>(null)</td></b>");
	break;
      }
      if (strlen(bufCol) > PAGE_SIZE) {
        sprintf(buf, "Row read has exceeded PAGE_SIZE length. Length is %d. Exiting now.\n", (int)strlen(bufCol));
        printf("%s", buf);
        result = SQLITE_ERROR;
        goto exit;
      }
      strcat(buf, bufCol);
    }
#ifdef PICO_QL_DEBUG
    sprintf(bufCol, "%d, row buffer's is %d, number of slots is %d", (int)strlen(resP), (int)strlen(buf), *argc_slots);
    printf("Row read: current result set chunk's length is %s.\n", bufCol);
#endif
    if (strlen(resP) + strlen(buf) + 40 > PAGE_SIZE) {	/* 40: For result set metadata text. */
#ifdef PICO_QL_DEBUG
      sprintf(bufCol, "%d", (int)strlen(resP));
      printf("Before new result set chunk, current chunk's length is %s.\n", bufCol);
#endif
      (*argc_slots)++;
      *res = (char **)sqlite3_realloc(*res, sizeof(char*) * (*argc_slots));
      if (!*res) {
        result = SQLITE_NOMEM;
        goto exit;
      }
      (*res)[*argc_slots - 1] = (char *)sqlite3_malloc(sizeof(char) * PAGE_SIZE);
      resP = (*res)[*argc_slots - 1];
      if (!resP) {
        result = SQLITE_NOMEM;
        goto exit;
      }
      strcpy(resP, buf);
      strcat(resP, "</tr>");
#ifdef PICO_QL_DEBUG
      sprintf(bufCol, "%d", (int)strlen(resP));
      printf("After malloc, new result set chunk's length is %s.\n", bufCol);
#endif
    } else {
      strcat(resP, buf);
      strcat(resP, "</tr>");
    }
  }
  strcat(resP,"</table>");
  strcat(resP, "<br>");
  sprintf(buf, "<b>%d rows in result set.</b><br>", rows);
#ifdef PICO_QL_DEBUG
  printf("%s.\n", buf);
#endif
  strcat(resP, buf);
#ifdef PICO_QL_DEBUG
  printf("Result set is\n\n%s\n\n", resP);
#endif

exit:
  sqlite3_free(bufCol);
  sqlite3_free(buf);
  return result;
}

/* Calls step_query for query execution. 
 * Collects and acts on the result status of a query 
 * execution.
 */
static int file_prep_exec(char ***res, int *argc_slots, sqlite3_stmt *stmt) {
  int result = 0;
  char *resP;
  result = step_query(res, argc_slots, stmt);
  resP = (*res)[*argc_slots - 1];
  switch (result) {
  case SQLITE_DONE:
#ifdef PICO_QL_DEBUG
    printf("\nDONE\n");
#endif
    break;
  case SQLITE_OK:
#ifdef PICO_QL_DEBUG
    printf("\nOK\n");
#endif
    break;
  case SQLITE_ERROR:
    strcat(resP, "<b>SQL error or missing database.\n</b>");
    break;
  case SQLITE_MISUSE:
    strcat(resP, "<b>Library used incorrectly.<br></b>");
    break;
  }
  return result;
}

// Takes care of query preparation and execution.
static int prep_exec(char ***res, int *argc_slots, sqlite3 *db, const char *q){
  sqlite3_stmt *stmt;
  int result, prepare;
  if ((prepare = sqlite3_prepare_v2(db, q, -1, &stmt, 0)) == SQLITE_OK) {
    if (res) {
      result = file_prep_exec(res, argc_slots, stmt);
    } else {
      /* Step only: queries with no resultset (check if 
       * table exists). For those queries preparation 
       * will always succeed.
       */ 
      result = sqlite3_step(stmt);
    }
  } else {
    if (res)
      sprintf((*res)[0], "Error in preparation of query: error no %d\n", prepare);
    return prepare;
  }
  sqlite3_finalize(stmt);
  return result;
}


static int chunk_write(int fd_picoQL_rs, char *page) {
  return write(fd_picoQL_rs, page, PAGE_SIZE);
} 

/* Builds the html page of the result set of a query 
 * along with the time it took to execute and the query 
 * itself.
 */
static int serve_query(int fd_picoQL_query, int fd_picoQL_rs, sqlite3 *db) {
  char *query = (char *)sqlite3_malloc(sizeof(char) * PAGE_SIZE);
  char *buf = (char *)sqlite3_malloc(sizeof(char) * PAGE_SIZE);
  while (1) {
    int re, rc = 0;
    int j = 0;
    char **res;
    int argc_slots = 1, n = 0;
    memset(query, 0, PAGE_SIZE);
    printf("Waiting for a picoQL query...\n");
    re = read(fd_picoQL_query, query, PAGE_SIZE);
    printf("Read query: %s.\n", query);
    if (!strcmp(query, "exit")) break;
    res = (char **)sqlite3_malloc(sizeof(char *));
    res[0] = (char *)sqlite3_malloc(sizeof(char) * PAGE_SIZE);
    memset(buf, 0, PAGE_SIZE);
    // j for debugging, execute the query multiple times.
    while (j < 1 && (rc = prep_exec(&res, &argc_slots, db, query)) == SQLITE_DONE) {
      j++;
    }
    if (rc == SQLITE_DONE) {
      strcat(res[0], "<%RS%>");
      sprintf(buf, "%d", argc_slots);
      strcat(res[0], buf);
    } else {
      sprintf(buf, "<br><b>Extended error message: <b>%s</b><br><br>", sqlite3_errmsg(db));
      strcat(res[argc_slots - 1], buf);
      sprintf(buf, "Extended error code No <b>%d.", sqlite3_extended_errcode(db));
      strcat(res[argc_slots - 1], buf);
      strcat(res[0], "<%RS%>-ERROR-");
    }
    while (n < argc_slots) {
      re = chunk_write(fd_picoQL_rs, res[n]);
      sqlite3_free(res[n]);
      n++;
    }
    sqlite3_free(res);
    clear_temp_structs();
    printf("Done processing picoQL query:\n%s\n", query);
  }
  sqlite3_free(query);
  sqlite3_free(buf);
  deinit_selectors();
  return SQLITE_OK;
}


/* Executes the SQL CREATE queries, opens the sqlite 
 * database connection and calls swill or pico_ql_test 
 * depending on the compile flag TEST.
 */
int register_table(int argc,
		   int view_index, 
		   const char **q, 
		   const char **sqlite_names) {
  /* This definition implicitly constraints a table name 
   * to 140 characters. It should be more than enough.
   */
  char sqlite_query[200];
  int re = -1, i = 0, fd_picoQL_query, fd_picoQL_rs;
  SysRes sr;
  sqlite3 *db;
  int output;
/* Virtual table schema will be in-memory and will not
   persist. Views can be included in the DSL */
  re = sqlite3_open(":memory:", &db); 
  if (re) {
    printf("Can't open database. Error code %d.\n", re);
    printf("Extended error code %d.\n", sqlite3_extended_errcode(db));
    printf("Extended error message:\n%s\n\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return re;
  }
  re = prep_exec(NULL, 0, db, "PRAGMA main.journal_mode=OFF;");  /* Turn off journals.*/
#ifdef PICO_QL_DEBUG
  sprintf(sqlite_query, "Query to turn off main.journal returned %d.\n", re);
  printf("%s", sqlite_query);
#endif
  re = prep_exec(NULL, 0, db, "PRAGMA temp.journal_mode=OFF;");
#ifdef PICO_QL_DEBUG
  sprintf(sqlite_query, "Query to turn off temp.journal returned %d.\n", re);
  printf("%s", sqlite_query);
#endif
  re = prep_exec(NULL, 0, db, "PRAGMA page_size = 4096;");  /* Set SQLite's page size at 4096 Bytes.*/
#ifdef PICO_QL_DEBUG
  sprintf(sqlite_query, "Query to set SQLite's page size at 4096 Bytes returned %d.\n", re);
  printf("%s", sqlite_query);
#endif
  re = prep_exec(NULL, 0, db, "PRAGMA cache_size = 10000;");  /* Set SQLite's cache size at 10000 pages.*/
#ifdef PICO_QL_DEBUG
  sprintf(sqlite_query, "Query to set SQLite's cache size at 10000 pages returned %d.\n", re);
  printf("%s", sqlite_query);
#endif
#ifdef PICO_QL_DEBUG
  for (i = 0; i < argc; i++) {
    printf("\nQuery to be executed: %s.\n", q[i]);
  }
#endif
  sqlite3_module *mod;
  mod = (sqlite3_module *)sqlite3_malloc(sizeof(sqlite3_module));
  fill_module(mod);
  output = sqlite3_create_module(db, "PicoQL", mod, NULL);
  if (output == 1) 
    printf("Error while registering module\n");
#ifdef PICO_QL_DEBUG
  else if (output == 0) 
    printf("Module registered successfully\n");
#endif
#ifndef __APPLE__
#ifndef SQLITE_OMIT_LOAD_EXTENSION
// sqlite3_load_extension() calls
  if (sqlite3_enable_load_extension(db, 1))
    printf("Extension loading failed.\n");
  if (sqlite3_load_extension(db, "math_func_sqlitext", NULL, NULL))
    printf("Extension loading failed.\n");
// sqlite3_create_function() calls
#endif
#endif
  for (i = 0; i < argc; i++) {
    char sqlite_type[20];
    if (i < view_index)
      strcpy(sqlite_type, "table");
    else
      strcpy(sqlite_type, "view");
    sprintf(sqlite_query, "SELECT * FROM sqlite_master WHERE type='%s' AND name='%s';", sqlite_type, sqlite_names[i]);
    if (prep_exec(NULL, 0, db, (const char *)sqlite_query) != SQLITE_ROW) {
      re = prep_exec(NULL, 0, db, (const char *)q[i]);
#ifdef PICO_QL_DEBUG
      sprintf(sqlite_type, "returned %d.\n", re);
      printf("Query %s", q[i]);
      printf("%s", sqlite_type);
#endif
      if (re != 101) {
	printf("Extended error code: %d.\n", sqlite3_extended_errcode(db));
	printf("Extended error message:\n%s.\n", sqlite3_errmsg(db));
	return re;
      }
    }
  }
  start_serving();
#ifndef PICO_QL_TEST
  sr = mknod("picoQL_query", S_IFIFO | 0660, 0);
  if (sr_isError(sr)) {
    printf("Creating picoQL query pipe with error code %d.\n", (int)sr_Res(sr));
    return SQLITE_ERROR;
  }
  printf("Created picoQL_query named pipe.\n");
  sr = mknod("picoQL_resultset", S_IFIFO | 0660, 0);
  if (sr_isError(sr)) {
    printf("Creating picoQL result set pipe with error code %d.\n", (int)sr_Res(sr));
    return SQLITE_ERROR;
  }
  printf("Created picoQL_resultset named pipe.\n");

  printf("Please execute ./picoQL-gui to initialize the web interface.\n");

  fd_picoQL_query = open("picoQL_query", O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP); /* Blocks until ./picoQL-gui is executed. */
  if (fd_picoQL_query < 0) {
    printf("Opening picoQL_query named pipe failed.\n");
    return SQLITE_ERROR;
  }
  printf("Opened picoQL_query named pipe.\n");

  fd_picoQL_rs = open("picoQL_resultset", O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
  if (fd_picoQL_rs < 0) {
    printf("Opening picoQL result set named pipe failed.\n");
    return SQLITE_ERROR;
  }
  printf("Opened picoQL_resultset named pipe.\n");

  re = serve_query(fd_picoQL_query, fd_picoQL_rs, db);

  close(fd_picoQL_query);
  close(fd_picoQL_rs);

  unlink("picoQL_query");
  unlink("picoQL_resultset");
#else
  re = call_test(db);
#endif
  sqlite3_free(mod);
  return re;
}
