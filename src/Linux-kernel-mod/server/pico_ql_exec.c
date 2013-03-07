#include <linux/slab.h>
#include <linux/types.h>
#include <linux/string.h>
#include <sqlite3.h>
#include "pico_ql_exec.h"

#define PICO_QL_TEXT picoQL_output == 10
#define PICO_QL_META picoQL_metadata == 1
static int serving = 0;
static struct timespec picoQL_ts_end;

/* Forwards  a query for execution to sqlite and
 * presents the resultset of a query.
 */
int step_query(sqlite3_stmt *stmt, 
	       char ***root_result_set,
               int *argc_slots) {
  int col, result, rows = 0;
  char *result_set_row, *placeholder, *result_set;
  result_set_row = (char *)sqlite3_malloc(sizeof(char) * PICO_QL_RESULT_SET_SIZE);
  placeholder = (char *)sqlite3_malloc(sizeof(char) * PICO_QL_RESULT_SET_SIZE);
  result_set = (*root_result_set)[0];
  if (PICO_QL_TEXT) {
    if (PICO_QL_META)
      strcpy(result_set, "\n");
    else
      strcpy(result_set, "");
  } else {
    strcpy(result_set, "<table>");
    if (PICO_QL_META)
      strcat(result_set, "</tr>");
  }
  if (PICO_QL_META) {
    for (col = 0; col < sqlite3_column_count(stmt); col++) {
      if (PICO_QL_TEXT)
        sprintf(placeholder, "%s ", sqlite3_column_name(stmt, col));
      else
        sprintf(placeholder, "<td><b>%s</td></b>", sqlite3_column_name(stmt, col));
      strcat(result_set, placeholder);
    }
  }
  if (!PICO_QL_TEXT) {
    if (PICO_QL_META)
      strcat(result_set, "</tr>");
    else
      strcpy(result_set, "</tr>");
  }
  while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
    rows++;
    if (PICO_QL_META) {
      if (PICO_QL_TEXT)
        strcpy(result_set_row, "\n");
      else
        strcpy(result_set_row, "<tr>");
    } else
        strcpy(result_set_row, "");
    for (col = 0; col < sqlite3_column_count(stmt); col++) {
      switch (sqlite3_column_type(stmt, col)) {
      case 1:
        if (PICO_QL_TEXT)
          sprintf(placeholder, "%li ", (long int)sqlite3_column_int64(stmt, col));
        else 
          sprintf(placeholder, "<td><b>%li</td></b>", (long)sqlite3_column_int64(stmt, col));
	strcat(result_set_row, placeholder);
	break;
      case 2:
/* instead of double */
        if (PICO_QL_TEXT)
          sprintf(placeholder, "%li ", (long int)sqlite3_column_int64(stmt, col));
        else
          sprintf(placeholder, "<td><b>%li</td></b>", (long int)sqlite3_column_int64(stmt, col));
	strcat(result_set_row, placeholder);
	break;
      case 3:
        if (PICO_QL_TEXT)
          sprintf(placeholder, "%s ", sqlite3_column_text(stmt, col));
        else
          sprintf(placeholder, "<td><b>%s</td></b>", sqlite3_column_text(stmt, col));
	strcat(result_set_row, placeholder);
	break;
      case 4:
        if (PICO_QL_TEXT)
          sprintf(placeholder, "%s ", (char *)sqlite3_column_blob(stmt, col));
        else
          sprintf(placeholder, "<td><b>%s</td></b>", (char *)sqlite3_column_blob(stmt, col));
	strcat(result_set_row, placeholder);
	break;
      case 5:
        if (PICO_QL_TEXT)
	  strcat(result_set_row, "null ");
        else
	  strcat(result_set_row, "<td><b>null</td></b>");
	break;
      }
    }
/* CONSTRAINT: ROW_SIZE < PICO_QL_RESULT_SET_SIZE */
    if (PICO_QL_TEXT)
      strcat(result_set_row, "\n");
    else
      strcat(result_set_row, "</tr>");
    if (strlen(result_set) + strlen(result_set_row) + 3 >= PICO_QL_RESULT_SET_SIZE) {
      (*argc_slots)++;
      *root_result_set = (char **)sqlite3_realloc(*root_result_set, sizeof(char *) * (*argc_slots));
      if (!*root_result_set)
        return SQLITE_NOMEM;
      (*root_result_set)[*argc_slots - 1] = (char *)sqlite3_malloc(sizeof(char) * PICO_QL_RESULT_SET_SIZE);
      result_set = (*root_result_set)[*argc_slots - 1];
      strcpy(result_set, (const char *)result_set_row);
    } else {
      strcat(result_set, (const char *)result_set_row);
    }
#ifdef PICO_QL_DEBUG
    printk(KERN_DEBUG "result_set of row %i is %s.\n", rows, result_set_row); 
#endif
  }
  if (result == SQLITE_DONE) {
    struct timespec picoQL_exec_time;
    char *metadata = (char *)sqlite3_malloc(sizeof(char) * 50);
    // printk(KERN_DEBUG "Query stepped successfully. Now decorating result_set of length %i, partition %i.\n", (int)strlen(result_set), *argc_slots - 1);
    if (PICO_QL_TEXT) {
    } else {
      strcpy(placeholder,"</table>");
      strcat(placeholder, "<br>");
    }
    if (PICO_QL_META) {
      if (PICO_QL_TEXT)
        sprintf(placeholder, "\n%i rows in result set.\n", rows);
      else {
        sprintf(metadata, "<br><b>%i rows in result set</b><br>", rows);
        strcat(placeholder, metadata);
        strcat(placeholder, "<br>");
      }
    }
    getrawmonotonic(&picoQL_ts_end);
    picoQL_exec_time = timespec_sub(picoQL_ts_end, picoQL_ts_start);
    if (PICO_QL_TEXT)
      sprintf(metadata, "Kernel execution time %luns.\n", (long unsigned int)timespec_to_ns((const struct timespec *)&picoQL_exec_time));
    else
      sprintf(metadata, "<b>Kernel execution time %luns.</b>\n", (long unsigned int)timespec_to_ns((const struct timespec *)&picoQL_exec_time));
    if (PICO_QL_META) {
      strcat(placeholder, metadata);
    }
#ifdef PICO_QL_DEBUG
    printk(KERN_DEBUG "picoQL query executed in %luns.\n", (long unsigned int)timespec_to_ns((const struct timespec *)&picoQL_exec_time));
#endif
    sqlite3_free(metadata);
    if (strlen(result_set) + strlen(placeholder) + 53 >= PICO_QL_RESULT_SET_SIZE) { // 50 for execution time, 3 for safety
      (*argc_slots)++;
      *root_result_set = (char **)sqlite3_realloc(*root_result_set, sizeof(char *) * (*argc_slots));
      if (!*root_result_set)
        return SQLITE_NOMEM;
      (*root_result_set)[*argc_slots - 1] = (char *)sqlite3_malloc(sizeof(char) * PICO_QL_RESULT_SET_SIZE);
      result_set = (*root_result_set)[*argc_slots - 1];
      if (PICO_QL_META)
        strcpy(result_set, (const char *)placeholder);
    } else
      if (PICO_QL_META)
        strcat(result_set, (const char *)placeholder);
    // printk(KERN_DEBUG "Decorated result set of length %i, partition %i.\n", (int)strlen(result_set), *argc_slots - 1);
  }
  sqlite3_free(result_set_row);
  sqlite3_free(placeholder);
  return result;
}

/* Calls step_query for query execution.
 * Collects and acts on the result status of a query
 * execution.
 */
int file_prep_exec(sqlite3* db,
                   sqlite3_stmt *stmt,
                   const char *q,
		   char ***root_result_set,
		   int *argc_slots) {
  char *placeholder = (char *)sqlite3_malloc(sizeof(char) * 256);
  char *result_set;
  int result = 0;
#ifdef PICO_QL_DEBUG
  printk(KERN_DEBUG "In file_prep_exec query to execute is %s, statement structure %lx.\n", q, (long)stmt);
#endif
  result = step_query(stmt, root_result_set, argc_slots);
  result_set = (*root_result_set)[*argc_slots - 1];
  switch (result) {
  case SQLITE_DONE:
    if (PICO_QL_META)
      strcat(result_set, "0");
    break;
  case SQLITE_OK:
    if (PICO_QL_META)
      strcat(result_set, "0");
    break;
  case SQLITE_ERROR:
    if (PICO_QL_TEXT) {
      strcpy(result_set, "SQL error or missing database.\n");
      sprintf(placeholder, "\nExtended error message:\n%s\n\n", sqlite3_errmsg(db));
    } else { 
      strcpy(result_set, "<b>SQL error or missing database.\n</b>");
      sprintf(placeholder, "<br><b>Extended error message:<br><b>%s</b><br><br>", sqlite3_errmsg(db));
    }
    strcat(result_set, placeholder);
    if (PICO_QL_TEXT)
      sprintf(placeholder, "\nExtended error code %i.\nPlease advise SQLite error codes.\n", sqlite3_extended_errcode(db));
    else 
      sprintf(placeholder, "<br><b>Extended error code <b>%i.<br>Please advise </b><a href=\"", sqlite3_extended_errcode(db));
    strcat(result_set, placeholder);
    if (PICO_QL_META)
      strcat(result_set, "1");
    break;
  case SQLITE_MISUSE:
    if (PICO_QL_TEXT) {
      strcpy(result_set, "Library used incorrectly.\n");
      sprintf(placeholder, "\nExtended error message:\n%s\n\n", sqlite3_errmsg(db));
    } else {
      strcpy(result_set, "<b>Library used incorrectly.<br></b>");
      sprintf(placeholder, "<br><b>Extended error message:<br><b>%s</b><br><br>", sqlite3_errmsg(db));
    }
    strcat(result_set, placeholder);
    if (PICO_QL_TEXT)
      sprintf(placeholder, "\nExtended error code %i.\n", sqlite3_extended_errcode(db));
    else
      sprintf(placeholder, "<br><b>Extended error code <b>%i.<br>Please advise </b><a href =\"", sqlite3_extended_errcode(db));
    strcat(result_set, placeholder);
    if (PICO_QL_META)
      strcat(result_set, "21");
    break;
  default:
    if (PICO_QL_TEXT) {
      sprintf(result_set, "Error code %i.\n", result);
      sprintf(placeholder, "\nExtended error message:\n%s\n\n", sqlite3_errmsg(db));
    } else {
      sprintf(result_set, "Error code %i.<br>", result);
      sprintf(placeholder, "<br><b>Extended error message:<br><b>%s</b><br><br>", sqlite3_errmsg(db));
    }
    strcat(result_set, placeholder);
    if (PICO_QL_TEXT)
      sprintf(placeholder, "\nExtended error code %i.\n", sqlite3_extended_errcode(db));
    else
      sprintf(placeholder, "<br><b>Extended error code <b>%i.<br>Please advise </b><a href =\"", sqlite3_extended_errcode(db));
    strcat(result_set, placeholder);
    if (PICO_QL_META)
      sprintf(placeholder, "%i", result);
    strcat(result_set, placeholder);
    break;
  }
#ifdef PICO_QL_DEBUG
  printk(KERN_DEBUG "file_prep_exec returns code %i and result_set %s.\n", result, result_set);
#endif
  sqlite3_free(placeholder);
  return result;
}

// Takes care of query preparation and execution.         
int prep_exec(sqlite3 *db, const char *q){
  sqlite3_stmt  *stmt;
  int re, prepare, i;
  char **root_result_set = (char **)sqlite3_malloc(sizeof(char *));
  int argc_slots = 1;
  char *result_set = (char *)sqlite3_malloc(sizeof(char) * PICO_QL_RESULT_SET_SIZE);
  root_result_set[0] = result_set;
#ifdef PICO_QL_DEBUG
  printk(KERN_DEBUG "In prep_exec query to execute is %s.\n", q);
#endif
  if ((prepare = sqlite3_prepare_v2(db, q, -1, &stmt, 0)) == SQLITE_OK) {
#ifdef PICO_QL_DEBUG
    printk(KERN_DEBUG "Preparing query %s returned %i. Statement is %lx.\n", q, prepare, (long)stmt);
#endif
    re = file_prep_exec(db, stmt, q, &root_result_set, &argc_slots);
#ifdef PICO_QL_DEBUG
    printk(KERN_DEBUG "Stepping query %s returned %i.\n", q, re);
    printk(KERN_DEBUG "Once again: query returned %i.\n", re);
#endif
    goto exit;
  } else {
    printk(KERN_ERR "Error during query preparation: error no %i.\n", prepare);
    if (PICO_QL_TEXT) 
      sprintf(result_set, "Error during query preparation: error no %i\n", prepare);
    else
      sprintf(result_set, "Error during query preparation: error no %i<br>", prepare);
    if (PICO_QL_META)
      strcat(result_set, "1");
    re = prepare;
    goto exit;
  }

exit:
  sqlite3_finalize(stmt);
#ifdef PICO_QL_DEBUG
  printk(KERN_DEBUG "Result set to place is partitioned in %i pieces. Last partition is \n%s.\n\nEnd.\n", argc_slots - 1, result_set);
#endif
  if (serving)
    place_result_set((const char **)root_result_set, &argc_slots);
  else {
    for (i=0; i < argc_slots; i++)
      sqlite3_free(root_result_set[i]);
    sqlite3_free(root_result_set);
  }
  return re;
}

/* Executes the SQL CREATE queries, opens the sqlite
 * database connection and calls swill or pico_ql_test
 * depending on the compile flag TEST.
 */
int register_table(sqlite3 *db,
                   int argc,
                   int view_index,
                   const char **q,
                   const char **sqlite_names) {
  int i, re;
  /* This definition implicitly constraints a table name
   * to 140 characters. It should be more than enough.
   */
  char *sqlite_query = (char *)sqlite3_malloc(sizeof(char) * 200);
  char *pragma_query = (char *)sqlite3_malloc(50);
  strcpy(pragma_query, "PRAGMA main.journal_mode=OFF;");
  re = prep_exec(db, pragma_query);
  sqlite3_free(pragma_query);
  re = prep_exec(db, "PRAGMA temp.journal_mode=OFF;");
#ifdef PICO_QL_DEBUG
  for (i = 0; i < argc; i++) {
    printk(KERN_DEBUG "Query to be executed: %s.", q[i]);
  }
#endif
  for (i = 0; i < argc; i++) {
    char sqlite_type[10];
    if (i < view_index)
      strcpy(sqlite_type, "table");
    else
      strcpy(sqlite_type, "view");
    sprintf(sqlite_query, "SELECT * FROM sqlite_master WHERE type='%s' AND name='%s';", sqlite_type, sqlite_names[i]);
    if ((re = prep_exec(db, (const char *)sqlite_query)) != SQLITE_ROW) {
      if ((re != SQLITE_OK) && (re != SQLITE_DONE)) {
        printk(KERN_ERR "Table existence query %s failed with return code %i.\n", sqlite_query, re);
        goto exit;
      }
      re = prep_exec(db, (const char *)q[i]);
#ifdef PICO_QL_DEBUG
      printk(KERN_DEBUG "Table registration query %s returned %i\n", q[i], re);
#endif
      if (re != 101) {
        printk(KERN_ERR "Extended error code: %i.\n", sqlite3_extended_errcode(db));
        printk(KERN_ERR "Extended error message:\n%s.\n", sqlite3_errmsg(db));
        goto exit;
      }
    }
  }

exit:
  sqlite3_free(sqlite_query);
  if (re == 101)
    serving = 1;
  return re;
}
