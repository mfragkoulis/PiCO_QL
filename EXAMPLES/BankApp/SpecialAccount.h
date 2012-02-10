#ifndef SPECIALACCOUNT_H
#define SPECIALACCOUNT_H

#include "SuperAccount.h"

class SpecialAccount: public SuperAccount{

 public:
  SpecialAccount(const char *sp_no, float sp_b, int sp_i);
  SpecialAccount(const char *sp_no, float sp_b);
  double get_rate();
  double bonus;
  void calculate_bonus();
};


#endif
