#include "Account.h"

using namespace std;

Account::Account(const char *acc_no, float b, int i, int iba) : SuperAccount(iba){
    account_no=acc_no;
    balance=b;
    isbn=i;
}

Account::Account(const char *acc_no, float b){
    account_no=acc_no;
    balance=b;
}

float Account::get_balance() const{
    return balance;
}

const char * Account::get_account_no() const{
    return account_no;
}

int Account::get_isbn() const{
    return isbn;
}
