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

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include "pico_ql_search_helper.h"

using namespace std;

/* Tests if current data structure or value passed by 
 * sqlite are either null or empty and returns 1 if so.
 */
int struct_empty_null(sqlite3_vtab_cursor *cur, sqlite3_value *val) {
  picoQLTableCursor *stcsr = (picoQLTableCursor *)cur;
  if ((stcsr->isInstanceNULL) || ((val != NULL) && (!strcmp((const char *)sqlite3_value_text(val), "(null)")))) {
    stcsr->isInstanceNULL = 1;
    stcsr->max_size = 1;
    stcsr->size = 1;     // Size 1 to print "(null)".
    return 1;
  }
  if ((stcsr->isInstanceEmpty) || ((val != NULL) && (!strcmp((const char *)sqlite3_value_text(val), "(empty)")))) {
    stcsr->isInstanceEmpty = 1;
    stcsr->max_size = 1;
    stcsr->size = 1;     // Size 1 to print "(empty)".
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
    sqlite3_result_text(con, "(null)", -1, SQLITE_STATIC);
    return 1;
  }
  if (stcsr->isInstanceEmpty) {
    sqlite3_result_text(con, "(empty)", -1, SQLITE_STATIC);
    return 1;
  }
  return 0;
}

/* Reallocates the space allocated to the resultset struct.
 * Useful for embedded picoQL data structures.
 */
int realloc_resultset(sqlite3_vtab_cursor *cur) {
    picoQLTableCursor *stcsr = (picoQLTableCursor *)cur;
    int arraySize;
    int *res;
    arraySize = get_datastructure_size(cur);
    if (arraySize == 0) {
      stcsr->isInstanceEmpty = 1;
      arraySize = 1;
      stcsr->size = 1;
    } else
      stcsr->isInstanceEmpty = 0;
    if (arraySize != stcsr->max_size ) {
        res = (int *)sqlite3_realloc(stcsr->resultSet, 
				     sizeof(int) * 
				     arraySize);
        if (res != NULL) {
            stcsr->resultSet = res;
            memset(stcsr->resultSet, -1,
                   sizeof(int) * arraySize);
#ifdef PICO_QL_DEBUG
            printf("\nReallocating resultSet..now max size %i \n\n", stcsr->max_size);
#endif
        }else{
            sqlite3_free(res);
            printf("Error (re)allocating memory\n");
            return SQLITE_NOMEM;
        }
    }
    if (stcsr->isInstanceEmpty) {   // Size 1 to print "(empty)".
        stcsr->max_size = 1;
    } else
	stcsr->max_size = arraySize;
    return SQLITE_OK;
}

/* Compares two integers and returns the result of the 
 * comparison.
 */
int compare(int dstr_value, int op, int value) {
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
}

/* Compares two long integers and returns the result of 
 * the comparison.
 */
int compare(long int dstr_value, int op, long int value) {
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
}


/* Compares two doubles and returns the result of the 
 * comparison.
 */
int compare(double dstr_value, int op, double value) {
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
}


/* Compares two void pointers and returns the result of the
 * comparison. Mem comparison.
 */
int compare(const void *dstr_value, int op, 
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
}

/* Compares two arrays of characters and returns the result
 * of the comparison.
 */
int compare(const unsigned char *dstr_value, int op,
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
}


/* Compares the current resultset with the one stored in 
 * the cursor. Their intersection survives.
 */
int compare_res(int count, picoQLTableCursor *stcsr, 
		int *temp_res) {
    int ia, ib;
    int *i_res;
    int i_count = 0;
    if ((stcsr->size == 0) && (stcsr->first_constr == 1)) {
      memcpy(stcsr->resultSet, temp_res, sizeof(int) *
               stcsr->max_size);
        stcsr->size = count;
        stcsr->first_constr = 0;
    } else if (stcsr->size > 0) {
        i_res = (int *)sqlite3_malloc(sizeof(int) *
                                      stcsr->max_size);
        if (i_res == NULL) {
            sqlite3_free(i_res);
            printf("Error (re)allocating memory\n");
            return SQLITE_NOMEM;
        }
        for (int a = 0; a < stcsr->size; a++) {
            for (int b = 0; b < count; b++) {
                ia = stcsr->resultSet[a];
                ib = temp_res[b];
                if (ia == ib) {
                    i_res[i_count++] = ia;
                } else if (ia < ib)
                    b = count;
            }
        }
        assert(i_count <= stcsr->max_size);
        memcpy(stcsr->resultSet, i_res, sizeof(int) *
               i_count);
        stcsr->size = i_count;
        sqlite3_free(i_res);
    }
    return SQLITE_OK;
}

/* Interprets the structure of constraint to operation and 
 * column it regards.
 */
void check_alloc(const char *constr, int &op, int &iCol) {
    op = constr[0] - 'A';
    iCol = constr[1] - 'a' + 1;
}
