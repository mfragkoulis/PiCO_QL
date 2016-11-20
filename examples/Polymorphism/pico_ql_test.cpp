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
 *   distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 *   express or implied.
 *   See the License for the specific language governing
 *   permissions and limitations under the License.
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "pico_ql.h"
using namespace picoQL;

/* Executes test queries. */
int exec_tests() {
  stringstream s;
  fstream fs;
  fs.open("polymorphism_test_current.txt", fstream::out);

  int i = 0;
  char q[400];

  strcpy(q, "select rownum, description, balance, available_amount, binded_amount from Account;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select rownum, description, balance, available_amount, binded_amount, overlimit, due_date from Account JOIN PremiumAccount ON PremiumAccount.base=Account.premiumaccount_id JOIN SavingsAccount ON SavingsAccount.base=Account.savingsaccount_id;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  //deinit_vt_selectors();
  //sqlite3_close(db);
  fs.close();
  return SQLITE_DONE;
}
