/*
 *   Declare the interface to SpecialAccount class.
 *
 *   Copyright 2012 Marios Fragkoulis
 *
 *   Licensed under the Apache License, Version 2.0
 *   (the "License");you may not use this file except in
 *   compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in
 *   writing, software distributed under the License is
 *   distributed on an "AS IS" BASIS.
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 *   express or implied.
 *   See the License for the specific language governing
 *  permissions and limitations under the License.
 */

#ifndef SPECIALACCOUNT_H
#define SPECIALACCOUNT_H

#include "SuperAccount.h"

class SpecialAccount: public SuperAccount {

 public:
  SpecialAccount(const char *sp_no, float sp_b, int sp_i);
  SpecialAccount(const char *sp_no, float sp_b);
  double get_rate();
  double bonus;
  void calculate_bonus();
};


#endif
