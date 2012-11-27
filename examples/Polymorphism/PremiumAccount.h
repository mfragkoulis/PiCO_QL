#ifndef PREMIUMACCOUNT_H
#define PREMIUMACCOUNT_H

#include <string>
#include "Account.h"

using namespace std;

class PremiumAccount : public Account {

 public:
  PremiumAccount();
  PremiumAccount(string d, double b, double aa, double ba,
		 double o);
  double overlimit;
  double get_balance();
};

#endif
