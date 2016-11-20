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

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
using namespace std;

#include "pico_ql.h"
using namespace picoQL;

int exec_tests() {
  stringstream s;
  fstream fs;
  fs.open("bankapp_test_current.txt", fstream::out);
  int i = 1;
  char q[200];

  strcpy(q, "select * from SuperAccounts;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select * from SuperAccounts where SuperAccountisbn > 0 and SuperAccountbalance < 500 and SuperAccountaccount_no>1000 order by id;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select * from Accounts;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select * from AccountsNULL;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select * from Accounts where rate >10 and balance<20000 order by account_no;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select * from AccountsNULL where rate >10 and balance<20000 order by account_no;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select * from SpecialAccounts;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select * from SpecialAccounts where bonus > 20 and rate >15 order by account_no;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select * from Myaccounts;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  pico_ql_exec_query(q, s, pico_ql_step_text);
  fs << s.str();
  s.str("");

  //deinit_vt_selectors();
  //sqlite3_close(db);
  fs.close();
  return SQLITE_DONE;
}
