#include "SuperAccount.h"

using namespace std;

SuperAccount::SuperAccount(const char *acc_no, float b, int i) {
    account_no = acc_no;
    balance = b;
    isbn = i;
}

SuperAccount::SuperAccount(const char *acc_no, float b) {
    account_no = acc_no;
    balance = b;
}

float SuperAccount::get_balance() const {
    return balance;
}

const char * SuperAccount::get_account_no() const {
    return account_no;
}

int SuperAccount::get_isbn() const {
    return isbn;
}

double SuperAccount::get_rate() {
    return balance / 100;
}
