/*
 *   Set up local query preparation and execution
 *   environment for testing purposes.
 *   Execute the queries user has included and write the
 *   output in test_current.txt.
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

#include "pico_ql_test.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico_ql.h"

int exec_tests() {
  FILE *f;
  f = fopen("bankapp_test_current.txt", "w");
  int i = 1;
  char *q;

  q = "select * from SuperAccounts;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select * from SuperAccounts where SuperAccountisbn > 0 and SuperAccountbalance < 500 and SuperAccountaccount_no>1000 order by id;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select * from Accounts;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select * from AccountsNULL;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select * from Accounts where rate >10 and balance<20000 order by account_no;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select * from AccountsNULL where rate >10 and balance<20000 order by account_no;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select * from SpecialAccounts;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select * from SpecialAccounts where bonus > 20 and rate >15 order by account_no;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select * from Myaccounts;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  //deinit_vt_selectors();
  //sqlite3_close(db);
  fclose(f);
  return SQLITE_DONE;
}
