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
using namespace std;

#include "pico_ql.h"

namespace picoQL {

/* Executes test queries. */
int exec_tests() {
  stringstream s;
  fstream fs;
  fs.open("cppapp_test_current.txt", fstream::out);

  int i = 0;
  char q[256];

  strcpy(q, "select nCurrency, rownum, name, price_mode, price, weight_mode, weight from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select rownum, name, price_mode, price, weight_mode, weight from Moneylist;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select nCurrency, rownum, name, price_mode, price, weight_mode, weight from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency where rownum = 1;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select rownum, name, price_mode, price, weight_mode, weight from Moneylist where rownum = 1;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select nCurrency, rownum, name, price_mode, price, weight_mode, weight from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency where rownum = 4;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select rownum, name, price_mode, price, weight_mode, weight from Moneylist where rownum = 4;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select nCurrency, rownum, name, price_mode, price, weight_mode, weight from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency where name like '%eu';");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select rownum, name, price_mode, price, weight_mode, weight from Moneylist where name like '%eu';");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select nCurrency, rownum, name, price_mode, price, weight_mode, weight from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency where price > 10;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select rownum, name, price_mode, price, weight_mode, weight from Moneylist where price > 10;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select nCurrency, rownum, name, price_mode, price, weight_mode, weight from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency where price > 10 and rownum = 0;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select rownum, name, price_mode, price, weight_mode, weight from Moneylist where price > 10 and rownum = 0;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select nCurrency, rownum, name, price_mode, price, weight_mode, weight from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency where price > 10 and rownum = 5;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select rownum, name, price_mode, price, weight_mode, weight from Moneylist where price > 10 and rownum = 5;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select nCurrency, rownum, name, price_mode, price, weight_mode, weight from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency where price like 'f%';");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select rownum, name, price_mode, price, weight_mode, weight from Moneylist where price like 'f%';");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select nCurrency, rownum, name, price_mode, price, weight_mode, weight from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency where price like 'd%';");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select rownum, name, price_mode, price, weight_mode, weight from Moneylist where price like 'd%';");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select nCurrency, rownum, name, price_mode, price, weight_mode, weight from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency join GoldEquivalent on GoldEquivalent.base = Money.weight where ounce > 10;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select rownum, name, price_mode, price, weight_mode, weight from Moneylist join GoldEquivalent on GoldEquivalent.base = Moneylist.weight where ounce > 10;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select nCurrency, rownum, name, price_mode, price, weight_mode, weight from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency where weight like 'a%';");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select rownum, name, price_mode, price, weight_mode, weight from Moneylist where weight like 'a%';");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select nCurrency, rownum, name, price_mode, price, weight_mode, weight from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency where weight like '%o%';");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select rownum, name, price_mode, price, weight_mode, weight from Moneylist where weight like '%o%';");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select * from Price;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  strcpy(q, "select * from Price where price > 10;");
  fs << "Query " << i++ << ":\n " << q << endl << endl;
  exec_query(q, s, step_text);
  fs << s.str();
  s.str("");

  //deinit_vt_selectors();
  //sqlite3_close(db);
  fs.close();
  return SQLITE_OK;
}

} // namespace picoQL
