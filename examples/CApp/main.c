#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Money.h"
#include "pico_ql_search.h"

/* .cpp
#include <vector>
using namespace std;
using namespace picoQL;
*/

int main() {
  struct Money* M = (struct Money *)malloc(sizeof(struct Money *));
  //.cpp:  vector<money> mon;
  strcpy((char *)M->name, "Euro");
  M->prc.sub = 9;
  M->price_mode = 2;
  M->prc.main = 35.4;
  M->price_mode = 0;
  strcpy(M->prc.name, "foewr");
  M->price_mode = 1;

  price prc;
  prc.p.main = 12.5;
  prc.price_mode = 0;
  pico_ql_register(&prc, "price");

  M->wgt.g_weight.pound = 9.8;
  M->wgt.g_weight.ounce = 25.8;
  M->wgt.g_weight.grain = 5013.1;
  M->wgt.g_weight.exchange_rate = 1.36;
  M->weight_mode = 3;
  M->wgt.gr = 612;
  M->weight_mode = 2;
  M->wgt.kg = 23.6;
  M->weight_mode = 1;
  strcpy(M->wgt.name, "bnofn");
  M->weight_mode = 0;
  M->next = NULL;
  //.cpp:  mon.push_back(M);
  pico_ql_register(M, "money");
  pico_ql_serve(8080);
  printf("Money Price main: %f\n", M->prc.main);
  //  printf("Money Price sub: %i\n", M->prc.sub);
  return 0;
}
