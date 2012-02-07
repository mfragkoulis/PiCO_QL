#ifndef SPECIALACCOUNT_H
#define SPECIALACCOUNT_H

#include "SuperAccount.h"

class SpecialAccount: public SuperAccount{

 private:
  const char *special_no;
  float special_balance;
  int special_isbn;

 public:
  SpecialAccount(const char *sp_no, float sp_b, int sp_i, int iba);
  SpecialAccount(const char *sp_no, float sp_b);
  float get_special_balance() const;
  const char * get_special_no() const;
  int get_special_isbn() const;
};


#endif
