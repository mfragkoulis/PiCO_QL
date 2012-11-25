/*
 *   Declare interface to pico_ql_search_helper.c
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

#ifndef PICO_QL_SEARCH_HELPER_H
#define PICO_QL_SEARCH_HELPER_H

#include <sqlite3.h>


#ifdef __cplusplus
extern "C" {
#endif

int struct_empty_null(sqlite3_vtab_cursor *cur, sqlite3_value *val, int structEmbedded, int nCol);
int struct_is_empty_null(sqlite3_vtab_cursor *cur, sqlite3_context *con);
int compare_int(int dstr_value, int op, int value);
int compare_int64(long int dstr_value, int op, long int value);
int compare_double(double dstr_value, int op, double value);
int compare_blob(const void *dstr_value, int op, 
	    const void *value);
int compare_text(const unsigned char *dstr_value, int op,
	    const unsigned char *value);

#ifdef __cplusplus
}
#endif


#endif
