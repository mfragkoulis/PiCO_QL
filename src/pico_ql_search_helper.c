/*
 *   Implement utility functions used mainly in the 
 *   filtering process of a query.
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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico_ql_search_helper.h"
#include "pico_ql_internal.h"


/* Tests if current data structure or value passed by 
 * sqlite are either null or empty and returns 1 if so.
 */
int struct_empty_null(sqlite3_vtab_cursor *cur, sqlite3_value *val, int structEmbedded, int nCol) {
  picoQLTableCursor *stcsr = (picoQLTableCursor *)cur;
  /* nCol > 0 allows to embedded structures
   * to propagate their error condition. At first
   * (always nCol = 0) their state has not been 
   * initialised yet.
   */
  if (((stcsr->isInstanceNULL) && ((!structEmbedded) || (nCol > 0))) || ((val != NULL) && (sqlite3_value_type(val) == SQLITE_NULL))) {
    stcsr->isInstanceNULL = 1;
    return 1;
  }
  if (((stcsr->isInstanceEmpty) && ((!structEmbedded) || (nCol > 0))) || ((val != NULL) && (!strcmp((const char *)sqlite3_value_text(val), "(empty)")))) {
    stcsr->isInstanceEmpty = 1;
    return 1;
  }
  return 0;
}

/* Tests if current data structure is null or empty and 
 * passes "(null)"/"(empty)" to sqlite and returns 1 if so.
 */
int struct_is_empty_null(sqlite3_vtab_cursor *cur, sqlite3_context *con) {
  picoQLTableCursor *stcsr = (picoQLTableCursor *)cur;
  if (stcsr->isInstanceNULL) {
    sqlite3_result_null(con);
    return 1;
  }
  if (stcsr->isInstanceEmpty) {
    sqlite3_result_text(con, "(empty)", -1, SQLITE_STATIC);
    return 1;
  }
  return 0;
}


/* Compares two integers and returns the result of the 
 * comparison.
 */
int compare_int(int dstr_value, int op, int value) {
  switch (op) {
  case 0:
    return dstr_value < value;
  case 1:
    return dstr_value <= value;
  case 2:
    return dstr_value == value;
  case 3:
    return dstr_value >= value;
  case 4:
    return dstr_value > value;
  }
  return SQLITE_INTERNAL;
}

/* Compares two long integers and returns the result of 
 * the comparison.
 */
int compare_int64(long int dstr_value, int op, long int value) {
  switch (op) {
  case 0:
    return dstr_value < value;
  case 1:
    return dstr_value <= value;
  case 2:
    return dstr_value == value;
  case 3:
    return dstr_value >= value;
  case 4:
    return dstr_value > value;
  }
  return SQLITE_INTERNAL;
}


/* Compares two doubles and returns the result of the 
 * comparison.
 */
int compare_double(double dstr_value, int op, double value) {
  switch (op) {
  case 0:
    return dstr_value < value;
  case 1:
    return dstr_value <= value;
  case 2:
    return dstr_value == value;
  case 3:
    return dstr_value >= value;
  case 4:
    return dstr_value > value;
  }
  return SQLITE_INTERNAL;
}


/* Compares two void pointers and returns the result of the
 * comparison. Mem comparison.
 */
int compare_blob(const void *dstr_value, int op, 
		 const void *value) {
  switch (op) {
  case 0:
    return dstr_value < value;
  case 1:
    return dstr_value <= value;
  case 2:
    return dstr_value == value;
  case 3:
    return dstr_value >= value;
  case 4:
    return dstr_value > value;
  }
  return SQLITE_INTERNAL;
}

/* Compares two arrays of characters and returns the result
 * of the comparison.
 */
int compare_text(const unsigned char *dstr_value, int op,
		 const unsigned char *value) {
  switch (op) {
  case 0:
    return strcmp((const char *)dstr_value,
		  (const char *)value) < 0;
  case 1:
    return strcmp((const char *)dstr_value,
		  (const char *)value) <= 0;
  case 2:
    return strcmp((const char *)dstr_value,
		  (const char *)value) == 0;
  case 3:
    return strcmp((const char *)dstr_value,
		  (const char *)value) >= 0;
  case 4:
    return strcmp((const char *)dstr_value,
		  (const char *)value) > 0;
  }
  return SQLITE_INTERNAL;
}
