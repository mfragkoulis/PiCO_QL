#include "Account.h"

using namespace std;

Account::Account(const char *acc_no, float b){
  account_no=acc_no;
  balance=b;
}

float Account::get_balance() {
  return balance;
}

const char * Account::get_account_no() {
  return account_no;
}
