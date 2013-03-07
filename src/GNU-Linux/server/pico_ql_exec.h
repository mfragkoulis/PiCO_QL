/*
 *   Declare the interface to pico_ql_exec.c
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

#ifndef PICO_QL_EXEC_H
#define PICO_QL_EXEC_H

#include <sqlite3.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PICO_QL_RESULT_SET_SIZE PAGE_SIZE - 1024

  extern struct timespec picoQL_ts_start;
  extern int picoQL_output;
  extern int picoQL_metadata;

  void ioctl_output(int pico_ql_output, int pico_ql_metadata);  
  int place_result_set(const char **result_set, int *argc_slots);
  int register_table(sqlite3 *db,
		     int argc,
		     int view_index,
		     const char **queries, 
		     const char **table_names);
  int prep_exec(sqlite3 *db, const char *q);
  
#ifdef __cplusplus
}
#endif

#endif
