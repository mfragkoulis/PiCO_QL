#include <string>
#include "Account.h"

using namespace std;

Account::Account(string d, double b, double aa, double ba) {

  description = d;
  balance = b;
  available_amount = aa;
  binded_amount = ba;

}


double Account::get_balance() {
  return balance;
}
