/*
 *   Declare the interface to user_functions.c
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
 *  permissions and limitations under the License.
 */

#ifndef USER_FUNCTIONS_H
#define USER_FUNCTIONS_H

#include <stdio.h>
#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

int register_table(const char *nDb, 
		   int argc, 
		   const char **queries, 
		   const char **table_names, 
		   void *data);
int prep_exec(FILE *f, sqlite3 *db, const char *q);

#ifdef __cplusplus
}
#endif

#endif
