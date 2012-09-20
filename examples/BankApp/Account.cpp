/*
 *   Implement the member methods of Account class.
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

#include <cstring>
#include <cstdio>
#include "Account.h"

using namespace std;

Account::Account(const char *acc_no, float b, int i, 
		 const char *t) : 
  SuperAccount(acc_no, b, i) {
    type = reinterpret_cast<const unsigned char *>(t);
}

Account::Account(const char *acc_no, float b) : 
  SuperAccount(acc_no, b) {}


double Account::get_rate() {
    return balance/50;
}
