#include <list>
#include "Account.h"
#include "PremiumAccount.h"
#include "SavingsAccount.h"

#ifndef PICO_QL_SINGLE_THREADED
#include <pthread.h>
#endif
#include "pico_ql_search.h"

using namespace std;
using namespace picoQL;

int main() {

  list<Account *> accounts;
  PremiumAccount pa121("pa121", 1231.2, 1231.2, 0, 200);
  accounts.push_back(&pa121);
  SavingsAccount sa989("sa989", 786.7, 586.7, 200, "31/12/2013");
  accounts.push_back(&sa989);
  pico_ql_register(&accounts, "accounts");

#ifndef PICO_QL_SINGLE_THREADED
  pthread_t t;
  void *exit_status = NULL;
  pico_ql_serve(8080, &t);
  pthread_join(t, &exit_status);
#else
  pico_ql_serve(8080, NULL);
#endif
  return 0;
}
