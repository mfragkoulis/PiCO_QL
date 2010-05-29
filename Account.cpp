#include "Account.h"

using namespace std;

Account::Account(char *acc_no, float b){
  account_no=acc_no;
  balance=b;
}
float Account::get_balance() {
  return balance;
}
