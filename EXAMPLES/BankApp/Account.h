#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "SuperAccount.h"

class Account: public SuperAccount{

 public:
  Account(const char *acc_no, float b, int i, const char *);
  Account(const char *acc_no, float b);
  Account & operator=(Account const&);
  double get_rate();
  const unsigned char *type;
};


#endif
