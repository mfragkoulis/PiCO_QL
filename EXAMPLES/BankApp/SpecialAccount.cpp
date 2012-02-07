#include "SpecialAccount.h"

using namespace std;

SpecialAccount::SpecialAccount(const char *sp_no, float sp_b, int sp_i, int iba) : SuperAccount(iba){
    special_no=sp_no;
    special_balance=sp_b;
    special_isbn=sp_i;
}

SpecialAccount::SpecialAccount(const char *sp_no, float sp_b){
    special_no=sp_no;
    special_balance=sp_b;
}

float SpecialAccount::get_special_balance() const{
    return special_balance;
}

const char * SpecialAccount::get_special_no() const{
    return special_no;
}

int SpecialAccount::get_special_isbn() const{
    return special_isbn;
}

