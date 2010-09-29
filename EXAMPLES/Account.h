#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "SuperAccount.h"

class Account: public SuperAccount{

 private:
  const char *account_no;
  float balance;
  int isbn;

 public:
  Account(const char *acc_no, float b, int i, int iba);
  Account(const char *acc_no, float b);
  float get_balance() const;
  const char * get_account_no() const;
  int get_isbn() const;
};


#endif
