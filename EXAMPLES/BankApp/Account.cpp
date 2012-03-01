#include "Account.h"
#include <string.h>
#include <stdio.h>

using namespace std;

Account::Account(const char *acc_no, float b, int i, const char *t) : SuperAccount(acc_no, b, i) {
    type = reinterpret_cast<const unsigned char *>(t);
}

Account::Account(const char *acc_no, float b) : SuperAccount(acc_no, b) {}

Account & Account::operator=(Account const&) {}

double Account::get_rate() {
    return balance/50;
}
