#include "SpecialAccount.h"

using namespace std;

SpecialAccount::SpecialAccount(const char *sp_no, float sp_b, int sp_i) : SuperAccount(sp_no, sp_b, sp_i){}

SpecialAccount::SpecialAccount(const char *sp_no, float sp_b) : SuperAccount(sp_no, sp_b) {}

double SpecialAccount::get_rate() {
    return balance/20;
}

void SpecialAccount::calculate_bonus() {
    bonus = get_rate() * 7;
}
