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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico_ql.h"

/* Executes test queries. */
int exec_tests() {
  FILE *f;
  f = fopen("capp_test_current.txt", "w");

  int i = 0;
  char *q;

  q = "select nCurrency, name, price_mode, price, weight_mode from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select name, price_mode, price, weight_mode from Moneylist;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select name, price_mode, price, weight_mode from MoneyArray;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select nCurrency, name, price_mode, price, weight_mode from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency where name like '%eu';";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select name, price_mode, price, weight_mode from Moneylist where name like '%eu';";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select name, price_mode, price, weight_mode from MoneyArray where name like '%eu';";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select nCurrency, name, price_mode, price, weight_mode from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency where price > 10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select name, price_mode, price, weight_mode from Moneylist where price > 10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select name, price_mode, price, weight_mode from MoneyArray where price > 10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select nCurrency, name, price_mode, price, weight_mode from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency where price like 'f%';";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select name, price_mode, price, weight_mode from Moneylist where price like 'f%';";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select name, price_mode, price, weight_mode from MoneyArray where price like 'f%';";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select nCurrency, name, price_mode, price, weight_mode from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency where price like 'd%';";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select name, price_mode, price, weight_mode from Moneylist where price like 'd%';";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select name, price_mode, price, weight_mode from MoneyArray where price like 'd%';";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select name, price_mode, price, weight_mode from (select DISTINCT name, price_mode, price, weight_mode, weight from Moneylist where price like '%pr%') m join GoldEquivalent on GoldEquivalent.base = m.weight where ounce > 10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select name, price_mode, price, weight_mode from (select DISTINCT name, price_mode, price, weight_mode, weight from MoneyArray where price like '%pr%') m join GoldEquivalent on GoldEquivalent.base = m.weight where ounce > 10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select nCurrency, name, price_mode, price, weight_mode from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency join GoldEquivalent on GoldEquivalent.base = Money.weight where ounce > 10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select name, price_mode, price, weight_mode from Moneylist join GoldEquivalent on GoldEquivalent.base = Moneylist.weight where ounce > 10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select name, price_mode, price, weight_mode from MoneyArray join GoldEquivalent on GoldEquivalent.base = MoneyArray.weight where ounce > 10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select nCurrency, name, price_mode, price, weight_mode from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency where weight like 'a%';";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select name, price_mode, price, weight_mode from Moneylist where weight like 'a%';";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select name, price_mode, price, weight_mode from MoneyArray where weight like 'a%';";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select nCurrency, name, price_mode, price, weight_mode from MonetarySystem JOIN Money ON Money.base = MonetarySystem.currency where weight like '%o%';";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select name, price_mode, price, weight_mode from Moneylist where weight like '%o%';";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select name, price_mode, price, weight_mode from MoneyArray where weight like '%o%';";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select * from Price;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select * from Price where price > 10;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select number from Number;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select number from Number where number > 5;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select number from Number where number > 8;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select number from ENumber;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select number from ENumber where number > 5;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  q = "select number from ENumber where number > 8;";
  fprintf(f, "Query %i:\n %s\n\n", i++, q);
  pico_ql_exec_query(q, f, pico_ql_step_text);

  //deinit_vt_selectors();
  //sqlite3_close(db);
  fclose(f);
  return SQLITE_DONE;
}
