#include "PremiumAccount.h"
#include "Account.h"

PremiumAccount::PremiumAccount(string d, double b, double aa, double ba, double o) : Account(d, b, aa, ba) {

  overlimit = o;

}

double PremiumAccount::get_balance() {
  return balance*1.05;
}
