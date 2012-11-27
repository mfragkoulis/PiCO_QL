#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

using namespace std;

class Account {

 protected:
  double balance;

 public:
  Account();
  Account(string d, double b, double aa, double ba);
  string description;
  double available_amount;
  double binded_amount;
  virtual double get_balance();

};

#endif
