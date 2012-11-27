#ifndef SAVINGSACCOUNT_H
#define SAVINGSACCOUNT_H

#include <string>
#include "Account.h"

class SavingsAccount : public Account {

 public:
  SavingsAccount();
  SavingsAccount(string d, double b, double aa, double ba,
		 string dd);
  string due_date;
  double get_balance();

};

#endif
