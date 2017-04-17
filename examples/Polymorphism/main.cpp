#include <list>
#include "Account.h"
#include "PremiumAccount.h"
#include "SavingsAccount.h"

#ifndef PICO_QL_SINGLE_THREADED
#include <pthread.h>
#endif
#include "pico_ql.h"

using namespace std;
using namespace picoQL;

int main() {

  list<Account *> accounts;
  PremiumAccount pa121("pa121", 1231.2, 1231.2, 0, 200);
  accounts.push_back(&pa121);
  SavingsAccount sa989("sa989", 786.7, 586.7, 200, "31/12/2013");
  accounts.push_back(&sa989);
  register_data(&accounts, "accounts");

  int re;
#ifndef PICO_QL_SINGLE_THREADED
  pthread_t t;
  void *exit_status = NULL;
  re = init(NULL, 0, 8080, &t);
  pthread_join(t, &exit_status);
#else
  re = init(NULL, 0, 8080);
#endif

  if (re)
    fprintf(stderr, "pico_ql_init() failed with code %d", re);

  /*FILE *f = fopen("polymorphism_resultset", "w");
  pico_ql_exec_query("select * from Account;", f, pico_ql_step_text);
  fclose(f);
  */

  shutdown();

  return 0;
}
